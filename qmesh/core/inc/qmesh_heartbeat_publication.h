/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_heartbeat_publication.h
 *  \brief This file has prototypes and defines for Heartbeat functionality
 */
 /*****************************************************************************/

#ifndef __QMESH_HEARTBEAT_PUBLICATION_H__
#define __QMESH_HEARTBEAT_PUBLICATION_H__

#include "qmesh_types.h"
#include "qmesh_hal_ifce.h"
#include "qmesh_private.h"


extern void QmeshHeartBeatTriggerStateChange(uint8_t prov_net_id, QMESH_DEVICE_FEATURE_MASK_T feature_mask);
extern void QmeshHeartBeatStopPublication(uint8_t prov_net_id, QMESH_HEARTBEAT_PUBLICATION_INFO_T *pub_state);
extern void QmeshHeartBeatSetPublish(uint8_t net_idx, QMESH_HEARTBEAT_PUBLICATION_INFO_T *pub_info,
                                     const QMESH_HEARTBEAT_PUBLICATION_STATE_T *pub_state);

#endif  /* #ifndef __QMESH_HEARTBEAT_PUBLICATION_H__ */

