#ifndef __QAPI_DSR_H__
#define __QAPI_DSR_H__
/*============================================================================
  @file dsr.h

  Function and data structure declarations for DSRs (Deferred Service
  Routines).


                Copyright (c) 2016 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.ioe/1.0/v2/rom/release/api/platform/qapi_dsr.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "com_dtypes.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
  QAPI_DSR_PRI_LOW,
  QAPI_DSR_PRI_MED,
  QAPI_DSR_PRI_HIGH,
  QAPI_DSR_NUM_PRIO
} qapi_dsr_priority_t;

//typedef struct dsr_obj_s dsr_obj_t;
typedef struct qapi_dsr_obj_s qapi_dsr_obj_t;
typedef void (*qapi_dsr_hdlr_t)(qapi_dsr_obj_t *dsr_obj, void *ctxt, void *payload);

/*
struct qapi_dsr_obj_s
{
  struct qapi_dsr_obj *next;
  qapi_dsr_hdlr_t cb;
  void *ctxt;
  void *payload;
  qapi_dsr_priority_t priority;
  boolean pending;
};
*/

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

/**
 * @brief Creates a DSR object
 *
 * @details This function creates a DSR object that can be enqueued later to
 *          defer work to thread context.
 *
 * @param[out] dsr_obj    DSR object
 * @param[in]  cb         Callback function invoked in DSR thread context
 * @param[in]  ctxt       Optional callback context; can be NULL
 * @param[in]  priority   DSR priority
 *
 * @return TRUE if successfully created; otherwise, FALSE
 */
qapi_Status_t qapi_dsr_create(qapi_dsr_obj_t **dsr_obj, qapi_dsr_hdlr_t cb, void *ctxt, qapi_dsr_priority_t priority);

/**
 * @brief Destroys a DSR object
 *
 * @details This function destroys a DSR object. If the DSR is currently
 *          queued then it will be removed from the queue (callback function
 *          will not get invoked).
 *
 * @param[in] dsr_obj    DSR object
 *
 * @return None
 */
qapi_Status_t qapi_dsr_destroy(qapi_dsr_obj_t *dsr_obj);

/**
 * @brief Enqueues a DSR object
 *
 * @details This function enqueues a DSR object into the DSR pending list.
 *          Worker threads process the pending list based on DSR priority.
 *
 *          Once a DSR is enqueued, it cannot be enqueued again (an object
 *          can only be inserted into a list once). In cases where drivers
 *          may attempt to enqueue a DSR already in the pending list, e.g.
 *          handling interrupts, DSR context should be maintained between
 *          the ISR and DSR.
 *
 *          Do not make blocking calls / wait on events in DSR context.
 *
 *          If this function returns FALSE then it was not enqueued. If
 *          the DSR is already in the pending list, this function will
 *          return FALSE and the payload will not be updated.
 *
 * @param[in] dsr_obj    DSR object
 * @param[in] payload    Optional payload context; can be NULL
 *
 * @return TRUE if successfully enqueued; otherwise, FALSE
 */
qapi_Status_t qapi_dsr_enqueue(qapi_dsr_obj_t *dsr_obj, void *payload);

#endif /* #ifndef __QAPI_DSR_H__ */

