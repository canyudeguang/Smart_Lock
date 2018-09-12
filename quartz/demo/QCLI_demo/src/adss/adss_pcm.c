/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * 2015-2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
 
#include <stdio.h>

#include "qurt_signal.h"

#include "qapi/qurt_thread.h"
#include "stdint.h"
#include "qapi/qapi_status.h"
#include "qurt_mutex.h"
#include "qapi_i2s.h"
#include "qapi_pcm.h"
#include <qcli.h>
#include <qcli_api.h>
#include "qapi/qapi_socket.h"
#include "qapi/qapi_netservices.h"
#include <qapi/qapi_i2c_master.h>

#include "malloc.h"

#include "adss_demo.h"
#include "adss_mem.h"
#include "netutils.h"

#include "adss_pcm.h"

ADSS_PCM_SESSION_t  *m_pPCMSession;


uint32_t    pcm_slot_bitmap = 0;
uint32_t    max_slots = 0;
uint8_t		fixed_slots[MAX_SLOTS];

void adss_pcm_dma_rcv_cb(void *hd, uint32_t status, void *param)
{
    uint32_t       sent_len;
	volatile uint8_t *pRcvBuf;
	
    loop_count++;

	pRcvBuf = (uint8_t *)param;

	if (pRcvBuf != 0)
	{
		qapi_PCM_Send_Data(hd_pcm_out, (uint8_t *)pRcvBuf, 0, &sent_len);		
	}

	qurt_signal_set(&pcm_rcv_done_signal, ADSS_USR_SEND_DONE_SIG_MASK);
}

void adss_pcm_dma_snd_cb(void *hd, uint32_t status, void *param)
{
    send_count++;	
}

void init_pcm_frame_data (uint32_t *pArray32, uint32_t count, uint16_t val)
{
	uint32_t   i, slot_id;
	uint32_t   ndx = 0;

	for (i=0; i < count; i++)
	{
		slot_id = fixed_slots[ndx++];
		if (ndx >= max_slots)
			ndx = 0;
		
		*pArray32++ = (slot_id << 16) | val; 	  			
	}	
}

void adss_pcm_send_receive(uint32_t pkt_count, uint32_t buf_size, uint32_t sample_freq)
{	
	qapi_Status_t status;
	uint8_t       **pbuf_v, **rpbuf_v;
	uint8_t       *pbuf;
	char          *freq_str;
	uint32_t      i, rcv_len;
	uint32_t      freq;
	
	pbuf_v = (uint8_t **)malloc(sizeof(uint8_t *) * pkt_count);
	if (pbuf_v == NULL)
	   return;
  
	rpbuf_v = (uint8_t **)malloc(sizeof(uint8_t *) * pkt_count);
	if (rpbuf_v == NULL)
	{
	   free(pbuf_v);
	   return;
	}

	switch(sample_freq)
	{
	case   0:
	    freq = QAPI_I2S_FREQ_8_KHZ_E;
		freq_str = "8KHz";
		break;
	case   1:
	    freq = QAPI_I2S_FREQ_16_KHZ_E;
		freq_str = "16KHz";
		break;
	default:
	    freq = QAPI_I2S_FREQ_32_KHZ_E;
		freq_str = "32KHz";
		break;
	}
	ADSS_DEBUG_PRINTF("PCM Receive & Send\n");
	ADSS_DEBUG_PRINTF("PCM Sample Rate :%s\n", freq_str);
	ADSS_DEBUG_PRINTF("DMA Descriptors :%d\n", pkt_count);
	ADSS_DEBUG_PRINTF("DMA Block Size :%d\n", buf_size);

    pcm_config.freq = freq;
	pcm_config.slots = pcm_slot_bitmap;       			            
	pcm_config.num_Tx_Desc = pkt_count;
	pcm_config.pcm_Buf_Size = buf_size;
	pcm_config.frame_Sync_Len = QAPI_PCM_FRAME_SYNC_ONE_CLK_E;
	
	status = qapi_PCM_Init(&pcm_config, &hd_pcm_out);
	if (status != QAPI_OK)
	{
		QCLI_Printf(qcli_adss_group, "pcm init fail=%d\n", status);		  
		return; 
	}
	status = qapi_PCM_Open(hd_pcm_out);
    status = qapi_PCM_Intr_Register(hd_pcm_out, adss_pcm_dma_snd_cb, 0);

    pcm_in_config.freq = freq;
	pcm_in_config.slots = pcm_slot_bitmap;     			            
	pcm_in_config.num_Tx_Desc = pkt_count;
	pcm_in_config.pcm_Buf_Size = buf_size;
	pcm_in_config.frame_Sync_Len = QAPI_PCM_FRAME_SYNC_ONE_SLOT_E;

	status = qapi_PCM_Init(&pcm_in_config, &hd_pcm_in);

	if (status != QAPI_OK)
	{
		QCLI_Printf(qcli_adss_group, "pcm init fail=%d\n", status);		  
		return; 
	}	
	status = qapi_PCM_Open(hd_pcm_in);
	
    status = qapi_PCM_Intr_Register(hd_pcm_in, adss_pcm_dma_rcv_cb, 0);

    qurt_signal_init(&pcm_rcv_done_signal);

	for (i=0; i < pkt_count - 1; i++)
	{
		status = qapi_I2S_Get_Buffer(&rpbuf_v[i]);
		init_pcm_frame_data ((uint32_t *)rpbuf_v[i], buf_size/4, 0xAA55);		
	}
  
	for (i=0; i < pkt_count; i++)
	{
		status = qapi_I2S_Get_Buffer(&pbuf_v[i]);
		init_pcm_frame_data ((uint32_t *)pbuf_v[i], buf_size/4, 0xBB66);
	}

	status = qapi_PCM_Send_Receive(hd_pcm_out, pbuf_v, pkt_count, hd_pcm_in, rpbuf_v, pkt_count-1);
	if (status != QAPI_OK)
	{
		return; 
	}	

	audio_echo_loop_flag = 1;
    do
    {
		status = qapi_I2S_Get_Buffer(&pbuf);
		if (status == QAPI_OK)
		{
			qapi_PCM_Receive_Data(hd_pcm_in, pbuf, 0, &rcv_len);
		}
		qurt_signal_wait(&pcm_rcv_done_signal, ADSS_USR_SEND_DONE_SIG_MASK, QURT_SIGNAL_ATTR_CLEAR_MASK);
	} while (audio_echo_loop_flag);
	
	loop_count = 0;
    send_count = 0;	
	
    qurt_signal_delete(&pcm_rcv_done_signal);

    qapi_I2S_Deinit (hd_pcm_out);
    qapi_I2S_Deinit (hd_pcm_in);
  
    free(pbuf_v);
    free(rpbuf_v);	
}
