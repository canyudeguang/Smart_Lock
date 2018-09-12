/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
 /******************************************************************************
 * spi_hal.c
 *
 * SPI Master side transport layer implementation
 *
 * Copyright (c) 2017
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 ******************************************************************************/
/*=============================================================================

                              EDIT HISTORY 
  $Header: 

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2017-12-14   mmtd      Initial creation
======================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <qcli.h>
#include <qcli_api.h>
#include "qurt_error.h"
#include "qurt_signal.h"
#include "qurt_thread.h"
#include "qurt_mutex.h"
#include "qapi_status.h"
#include "qapi_delay.h"
#include "qapi_dsr.h"
#include "qapi_fatal_err.h"
#include "qapi_spi_master.h"
#include "spi_hal.h"
#include "osal_types.h"
#include "htc_defs.h"
#include "transport.h"
#include "htc.h"
#include "dl_list.h"
#include "htc_internal.h"
#include "hif.h"
#include "hif_internal.h"
#include "htc_host_reg.h"
#include "spi_hal_api.h"
#include "qurt_timer.h"
#include "qurt_types.h"


/* Defines */
#define THREAD_STACK_SIZE  1024

/* Globals */
QCLI_Group_Handle_t qcli_spim_group;               /* Handle for our QCLI Command Group. */
qurt_thread_attr_t spi_thread_attr;

#define ASSERT( __bool ) \
        do { \
            if (0 == (__bool)) { \
                __asm volatile("bkpt 0"); \
            } \
        } while (0)


/* Prototypes */
static htc_status_t spi_hal_read_data (void *tdev, io_mode_t iomode, unsigned int addr, void *src, int count);
static htc_status_t spi_hal_write_data (void *tdev, io_mode_t iomode, unsigned int addr, void *src, int count);
static htc_status_t spi_hal_register_interrupt (void *tdev, hif_isr_handler_t pfunc);
static htc_status_t spi_hal_deregister_interrupt (void *tdev);
static htc_status_t spi_hal_enable_trans (void *tdev);
static htc_status_t spi_hal_disable_trans (void *tdev);
static void spi_hal_gpio_isr(qapi_GPIOINT_Callback_Data_t data);


static void spi_io_complete_cb (uint32_t i_status, void *i_ctxt)
{
    spi_device_t *dev = (spi_device_t *)i_ctxt;

    qurt_signal_set(&dev->spi_Tx_done, dev->sig_mask);
}

static htc_status_t spi_get_deviceid_func(uint32 u32InputParam, qapi_SPIM_Instance_t *Dev_id)
{
    switch (u32InputParam)
    {
        case 1:
        {
            *Dev_id = QAPI_SPIM_INSTANCE_1_E;
            return HTC_OK;
        }
        case 2:
        {
            *Dev_id = QAPI_SPIM_INSTANCE_2_E;
            return HTC_OK;
        }
        case 3:
        {
            *Dev_id = QAPI_SPIM_INSTANCE_3_E;
            return HTC_OK;
        }
        case 4:
        {
            *Dev_id = QAPI_SPIM_INSTANCE_4_E;
            return HTC_OK;
        }
        default:
        {
            return HTC_EINVAL;
        }
    }
}

static htc_status_t spi_config_func(spi_device_t *dev, qbool_t loopback_mode, spi_trans_bit_width_t width)
{

  dev->spi_trans_config.SPIM_Clk_Polarity =  QAPI_SPIM_CLK_IDLE_HIGH_E;
  dev->spi_trans_config.SPIM_Shift_Mode = QAPI_SPIM_INPUT_FIRST_MODE_E;
  dev->spi_trans_config.SPIM_CS_Polarity = QAPI_SPIM_CS_ACTIVE_LOW_E;
  dev->spi_trans_config.SPIM_CS_Mode = QAPI_SPIM_CS_DEASSERT_E;
  dev->spi_trans_config.SPIM_Clk_Always_On =  QAPI_SPIM_CLK_NORMAL_E;
  dev->spi_trans_config.SPIM_Bits_Per_Word = (width * 8);
  dev->spi_trans_config.SPIM_Slave_Index = 0;
  dev->spi_trans_config.min_Slave_Freq_Hz = 0;
  dev->spi_trans_config.max_Slave_Freq_Hz = SPI_SLV_CLOCK_FREQUENCY;
  dev->spi_trans_config.deassertion_Time_Ns = 0;
  dev->spi_trans_config.loopback_Mode = loopback_mode;
  dev->spi_trans_config.hs_Mode = 1;


  return HTC_OK;
}


htc_status_t spi_hal_reg_read (spi_device_t *dev, uint16 regaddr, uint32 *pRegData, spi_trans_bit_width_t data_width)
{
    uint32 Test_Result = HTC_OK;
    uint32  result = 0;
    qapi_SPIM_Instance_t qup_instance;
	uint16 spi_command = 0;


    if (pRegData == NULL) {
        
        return HTC_EINVAL;
    }

    result = spi_get_deviceid_func(dev->id, &qup_instance);

    if(HTC_OK != result)
    {
    
        return HTC_ERROR;
    }
	
    /* Populate the spi config params */
    result = spi_config_func(dev, 0, SPI_TRANS_SHORT); /* Disable loopback */
    if(HTC_OK != result )
    {
    
        return HTC_ERROR;
    }
    
	spi_command = (SPI_READ_16BIT_MODE) | (SPI_INTERNAL_16BIT_MODE) | (regaddr & (SPI_REG_ADDRESS_MASK));

    // Allocate the desc for write and read
    dev->tx_desc.buf_len = 2;

    result = qapi_SPIM_Open(qup_instance, &dev->hSPI);
    if (result != 0)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }

    result = qapi_SPIM_Enable(dev->hSPI);
    if (result != 0)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }

    dev->tx_desc.buf_phys_addr = &spi_command;

    //Perform only Write (rx info = NULL)
    result = qapi_SPIM_Full_Duplex(dev->hSPI, &dev->spi_trans_config, &dev->tx_desc, NULL, spi_io_complete_cb, dev);
    if (result != 0)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }

    // wait for test thread finish
    result = qurt_signal_wait(&dev->spi_Tx_done, dev->sig_mask, QURT_SIGNAL_ATTR_WAIT_ALL);
    if (result != dev->sig_mask)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }
    qurt_signal_clear(&dev->spi_Tx_done, dev->sig_mask);
    
    result = spi_config_func(dev, 0, data_width); /* Disable loopback */


    // Allocate the desc for read
    dev->rx_desc.buf_len = data_width;
    dev->rx_desc.buf_phys_addr = dev->pbuf;
    memset(dev->pbuf, 0 , sizeof(uint32));
   

    //Perform only Read (tx info = NULL)
    result = qapi_SPIM_Full_Duplex(dev->hSPI, &dev->spi_trans_config, NULL, &dev->rx_desc, spi_io_complete_cb, dev);
    if (result != 0)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }

    // wait for test thread finish
    result = qurt_signal_wait(&dev->spi_Tx_done, dev->sig_mask, QURT_SIGNAL_ATTR_WAIT_ALL);
    if (result != dev->sig_mask)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }
	
    qurt_signal_clear(&dev->spi_Tx_done, dev->sig_mask);

    result = qapi_SPIM_Disable(dev->hSPI);
    if (result != 0)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }

    result = qapi_SPIM_Close(dev->hSPI);
    if (result != 0)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }

    memcpy(pRegData, dev->pbuf, sizeof(uint32));
    
    return Test_Result;
}

htc_status_t spi_hal_reg_write (spi_device_t *dev, uint16 regaddr, uint32 RegData, spi_trans_bit_width_t data_width)
{
    uint32 Test_Result = HTC_OK;
    uint32 result = 0;
    qapi_SPIM_Instance_t qup_instance;
	uint16 spi_command = 0;
	
    result = spi_get_deviceid_func(1, &qup_instance);

    if(HTC_OK != result)
    {
    
        return HTC_ERROR;
    }
	
    /* Populate the spi config params */
     result = spi_config_func(dev, 0, SPI_TRANS_SHORT); /* Disable loopback */
    
    if(HTC_OK != result )
    {
    
        return HTC_EINVAL;
    }
	
	spi_command = ((SPI_WRITE_16BIT_MODE) | (SPI_INTERNAL_16BIT_MODE) | (regaddr & SPI_REG_ADDRESS_MASK));

    // Allocate the desc for write and read
    dev->tx_desc.buf_len = 2;

    result = qapi_SPIM_Open(qup_instance, &dev->hSPI);
    if (result != 0)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }

    result = qapi_SPIM_Enable(dev->hSPI);
    if (result != 0)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }

    dev->tx_desc.buf_phys_addr = &spi_command;

    //Perform only Write (rx info = NULL)
    result = qapi_SPIM_Full_Duplex(dev->hSPI, &dev->spi_trans_config, &dev->tx_desc, NULL, spi_io_complete_cb, dev);
    if (result != 0)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }

    // wait for test thread finish
    result = qurt_signal_wait(&dev->spi_Tx_done, dev->sig_mask,  QURT_SIGNAL_ATTR_WAIT_ALL);
    if (result != dev->sig_mask)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }
    qurt_signal_clear(&dev->spi_Tx_done, dev->sig_mask);
    
    memcpy(dev->pbuf, &RegData , sizeof(uint32));

    // Allocate the desc for write and read
    dev->tx_desc.buf_len = data_width;
    dev->tx_desc.buf_phys_addr = dev->pbuf;
    
    result = spi_config_func(dev, 0, data_width); /* Disable loopback */

    //Perform only Write (rx info = NULL)
    result = qapi_SPIM_Full_Duplex(dev->hSPI, &dev->spi_trans_config, &dev->tx_desc, NULL, spi_io_complete_cb, dev);
    if (result != 0)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }

    // wait for test thread finish
    result = qurt_signal_wait(&dev->spi_Tx_done, dev->sig_mask, QURT_SIGNAL_ATTR_WAIT_ALL);
    if (result != dev->sig_mask)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }
    qurt_signal_clear(&dev->spi_Tx_done, dev->sig_mask);
 
    result = qapi_SPIM_Disable(dev->hSPI);
    if (result != 0)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }

    result = qapi_SPIM_Close(dev->hSPI);
    if (result != 0)
    {
    
        Test_Result = HTC_ERROR;
        return Test_Result;
    }
    
    return Test_Result;
}

uint32 spi_hal_default_spi_slave_config (spi_device_t *dev, SPI_SLV_CONFIG_OPTIONS spiSlaveOptions)
{
    uint16 u16ConfigRegVal = 0;

    if (spiSlaveOptions & SPI_CONFIG_DEFAULT)
    {
        u16ConfigRegVal = (u16ConfigRegVal | SPI_SLV_REG_SPI_CONFIG_VAL_DEFAULT);
    }

    return spi_hal_reg_write(dev, SPI_SLV_REG_SPI_CONFIG, u16ConfigRegVal, SPI_TRANS_SHORT);
}


htc_status_t spi_hal_hcr_read_reg (spi_device_t *dev, uint16 regaddr, uint8 *pRegData)
{
    htc_status_t status = HTC_OK;
    uint32 reg_data=0;
    uint32 retry_count = SPI_SLV_TRANS_RETRY_COUNT;

    qurt_mutex_lock(&dev->spi_data_xfer_mutex);

    /* PIO write number of bytes (N) to HOST_CTRL_BYTE_SIZE register (max of 32 bytes). */
    reg_data = 1;
    status = spi_hal_reg_write (dev, SPI_SLV_REG_HOST_CTRL_BYTE_SIZE, reg_data, SPI_TRANS_SHORT);


    /* PIO write to HOST_CTRL_CONFIG.address, HOST_CTRL_CONFIG.direction and HOST_CTRL_CONFIG.enable. */
    reg_data = (HOST_CTRL_REG_DIR_RD | (regaddr & HOST_CTRL_REG_ADDRESS_MSK));
    status = spi_hal_reg_write (dev, SPI_SLV_REG_HOST_CTRL_CONFIG, reg_data, SPI_TRANS_SHORT);

    reg_data = (HOST_CTRL_REG_ENABLE | HOST_CTRL_REG_DIR_RD | (regaddr & HOST_CTRL_REG_ADDRESS_MSK));
    status = spi_hal_reg_write(dev, SPI_SLV_REG_HOST_CTRL_CONFIG, reg_data, SPI_TRANS_SHORT);

    reg_data = 0;
    /* PIO read from SPI_STATUS.host_access_done */
    status = spi_hal_reg_read(dev, SPI_SLV_REG_SPI_STATUS, &reg_data, SPI_TRANS_SHORT);

    while ( ((reg_data & SPI_SLV_REG_SPI_STATUS_BIT_HOST_ACCESS_DONE_MSK)
            != SPI_SLV_REG_SPI_STATUS_BIT_HOST_ACCESS_DONE_VAL) && (retry_count != 0))
    {
        qapi_Task_Delay(SPI_SLV_IO_DELAY); /* 500ms */
        status = spi_hal_reg_read(dev, SPI_SLV_REG_SPI_STATUS, &reg_data, SPI_TRANS_SHORT);
        retry_count--;
    }

    if (!retry_count) {
        ASSERT(0);
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);
        
        return HTC_ERROR;
    }

    /* Upon polling success ,PIO read to HOST_CTRL_RD_PORT using N data frames to retrieve read data. */
    reg_data = 0;
    spi_hal_reg_read(dev, SPI_SLV_REG_HOST_CTRL_RD_PORT,  &reg_data, SPI_TRANS_BYTE);
    *pRegData = (uint8) reg_data;
    qurt_mutex_unlock(&dev->spi_data_xfer_mutex);
    return status;
}


htc_status_t  spi_hal_hcr_write_reg (spi_device_t *dev, uint16 regaddr, uint8 RegData)
{
    htc_status_t status = HTC_OK;
    uint32 retry_count = SPI_SLV_TRANS_RETRY_COUNT;
    uint32 reg_data=0;

    qurt_mutex_lock(&dev->spi_data_xfer_mutex);


    /* PIO write number of bytes (N) to HOST_CTRL_BYTE_SIZE register (max of 32 bytes). */
    reg_data = 1;
    status = spi_hal_reg_write (dev, SPI_SLV_REG_HOST_CTRL_BYTE_SIZE, reg_data, SPI_TRANS_SHORT);

    // 2. PIO write to HOST_CTRL_WR_PORT with N data frames.
    status = spi_hal_reg_write(dev, SPI_SLV_REG_HOST_CTRL_WR_PORT, RegData, SPI_TRANS_BYTE);


    // 3. PIO write to HOST_CTRL_CONFIG.address, HOST_CTRL_CONFIG.direction and HOST_CTRL_CONFIG.enable.
    reg_data = (HOST_CTRL_REG_DIR_WR | (regaddr & HOST_CTRL_REG_ADDRESS_MSK));
    status = spi_hal_reg_write(dev, SPI_SLV_REG_HOST_CTRL_CONFIG, reg_data, SPI_TRANS_SHORT);
    
    reg_data = (HOST_CTRL_REG_ENABLE | HOST_CTRL_REG_DIR_WR | (regaddr & HOST_CTRL_REG_ADDRESS_MSK));
    status = spi_hal_reg_write(dev, SPI_SLV_REG_HOST_CTRL_CONFIG, reg_data, SPI_TRANS_SHORT);

    reg_data = 0;
    // 4. PIO read from SPI_STATUS.host_access_done
    status = spi_hal_reg_read(dev, SPI_SLV_REG_SPI_STATUS, &reg_data, SPI_TRANS_SHORT);

    while (((reg_data & SPI_SLV_REG_SPI_STATUS_BIT_HOST_ACCESS_DONE_MSK)
            != SPI_SLV_REG_SPI_STATUS_BIT_HOST_ACCESS_DONE_VAL) && (retry_count != 0))
    {
        qapi_Task_Delay(SPI_SLV_IO_DELAY); /* 500ms */
        status = spi_hal_reg_read(dev, SPI_SLV_REG_SPI_STATUS, &reg_data, SPI_TRANS_SHORT);
        retry_count--;
    }
    if (retry_count == 0) {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);        
        return HTC_ERROR;
    }
    qurt_mutex_unlock(&dev->spi_data_xfer_mutex);    
    return status;
}

htc_status_t spi_hal_hcr_read (spi_device_t *dev, io_mode_t iomode, uint16 regaddr, uint8 *pRegData, uint32 count)
{
    htc_status_t status = HTC_OK;
    uint32 i=0;

    for (i = 0; i<count; i++)
    {
        if (iomode == FIFO_MODE)
        {
            status = spi_hal_hcr_read_reg (dev, (regaddr), &pRegData[i]); 
        } else
        {
            status = spi_hal_hcr_read_reg (dev, (regaddr+i), &pRegData[i]);             
        }
        if (status == HTC_ERROR) return status;
    }
    return status;
}

htc_status_t  spi_hal_hcr_write (spi_device_t *dev, io_mode_t iomode, uint16 regaddr, uint8* pRegData, uint32 count)
{
    htc_status_t status = HTC_OK;
    uint32 i=0;

    for (i=0; i<count; i++)
    {
        if (iomode == FIFO_MODE)
        {
            status = spi_hal_hcr_write_reg (dev, (regaddr), pRegData[i]);
        } else
        {
            status = spi_hal_hcr_write_reg (dev, (regaddr+i), pRegData[i]);
        }
        if (status == HTC_ERROR) return status;
    }
    return status;
}

htc_status_t spi_hal_dma_read (spi_device_t *dev, uint16 regaddr, uint8 *pRegData, uint32 size)
{
    uint32 u16RdByteAvl = 0;
	uint16 spi_command = 0;
    qapi_Status_t status;
    qapi_SPIM_Instance_t qup_instance;

    qurt_mutex_lock(&dev->spi_data_xfer_mutex);    


    spi_hal_reg_read(dev, SPI_SLV_REG_RDBUF_BYTE_AVA, &u16RdByteAvl, SPI_TRANS_SHORT);

    if (u16RdByteAvl < size) {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);            
        return HTC_PENDING;
    }

    /* Program DMA transfer size */
    spi_hal_reg_write(dev, SPI_SLV_REG_DMA_SIZE, size, SPI_TRANS_SHORT);
   
    spi_get_deviceid_func(1, &qup_instance);
    
    status = qapi_SPIM_Open(qup_instance, &dev->hSPI);
    if (status != 0)
    {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);            
        return qapi_to_htcstatus(status);
    }

    status = qapi_SPIM_Enable(dev->hSPI);
    if (status != 0)
    {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);            
        return qapi_to_htcstatus(status);
    }

    /* Command phase */
    /* Populate the spi config params */
    spi_config_func(dev, 0, SPI_TRANS_SHORT); /* Disable loopback */
	spi_command = (SPI_READ_16BIT_MODE) | (SPI_EXTERNAL_16BIT_MODE) | (regaddr & SPI_REG_ADDRESS_MASK );

    // Allocate the desc for write and read
    dev->tx_desc.buf_len = 2;
    dev->tx_desc.buf_phys_addr = &spi_command;

    //Perform only Write (rx info = NULL)
    status = qapi_SPIM_Full_Duplex(dev->hSPI, &dev->spi_trans_config, &dev->tx_desc, NULL, spi_io_complete_cb, dev);
    if (status != 0)
    {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);            
        return status;
    }

    // wait for test thread finish
    status = qurt_signal_wait(&dev->spi_Tx_done, dev->sig_mask, QURT_SIGNAL_ATTR_WAIT_ALL);
    if (status != dev->sig_mask)
    {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);            
        return qapi_to_htcstatus(status);
    }
    qurt_signal_clear(&dev->spi_Tx_done, dev->sig_mask);

    /* Data phase */
    spi_config_func(dev, 0, SPI_TRANS_BYTE);
    
    // Allocate the desc for read
    dev->rx_desc.buf_len = size;
    dev->rx_desc.buf_phys_addr = dev->pdata_buf;

    //Perform only Read (tx info = NULL)
    status = qapi_SPIM_Full_Duplex(dev->hSPI, &dev->spi_trans_config, NULL, &dev->rx_desc, spi_io_complete_cb, dev);
    if (status != 0)
    {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);            
        return qapi_to_htcstatus(status);

    }

    // wait for test thread finish
    status = qurt_signal_wait(&dev->spi_Tx_done, dev->sig_mask, QURT_SIGNAL_ATTR_WAIT_ALL);
    if (status != dev->sig_mask)
    {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);            
        return qapi_to_htcstatus(status);
    }

    qurt_signal_clear(&dev->spi_Tx_done, dev->sig_mask);

    status = qapi_SPIM_Disable(dev->hSPI);
    if (status != 0)
    {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);            
        return qapi_to_htcstatus(status);
    }

    status = qapi_SPIM_Close(dev->hSPI);
    if (status != 0)
    {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);            
        return qapi_to_htcstatus(status);

    }

    /* Copy the read data to app buffer from driver buffer */
    memcpy(pRegData, dev->pdata_buf, size);
    qurt_mutex_unlock(&dev->spi_data_xfer_mutex);            

    return qapi_to_htcstatus(status);
}

htc_status_t spi_hal_dma_write (spi_device_t *dev, uint16 regaddr, uint8 *pRegData, uint32 size)
{
    uint32 u16WrByteAvl = 0;
	uint16 spi_command = 0;
    qapi_Status_t status;
    qapi_SPIM_Instance_t qup_instance;

    qurt_mutex_lock(&dev->spi_data_xfer_mutex);  

    spi_hal_reg_read (dev, SPI_SLV_REG_WRBUF_SPC_AVA, &u16WrByteAvl, SPI_TRANS_SHORT);

    if (u16WrByteAvl < size) {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);                    
        return HTC_PENDING;
    }

    /* Program DMA transfer size */
    spi_hal_reg_write (dev, SPI_SLV_REG_DMA_SIZE, size, SPI_TRANS_SHORT);
   
    status = spi_get_deviceid_func(1, &qup_instance);
    status = qapi_SPIM_Open(qup_instance, &dev->hSPI);
    if (status != 0)
    {
       qurt_mutex_unlock(&dev->spi_data_xfer_mutex);                    
       return qapi_to_htcstatus(status);
    }

    status = qapi_SPIM_Enable(dev->hSPI);
    if (status != 0)
    {
       qurt_mutex_unlock(&dev->spi_data_xfer_mutex);                        
       return qapi_to_htcstatus(status);
    }

    /* Command phase */
    /* Populate the spi config params */
    spi_config_func(dev, 0, SPI_TRANS_SHORT); /* Disable loopback */

	spi_command = (SPI_WRITE_16BIT_MODE) | (SPI_EXTERNAL_16BIT_MODE) | (regaddr & (SPI_REG_ADDRESS_MASK));

    qurt_signal_init(&dev->spi_Tx_done);

    // Allocate the desc for write and read
    dev->tx_desc.buf_len = 2;
    dev->tx_desc.buf_phys_addr = &spi_command;

    //Perform only Write (rx info = NULL)
    status = qapi_SPIM_Full_Duplex(dev->hSPI, &dev->spi_trans_config, &dev->tx_desc, NULL, spi_io_complete_cb, dev);
    if (status != 0)
    {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);                            
        return qapi_to_htcstatus(status);
    }

    // wait for test thread finish
    status = qurt_signal_wait(&dev->spi_Tx_done, dev->sig_mask, QURT_SIGNAL_ATTR_WAIT_ALL);
    if (status != dev->sig_mask)
    {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);                                
        return qapi_to_htcstatus(status);
    }
    qurt_signal_clear(&dev->spi_Tx_done, dev->sig_mask);

    /* Data phase */
    spi_config_func(dev, 0, SPI_TRANS_BYTE);

    /* Copy the writeable data to driver buffer */
    memcpy(dev->pdata_buf, pRegData, size);

    // Allocate the desc for read
    dev->tx_desc.buf_len = size;
    dev->tx_desc.buf_phys_addr = dev->pdata_buf;

    status = qapi_SPIM_Full_Duplex(dev->hSPI, &dev->spi_trans_config, &dev->tx_desc, NULL, spi_io_complete_cb, dev);
    if (status != 0)
    {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);                                
        return qapi_to_htcstatus(status);
    }

    // wait for test thread finish
    status = qurt_signal_wait(&dev->spi_Tx_done, dev->sig_mask, QURT_SIGNAL_ATTR_WAIT_ALL);
    if (status != dev->sig_mask)
    {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);                                
        return qapi_to_htcstatus(status);
    }

    qurt_signal_clear(&dev->spi_Tx_done, dev->sig_mask);

    status = qapi_SPIM_Disable(dev->hSPI);
    if (status != 0)
    {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);                            
        return qapi_to_htcstatus(status);
    }

    status = qapi_SPIM_Close(dev->hSPI);
    if (status != 0)
    {
        qurt_mutex_unlock(&dev->spi_data_xfer_mutex);                                
        return qapi_to_htcstatus(status);
    }
    qurt_mutex_unlock(&dev->spi_data_xfer_mutex);                                
    return qapi_to_htcstatus(status);
}


void spi_hal_int_task (void* h)
{
   trans_device_t *trans_dev = (trans_device_t *)h;
   uint32 signalled;
   spi_device_t *dev;
   hif_isr_handler_t pfunc;
   
   dev = (spi_device_t *) trans_dev->dev;

   if (dev == NULL)
   { 
       QAPI_FATAL_ERR(0,0,0);
       qurt_signal_destroy(&(dev->spi_isr_signal));
       qurt_thread_stop();
       return;
   }
   

   // Clear any pending signal before going into the wait loop
   qurt_signal_clear(&dev->spi_isr_signal, (SPI_HAL_ISR_TASK_TERM_EVT | SPI_HAL_ISR_TASK_EVT));   

   for(;;)
   {
      signalled = qurt_signal_wait(&dev->spi_isr_signal, 
        (SPI_HAL_ISR_TASK_TERM_EVT | SPI_HAL_ISR_TASK_EVT), QURT_SIGNAL_ATTR_CLEAR_MASK);
      qurt_signal_clear(&(dev->spi_isr_signal), signalled);

      if (signalled & SPI_HAL_ISR_TASK_TERM_EVT)
      {
      	/* Signal back the requested thread to kill async task */
         qurt_signal_set(&(dev->spi_isr_task_completion), SPI_HAL_ISR_TASK_TERM_COMPLETE_EVT);
         qurt_signal_destroy(&(dev->spi_isr_signal));
         qurt_thread_stop();
         break;
      }

      if (signalled & SPI_HAL_ISR_TASK_EVT)
      {
          pfunc = dev->isr_cb;   
         
          /* Call  HIF layer interrupt call back */
          if (pfunc != NULL)
          {
             pfunc(trans_dev);
          }
          qapi_GPIOINT_Enable_Interrupt(dev->spi_intr_gpio_hdl, SPI_SLV_INTR_GPIO_NUM);
      }
   }
}

void spi_hal_gpio_isr(qapi_GPIOINT_Callback_Data_t data)
{
    trans_device_t *trans_dev = (trans_device_t *)data;
    spi_device_t *dev;
    
    dev = (spi_device_t *) trans_dev->dev;
    if (dev == NULL) return;
    
    qapi_GPIOINT_Disable_Interrupt(dev->spi_intr_gpio_hdl, SPI_SLV_INTR_GPIO_NUM);
    
    qurt_signal_set(&dev->spi_isr_signal, SPI_HAL_ISR_TASK_EVT);

}

static trans_dev_ops_t spi_ops =  {
   .read_data = spi_hal_read_data,
   .write_data = spi_hal_write_data,
   .register_interrupt = spi_hal_register_interrupt,
   .deregister_interrupt = spi_hal_deregister_interrupt,
   .enable_trans = spi_hal_enable_trans,
   .disable_trans = spi_hal_disable_trans,
};

htc_status_t spi_hal_read_data (void *tdev, io_mode_t iomode, unsigned int addr, void *src, int count)
{
    trans_device_t *trans_dev = tdev;
	spi_device_t *dev = NULL;
    htc_status_t status = HTC_OK;
    uint32 temp;

    if (tdev == NULL) return HTC_EINVAL;
    dev = trans_dev->dev;

    if ((addr >= HIF_HOST_CONTROL_REG_START_ADDR) && (addr <= HIF_HOST_CONTROL_REG_END_ADDR))
    {
        if (addr == SPI_STATUS_ADDRESS)
        {
            status = spi_hal_reg_read(dev, SPI_SLV_REG_INTR_CAUSE, (uint32 *)src, SPI_TRANS_SHORT);
        } else if (addr == RX_LOOKAHEAD0_ADDRESS)
        {
            temp = 0;
            *(uint32*)src = 0;
            status = spi_hal_reg_read(dev, SPI_SLV_REG_RDBUF_LOOKAHEAD1, &temp, SPI_TRANS_SHORT);
           
            *(uint32*)src = (((temp >> 8) & 0xFF) |((temp << 8)  & 0xFF00));
            status = spi_hal_reg_read(dev, SPI_SLV_REG_RDBUF_LOOKAHEAD2, &temp, SPI_TRANS_SHORT);
            *(uint32*)src |= ((((temp >> 8) & 0xFF) |((temp << 8)  & 0xFF00)) << 16);

        } else
        {
            status = spi_hal_hcr_read (dev, iomode, addr, src, count);
        }
    }
    else
    {
        status = spi_hal_dma_read (dev, addr, src, count);
    }
	return status;
}

htc_status_t spi_hal_write_data (void *tdev, io_mode_t iomode, unsigned int addr, void *src, int count)
{
    trans_device_t *trans_dev = tdev;
	spi_device_t *dev = NULL;
    htc_status_t status = HTC_OK;

    if (tdev == NULL) return HTC_EINVAL;
    dev = trans_dev->dev;

    if ((addr >= HIF_HOST_CONTROL_REG_START_ADDR) && (addr <= HIF_HOST_CONTROL_REG_END_ADDR))
    {
        status = spi_hal_hcr_write (dev, iomode, addr, src, count);
    } 
    else if (addr == SPI_STATUS_ADDRESS)
    {
        status = spi_hal_reg_write(dev, SPI_SLV_REG_INTR_CAUSE, (*(uint32 *)src), SPI_TRANS_SHORT);
    }
    else
    {
        status = spi_hal_dma_write (dev, addr, src, count);
    }
	return status;
}

static htc_status_t spi_hal_enable_trans (void *tdev) {
    htc_status_t  status;
	spi_device_t *dev = NULL;
    trans_device_t *trans_dev = tdev;

    if(tdev == NULL)
    {
        return HTC_EINVAL;
    }
    dev = (spi_device_t*) trans_dev->dev;
    qurt_mutex_lock(&dev->spi_data_xfer_mutex);    
    
	/* Enable IO */
    status = spi_hal_reg_write(dev, SPI_SLV_REG_SPI_CONFIG, SPI_SLV_REG_SPI_CONFIG_IO_ENABLE, SPI_TRANS_SHORT);
    qapi_Task_Delay(SPI_SLV_IO_DELAY); /* 500ms */

    status = spi_hal_default_spi_slave_config(dev, SPI_CONFIG_DEFAULT);
    qapi_Task_Delay(100); /* 100us */	

    qurt_mutex_unlock(&dev->spi_data_xfer_mutex);    

	/* Enable the card interrupt*/ 
    qapi_GPIOINT_Enable_Interrupt(dev->spi_intr_gpio_hdl, SPI_SLV_INTR_GPIO_NUM);
	
	return status;
}


htc_status_t spi_hal_disable_trans (void *tdev) {
    qapi_Status_t  status;
	spi_device_t *dev = NULL;
    trans_device_t *trans_dev = tdev;

    if(tdev == NULL)
    {
        return HTC_EINVAL;
    }
    dev = (spi_device_t*) trans_dev->dev;
    
    qurt_mutex_lock(&dev->spi_data_xfer_mutex);    

	/* Disable IO */
    spi_hal_reg_write(dev, SPI_SLV_REG_SPI_CONFIG, 0, SPI_TRANS_SHORT);
    qapi_Task_Delay(SPI_SLV_IO_DELAY); /* 500ms */
    qurt_mutex_unlock(&dev->spi_data_xfer_mutex);    

    status = qapi_GPIOINT_Disable_Interrupt(dev->spi_intr_gpio_hdl, SPI_SLV_INTR_GPIO_NUM);
    
    
	
	return qapi_to_htcstatus(status);
}

htc_status_t spi_hal_register_interrupt (void *tdev, hif_isr_handler_t pfunc) {
	spi_device_t *dev = NULL;
    trans_device_t *trans_dev = tdev;

    if(tdev == NULL)
    {
        return HTC_EINVAL;
    }
    dev = (spi_device_t*) trans_dev->dev;

    if (dev == NULL)
    {
        return HTC_EINVAL;
    }

    dev->isr_cb = (void *) pfunc;
    

    /* Register GPIO interrupt */
    if (QAPI_OK !=  qapi_GPIOINT_Register_Interrupt(dev->spi_intr_gpio_hdl,
                                                    SPI_SLV_INTR_GPIO_NUM,
                                                    spi_hal_gpio_isr,
                                                    (qapi_GPIOINT_Callback_Data_t)tdev,
                                                    QAPI_GPIOINT_TRIGGER_LEVEL_LOW_E,
                                                    QAPI_GPIOINT_PRIO_HIGH_E, false ))
    {
        return HTC_ERROR;
    }


    qapi_GPIOINT_Enable_Interrupt(dev->spi_intr_gpio_hdl, SPI_SLV_INTR_GPIO_NUM);

    spi_hal_reg_write(dev, SPI_SLV_REG_INTR_ENABLE, 0x21, SPI_TRANS_SHORT);

	return HTC_OK;
}


htc_status_t spi_hal_deregister_interrupt (void *tdev) {
	qapi_Status_t status;
	spi_device_t *dev = NULL;
    trans_device_t *trans_dev = tdev;

    if(tdev == NULL)
    {
        return HTC_EINVAL;
    }
    dev = (spi_device_t*) trans_dev->dev;

    spi_hal_reg_write(dev, SPI_SLV_REG_INTR_ENABLE, 0x00, SPI_TRANS_SHORT);


    status = qapi_GPIOINT_Disable_Interrupt(dev->spi_intr_gpio_hdl, SPI_SLV_INTR_GPIO_NUM);
    status = qapi_GPIOINT_Deregister_Interrupt (dev->spi_intr_gpio_hdl, SPI_SLV_INTR_GPIO_NUM);


	return qapi_to_htcstatus(status);

}


htc_status_t spi_hal_init (trans_device_t *tdev)
{
    spi_device_t *dev = NULL;
    uint32 regdata  = 0;


    /* Allocate the device context */
    dev = (spi_device_t*) malloc(sizeof(spi_device_t));

    if(dev == NULL)
    {
        return HTC_ERROR;
    }

    tdev->ops = &spi_ops;
    tdev->dev = dev;

    memset (dev, 0, sizeof(spi_device_t));
    dev->id = (tdev->id + QAPI_SPIM_INSTANCE_1_E);

    /* Align the buffers */
    dev->pbuf = (uint8 *) (((uint32)dev->buffer + sizeof(uint32) - 1) & \
                           ~(sizeof(uint32) - 1));
    dev->pdata_buf = (uint8 *) (((uint32)dev->data_buf + sizeof(uint32) - 1) & \
                                ~(sizeof(uint32) - 1));

    qurt_signal_init(&dev->spi_Tx_done);
    dev->sig_mask = 0x1111;
    qurt_mutex_init(&dev->spi_data_xfer_mutex);
    qurt_signal_init(&dev->spi_isr_signal);
    qurt_signal_init(&dev->spi_isr_task_completion);

    qurt_thread_attr_set_name(&dev->spi_int_task_attr, SPI_HAL_INT_TASK_NAME);
    qurt_thread_attr_set_priority(&dev->spi_int_task_attr,
                                    SPI_HAL_INT_TASK_PRIO);
    qurt_thread_attr_set_stack_size(&dev->spi_int_task_attr,
                                    SPI_HAL_INT_TASK_STK_SIZE);

    if(QURT_EOK != qurt_thread_create(&dev->spi_int_task_id,
                    &dev->spi_int_task_attr, spi_hal_int_task, (void *)tdev))
    {
        dev->spi_int_task_id = (unsigned long) NULL;
        goto ERROR;
    }

   
    /* Reset the Block */
    if(spi_hal_reg_write(dev, SPI_SLV_REG_SPI_CONFIG,
                SPI_SLV_REG_SPI_CONFIG_VAL_RESET, SPI_TRANS_SHORT) != HTC_OK)
    {
        goto ERROR;
    }

    qapi_Task_Delay(SPI_SLV_IO_DELAY);

    /* do some dummy reads */
    if ((spi_hal_reg_read(dev, SPI_SLV_REG_SPI_STATUS, &regdata,
                        SPI_TRANS_SHORT) != QAPI_OK) || regdata == 0)
    {
        goto ERROR;
    }

    /* Pin configuration */
    dev->spi_intr_gpio_config.pin = SPI_SLV_INTR_GPIO_NUM;
    dev->spi_intr_gpio_config.func = 0;
    dev->spi_intr_gpio_config.dir = 0;
    dev->spi_intr_gpio_config.pull = 0;
    dev->spi_intr_gpio_config.drive = 1;

    if(qapi_TLMM_Get_Gpio_ID(&dev->spi_intr_gpio_config,
                                &dev->spi_intr_gpio_id)  != QAPI_OK)
    {
        goto ERROR;
    }
    
    if (qapi_TLMM_Config_Gpio(dev->spi_intr_gpio_id,
                                &dev->spi_intr_gpio_config)  != QAPI_OK)
    {
        goto ERROR;
    }

    /*
     * Note, qurt SDCC driver does not have hotplug detection functionality.
     * hence it is assumed that the device is always present.
     */
    if(tdev->hif_callbacks.hif_trans_device_inserted != NULL)
    {
        tdev->hif_callbacks.hif_trans_device_inserted(tdev, FALSE);
    }

    return HTC_OK;

ERROR:
    tdev->hif_callbacks.hif_trans_device_inserted = NULL;
    tdev->hif_callbacks.hif_trans_device_removed = NULL;
    spi_hal_deinit(tdev);
    return HTC_ERROR;
}


htc_status_t spi_hal_deinit (trans_device_t *tdev)
{
    spi_device_t *dev;
    uint32 signalled;

    dev = tdev->dev;

    /*
     * This shall also be called from hot plug device removal handler
     * Since Quartz SDCC driver does not have hotplug functionality,
     * the device disconnect event is sent during de-initialization
     */
    if (tdev->hif_callbacks.hif_trans_device_removed != NULL)
        tdev->hif_callbacks.hif_trans_device_removed(tdev, FALSE);

    if(dev->spi_intr_gpio_id)
        qapi_TLMM_Release_Gpio_ID(&dev->spi_intr_gpio_config,
                                    dev->spi_intr_gpio_id);

    /* Send terminate signal to async_task */
    if(dev->spi_isr_signal)
        qurt_signal_set(&(dev->spi_isr_signal),
                        SPI_HAL_ISR_TASK_TERM_COMPLETE_EVT);
    
    /* Wait till the async_task gets killed */
    if(dev->spi_isr_task_completion)
    {
        signalled = qurt_signal_wait(&(dev->spi_isr_task_completion),
                                      SPI_HAL_ISR_TASK_TERM_COMPLETE_EVT,
                                      QURT_SIGNAL_ATTR_WAIT_ANY);

        if (signalled & SPI_HAL_ISR_TASK_TERM_COMPLETE_EVT)
        {
            qurt_signal_clear(&(dev->spi_isr_task_completion),
                (SPI_HAL_ISR_TASK_TERM_COMPLETE_EVT | SPI_HAL_ISR_TASK_EVT));
        }
        else
        {
            QAPI_FATAL_ERR(0,0,0);
        }
    }

    if(dev->spi_isr_task_completion)
        qurt_signal_destroy(&dev->spi_isr_task_completion);

    if(dev->spi_Tx_done)
        qurt_signal_destroy(&dev->spi_Tx_done);

    if(dev->spi_data_xfer_mutex)
        qurt_mutex_destroy(&dev->spi_data_xfer_mutex);

    free(dev);
    tdev->ops = NULL;
    tdev->dev = NULL;

    return HTC_OK;
}

