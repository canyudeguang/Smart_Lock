/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_frnd.h
 *  \brief defines and functions for qmesh_frnd.h
 *
 *   This file contains the prototype for friend functionality
 */
 /*****************************************************************************/

#ifndef __QMESH_FRND_H__
#define __QMESH_FRND_H__

#include "qmesh_friendship.h"
#include "qmesh_private.h"

/* Information of friend feature */
typedef struct
{
    uint8_t                 recv_window;
    uint16_t                frnd_counter;
    uint8_t                 num_lpn; /* Max num of lpns supported */
    QMESH_LPN_INFO_T        *lpn_info; /* LPN information for all the LPNs */
} QMESH_FRND_INFO_T;

#ifdef PLATFORM_MULTITHREAD_SUPPORT
extern QMESH_MUTEX_T             frnd_mutex;

#define QMESH_FRND_LOCK()             do { QmeshMutexLock(&frnd_mutex); } while (0)
#define QMESH_FRND_UNLOCK()           do { QmeshMutexUnlock(&frnd_mutex); } while(0)
#else
#define QMESH_FRND_LOCK()
#define QMESH_FRND_UNLOCK()
#endif

extern QMESH_RESULT_T mGetLpnIndexFromAddressAndKeyIndex(uint16_t lpn_addr,
                        QMESH_SUBNET_KEY_IDX_INFO_T key_idx, uint8_t *lpn_idx);
extern bool mGetLPNSubAddressIndex(uint16_t sub_addr, uint16_t *sub_list,
                                      uint8_t len_sub_list, uint8_t *entry_idx);

extern uint32_t QmeshGetLpnPollTimeout(QMESH_SUBNET_KEY_IDX_INFO_T key_idx, uint16_t lpn_addr);

extern void QmeshFrndUpdateNwState(QMESH_SUBNET_KEY_IDX_INFO_T key_idx);
extern bool QmeshIsItFrndMesg(QMESH_SUBNET_KEY_IDX_INFO_T key_idx, uint16_t addr);


extern QMESH_RESULT_T frndRemoveFriendshipData(QMESH_SUBNET_KEY_IDX_INFO_T subnet_key_idx, bool all_subnets);
extern bool QmeshIsDstAddrForProxyRelayALLNodes(uint16_t dstaddr);
#endif /* __QMESH_FRND_H__ */

