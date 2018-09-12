/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file Generic_level_client.c
 *  \brief   This file implements the  Generic_level_client APIs of the
 *           SIG Mesh control protocol
 */
/******************************************************************************/

/*============================================================================*
 *  Library Header Files
 *===========================================================================*/

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/
#include "generic_level_client.h"

/*============================================================================*
 *  Private Data Type
 *===========================================================================*/

/*============================================================================*
 *  Private Function Prototypes
 *===========================================================================*/

/*============================================================================*
 *  Private Function Implementations
 *===========================================================================*/

/*============================================================================*
 *  Public Function Implementations
 *===========================================================================*/
/*----------------------------------------------------------------------------*
 *   GenericLevelClientGet
 *
 * \brief Generic Level Get is a reliable message used to set the Generic Level
		 state of an element to a new absolute value.The response to the Generic
		 Level Set message is a Generic Level Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericLevelClientGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_GENERIC_LEVEL_CLIENT_GET;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2] = {0}; /*2 - Opcode*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;

    /* key info */
    app_payload_key_info.key_type = QMESH_KEY_TYPE_APP_KEY;
    app_payload_key_info.key_info.prov_net_idx = header->keyInfo.prov_net_idx;
    app_payload_key_info.key_info.key_idx = header->keyInfo.key_idx;
    app_payload_key_info.use_frnd_keys = header->use_frnd_keys;

    /* payload info */
    data_info.src_addr = header->srcAddr;
    data_info.dst_addr = header->dstAddr;
    data_info.payload_len = index;
    data_info.payload = buffer;
    data_info.ttl = header->ttl;
    data_info.trans_mic_size = 4;
    data_info.trans_ack_required = FALSE;

    /* send to stack */
    status = QmeshSendMessage(&app_payload_key_info, &data_info);

    return status;
}

/*----------------------------------------------------------------------------*
 *   GenericLevelClientSet
 *
 * \brief Generic Level Set is a reliable message used to set the Generic Level
		 state of an element to a new absolute value.The response to the Generic
		 Level Set message is a Generic Level Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    GENERIC_LEVEL_CLIENT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericLevelClientSet(QMESH_MSG_HEADER_T *header, GENERIC_LEVEL_CLIENT_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_GENERIC_LEVEL_CLIENT_SET : QMESH_GENERIC_LEVEL_CLIENT_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 5] = {0}; /*2 - Opcode, 5 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_int16_t(buffer, data->level, &index); /* level */
    WRITE_uint8_t(buffer, data->tid, &index); /* tid */
    if (data->validTransitionTime)
    {
        WRITE_uint8_t(buffer, data->transitionTime, &index); /* transitionTime */
        WRITE_uint8_t(buffer, data->delay, &index); /* delay */
    }

    /* key info */
    app_payload_key_info.key_type = QMESH_KEY_TYPE_APP_KEY;
    app_payload_key_info.key_info.prov_net_idx = header->keyInfo.prov_net_idx;
    app_payload_key_info.key_info.key_idx = header->keyInfo.key_idx;
    app_payload_key_info.use_frnd_keys = header->use_frnd_keys;

    /* payload info */
    data_info.src_addr = header->srcAddr;
    data_info.dst_addr = header->dstAddr;
    data_info.payload_len = index;
    data_info.payload = buffer;
    data_info.ttl = header->ttl;
    data_info.trans_mic_size = 4;
    data_info.trans_ack_required = FALSE;

    /* send to stack */
    status = QmeshSendMessage(&app_payload_key_info, &data_info);

    return status;
}

/*----------------------------------------------------------------------------*
 *   GenericLevelClientDeltaSet
 *
 * \brief Generic Delta Set is a reliable message used to set the Generic Level
		 state of an element by a relative value.The message is transactional
		 – it supports changing the state by a cumulative value with a sequence
		 of messages that are partof a transaction.The response to the Generic
		 Delta Set message is a Generic Level Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    GENERIC_LEVEL_CLIENT_DELTA_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericLevelClientDeltaSet(QMESH_MSG_HEADER_T *header, GENERIC_LEVEL_CLIENT_DELTA_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_GENERIC_LEVEL_CLIENT_DELTA_SET : QMESH_GENERIC_LEVEL_CLIENT_DELTA_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 7] = {0}; /*2 - Opcode, 7 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_int32_t(buffer, data->deltaLevel, &index); /* deltaLevel */
    WRITE_uint8_t(buffer, data->tid, &index); /* tid */
    if (data->validTransitionTime)
    {
        WRITE_uint8_t(buffer, data->transitionTime, &index); /* transitionTime */
        WRITE_uint8_t(buffer, data->delay, &index); /* delay */
    }

    /* key info */
    app_payload_key_info.key_type = QMESH_KEY_TYPE_APP_KEY;
    app_payload_key_info.key_info.prov_net_idx = header->keyInfo.prov_net_idx;
    app_payload_key_info.key_info.key_idx = header->keyInfo.key_idx;
    app_payload_key_info.use_frnd_keys = header->use_frnd_keys;

    /* payload info */
    data_info.src_addr = header->srcAddr;
    data_info.dst_addr = header->dstAddr;
    data_info.payload_len = index;
    data_info.payload = buffer;
    data_info.ttl = header->ttl;
    data_info.trans_mic_size = 4;
    data_info.trans_ack_required = FALSE;

    /* send to stack */
    status = QmeshSendMessage(&app_payload_key_info, &data_info);

    return status;
}

/*----------------------------------------------------------------------------*
 *   GenericLevelClientMoveSet
 *
 * \brief Generic Move Set is a reliable message used to start a process of
		 changing the Generic Level state of an element with a defined transition
		 speed The response to the Generic Move Set message is a Generic Level
		 Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    GENERIC_LEVEL_CLIENT_MOVE_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericLevelClientMoveSet(QMESH_MSG_HEADER_T *header, GENERIC_LEVEL_CLIENT_MOVE_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_GENERIC_LEVEL_CLIENT_MOVE_SET : QMESH_GENERIC_LEVEL_CLIENT_MOVE_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 5] = {0}; /*2 - Opcode, 5 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_int16_t(buffer, data->deltaLevel, &index); /* deltaLevel */
    WRITE_uint8_t(buffer, data->tid, &index); /* tid */
    if (data->validTransitionTime)
    {
        WRITE_uint8_t(buffer, data->transitionTime, &index); /* transitionTime */
        WRITE_uint8_t(buffer, data->delay, &index); /* delay */
    }

    /* key info */
    app_payload_key_info.key_type = QMESH_KEY_TYPE_APP_KEY;
    app_payload_key_info.key_info.prov_net_idx = header->keyInfo.prov_net_idx;
    app_payload_key_info.key_info.key_idx = header->keyInfo.key_idx;
    app_payload_key_info.use_frnd_keys = header->use_frnd_keys;

    /* payload info */
    data_info.src_addr = header->srcAddr;
    data_info.dst_addr = header->dstAddr;
    data_info.payload_len = index;
    data_info.payload = buffer;
    data_info.ttl = header->ttl;
    data_info.trans_mic_size = 4;
    data_info.trans_ack_required = FALSE;

    /* send to stack */
    status = QmeshSendMessage(&app_payload_key_info, &data_info);

    return status;
}


