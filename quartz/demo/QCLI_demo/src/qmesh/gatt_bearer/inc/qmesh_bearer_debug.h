/*=============================================================================
 Copyright (c) 2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_bearer_debug.h
 *  \brief Qmesh library debug macros
 *   This file contains the debug marco's and debug level used within the 
 *   QMesh application and platform code.
 */
 /******************************************************************************/
#ifndef __QMESH_BEARER_DEBUG_H__
#define __QMESH_BEARER_DEBUG_H__

#include "qmesh_demo_config.h"
#include "qmesh_demo_debug.h"

#ifdef DEBUG_GAP_GATT_BEARER
/* 
 * To enable GAP/GATT bearer module logging please enable macros DEBUG_GAP_GATT_BEARER
 * and DEBUG in qmesh_core_config.h
 */
#define DEBUG_BEARER_INFO QCLI_LOGI

#else

#define DEBUG_BEARER_INFO( m, ...)

#endif /* DEBUG_GAP_GATT_BEARER */

#define DEBUG_BEARER_ERROR QCLI_LOGE

#endif /* __QMESH_BEARER_DEBUG_H__ */

