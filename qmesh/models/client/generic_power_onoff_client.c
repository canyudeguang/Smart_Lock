/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file Generic_power_onoff_client.c
 *  \brief   This file implements the  Generic_power_onoff_client APIs of the
 *           SIG Mesh control protocol
 */
/******************************************************************************/

/*============================================================================*
 *  Library Header Files
 *===========================================================================*/

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/
#include "generic_power_onoff_client.h"

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
 *   GenericPowerOnoffClientOnpowerupGet
 *
 * \brief Generic Power OnOff Client's OnPowerUp Get is a reliable message used to get the Generic
		 OnPowerUp state of an element.The response to the Generic OnPowerUp
		 Get message is a Generic OnPowerUp Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerOnoffClientOnpowerupGet(QMESH_MSG_HEADER_T *header)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = QMESH_GENERIC_POWER_ONOFF_CLIENT_ONPOWERUP_GET;
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
 *   GenericPowerOnoffClientOnpowerupSet
 *
 * \brief Generic Power OnOff Client's OnPowerUp Set is a reliable message used to set the Generic
		 OnPowerUp state of an element.The response to the Generic OnPowerUp
		 Set message is a Generic OnPowerUp Status message.
 *
 * \param header  QMESH_MSG_HEADER_T
 * \param data    GENERIC_POWER_ONOFF_CLIENT_ONPOWERUP_SET_T
 *
 * \returns \ref QMESH_RESULT_T
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T GenericPowerOnoffClientOnpowerupSet(QMESH_MSG_HEADER_T *header, GENERIC_POWER_ONOFF_CLIENT_ONPOWERUP_SET_T *data)
{
    QMESH_RESULT_T status = QMESH_RESULT_SUCCESS;
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_payload_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    uint16_t opcode = header->reliable ? QMESH_GENERIC_POWER_ONOFF_CLIENT_ONPOWERUP_SET : QMESH_GENERIC_POWER_ONOFF_CLIENT_ONPOWERUP_SET_UNRELIABLE;
    uint8_t opcodeLen = 2;
    uint8_t index = 0;
    uint8_t buffer[2 + 1] = {0}; /*2 - Opcode, 1 - Params*/

    /* serialize params */
    WRITE_Opcode(buffer, opcode, opcodeLen); /* opcode */
    index += opcodeLen;
    WRITE_uint8_t(buffer, data->onPowerUp, &index); /* onPowerUp */

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


