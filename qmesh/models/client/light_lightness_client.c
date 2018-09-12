/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file Light_lightness_client.c
 *  \brief   This file implements the  Light_lightness_client APIs of the
 *           SIG Mesh control protocol
 */
/******************************************************************************/

/*============================================================================*
 *  Library Header Files
 *===========================================================================*/

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/
#include "light_lightness_client.h"

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
 *   LightLightnessClientGet
 *
 * \brief Light Lightness Get is a reliable message used to get the Light
		 Lightness Actual state of an element.The response to the Light
		 Lightness Get message is a Light Lightness Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_LIGHT_LIGHTNESS_CLIENT_GET;
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
 *   LightLightnessClientSet
 *
 * \brief The Light Lightness Set is a reliable message used to set the Light
		 Lightness Actual state of an element.The response to the Light
		 Lightness Set message is a Light Lightness Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    LIGHT_LIGHTNESS_CLIENT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientSet(QMESH_MSG_HEADER_T *header, LIGHT_LIGHTNESS_CLIENT_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_LIGHT_LIGHTNESS_CLIENT_SET : QMESH_LIGHT_LIGHTNESS_CLIENT_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 5] = {0}; /*2 - Opcode, 5 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_uint16_t(buffer, data->lightness, &index); /* lightness */
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
 *   LightLightnessClientLinearGet
 *
 * \brief Light Lightness Linear Get is a reliable message used to get the Light
		 Lightness Linear state of an element.The response to the Light
		 Lightness Linear Get message is a Light Lightness Linear Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientLinearGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_LIGHT_LIGHTNESS_CLIENT_LINEAR_GET;
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
 *   LightLightnessClientLinearSet
 *
 * \brief The Light Lightness Linear Set is a reliable message used to set the
		 Light Lightness Linear state of an element.The response to the Light
		 Lightness Linear Set message is a Light Lightness Linear Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    LIGHT_LIGHTNESS_CLIENT_LINEAR_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientLinearSet(QMESH_MSG_HEADER_T *header, LIGHT_LIGHTNESS_CLIENT_LINEAR_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_LIGHT_LIGHTNESS_CLIENT_LINEAR_SET : QMESH_LIGHT_LIGHTNESS_CLIENT_LINEAR_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 5] = {0}; /*2 - Opcode, 5 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_uint16_t(buffer, data->lightness, &index); /* lightness */
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
 *   LightLightnessClientLastGet
 *
 * \brief Light Lightness Last Get is a reliable message used to get the Light
		 Lightness Last state of an element.The response to the Light Lightness
		 Last Get message is a Light Lightness Last Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientLastGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_LIGHT_LIGHTNESS_CLIENT_LAST_GET;
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
 *   LightLightnessClientDefaultGet
 *
 * \brief Light Lightness Default Get is a reliable message used to get the
		 Light Lightness Default state of an element.The response to the Light
		 Lightness Default Get message is a Light Lightness Default Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientDefaultGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_LIGHT_LIGHTNESS_CLIENT_DEFAULT_GET;
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
 *   LightLightnessClientDefaultSet
 *
 * \brief The Light Lightness Default Set is a reliable message used to set the
		 Light Lightness Default state of an element
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    LIGHT_LIGHTNESS_CLIENT_DEFAULT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientDefaultSet(QMESH_MSG_HEADER_T *header, LIGHT_LIGHTNESS_CLIENT_DEFAULT_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_LIGHT_LIGHTNESS_CLIENT_DEFAULT_SET : QMESH_LIGHT_LIGHTNESS_CLIENT_DEFAULT_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 2] = {0}; /*2 - Opcode, 2 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_uint16_t(buffer, data->lightness, &index); /* lightness */

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
 *   LightLightnessClientRangeGet
 *
 * \brief The Light Lightness Range Get is a reliable message used to get the
		 Light Lightness Range state of an element.The response to the Light
		 Lightness Range Get message is a Light Lightness Range Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientRangeGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_LIGHT_LIGHTNESS_CLIENT_RANGE_GET;
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
 *   LightLightnessClientRangeSet
 *
 * \brief Light Lightness Range Set is a reliable message used to set the Light
		 Lightness Range state of an element.The response to the Light Lightness
		 Range Get message is a Light Lightness Range Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    LIGHT_LIGHTNESS_CLIENT_RANGE_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightLightnessClientRangeSet(QMESH_MSG_HEADER_T *header, LIGHT_LIGHTNESS_CLIENT_RANGE_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_LIGHT_LIGHTNESS_CLIENT_RANGE_SET : QMESH_LIGHT_LIGHTNESS_CLIENT_RANGE_SET_UNRELIABLE;
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


