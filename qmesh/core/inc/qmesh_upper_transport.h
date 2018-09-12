/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_upper_transport.h
 *
 *  \brief Qmesh Upper Transport Layer API
 *
 *   This file contains the API for Qmesh Upper Transport Layer.
 */
 /*****************************************************************************/

#ifndef _QMESH_UPPER_TRANSPORT_H_
#define _QMESH_UPPER_TRANSPORT_H_

#include "qmesh_lower_transport.h"

/*============================================================================*
    Macro Definitions
 *============================================================================*/

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/
/* !< \brief Transport Control Opcodes. */
typedef enum
{
    QMESH_TRANS_CNTRL_SEG_ACK                   = 0,

    QMESH_TRANS_CNTRL_FRND_POLL                 = 1,
    QMESH_TRANS_CNTRL_FRND_UPDATE               = 2,
    QMESH_TRANS_CNTRL_FRND_REQUEST              = 3,
    QMESH_TRANS_CNTRL_FRND_OFFER                = 4,
    QMESH_TRANS_CNTRL_FRND_CLEAR                = 5,
    QMESH_TRANS_CNTRL_FRND_CLEAR_CONFIRM        = 6,
    QMESH_TRANS_CNTRL_FRND_SUBSLIST_ADD         = 7,
    QMESH_TRANS_CNTRL_FRND_SUBSLIST_REMOVE      = 8,
    QMESH_TRANS_CNTRL_FRND_SUBSLIST_CONFIRM     = 9,

    QMESH_TRANS_CNTRL_HEARTBEAT                 = 10,
} QMESH_TRANS_CNTRL_OPCODE_T;


/*============================================================================*
    Data Type Definitions
 *============================================================================*/

typedef QMESH_ACCESS_PAYLOAD_KEY_INFO_T QMESH_UT_KEY_INFO_T;

typedef struct
{
    uint16_t                        peer_addr;          /*!< Peer Address. */
    QMESH_SUBNET_KEY_IDX_INFO_T     key_idx;            /*!< Subnetwork Key Index. See \ref QMESH_SUBNET_KEY_IDX_INFO_T. */
    uint16_t                        frnd_lpn_addr;      /*!< Friend or LPN Address. */
    uint8_t                         ttl;                /*!< Time To Live. */
    QMESH_TRANS_CNTRL_OPCODE_T      opcode;             /*!< Transport Control Opcode. See \ref QMESH_TRANS_CNTRL_OPCODE_T. */
    uint8_t                         num_retries;        /*!< Number of Retries. */
} QMESH_CNTRL_INFO_T;

typedef struct
{
    QMESH_UT_KEY_INFO_T                  key_info;
    uint16_t                             src_addr;
    uint16_t                             dst_addr;
    const uint16_t                      *uuid;
    uint8_t                              ttl;
    uint8_t                              mic_len;
    bool                                 use_trans_ack;
    uint8_t                              num_retries;
} QMESH_ACCESS_INFO_T;


/*============================================================================*
    API Definitions
 *============================================================================*/

QMESH_RESULT_T QmeshUpperTransportInit(void);

QMESH_RESULT_T QmeshUpperTransportTXControlPDU(const QMESH_CNTRL_INFO_T *ctrl_info,
                                               uint8_t *cntrl_pdu, uint8_t cntrl_pdu_len);

QMESH_RESULT_T QmeshUpperTransportTXAccessPDU(const QMESH_ACCESS_INFO_T *access_info,
                                              const uint8_t *access_pdu, uint16_t access_pdu_len);

QMESH_RESULT_T QmeshUpperTransportRXPDU(const QMESH_BEARER_HEADER_T *bearer_hdr, const QMESH_NETWORK_INFO_T *nw_info,
                                        const QMESH_LT_RX_INFO_T *lt_rx_info, QMESH_UT_PDU_T *ut_pdu);

void QmeshUpperTransportDeInit(void);

#endif /* _QMESH_UPPER_TRANSPORT_H_ */

