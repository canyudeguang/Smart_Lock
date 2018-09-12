/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_generic_provisioning.h
 *  \brief This file contains the Prototype for generic provisioning layer 
 *      functions.
 */
 /*****************************************************************************/
 
#ifndef __QMESH_GENERIC_PROVISIONING_H__
#define __QMESH_GENERIC_PROVISIONING_H__

/* Identify states */
#define MESH_IDENTIFY_STATE_OFF        0x00

#define GPL_LINK_LOSS_EVENT                0
#define GPL_LINK_CLOSE_EVENT               1

typedef enum
{
    QMESH_GP_STATE_IDLE,
    QMESH_GP_STATE_TX_IN_PROGRESS,
    QMESH_GP_STATE_RX_IN_PROGRESS
} QMESH_GP_STATE_T;

typedef struct
{
    uint8_t     fcs;
    uint16_t    pdu_len;
    uint8_t     num_of_segments;
} QMESH_GP_TX_PDU_INFO_T;

typedef struct
{
    /* Current rx pdu info */
    uint8_t     segN;
    uint16_t    total_len;
    uint8_t     fcs;
    uint8_t     rcv_packet_len;
    uint16_t    frag_idx_mask; /* Bit mask of fragment indexes */
} QMESH_GP_RX_PDU_INFO_T;

typedef struct
{
    QMESH_GP_STATE_T state;

    QMESH_GP_RX_PDU_INFO_T rx_pdu_info;

    /* Current tx pp pdu info, used in retransmission */
    QMESH_GP_TX_PDU_INFO_T tx_pdu_info;

    /* Message request Timeout */
    QMESH_TIMER_HANDLE_T req_timeout_tid;

    /* Message retry Timer */
    QMESH_TIMER_HANDLE_T retry_timeout_tid;

    bool ctrl_msg;
    uint8_t ctrl_opcode;
} QMESH_GP_CONTEXT_T;

extern QmeshResult GenProvLinkOpen(const QMESH_DEVICE_UUID_T uuid);
extern void GenProvProcessMsg(uint8_t *msg, uint16_t msg_len);
extern QmeshResult GenProvSendMsg(uint16_t msg_len, bool ctrl_msg, uint8_t ctrl_opcode);
extern void GenericProvHandleLinkEvent(uint16_t event_id);
extern void GenericProvSetBuffer(void);
extern void GenProvReset(uint8_t reason);
extern void GenProvStopTimers(void);

#endif /* __QMESH_GENERIC_PROVISIONING_H__ */

