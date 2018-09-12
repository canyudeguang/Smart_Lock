/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_qca402x_sched_internal.h
 *  \brief Qmesh library functions for sending messages to scheduler
 */
 /******************************************************************************/

#ifndef __QMESH_QCA402X_SCHED_Q_H__
#define __QMESH_QCA402X_SCHED_Q_H__

#include "qmesh_data_types.h"
#include "qmesh_result.h"
#include "qmesh_bearer.h"
#include "qmesh_soft_timers.h"

/*Event masks handled by the scheduler thread*/
#define QMESH_SCHED_EVENT_TIMER     (0x01)    /*mask for timer event*/
#define QMESH_SCHED_EVENT_TX        (0x02)    /*mask for tx event*/
#define QMESH_SCHED_EVENT_RX        (0x04)    /*mask for rx event*/
#define QMESH_SCHED_EVENT_GATT_RX   (0x08)    /*mask for gatt rx event*/
#define QMESH_SCHED_EVENT_EXIT      (0x10)    /*mask to exit event therad*/
#define QMESH_SCHED_EVENT_ALL       (QMESH_SCHED_EVENT_TIMER|\
                                     QMESH_SCHED_EVENT_TX|\
                                     QMESH_SCHED_EVENT_RX|\
                                     QMESH_SCHED_EVENT_GATT_RX|\
                                     QMESH_SCHED_EVENT_EXIT)

/*! \addtogroup Scheduler
 * @{
 */

/*----------------------------------------------------------------------------*
 * QmeshSchedAddMsgToTxQueue
 */
/*! \brief This API is used to send a message over the LE Advert bearer on priority.
 *
 *  NOTE: Currently unsupported on QCA402x. It sends message with same priority.
 *
 * \param msg_param Advert message parameter. See \ref QMESH_BEARER_MSG_PARAM_T.
 * \param msg       Pointer the payload.
 * \param msg_len   Length of the payload in octets.
 *
 * \returns QMESH_RESULT_T.  Refer to \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
QMESH_RESULT_T QmeshSchedAddMsgToTxQueue(const QMESH_ADV_BEARER_MSG_PARAM_T *msg_param, 
                                         const uint8_t *msg, uint8_t msg_len);

/*----------------------------------------------------------------------------*
 * QmeshQueueTimerEvent
 */
/*! \brief Queue the Timer event in Timer Event Pipe for scheduler thread to 
 *      handle.
 *  \returns see \ref QMESH_RESULT_T
 *           -1 : Failed to queue the event.
 *            0 : Successfully queued the event. 
 */
/*---------------------------------------------------------------------------*/
int QmeshQueueTimerEvent (QMESH_TIMER_HANDLE_T timer_handle,
                          void * callback,
                          void * params);
						  
/*!@} */

#endif /*__QMESH_QCA402X_SCHED_H__*/

