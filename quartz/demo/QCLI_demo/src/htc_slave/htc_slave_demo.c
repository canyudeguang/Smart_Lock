/*==================================================================================
       Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
                       All Rights Reserved.
       Confidential and Proprietary - Qualcomm Technologies, Inc.
==================================================================================*/
 /*********************************************************************
 *
 * @file  htc_slave_demo.c
 * @brief Test application for HTC Slave
*/

/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     -----------------------------------------------------------------
21/09/17   mmtd    Initial version
==================================================================================*/

/*
 * htc slave ping is a test/demo application, mainly for use during bringup.
 * This application runs on the Target and sits on top of the Host Target
 * Communications (HTC) module.  It waits for messages from the Host to
 * be received, and it immediately echoes each message back to the Host.  
 *
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "com_dtypes.h"
#include "qcli_api.h"
#include "qapi_status.h"
#include "qapi_slp.h"
#include "qapi_htc_slave.h"
#include "htc_slave_demo.h"
#include "malloc.h"

#define NUM_PING_BUFFERS_PER_ENDPOINT  QAPI_NUM_BUFFERS_PER_ENDPOINT
#define PING_BUFSZ 256

extern QCLI_Group_Handle_t qcli_peripherals_group;              /* Handle for our peripherals subgroup. */
QCLI_Group_Handle_t qcli_htc_slave_hdl;


/* Prototypes */
QCLI_Command_Status_t htc_slave_demo_start(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters);
QCLI_Command_Status_t htc_slave_demo_stop(uint32_t parameters_count,
                                    QCLI_Parameter_t * parameters);
QCLI_Command_Status_t htc_slave_demo_pause(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters);
QCLI_Command_Status_t htc_slave_demo_resume(uint32_t parameters_count,
                                      QCLI_Parameter_t * parameters);
QCLI_Command_Status_t htc_slave_demo_inc_txcredit(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters);
QCLI_Command_Status_t htc_slave_demo_recycle_htcbuf(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters);
QCLI_Command_Status_t htc_slave_demo_dump_recv_queued_buffers(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters);
QCLI_Command_Status_t htc_slave_demo_dump_send_queued_buffers(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters);
QCLI_Command_Status_t htc_slave_demo_dump_recycle_queue_htcbuf(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters);


typedef struct {
    uint8 buffer[NUM_PING_BUFFERS_PER_ENDPOINT][PING_BUFSZ];
} ping_buffers_t;

typedef struct {
    qapi_HTC_bufinfo_t HTC_bufinfo[NUM_PING_BUFFERS_PER_ENDPOINT];
} ping_buf_info_t;

static  qapi_HTC_bufinfo_t    *HTC_buf_free_pool = NULL;
static ping_buffers_t *pbuffers[QAPI_HTC_SLAVE_MAX_ENDPOINTS] = {0};
static ping_buf_info_t *pbufinfo[QAPI_HTC_SLAVE_MAX_ENDPOINTS] = {0};
#define PING_BUFFER(endpoint, which) pbuffers[endpoint]->buffer[which]
#define PING_BUF_INFO(endpoint, which) pbufinfo[endpoint]->HTC_bufinfo[which]

static uint32 transId = QAPI_HTC_SDIO_SLAVE_INSTANCE_ID;


const QCLI_Command_t htc_slave_cmd_list[] =
{
   /* cmd_function      thread  cmd_string  usage_string    description */
    {htc_slave_demo_start,    false,  "Start",    "\n",           "Start Loopback test\n"},
    {htc_slave_demo_stop,     false,  "Stop",     "\n",           "Stop HTC_Slave application\n"},
    {htc_slave_demo_pause,    false,  "Pause",    "\n",           "Pause MBox\n"},
    {htc_slave_demo_resume,   false,  "Resume",   "\n",           "Resume MBox\n"},
    {htc_slave_demo_inc_txcredit,   false,  "TxCreditInc",   "\n",           "Increment Host TX credit\n"},
    {htc_slave_demo_recycle_htcbuf,   false,  "Recycle htc bufs",   "\n",    "Recycle free HTC buffers\n"},
    {htc_slave_demo_dump_recv_queued_buffers,   false,  "dump htc_bufs  rx queue",   "\n",    "dump htc_bufs  rx queue\n"},
    {htc_slave_demo_dump_send_queued_buffers,   false,  "dump htc_bufs tx queue",   "\n",    "dump htc_bufs  rx queue\n"},    
    {htc_slave_demo_dump_recycle_queue_htcbuf,   false,  "dump recycle htc_bufs queue",   "\n",    "dump recycle htc_bufs queue\n"},
};

const QCLI_Command_Group_t htc_slave_cmd_group =
{
    "HTCSlave",   /* Group_String: will display cmd prompt as "HTC_Slave> " */
    sizeof(htc_slave_cmd_list)/sizeof(htc_slave_cmd_list[0]), /* Command_Count */
    htc_slave_cmd_list        /* Command_List */
};

/*****************************************************************************
 * This function is used to register the Fs Command Group with QCLI.
 *****************************************************************************/
void Initialize_HTCSlave_Demo(void)
{
    /* Attempt to reqister the Command Groups with the qcli framework.*/
    qcli_htc_slave_hdl = QCLI_Register_Command_Group(qcli_peripherals_group, &htc_slave_cmd_group);
    if (qcli_htc_slave_hdl)
    {
        QCLI_Printf(qcli_htc_slave_hdl, "MboxPing Registered\n");
    }

    return;
}

void debug_buffer_print(qapi_HTC_bufinfo_t *buf, const char *str, int endpoint)
{
    int num = 0;
    QCLI_Printf(qcli_htc_slave_hdl,"BUFS-%d-%s: ", endpoint, str);
    if (buf == NULL) {
        QCLI_Printf(qcli_htc_slave_hdl, "none");
    }

    while (buf) {
        QCLI_Printf(qcli_htc_slave_hdl, "[%d:0x%x:0x%x/0x%x] ",
                 num,
                 buf,
                 buf->buffer,
                 buf->actual_length);
        buf = buf->next;
        num++;
    }

    QCLI_Printf(qcli_htc_slave_hdl, "\n");
}

void htc_slave_enqueue_htcbuf(qapi_HTC_bufinfo_t *htc_buf)
{
    qapi_HTC_bufinfo_t *queue_tail = NULL;

    if (HTC_buf_free_pool == NULL) {
        HTC_buf_free_pool = htc_buf;
    } else {
        /* Traverse till end of the queue */
        queue_tail = HTC_buf_free_pool;
        while(queue_tail->next != NULL)
        {
            queue_tail = queue_tail->next;
        }
        queue_tail->next = htc_buf;
    }
}
qapi_HTC_bufinfo_t* htc_slave_dequeue_htcbuf(int num_bufs, int *pout_num_bufs)
{
    int cnt = 0;
    qapi_HTC_bufinfo_t *htc_buf = HTC_buf_free_pool;
    qapi_HTC_bufinfo_t *last_htc_buf = HTC_buf_free_pool;
    if ((htc_buf == NULL) || (num_bufs == 0)) {
        *pout_num_bufs = cnt;
        return NULL;
    } else {
        while ((HTC_buf_free_pool) && (cnt < num_bufs))
        {
            last_htc_buf = HTC_buf_free_pool;
            HTC_buf_free_pool = HTC_buf_free_pool->next;
            cnt++;
        }
        last_htc_buf->next = NULL;
        *pout_num_bufs = cnt;
        return htc_buf;
    }
}


static void htc_slave_initdone(uint32 Instance, uint32 arg1, uint32 arg2)
{
    int bufnum;
    uint32 msgsize = 0;
    qapi_HTCSlaveEndpointID_t endpoint = (qapi_HTCSlaveEndpointID_t) arg1;

    QCLI_Printf(qcli_htc_slave_hdl,
                "Host/Target Communications are initialized for endpoint %d+ \n", endpoint);

    /* override MBOX 0 max message size (defaults to 256) to be the same as the other
    * 3 mailboxes. This allows the host side mailbox 
    * application to perform 1500+ byte pings to verify mailbox 0 operation
    */
    qapi_HTC_Slave_Max_MsgSize_Get(transId, endpoint, &msgsize);
    
    qapi_HTC_Slave_Max_MsgSize_Set(transId, endpoint, msgsize);


    /* TODO: buffer size should be same as negotiated block size, as Host sends
       all packets rounded off to this size, and DMA is also programmed to
       interrupt on receipt of this size, Short packet handling is not present in HTC,
       all packets are rounded off by padding and then sent */
    
    /* Write buffer size */
    qapi_HTC_Slave_BufSize_Set(transId, endpoint, PING_BUFSZ);
 
    /* Supply buffers to HTC. */
    for (bufnum=0; bufnum < NUM_PING_BUFFERS_PER_ENDPOINT; bufnum++) {
        qapi_HTC_bufinfo_t *bufinfo;
        
        bufinfo = &PING_BUF_INFO(endpoint, bufnum);
        bufinfo->buffer = PING_BUFFER(endpoint, bufnum);
        bufinfo->next = NULL;
        qapi_HTC_Slave_Receive_Data(transId, endpoint, bufinfo);
    }
   
    QCLI_Printf(qcli_htc_slave_hdl,
                "Host/Target Communications are initialized for endpoint %d -\n", endpoint);    

    QCLI_Printf(qcli_htc_slave_hdl, "-\n");


}

static void htc_slave_senddone(uint32 Instance, uint32 arg1, uint32 arg2)
{
    qapi_HTCSlaveEndpointID_t endpoint = (qapi_HTCSlaveEndpointID_t) arg1;
    qapi_HTC_bufinfo_t *bufinfo = (qapi_HTC_bufinfo_t *) arg2;    

    QCLI_Printf(qcli_htc_slave_hdl,
        ">>>htc_slave_senddone htcbuffer:0x%x buffer:0x%x length %d endpoint:%d\n",
         bufinfo, bufinfo->buffer, bufinfo->actual_length, endpoint);
    htc_slave_enqueue_htcbuf(bufinfo);
	
#if 0
    /* Recycle buffers */
    qapi_HTC_Slave_Receive_Data(transId, endpoint, bufinfo);
#endif

}

static void htc_slave_recvdone(uint32 Instance, uint32 arg1, uint32 arg2)
{
    uint32 i=0;
    qapi_HTCSlaveEndpointID_t endpoint = (qapi_HTCSlaveEndpointID_t) arg1;
    qapi_HTC_bufinfo_t *bufinfo = (qapi_HTC_bufinfo_t *) arg2;    

    QCLI_Printf(qcli_htc_slave_hdl,
        ">>>htc_slave_recvdone (htcbuffer:0x%x) buffer=0x%x length %d, endpoint=%d\n",
        bufinfo, bufinfo->buffer,bufinfo->actual_length, endpoint);
    
    for (i=0; i<bufinfo->actual_length; i++)
    {
        QCLI_Printf(qcli_htc_slave_hdl, "%x ",bufinfo->buffer[i]);
    }
    QCLI_Printf(qcli_htc_slave_hdl, "\n");
    
   /* Reflect this message */
    qapi_HTC_Slave_Send_Data(transId, endpoint, bufinfo);
}

static void htc_slave_resetdone(uint32 Instance, uint32 arg1, uint32 arg2)
{
    uint8 endpoint_used = 0;
    uint8 endpoint;
    uint32 msgsize=0; 
    qapi_Status_t status;
    
    status = qapi_HTC_Slave_Get_Num_Endpoints(transId, &endpoint_used);
    
    status = qapi_HTC_Slave_Stop(transId);
    
    qapi_HTC_Slave_Shutdown (transId);

    /* Clear all the buffers and buf_info structures for each endpoint */
    for (endpoint = 0; endpoint < endpoint_used; endpoint++)
    {
        memset(pbuffers[endpoint], 0, sizeof(ping_buffers_t));
        memset(pbufinfo[endpoint], 0, sizeof(ping_buf_info_t));          
    }
    
    status = qapi_HTC_Slave_Init(transId);

    status = qapi_HTC_Event_Register (transId, 
                                      QAPI_HTC_SLAVE_INIT_COMPLETE,
                                      htc_slave_initdone,
                                      NULL);

    status = qapi_HTC_Event_Register (transId, 
                                      QAPI_HTC_SLAVE_BUFFER_RECEIVED,
                                      htc_slave_recvdone,
                                      NULL);

    status = qapi_HTC_Event_Register (transId, 
                                      QAPI_HTC_SLAVE_BUFFER_SENT,
                                      htc_slave_senddone,
                                      NULL);

    status = qapi_HTC_Event_Register (transId, 
                                      QAPI_HTC_SLAVE_RESET_COMPLETE,
                                      htc_slave_resetdone,
                                      NULL);


    status = qapi_HTC_Slave_Start(transId);

    for (endpoint=0; endpoint < endpoint_used; endpoint++) {
        QCLI_Printf(qcli_htc_slave_hdl,"Restart Mailbox Ping test on endpoint %d\n", endpoint);
        QCLI_Printf(qcli_htc_slave_hdl,"Buffers Per MBOX: %d\n", NUM_PING_BUFFERS_PER_ENDPOINT);
        QCLI_Printf(qcli_htc_slave_hdl,"Buffer Size: %d\n", PING_BUFSZ); 
        status = qapi_HTC_Slave_Max_MsgSize_Get(transId, (qapi_HTCSlaveEndpointID_t)endpoint, &msgsize);
        QCLI_Printf(qcli_htc_slave_hdl,"Max Message Size: %d\n", msgsize);        
        QCLI_Printf(qcli_htc_slave_hdl,"Max Message Size: %d\n", msgsize);
        QCLI_Printf(qcli_htc_slave_hdl,"Credits Per MBOX: %d\n", 
            (NUM_PING_BUFFERS_PER_ENDPOINT * PING_BUFSZ) / msgsize);
    }
    if(status){}
    
}

QCLI_Command_Status_t htc_slave_demo_start(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters)
{
    int endpoint;
    uint32 msgsize=0;
    uint8 endpoint_used;
    qapi_Status_t status;
    
    transId = (uint8)parameters[0].Integer_Value;

    if (transId >= QAPI_HTC_MAX_INSTANCES)
    {
        QCLI_Printf(qcli_htc_slave_hdl,"Invalid transport id\n");
        QCLI_Printf(qcli_htc_slave_hdl,"Enter SDIO: 0, SPI: 1\n");
        return QCLI_STATUS_ERROR_E;
    }
        

    /*qapi_Slp_Set_Max_Latency(QAPI_SLP_LAT_PERF); */

    status = qapi_HTC_Slave_Init(transId);

    /* Get number of endpoint supported */
    qapi_HTC_Slave_Get_Num_Endpoints(transId, &endpoint_used);

    /* Allocate buffers and buf_info structures for each endpoint */
    for ( endpoint = 0; endpoint < endpoint_used; endpoint++)
    {
        pbuffers[endpoint] = malloc (sizeof(ping_buffers_t));
        if (pbuffers[endpoint] == NULL)
        {
            QCLI_Printf(qcli_htc_slave_hdl,"Malloc failure at line %d for endpoint %d\n", __LINE__, endpoint);
            return QCLI_STATUS_ERROR_E;
        }
        memset(pbuffers[endpoint], 0, sizeof(ping_buffers_t));
        
        pbufinfo[endpoint] = malloc (sizeof(ping_buf_info_t));
        if (pbufinfo[endpoint] == NULL)
        {
            QCLI_Printf(qcli_htc_slave_hdl,"Malloc failure at line %d for endpoint %d\n", __LINE__, endpoint);
            return QCLI_STATUS_ERROR_E;
        }
        memset(pbufinfo[endpoint], 0, sizeof(ping_buf_info_t));          
    }


    status = qapi_HTC_Event_Register (transId, 
                                      QAPI_HTC_SLAVE_INIT_COMPLETE,
                                      htc_slave_initdone,
                                      NULL);

    status = qapi_HTC_Event_Register (transId, 
                                      QAPI_HTC_SLAVE_BUFFER_RECEIVED,
                                      htc_slave_recvdone,
                                      NULL);

    status = qapi_HTC_Event_Register (transId, 
                                      QAPI_HTC_SLAVE_BUFFER_SENT,
                                      htc_slave_senddone,
                                      NULL);

    status = qapi_HTC_Event_Register (transId, 
                                      QAPI_HTC_SLAVE_RESET_COMPLETE,
                                      htc_slave_resetdone,
                                      NULL);


    qapi_HTC_Slave_Start(transId);

    for (endpoint=0; endpoint < endpoint_used; endpoint++) {
        QCLI_Printf(qcli_htc_slave_hdl,"Start Mailbox Ping test on endpoint %d\n", endpoint);
        QCLI_Printf(qcli_htc_slave_hdl,"Buffers Per MBOX: %d\n", NUM_PING_BUFFERS_PER_ENDPOINT);
        QCLI_Printf(qcli_htc_slave_hdl,"Buffer Size: %d\n", PING_BUFSZ); 
        qapi_HTC_Slave_Max_MsgSize_Get(transId, (qapi_HTCSlaveEndpointID_t)endpoint, &msgsize);
        QCLI_Printf(qcli_htc_slave_hdl,"Max Message Size: %d\n", msgsize);        
        QCLI_Printf(qcli_htc_slave_hdl,"Max Message Size: %d\n", msgsize);
        QCLI_Printf(qcli_htc_slave_hdl,"Credits Per MBOX: %d\n", 
            (NUM_PING_BUFFERS_PER_ENDPOINT * PING_BUFSZ) / msgsize);
    }
    if(status){}    
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t htc_slave_demo_stop(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters)
{
    uint8 endpoint_used = 0;
    uint8 endpoint = 0;
    
    qapi_HTC_Slave_Get_Num_Endpoints(transId, &endpoint_used);
    
    qapi_HTC_Slave_Stop(transId);

    qapi_HTC_Event_Deregister (transId, 
                                      QAPI_HTC_SLAVE_INIT_COMPLETE);

    qapi_HTC_Event_Deregister (transId, 
                                      QAPI_HTC_SLAVE_BUFFER_RECEIVED);


    qapi_HTC_Event_Deregister (transId, 
                                      QAPI_HTC_SLAVE_BUFFER_SENT);


    qapi_HTC_Event_Deregister (transId, 
                                      QAPI_HTC_SLAVE_RESET_COMPLETE);

    
    qapi_HTC_Slave_Shutdown (transId);
    
    /* Free all the buffers allocated */
    for ( endpoint = 0; endpoint < endpoint_used; endpoint++)
    {
        if (pbuffers[endpoint] != NULL)
        {
            free(pbuffers[endpoint]);
            pbuffers[endpoint] = NULL;
        }
        
        if (pbufinfo[endpoint] != NULL)
        {
            free(pbufinfo[endpoint] );
            pbufinfo[endpoint] = NULL;
        }
    }
    /* qapi_Slp_Set_Max_Latency(QAPI_SLP_LAT_POWER); */
    
    /* Freed successfully */
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t htc_slave_demo_pause(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters)
{
    uint8 endpoint = (uint8)parameters[0].Integer_Value;
    qapi_HTC_Slave_Recv_Pause(transId, (qapi_HTCSlaveEndpointID_t)endpoint);
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t htc_slave_demo_resume(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters)
{
    uint8 endpoint = (uint8)parameters[0].Integer_Value;
    qapi_HTC_Slave_Recv_Resume(transId, (qapi_HTCSlaveEndpointID_t)endpoint);
    return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t htc_slave_demo_inc_txcredit(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters)
{
    uint8 endpoint = (uint8)parameters[0].Integer_Value;
   /* Allow Host to send sizing information. */
    qapi_HTC_slave_tx_credit_inc(transId, endpoint);

    return QCLI_STATUS_SUCCESS_E;
}
QCLI_Command_Status_t htc_slave_demo_recycle_htcbuf(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters)
{
    qapi_HTC_bufinfo_t    *htc_buf;
    int num_out_bufs = 0;
    int endpoint = (int)parameters[0].Integer_Value;
    int num_bufs = (int)parameters[1].Integer_Value;

    /* Dequeue from recycle queue */
    htc_buf = htc_slave_dequeue_htcbuf(num_bufs, &num_out_bufs);
    if ( htc_buf != NULL) {
        /* Recycle buffers */
        qapi_HTC_Slave_Receive_Data(transId, (qapi_HTCSlaveEndpointID_t)endpoint, htc_buf);
    }

    return QCLI_STATUS_SUCCESS_E;
}
QCLI_Command_Status_t htc_slave_demo_dump_recv_queued_buffers(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters)
{
    int endpoint = (int)parameters[0].Integer_Value;
    
    qapi_HTC_slave_dump_recv_queued_buffers(transId, endpoint);    
    return QCLI_STATUS_SUCCESS_E;
}
QCLI_Command_Status_t htc_slave_demo_dump_send_queued_buffers(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters)
{
    int endpoint = (int)parameters[0].Integer_Value;
    
    qapi_HTC_slave_dump_send_queued_buffers(transId, endpoint);
    return QCLI_STATUS_SUCCESS_E;
}
QCLI_Command_Status_t htc_slave_demo_dump_recycle_queue_htcbuf(uint32_t parameters_count,
                                     QCLI_Parameter_t * parameters)
{
    int endpoint = (int)parameters[0].Integer_Value;
    
    if(HTC_buf_free_pool != NULL)
        debug_buffer_print(HTC_buf_free_pool, "recycle bufs pool", endpoint);
    else
        QCLI_Printf(qcli_htc_slave_hdl, "recycle queue empty\n");
    return QCLI_STATUS_SUCCESS_E;
}
