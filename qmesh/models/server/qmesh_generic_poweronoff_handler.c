/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
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
#include "qmesh_generic_poweronoff_handler.h"
#include "qmesh_model_nvm.h"


/*============================================================================*
 *  Private Data
 *===========================================================================*/

/*============================================================================*
*  Private Function Definitions
*============================================================================*/
static void qmeshGenericPowerOnOffPublishRetransTimerCb (
    QMESH_TIMER_HANDLE_T timerHandle,
    void *context);

/*----------------------------------------------------------------------------*
*  NAME
*      QmeshGenericOnOffPublishRetransTimerCb
*
*  DESCRIPTION
*      This function sends the OnOf status message to the publish address for
*  the OnOff model periodically every 'Publish_Period' until new tranistion
*  begins.
*
*  RETURNS/MODIFIES
*      None
*
*----------------------------------------------------------------------------*/
static void qmeshGenericPowerOnOffPublishRetransTimerCb (
    QMESH_TIMER_HANDLE_T timerHandle,
    void *context)
{
    /* Retrieve the context */
    QMESH_GEN_POWERONOFF_DATA_T *model_context =
        (QMESH_GEN_POWERONOFF_DATA_T *)context;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER, "******%s, cnt=0x%x:\n",
                      __FUNCTION__,
                      model_context->publish_restrans_cnt);
    model_context ->publish_timer = QMESH_TIMER_INVALID_HANDLE;
    /* Send the Status Message to Publish Address */
    QmeshGenericPowerOnOffStatusPublish (model_context);
    model_context->publish_restrans_cnt--;

    /* Restart the timer */
    if (model_context->publish_restrans_cnt)
    {
        if ((model_context->publish_interval > 0) &&
            (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
            ((model_context->publish_retrans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericPowerOnOffPublishRetransTimerCb,
                   (void *)model_context,
                   (model_context->publish_state->retransmit_param.interval_step + 1) * 50))
                   == QMESH_TIMER_INVALID_HANDLE))
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                              "Publish Retransmission Timer Creation Failed or publish data not set\n");
        }
    }
    else
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                          "------------------- Publish Retransmission done\n");
    }
}

/*----------------------------------------------------------------------------*
*  NAME
 *      qmeshGenericPowerOnOffUpdateNvm
 *
 *  DESCRIPTION
 *      This function writes the current state to NVM.
 *
 *  RETURNS/MODIFIES
 *      None.
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffUpdateNvm(QMESH_GEN_POWERONOFF_DATA_T
                                                     *model_context)
{
    QMESH_MODEL_GEN_PWR_ONOFF_NVM_STATE_T nvm_state;

    nvm_state.onpowerup = model_context->onpowerup;
    nvm_state.last_msg_seq_no =model_context->last_msg_seq_no;
    nvm_state.last_src_addr =model_context->last_src_addr;

    /* Update NVM */
    NVMWrite_ModelGenPowerOnOffState((uint8_t*)&nvm_state);
}

/*----------------------------------------------------------------------------*
*  NAME
*      QmeshGenericPowerOnOffPublishTimerCb
*
*  DESCRIPTION
*      This function sends the OnOf status message to the publish address for
*  the OnOff model periodically every 'Publish_Period' until new tranistion
*  begins.
*
*  RETURNS/MODIFIES
*      None
*
*----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffPublishTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                      "*************************************************%s:\n", __FUNCTION__);
    /* Retrieve the context */
    QMESH_GEN_POWERONOFF_DATA_T *model_context =
        (QMESH_GEN_POWERONOFF_DATA_T *)context;
    /* Send Status Message to Publish Address. Enable below code with new stack release */
    QmeshGenericPowerOnOffStatusPublish (model_context);
    model_context ->publish_timer = QMESH_TIMER_INVALID_HANDLE;

    /* Stop any retransmission of publish message */
    if (model_context->publish_retrans_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

    /* Send Message to Publish Address periodically after every
       "Publish_Period" */
    if ((model_context->publish_interval > 0) &&
        (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
        ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   QmeshGenericPowerOnOffPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                          "Periodic Publish Timer Creation Failed\n");
    }

    /* Decrement the retransmission count */
    model_context->publish_restrans_cnt =
        model_context->publish_state->retransmit_param.count;

    /* Retransmit status Message to the Publish Address periodically
        "publish_restrans_cnt" times at every
         retransmission interval = (Publish Retransmit Interval Steps + 1) * 50 millseconds */
    if (model_context->publish_restrans_cnt &&
        ((model_context->publish_retrans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericPowerOnOffPublishRetransTimerCb,
                   (void *)model_context,
                   (model_context->publish_state->retransmit_param.interval_step + 1) * 50))
                   == QMESH_TIMER_INVALID_HANDLE))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                          "Publish Retransmission Timer Creation Failed\n");
    }
}


/*============================================================================*
 *  Public Function Implementations
 *===========================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerOnOffStatusSend
 *
 *  DESCRIPTION
 *      Sends Generic Power On/Off status message
 *
 *  RETURNS/MODIFIES
 *      returns QMESH_RESULT_SUCCESS if message is handled successfully else other
 *      values as mentioned in 'QMESH_RESULT_T' appropriately.
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffStatusSend (
    QMESH_GEN_POWERONOFF_DATA_T *model_context,
    QMESH_MODEL_MSG_COMMON_T *msg_cmn)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    if (model_context == NULL)
        return;

    /* Construct the status payload */
    model_context->status[0] = ((QMESH_GENERIC_POWERONOFF_STATUS & 0xFF00) >> 8);
    model_context->status[1] = (QMESH_GENERIC_POWERONOFF_STATUS & 0x00FF);
    model_context->status[2] = model_context->onpowerup;
    data_info.payload = model_context->status;
    data_info.payload_len = POWERONOFF_STATUS_REPORT_SIZE;
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
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                          "%s %d QmeshSendMessage failed\n", __FUNCTION__, __LINE__);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerOnOffStatusPublish
 *
 *  DESCRIPTION
 *      Publish Generic Power On/Off status message to the publish address.
 *
 *  RETURNS/MODIFIES
 *      returns QMESH_RESULT_SUCCESS if message is handled successfully else other
 *      values as mentioned in 'QMESH_RESULT_T' appropriately.
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffStatusPublish (
    QMESH_GEN_POWERONOFF_DATA_T *model_context)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    if (model_context == NULL)
        return;

    /* Construct the status payload */
    model_context->status[0] = ((QMESH_GENERIC_POWERONOFF_STATUS & 0xFF00) >> 8);
    model_context->status[1] = (QMESH_GENERIC_POWERONOFF_STATUS & 0x00FF);
    model_context->status[2] = model_context->onpowerup;
    data_info.payload = model_context->status;
    data_info.payload_len = POWERONOFF_STATUS_REPORT_SIZE;
    data_info.ttl = model_context->publish_state->publish_ttl;
    data_info.src_addr = model_context->elm_addr;
    data_info.dst_addr = model_context->publish_state->publish_addr;
    data_info.trans_ack_required = FALSE;
    data_info.trans_mic_size = QMESH_MIC_4_BYTES;
    /* key info */
    app_key_info.key_type = QMESH_KEY_TYPE_APP_KEY;
    app_key_info.key_info.prov_net_idx = 0;
    app_key_info.key_info.key_idx = model_context->publish_state->app_key_idx &
                                    0xFF;

    if (QmeshSendMessage (&app_key_info, &data_info)
        != QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                          "%s %d QmeshSendMessage failed\n", __FUNCTION__, __LINE__);
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericOnOffUpdatePublicationPeriod
 *
 *  DESCRIPTION
 *      Calculates the publish interval from the Publish state data for the model.
 *
 *  RETURNS/MODIFIES
 *
 *----------------------------------------------------------------------------*/
bool QmeshGenericPowerOnOffUpdatePublicationPeriod (QMESH_MODEL_DATA_T
        *model_data)
{
    uint8_t no_of_steps, step_resolution;
    QMESH_GEN_POWERONOFF_DATA_T *model_context = NULL;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER, "%s:\n", __FUNCTION__);
    /* Get the model context data for the element ID 'elm_addr' from the composition data */
    model_context = (QMESH_GEN_POWERONOFF_DATA_T *) model_data->model_priv_data;
    /* 6 bits number of steps */
    no_of_steps = model_data->publish_state->publish_period.steps;
    /* 2 bits step resolution */
    step_resolution = model_data->publish_state->publish_period.resolution;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                      "%s:num_steps=0x%x, resl=0x%x\n",
                      __FUNCTION__, no_of_steps, step_resolution);

    if (no_of_steps != 0x00)
    {
        switch (step_resolution)
        {
            case QMESH_MODEL_STEP_RESOLUTION_100_MSEC:
                model_context->publish_interval =  ((100 * QMESH_MILLISECOND) * no_of_steps);
                break;

            case QMESH_MODEL_STEP_RESOLUTION_1_SEC:
                model_context->publish_interval =  ((1 * QMESH_SECOND) * no_of_steps);
                break;

            case QMESH_MODEL_STEP_RESOLUTION_10_SEC:
                model_context->publish_interval = ((10 * QMESH_SECOND) * no_of_steps);
                break;

            case QMESH_MODEL_STEP_RESOLUTION_10_MINUTES:
                model_context->publish_interval = ((10 * QMESH_MINUTE) * no_of_steps);
                break;
        }

        /* Publication procedure enabled */
        return TRUE;
    }

    /* Publication procedure disabled */
    return FALSE;
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerOnOffPublicationHandler
 *
 *  DESCRIPTION
 *      Publication configuration handler for Generic Power OnOff model
 *
 *  RETURNS/MODIFIES
 *      None
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffPublicationHandler (QMESH_MODEL_DATA_T
        *model_data)
{
    QMESH_GEN_POWERONOFF_DATA_T *model_context = NULL;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER, "%s:\n", __FUNCTION__);
    /* Get the model context data for the element ID 'elm_addr' from the composition data */
    model_context = (QMESH_GEN_POWERONOFF_DATA_T *) model_data->model_priv_data;

    if (model_context == NULL)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                          "Context data is NULL \n");
        return;
    }

    model_context->publish_state = model_data->publish_state;

    if (QmeshGenericPowerOnOffUpdatePublicationPeriod (model_data))
    {
        if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        {
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                              "%s: 0x%x, 0x%x, 0x%x,0x%x,0x%x,0x%x, \n", __FUNCTION__,
                              model_context->publish_interval,
                              model_context->publish_restrans_cnt, model_context->publish_state->publish_addr,
                              model_context->publish_state->publish_ttl,
                              model_context->publish_state->retransmit_param.count,
                              model_context->publish_state->retransmit_param.interval_step);

            /* Start publish timer based on publish interval */
            if ((model_context->publish_interval > 0) &&
                (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
                ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   QmeshGenericPowerOnOffPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                                  "Periodic Publish Timer Creation Failed\n");
            }
        }
        else
        {
            /* Publish timer not running so just return */
            return;
        }
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerOnOffServerHandler
 *
 *  DESCRIPTION
 *      This function handles events for Generic PowerOnOff server
 *
 *  RETURNS/MODIFIES
 *      returns QMESH_RESULT_T
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericPowerOnOffServerHandler (
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
        case QMESH_GENERIC_POWERONOFF_GET:
        {
            QmeshGenericPowerOnOffStatusSend (model_context, &msg_cmn);
        }
        break;

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

                QmeshUpdateBoundStates (model_context->elm_addr,
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
 *      QmeshGenericLevelInit
 *
 *  DESCRIPTION
 *      This function initialises Generic Power onoff
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffInit (QMESH_GEN_POWERONOFF_DATA_T *model_data)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_ONOFF_SERVER,
                      "%s\n", __FUNCTION__);
    /* Restoring context from NVM needs to be added - after NVM support */
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerOnOffSet
 *
 *  DESCRIPTION
 *      Application calls this API to set the associated state value.
 *
 *  RETURNS/MODIFIES
 *      returns nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerOnOffSet (QMESH_GEN_POWERONOFF_DATA_T
                                       *context, uint8_t OnPowerUp)
{
    if (OnPowerUp <= GENERIC_POWERUP_STATE_VALUE_MAX)
    {
        context->onpowerup = OnPowerUp;
        QmeshUpdateBoundStates (context->elm_addr,
                                QAPP_ONPOWERUP_UPDATED,
                                (void *)&context->onpowerup);
        /* Update NVM */
        QmeshGenericPowerOnOffUpdateNvm(context);
    }
}
