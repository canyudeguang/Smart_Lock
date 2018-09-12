/******************************************************************************
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_model_generic_powerlevel_setup_handler.c
 *  \brief defines and functions for SIGmesh Generic PowerLevel Setup Server model.
 *
 *   This file contains the implementation of Generic PowerLevel Setup Server model.
 *  This file should be built as part of the model Application.
 */
/******************************************************************************/

#include "qmesh_hal_ifce.h"
#include "qmesh_types.h"
#include "qmesh_model_common.h"
#include "qmesh_model_debug.h"

#include "qmesh_model_config.h"
#include "qmesh_cache_mgmt.h"
#include "qmesh_generic_powerlevel_setup_handler.h"
#include "qmesh_model_nvm.h"

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelSetupServerHandler
 *
 *  DESCRIPTION
 *      This method is the Application model callback.
 *  This API will be registered with the model library (via GenericPowerLevelServerSetupAppInit()). This API
 *  gets called from the model library whenever mesh stack sends the model message.
 *  This API will also be called from other extended models which has bound state with this model.
 *
 *  RETURNS/MODIFIES
 *      returns SUCCESS if message is handled sucessfully else FALSE
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericPowerLevelSetupServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len)
{
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context = NULL;
    uint16_t current_time[3];
    QMESH_MODEL_MSG_COMMON_T msg_cmn;
    uint32_t opcode, seq_no;
    uint16_t src_addr;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s msg_len=%d\n",
                      __FUNCTION__, msg_len);
    /* Get the current time in ms */
    QmeshGetCurrentTimeInMs (current_time);
    /* Get the model data for the element with address 'elm_addr' */
    model_context = (QMESH_GENERIC_POWERLEVEL_CONTEXT_T *)
                    model_data->model_priv_data;

    if ((model_context == NULL) ||
        (!msg || msg_len <= 0))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                          "Context data is NULL or msg is invalid \n");
        return QMESH_RESULT_FAILURE;
    }

    /* Retrieve the opcode from the message */
    if (get_opcode_format (msg[MODEL_OPCODE_OFFSET]) ==
        QMESH_OPCODE_FORMAT_TWO_BYTE)
    {
        opcode = QMESH_PACK_TWO_BYTE_OPCODE (msg[0], msg[1]);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s: Opcode:0x%x\n",
                          __FUNCTION__, opcode);
    }
    else
    {
        /* Power Level server handles only opcode of 2 bytes in length */
        return QMESH_RESULT_FAILURE;
    }

    /* Handle the bound model messages */
    switch (opcode)
    {
        /* Handle Generic OnOff model messages */
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

        /* Handle Generic Level model messages */
        case QMESH_GENERIC_LEVEL_GET:
        case QMESH_GENERIC_LEVEL_SET:
        case QMESH_GENERIC_LEVEL_SET_UNRELIABLE:
        case QMESH_GENERIC_DELTA_SET:
        case QMESH_GENERIC_DELTA_SET_UNRELIABLE:
        case QMESH_GENERIC_MOVE_SET:
        case QMESH_GENERIC_MOVE_SET_UNRELIABLE:
        {
            /* Get the model data for the GDTT model from the element data */
            QMESH_MODEL_DATA_T *mdl_data = QmeshGetModelData (elem_data,
                                           QMESH_MODEL_GENERIC_LEVEL);

            if (mdl_data == NULL)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                                  "Model data is NULL for OnOff model \n");
                break;
            }

            return QmeshGenericLevelServerHandler (nw_hdr, key_info, elem_data, mdl_data,
                                                   msg, msg_len);
        }

        /* Handle Generic Default Transition Time messages*/
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

            return QmeshGenericDTTServerHandler (nw_hdr, key_info, elem_data, mdl_data,
                                                 msg, msg_len);
        }

        /* Handle Generic Power OnOff model messages*/
        case QMESH_GENERIC_POWERONOFF_GET:
        case QMESH_GENERIC_POWERONOFF_SET_UNRELIABLE:
        case QMESH_GENERIC_POWERONOFF_SET:
        {
            /* Get the model data for the GDTT model from the element data */
            QMESH_MODEL_DATA_T *mdl_data = QmeshGetModelData (elem_data,
                                           QMESH_MODEL_GENERIC_POWERONOFFSETUPSERVER);

            if (mdl_data == NULL)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                                  "Model data is NULL for OnOff model \n");
                break;
            }

            return QmeshGenericPowerOnOffServerHandler (nw_hdr, key_info, elem_data,
                    mdl_data,
                    msg, msg_len);
        }
    }

    if (model_context->last_msg_seq_no == nw_hdr->seq_num &&
        model_context->last_src_addr == nw_hdr->src_addr)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s: seq num 0x%08x processed\n", __FUNCTION__, nw_hdr->seq_num);
        return QMESH_RESULT_SUCCESS;
    }

    /* Pass PowerLevel messages to PowerLevel handler */
    switch(opcode)
    {
        case QMESH_GENERIC_POWER_LEVEL_GET:
        case QMESH_GENERIC_POWER_LEVEL_SET:
        case QMESH_GENERIC_POWER_LEVEL_SET_UNRELIABLE:
        case QMESH_GENERIC_POWER_LAST_GET:
        case QMESH_GENERIC_POWER_DEFAULT_GET:
        case QMESH_GENERIC_POWER_RANGE_GET:
        {
            /* Get the model data for the PowerLevel model from the element data */
            QMESH_MODEL_DATA_T *mdl_data = QmeshGetModelData (elem_data,
                                           QMESH_MODEL_GENERIC_POWERLEVEL_SERVER);

            if (mdl_data == NULL)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                                  "Model data is NULL for PowerLevel model \n");
                break;
            }

            return QmeshGenericPowerLevelServerHandler (nw_hdr, key_info, elem_data,
                    mdl_data,
                    msg, msg_len);
        }
    }

    QmeshMemSet(&msg_cmn, 0, sizeof(msg_cmn));

    /* Store the common message parameters */
    model_context->elm_addr = nw_hdr->src_addr;
    msg_cmn.opcode = opcode;
    msg_cmn.src = nw_hdr->src_addr;
    msg_cmn.dst = nw_hdr->dst_addr;
    QmeshMutexLock (&model_context->pwr_lvl_mutex);
    msg += QMESH_OPCODE_FORMAT_TWO_BYTE;
    msg_len -= QMESH_OPCODE_FORMAT_TWO_BYTE;
    QmeshMemCpy (&model_context->key_info, key_info,
                 sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));
    /* Store the previous seqence number and source address */
    seq_no = model_context->last_msg_seq_no;
    src_addr = model_context->last_src_addr;
    /* Store the sequence number and src address of the new message */
    model_context->last_msg_seq_no = nw_hdr->seq_num;
    model_context->last_src_addr = nw_hdr->src_addr;

    /* Handle Power Level Setup model the messages */
    switch (opcode)
    {
        case QMESH_GENERIC_POWER_DEFAULT_SET:
        case QMESH_GENERIC_POWER_DEFAULT_SET_UNRELIABLE:
        {
            if (msg_len != POWER_DEFAULT_SET_MSG_LEN)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s: dropping msg\n",
                                  __FUNCTION__);
                QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
                return QMESH_RESULT_SUCCESS;
            }

            /* Update the power default value */
            model_context->power_default =
                (uint16_t)QMESH_PACK_TWO_BYTE_OPCODE (msg[1], msg[0]);

            /* Update NVM */
            QmeshGenericPowerLevelUpdateNvm(model_context);

            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                              "%s: Updated power_default=0x%x\n", __FUNCTION__, model_context->power_default);
        }
        break;

        case QMESH_GENERIC_POWER_RANGE_SET:
        case QMESH_GENERIC_POWER_RANGE_SET_UNRELIABLE:
        {
            uint16_t min, max;

            if (msg_len != POWER_RANGE_SET_MSG_LEN)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s:dropping msg\n",
                                  __FUNCTION__);
                QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
                return QMESH_RESULT_SUCCESS;
            }

            min = (uint16_t)QMESH_PACK_TWO_BYTE_OPCODE (msg[1], msg[0]);
            max = (uint16_t)QMESH_PACK_TWO_BYTE_OPCODE (msg[3], msg[2]);
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                              "%s: Range Min=0x%x, Range Max=0x%x \n", __FUNCTION__, min, max);

            /* Range Min and Range Max should be none zero value */
            if (!min)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                                  "%s:Invaild Range Min=0x%x \n", __FUNCTION__, max);
                model_context->power_range.status_code
                    = QMESH_MODEL_STATUS_RANGE_MIN_SET_FAILURE;
                break;
            }

            /* Range Max value should be greater or equal to Range Min value */
            if (!max)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                                  "%s:Invaild Range Max=0x%x \n", __FUNCTION__, max);
                model_context->power_range.status_code
                    = QMESH_MODEL_STATUS_RANGE_MAX_SET_FAILURE;
                break;
            }

            if (max < min)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                                  "%s: Range Max < Range min \n", __FUNCTION__);
                QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
                return QMESH_RESULT_SUCCESS;
            }

            /* Update the power range max and min values */
            model_context->power_range.pwr_range_min = min;
            model_context->power_range.pwr_range_max = max;
            model_context->power_range.status_code = (uint8_t)QMESH_MODEL_STATUS_SUCCESS;

            /* Update NVM */
            QmeshGenericPowerLevelUpdateNvm(model_context);

            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                              "%s: Updated Range Min=0x%x, Range Max=0x%x, status=0x%x \n",
                              __FUNCTION__,
                              model_context->power_range.pwr_range_min,
                              model_context->power_range.pwr_range_max,
                              model_context->power_range.status_code);
        }
        break;

        default:
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s \n", __FUNCTION__);
            /* Retore the previous sequence number and src address - since the message is not processed */
            model_context->last_msg_seq_no = seq_no;
            model_context->last_src_addr = src_addr;
            QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
            return QMESH_RESULT_FAILURE;
        }
    }

    /* Send the status message */
    QmeshGenericPowerLevelStatusSend (model_context, &msg_cmn);

    if (opcode == QMESH_GENERIC_POWER_RANGE_SET_UNRELIABLE)
    {
        /* Reset the status code to success */
        model_context->power_range.status_code = (uint8_t)QMESH_MODEL_STATUS_SUCCESS;
    }

    QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelSetupServerAppInit
 *
 *  DESCRIPTION
 *      Initialise Generic Power Level Setup server model and registers callback with the mesh stack.
 *
 *  RETURNS/MODIFIES
 *      none
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericPowerLevelSetupServerAppInit (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,  "%s: \n", __FUNCTION__);
    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelSetupServerAppDeInit
 *
 *  DESCRIPTION
 *      This function De-initializes Generic PowerLevel Setup model state data and Unregisters the model
 * from the stack.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelSetupServerAppDeInit (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                      "GenericPowerLevelServerAppDeInit\n");
    QmeshGenericPowerLevelAbortTransition (model_context);
    QmeshMutexDestroy (&model_context->pwr_lvl_mutex);
}
