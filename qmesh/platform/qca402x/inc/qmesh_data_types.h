/*=============================================================================
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/

/*****************************************************************************/ 
/*! \file qmesh_data_types.h
 *  \brief QMesh library data types
 *
 *   This file contains the different data types used in QMesh library
 *
 */
 /*****************************************************************************/

#ifndef __QMESH_DATA_TYPES_H__
#define __QMESH_DATA_TYPES_H__

/*! \addtogroup Core
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "qurt_mutex.h"
#include "qapi_timer.h"

typedef uint32_t uint24_t;

typedef qurt_mutex_t QMESH_MUTEX_T;

#define min(x, y)    (((x) < (y)) ? (x) : (y))

#ifndef TRUE
#define TRUE              1
#endif

#ifndef FALSE
#define FALSE             0
#endif


#define DEFINE_STATIC(VAR_TYPE, VAR_NAME, VAR_INIT_VAL) static VAR_TYPE VAR_NAME = VAR_INIT_VAL
#define DEFINE_STATIC_CONST(VAR_TYPE, VAR_NAME, VAR_INIT_VAL) static const VAR_TYPE VAR_NAME = VAR_INIT_VAL
#define DEFINE_EXTERN(VAR_TYPE, VAR_NAME, VAR_INIT_VAL) extern VAR_TYPE VAR_NAME = VAR_INIT_VAL

#define DSTATIC static
#define DEXTERN extern

#ifdef __cplusplus
}
#endif

/*!@} */
#endif /*__QMESH_DATA_TYPES_H__ */

