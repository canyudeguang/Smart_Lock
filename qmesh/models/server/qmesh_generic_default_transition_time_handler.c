/******************************************************************************
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_generic_default_transition_time_handler.c
 *  \brief defines and functions for SIGmesh Generic Default Transition Time
 *         Handler
 *
 *   This file contains the implementation on SIGmesh Generic Default Transition
 *   Time Handler
 */
/******************************************************************************/

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "qmesh_generic_default_transition_time_handler.h"
#include "qmesh_model_nvm.h"

/*============================================================================ *
 *  Private Function Prototypes
 *============================================================================*/

/* This function returns the default transition time status */
static void getGenericDTTStatus (QMESH_GENERIC_DTT_CONTEXT_T *model_context);

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshGenericDTTUpdateNvm
 *
 *  DESCRIPTION
 *      This function writes the current state to NVM.
 *
 *  RETURNS/MODIFIES
 *      None.
 *
 *----------------------------------------------------------------------------*/
static void qmeshGenericDTTUpdateNvm(QMESH_GENERIC_DTT_CONTEXT_T
                                                     *model_context)
{
    QMESH_MODEL_GEN_DTT_NVM_STATE_T nvm_state;

    nvm_state.transition_time = model_context->transition_time;
    nvm_state.last_msg_seq_no =model_context->last_msg_seq_no;
    nvm_state.last_src_addr =model_context->last_src_addr;

    /* Update NVM */
    NVMWrite_ModelGenDTTState((uint8_t*)&nvm_state);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      getGenericDTTStatus
 *
 *  DESCRIPTION
 *      This function returns the default transition time status
 *
 *  RETURNS/MODIFIES
 *      Default Transition Time Status
 *
 *----------------------------------------------------------------------------*/
static void getGenericDTTStatus (QMESH_GENERIC_DTT_CONTEXT_T *model_context)
{
    /* Get the status opcode */
    model_context->status[0] =
        (uint8_t) (QMESH_GENERIC_DEFAULT_TRANSITION_TIME_STATUS >> 8);
    model_context->status[1] =
        (uint8_t)QMESH_GENERIC_DEFAULT_TRANSITION_TIME_STATUS;
    /* Get the current transition time */
    model_context->status[2] = (uint8_t)model_context->transition_time;
}

/*----------------------------------------------------------------------------*
*  NAME
*      qmeshGenericDTTPublishRetransTimerCb
*
*  DESCRIPTION
*      This function sends the OnOf status message to the publish address for
*  the Generic default time transition model periodically every 'Publish_Period'
*  until new tranistion begins.
*
*  RETURNS/MODIFIES
*      None
*
*----------------------------------------------------------------------------*/
static  void qmeshGenericDTTPublishRetransTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    /* Retrieve the context */
    QMESH_GENERIC_DTT_CONTEXT_T *model_context =
        (QMESH_GENERIC_DTT_CONTEXT_T *)context;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER, "%s, cnt=0x%x:\n", __FUNCTION__,
                      model_context->publish_restrans_cnt);
    model_context ->publish_timer = QMESH_TIMER_INVALID_HANDLE;
    /* Send the Status Message to Publish Address */
    QmeshGenericDTTStatusPublish (model_context);
    model_context->publish_restrans_cnt--;

    /* Restart the timer */
    if (model_context->publish_restrans_cnt)
    {
        if ((model_context->publish_interval > 0) &&
            (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
            ((model_context->publish_retrans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericDTTPublishRetransTimerCb,
                   (void *)model_context,
                   (model_context->publish_state->retransmit_param.interval_step + 1) * 50))
                   == QMESH_TIMER_INVALID_HANDLE))
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                              "Publish Retransmission Timer Creation Failed or publish data not set\n");
        }
    }
    else
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                          "Publish Retransmission done\n");
    }
}

/*----------------------------------------------------------------------------*
*  NAME
*      qmeshGenericDTTPublishTimerCb
*
*  DESCRIPTION
*      This function sends the GenericDTT status message to the publish address for
*  the Generic default time transition model periodically every 'Publish_Period' until new tranistion
*  begins.
*
*  RETURNS/MODIFIES
*      None
*
*----------------------------------------------------------------------------*/
static void qmeshGenericDTTPublishTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER, "%s:\n", __FUNCTION__);
    /* Retrieve the context */
    QMESH_GENERIC_DTT_CONTEXT_T *model_context =
        (QMESH_GENERIC_DTT_CONTEXT_T *)context;
    /* Send Status Message to Publish Address. Enable below code with new stack release */
    QmeshGenericDTTStatusPublish (model_context);
    model_context ->publish_timer = QMESH_TIMER_INVALID_HANDLE;

    /* Stop any retransmission of publish message */
    if (model_context->publish_retrans_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

    /* Send Message to Publish Address periodically after every
       "Publish_Period" */
    if ((model_context->publish_interval > 0) &&
        (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
        ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                                                   qmeshGenericDTTPublishTimerCb,
                                                   (void *)model_context,
                                                    model_context->publish_interval))
                                                    == QMESH_TIMER_INVALID_HANDLE))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                          "Periodic Publish Timer Creation Failed\n");
    }

    /* Decrement the retransmission count */
    model_context->publish_restrans_cnt =
        model_context->publish_state->retransmit_param.count;

    /* Retransmit status Message to the Publish Address periodically
        "publish_restrans_cnt" times at every
         retransmission interval = (Publish Retransmit Interval Steps + 1) * 50 millseconds */
    if ((model_context->publish_restrans_cnt) &&
        ((model_context->publish_retrans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericDTTPublishRetransTimerCb,
                   (void *)model_context,
                   (model_context->publish_state->retransmit_param.interval_step + 1) * 50))
                   == QMESH_TIMER_INVALID_HANDLE))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                          "Publish Retransmission Timer Creation Failed\n");
    }
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericDTTStatusSend
 *
 *  DESCRIPTION
 *      This function sends the DTT status message in response to reliable
 *      GET/SET message
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericDTTStatusSend (QMESH_GENERIC_DTT_CONTEXT_T
                                       *model_context,
                                       QMESH_MODEL_MSG_COMMON_T *msg_cmn)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    /* Check for validity of context data */
    if (!model_context || !msg_cmn)
    {
        DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GDTT_SERVER,
                           "\nNull dtt model context/Null common msg\n");
        return;
    }

    if (msg_cmn->opcode == QMESH_GENERIC_DEFAULT_TRANSITION_TIME_GET ||
        msg_cmn->opcode == QMESH_GENERIC_DEFAULT_TRANSITION_TIME_SET)
    {
        /* Get the DTT status  */
        getGenericDTTStatus (model_context);
        /* key info */
        app_key_info.key_type = model_context->key_info.key_type;
        app_key_info.key_info.prov_net_idx =
            model_context->key_info.key_info.prov_net_idx;
        app_key_info.key_info.key_idx = model_context->key_info.key_info.key_idx;
        /* payload info */
        data_info.src_addr = model_context->elm_addr;
        data_info.dst_addr = msg_cmn->src;
        data_info.payload_len = GENERIC_DTT_STATUS_MSG_LEN;
        data_info.payload = model_context->status;
        data_info.ttl = QMESH_MODEL_DEFAULT_TTL;
        data_info.trans_mic_size = QMESH_MIC_4_BYTES;
        data_info.trans_ack_required = FALSE;

         DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GDTT_SERVER,
                           "\nDTT Status Sent\n");
        /* send to stack */
        (void)QmeshSendMessage (&app_key_info, &data_info);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericDTTStatusPublish
 *
 *  DESCRIPTION
 *      This function sends the default transition time status message to
 *      assigned publish address
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericDTTStatusPublish (QMESH_GENERIC_DTT_CONTEXT_T
        *model_context)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    /* Check for validity of context data */
    if (!model_context)
    {
        DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GDTT_SERVER,
                           "\nNull dtt model context\n");
        return;
    }

    /* Get the default transition time status  */
    getGenericDTTStatus (model_context);
    data_info.payload = model_context->dtt_status;
    data_info.payload_len = GENERIC_DTT_STATUS_MSG_LEN;
    /* Fill the 'data_info' */
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

    if (QmeshSendMessage (&app_key_info, &data_info) != QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                          "%s %d QmeshSendMessage failed\n", __FUNCTION__, __LINE__);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericDTTUpdatePublicationPeriod
 *
 *  DESCRIPTION
 *      Calculates the publish interval from the Publish state data for the model.
 *
 *  RETURNS/MODIFIES
 *
 *----------------------------------------------------------------------------*/
extern bool QmeshGenericDTTUpdatePublicationPeriod (QMESH_MODEL_DATA_T
        *model_data)
{
    uint8_t no_of_steps, step_resolution;
    QMESH_GENERIC_DTT_CONTEXT_T *model_context = NULL;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER, "%s:\n", __FUNCTION__);
    /* Get the model context data for the element ID 'elm_addr' from the composition data */
    model_context = (QMESH_GENERIC_DTT_CONTEXT_T *) model_data->model_priv_data;
     model_context->publish_state = model_data->publish_state;
    /* 6 bits number of steps */
    no_of_steps = model_data->publish_state->publish_period.steps;
    /* 2 bits step resolution */
    step_resolution = model_data->publish_state->publish_period.resolution;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER, "%s:num_steps=0x%x, resl=0x%x\n",
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
 *      QmeshGenericDTTPublicationHandler
 *
 *  DESCRIPTION
 *      Publication configuration handler for Generic Default Time Transition server model
 *
 *  RETURNS/MODIFIES
 *      None
 *----------------------------------------------------------------------------*/
extern void QmeshGenericDTTPublicationHandler (QMESH_MODEL_DATA_T *model_data)
{
    QMESH_GENERIC_DTT_CONTEXT_T *model_context = NULL;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER, "%s:\n", __FUNCTION__);
    /* Get the model context data for the element ID 'elm_addr' from the composition data */
    model_context = (QMESH_GENERIC_DTT_CONTEXT_T *) model_data->model_priv_data;

    if (model_context == NULL)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                          "Context data is NULL \n");
        return;
    }

    model_context->publish_state = model_data->publish_state;

    if (QmeshGenericDTTUpdatePublicationPeriod (model_data))
    {
        if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        {
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
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
                   qmeshGenericDTTPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
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
 *      QmeshGenericDTTGetTransitionTime
 *
 *  DESCRIPTION
 *      This function returns transition time as non zero value if DTT is
 *      supported on that element
 *
 *  RETURNS/MODIFIES
 *      Transition Time
 *
 *----------------------------------------------------------------------------*/
extern uint8_t QmeshGenericDTTGetTransitionTime (QMESH_GENERIC_DTT_CONTEXT_T
        *model_context)
{
    uint8_t trans_time = 0;

    if (model_context != NULL)
        trans_time = model_context->transition_time;

    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                          "Transition Time Get %d\n",
                          model_context->transition_time);

    return trans_time;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericDTTTransitionTimeSet
 *
 *  DESCRIPTION
 *      This function sets the default transition time
 *
 *  RETURNS/MODIFIES
 *      Nothings
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericDTTTransitionTimeSet (QMESH_GENERIC_DTT_CONTEXT_T
        *model_context, uint8_t transition_time)
{
    if (model_context != NULL)
    {
        model_context->transition_time = transition_time;
        /* Update NVM */
        qmeshGenericDTTUpdateNvm(model_context);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                          "Transition Time Set %d\n",
                          model_context->transition_time);
    }
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericDTTServerAppInit
 *
 *  DESCRIPTION
 *      This function initialises Generic Deafult Transition Time model state
 *      data
 *
 *  RETURNS/MODIFIES
 *      QMESH_RESULT_T
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericDTTServerAppInit (QMESH_GENERIC_DTT_CONTEXT_T
        *model_context)
{
    /* Check if model_context is valid */
    if (model_context == NULL)
    {
        DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GDTT_SERVER,
                           "Null dtt model context");
        return QMESH_RESULT_FAILURE;
    }

    if (QmeshMutexCreate (&model_context->dtt_mutex) !=
        QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GDTT_SERVER,
                           "QmeshGenericDTTInit:Creating mutex Failed\n");
        return QMESH_RESULT_FAILURE;
    }

    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericDTTServerAppDeInit
 *
 *  DESCRIPTION
 *      This function de-initialises Generic Deafult Transition Time model
 *      state data
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericDTTServerAppDeInit (QMESH_GENERIC_DTT_CONTEXT_T
        *model_context)
{
    /* Destroy the mutex for all instances of generic dtt */
    QmeshMutexDestroy (&model_context->dtt_mutex);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                      "QmeshGenericDTTServerAppDeInit\n");
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericDTTServerAppHandler
 *
 *  DESCRIPTION
 *      This function handles events for Generic Default Transition Time server
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericDTTServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len)
{
    QMESH_MODEL_MSG_COMMON_T msg_cmn;
    QMESH_GENERIC_DTT_CONTEXT_T *model_context = NULL;
    uint32_t opcode, seq_no;
    uint16_t src_addr;
    /* Get dtt context */
    model_context = (QMESH_GENERIC_DTT_CONTEXT_T *)model_data->model_priv_data;

    if ((model_context == NULL) ||
        (!msg || msg_len <= 0))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                          "Context data is NULL or msg is invalid \n");
        return QMESH_RESULT_FAILURE;
    }

    if (model_context->last_msg_seq_no == nw_hdr->seq_num &&
        model_context->last_src_addr == nw_hdr->src_addr)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER, "%s: seq num 0x%08x processed\n",
                          __FUNCTION__, nw_hdr->seq_num);
        return QMESH_RESULT_SUCCESS;
    }

    /* Retrieve the opcode from the message */
    if ((get_opcode_format (msg[MODEL_OPCODE_OFFSET]) ==
         QMESH_OPCODE_FORMAT_TWO_BYTE))
    {
        opcode = QMESH_PACK_TWO_BYTE_OPCODE (msg[0], msg[1]);
        msg += QMESH_OPCODE_FORMAT_TWO_BYTE;
        msg_len -= QMESH_OPCODE_FORMAT_TWO_BYTE;
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                          "Src Addr: %x Elm Idx: %x Opcode: %x, Msg Len: %d\n",
                          nw_hdr->src_addr, nw_hdr->dst_addr, opcode, msg_len);
    }
    else
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                          "Incorrect Opcode Length\n");
        return QMESH_RESULT_FAILURE;
    }

    QmeshMemSet(&msg_cmn, 0, sizeof(msg_cmn));

    /* Lock the mutex */
    QmeshMutexLock (&model_context->dtt_mutex);
    QmeshMemCpy (&model_context->key_info, key_info,
                 sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));
    /* Store the common message parameters */
    msg_cmn.opcode = opcode;
    msg_cmn.src = nw_hdr->src_addr;
    msg_cmn.dst = nw_hdr->dst_addr;
    /* Store the previous seqence number and source address */
    seq_no = model_context->last_msg_seq_no;
    src_addr = model_context->last_src_addr;
    /* Store the sequence number and src address of the new message */
    model_context->last_msg_seq_no = nw_hdr->seq_num;
    model_context->last_src_addr = nw_hdr->src_addr;

    switch (opcode)
    {
        case QMESH_GENERIC_DEFAULT_TRANSITION_TIME_GET:
        {
            if (msg_len != GENERIC_DTT_GET_MSG_LEN)
            {
                QmeshMutexUnlock (&model_context->dtt_mutex);
                return QMESH_RESULT_SUCCESS;
            }
        }
        break;

        case QMESH_GENERIC_DEFAULT_TRANSITION_TIME_SET:
        case QMESH_GENERIC_DEFAULT_TRANSITION_TIME_SET_UNRELIABLE:
        {
            if ((msg_len != GENERIC_DTT_SET_MSG_LEN) ||
                (NUM_OF_TRANSITION_STEPS (msg[OFFSET_DEFAULT_TRANSITION]) ==
                 QMESH_MODEL_INVALID_TRANSITION_TIME))
            {
                QmeshMutexUnlock (&model_context->dtt_mutex);
                return QMESH_RESULT_SUCCESS;
            }

            QmeshGenericDTTTransitionTimeSet (model_context,
                                              msg[OFFSET_DEFAULT_TRANSITION]);

            /* Delete Publish timer if running */
            if (model_context ->publish_timer != QMESH_TIMER_INVALID_HANDLE)
                QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

            /* Publish the 'status' message */
            if (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS)
                QmeshGenericDTTStatusPublish (model_context);

            /* If publish time is valid (non-zero) start the publish timer */
            if (model_context->publish_interval != 0 &&
                model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
                ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericDTTPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                                  "Publish Timer Creation Failed \n");
            }
        }
        break;

        default:
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                              "Opcode did not match\n");
            /* Retore the previous sequence number and src address - since the message is not processed */
            model_context->last_msg_seq_no = seq_no;
            model_context->last_src_addr = src_addr;
            QmeshMutexUnlock (&model_context->dtt_mutex);
            return QMESH_RESULT_FAILURE;
        }
    }

    QmeshGenericDTTStatusSend (model_context, &msg_cmn);
    /* Unlock the mutex */
    QmeshMutexUnlock (&model_context->dtt_mutex);
    return QMESH_RESULT_SUCCESS;
}

