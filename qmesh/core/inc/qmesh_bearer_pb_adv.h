/******************************************************************************
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_bearer_pb_adv.h
 *  \brief Qmesh bearer porting APIs for Qmesh library
 *
 *   This file contains and data type for bearer porting.
 */
 /******************************************************************************/

#ifndef __QMESH_BEARER_PB_ADV_H__
#define __QMESH_BEARER_PB_ADV_H__

#include "qmesh_data_types.h"
#include "qmesh_result.h"
#include "qmesh_bearer.h"

/* Generic provisioning message types */
#define GPCF_TRANS_START                 0x00
#define GPCF_TRANS_ACK                   0x01
#define GPCF_TRANS_CONT                  0x02
#define GPCF_BEARER_CTRL                 0x03

#define GPCF_BITMASK                     0x03

/* PB ADV Bearer Control message sub types */
#define LINK_OPEN_REQ   ((0x00 << 2)|GPCF_BEARER_CTRL)
#define LINK_ACK        ((0x01 << 2)|GPCF_BEARER_CTRL)
#define LINK_CLOSE_IND  ((0x02 << 2)|GPCF_BEARER_CTRL)

#define LINK_OPEN_REQ_LEN_OCTETS         17
#define LINK_OPEN_CFM_LEN_OCTETS          1
#define LINK_CLOSE_IND_LEN_OCTETS         2

/* Link close reason */
#define LINK_CLOSE_REASON_SUCCESS         0
#define LINK_CLOSE_REASON_TIMEOUT         1
#define LINK_CLOSE_REASON_FAIL            2

#define PB_ADV_HDR_SIZE_IN_BYTES          5 /* Link ID (4 bytes), trans (1 byte) */

/* Link Id */
#define PB_ADV_LINK_ID_OFFSET             0
#define PB_ADV_LINK_ID_LEN                4

/* Link ID (4 bytes), trans (1 byte) */
/* Transaction num */
#define GP_TRANS_NUM_OFFSET               (PB_ADV_LINK_ID_OFFSET + PB_ADV_LINK_ID_LEN)
#define GP_TRANS_NUM_LEN                  1

/* GP Control field */
#define GP_CTRL_OFFSET                    (GP_TRANS_NUM_OFFSET + GP_TRANS_NUM_LEN)
#define GP_CTRL_LEN                       1

/* GP Control PDU offset */
#define GP_CTRL_PDU_OFFSET                (GP_CTRL_OFFSET + GP_CTRL_LEN)

/* Total len */
#define GP_TOTAL_LEN_OFFSET               (GP_CTRL_OFFSET + GP_CTRL_LEN)
#define GP_TOTAL_LEN_LEN                  2

/* FCS */
#define GP_FCS_OFFSET                     (GP_TOTAL_LEN_OFFSET + GP_TOTAL_LEN_LEN)
#define GP_FCS_LEN                        1

/* Data  */
#define GP_START_PDU_OFFSET               (GP_FCS_OFFSET + GP_FCS_LEN)
#define GP_CONT_PDU_OFFSET                (GP_CTRL_OFFSET + GP_CTRL_LEN)

#define LINK_ACK_MSG_LEN_IN_BYTES         1

/* Link states */
typedef enum
{
    PB_ADV_LINK_STATE_IDLE,     /*!< \brief No link */
    PB_ADV_LINK_STATE_OPEN_REQ, /*!< \brief Provisioner is waiting for Link open ack */
    PB_ADV_LINK_STATE_OPEN,     /*!< \brief PB Adv Link active */
} PB_ADV_LINK_STATE_T;

typedef struct
{
    PB_ADV_LINK_STATE_T     state;
    uint8_t                 link_id[PB_ADV_LINK_ID_LEN];
    uint8_t                 last_rx_trans_num;
    uint8_t                 tx_trans_num;
} PB_ADV_LINK_INFO_T;;

extern QMESH_RESULT_T PbAdvLinkOpen(void);
extern void PbAdvLinkReset(uint8_t reason);
extern void PbAdvBearerInit(void);
extern bool PbAdvIsLinkActive(void);
extern bool PbAdvValidateLink(uint8_t *msg);
extern void PbAdvSendMsg(uint8_t msg_type, uint8_t trans, uint8_t *msg, uint16_t msg_len);
extern QMESH_RESULT_T PbAdvBrerHandleCtrlMsg(uint8_t gp_ctrl_info, uint8_t *msg, uint16_t msg_len);

#endif /* __QMESH_BEARER_PB_ADV_H__ */

