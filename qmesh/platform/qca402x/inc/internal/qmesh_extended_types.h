/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*
 *  FILE
 *      qmesh_extended_types.h
 *
 *  DESCRIPTION
 *      This file provides type definitions for QURT based HAL interface.
 *      - All types used by HAL layer are defined here.
 *      - platform specific headers should be restricted to this file only.
 *****************************************************************************/
#ifndef __QMESH_EXTENDED_TYPES_H__
#define __QMESH_EXTENDED_TYPES_H__

/*============================================================================*
 *  Standard Header Files
 *===========================================================================*/
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/
#include "qmesh_data_types.h"

/*============================================================================*
 *  Platform Header Files
 *===========================================================================*/
#include "qurt_timer.h"
#include "qapi_timer.h"
#include "qurt_types.h"
#include "qurt_error.h"
#include "qurt_mutex.h"
#include "qurt_thread.h"
#include "qurt_signal.h"
#include "qurt_pipe.h"
#include <qapi/qapi_crypto.h>
#include <qapi/qapi_status.h>
#include <qapi/qapi_fs.h>
//#include "CeML.h"

/*
 *Basic Types
 */
typedef uint8_t QUint8;
typedef uint16_t QUint16;
typedef uint32_t QUint24;
typedef uint32_t QUint32;
typedef uint64_t   QUlong;
/* Signed fixed width types */
typedef int8_t QInt8;
typedef int16_t QInt16;
typedef int32_t QInt32;

/* String types */
typedef char QCharString;
typedef uint16_t QSize;
typedef uint8_t QBool;

/*============================================================================*
 *  Timers
 *===========================================================================*/
/* Timer data i.e. QMESH_TIMER_PARAMS_T is currently defined as void *. So
 * just check for NULL
 */
#define TIMER_DATA_IS_VALID(x) ((x!=NULL)?1:0)

typedef qapi_TIMER_handle_t QMESH_HW_TIMER_HANDLE_T;
typedef void ( *QMESH_TIMERCB_FN)( void * );
typedef void * QMESH_STACKTIMERCB_FN;
typedef void * QMESH_TIMER_PARAMS_T;
/*
 *typedef void ( *QMESH_TIMERCB_FN)( QMESH_HW_TIMER_HANDLE_T handle );
 */
typedef struct {
    QMESH_STACKTIMERCB_FN cb; /* TODO we should be able to cast this to any
                              other function type */
    QMESH_TIMER_PARAMS_T params;
    QUint32 index;
    uint32_t  handle_cnt;
    uint64_t  expiry_ts;
    QMESH_HW_TIMER_HANDLE_T tid;
} QMESH_TIMERINFO_T;




/*============================================================================*
 *  Mutex
 *===========================================================================*/

/*============================================================================*
 *  Threads
 *===========================================================================*/
/*
 *TODO Thread properties set here needs to be finalized.
 *especially the stack size
 */
#define QMESH_THREAD_DEF_PRIORITY   16
#define QMESH_THREAD_DEF_STACK_SIZE 1024
typedef void (*QMESH_THREAD_ENTRY_T)(void *);
typedef void * QmeshThreadAttrib_t;
typedef qurt_thread_t QMESH_THREADID_T;

/*============================================================================*
 *  Filesystem
 *===========================================================================*/
typedef int QMESH_FILE_HDL_T;
#define QMESH_FS_OP_READ_ONLY   QAPI_FS_O_RDONLY
#define QMESH_FS_OP_READ_WRITE  QAPI_FS_O_RDWR
#define QMESH_FS_OP_CREATE      QAPI_FS_O_CREAT
#define QMESH_FS_OP_TRUNC       QAPI_FS_O_TRUNC

/*============================================================================*
 *  Pipes
 *===========================================================================*/
typedef qurt_pipe_t QMESH_PIPE_T;
/*============================================================================*
 *  Conditions
 *===========================================================================*/
typedef qurt_signal_t QMESH_CONDITION_T;
typedef QMESH_HW_TIMER_HANDLE_T QmeshTimerHandle;
typedef QMESH_MUTEX_T CsrMutex;
#endif
