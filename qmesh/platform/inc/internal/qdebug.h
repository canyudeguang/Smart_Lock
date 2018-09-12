/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qdebug.h
 *  \brief Library debug defines
 *
 *   This file contains the debug marco's and debug level used within the
 *   QMesh library
 */
 /******************************************************************************/

#ifndef __QDEBUG_H__
#define __QDEBUG_H__

#include "qmesh_debug.h"

/* Debug module masks */
#define DEBUG_MASK_SCHEDULER            (1UL << 0)
#define DEBUG_MASK_BEARER               (1UL << 1)
#define DEBUG_MASK_NETWORK              (1UL << 2)
#define DEBUG_MASK_LOWER_TRANSPORT      (1UL << 3)
#define DEBUG_MASK_UPPER_TRANSPORT      (1UL << 4)
#define DEBUG_MASK_ACCESS               (1UL << 5)
#define DEBUG_MASK_PROVISIONING_LAYER   (1UL << 6)
#define DEBUG_MASK_DATABASE             (1UL << 7)
#define DEBUG_MASK_CONFIG_MODEL         (1UL << 8)
#define DEBUG_MASK_APP_INTERFACE        (1UL << 9)
#define DEBUG_MASK_CRYPTO               (1UL << 10)
#define DEBUG_MASK_BEACON               (1UL << 11)
#define DEBUG_MASK_FRND                 (1UL << 12)
#define DEBUG_MASK_HEALTH_MODEL         (1UL << 13)
#define DEBUG_MASK_HEART_BEAT           (1UL << 14)
#define DEBUG_MASK_LIST                 (1UL << 15)
#define DEBUG_MASK_PROXY                (1UL << 16)
#define DEBUG_MASK_SECTOOLS             (1UL << 17)

#define QMESH_DBG_MODULE_MASK           (/* DEBUG_MASK_SCHEDULER          | */ \
                                         /* DEBUG_MASK_BEARER             | */ \
                                         /* DEBUG_MASK_NETWORK            | */ \
                                         /* DEBUG_MASK_LOWER_TRANSPORT    | */ \
                                         /* DEBUG_MASK_UPPER_TRANSPORT    | */ \
                                         /* DEBUG_MASK_ACCESS             | */ \
                                         /* DEBUG_MASK_PROVISIONING_LAYER | */ \
                                         /* DEBUG_MASK_DATABASE           | */ \
                                         /* DEBUG_MASK_CONFIG_MODEL       | */ \
                                         /* DEBUG_MASK_APP_INTERFACE      | */ \
                                         /* DEBUG_MASK_CRYPTO             | */ \
                                         /* DEBUG_MASK_BEACON             | */ \
                                         /* DEBUG_MASK_FRND               | */ \
                                         /* DEBUG_MASK_HEALTH_MODEL       | */ \
                                         /* DEBUG_MASK_HEART_BEAT         | */ \
                                         /* DEBUG_MASK_LIST               | */ \
                                         /* DEBUG_MASK_SECTOOLS           | */ \
                                         /* DEBUG_MASK_PROXY              | */ \
                                         0)

/* All stack files including this file should define
 * the QMESH_DBG_MODULE to one of the corresponding layers
 * defined above
 */
#ifndef QMESH_DBG_MODULE
#define QMESH_DBG_MODULE 0
#endif

/* Define Debug Levels */
#define DBG_VERBOSE_LEVEL             (4) /*!< \brief Enables extensive logs  */
#define DBG_INFO_LEVEL                (3) /*!< \brief Enables Info + Test + Error messages */
#define DBG_TEST_LEVEL                (2) /*!< \brief Enables        Test + Error messages */
#define DBG_ERROR_LEVEL               (1) /*!< \brief Enables               Error messages */

/*! \brief Stack debug log level setting  */
#define QMESH_DBG_LEVEL               DBG_INFO_LEVEL

#if ((QMESH_DBG_MODULE & QMESH_DBG_MODULE_MASK) != 0)

#ifndef QMESH_DBG_EN
#define QMESH_DBG_EN 1
#endif

#else  /* (QMESH_DBG_MODULE & QMESH_DBG_MODULE_MASK) != 0) */

#ifdef QMESH_DBG_EN
#undef QMESH_DBG_EN
#endif

#endif /* (QMESH_DBG_MODULE & QMESH_DBG_MODULE_MASK) != 0) */

/*---------------------------------------------------------------------------*
 * DEBUG macro definitions
 *--------------------------------------------------------------------------*/

#ifdef QMESH_DBG_EN

    /* Debug Level - INFO */
    #if (QMESH_DBG_LEVEL >= DBG_INFO_LEVEL)
    #define DEBUG_INFO_STR(s)              DEBUG_ERROR_STR(s)
    #define DEBUG_INFO_U32(u)              DEBUG_ERROR_U32(u)
    #define DEBUG_INFO_U16(u)              DEBUG_ERROR_U16(u)
    #define DEBUG_INFO_U8(u)               DEBUG_ERROR_U8(u)
    #define DEBUG_INFO_U8_ARR(x,offset,n)  DEBUG_ERROR_U8_ARR(x,offset,n)
    #define DEBUG_INFO_U16_ARR(x,offset,n) DEBUG_ERROR_U16_ARR(x,offset,n)
    #else
    #define DEBUG_INFO_STR(s)
    #define DEBUG_INFO_U32(u)
    #define DEBUG_INFO_U16(u)
    #define DEBUG_INFO_U8(u)
    #define DEBUG_INFO_U8_ARR(x,offset,n)
    #define DEBUG_INFO_U16_ARR(x,offset,n)
    #endif /* DEBUG_LEVEL_INFO */

    /* Debug Level - TEST */
    #if (QMESH_DBG_LEVEL >= DBG_TEST_LEVEL)
    #define DEBUG_TEST_STR     DEBUG_ERROR_STR
    #define DEBUG_TEST_U32     DEBUG_ERROR_U32
    #define DEBUG_TEST_U16     DEBUG_ERROR_U16
    #define DEBUG_TEST_U8      DEBUG_ERROR_U8
    #define DEBUG_TEST_U8_ARR  DEBUG_ERROR_U8_ARR
    #define DEBUG_TEST_U16_ARR DEBUG_ERROR_U16_ARR
    #else
    #define DEBUG_TEST_STR(s)
    #define DEBUG_TEST_U32(u)
    #define DEBUG_TEST_U16(u)
    #define DEBUG_TEST_U8(u)
    #define DEBUG_TEST_U8_ARR(x,offset,n)
    #define DEBUG_TEST_U16_ARR(x,offset,n)
    #endif /* >= DEBUG_TEST_LEVEL */

    /* Debug Level - ERROR */
    #if (QMESH_DBG_LEVEL >= DBG_ERROR_LEVEL)
    #define DEBUG_ERROR_STR(s) QmeshDebugWriteString(s)
    #define DEBUG_ERROR_U32(u) QmeshDebugWriteUint32(u)
    #define DEBUG_ERROR_U16(u) QmeshDebugWriteUint16(u)
    #define DEBUG_ERROR_U8(u) QmeshDebugWriteUint8(u)
    #define DEBUG_ERROR_U8_ARR(x,offset,n) QmeshDebugWriteUint8Arr(x,offset,n)
    #define DEBUG_ERROR_U16_ARR(x,offset,n) QmeshDebugWriteUint16Arr(x,offset,n)

    #else
    #define DEBUG_ERROR_STR(s)
    #define DEBUG_ERROR_U32(u)
    #define DEBUG_ERROR_U16(u)
    #define DEBUG_ERROR_U8(u)
    #define DEBUG_ERROR_U8_ARR(x,offset,n)
    #define DEBUG_ERROR_U16_ARR(x,offset,n)
    #endif /* >= DEBUG_ERROR_LEVEL */

#else /* DEBUG_ENABLE */

    #define DEBUG_TEST_STR(s)
    #define DEBUG_TEST_U32(u)
    #define DEBUG_TEST_U16(u)
    #define DEBUG_TEST_U8(u)
    #define DEBUG_TEST_U8_ARR(x,offset,n)
    #define DEBUG_TEST_U16_ARR(x,offset,n)
    #define DEBUG_INFO_STR(s)
    #define DEBUG_INFO_U32(u)
    #define DEBUG_INFO_U16(u)
    #define DEBUG_INFO_U8(u)
    #define DEBUG_INFO_U8_ARR(x,offset,n)
    #define DEBUG_INFO_U16_ARR(x,offset,n)
    #define DEBUG_ERROR_STR(s)
    #define DEBUG_ERROR_U32(u)
    #define DEBUG_ERROR_U16(u)
    #define DEBUG_ERROR_U8(u)
    #define DEBUG_ERROR_U8_ARR(x,offset,n)
    #define DEBUG_ERROR_U16_ARR(x,offset,n)
    #define DEBUG_CRITICAL_STR(s)
    #define DEBUG_CRITICAL_U32(u)
    #define DEBUG_CRITICAL_U16(u)
    #define DEBUG_CRITICAL_U8(u)
    #define DEBUG_CRITICAL_U8_ARR(x,offset,n)
    #define DEBUG_CRITICAL_U16_ARR(x,offset,n)

#endif /* QMESH_DBG_EN */

#endif /* __QDEBUG_H__ */

