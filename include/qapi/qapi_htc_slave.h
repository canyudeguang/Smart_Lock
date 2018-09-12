#ifndef __QAPI_HTC_SLAVE_H__
#define __QAPI_HTC_SLAVE_H__

/*
 * Copyright (c) 2017 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 * $ATH_LICENSE_TARGET_C$
 */

/*==================================================================================
       Copyright (c) 2017 Qualcomm Technologies, Inc.
                       All Rights Reserved.
       Confidential and Proprietary - Qualcomm Technologies, Inc.
==================================================================================*/
 /*********************************************************************
 *
 * @file  qapi_htc_slave.h
 * @brief HTC Slave QAPI APIs
*/

/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     -----------------------------------------------------------------
10/3/17    leo     (Tech Comm) Edited/added Doxygen comments and markup.
07/26/17   mmtd    Initial version
==================================================================================*/

/** @addtogroup qapi_htc_slave
@{ */

/**
 * The maximum number of bytes to which the caller can set the actual_length
 * field when sending a message to the host, and the maximum size
 * that can be specified to HTC_endpoint_bufsz_set.
 */
#define QAPI_HTC_BUFSZ_MAX_SEND 2048


#define QAPI_NUM_BUFFERS_PER_ENDPOINT 4

/**
 * Structure for use by HTC to track buffers, and for the caller and
 * HTC to communicate about buffers. The caller passes in a
 * pointer to one of these buffers on every send and every receive. There
 * must be a unique HTC_BUFFER for each buffer that is passed
 * in. HTC owns the HTC_BUFFER from the time it is passed in
 * until the time a send/receive completion is indicated, and the
 * caller must not modify it during that time.
 */

typedef struct qapi_HTC_bufinfo_s {
    struct qapi_HTC_bufinfo_s  *next;    /**< HTC_BUFFER list linkage field;
                                            used by HTC when HTC owns bufinfo,
                                            used by the caller when the caller owns
                                            bufinfo. */
    uint8                 *buffer;       /**< Pointer to the data buffer.
                                              This should not be modified by HTC. */
    uint16                actual_length; /**< Actual length in bytes.
                                              In recv_done, this indicates the valid
                                              bytes in buf, which is filled by HTC.
                                              Set maximum buffer size for Endpoint. \n
                                              In send, this indicates to HTC the 
                                              number of bytes to send to the host,
                                              and is filled by the caller.*/
    uint8               htc_flags;      /**< HTC flags (used by the HTC layer only). */
    uint8               end_point;      /**< Endpoint to which this buffer belongs (used by the HTC layer). */
    uint16              buffer_offset;  /**< Buffer offset from the reserved headroom for the HTC header.
                                             If the caller can shift the start of the payload such that
                                             the buffer can no longer satisfy the TrailerSpcCheckLimit
                                             set by the service, the caller can specify an offset.
                                             The offset is the distance between the current position and the
                                             area reserved for the HTC headroom.
                                             This value is zeroed by HTC when the buffer is processed.
                                             Applications that do not shift the buffer position can
                                             leave this value at 0. */
    uint16              app_context;    /**< Application context for the buffer. */
} qapi_HTC_bufinfo_t;


/** HTC slave Endpoint hardware ID. */
typedef enum
{
    QAPI_HTC_SLAVE_ENDPOINT_ID0 = 0,  /**< Endpoint ID is 0. */
    QAPI_HTC_SLAVE_ENDPOINT_ID1 = 1,  /**< Endpoint ID is 1. */
    QAPI_HTC_SLAVE_ENDPOINT_ID2 = 2,  /**< Endpoint ID is 2. */
    QAPI_HTC_SLAVE_ENDPOINT_ID3 = 3,   /**< Endpoint ID is 3. */
    QAPI_HTC_SLAVE_MAX_ENDPOINTS    
} qapi_HTCSlaveEndpointID_t;


/* HTC Slave Event ID */
typedef enum
{
    QAPI_HTC_SLAVE_BUFFER_RECEIVED,
    QAPI_HTC_SLAVE_BUFFER_SENT,
    QAPI_HTC_SLAVE_INIT_COMPLETE,
    QAPI_HTC_SLAVE_RESET_COMPLETE,     
    QAPI_HTC_SLAVE_MAX_EVENTS
} qapi_HTCSlave_event_id_t;

/* HTC instance IDs */
typedef enum {
    QAPI_HTC_SDIO_SLAVE_INSTANCE_ID = 0,
    QAPI_HTC_SPI_SLAVE_INSTANCE_ID,
    QAPI_HTC_MAX_INSTANCES
} qapi_HTC_Instance_id_t;


typedef void (*qapi_HTC_callback_pfn_t)(uint32 Instance, uint32 arg1, uint32 arg2);



/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
* Initializes a slave. This function is called once before any other HTC slave calls are made.
* This is a common initialization function for all Endpoint endpoints.
*
*  @param[in] Instance  Instance id of HTC slave
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*
*/

qapi_Status_t qapi_HTC_Slave_Init (uint32 Instance);

/**
* Shutsdown the slave. This function is called once before any other HTC slave calls are made.
* This is a common shutdown function for all Endpoint endpoints.
*
*  @param[in] Instance  Instance id of HTC slave
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*
*/

qapi_Status_t qapi_HTC_Slave_Shutdown (uint32 Instance);


/**
* Starts the slave. This function is called once before any other HTC slave calls are made.
* This is a common initialization function for all Endpoint endpoints.
*
*  @param[in] Instance  Instance id of HTC slave
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*
*/

qapi_Status_t qapi_HTC_Slave_Start (uint32 Instance);

/**
*  Stops a slave. This function disables all the Endpoint endpoints, frees all memory resources, 
* interrupts and deinitializes the Endpoint hardware, and terminates the HTC connection.
*
*  @param[in] Instance  Instance id of HTC slave
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/

qapi_Status_t qapi_HTC_Slave_Stop (uint32 Instance);


/**
* Sends data from the target to the host.
*
* This function queues a chain of buffer descriptors that are to be sent to the host.
* All buffers and the buffer descriptor memory must be provided by the application.
* The driver does not have any internal buffers.
*
* The function sends a message over the specified mailbox.
* A message is specified as a NULL-terminated list of bufinfo entries.
* The caller's Send Done callback is invoked for each buffer of the message.
* HTC may split and aggregate notification for completed buffers, as long
* as each sent buffer is included in some buffer list sent to the Send
* Done callback.

*
*  @param[in] Instance  Instance id of HTC slave
*  @param[in] inst         Endpoint ID on which the transfer is to be done. 
*  @param[in] buf          Pointer to the head of the buffer descriptor list
*                          that is to be sent on the specified Endpoint ID. 
*                          See #qapi_HTC_bufinfo_t for a description of the structure.
* 
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t qapi_HTC_Slave_Send_Data(uint32 Instance, qapi_HTCSlaveEndpointID_t inst,
                                       qapi_HTC_bufinfo_t *buf);

/**
* Receives data from the host to the target over the specified mailbox.
*
* A message is specified as a NULL-terminated list of bufinfo entries.
* The caller's Receive Done callback is invoked for each buffer of the message.
* HTC may split and aggregate notification for completed buffers, as long
* as each sent buffer is included in some buffer list sent to the Receive
* Done callback.
*
* This function queues a chain of empty buffer descriptors that can receive the data from the host.
* All buffers and the buffer descriptor memory must be provided by the application.
* The driver does not have any internal buffers.
*
*
*  @param[in] Instance  Instance id of HTC slave
*  @param[in] inst         Endpoint ID on which the transfer is to be done. 
*  @param[in] buf          Pointer to the head of the buffer descriptor list
*                          that is used when the data from the host is received and, on completion,
*                          the corresponding callback function is called with the specified Endpoint ID. \n
*                          See #qapi_HTC_bufinfo_t for a description of the structure.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t qapi_HTC_Slave_Receive_Data(uint32 Instance, qapi_HTCSlaveEndpointID_t inst,
                                       qapi_HTC_bufinfo_t *buf);

/**
* Sets the maximum message size that is sent/received.
* The message size = n*buffer size. It is greater than the buffer size.
*
*  @param[in] Instance  Instance id of HTC slave
*  @param[in] inst         Endpoint ID on which the transfer is to be done. 
*  @param[in] max_msgsz    Maximum message size.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/

qapi_Status_t qapi_HTC_Slave_Max_MsgSize_Set(uint32 Instance, qapi_HTCSlaveEndpointID_t inst,
                                        uint32 max_msgsz);

/**
 * Returns the maximum message size that is allowed to be received from the host
 * over a given mailbox. This value is negotiated with the host during
 * startup. The target side caller may use this during initialization in
 * order to choose an appropriate buffer size for each mailbox.
 *
 *  @param[in] Instance  Instance id of HTC slave
 *  @param[in] inst          Endpoint ID on which the transfer is to be done. 
 *  @param[out] max_msgsz    Get the maximum message size.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
 */
qapi_Status_t qapi_HTC_Slave_Max_MsgSize_Get(uint32 Instance, qapi_HTCSlaveEndpointID_t inst,
                                        uint32 *max_msgsz);

/**
 * Sets the slave buffer sizes.
 *
 * During initialization, the caller must specify the size of the buffers
 * to be used for receiving on each mailbox. This should be done before
 * an attempt is made to receive (before qapi_HTC_Slave_Receive_Data() is called).
 *
 *  @param[in] Instance  Instance id of HTC slave
 *  @param[in] inst         Endpoint ID on which the transfer has to be done. 
 *  @param[in] bufsz        Set the buffer size of the buffer supplied for receiving.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
 */
qapi_Status_t qapi_HTC_Slave_BufSize_Set(uint32 Instance, qapi_HTCSlaveEndpointID_t inst,
                                        uint32 bufsz);

/**
 * Pauses a receive (Rx) activity over a mailbox.
 *
 * While receives are paused, the target hardware may continue to
 * gather messages from the host, but they will not be reported to
 * the caller until the caller resumes the receive activity.
 *
 *  @param[in] Instance  Instance id of HTC slave
 *  @param[in] inst         Endpoint ID on which the transfer is to be done. 
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
 */
qapi_Status_t qapi_HTC_Slave_Recv_Pause (uint32 Instance, qapi_HTCSlaveEndpointID_t inst);

/**
 * Resumes receive (Rx) activity over a mailbox.
 *
 * While receives are paused, the target hardware may continue to
 * gather messages from the host, but they will not be reported to
 * the caller until the caller resumes the receive activity.
 *
 *  @param[in] Instance  Instance id of HTC slave
 *  @param[in] inst         Endpoint ID on which the transfer is to be done. 
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. 
 */
qapi_Status_t qapi_HTC_Slave_Recv_Resume (uint32 Instance, qapi_HTCSlaveEndpointID_t inst);

/**
 * Returns the maximum number of Endpoint endpoints supported/enabled by the HTC layer.
 *
 *  @param[in] Instance  Instance id of HTC slave
 *  @param[out] pNumEndpoint    Pointer to the number of Endpoint endpoints supported. The application
 *                          should use this API to determine the number of endpoints and
 *                          use it for IO.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. 
 */
qapi_Status_t qapi_HTC_Slave_Get_Num_Endpoints(uint32 Instance, uint8 *pNumEndpoint);


/**
 * Registers the application's handler for HTC Events.
 * This is common for all the endpoints. The endpoint id shall be passed as arg.
 *
 *  @param[in] Instance       Instance id of HTC slave
 *  @param[in] eventId        Event ID for which the handler is registered.
 *  @param[in] eventHandler   Application Event handler ID pointer.
 *  @param[in] param          Parameter which is passed event handler.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. 
 */

qapi_Status_t qapi_HTC_Event_Register (uint32 Instance, 
                                       qapi_HTCSlave_event_id_t eventId,
                                       qapi_HTC_callback_pfn_t eventHandler,
                                       void *param);
/**
 * De-Registers the application's handler for HTC Events.
 * This is common for all the endpoints. The endpoint id shall be passed as arg.
 *
 *  @param[in] Instance       Instance id of HTC slave
 *  @param[in] eventId        Event ID for which the handler is registered.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. 
 */

qapi_Status_t qapi_HTC_Event_Deregister (uint32 Instance,
                                         qapi_HTCSlave_event_id_t eventId);


/**
 * Get number of interfaces on which HTC slave can be made to communicate with host.
 *
 *  @param[out] eventId       Number of interfaces.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. 
 */

qapi_Status_t qapi_HTC_slave_get_num_interfaces (uint32 *pNumInterfaces);


/**
 * Get number of interfaces on which HTC slave can be made to communicate with host.
 *
 *  @param[out] Interface_array       Array of interface numbers which is returned by this QAPI.
 *  @param[in] pNumInterfaces       Number of interfaces.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. 
 */

qapi_Status_t qapi_HTC_slave_get_interface_ids ( uint32 *Interface_array,  uint32 pNumInterfaces);



/**
 * Dump the list of queued buffers for queued for receiving data from host.
 * This is used to mainly for debuggging.
 *
 *  @param[in] Instance                     Instance id of HTC slave
 *  @param[in] qapi_HTCSlaveEndpointID_t    Endpoint ID.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. 
 */

qapi_Status_t qapi_HTC_slave_dump_recv_queued_buffers(uint32 InstanceId,
                                                                              qapi_HTCSlaveEndpointID_t endpoint);


/**
 * Dump the list of queued buffers for queued for sending data to host.
 * This is used to mainly for debuggging.
 *
 *  @param[in] Instance                     Instance id of HTC slave
 *  @param[in] qapi_HTCSlaveEndpointID_t    Endpoint ID.
 *
 *  @return
 *  QAPI_OK                -- Call succeeded. \n
 *  QAPI_ERROR             -- Call failed. \n
 *  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. 
 */


qapi_Status_t qapi_HTC_slave_dump_send_queued_buffers(uint32 InstanceId,
                                                                                qapi_HTCSlaveEndpointID_t endpoint);



qapi_Status_t qapi_HTC_slave_tx_credit_inc (uint32 InstanceId, qapi_HTCSlaveEndpointID_t endpoint);


#endif /* __QAPI_HTC_SLAVE_H__ */

/** @} */ /* end_addtogroup qapi_htc_slave */
