/*=============================================================================
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/******************************************************************************/
/*! \file qmesh_model_debug.h
 *  \brief Model Library debug defines
 *  
 *      This file contains the debug marco's and debug level used within the
 *      Mesh Server Model library.
 */
/*****************************************************************************/
#ifndef __QMESH_MODEL_DEBUG_H__
#define __QMESH_MODEL_DEBUG_H__
#include <stdarg.h>
#include <stdio.h>

/* Server Model Specific Masks */
#define DBUG_MODEL_MASK_GEN_ONOFF_SERVER     (1UL << 0)
#define DBUG_MODEL_MASK_GEN_LEVEL_SERVER     (1UL << 1)
#define DBUG_MODEL_MASK_GDTT_SERVER          (1UL << 2)
#define DBUG_MODEL_MASK_PWR_ONOFF_SERVER     (1UL << 3)
#define DBUG_MODEL_MASK_PWR_LEVEL_SERVER     (1UL << 4)
#define DBUG_MODEL_MASK_LIGHTNESS_SERVER     (1UL << 5)
#define DBUG_MODEL_MASK_DELAY_CACHE          (1UL << 6)
#define DBUG_MODEL_MASK_MODEL_COMMON         (1UL << 7)
#define DBUG_MODEL_MASK_VENDOR_MODEL         (1UL << 8)

#ifndef DBUG_MODEL_MASK
#define DBUG_MODEL_MASK ( /*DBUG_MODEL_MASK_GEN_ONOFF_SERVER |*/\
                          /*DBUG_MODEL_MASK_PWR_LEVEL_SERVER |*/\
                          /*DBUG_MODEL_MASK_GEN_LEVEL_SERVER |*/\
                          /*DBUG_MODEL_MASK_PWR_ONOFF_SERVER |*/\
                          /*DBUG_MODEL_MASK_DELAY_CACHE | */\
                          /*DBUG_MODEL_MASK_LIGHTNESS_SERVER |*/\
                          /*DBUG_MODEL_MASK_MODEL_COMMON | */\
                          /*DBUG_MODEL_MASK_POWERONOFF_SERVER | */\
                          /*DBUG_MODEL_MASK_GDTT_SERVER |*/ \
                          /*DBUG_MODEL_MASK_VENDOR_MODEL |*/ \
                                                   0)
#endif

void model_debug_log(const char *format, ...);

#define DEBUG_MODEL_PRINT(...) \
        do { \
            model_debug_log (__VA_ARGS__);\
        }while(0)

#define DEBUG_MODEL_PRINT_ARR_U8(m, x, n)  

#define DEBUG_MODEL_INFO( m, ...)\
        do {\
            if (m & DBUG_MODEL_MASK)\
                model_debug_log (__VA_ARGS__);\
        }while(0)

#define DEBUG_MODEL_TRACE(m, ...)\
        do {\
            if (m & DBUG_MODEL_MASK)\
                model_debug_log (__VA_ARGS__);\
        }while(0)

#define DEBUG_MODEL_ERROR(m, ...) \
        do {\
            if (m & DBUG_MODEL_MASK)\
                model_debug_log (__VA_ARGS__);\
        }while(0)
#endif /* __QMESH_MODEL_DEBUG_H__ */
