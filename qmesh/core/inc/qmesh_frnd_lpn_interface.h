/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_frnd_lpn_interface.h
 *
 *  \brief Qmesh Friend and LPN Interface API
 *
 *   This file contains the API for Qmesh Friend and LPN Interface.
 */
 /*****************************************************************************/
#ifndef _QMESH_FRND_LPN_INTERFACE_H_
#define _QMESH_FRND_LPN_INTERFACE_H_

/*============================================================================*
    Header Files
 *============================================================================*/

#include "qmesh_hal_ifce.h"
#include "qmesh_network.h"
#include "qmesh_lower_transport.h"
#include "qmesh_upper_transport.h"
#include "qmesh_db.h"

/*============================================================================*
    Macro Definitions
 *============================================================================*/

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/

/*============================================================================*
    Data Type Definitions
 *============================================================================*/
typedef struct
{
    void (*frndUpdateState)(QMESH_SUBNET_KEY_IDX_INFO_T key_idx);
    bool (*frndMesg)(QMESH_SUBNET_KEY_IDX_INFO_T key_idx, uint16_t addr);
    QMESH_RESULT_T (*frndRXControlPDU)(const QMESH_BEARER_HEADER_T *bearer_hdr, const QMESH_CNTRL_INFO_T *ctrl_info,
                                       uint8_t *cntrl_pdu, uint8_t cntrl_pdu_len);
    QMESH_RESULT_T (*frndRXTransportPDU)(const QMESH_BEARER_HEADER_T *bearer_hdr, const QMESH_NETWORK_INFO_T *nw_info,
                                         const QMESH_LT_PDU_LIST_T *lt_pdu_list);
    uint32_t (*frndLpnPollTimeout)(QMESH_SUBNET_KEY_IDX_INFO_T key_idx, uint16_t lpn_addr);
    QMESH_RESULT_T (*frndRemoveFriendship)(QMESH_SUBNET_KEY_IDX_INFO_T subnet_key_idx, bool all_subnets);
} QMESH_FRIEND_HANDLERS_T;


/*! \brief LPN handler type */
typedef struct
{
    QMESH_RESULT_T (*lpnUpdateStatus)(QMESH_SUBNET_KEY_IDX_INFO_T subnet_idx);     /*!< \brief  LPN update status handler type */
    QMESH_RESULT_T (*lpnRXControlPDU)(const QMESH_BEARER_HEADER_T *bearer_hdr,
                                      const QMESH_CNTRL_INFO_T *ctrl_info,
                                      uint8_t *cntrl_pdu, uint8_t cntrl_pdu_len);   /*!< \brief  LPN Rx Control PDU handler type */
    bool           (*lpnIsFrndEstablished)(QMESH_SUBNET_KEY_IDX_INFO_T subnet_idx);
    QMESH_RESULT_T (*lpnRemoveFriendship)(QMESH_SUBNET_KEY_IDX_INFO_T subnet_key_idx, bool all_subnets);
} QMESH_LPN_HANDLERS_T;

/*============================================================================*
    API Definitions
 *============================================================================*/

QMESH_RESULT_T QmeshRegisterFrndAPI(const QMESH_FRIEND_HANDLERS_T *frnd_api_hdlrs);

QMESH_RESULT_T QmeshRegisterLpnAPI(const QMESH_LPN_HANDLERS_T *lpn_api_hdlrs);

void QmeshFriendUpdateState(QMESH_SUBNET_KEY_IDX_INFO_T key_idx);

bool QmeshIsFrndMesg(QMESH_SUBNET_KEY_IDX_INFO_T key_idx, uint16_t addr);

QMESH_RESULT_T QmeshFrndLpnRXCntrlPDU(const QMESH_BEARER_HEADER_T *bearer_hdr, const QMESH_CNTRL_INFO_T *ctrl_info,
                                      uint8_t *cntrl_pdu, uint8_t cntrl_pdu_len);

QMESH_RESULT_T QmeshFriendTransportRXPDU(const QMESH_BEARER_HEADER_T *bearer_hdr, const QMESH_NETWORK_INFO_T *nw_info,
                                         const QMESH_LT_PDU_LIST_T *lt_pdu_list);

QMESH_RESULT_T QmeshFrndLpnRemoveFriendship(QMESH_SUBNET_KEY_IDX_INFO_T subnet_key_idx, bool all_subnets);

uint32_t QmeshFriendGetLpnPollTimeOut(QMESH_SUBNET_KEY_IDX_INFO_T key_idx, uint16_t lpn_addr);

QMESH_RESULT_T QmeshLpnUpdateStatus(QMESH_SUBNET_KEY_IDX_INFO_T subnet_idx);

bool QmeshLpnIsFriendshipEstablished(QMESH_SUBNET_KEY_IDX_INFO_T subnet_idx);

#endif /* _QMESH_FRND_LPN_INTERFACE_H_ */


