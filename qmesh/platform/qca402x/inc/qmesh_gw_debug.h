/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_gw_debug.h
 *  \brief Qmesh library debug macros
 *   This file contains the debug marco's and debug level used within the 
 *   QMesh application and platform code.
 */
 /******************************************************************************/
#ifndef __QMESH_GW_DEBUG_H__
#define __QMESH_GW_DEBUG_H__

#include <stdarg.h>
#include <stdio.h>


/* Gateway Specific Masks */
#define DBUG_GW_MASK_BRER         (1UL << 3)
#define DEBUG_GW_MASK_PS          (1UL << 5)
#define DEBUG_MASK_SOFTTIMER      (1UL << 6)
#define DBUG_GW_MASK_SERVICE      (1UL << 7)
#define DBUG_GW_MASK_OSAL         (1UL << 8)
#define DBUG_GW_MASK_SCHED        (1UL << 9)
#define DBUG_ARR_MAX_LEN          (256)

#define DBUG_GW_MASK (DBUG_GW_MASK_OSAL         | \
                      /* DBUG_GW_MASK_BRER      | */ \
                      /* DBUG_GW_MASK_SCHED     | */ \
                      0)

#define DEBUG_GW_ENABLE

#ifdef _QMESH_ASSERT_ENABLE_
#define QMESHASSERT assert 
#else
#define QMESHASSERT(x)
#endif

#define DEBUG_ASSERT(...)\
    { \
        DEBUG_PRINT ("ASSERT : ");\
        DEBUG_PRINT(__VA_ARGS__);\
        QMESHASSERT(0);\
    }

#ifdef DEBUG_GW_ENABLE
void gw_debug_log(const char *format, ...);
#define DEBUG_PRINT(...) \
    do { \
        gw_debug_log (__VA_ARGS__);\
    }while(0)
#define DEBUG_PRINT_ARR_U8(m, x, n)  
#define DEBUG_INFO( m, ...)\
    do {\
        if (m & DBUG_GW_MASK)\
            gw_debug_log (__VA_ARGS__);\
    }while(0)
#define DEBUG_TRACE(m, ...)\
    do {\
        if (m & DBUG_GW_MASK)\
            gw_debug_log (__VA_ARGS__);\
    }while(0)
#define DEBUG_ERROR(m, ...) \
    do {\
        if (m & DBUG_GW_MASK)\
            gw_debug_log (__VA_ARGS__);\
    }while(0)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINT_ARR_U8(m, x, n)
#define DEBUG_INFO( m, ...)
#define DEBUG_TRACE(m, ...)
#define DEBUG_ERROR(m, ...)
#endif /* DEBUG_GW_ENABLE */

typedef void (*DEBUGFUNC)(void * handle, const char *format, ...);
void  QmeshDebugHandleSet (DEBUGFUNC func, void * handle);
#endif /* __QMESH_GW_DEBUG_H__ */

