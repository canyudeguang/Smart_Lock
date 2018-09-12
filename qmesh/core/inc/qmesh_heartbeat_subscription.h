/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_heartbeat_subscription.h
 *  \brief This file has prototypes and defines for Heartbeat functionality
 */
 /*****************************************************************************/

#ifndef __QMESH_HEARTBEAT_SUBSCRIPTION_H__
#define __QMESH_HEARTBEAT_SUBSCRIPTION_H__

#include "qmesh_types.h"
#include "qmesh_hal_ifce.h"
#include "qmesh_network.h"


extern void QmeshHeartBeatHandleMessage(const QMESH_NETWORK_HEADER_T *nw_hdr, const uint8_t *hb_pdu, uint16_t hb_pdu_len);

extern void QmeshHeartBeatSetSubscription(uint8_t net_idx, QMESH_HEARTBEAT_SUBSCRIPTION_INFO_T *hb_sub_info,
                                          uint16_t src, uint16_t dst, uint8_t period_log);


#endif  /* #ifndef __QMESH_HEARTBEAT_SUBSCRIPTION_H__ */

