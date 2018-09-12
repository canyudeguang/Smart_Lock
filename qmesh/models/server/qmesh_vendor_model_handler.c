/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_vendor_model_handler.c
 *  \brief defines and functions for Vendor model Server model.
 *
 *   This file contains the implementation of Vendor Model Server model. This file
 *   should be built as part of the model Application.
 */
/******************************************************************************/

#include "qmesh_model_common.h"
#include "qmesh_model_config.h"
#include "qmesh_vendor_model_handler.h"


static uint32_t count = 0;
/*============================================================================*
 *  Public Function Implementations
 *===========================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshVendorModelStatusSend
 *
 *  DESCRIPTION
 *      Sends vendor model status message
 *
 *  RETURNS
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshVendorModelStatusSend (
    QMESH_VENDOR_MODEL_DATA_T *model_context,
    QMESH_MODEL_MSG_COMMON_T *msg_cmn, uint16_t len)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    if (model_context == NULL)
        return;

    /* Construct the status payload */
    model_context->data[0] = ((QMESH_VENDOR_MODEL_OPCODE_STATUS_1 & 0xFF0000) >> 16);
    model_context->data[1] = ((QMESH_VENDOR_MODEL_OPCODE_STATUS_1 & 0x00FF00) >> 8);
    model_context->data[2] = (QMESH_VENDOR_MODEL_OPCODE_STATUS_1 & 0x00FF);

    data_info.payload = model_context->data;
    data_info.payload_len = len + QMESH_OPCODE_FORMAT_THREE_BYTE;
    data_info.ttl = QMESH_MODEL_DEFAULT_TTL;
    data_info.src_addr = model_context->elm_addr;
    data_info.dst_addr = msg_cmn->src;
    data_info.trans_ack_required = FALSE;
    data_info.trans_mic_size = QMESH_MIC_4_BYTES;
    /* key info */
    app_key_info.key_type = model_context->key_info.key_type;
    app_key_info.key_info.prov_net_idx =
        model_context->key_info.key_info.prov_net_idx;
    app_key_info.key_info.key_idx = model_context->key_info.key_info.key_idx;

    if (QmeshSendMessage (&app_key_info, &data_info)
        != QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_PRINT ("%s : QmeshSendMessage failed\n", __FUNCTION__);
    }
    DEBUG_MODEL_PRINT ("%s : QmeshSendMessage done\n", __FUNCTION__);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshVendorModelSet
 *
 *  DESCRIPTION
 *      Set associated State value
 *
 *  RETURNS
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshVendorModelSet (QMESH_VENDOR_MODEL_DATA_T
                                       *context, uint8_t *data)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_VENDOR_MODEL, "%s\n", __FUNCTION__);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshVendorModelServerHandler
 *
 *  DESCRIPTION
 *      This function handles events for vendor model server
 *
 *  RETURNS
 *      QMESH_RESULT_T
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshVendorModelServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len)
{
    QMESH_RESULT_T result = QMESH_RESULT_SUCCESS;
    QMESH_MODEL_MSG_COMMON_T msg_cmn = {0};
    QMESH_VENDOR_MODEL_DATA_T *model_context = NULL;
    uint32_t opcode, seq_no;
    uint16_t src_addr;

    /* Get the model context data for the element ID 'elm_addr' from the composition data */
    model_context = (QMESH_VENDOR_MODEL_DATA_T *) model_data->model_priv_data;

    if ((model_context == NULL) ||
        (!msg || msg_len <= 0))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_VENDOR_MODEL,
                          "Context data is NULL or msg is invalid \n");
        return QMESH_RESULT_FAILURE;
    }

    /* Extract the opcode */
    if ( QMESH_OPCODE_FORMAT_THREE_BYTE == get_opcode_format (
             msg[MODEL_OPCODE_OFFSET]))
    {
        opcode = QMESH_PACK_THREE_BYTE_OPCODE(msg[0], msg[1], msg[2]);
    }
    else
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_VENDOR_MODEL, "%s: Opcode < 3 bytes\n", __FUNCTION__);
        return QMESH_RESULT_FAILURE;
    }

    count++;
    DEBUG_MODEL_PRINT ("%s:Opcode=0x%06x, src=0x%x, seq_num=0x%06x, count=%d\n", 
                    __FUNCTION__, opcode, nw_hdr->src_addr, nw_hdr->seq_num, count);

    model_context->elm_addr = nw_hdr->dst_addr;
    msg_cmn.opcode = opcode;
    msg_cmn.src = nw_hdr->src_addr;
    msg_cmn.dst = nw_hdr->dst_addr;
    QmeshMemCpy (&model_context->key_info, key_info,
                 sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));

    /* If the message is the last processed then ignore the message */
    if (model_context->last_msg_seq_no == nw_hdr->seq_num &&
        model_context->last_src_addr == nw_hdr->src_addr)
    {
        DEBUG_MODEL_PRINT ("%s: seq num 0x%08x processed\n", __FUNCTION__, nw_hdr->seq_num);
        return QMESH_RESULT_SUCCESS;
    }

    /* Store the previous seqence number and source address */
    seq_no = model_context->last_msg_seq_no;
    src_addr = model_context->last_src_addr;
    /* Store the sequence number and src address of the new message */
    model_context->last_msg_seq_no = nw_hdr->seq_num;
    model_context->last_src_addr = nw_hdr->src_addr;
    msg += QMESH_OPCODE_FORMAT_THREE_BYTE;
    msg_len -= QMESH_OPCODE_FORMAT_THREE_BYTE;

    switch (opcode)
    {
        case QMESH_VENDOR_MODEL_OPCODE_SET_STATE_1:
        case QMESH_VENDOR_MODEL_OPCODE_SET_UNRELIABLE_STATE_1:
        {
            if((msg_len == 0) || (msg_len > (QMESH_VENDOR_MODEL_MAX_DATA_LEN - QMESH_OPCODE_FORMAT_THREE_BYTE)))
            {
                DEBUG_MODEL_PRINT ("%s:Invalid msg_len=0x%x\n", __FUNCTION__, msg_len);
                return QMESH_RESULT_FAILURE;
            }
            QmeshMemSet(model_context->data, 0, QMESH_VENDOR_MODEL_MAX_DATA_LEN);
            QmeshMemCpy(&model_context->data[QMESH_OPCODE_FORMAT_THREE_BYTE], &msg[0], msg_len);
        }
        break;

        case QMESH_VENDOR_MODEL_OPCODE_GET_STATE_1:
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_VENDOR_MODEL, "%s:QMESH_VENDOR_MODEL_OPCODE_GET_STATE_1\n", __FUNCTION__);
        }
        break;

        default:
        {
            /* Retore the previous sequence number and src address - since the message is not processed */
            model_context->last_msg_seq_no = seq_no;
            model_context->last_src_addr = src_addr;
            return QMESH_RESULT_FAILURE;
        }
    }

    if(opcode != QMESH_VENDOR_MODEL_OPCODE_SET_UNRELIABLE_STATE_1)
        QmeshVendorModelStatusSend(model_context, &msg_cmn, QMESH_VENDOR_MODEL_MAX_DATA_LEN - QMESH_OPCODE_FORMAT_THREE_BYTE);

    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshVendorModelInit
 *
 *  DESCRIPTION
 *      Initializes vendor sever model
 *
 *  RETURNS
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshVendorModelInit (QMESH_VENDOR_MODEL_DATA_T *model_data)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_VENDOR_MODEL, "%s\n", __FUNCTION__);
    count = 0;
    /* Restoring context from NVM needs to be added - after NVM support */
}
