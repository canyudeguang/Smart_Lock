/*=============================================================================
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/

/******************************************************************************
 *  FILE
 *      qmesh_soft_timers.h
 *
 *  DESCRIPTION
 *      This file provides functionality to maintain a groups of virtual timers.
 *      1 system timer is used for one group. Max. no. of timers can be set
 *      using - MAX_VIRTUAL_TIMERS (64 default)
 *****************************************************************************/
#ifndef __QMESH_SOFT_TIMERS_H__
#define __QMESH_SOFT_TIMERS_H__

#include "qmesh_data_types.h"
#include "qmesh_result.h"

/******************************************************************************
 *Timer Configurations
 *****************************************************************************/
#define MAX_VIRTUAL_TIMERS           (64)   /* Total virtual timers supported */
#define MAX_TIMER_GROUPS             (4)    /* MAX hardware timers */

/*! \brief Handle for a timer group*/
typedef void * QMESH_TIMER_GROUP_HANDLE_T;

/*! \brief Handle for a timer*/
typedef void * QMESH_TIMER_HANDLE_T;

/*! \brief Definition of the Soft Timer callback method*/
typedef void ( *QMESH_TIMER_CALLBACK_T)(QMESH_TIMER_HANDLE_T, void * params);

/*! Indicates that a timer handle is invalid and not in use */
#define QMESH_TIMER_INVALID_HANDLE  (0)

#if defined (PLATFORM_QUARTZ)
#define QMESH_TIMER_INVALID_GROUP_HANDLE   (QMESH_TIMER_GROUP_HANDLE_T)(0xFF)
#else
#define QMESH_TIMER_INVALID_GROUP_HANDLE   (NULL) /* Invalid Timer group ID*/
#endif /*defined (PLATFORM_QUARTZ)*/


/******************************************************************************
 * APIs
 *****************************************************************************/
/*----------------------------------------------------------------------------*
 * QmeshInitializeTimerContext
 */
/*! \brief This function initializes the timer context. This needs to be called
 *         before creating any timer groups.
 *
 *  \returns \ref QMESH_RESULT_T
 */
/*---------------------------------------------------------------------------*/
QMESH_RESULT_T QmeshInitializeTimerContext (void);

/*----------------------------------------------------------------------------*
 * QmeshDeinitializeTimerContext
 */
/*! \brief This function removes all timer groups and timers.
 *
 *  \returns void
 */
/*---------------------------------------------------------------------------*/
void QmeshDeinitializeTimerContext (void);

/*----------------------------------------------------------------------------*
 * QmeshTimerCreateGroup
 */
/*! \brief This function creates a timer group. Each timer groups uses a single
 *         system timer for all its soft timers.
 *
 *  \param[in] no_of_timers Number of timers to be reserved for this group.
 *
 *  \returns \ref QMESH_TIMER_GROUP_HANDLE_T
 */
/*---------------------------------------------------------------------------*/
QMESH_TIMER_GROUP_HANDLE_T QmeshTimerCreateGroup (uint16_t no_of_timers);

/*----------------------------------------------------------------------------*
 * QmeshTimerDeleteGroup
 */
/*! \brief This function deletes a timer group and remove all the running soft
 *         timers in this group.
 *
 *  \param[in, out] ghdl is Pointer to the group handle.
 *
 *  \returns void
 */
/*---------------------------------------------------------------------------*/
void QmeshTimerDeleteGroup (QMESH_TIMER_GROUP_HANDLE_T *ghdl);

/*----------------------------------------------------------------------------*
 * QmeshTimerCreate
 */
/*! \brief This function creates a soft timers for a group.
 *
 *  \param[in] ghdl is pointer to the group handle.
 *  \param[in] timerCb is the callback handler.
 *  \param[in] timerParams is the pointer to callback data.
 *  \param[in] timeInMs is the duration in milliseconds.
 *
 *  \returns \ref QMESH_TIMER_HANDLE_T
 */
/*---------------------------------------------------------------------------*/
QMESH_TIMER_HANDLE_T QmeshTimerCreate (const QMESH_TIMER_GROUP_HANDLE_T *ghdl,
                                       QMESH_TIMER_CALLBACK_T timerCb,
                                       void *timerParams,
                                       uint32_t timeInMs);

/*----------------------------------------------------------------------------*
 * QmeshTimerDelete
 */
/*! \brief This function deletes a soft timer
 *
 *  \param[in] ghdl is pointer to the group handle.
 *  \param[in, out] thdl is the pointer to the timer handle.
 *
 *  \returns \ref QMESH_RESULT_T
 */
/*---------------------------------------------------------------------------*/
QMESH_RESULT_T QmeshTimerDelete (const QMESH_TIMER_GROUP_HANDLE_T *ghdl,
                                 QMESH_TIMER_HANDLE_T *thdl);

/*----------------------------------------------------------------------------*
 * QmeshTimerGetRemainingTime
 */
/*! \brief This function returns the time remaining for a timer's expiry.
 *
 *  \param[in] ghdl is pointer to the group handle.
 *  \param[in] thdl is the pointer to the timer handle.
 *  \param[out] remainingTimeInMs is pointer to remaining time in milliseconds.
 *
 *  \returns \ref QMESH_RESULT_T
 */
/*---------------------------------------------------------------------------*/
QMESH_RESULT_T QmeshTimerGetRemainingTime (const QMESH_TIMER_GROUP_HANDLE_T *ghdl,
                                           const QMESH_TIMER_HANDLE_T *thdl,
                                           uint32_t *remainingTimeInMs);
#endif
