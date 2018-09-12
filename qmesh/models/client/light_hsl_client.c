/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file Light_hsl_client.c
 *  \brief   This file implements the  Light_hsl_client APIs of the
 *           SIG Mesh control protocol
 */
/******************************************************************************/

/*============================================================================*
 *  Library Header Files
 *===========================================================================*/

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/
#include "light_hsl_client.h"

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
 *   LightHslClientGet
 *
 * \brief Light HSL Get is a reliable message used to get the Light
		 HSL Lightness, Light HSL Hue and Light HSL Saturation states
		 of an element. The response to the Light HSL Get message is
		  a Light HSL Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_LIGHT_HSL_CLIENT_GET;
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
 *   LightHslClientSet
 *
 * \brief The Light HSL Set is a message used to set the Light
		 HSL Lightness state, Light HSL Hue State and Light HSL Saturation state
		 of an element.The response to the Light HSL Set message is
		 a Light HSL Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    LIGHT_HSL_CLIENT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientSet(QMESH_MSG_HEADER_T *header, LIGHT_HSL_CLIENT_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_LIGHT_HSL_CLIENT_SET : QMESH_LIGHT_HSL_CLIENT_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 9] = {0}; /*2 - Opcode, 9 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_uint16_t(buffer, data->hSLLightness, &index); /* hSLLightness */
    WRITE_uint16_t(buffer, data->hSLHue, &index); /* hSLHue */
    WRITE_uint16_t(buffer, data->hSLSaturation, &index); /* hSLSaturation */
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
 *   LightHslClientHueGet
 *
 * \brief Light HSL Hue Get is a reliable message used to get the Light
		 HSL Hue state of an element.The response to the Light
		 HSL Hue Get message is a Light HSL Hue Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientHueGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_LIGHT_HSL_CLIENT_HUE_GET;
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
 *   LightHslClientHueSet
 *
 * \brief The Light HSL Hue Set is a message used to set the
		 Light HSL Hue state of an element.The response to the Light
		 HSL Hue Set message is a Light HSL Hue Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    LIGHT_HSL_CLIENT_HUE_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientHueSet(QMESH_MSG_HEADER_T *header, LIGHT_HSL_CLIENT_HUE_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_LIGHT_HSL_CLIENT_HUE_SET : QMESH_LIGHT_HSL_CLIENT_HUE_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 5] = {0}; /*2 - Opcode, 5 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_uint16_t(buffer, data->hue, &index); /* hue */
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
 *   LightHslClientSaturationGet
 *
 * \brief Light HSL Saturation Get is a reliable message used to get the Light
		 HSL Saturation state of an element.The response to the Light HSL
		 Saturation Get message is a Light HSL Saturation Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientSaturationGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_LIGHT_HSL_CLIENT_SATURATION_GET;
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
 *   LightHslClientSaturationSet
 *
 * \brief The Light HSL Saturation Set is a reliable message used to set the
		 Light HSL Saturation state of an element.The response to the Light
		 HSL Saturation Set message is a Light HSL Saturation Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    LIGHT_HSL_CLIENT_SATURATION_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientSaturationSet(QMESH_MSG_HEADER_T *header, LIGHT_HSL_CLIENT_SATURATION_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_LIGHT_HSL_CLIENT_SATURATION_SET : QMESH_LIGHT_HSL_CLIENT_SATURATION_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 5] = {0}; /*2 - Opcode, 5 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_uint16_t(buffer, data->saturation, &index); /* saturation */
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
 *   LightHslClientTargetGet
 *
 * \brief Light HSL Target Get is a reliable message used to get the
		 the target Light HSL Lightness, Light HSL Hue and Light HSL
		  Saturation states of an element.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientTargetGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_LIGHT_HSL_CLIENT_TARGET_GET;
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
 *   LightHslClientDefaultGet
 *
 * \brief Light HSL Default Get is a reliable message used to get the
		 Light Lightness Default, the Light HSL Hue Default
		 and Light HSL Saturation Default states of an element.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientDefaultGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_LIGHT_HSL_CLIENT_DEFAULT_GET;
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
 *   LightHslClientDefaultSet
 *
 * \brief The Light HSL Default Set is a reliable message used to set the
		 Light Lightness Default, the Light HSL Hue Default
		 and Light HSL Saturation Default states of an element.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    LIGHT_HSL_CLIENT_DEFAULT_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientDefaultSet(QMESH_MSG_HEADER_T *header, LIGHT_HSL_CLIENT_DEFAULT_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_LIGHT_HSL_CLIENT_DEFAULT_SET : QMESH_LIGHT_HSL_CLIENT_DEFAULT_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 6] = {0}; /*2 - Opcode, 6 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_uint16_t(buffer, data->lightness, &index); /* lightness */
    WRITE_uint16_t(buffer, data->hue, &index); /* hue */
    WRITE_uint16_t(buffer, data->saturation, &index); /* saturation */

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
 *   LightHslClientRangeGet
 *
 * \brief The Light HSL Range Get is a reliable message used to get the
		 Light HSL Hue Range and Light HSL Saturation Range states
		 of an element.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientRangeGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_LIGHT_HSL_CLIENT_RANGE_GET;
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
 *   LightHslClientRangeSet
 *
 * \brief Light HSL Range Set is a message used to set the
		 Light HSL Hue Range and Light HSL Saturation Range states
		 of an element.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    LIGHT_HSL_CLIENT_RANGE_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T LightHslClientRangeSet(QMESH_MSG_HEADER_T *header, LIGHT_HSL_CLIENT_RANGE_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_LIGHT_HSL_CLIENT_RANGE_SET : QMESH_LIGHT_HSL_CLIENT_RANGE_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 8] = {0}; /*2 - Opcode, 8 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_uint16_t(buffer, data->hueRangeMin, &index); /* hueRangeMin */
    WRITE_uint16_t(buffer, data->hueRangeMax, &index); /* hueRangeMax */
    WRITE_uint16_t(buffer, data->saturationRangeMin, &index); /* saturationRangeMin */
    WRITE_uint16_t(buffer, data->saturationRangeMax, &index); /* saturationRangeMax */

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


