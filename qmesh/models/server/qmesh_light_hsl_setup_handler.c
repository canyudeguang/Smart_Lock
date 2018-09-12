/******************************************************************************
 *  Copyright 2017-2018 Qualcomm Technologies International, Ltd.
 ******************************************************************************/
/*! \file qmesh_light_hsl_setup_handler.c
 *  \brief defines and functions for SIGMesh Light HSL setup model.
 */
/******************************************************************************/
#include "qmesh_model_common.h"
#include "qmesh_model_debug.h"
#include "qmesh_light_lightness_handler.h"
#include "qmesh_light_hsl_setup_handler.h"
#include "qmesh_generic_onoff_handler.h"
#include "qmesh_generic_level_handler.h"
#include "qmesh_generic_poweronoff_handler.h"
#include "qmesh_generic_poweronoff_setup_handler.h"
#include "qmesh_generic_default_transition_time_handler.h"
#include "qmesh_model_nvm.h"


/*============================================================================*
 *  Private Data
 *===========================================================================*/

/* Lightness Status report buffer */
uint8_t light_hsl_setup_status[LIGHT_HSL_SETUP_STATUS_SIZE];

/*============================================================================*
 *  Private Function Implementations
 *===========================================================================*/
/*----------------------------------------------------------------------------*
*  NAME
*      qmeshSendLightHSLSetupServerStatusUpdate
*
*  DESCRIPTION
*      Send Present Light HSL status message information.
*
*  RETURNS/MODIFIES
*      None
*
*----------------------------------------------------------------------------*/
static void qmeshSendLightHSLSetupServerStatusUpdate (uint16_t elem_id,
        uint16_t src_addr,
        uint16_t opcode,
        const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
        QMESH_LIGHT_HSL_MODEL_CONTEXT_T *context)
{
    QMESH_ACCESS_PAYLOAD_INFO_T status_data_info;
    /* Fill in the response payload */
    status_data_info.ttl = QMESH_MODEL_DEFAULT_TTL;
    status_data_info.src_addr = elem_id;
    status_data_info.dst_addr = src_addr;
    status_data_info.trans_ack_required = FALSE;
    status_data_info.trans_mic_size = QMESH_MIC_4_BYTES;

    switch (opcode)
    {
        case QMESH_LIGHT_HSL_DEFAULT_SET:
        case QMESH_LIGHT_HSL_DEFAULT_GET:
        {
            /* Construct the payload data in the little endian format */
            uint16_t light_def = 0;
            QmeshUpdateBoundStates (context->elm_id, QAPP_LIGHT_HSL_GET_LIGHT_DEFAULT,
                                    &light_def);
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                              "Hue def = %d ,Sat def = %d light_def = %d\n", \
                              context->light_hsl_hue_def, context->light_hsl_sat_def, light_def);
            /* Construct the opcode data in the big endian format */
            light_hsl_setup_status[0] = (uint8_t) ((QMESH_LIGHT_HSL_DEFAULT_STATUS & 0xFF00)
                                                 >> 8);
            light_hsl_setup_status[1] = (uint8_t) (QMESH_LIGHT_HSL_DEFAULT_STATUS & 0x00FF);
            /* Construct the payload data in the little endian format */
            light_hsl_setup_status[2] = (uint8_t) (light_def & 0x00FF);
            light_hsl_setup_status[3] = (uint8_t) ((light_def & 0xFF00) >> 8);
            light_hsl_setup_status[4] = (uint8_t) (context->light_hsl_hue_def & 0x00FF);
            light_hsl_setup_status[5] = (uint8_t) ((context->light_hsl_hue_def & 0xFF00) >>
                                                 8);
            light_hsl_setup_status[6] = (uint8_t) (context->light_hsl_sat_def & 0x00FF);
            light_hsl_setup_status[7] = (uint8_t) ((context->light_hsl_sat_def & 0xFF00) >>
                                                 8);

            status_data_info.payload_len = LIGHT_HSL_SETUP_DEFAULT_STATUS_SIZE;
            status_data_info.payload = light_hsl_setup_status;
        }
        break;

        case QMESH_LIGHT_HSL_RANGE_SET:
        case QMESH_LIGHT_HSL_RANGE_GET:
        {
            /* Construct the opcode data in the big endian format */
            light_hsl_setup_status[0] = (uint8_t) ((QMESH_LIGHT_HSL_RANGE_STATUS & 0xFF00) >>
                                                 8);
            light_hsl_setup_status[1] = (uint8_t) (QMESH_LIGHT_HSL_RANGE_STATUS & 0x00FF);
            light_hsl_setup_status[2] = (uint8_t) (context->light_range_status);
            /* Construct the payload data in the little endian format */
            light_hsl_setup_status[3] = (uint8_t) (context->light_hsl_hue_min & 0x00FF);
            light_hsl_setup_status[4] = (uint8_t) ((context->light_hsl_hue_min & 0xFF00) >>
                                                 8);
            light_hsl_setup_status[5] = (uint8_t) (context->light_hsl_hue_max & 0x00FF);
            light_hsl_setup_status[6] = (uint8_t) ((context->light_hsl_hue_max & 0xFF00) >>
                                                 8);
            light_hsl_setup_status[7] = (uint8_t) (context->light_hsl_sat_min & 0x00FF);
            light_hsl_setup_status[8] = (uint8_t) ((context->light_hsl_sat_min & 0xFF00) >>
                                                 8);
            light_hsl_setup_status[9] = (uint8_t) (context->light_hsl_sat_max & 0x00FF);
            light_hsl_setup_status[10] = (uint8_t) ((context->light_hsl_sat_max & 0xFF00) >>
                                                  8);
            status_data_info.payload_len = LIGHT_HSL_SETUP_STATUS_SIZE;
            status_data_info.payload = light_hsl_setup_status;
        }
        break;

        default:
            break;
    }

    /* Send the status back */
    QmeshSendMessage (key_info, &status_data_info);
}

/*============================================================================*
 *  Public Function Implementations
 *===========================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHSLSetupServerHandler
 *
 *  DESCRIPTION
 *      Light HSL Setup server event handler.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLSetupServerHandler (const QMESH_NW_HEADER_INFO_T
        *nw_hdr,
        const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
        const QMESH_ELEMENT_CONFIG_T *elem_data,
        QMESH_MODEL_DATA_T *model_data,
        const uint8_t *msg,
        uint16_t msg_len)
{
    uint16_t opcode;
    QMESH_LIGHT_HSL_MODEL_CONTEXT_T *model_context =
        (QMESH_LIGHT_HSL_MODEL_CONTEXT_T *) \
        (model_data->model_priv_data);

    if ((model_context == NULL) || (!msg || msg_len <= 0))
    {
        return QMESH_RESULT_FAILURE;
    }

      /* If the message is the last processed then ignore the message */
    if (model_context->last_msg_seq_no == nw_hdr->seq_num &&
        model_context->last_src_addr == nw_hdr->src_addr)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "%s: seq num 0x%08x processed\n",
                          __FUNCTION__,
                          nw_hdr->seq_num);
        return QMESH_RESULT_SUCCESS;
    }

    /* Retrieve the opcode from the message */
    if (get_opcode_format (msg[MODEL_OPCODE_OFFSET]) ==
        QMESH_OPCODE_FORMAT_TWO_BYTE)
    {
        opcode = QMESH_PACK_TWO_BYTE_OPCODE (msg[0], msg[1]);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s: Opcode:0x%x\n",
                          __FUNCTION__, opcode);
    }
    else
    {
        return QMESH_RESULT_FAILURE;
    }

    /* Handle messages */
    switch (opcode)
    {
        case QMESH_LIGHT_HSL_DEFAULT_GET:
        case QMESH_LIGHT_HSL_RANGE_GET:
        {
           /* Store the sequence number and src address of the new message */
           model_context->last_msg_seq_no = nw_hdr->seq_num;
           model_context->last_src_addr = nw_hdr->src_addr;

           qmeshSendLightHSLSetupServerStatusUpdate (model_context->elm_id, 
                                                     nw_hdr->src_addr,
                                                     opcode,
                                                     key_info,
                                                     model_context);
            return QMESH_RESULT_SUCCESS;
        }
        break;
        case QMESH_LIGHT_HSL_DEFAULT_SET:
        case QMESH_LIGHT_HSL_DEFAULT_SET_UNRELIABLE:
        {
           /* Store the sequence number and src address of the new message */
           model_context->last_msg_seq_no = nw_hdr->seq_num;
           model_context->last_src_addr = nw_hdr->src_addr;

            msg += QMESH_OPCODE_FORMAT_TWO_BYTE;
            msg_len -= QMESH_OPCODE_FORMAT_TWO_BYTE;
            uint16_t light_def = ((msg[OFFSET_VALUE_BYTE_MSB] << 8) |
                                msg[OFFSET_VALUE_BYTE_LSB]);
            /* Update Bound States */
            QmeshUpdateBoundStates (model_context->elm_id,
                                    QAPP_LIGHT_HSL_UPDATE_LIGHT_DEFAULT, &light_def);
            model_context->light_hsl_hue_def = ((msg[OFFSET_VALUE_BYTE_MSB + 2] << 8) |
                                                msg[OFFSET_VALUE_BYTE_LSB + 2]);
            model_context->light_hsl_sat_def = ((msg[OFFSET_VALUE_BYTE_MSB + 4] << 8) |
                                                msg[OFFSET_VALUE_BYTE_LSB + 4]);
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                              "Hue def = %d ,Sat def = %d light_def = %d\n", \
                              model_context->light_hsl_hue_def, model_context->light_hsl_sat_def, light_def);

            if(opcode != QMESH_LIGHT_HSL_DEFAULT_SET_UNRELIABLE)
            {
                 qmeshSendLightHSLSetupServerStatusUpdate (nw_hdr->dst_addr, nw_hdr->src_addr,
                    opcode, key_info, model_context);
            }
            return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_LIGHT_HSL_RANGE_SET:
        case QMESH_LIGHT_HSL_RANGE_SET_UNRELIABLE:
        {
           /* Store the sequence number and src address of the new message */
           model_context->last_msg_seq_no = nw_hdr->seq_num;
           model_context->last_src_addr = nw_hdr->src_addr;

            QMESH_ACCESS_PAYLOAD_INFO_T status_data_info;
            /* Fill in the response payload */
            status_data_info.ttl = QMESH_MODEL_DEFAULT_TTL;
            status_data_info.src_addr = model_context->elm_id;
            status_data_info.dst_addr = nw_hdr->src_addr;
            status_data_info.trans_ack_required = FALSE;
            status_data_info.trans_mic_size = QMESH_MIC_4_BYTES;
            msg += QMESH_OPCODE_FORMAT_TWO_BYTE;
            msg_len -= QMESH_OPCODE_FORMAT_TWO_BYTE;
            uint16_t hue_min_range = ((msg[OFFSET_VALUE_BYTE_MSB] << 8) |
                                    msg[OFFSET_VALUE_BYTE_LSB]);
            uint16_t hue_max_range = ((msg[OFFSET_VALUE_BYTE_MSB + 2] << 8) |
                                    msg[OFFSET_VALUE_BYTE_LSB + 2]);
            uint16_t sat_min_range = ((msg[OFFSET_VALUE_BYTE_MSB + 4] << 8) |
                                    msg[OFFSET_VALUE_BYTE_LSB + 4]);
            uint16_t sat_max_range = ((msg[OFFSET_VALUE_BYTE_MSB + 6] << 8) |
                                    msg[OFFSET_VALUE_BYTE_LSB + 6]);
            uint8_t error = LIGHTNESS_RANGE_SET_SUCCESS;

            if (hue_min_range > hue_max_range)
            {
                error = LIGHT_HSL_SETUP_CANNOT_SET_RANGE_MIN;
            }
            else if (hue_max_range < hue_min_range)
            {
                error = LIGHT_HSL_SETUP_CANNOT_SET_RANGE_MAX;
            }
            else if (sat_min_range > sat_max_range)
            {
                error = LIGHT_HSL_SETUP_CANNOT_SET_RANGE_MIN;
            }
            else if (sat_max_range < sat_min_range)
            {
                error = LIGHT_HSL_SETUP_CANNOT_SET_RANGE_MAX;
            }

            /* Construct the opcode data in the big endian format */
            light_hsl_setup_status[0] = (uint8_t) ((QMESH_LIGHT_HSL_RANGE_STATUS & 0xFF00) >>
                                                 8);
            light_hsl_setup_status[1] = (uint8_t) (QMESH_LIGHT_HSL_RANGE_STATUS & 0x00FF);
            light_hsl_setup_status[2] = (uint8_t) (error);
            /* Construct the payload data in the little endian format */
            light_hsl_setup_status[3] = (uint8_t) (hue_min_range & 0x00FF);
            light_hsl_setup_status[4] = (uint8_t) ((hue_min_range & 0xFF00) >> 8);
            light_hsl_setup_status[5] = (uint8_t) (hue_max_range & 0x00FF);
            light_hsl_setup_status[6] = (uint8_t) ((hue_max_range & 0xFF00) >> 8);
            light_hsl_setup_status[7] = (uint8_t) (sat_min_range & 0x00FF);
            light_hsl_setup_status[8] = (uint8_t) ((sat_min_range & 0xFF00) >> 8);
            light_hsl_setup_status[9] = (uint8_t) (sat_max_range & 0x00FF);
            light_hsl_setup_status[10] = (uint8_t) ((sat_max_range & 0xFF00) >> 8);
            status_data_info.payload_len = LIGHT_HSL_SETUP_STATUS_SIZE;
            status_data_info.payload = light_hsl_setup_status;

            /* When the min and max values are correct however if the device (hardware) doesn't support
             * this range values return an error message. If the provided range is invalid, do not send 
             * any response back.
             */
            if (opcode != QMESH_LIGHT_HSL_RANGE_SET_UNRELIABLE && error == LIGHTNESS_RANGE_SET_SUCCESS)
                QmeshSendMessage (key_info, &status_data_info);

            if (error == LIGHTNESS_RANGE_SET_SUCCESS)
            {
                model_context->light_hsl_hue_min = hue_min_range;
                model_context->light_hsl_hue_max = hue_max_range;
                model_context->light_hsl_sat_min = sat_min_range;
                model_context->light_hsl_sat_max = sat_max_range;
            }

            return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_LIGHT_LIGHTNESS_SET: /*Lightness Model Messages */
        case QMESH_LIGHT_LIGHTNESS_GET:
        case QMESH_LIGHT_LIGHTNESS_SET_UNRELIABLE:
        case QMESH_LIGHT_LIGHTNESS_LAST_GET:
        case QMESH_LIGHT_LIGHTNESS_DEFAULT_GET:
        case QMESH_LIGHT_LIGHTNESS_RANGE_GET:
        case QMESH_LIGHT_LIGHTNESS_RANGE_SET:
        {
            QMESH_MODEL_DATA_T *p_model_data = QmeshGetModelData (elem_data,
                                               QMESH_MODEL_LIGHT_LIGHTNESS);

            if (p_model_data != NULL)
            {
                QmeshLightnessServerHandler (nw_hdr,
                                             key_info,
                                             elem_data,
                                             p_model_data,
                                             msg,
                                             msg_len);
            }
            else
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "Model data for PowerOnOff model is NULL\n");
            }

            return QMESH_RESULT_SUCCESS;
        }
        break;
        case QMESH_GENERIC_POWERONOFF_GET: /* Generic Poweronoff Messages */
        {
            QMESH_MODEL_DATA_T *p_model_data = QmeshGetModelData (elem_data,
                                               QMESH_MODEL_GENERIC_POWERONOFF);

            if (p_model_data != NULL)
            {
                QmeshGenericPowerOnOffServerHandler (nw_hdr,
                                                     key_info,
                                                     elem_data,
                                                     p_model_data,
                                                     msg,
                                                     msg_len);
            }
            else
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "Model data for PowerOnOff model is NULL\n");
            }

            return QMESH_RESULT_SUCCESS;
        }

        case QMESH_GENERIC_POWERONOFF_SET:
        case QMESH_GENERIC_POWERONOFF_SET_UNRELIABLE:
        {
            QMESH_MODEL_DATA_T *p_model_data = QmeshGetModelData (elem_data,
                                               QMESH_MODEL_GENERIC_POWERONOFFSETUPSERVER);

            if (p_model_data != NULL)
            {
                QmeshGenericPowerOnOffSetupServerHandler (nw_hdr,
                                                     key_info,
                                                     elem_data,
                                                     p_model_data,
                                                     msg,
                                                     msg_len);
            }
            else
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "Model data for PowerOnOff model is NULL\n");
            }

            return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_GENERIC_ONOFF_GET: /* Generic OnOff messages */
        case QMESH_GENERIC_ONOFF_SET:
        case QMESH_GENERIC_ONOFF_SET_UNRELIABLE:
        {
            QMESH_MODEL_DATA_T *p_model_data = QmeshGetModelData (elem_data,
                                               QMESH_MODEL_GENERIC_ONOFF);

            if (p_model_data != NULL)
            {
                QmeshGenericOnOffServerHandler (nw_hdr,
                                                key_info,
                                                elem_data,
                                                p_model_data,
                                                msg,
                                                msg_len);
            }
            else
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "Model data for PowerOnOff model is NULL\n");
            }

            return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_GENERIC_DEFAULT_TRANSITION_TIME_GET: /* Generic DTT messages */
        case QMESH_GENERIC_DEFAULT_TRANSITION_TIME_SET:
        case QMESH_GENERIC_DEFAULT_TRANSITION_TIME_SET_UNRELIABLE:
        {
            QMESH_MODEL_DATA_T *p_model_data = QmeshGetModelData (elem_data,
                                               QMESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME);

            if (p_model_data != NULL)
            {
                QmeshGenericDTTServerHandler (nw_hdr,
                                              key_info,
                                              elem_data,
                                              p_model_data,
                                              msg,
                                              msg_len);
            }
            else
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "Model data for DTT model is NULL\n");
            }
            return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_GENERIC_LEVEL_GET: /* Generic Level Messages */
        case QMESH_GENERIC_LEVEL_SET:
        case QMESH_GENERIC_LEVEL_SET_UNRELIABLE:
        case QMESH_GENERIC_DELTA_SET:
        case QMESH_GENERIC_DELTA_SET_UNRELIABLE:
        case QMESH_GENERIC_MOVE_SET:
        case QMESH_GENERIC_MOVE_SET_UNRELIABLE:
        {
            QMESH_MODEL_DATA_T *p_model_data = QmeshGetModelData (elem_data,
                                               QMESH_MODEL_GENERIC_LEVEL);

            if (p_model_data != NULL)
            {
                QmeshGenericLevelServerHandler (nw_hdr,
                                                key_info,
                                                elem_data,
                                                p_model_data,
                                                msg,
                                                msg_len);
            }
            else
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "Model data for PowerOnOff model is NULL\n");
            }

            return QMESH_RESULT_SUCCESS;
        }
        break;

        default:
        {
            return QMESH_RESULT_FAILURE;
        }
        break;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHSLSetupServerAppInit
 *
 *  DESCRIPTION
 *      Application calls this API to initialize Light HSL Setup Server.
 *
 *  RETURNS/MODIFIES
 *      returns QMESH_RESULT_SUCCESS
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLSetupServerAppInit()
{
    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHSLSetupServerAppDeInit
 *
 *  DESCRIPTION
 *      Application calls this API to de-initialize Light HSL Setup Server.
 *
 *  RETURNS/MODIFIES
 *      returns QMESH_RESULT_SUCCESS
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLSetupServerAppDeInit()
{
    return QMESH_RESULT_SUCCESS;
}