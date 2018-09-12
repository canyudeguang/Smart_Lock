/******************************************************************************
 *  Copyright 2017-2018 Qualcomm Technologies International, Ltd.
 ******************************************************************************/
/*! \file  qmesh_light_lightness_setupmodel.c
 *  \brief defines and functions for SIGmesh Light Lightness Setup model
 *         interfacing with the stack
 */
/******************************************************************************/


#include "qmesh_model_common.h"
#include "qmesh_light_lightness_setup_handler.h"
#include "qmesh_light_lightness_handler.h"
#include "qmesh_generic_onoff_handler.h"
#include "qmesh_generic_level_handler.h"
#include "qmesh_generic_poweronoff_handler.h"
#include "qmesh_generic_poweronoff_setup_handler.h"
#include "qmesh_generic_default_transition_time_handler.h"
#include "qmesh_model_nvm.h"

/* Lightness Status report buffer */
uint8_t lightness_setup_status[LIGHTNESS_STATUS_TRANSITION_REPORT_SIZE];

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessSetupServerHandler
 *
 *  DESCRIPTION
 *      Generic PowerOnOffSetup server event handler.
 *
 *  RETURNS/MODIFIES
 *      returns QMESH_RESULT_SUCCESS if message is handled sussefully else other values as mentioned in
 *      'QMESH_RESULT_T' appropriately.
 *
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T QmeshLightnessSetupServerHandler (const QMESH_NW_HEADER_INFO_T
        *nw_hdr,
        const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
        const QMESH_ELEMENT_CONFIG_T *elem_data,
        QMESH_MODEL_DATA_T *model_data,
        const uint8_t *msg,
        uint16_t msg_len)
{
    uint16_t opcode;
    uint16_t len;

    QMESH_LIGHTNESS_MODEL_CONTEXT_T *lightness_context =
       (QMESH_LIGHTNESS_MODEL_CONTEXT_T *) \
       (model_data->model_priv_data);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                              "Lighntess Setup Msg Recv \n");

    if ((lightness_context == NULL) || (!msg || msg_len <= 0))
    {
         DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                              "Lighntesssetup Context or msg len invalid \n");
        return QMESH_RESULT_FAILURE;
    }

    /* If the message is the last processed then ignore the message */
    if (lightness_context->last_msg_seq_no == nw_hdr->seq_num &&
        lightness_context->last_src_addr == nw_hdr->src_addr)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                              "Lighntesssetup dup message \n");
        return QMESH_RESULT_SUCCESS;
    }

    /* Validate if the opcode has the correct length */
    if ((get_opcode_format (msg[MODEL_OPCODE_OFFSET]) ==
         QMESH_OPCODE_FORMAT_TWO_BYTE))
    {
        opcode = QMESH_PACK_TWO_BYTE_OPCODE (msg[0], msg[1]);
    }
    else
    {
          DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                              "Lighntesssetup invalid opcode \n");
        return QMESH_RESULT_FAILURE;
    }

    switch (opcode)
    {
        /* Validate message length on lightness messages */
        case QMESH_LIGHT_LIGHTNESS_GET:
        case QMESH_LIGHT_LIGHTNESS_DEFAULT_GET:
        case QMESH_LIGHT_LIGHTNESS_RANGE_GET:
        case QMESH_LIGHT_LIGHTNESS_LAST_GET:
            break;

        case QMESH_LIGHT_LIGHTNESS_SET:
        case QMESH_LIGHT_LIGHTNESS_SET_UNRELIABLE:
        {
            len = msg_len - QMESH_OPCODE_FORMAT_TWO_BYTE;

            if (! (len == LIGHTNESS_SET_NON_TRANS_MSG_LEN ||
                   len == LIGHTNESS_SET_TRANS_MSG_LEN))
                return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_LIGHT_LIGHTNESS_DEFAULT_SET:
        case QMESH_LIGHT_LIGHTNESS_DEFAULT_SET_UNRELIABLE:
        {
            /* Store the sequence number and src address of the new message */
            lightness_context->last_msg_seq_no = nw_hdr->seq_num;
            lightness_context->last_src_addr = nw_hdr->src_addr;

            msg += QMESH_OPCODE_FORMAT_TWO_BYTE;
            msg_len -= QMESH_OPCODE_FORMAT_TWO_BYTE;

            if (! (msg_len == LIGHTNESS_DEFAULT_SET_MSG_LEN))
            {
                 DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                              "Lighntesssetup invalid msg len \n");
                return QMESH_RESULT_SUCCESS;
            }

            QmeshMutexLock (&lightness_context->mutex_handle);
            /* Update Lightness Default */
            lightness_context->lightness_default = ((msg[OFFSET_VALUE_BYTE_MSB] << 8) \
                                                    | msg[OFFSET_VALUE_BYTE_LSB]);
            QmeshMutexUnlock (&lightness_context->mutex_handle);

            /* Send response back for reliable message */
            if (opcode == QMESH_LIGHT_LIGHTNESS_DEFAULT_SET)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                              "Lighntesssetup def status update send \n");
                QmeshSendLightnessStatusUpdate (lightness_context->elm_id, 
                                                nw_hdr->src_addr, 
                                                opcode,
                                                key_info, 
                                                lightness_context);
            }

            return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_LIGHT_LIGHTNESS_RANGE_SET:
        case QMESH_LIGHT_LIGHTNESS_RANGE_SET_UNRELIABLE:
        {
             /* Store the sequence number and src address of the new message */
            lightness_context->last_msg_seq_no = nw_hdr->seq_num;
            lightness_context->last_src_addr = nw_hdr->src_addr;

            msg += QMESH_OPCODE_FORMAT_TWO_BYTE;
            msg_len -= QMESH_OPCODE_FORMAT_TWO_BYTE;

            if (! (msg_len == LIGHTNESS_RANGE_SET_MSG_LEN))
                return QMESH_RESULT_SUCCESS;

            QmeshMutexLock (&lightness_context->mutex_handle);
            uint16_t min_range = ((msg[OFFSET_VALUE_BYTE_MSB] << 8) |
                                msg[OFFSET_VALUE_BYTE_LSB]);
            uint16_t max_range = ((msg[OFFSET_VALUE_BYTE_MSB + 2] << 8) |
                                msg[OFFSET_VALUE_BYTE_LSB + 2]);
            uint8_t error = LIGHTNESS_RANGE_SET_SUCCESS;

            /* Update lightness range - check for invalid range values*/
            if ( (min_range > 0 ) && (min_range < max_range))
            {
                lightness_context->lightness_min_range = min_range;
            }
            else
            {
                error = LIGHTNESS_CANNOT_SET_RANGE_MIN;
            }

            if ( (max_range > 0) && (max_range > min_range))
            {
                lightness_context->lightness_max_range = max_range;
            }
            else
            {
                error = LIGHTNESS_CANNOT_SET_RANGE_MAX;
            }

            QmeshMutexUnlock (&lightness_context->mutex_handle);

            /* Send response back for a reliable set */
            if (opcode == QMESH_LIGHT_LIGHTNESS_RANGE_SET)
            {
                QMESH_ACCESS_PAYLOAD_INFO_T status_data_info;
                /* Fill in the response payload */
                status_data_info.ttl = QMESH_MODEL_DEFAULT_TTL;
                status_data_info.src_addr = lightness_context->elm_id;
                status_data_info.dst_addr = nw_hdr->src_addr;
                status_data_info.trans_ack_required = FALSE;
                status_data_info.trans_mic_size = QMESH_MIC_4_BYTES;
                /* Construct the opcode data in the big endian format */
                lightness_setup_status[0] = (uint8_t) ((QMESH_LIGHT_LIGHTNESS_RANGE_STATUS & 0xFF00) >>
                                               8);
                lightness_setup_status[1] = (uint8_t) (QMESH_LIGHT_LIGHTNESS_RANGE_STATUS & 0x00FF);
                /* Construct the payload data in the little endian format */
                lightness_setup_status[2] = error;

                if (error)
                {
                    status_data_info.payload_len = LIGHTNESS_STATUS_REPORT_SIZE + 1;
                    lightness_setup_status[3] = (uint8_t)(min_range & 0x00FF);
                    lightness_setup_status[4] = (uint8_t)((min_range & 0xFF00) >> 8);
                    lightness_setup_status[5] = (uint8_t)(max_range & 0x00FF);
                    lightness_setup_status[6] = (uint8_t)((max_range & 0xFF00) >> 8);
                }
                else
                {
                    lightness_setup_status[3] = (uint8_t) (lightness_context->lightness_min_range & 0x00FF);
                    lightness_setup_status[4] = (uint8_t) ((lightness_context->lightness_min_range & 0xFF00)
                                                   >> 8);
                    lightness_setup_status[5] = (uint8_t) (lightness_context->lightness_max_range & 0x00FF);
                    lightness_setup_status[6] = (uint8_t) ((lightness_context->lightness_max_range & 0xFF00)
                                                   >> 8);
                    status_data_info.payload_len = LIGHTNESS_STATUS_REPORT_SIZE + 3;
                }

                status_data_info.payload = lightness_setup_status;
                /* Send the status back */
                QmeshSendMessage (key_info, &status_data_info);
            }

            return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_GENERIC_POWERONOFF_GET:
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
        break;

        case QMESH_GENERIC_POWERONOFF_SET_UNRELIABLE:
        case QMESH_GENERIC_POWERONOFF_SET:
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

        case QMESH_GENERIC_ONOFF_GET:
        case QMESH_GENERIC_ONOFF_SET:
        case QMESH_GENERIC_ONOFF_SET_UNRELIABLE:
        {
            QMESH_MODEL_DATA_T *p_model_data = QmeshGetModelData (elem_data,
                                               QMESH_MODEL_GENERIC_ONOFF);

            /* Call generic on/off server handler */
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
                                  "Model data for OnOff model is NULL\n");
            }

            return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_GENERIC_DEFAULT_TRANSITION_TIME_GET:
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
                                  "Model data for GDTT model is NULL\n");
            }

            return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_GENERIC_LEVEL_GET:
        case QMESH_GENERIC_LEVEL_SET:
        case QMESH_GENERIC_LEVEL_SET_UNRELIABLE:
        case QMESH_GENERIC_DELTA_SET:
        case QMESH_GENERIC_DELTA_SET_UNRELIABLE:
        case QMESH_GENERIC_MOVE_SET:
        case QMESH_GENERIC_MOVE_SET_UNRELIABLE:
        {
            QMESH_MODEL_DATA_T *p_model_data = QmeshGetModelData (elem_data,
                                               QMESH_MODEL_GENERIC_LEVEL);

            /* Call Level server handler */
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
                                  "Model data for Level  model is NULL\n");
            }

            return QMESH_RESULT_SUCCESS;
        }
        break;

        default:
        {
            /* Return failure as we are not handling this unknown event */
            return QMESH_RESULT_FAILURE;
        }
        break;
    }

    QmeshLightnessServerHandler (nw_hdr,
                                 key_info,
                                 elem_data,
                                 model_data,
                                 msg,
                                 msg_len);
    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessSetupServerAppInit
 *
 *  DESCRIPTION
 *      Application calls this API initialize Lightness Setup Server
 *
 *  RETURNS/MODIFIES
 *      returns QMESH_RESULT_SUCCESS
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightnessSetupServerAppInit()
{
    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessSetupServerAppDeInit
 *
 *  DESCRIPTION
 *      Application calls this API to de-initialize Lightness Setup Server
 *
 *  RETURNS/MODIFIES
 *      returns QMESH_RESULT_SUCCESS
 *----------------------------------------------------------------------------*/
extern void QmeshLightnessSetupServerAppDeInit (void)
{
}
