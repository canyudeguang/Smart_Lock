/*=============================================================================
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
 /*! \file qmesh_friend_lpn_common.h
  *  \brief Qmesh API
  *
  *   This file contains the API for Qmesh Low Power Node feature.
  *
  */
/******************************************************************************/

#ifndef __QMESH_FRIEND_LPN_COMMON_H__
#define __QMESH_FRIEND_LPN_COMMON_H__

#include "qmesh_types.h"

#ifdef __cplusplus
 extern "C" {
#endif

/*! \brief Friendship keys */
typedef struct
{
    uint8_t frnd_nid;              /*!< Friend nid. */
    QMESH_SEC_KEY_T frnd_priv_key; /*!< Friend privacy key. */
    QMESH_SEC_KEY_T frnd_encp_key; /*!< Friend encryption key. */
} FRND_KEY_INFO_T;

/*! \addtogroup Core_Friendship
 * @{
 */

/*! \brief Friendship Criteria */
typedef struct
{
    uint8_t rssi_factor;           /*!< Contribution of rssi factor. */
    uint8_t recv_window_factor;    /*!< Contribution of receive window. */
    uint8_t min_cache_size;        /*!< Minimum number of messages friend node can store in its friend queue. */
} QMESH_FRND_CRITERIA_T;
/*============================================================================*
    Public Function Implementations
 *============================================================================*/

/*! @} */

#ifdef __cplusplus
 }
#endif
#endif /* __QMESH_FRIEND_LPN_COMMON_H__ */

