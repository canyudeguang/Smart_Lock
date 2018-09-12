#ifndef __QAPI_SDCC_H__
#define __QAPI_SDCC_H__

/** 
    @file  qapi_master_sdcc.h
    @brief MASTER SDCC qapi
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/** @addtogroup qapi_master_sdcc
@{ */

/*----------------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
 
/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
	/**< By default, read operation is performed  */
    QAPI_SDCC_DIR_READ_E = 0,   	
	/**< Write operation   */	
    QAPI_SDCC_DIR_WRITE_E = (1 << 0),   
	/**< Use block mode instead of byte mode */
    QAPI_SDCC_BLOCK_MODE_E = (1 << 1),   
	/**< Increase address register after R/W operation  */
    QAPI_SDCC_OP_INC_ADDR_E = (1 << 2), 
	/**<  use DMA */  
    QAPI_SDCC_USES_DMA_E = (1 << 3),   
	/**< Non-blocking OP, return immidiately */
    QAPI_SDCC_NON_BLOCKING_E = (1 << 4),      
	/**< Operation has data transfer phase */
	QAPI_SDCC_DATA_XFER_E = (1 << 5),      
	/**< Issue a command that is IO Abort */     
	QAPI_SDCC_ABORT_CMD_TYP_E = (1 << 6),       
	/**< Issue a command that is function select */
	QAPI_SDCC_RESUME_CMD_TYP_E = (1 << 7),       
	/**< Issue a command that is bus resume */
	QAPI_SDCC_SUSPEND_CMD_TYP_E = (1 << 8),     
	/**< Command that doesn't use CRC in response */ 
	QAPI_SDCC_NOCRC_CMD_TYP_E = (1 << 9),     
	/**< Command that doesn't use index in response */ 
	QAPI_SDCC_NOIDX_CMD_TYP_E = (1 << 10),     
	/**< Command that doesn't have a response */
	QAPI_SDCC_NORESP_CMD_TYP_E = (1 << 11),     
	/**< Read after write operation for CMD52 */
	QAPI_SDCC_READ_AFT_WRITE_E = (1 << 12), 
	/**< Use DDR mode */    
	QAPI_SDCC_USES_DDR_E = (1 << 13),     
	/**< No card detection */
	QAPI_SDCC_NO_CARD_DETECT_E = (1 << 14),   
	/**< Enable interrupt */  
	QAPI_SDCC_INTR_ENABLE_E = (1 << 15),     
	/**< Disable interrupt */
	QAPI_SDCC_INTR_DISABLE_E = (1 << 16)     
} qapi_SDCC_Op_Flags_e;

typedef struct {
	/**< adma descriptor items, indicate max count of qapi_SDCC_Dma_Item item */
    uint32_t  adma_Descr_Items;	
	/**< SDCC bus width 1 bit or 4 bits 
	  1 - 1 bit mode; 4 - 4 bits mode
	*/	
    uint32_t  SDCC_Bus_Width;		
	/**< bus frequency; indicates sdcc baud rate in Hz unit */
    uint32_t  freq;
	/**< block size; 
	   when using block transfer mode, it indicates size of each block 
	*/
	uint32_t  block_Size;	
} qapi_SDCC_Config_t;

typedef struct {
   /**< Pointer to the data buffer */
   uint8  *data_Buf_Ptr;      
   /**< Size of the data in bytes */
   uint32  size;          
} qapi_SDCC_Dma_Item;

typedef void  *qapi_SDCC_Handle;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
*  Initialize MASTER SDCC context. 
*
*  @param[in] sdcc_Config   SDCC configuration.
*  @param[out] sdcc_Handle  return SDCC context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*/
qapi_Status_t
qapi_SDCCM_Init(
	qapi_SDCC_Config_t  *sdcc_Config, 
	qapi_SDCC_Handle *sdcc_Handle
);

/**
*  de-initialize SDCC context.
*
*  @param[in] sdcc_Handle   SDCC context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*/
qapi_Status_t
qapi_SDCCM_Deinit (
    qapi_SDCC_Handle sdcc_Handle
);

/**
*  open SDCC device. 
*  activates SDCC device, if successful, the SDCC is ready for data transfer
*
*  @param[in] sdcc_Handle   SDCC context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*/
qapi_Status_t
qapi_SDCCM_Open
(
    qapi_SDCC_Handle sdcc_Handle
);

/**
*  close SDCC_Handle.
*
*  @param[in] sdcc_Handle   MASTER SDCC context pointer.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*/
qapi_Status_t
qapi_SDCCM_Close
(
    qapi_SDCC_Handle sdcc_Handle
);

/**
 *  register callback function.
 *
 *  @param[in] sdcc_Handle   MASTER SDCC context pointer.
 *  @param[in] isr_Cb   callback function.
 *  @param[in] param    callback function's parameter pointer.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
 */
qapi_Status_t
qapi_SDCCM_Intr_Register (
    qapi_SDCC_Handle sdcc_Handle,
    void (*isr_Cb)(void *param), 
    void *param
);

/**
 *  deregister callback function.
 *
 *  @param[in] sdcc_Handle   SDCC context pointer.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
 */
qapi_Status_t
qapi_SDCCM_Intr_Deregister (
    qapi_SDCC_Handle sdcc_Handle
);

/**
 *  send data into device by SDCC with extension command 53.
 *
 *  1. support DMA & FIFO mode
 *  2. support  block mode & byte mode
 *  3. if using block mode, then data_Len_In_Bytes = n * block_size
 *  4. data_Buf has to align 4-byte boundary
 *
 *  @param[in] sdcc_Handle        MASTER SDCC context pointer.
 *  @param[in] flags              operation flag.
 *  @param[in] dev_Fn             dev function number.
 *  @param[in] reg_Addr           register address.
 *  @param[in] data_Len_In_Bytes  data length.
 *  @param[in] data_Buf           data buffer pointer.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
 */

qapi_Status_t
qapi_SDCCM_Send_Data_Ext (
    qapi_SDCC_Handle sdcc_Handle,
	qapi_SDCC_Op_Flags_e flags, 
	uint32 dev_Fn, 
	uint32 reg_Addr, 
    uint32 data_Len_In_Bytes, 
	uint8 *data_Buf
);

/**
 *  fetch data on device by SDCC with extension Command 53.
 *
 *  1. support DMA & FIFO mode
 *  2. support  block mode & byte mode
 *  3. if using block mode, then data_Len_In_Bytes = n * block_size
 *  4. data_Buf has to align 4-byte boundary
 *
 *  @param[in] sdcc_Handle        MASTER SDCC context pointer.
 *  @param[in] flags              operation flag.
 *  @param[in] dev_Fn             dev function number.
 *  @param[in] reg_Addr           register address.
 *  @param[in] data_Len_In_Bytes  data length. 
 *  @param[in] data_Buf           data buffer pointer.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
 */

qapi_Status_t
qapi_SDCCM_Receive_Data_Ext (
    qapi_SDCC_Handle sdcc_Handle,
	qapi_SDCC_Op_Flags_e flags, 
	uint32 dev_Fn, 
	uint32 reg_Addr, 
    uint32 data_Len_In_Bytes, 
	uint8 *data_Buf
);

/** 
 *  send/receive one byte data into/from device by SDCC with CMD 52.
 *
 *  @param[in] sdcc_Handle        MASTER SDCC context pointer.
 *  @param[in] flags              operation flag.
 *  @param[in] dev_Fn             dev function number.
 *  @param[in] reg_Addr           register address.
 *  @param[in] data_In            sent one byte data.
 *  @param[in] data_Out           received one byte data.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
 */ 

qapi_Status_t
qapi_SDCCM_Send_Receive_Byte (
    qapi_SDCC_Handle sdcc_Handle,
	qapi_SDCC_Op_Flags_e flags, 
	uint32 dev_Fn, 
	uint32 reg_Addr, 
    uint8 data_In, 
	uint8 *data_Out
);

/** 
 *  send/receive data into/from device by SDCC with CMD 53.
 *
 *  1. only DMA mode transfer
 *  2. support  block mode & byte mode
 *  3. if using block mode, then data_Len_In_Bytes = n * block_size
 *  4. dma_Tab_Count max value see qapi_SDCC_Config_t
 *  5. data_Buf_Ptr in dma_Tab has to align 4-byte boundary
 *
 *  @param[in] sdcc_Handle        MASTER SDCC context pointer.
 *  @param[in] flags              operation flag.
 *  @param[in] dev_Fn             dev function number.
 *  @param[in] reg_Addr           register address.
 *  @param[in] data_Len_In_Bytes  data length (sum all data segment size). 
 *  @param[in] dma_Tab_Count      dma entry count. MAX value see qapi_SDCCM_Init
 *  @param[in] dma_Tab            dma entry array pointer.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
 */ 

qapi_Status_t
qapi_SDCCM_Send_Receive_Data_Scatter_Gather (
    qapi_SDCC_Handle sdcc_Handle,
	qapi_SDCC_Op_Flags_e flags, 
	uint32 dev_Fn, 
	uint32 reg_Addr, 
    uint32 data_Len_In_Bytes, 
	uint32 dma_Tab_Count, 
    qapi_SDCC_Dma_Item *dma_Tab
);

/** 
 *  enable/disable SDCC interrupt.
 *
 *  @param[in] sdcc_Handle        MASTER SDCC context pointer.
 *  @param[in] enable             enable/disable Interrupt.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 */
qapi_Status_t 
qapi_SDCCM_EnableDisableIrq(
    qapi_SDCC_Handle sdcc_Handle, 
	boolean enable
);

/** @} */ /* end_addtogroup qapi_master_sdcc */

#endif // ifndef __QAPI_MASTER_SDCC_H__
