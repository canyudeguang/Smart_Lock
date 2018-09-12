/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file Generic_power_level_client.c
 *  \brief   This file implements the  Generic_power_level_client APIs of the
 *           SIG Mesh control protocol
 */
/******************************************************************************/

/*============================================================================*
 *  Library Header Files
 *===========================================================================*/

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/
#include "generic_power_level_client.h"

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
 *   GenericPowerLevelClientGet
 *
 * \brief Generic Power Level Get message is a reliable message used to get the
		 Generic Power Actual state of an element. The response to the Generic
		 Power Level Get message is a Generic Power Level Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerLevelClientGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_GENERIC_POWER_LEVEL_CLIENT_GET;
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
 *   GenericPowerLevelClientSet
 *
 * \brief Generic Power Level Set is a reliable message used to set the Generic
		 Power Actual state of an element.The response to the Generic Power
		 Level Set message is a Generic Power Level Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    GENERIC_POWER_LEVEL_CLIENT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerLevelClientSet(QMESH_MSG_HEADER_T *header, GENERIC_POWER_LEVEL_CLIENT_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_GENERIC_POWER_LEVEL_CLIENT_SET : QMESH_GENERIC_POWER_LEVEL_CLIENT_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 5] = {0}; /*2 - Opcode, 5 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_int16_t(buffer, data->power, &index); /* power */
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
 *   GenericPowerLevelClientLastGet
 *
 * \brief Generic Power Last Get is a reliable message used to get the Generic
		 Power Last state of an element.The response to a Generic Power Last
		 Get message is a Generic Power Last Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerLevelClientLastGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_GENERIC_POWER_LEVEL_CLIENT_LAST_GET;
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
 *   GenericPowerLevelClientDefaultGet
 *
 * \brief Generic Power Default Get is a reliable message used to get the
		 Generic Power Default state of an element.The response to a Generic
		 Power Default Get message is a Generic Power Default Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerLevelClientDefaultGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_GENERIC_POWER_LEVEL_CLIENT_DEFAULT_GET;
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
 *   GenericPowerLevelClientDefaultSet
 *
 * \brief Generic Power Default Set is a reliable message used to set the
		 Generic Power Default state of an element.The response to the Generic
		 Power Default Set message is a Generic Power Default Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    GENERIC_POWER_LEVEL_CLIENT_DEFAULT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerLevelClientDefaultSet(QMESH_MSG_HEADER_T *header, GENERIC_POWER_LEVEL_CLIENT_DEFAULT_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_GENERIC_POWER_LEVEL_CLIENT_DEFAULT_SET : QMESH_GENERIC_POWER_LEVEL_CLIENT_DEFAULT_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 2] = {0}; /*2 - Opcode, 2 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_int16_t(buffer, data->power, &index); /* power */

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
 *   GenericPowerLevelClientRangeGet
 *
 * \brief Generic Power Range Get is a reliable message used to get the Generic
		 Power Range state of an element.The response to the Generic Power Range
		 Get message is a Generic Power Range Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerLevelClientRangeGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_GENERIC_POWER_LEVEL_CLIENT_RANGE_GET;
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
 *   GenericPowerLevelClientRangeSet
 *
 * \brief Generic Power Range Set is a reliable message used to set the Generic
		 Power Range state of an element.The response to the Generic Power Range
		 Set message is a Generic Power Range Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    GENERIC_POWER_LEVEL_CLIENT_RANGE_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerLevelClientRangeSet(QMESH_MSG_HEADER_T *header, GENERIC_POWER_LEVEL_CLIENT_RANGE_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_GENERIC_POWER_LEVEL_CLIENT_RANGE_SET : QMESH_GENERIC_POWER_LEVEL_CLIENT_RANGE_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 4] = {0}; /*2 - Opcode, 4 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_uint16_t(buffer, data->rangeMin, &index); /* rangeMin */
    WRITE_uint16_t(buffer, data->rangeMax, &index); /* rangeMax */

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


