/******************************************************************************
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_model_generic_handler.c
 *  \brief defines and functions for SIGmesh Generic_OnOff Server model.
 *
 *   This file contains the implementation of Generic_OnOff Server model.
 *   This file should be built as part of the model Application.
 */
/******************************************************************************/

#include "qmesh_model_common.h"
#include "qmesh_model_debug.h"

#include "qmesh_model_config.h"
#include "qmesh_cache_mgmt.h"
#include "qmesh_generic_onoff_handler.h"
#include "qmesh_model_nvm.h"

/*=============================================================================*
 *  Private Function Prototypes
 *============================================================================*/
static void qmeshGenericOnOffPublishTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context);
static void qmeshGenericOnOffTransitionTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context);

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshGenericOnOffUpdateNvm
 *
 *  DESCRIPTION
 *      This function writes the current state to NVM.
 *
 *  RETURNS/MODIFIES
 *      None.
 *
 *----------------------------------------------------------------------------*/
static void qmeshGenericOnOffUpdateNvm(QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
                                                     *model_context)
{
    QMESH_MODEL_GEN_ONOFF_NVM_STATE_T nvm_state;

    nvm_state.cur_onoff = model_context->onoff_state.cur_onoff;
    nvm_state.trgt_onoff = model_context->onoff_state.trgt_onoff;
    nvm_state.last_msg_seq_no = model_context->last_msg_seq_no ;
    nvm_state.last_src_addr = model_context->last_src_addr;

    /* Update NVM */
    NVMWrite_ModelGenOnOffState((uint8_t*)&nvm_state);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshInitiateTransition
 *
 *  DESCRIPTION
 *      This function starts the timer for 'transition_time' if non-zero and
 *  sends the 'status' message to 'Publish' address.
 *
 *  RETURNS/MODIFIES
 *      TRUE if transition timer is started successfully. FALSE otherwise.
 *
 *----------------------------------------------------------------------------*/
static bool  qmeshInitiateTransition (QMESH_MODEL_MSG_T *model_msg,
                                      void *context)
{
    uint32_t milli_sec = 0;
    uint16_t trans_ms[3] = {0};
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context =
        (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *)context;

    /* Calculate 'transition' time in milliseconds */
    milli_sec = QmeshTransitionTimeToMilliSec (model_msg->transition_time);

    if (milli_sec == 0)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s %d Transition time is not valid - Ignoring the message!!\n",
                          __FUNCTION__, __LINE__);
        return FALSE;
    }

    QmeshGetCurrentTimeInMs (model_context->onoff_state.cur_time);
    /* target time = current time + transition time*/
    /* Note: time should be stored in milliseconds going forward -
                    as microseonds not required as per Model specification */
    QmeshMemSet (model_context->onoff_state.trgt_time, 0,
                 sizeof (model_context->onoff_state.trgt_time));
    /* time value is 32 bits */
    trans_ms[0] = (uint16_t)milli_sec;
    trans_ms[1] = (uint16_t)(milli_sec >> 16);
    QmeshTimeAdd (model_context->onoff_state.trgt_time,
                  model_context->onoff_state.cur_time,
                  trans_ms);
    QmeshMutexLock (&model_context->onoff_mutex);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                      "%s %d Target time is trgttime [2]=0x%x, [1]=0x%x,[0]=0x%x \n",
                      __FUNCTION__, __LINE__,
                      model_context->onoff_state.trgt_time[2],
                      model_context->onoff_state.trgt_time[1],
                      model_context->onoff_state.trgt_time[0]);

    if ((model_context->onoff_state.cur_timer = QmeshTimerCreate (&model_timer_ghdl,
                                                                  qmeshGenericOnOffTransitionTimerCb,
                                                                  (void *)model_context,
                                                                  milli_sec)) == QMESH_TIMER_INVALID_HANDLE )
    {
        /* Set the target state = current state and reset 'transition' time */
        model_context->transition_time = QMESH_MODEL_TRANSITION_NOT_INPROGRESS;
        model_context->onoff_state.trgt_onoff =
        model_context->onoff_state.initial_onoff =
                        model_context->onoff_state.cur_onoff;
        /* Update NVM */
        qmeshGenericOnOffUpdateNvm(model_context);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s %d Timer create failed - Current message dropped!! \n",
                          __FUNCTION__, __LINE__);
        QmeshMutexUnlock (&model_context->onoff_mutex);
        return FALSE;
    }

    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                      "%s: Transition timer started for %d millisec \n", __FUNCTION__, milli_sec);

    if (model_msg->msg[GENERIC_ONOFF_VALUE_OFFSET] == GENERIC_ONOFF_STATE_ON)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s:  Setting the state to ON \n", __FUNCTION__);
        /* Set the initial state to current state */
        model_context->onoff_state.initial_onoff =
             model_context->onoff_state.cur_onoff;
        /* Set the current state to ON(0x01) state if moving from OFF to ON */
        model_context->onoff_state.cur_onoff =
        model_context->onoff_state.trgt_onoff =
              model_msg->msg[GENERIC_ONOFF_VALUE_OFFSET];
    }
    else
    {
        /* Set the initial state to current state */
        model_context->onoff_state.initial_onoff =
            model_context->onoff_state.cur_onoff;
        model_context->onoff_state.trgt_onoff =
            model_msg->msg[GENERIC_ONOFF_VALUE_OFFSET];
    }

    /* Update NVM */
    qmeshGenericOnOffUpdateNvm(model_context);

    /* Send additional status message if transition time is greater than or equal to 2 secs.
         See Model specificaiton section 7.4.1.3*/
    if (milli_sec >= TRANSITION_TIME_PUBLISH_THRESHOLD)
    {
        QmeshGenericOnOffStatusPublish (model_context);
    }

    QmeshMutexUnlock (&model_context->onoff_mutex);
    return TRUE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshGenericOnOffTransitionHandler
 *
 *  DESCRIPTION
 *      This function can be called from,
 *      1. Delay cache upon expiry of 'delay' and to start 'Transition' to
 *         the target state
 *      2. If 'Delay' is 0, will be called directly from 'Extended' model
 *
 *  Note: The caller should call this only if 'target' state is not equal
 *  to the 'current' state.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
static void qmeshGenericOnOffTransitionHandler (QMESH_MODEL_MSG_T *model_msg,
        void *context)
{
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context
        = (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *)context;
    uint8_t num_steps = 0, step_resolution = 0;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "\n%s:\n", __FUNCTION__);
    /*
        1. Check if the 'state' is in transition (if yes, stop/abort) and
           initiate change the current state to new state as per the
           'Transition' time
        2. If msg is reliable, send the 'status' msg back to the 'src'
           if msg is reliable
        3. If 'state' changed - then send the 'status' msg to the Publish
           addresses as mentioned in section 7.4.1.3 in Model spec.
    */
    QmeshMutexLock (&model_context->onoff_mutex);

    /* Check if current state is in transition. */
    if (model_context->onoff_state.initial_onoff !=
        model_context->onoff_state.trgt_onoff)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s %d Transition In-Progress - Abort the current transition \n",
                          __FUNCTION__, __LINE__);
        /* Cancel the current transition */
        QmeshMutexUnlock (&model_context->onoff_mutex);
        QmeshGenericOnOffAbortTransition (model_context);
        QmeshMutexLock (&model_context->onoff_mutex);
    }

    //model_context->elm_addr = model_msg->cmn_msg.elem_id;
    model_context->transition_time = model_msg->transition_time;
    model_context->delay = model_msg->delay;
    QmeshMemCpy (&model_context->key_info, &model_msg->key_info,
                 sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));
    /* Retrieve 'num_steps' from 'transition_time' */
    num_steps = NUM_OF_TRANSITION_STEPS (model_context->transition_time) ;
    /* Retrieve 'step_resolution' from 'transition_time' */
    step_resolution = STEP_RESOLUTION (model_context->transition_time);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                      "%s %d num_steps=0x%x, step_resl=0x%x \n",
                      __FUNCTION__, __LINE__, num_steps, step_resolution);

    /* Abort any transitions on-going in other bound models */
    QmeshUpdateBoundStates (model_context->elm_addr, QAPP_GEN_ONOFF_TRANS_BEGIN,
                            (void *)&model_context->onoff_state.trgt_onoff);

    /* If 'Transtion' time is present and 'transition time' is not immediate */
    if (num_steps > 0)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s %d TransitionTime=0x%x \n", __FUNCTION__, __LINE__,
                          model_msg->msg[GENERIC_ONOFF_TRANSITION_TIME_OFFSET]);

        /* Stop publishing status messages */
        if (model_context ->publish_timer != QMESH_TIMER_INVALID_HANDLE)
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

        if (model_context ->publish_retrans_timer != QMESH_TIMER_INVALID_HANDLE)
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

        /* Initiate transition to the target state and send 'status' message */
        if (qmeshInitiateTransition (model_msg, context) == FALSE)
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                              "%s %d Failed to Initiate transition\n", __FUNCTION__, __LINE__);
            QmeshMutexUnlock (&model_context->onoff_mutex);
            return;
        }
    }
    else /* Transition time is unavailable or zero */
    {
        /* Set the new target state immeditely */
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s %d Moving to new state=0x%x \n", __FUNCTION__,
                          __LINE__, model_msg->msg[GENERIC_ONOFF_VALUE_OFFSET]);

        model_context->transition_time = QMESH_MODEL_TRANSITION_NOT_INPROGRESS;
        model_context->delay = 0;
        /* Set the new state */
        model_context->onoff_state.initial_onoff =
        model_context->onoff_state.cur_onoff =
        model_context->onoff_state.trgt_onoff =
              model_msg->msg[GENERIC_ONOFF_VALUE_OFFSET];

        /* Publish the 'status' message immediately and periodically according to the Publish data */

        /* Send 'status' message to Publish address as mentioned in
          section 7.4.1.3 in Model spec. */
        if (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS)
            QmeshGenericOnOffStatusPublish (model_context);

        if (model_context ->publish_timer != QMESH_TIMER_INVALID_HANDLE)
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

        /* If publish time is valid (non-zero) start the publish timer */
        if (model_context->publish_interval != 0 &&
            model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
            ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericOnOffPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                              "Publish Timer Creation Failed \n");
        }

        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s %d mutex un-locked \n", __FUNCTION__, __LINE__);
        /* Update the bound models with the new state */
        QmeshUpdateBoundStates (model_context->elm_addr, QAPP_GEN_ONOFF_UPDATED,
                                (void *)&model_context->onoff_state.trgt_onoff);
    }

    /* Update NVM */
    qmeshGenericOnOffUpdateNvm(model_context);

    QmeshMutexUnlock (&model_context->onoff_mutex);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      getOnOffStatus
 *
 *  DESCRIPTION
 *      Returns the current OnOff state in the context 'status' field.
 *
 *  RETURNS/MODIFIES
 *      OnOff Status
 *----------------------------------------------------------------------------*/
static void getOnOffStatus (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s: \n", __FUNCTION__);
    /* Get the opcode, present level and target level */
    model_context->status[0] = (uint8_t) (QMESH_GENERIC_ONOFF_STATUS >> 8);
    model_context->status[1] = (uint8_t)QMESH_GENERIC_ONOFF_STATUS;
    model_context->status[2] = (uint8_t)model_context->onoff_state.cur_onoff;
    model_context->status[3] = (uint8_t)model_context->onoff_state.trgt_onoff;
    model_context->remaining_time =
        QmeshGetRemainingTime (((uint32_t)model_context->onoff_state.trgt_time[1] << 16) |
                               (uint32_t)model_context->onoff_state.trgt_time[0]);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                      "%s %d remaining time=0x%x \n", __FUNCTION__, __LINE__,
                      model_context->remaining_time);

    /* Transition NOT in progress */
    if (model_context->remaining_time == 0 )
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s %d non-trans, remaining time is 0 \n", __FUNCTION__, __LINE__);
        /* Remaining time should be 0 */
        model_context->status[4] = 0;
    }
    else  /* Transition in progress */
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s %d Trans on-going\n", __FUNCTION__, __LINE__);
        /* Get the remaining time */
        model_context->status[4] = model_context->remaining_time;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshGenericOnOffTransitionTimerCb
 *
 *  DESCRIPTION
 *      This callback is called when the 'transition' time expires.
 *  This function sets the current state to the target state and send the
 * 'status' message to the Publish address accordingly.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
static void qmeshGenericOnOffTransitionTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context =
        (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *)context;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s: \n", __FUNCTION__);
    QmeshMutexLock (&model_context->onoff_mutex);

    /* Current value must be set to Target value */
    model_context->onoff_state.initial_onoff =
    model_context->onoff_state.cur_onoff =
          model_context->onoff_state.trgt_onoff;
    model_context->transition_time = QMESH_MODEL_TRANSITION_NOT_INPROGRESS;
    model_context->delay = QMESH_MODEL_TRANSITION_NOT_INPROGRESS;
    model_context->remaining_time = QMESH_MODEL_TRANSITION_NOT_INPROGRESS;

    /* Update NVM */
    qmeshGenericOnOffUpdateNvm(model_context);

    QmeshMemSet (model_context->onoff_state.trgt_time, 0,
                 sizeof (model_context->onoff_state.trgt_time));

    /* Send 'status' message to Publish address as mentioned in
      section 7.4.1.3 in Model spec. */
    if (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS)
        QmeshGenericOnOffStatusPublish (model_context);

    /* Stop on-going publishing status message */
    if (model_context ->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

    /* If publish time is valid (non-zero) start the publish timer */
    if ((model_context->publish_interval > 0) &&
        (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
        ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericOnOffPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "Publish Timer Creation Failed \n");
    }

    QmeshUpdateBoundStates (model_context->elm_addr, QAPP_GEN_ONOFF_UPDATED,
                            (void *)&model_context->onoff_state.trgt_onoff);
    QmeshMutexUnlock (&model_context->onoff_mutex);
}

/*----------------------------------------------------------------------------*
*  NAME
*      qmeshGenericOnOffPublishRetransTimerCb
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
static void qmeshGenericOnOffPublishRetransTimerCb (QMESH_TIMER_HANDLE_T
        timerHandle,
        void *context)
{
    /* Retrieve the context */
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context =
        (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *)context;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                      "%s: Re-transmit count=0x%x:\n", __FUNCTION__,
                      model_context->publish_restrans_cnt);
    model_context ->publish_timer = QMESH_TIMER_INVALID_HANDLE;
    /* Send Status Message to Publish Address */
    QmeshGenericOnOffStatusPublish (model_context);
    model_context->publish_restrans_cnt--;

    /* Restart the timer */
    if (model_context->publish_restrans_cnt)
    {
        if ((model_context->publish_interval > 0) &&
            (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
            ((model_context->publish_retrans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericOnOffPublishRetransTimerCb,
                   (void *)model_context,
                   (model_context->publish_state->retransmit_param.interval_step + 1) * 50))
                   == QMESH_TIMER_INVALID_HANDLE))
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                              "Publish Retransmission Timer Creation Failed or publish data not set\n");
        }
    }
}

/*----------------------------------------------------------------------------*
*  NAME
*      qmeshGenericOnOffPublishTimerCb
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
static void qmeshGenericOnOffPublishTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s:\n", __FUNCTION__);
    /* Retrieve the context */
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context =
        (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *)context;
    /* Send Status Message to Publish Address. Enable below code with new stack release */
    QmeshGenericOnOffStatusPublish (model_context);
    model_context ->publish_timer = QMESH_TIMER_INVALID_HANDLE;

    /* Stop any retransmission of publish message */
    if (model_context->publish_retrans_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

    /* Send Message to Publish Address periodically after every
       "Publish_Period" */
    if ((model_context->publish_interval > 0) &&
        (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
        ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericOnOffPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "Periodic Publish Timer Creation Failed\n");
    }

    model_context->publish_restrans_cnt =
        model_context->publish_state->retransmit_param.count;

    /* Retransmit status Message to the Publish Address periodically
        "publish_restrans_cnt" times at every
         retransmission interval = (Publish Retransmit Interval Steps + 1) * 50 millseconds */
    if (model_context->publish_restrans_cnt &&
        ((model_context->publish_retrans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericOnOffPublishRetransTimerCb,
                   (void *)model_context,
                   (model_context->publish_state->retransmit_param.interval_step + 1) * 50))
                   == QMESH_TIMER_INVALID_HANDLE))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "Publish Retransmission Timer Creation Failed\n");
    }
}


/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericOnOffStatusSend
 *
 *  DESCRIPTION
 *      This function sends the current Generic OnOff Status message to the
 *  remote device as mentioned in 'msg_cmn' parameter.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericOnOffStatusSend (
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context,
    QMESH_MODEL_MSG_COMMON_T *msg_cmn)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s %d opcode=0x%x\n",
                      __FUNCTION__, __LINE__, msg_cmn->opcode);

    if (!model_context)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s: NULL model context\n", __FUNCTION__);
        return;
    }

    QmeshMemSet (model_context->status, 0, sizeof (model_context->status));

    switch (msg_cmn->opcode)
    {
        case QMESH_GENERIC_ONOFF_GET:
        case QMESH_GENERIC_ONOFF_SET:
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s %d \n",
                              __FUNCTION__, __LINE__);
            /* Get the OnOff status  */
            getOnOffStatus (model_context);

            /* For QMESH_GENERIC_ONOFF_SET message, send the remaining transition
             * time as received in the request message
             */
            if (msg_cmn->opcode == QMESH_GENERIC_ONOFF_SET)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                                  "%s %d Setting Tran Time to 0x%x \n", __FUNCTION__, __LINE__,
                                  model_context->transition_time);
                model_context->status[GENERIC_ONOFF_STATUS_MSG_TIME_OFFSET] =
                    model_context->transition_time ;

                /* If the message is onoff_set with target state set to ON with transition ongoing
                   - send the status back with
                    present state = OFF and target state = ON */
                if ((model_context->onoff_state.trgt_onoff ==
                     model_context->onoff_state.cur_onoff) &&
                    (model_context->onoff_state.trgt_onoff == GENERIC_ONOFF_STATE_ON) &&
                    (model_context->remaining_time > 0))
                {
                    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                                      "%s %d setting cur to off \n", __FUNCTION__, __LINE__);
                    model_context->status[GENERIC_ONOFF_STATUS_MSG_PRESENT_ONOFF_OFFSET] =
                        GENERIC_ONOFF_STATE_OFF;
                }
            }

            if (model_context->status[GENERIC_ONOFF_STATUS_MSG_TIME_OFFSET] == 0)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                                  "%s %d length set to non-trans \n", __FUNCTION__, __LINE__);
                data_info.payload_len = GENERIC_ONOFF_STATUS_NON_TRANS_MSG_LEN;
            }
            else
            {
                data_info.payload_len = GENERIC_ONOFF_STATUS_TRANS_MSG_LEN;
            }
        }
        break;

        default:
            return;
    }

    data_info.payload = model_context->status;
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
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s %d QmeshSendMessage failed\n", __FUNCTION__, __LINE__);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericOnOffStatusPublish
 *
 *  DESCRIPTION
 *      This function sends the current Generic OnOff Status message to the
 *  Publish address assigned for this model.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericOnOffStatusPublish (
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s:\n", __FUNCTION__);

    if (!model_context)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s: NULL model context\n", __FUNCTION__);
        return;
    }

    QmeshMemSet (model_context->status, 0, sizeof (model_context->status));
    /* Get the current OnOff status */
    getOnOffStatus (model_context);

    if (model_context->status[GENERIC_ONOFF_STATUS_MSG_TIME_OFFSET] == 0)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s %d length set to non-trans \n", __FUNCTION__, __LINE__);
        data_info.payload_len = GENERIC_ONOFF_STATUS_NON_TRANS_MSG_LEN;
    }
    else
    {
        data_info.payload_len = GENERIC_ONOFF_STATUS_TRANS_MSG_LEN;
    }

    data_info.payload = model_context->status;
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
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s %d QmeshSendMessage failed\n", __FUNCTION__, __LINE__);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericOnOffSet
 *
 *  DESCRIPTION
 *      This function sets the Generic OnOff state to the value passed in the
 *   'onoff' parameter as per transition_time and delay parameters.
 *
 *  Note: The caller should abort any ongoing transtion by calling
 *        QmeshGenericOnOffAbortTransition before calling this API.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericOnOffSet (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
                                  *model_context, uint8_t transition_time, uint8_t delay, uint8_t onoff)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s %d \n",
                      __FUNCTION__, __LINE__);
    /* Note: transition_time and delay params are for future requirements */
    QmeshGenericOnOffAbortTransition (model_context);

    /* Abort any transitions on-going in other bound models */
    QmeshUpdateBoundStates (model_context->elm_addr, QAPP_GEN_ONOFF_TRANS_BEGIN,
                            (void *)&model_context->onoff_state.trgt_onoff);
    QmeshMutexLock (&model_context->onoff_mutex);
    model_context->onoff_state.cur_onoff = onoff;
    model_context->onoff_state.initial_onoff = onoff;
    model_context->onoff_state.trgt_onoff = onoff;
    QmeshUpdateBoundStates (model_context->elm_addr, QAPP_GEN_ONOFF_UPDATED,
                            (void *)&model_context->onoff_state.trgt_onoff);
    /* Update NVM */
    qmeshGenericOnOffUpdateNvm(model_context);
    QmeshMutexUnlock (&model_context->onoff_mutex);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericOnOffGet
 *
 *  DESCRIPTION
 *      This function returns the current Generic OnOff state.
 *
 *  Note: The caller should abort any ongoing transtion by calling
 *        QmeshGenericOnOffAbortTransition before calling this API.
 *
 *  RETURNS/MODIFIES
 *      Current OnOff state.
 *
 *----------------------------------------------------------------------------*/
extern uint8_t QmeshGenericOnOffGet (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
                                   *model_context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s %d \n",
                      __FUNCTION__, __LINE__);
    return model_context->onoff_state.cur_onoff;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericOnOffUpdate
 *
 *  DESCRIPTION
 *      This function sets the Generic OnOff state to the value passed in the
 *   'onoff' parameter.
 *
 *  Note: The caller should abort any ongoing transtion by calling
 *        QmeshGenericOnOffAbortTransition before calling this API.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericOnOffUpdate (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T
                                     *model_context, uint8_t onoff)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s %d \n",
                      __FUNCTION__, __LINE__);
    QmeshMutexLock (&model_context->onoff_mutex);
    model_context->onoff_state.cur_onoff = onoff;
    model_context->onoff_state.initial_onoff = onoff;
    model_context->onoff_state.trgt_onoff = onoff;
    /* Update NVM */
    qmeshGenericOnOffUpdateNvm(model_context);
    QmeshMutexUnlock (&model_context->onoff_mutex);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericOnOffAbortTransition
 *
 *  DESCRIPTION
 *      This function aborts any ongoing transition and resets the state to
 *  initial state of the transtion.
 *
 *  RETURNS/MODIFIES
 *      TRUE if transition timer is started successfully. FALSE otherwise.
 *
 *----------------------------------------------------------------------------*/
extern void  QmeshGenericOnOffAbortTransition (void *context)
{
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context =
        (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *)context;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s:\n", __FUNCTION__);
    QmeshMutexLock (&model_context->onoff_mutex);

    /* Cancel the current transition timer and Publish timer */
    if (model_context->onoff_state.cur_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->onoff_state.cur_timer);

    if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

    if (model_context->publish_retrans_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

    /* Current and Target value must be set to Initial value */
    model_context->onoff_state.cur_onoff = model_context->onoff_state.trgt_onoff
                                           =  model_context->onoff_state.initial_onoff;

    /* Update NVM */
    qmeshGenericOnOffUpdateNvm(model_context);

    model_context->transition_time = QMESH_MODEL_TRANSITION_NOT_INPROGRESS;
    model_context->delay = 0;
    model_context->remaining_time = QMESH_MODEL_TRANSITION_NOT_INPROGRESS;
    QmeshMemSet (model_context->onoff_state.trgt_time, 0,
                 sizeof (model_context->onoff_state.trgt_time));

    QmeshMutexUnlock (&model_context->onoff_mutex);
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
extern bool QmeshGenericOnOffUpdatePublicationPeriod (QMESH_MODEL_DATA_T
        *model_data)
{
    uint8_t no_of_steps, step_resolution;
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context = NULL;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s:\n", __FUNCTION__);
    /* Get the model context data for the element ID 'elm_addr' from the composition data */
    model_context = (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *)
                    model_data->model_priv_data;
    /* 6 bits number of steps */
    no_of_steps = model_data->publish_state->publish_period.steps;
    /* 2 bits step resolution */
    step_resolution = model_data->publish_state->publish_period.resolution;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
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
 *      QmeshGenericOnOffPublicationHandler
 *
 *  DESCRIPTION
 *      Publication configuration handler for Generic OnOff model
 *
 *  RETURNS/MODIFIES
 *      None
 *----------------------------------------------------------------------------*/
extern void QmeshGenericOnOffPublicationHandler (QMESH_MODEL_DATA_T *model_data)
{
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context = NULL;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s:\n", __FUNCTION__);
    /* Get the model context from the composition data */
    model_context = (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *)
                    model_data->model_priv_data;

    if (model_context == NULL)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "Context data is NULL \n");
        return;
    }

    model_context->publish_state = model_data->publish_state;

    if (QmeshGenericOnOffUpdatePublicationPeriod (model_data))
    {
        if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        {
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

            /* Start publish timer based on publish interval */
           if ((model_context->publish_interval > 0) &&
                (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
                ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericOnOffPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
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
 *      QmeshGenericOnOffServerAppHandler
 *
 *  DESCRIPTION
*       Generic OnOff server model handler callback registered with the mesh
 *  stack. This function gets called from the model mesh stack to send the model message.
 *  This API will also be called from other extended models which has bound
 *  state with this model.
 *
 *  RETURNS/MODIFIES
 *      returns SUCCESS if message is handled sucessfully else FALSE
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericOnOffServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len)
{
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context = NULL;
    QMESH_MODEL_MSG_COMMON_T msg_cmn;// = {};
    QMESH_MODEL_MSG_T model_msg;// = {};
    uint32_t opcode, seq_no;
    uint16_t current_time[3], src_addr;
   /* Get the current time in ms */
    QmeshGetCurrentTimeInMs (current_time);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s:\n", __FUNCTION__);
    /* Get the model context from the composition data */
    model_context = (QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *)
                    model_data->model_priv_data;

    if ((model_context == NULL) ||
        (!msg || msg_len <= 0))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "Context data is NULL or msg is invalid \n");
        return QMESH_RESULT_FAILURE;
    }

    /* If the message is the last processed then ignore the message */
    if (model_context->last_msg_seq_no == nw_hdr->seq_num &&
        model_context->last_src_addr == nw_hdr->src_addr)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s: seq num 0x%08x processed\n", __FUNCTION__, nw_hdr->seq_num);
        return QMESH_RESULT_SUCCESS;
    }

    /* Retrieve the opcode from the message */
    if (get_opcode_format (msg[MODEL_OPCODE_OFFSET]) ==
        QMESH_OPCODE_FORMAT_TWO_BYTE)
    {
        opcode = QMESH_PACK_TWO_BYTE_OPCODE (msg[0], msg[1]);
        msg += QMESH_OPCODE_FORMAT_TWO_BYTE;
        msg_len -= QMESH_OPCODE_FORMAT_TWO_BYTE;
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s: Opcode:0x%x, seq_num=0x%08x\n",
                          __FUNCTION__, opcode, nw_hdr->seq_num);
    }
    else
    {
        /* Generic OnOff server handles only opcode of 2 bytes in length */
        return QMESH_RESULT_FAILURE;
    }

    QmeshMemSet(&msg_cmn, 0, sizeof(msg_cmn));

    /* Store the common message parameters */
    msg_cmn.opcode = opcode;
    msg_cmn.src = nw_hdr->src_addr;
    msg_cmn.dst = nw_hdr->dst_addr;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s:before lock\n",
                      __FUNCTION__);
    QmeshMutexLock (&model_context->onoff_mutex);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s:after lock\n",
                      __FUNCTION__);
    QmeshMemCpy (&model_context->key_info, key_info,
                 sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));
    /* Store the previous seqence number and source address */
    seq_no = model_context->last_msg_seq_no;
    src_addr = model_context->last_src_addr;
    /* Store the sequence number and src address of the new message */
    model_context->last_msg_seq_no = nw_hdr->seq_num;
    model_context->last_src_addr = nw_hdr->src_addr;

    /* Handle the OnOff server messages */
    switch (opcode)
    {
        case QMESH_GENERIC_ONOFF_GET:
        {
            /* Check the message length */
            if (msg_len != GENERIC_ONOFF_GET_MSG_LEN)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                                  "%s: Invalid message length \n", __FUNCTION__);
                QmeshMutexUnlock (&model_context->onoff_mutex);
                return QMESH_RESULT_SUCCESS;
            }
        }
        break;

        case QMESH_GENERIC_ONOFF_SET:
        case QMESH_GENERIC_ONOFF_SET_UNRELIABLE:
        {
            /* Check the message length */
            if (! (msg_len == GENERIC_ONOFF_SET_NON_TRANS_MSG_LEN ||
                   msg_len == GENERIC_ONOFF_SET_TRANS_MSG_LEN))
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                                  "%s:Invalid message length \n", __FUNCTION__);
                QmeshMutexUnlock (&model_context->onoff_mutex);
                return QMESH_RESULT_SUCCESS;
            }

            /* Check if OnOff state value is valid. 0 or 1 are valid */
            if (msg[GENERIC_ONOFF_VALUE_OFFSET] > 1)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                                  "%s %d INVALID value for onoff state=0x%x. Dropping the message!!\n",
                                  __FUNCTION__, __LINE__, msg[GENERIC_ONOFF_VALUE_OFFSET]);
                QmeshMutexUnlock (&model_context->onoff_mutex);
                return QMESH_RESULT_SUCCESS;
            }

            /* Retrieve the Transaction ID */
            msg_cmn.tid  = msg[GENERIC_ONOFF_TID_OFFSET];

            /* Check if the message is in the 6 seconds window cache. Returns
               TRUE if the message is a 'new' message and cache is updated with
               the 'new' message */
            if (QmeshModelCacheAddMsg (&msg_cmn, FALSE))
            {
                /* New message is valid - hence consume */
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                                  "%s %d Message added to the Cache \n",
                                  __FUNCTION__, __LINE__);
                /* Copy the message data - Required to send response and to
                   transition to the target state */
                QmeshMemCpy (&model_msg.cmn_msg, &msg_cmn,
                             sizeof (QMESH_MODEL_MSG_COMMON_T));
                QmeshMemCpy (&model_msg.key_info, key_info,
                             sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));
                QmeshMemCpy (&model_msg.timestamp,
                             current_time,
                             sizeof (current_time));

                model_msg.transition_time =
                    GENERIC_ONOFF_GET_TRANS_TIME (opcode,msg, msg_len,
                                                  elem_data);
                model_context->transition_time = model_msg.transition_time;
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                                  "%s: Transition time=0x%x \n", __FUNCTION__,
                                  model_msg.transition_time);

                /* If transition time is invalid, return */
                if (NUM_OF_TRANSITION_STEPS (model_msg.transition_time) ==
                    QMESH_MODEL_INVALID_TRANSITION_TIME)
                {
                    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                                      "%s: Invalid number of steps\n", __FUNCTION__);
                    QmeshMutexUnlock (&model_context->onoff_mutex);
                    return QMESH_RESULT_SUCCESS;
                }

                model_msg.delay = GENERIC_ONOFF_GET_DELAY_TIME (
                                      opcode, msg, msg_len, elem_data);
                model_context->delay = model_msg.delay;
                /* If Delay is present and non-zero - add the message
                   into the 'Delay' cache */
#if 0
                if (model_msg.delay > 0)
                {
                    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                                      " mutex unlocked beforemsg added to delay cache \n");
                    /* qmeshGenericOnOffTransitionHandler gets called immediately so
                       unlock the mutex */
                    QmeshMutexUnlock (&model_context->onoff_mutex);

                    /* Add the message into the delay cache */
                    if (QmeshAddMsgToDelayCache (&model_msg,
                                                 qmeshGenericOnOffTransitionHandler,
                                                 model_context,
                                                 msg,
                                                 msg_len)
                        != QMESH_RESULT_SUCCESS)
                    {
                        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                                          " Failed to add the message into the Delay cache\n");
                        QmeshMutexUnlock (&model_context->onoff_mutex);
                        return QMESH_RESULT_SUCCESS;
                    }

                    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                                      " mutex locked after msg added to delay cache \n");
                    QmeshMutexLock (&model_context->onoff_mutex);
                }
                else
#endif
                {
                    /* Delay is zero, hence start the transition to the target
                       state */
                    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                                      "%s: Delay NOT present or Zero\n", __FUNCTION__);
                    QmeshMutexUnlock (&model_context->onoff_mutex);

                    /* Alloc for model_msg */
                    model_msg.msg = (uint8_t *)QmeshMalloc (QMESH_MM_SECTION_MODEL_LAYER, msg_len);
                    if(model_msg.msg != NULL)
                    {
                      QmeshMemCpy (model_msg.msg, msg, msg_len);
                      model_msg.msg_len = msg_len;
                      qmeshGenericOnOffTransitionHandler (&model_msg, model_context);
                      QmeshFree((void*)model_msg.msg);
                    }
                    QmeshMutexLock (&model_context->onoff_mutex);
                }
            }
            else
            {
                /*
                    Message is in the cache (i.e., within 6 seconds window),
                    it means one of following,
                    1. Duplicate message for the current TID,
                       i.e., same message received within 6 seconds
                    2. Old Message with aborted TID. i.e., message
                       received within 6 seconds but TID is not current
                       TID - aborted messages.
                    Either case ignore the message
                */
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                                  "%s %d Message is in the cache - Hence dropping\n",
                                  __FUNCTION__, __LINE__);
                QmeshMutexUnlock (&model_context->onoff_mutex);
                return QMESH_RESULT_SUCCESS;
            }
        }
        break;

        default:
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                              "%s: Not a OnOff message\n", __FUNCTION__);
            /* Retore the previous sequence number and src address - since the message is not processed */
            model_context->last_msg_seq_no = seq_no;
            model_context->last_src_addr = src_addr;
            QmeshMutexUnlock (&model_context->onoff_mutex);
            return QMESH_RESULT_FAILURE;
        }
    }

    /* Send the Generic OnOff status to the remote device */
    QmeshGenericOnOffStatusSend (model_context, &msg_cmn);
    QmeshMutexUnlock (&model_context->onoff_mutex);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                      "%s: storing seq num 0x%08x \n", __FUNCTION__, nw_hdr->seq_num);
    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericOnOffServerAppInit
 *
 *  DESCRIPTION
 *      This function initializes Generic OnOff Server model data, registers
 *  the model with the stack and registers Application
 *  callback(QmeshGenericOnOffServerAppHandler).
 *
 *  RETURNS/MODIFIES
 *      QMESH_RESULT_SUCCESS on success.
 *      QMESH_RESULT_FAILURE on failure
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericOnOffServerAppInit (
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s\n", __FUNCTION__);

    if (model_context == NULL)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s: Context is NULL!!\n", __FUNCTION__);
        return QMESH_RESULT_FAILURE;
    }

    /* Create separate mutexes for all instances of generic level server. If
     * mutex creation fails, return failure.
     */
    if (QmeshMutexCreate (&model_context->onoff_mutex) !=
        QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER,
                          "%s:Creating mutex Failed\n", __FUNCTION__);
        /* Destory the mutex */
        QmeshMutexDestroy (&model_context->onoff_mutex);
        return QMESH_RESULT_FAILURE;
    }

    model_context->transition_time
        = QMESH_MODEL_TRANSITION_NOT_INPROGRESS;
    model_context->delay = 0;
    model_context->remaining_time = 0;
    model_context->onoff_state.cur_timer = QMESH_TIMER_INVALID_HANDLE;
    model_context->publish_timer = QMESH_TIMER_INVALID_HANDLE;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s pub interval=0x%4x\n",
                      __FUNCTION__, model_context->publish_interval);
    QmeshMemSet (model_context->status, 0,
                 sizeof (model_context->status));
    QmeshMemSet (model_context->onoff_state.trgt_time, 0,
                 sizeof (model_context->onoff_state.trgt_time));
    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericOnOffServerDeInit
 *
 *  DESCRIPTION
 *      This function resets the Generic OnOff Server model data and
 *  unregisters the model with the stack.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericOnOffServerAppDeInit (
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *model_context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s %d \n",
                      __FUNCTION__, __LINE__);
    /* Abort on-going transitions if any and reset the state data */
    QmeshGenericOnOffAbortTransition (model_context);
    QmeshMutexDestroy (&model_context->onoff_mutex);
}
