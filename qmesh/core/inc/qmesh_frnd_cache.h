/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_frnd_cache.h
 *  \brief defines and functions for qmesh_frnd_cache.h
 *
 *   This file contains the implementation of transport layer functionality. The
 *   transport layer uses AES-CCM to encrypt and authenticate application data. It
 *   also defines how application messages are segmented and reassembled into multi-
 *   ple transport PDUs.
 */
 /*****************************************************************************/

#ifndef __QMESH_FRIEND_CACHE_H__
#define __QMESH_FRIEND_CACHE_H__

#include "qmesh_frnd_common.h"
#include "qmesh_network.h"
#include "qmesh_lower_transport.h"

extern QMESH_FRND_INFO_T *g_p_frnd_info;

extern QmeshResult ReadMessageFromQueue(uint8_t lpn_idx,
                                 QMESH_NETWORK_HEADER_T *nw_hdr,
                                 uint8_t **trans_pdu,
                                 uint8_t *trans_pdu_len,
                                 more_data_field_t *md,
                                 uint8_t *flag,
                                 uint32_t *iv_idx);
extern void RemoveOldMsg(uint8_t lpn_idx);

extern QMESH_RESULT_T FrndCacheWriteUpdateMsg(QMESH_SUBNET_KEY_IDX_INFO_T key_idx);
extern void FrndCacheDeleteMsgsForAddr(uint8_t lpn_idx, uint16_t addr);

extern void QmeshFriendClearLpnCache(uint8_t lpn_index);

extern QMESH_RESULT_T QmeshFriendLowerTransportRXPDU(const QMESH_BEARER_HEADER_T *bearer_hdr, const QMESH_NETWORK_INFO_T *nw_info,
                                        const QMESH_LT_PDU_LIST_T *lt_pdu_list);

extern bool AreFreeSlotsAvailableInQueue(uint8_t lpn_idx, uint8_t num_segments);
extern QMESH_RESULT_T MakeRoomForReceivedSlotsInQueue(uint8_t lpn_idx, uint8_t num_segments);
extern bool IsItAnAckMessage(QMESH_LT_PDU_T *lt_pdu);
extern void AddReceivedMessageToQueue(uint8_t lpn_idx, const QMESH_NETWORK_INFO_T *nw_info,
                                        const QMESH_LT_PDU_LIST_T *lt_pdu_list);
extern bool IsQueueEmpty(uint8_t lpn_idx);

#endif /* __QMESH_FRIEND_CACHE_H__ */

