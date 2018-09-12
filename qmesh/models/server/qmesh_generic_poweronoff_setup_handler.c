/******************************************************************************
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_model_generic_handler.c
 *  \brief defines and functions for SIGmesh Generic_OnOff Server model.
 *
 *   This file contains the implementation of Generic_OnOff Server model. This file
 *   should be built as part of the model Application.
 */
/******************************************************************************/

#include "qmesh_model_common.h"
#include "qmesh_model_config.h"
#include "qmesh_generic_poweronoff_setup_handler.h"
#include "qmesh_model_nvm.h"
/*============================================================================*
 *  Public Function Implementations
 *===========================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerOnOffSetupServerHandler
 *
 *  DESCRIPTION
 *      This function handles events for Generic PowerOnOff Setup server
 *
 *  RETURNS/MODIFIES
 *      returns QMESH_RESULT_T
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericPowerOnOffSetupServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len)
{
    QMESH_RESULT_T result = QMESH_RESULT_SUCCESS;
    QMESH_MODEL_MSG_COMMON_T msg_cmn;
    QMESH_GEN_POWERONOFF_DATA_T *model_context = NULL;
    uint32_t opcode, seq_no;
    uint16_t src_addr;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER, "%s:\n", __FUNCTION__);
    /* Get the model context data for the element ID 'elm_addr' from the composition data */
    model_context = (QMESH_GEN_POWERONOFF_DATA_T *) model_data->model_priv_data;

    if ((model_context == NULL) ||
        (!msg || msg_len <= 0))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                          "Context data is NULL or msg is invalid \n");
        return QMESH_RESULT_FAILURE;
    }

    /* Extract the opcode */
    if ( QMESH_OPCODE_FORMAT_TWO_BYTE == get_opcode_format (
             msg[MODEL_OPCODE_OFFSET]))
    {
        opcode = QMESH_PACK_TWO_BYTE_OPCODE (msg[0], msg[1]);
    }
    else
    {
        return QMESH_RESULT_FAILURE;
    }

    switch (opcode)
    {
        case QMESH_GENERIC_ONOFF_GET:
        case QMESH_GENERIC_ONOFF_SET:
        case QMESH_GENERIC_ONOFF_SET_UNRELIABLE:
        {
            /* Get the model data for the OnOff model from the element data */
            QMESH_MODEL_DATA_T *mdl_data = QmeshGetModelData (elem_data,
                                           QMESH_MODEL_GENERIC_ONOFF);

            if (mdl_data == NULL)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                                  "Model data is NULL for OnOff model \n");
                break;
            }

            /* Call Generic On/Off Server Handler */
            return QmeshGenericOnOffServerHandler (nw_hdr, key_info, elem_data,
                                                   mdl_data, msg, msg_len);
        }
        break;

        case QMESH_GENERIC_DEFAULT_TRANSITION_TIME_GET:
        case QMESH_GENERIC_DEFAULT_TRANSITION_TIME_SET:
        case QMESH_GENERIC_DEFAULT_TRANSITION_TIME_SET_UNRELIABLE:
        {
            /* Get the model data for the GDTT model from the element data */
            QMESH_MODEL_DATA_T *mdl_data = QmeshGetModelData (elem_data,
                                           QMESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME);

            if (mdl_data == NULL)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                                  "Model data is NULL for OnOff model \n");
                break;
            }

            return QmeshGenericDTTServerHandler (nw_hdr, key_info, elem_data,
                                                 mdl_data, msg, msg_len);
        }
        break;

        case QMESH_GENERIC_POWERONOFF_GET:
        {
            /* Get the model data for the PowerOnOff model from the element data */
            QMESH_MODEL_DATA_T *mdl_data = QmeshGetModelData (elem_data,
                                           QMESH_MODEL_GENERIC_POWERONOFF);

            if (mdl_data == NULL)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                                  "Model data is NULL for PowerOnOff model \n");
                break;
            }

            return QmeshGenericPowerOnOffServerHandler (nw_hdr, key_info, elem_data,
                                                 mdl_data, msg, msg_len);
        }
        break;
    }

    QmeshMemSet(&msg_cmn, 0, sizeof(msg_cmn));

    msg_cmn.opcode = opcode;
    msg_cmn.src = nw_hdr->src_addr;
    msg_cmn.dst = nw_hdr->dst_addr;
    QmeshMemCpy (&model_context->key_info, key_info,
                 sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));

    /* If the message is the last processed then ignore the message */
    if (model_context->last_msg_seq_no == nw_hdr->seq_num &&
        model_context->last_src_addr == nw_hdr->src_addr)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s: seq num 0x%08x processed\n", __FUNCTION__, nw_hdr->seq_num);
        return QMESH_RESULT_SUCCESS;
    }

    /* Store the previous seqence number and source address */
    seq_no = model_context->last_msg_seq_no;
    src_addr = model_context->last_src_addr;
    /* Store the sequence number and src address of the new message */
    model_context->last_msg_seq_no = nw_hdr->seq_num;
    model_context->last_src_addr = nw_hdr->src_addr;
    msg += QMESH_OPCODE_FORMAT_TWO_BYTE;
    msg_len -= QMESH_OPCODE_FORMAT_TWO_BYTE;

    switch (opcode)
    {
        case QMESH_GENERIC_POWERONOFF_SET_UNRELIABLE:
        case QMESH_GENERIC_POWERONOFF_SET:
        {
            if (msg_len != QMESH_GENERIC_ONPOWERUP_MSG_LEN)
                return QMESH_RESULT_SUCCESS;

            /* Valid OnPowerUp state values are 0x0 ~ 0x2 */
            if (msg[0] <= GENERIC_POWERUP_STATE_VALUE_MAX)
            {
                model_context->onpowerup = msg[0];

                /* Update NVM */
                QmeshGenericPowerOnOffUpdateNvm(model_context);

                /* Send the status response back */
                if (opcode == QMESH_GENERIC_POWERONOFF_SET)
                    QmeshGenericPowerOnOffStatusSend (model_context, &msg_cmn);

                /* Delete Publish timer if running */
                if (model_context ->publish_timer != QMESH_TIMER_INVALID_HANDLE)
                    QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

                /* Publish the 'status' message */
                if (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS)
                    QmeshGenericPowerOnOffStatusPublish (model_context);

                /* If publish time is valid (non-zero) start the publish timer */
                if (model_context->publish_interval != 0 &&
                    model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
                    ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   QmeshGenericPowerOnOffPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
                {
                    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                                      "Publish Timer Creation Failed \n");
                }

                QmeshUpdateBoundStates (0,
                                        QAPP_ONPOWERUP_UPDATED,
                                        (void *)&model_context->onpowerup);
            }
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

    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerOnOffSetupInit
 *
 *  DESCRIPTION
 *      This function initialises Generic Power OnOff Setup sever model
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffSetupInit (QMESH_GEN_POWERONOFF_DATA_T *model_data)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                      "%s\n", __FUNCTION__);
    /* Restoring context from NVM needs to be added - after NVM support */
}
