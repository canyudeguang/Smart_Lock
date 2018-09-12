#ifndef __HTC_SLAVE_API_H__
#define __HTC_SLAVE_API_H__

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
 * @file  htc_slave_api.h
 * @brief HTC Slave APIs
 * Copyright (c) 2017
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 */

/*====================================================================

                              EDIT HISTORY 

  $Header: 

====================================================================*/


/*
 * Structure for use by HTC to track buffers, and for caller and
 * HTC to communicate about buffers.  The caller passes in a
 * pointer to one of these on every send and every receive.  There
 * must be a unique HTC_BUFFER for each buffer that is passed
 * in.  HTC owns the HTC_BUFFER from the time it is passed in
 * until the time a send/receive completion is indicated, and the
 * caller must not modify it during that time.
 */

typedef struct HTC_bufinfo_s {
    struct HTC_bufinfo_s  *next;          /* HTC_BUFFER list linkage field,
                                            used by HTC when HTC owns bufinfo,
                                            used by caller when caller owns
                                            bufinfo */
    uint8                 *buffer;        /* Pointer to data buffer.
                                            Not modified by HTC. */
    uint16                actual_length; /* In Bytes.
                                            In recv_done, indicates valid
                                            bytes in buf; filled in by HTC.
                                            Maximum: set buffer size for mbox.
                                            In send, indicates to HTC the
                                            number of bytes to send to the Host;
                                            filled in by caller.*/
    uint8               htc_flags;      /* htc flags (used by HTC layer only) */
    uint8               end_point;      /* endpoint that this buffer belongs (used by HTC layer) */
    uint16              buffer_offset;  /* Buffer offset from the reserved headroom for the HTC header
                                             If the caller can shift the start of the payload such that
                                             the buffer can no longer satisfy the TrailerSpcCheckLimit
                                             set by the service, the caller can specify an offset.
                                             The offset is the distance of the current position and the
                                             area reserved for the HTC headroom.
                                             This value is zeroed by HTC when the buffer is processed.
                                             Applications that do not shift the buffer position can
                                             leave this value 0 */
    uint16              app_context;    /* application context for buffer */
} HTC_BUFFER;

/* Function status codes. */
typedef enum
{
    HTC_SLAVE_SUCCESS = 0,
    HTC_SLAVE_ERROR_INVALID_PARAMETER,
    HTC_SLAVE_ERROR_ALLOCATION,
    HTC_SLAVE_ENDPOINT_ERROR
} HTC_SLAVE_STATUS;


/* HTC Slave Mobx HW ID */
typedef enum
{
    HTC_SLAVE_ENDPOINT_ID0 = 0,
    HTC_SLAVE_ENDPOINT_ID1 = 1,
    HTC_SLAVE_ENDPOINT_ID2 = 2,
    HTC_SLAVE_ENDPOINT_ID3 = 3,
    HTC_SLAVE_MAX_ENDPOINTS
} HTC_SLAVE_ENDPOINT_ID;

 /* HTC Slave Event ID */
 typedef enum
 {
     HTC_SLAVE_BUFFER_RECEIVED,
     HTC_SLAVE_BUFFER_SENT,
     HTC_SLAVE_INIT_COMPLETE,
     HTC_SLAVE_RESET_COMPLETE,     
     HTC_SLAVE_MAX_EVENTS
 } HTC_SLAVE_EVENT_ID;

 /* HTC instance IDs */
 typedef enum {
     HTC_SDIO_SLAVE_INSTANCE_ID = 0,   /* default host interface mailbox */
     HTC_SPI_SLAVE_INSTANCE_ID,
     HTC_MAX_INSTANCES
 } HTC_INSTANCE_IDS;

 /*
 * Structure for use by HTC to track buffers, and for caller and
 * HTC to communicate about buffers.  The caller passes in a
 * pointer to one of these on every send and every receive.  There
 * must be a unique HTC_bufinfo_s for each buffer that is passed
 * in.  HTC owns the HTC_bufinfo_s from the time it is passed in
 * until the time a send/receive completion is indicated, and the
 * caller must not modify it during that time.
 */
typedef HTC_BUFFER HTC_SLAVE_BUFFER;

typedef void (*htc_callback_pfn_t)(uint32 Instance, uint32 arg1, uint32 arg2);


#define NUM_BUFFERS_PER_ENDPOINT    8
#endif /* __HTC_SLAVE_API_H__ */
