/******************************************************************************
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_model_generic_powerlevel_handler.c
 *  \brief defines and functions for SIGmesh Generic PowerLevel Server model.
 *
 *   This file contains the implementation of Generic PowerLevel Server model.
 *  This file should be built as part of the model Application.
 */
/******************************************************************************/

#include "qmesh_hal_ifce.h"
#include "qmesh_types.h"
#include "qmesh_model_common.h"
#include "qmesh_model_debug.h"

#include "qmesh_model_config.h"
#include "qmesh_cache_mgmt.h"
#include "qmesh_generic_powerlevel_handler.h"
#include "qmesh_model_nvm.h"

/*=============================================================================*
 *  Private Function Prototypes
 *============================================================================*/
static void qmeshGenericPowerLevelPublishTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context);
static void qmeshGenericPowerLevelPublishRetransTimerCb (
    QMESH_TIMER_HANDLE_T timerHandle,
    void *context);
static void qmeshPowerLvlNonLinearTransitionTimerCb (
    QMESH_TIMER_HANDLE_T timerHandle,
    void *context);
static void qmeshPowerLevelTransitionTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context);
/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshTrimPowerLevel
 *
 *  DESCRIPTION
 *      The API trims the Power Level within the Power Level Range Minimum and
 *  Power Level Range Maximum values.
 *
 *  RETURNS/MODIFIES
 *      Power Level value within the Power Level Range
 *
 *----------------------------------------------------------------------------*/
static uint16_t qmeshTrimPowerLevel (int32_t pwr_level,
                                   QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context)
{
    if (model_context->power_range.pwr_range_min == 0 &&
        model_context->power_range.pwr_range_max == 0)
    {
        DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                           "%s: value NOT trimmed \n", __FUNCTION__);
        return pwr_level;
    }

    if (pwr_level < model_context->power_range.pwr_range_min)
        return model_context->power_range.pwr_range_min;
    else if (pwr_level > model_context->power_range.pwr_range_max)
        return model_context->power_range.pwr_range_max;
    else
        return pwr_level;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelContextReset
 *
 *  DESCRIPTION
 *      The API resets the state context values related to the current transition
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
void QmeshGenericPowerLevelContextReset (QMESH_GENERIC_POWERLEVEL_CONTEXT_T
                                    *model_context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,  "%s: \n", __FUNCTION__);

    if (model_context->pwr_actual_state.trans_data.trans_timer != QMESH_TIMER_INVALID_HANDLE)
       QmeshTimerDelete (&model_timer_ghdl,&model_context->pwr_actual_state.trans_data.trans_timer);

    if (model_context->pwr_actual_state.trans_data.nl_trans_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->pwr_actual_state.trans_data.nl_trans_timer);

    if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

    if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

    model_context->pwr_actual_state.trans_data.rem_count = 0;
    model_context->pwr_actual_state.trans_data.linear_value = 0;
    model_context->pwr_actual_state.trans_data.step_resolution = 0;
    model_context->pwr_actual_state.trans_data.transition_time = 0;
    model_context->pwr_actual_state.trans_data.adjust_step_resolution = FALSE;
    QmeshMemSet (model_context->pwr_actual_state.trgt_time, 0,
                 sizeof (model_context->pwr_actual_state.trgt_time));
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelAbortTransition
 *
 *  DESCRIPTION
 *      The API aborts the current running transition
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelAbortTransition (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,  "%s:\n", __FUNCTION__);
    QmeshMutexLock (&model_context->pwr_lvl_mutex);
    QmeshGenericPowerLevelContextReset (model_context);
    QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      startPowerLevelTransition
 *
 *  DESCRIPTION
 *      The API starts a new transition by incrementing the current state to
 *  the target state linearly.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void startPowerLevelTransition (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context,
    int32_t delta_level,
    uint16_t timestamp[])
{
    uint8_t num_steps, step_resolution;
    /* Retrieve 'num_steps' from 'transition_time' */
    num_steps = NUM_OF_TRANSITION_STEPS
                (model_context->pwr_actual_state.trans_data.transition_time);
    /* Retrieve 'step_resolution' from 'transition_time' */
    step_resolution = STEP_RESOLUTION
                      (model_context->pwr_actual_state.trans_data.transition_time);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                      "%s:num_steps=0x%x,  step_resolution=0x%x\n",
                      __FUNCTION__, num_steps, step_resolution);

    /* Current and Target state are equal. Just return the status */
    if (delta_level ==  0)
    {
        return;
    }
    else if (num_steps == 0) /* 'num_steps' is 0 so  update instantly */
    {
        /* set the value within the range */
        model_context->pwr_actual_state.current_pwr_actual =
            model_context->pwr_actual_state.trgt_pwr_actual;
        model_context->pwr_actual_state.initial_pwr_actual =
            model_context->pwr_actual_state.current_pwr_actual;
        QmeshUpdateBoundStates (model_context->elm_addr,
                                QAPP_PWR_LVL_ACTUAL_UPDATED,
                                &model_context->pwr_actual_state.current_pwr_actual);

        /* Update power last whenever power actual chages to non-zero value */
        if (model_context->pwr_actual_state.current_pwr_actual)
        {
            model_context->power_last =
                model_context->pwr_actual_state.current_pwr_actual;
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                              "Power Last updated to  0x%x\n",
                              model_context->power_last);
        }

        /* Update NVM */
        QmeshGenericPowerLevelUpdateNvm(model_context);

        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                          "Updated Power Level: 0x%x\n",
                          model_context->pwr_actual_state.current_pwr_actual);
        /* Reset transition related variables */
        QmeshGenericPowerLevelContextReset (model_context);

        /* Send Message to Publish Address Immediately */
        if (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS)
            QmeshGenericPowerLevelStatusPublish (model_context);

        /* If publish time is valid (non-zero) start the publish timer */
        if (model_context->publish_interval != 0 &&
            model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
            ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericPowerLevelPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
        {
            DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                               "Publish Timer Creation Failed > 2 secs or Publish data is not set\n");
        }
    }
    else /* Start the transition timer for state update */
    {
        uint16_t cur_time_us[3];
        uint32_t cur_time_ms, nl_trans_time_ms;
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                          "%s:Start transition\n", __FUNCTION__);

        /* Calculate the transition time in milliseconds */
        if (step_resolution ==  QMESH_MODEL_STEP_RESOLUTION_100_MSEC)
            model_context->pwr_actual_state.trans_data.step_resolution =
                QMESH_100_MILLI_SEC;
        else if (step_resolution ==  QMESH_MODEL_STEP_RESOLUTION_1_SEC)
            model_context->pwr_actual_state.trans_data.step_resolution = QMESH_SEC;
        else if (step_resolution ==  QMESH_MODEL_STEP_RESOLUTION_10_SEC)
            model_context->pwr_actual_state.trans_data.step_resolution = QMESH_10_SEC;
        else if (step_resolution ==  QMESH_MODEL_STEP_RESOLUTION_10_MINUTES)
            model_context->pwr_actual_state.trans_data.step_resolution = QMESH_10_MINUTES;

        model_context->pwr_actual_state.trans_data.trans_time_ms =
            (num_steps * model_context->pwr_actual_state.trans_data.step_resolution);
        /* Set transition values for other level messages. */
        /* Get the current time in ms */
        QmeshGetCurrentTimeInMs (cur_time_us);
        cur_time_ms = ((uint32_t)cur_time_us[1] << 16) | (uint32_t) (cur_time_us[0]);
        /* Calculate the target time in,
           milliseconds = current time + delay + transition time */
        model_context->pwr_actual_state.target_time =
            (((uint32_t)timestamp[1] << 16) | (uint32_t) (timestamp[0]));
        model_context->pwr_actual_state.target_time +=
            (model_context->pwr_actual_state.delay * 5) +
            (model_context->pwr_actual_state.trans_data.trans_time_ms);
        /* Get the non linear transition time */
        nl_trans_time_ms = model_context->pwr_actual_state.target_time
                           - cur_time_ms;

        /* Absolute value of 'delta_level' is greater than equal to'num_steps'. */
        if (ABS_VAL (delta_level) >= num_steps)
        {
            model_context->pwr_actual_state.trans_data.rem_count = num_steps;
            model_context->pwr_actual_state.trans_data.linear_value = delta_level /
                    num_steps;
            model_context->pwr_actual_state.trans_data.adjust_step_resolution = FALSE;
        }
        else /* Absolute value of 'delta_level' is less than 'num_steps' */
        {
            model_context->pwr_actual_state.trans_data.rem_count =  ABS_VAL (delta_level);
            model_context->pwr_actual_state.trans_data.linear_value =
                delta_level / ABS_VAL (delta_level) ;
            model_context->pwr_actual_state.trans_data.step_resolution *=
                (num_steps / ABS_VAL (delta_level) );
            model_context->pwr_actual_state.trans_data.adjust_step_resolution = TRUE;
        }

        model_context->pwr_actual_state.trans_data.nl_trans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshPowerLvlNonLinearTransitionTimerCb,
                   (void *)model_context,
                   nl_trans_time_ms);
        if(model_context->pwr_actual_state.trans_data.nl_trans_timer == QMESH_TIMER_INVALID_HANDLE)
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                              "Non Linear Transition Timer Creation Failed\n");
        }

        /* Send additional status message if transition time is greater than or equal to 2 secs.
         See Model specificaiton section 7.4.1.3*/
        if (model_context->pwr_actual_state.trans_data.trans_time_ms >=
            TRANSITION_TIME_PUBLISH_THRESHOLD)
        {
            /* Send additional status message*/
            if (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS)
                QmeshGenericPowerLevelStatusPublish (model_context);
        }

        /* Start the timer of time 'step_resolution'  for 'rem_count' times */
        model_context->pwr_actual_state.trans_data.trans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshPowerLevelTransitionTimerCb,
                   (void *)model_context,
                   model_context->pwr_actual_state.trans_data.step_resolution);

        if(model_context->pwr_actual_state.trans_data.trans_timer == QMESH_TIMER_INVALID_HANDLE)
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                              "Transition Timer Creation Failed for linear transition time\n");
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      getPowerLevelStatus
 *
 *  DESCRIPTION
 *      Returns the Power Level Status
 *
 *  RETURNS/MODIFIES
 *      Power Level Status
 *
 *----------------------------------------------------------------------------*/
static void getPowerLevelStatus (QMESH_GENERIC_POWERLEVEL_CONTEXT_T
                                 *model_context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s: \n", __FUNCTION__);
    /* Get the opcode, present level and target level */
    model_context->status[0] = (uint8_t) (QMESH_GENERIC_POWER_LEVEL_STATUS >> 8);
    model_context->status[1] = (uint8_t)QMESH_GENERIC_POWER_LEVEL_STATUS;
    model_context->status[2] = (uint8_t)
                               model_context->pwr_actual_state.current_pwr_actual;
    model_context->status[3] = (uint8_t) (
                                   model_context->pwr_actual_state.current_pwr_actual >> 8);
    model_context->status[4] = (uint8_t)
                               model_context->pwr_actual_state.trgt_pwr_actual;
    model_context->status[5] = (uint8_t) (
                                   model_context->pwr_actual_state.trgt_pwr_actual >> 8);

    /* Transition NOT in progress */
    if (model_context->pwr_actual_state.current_pwr_actual ==
        model_context->pwr_actual_state.trgt_pwr_actual)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                          "%s: non-trans \n", __FUNCTION__);
        /* Remaining time should be 0 */
        model_context->status[6] = 0;
    }
    else  /* Transition in progress */
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                          "%s: Trans on-going\n", __FUNCTION__);
        /* Get the remaining time */
        model_context->status[6] = QmeshGetRemainingTime (
                                       model_context->pwr_actual_state.target_time);
    }
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      getPowerDefaultStatus
 *
 *  DESCRIPTION
 *      Returns the Power Default Status
 *
 *  RETURNS/MODIFIES
 *      Power Default Status
 *
 *----------------------------------------------------------------------------*/
static void getPowerDefaultStatus (QMESH_GENERIC_POWERLEVEL_CONTEXT_T
                                   *model_context)
{
    /* Get the opcode, present level and target level */
    model_context->status[0] = (uint8_t) (QMESH_GENERIC_POWER_DEFAULT_STATUS >> 8);
    model_context->status[1] = (uint8_t)QMESH_GENERIC_POWER_DEFAULT_STATUS;
    model_context->status[2] = (uint8_t)model_context->power_default;
    model_context->status[3] = (uint8_t) (model_context->power_default >> 8);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      getPowerLastStatus
 *
 *  DESCRIPTION
 *      Returns the Power Last Status
 *
 *  RETURNS/MODIFIES
 *      Power Default Status
 *
 *----------------------------------------------------------------------------*/
static void getPowerLastStatus (QMESH_GENERIC_POWERLEVEL_CONTEXT_T
                                *model_context)
{
    /* Get the opcode, present level and target level */
    model_context->status[0] = (uint8_t) (QMESH_GENERIC_POWER_LAST_STATUS >> 8);
    model_context->status[1] = (uint8_t)QMESH_GENERIC_POWER_LAST_STATUS;
    model_context->status[2] = (uint8_t)model_context->power_last;
    model_context->status[3] = (uint8_t) (model_context->power_last >> 8);
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      getPowerRangeStatus
 *
 *  DESCRIPTION
 *      Returns the Power Range Status
 *
 *  RETURNS/MODIFIES
 *      Power Range Status
 *
 *----------------------------------------------------------------------------*/
static void getPowerRangeStatus (QMESH_GENERIC_POWERLEVEL_CONTEXT_T
                                 *model_context)
{
    /* Get the opcode, present level and target level */
    model_context->status[0] = (uint8_t) (QMESH_GENERIC_POWER_RANGE_STATUS >> 8);
    model_context->status[1] = (uint8_t)QMESH_GENERIC_POWER_RANGE_STATUS;
    model_context->status[2] = (uint8_t)model_context->power_range.status_code;
    model_context->status[3] = (uint8_t) (model_context->power_range.pwr_range_min);
    model_context->status[4] = (uint8_t) (model_context->power_range.pwr_range_min >>
                                        8);
    model_context->status[5] = (uint8_t) (model_context->power_range.pwr_range_max);
    model_context->status[6] = (uint8_t) (model_context->power_range.pwr_range_max >>
                                        8);
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshGenericPowerLevelTransitionHandler
 *
 *  DESCRIPTION
 *      This function can be called from,
 *      1. Delay cache upon expiry of 'delay' and to start 'Transition' to target state
 *      2. If 'Delay' is 0, will be called directly from 'Extended' model or
 *      3. If 'Delay' is 0, will be called from the 'message' handler function of this model for 'Non-extended' case.
 *
 *  Note: The caller should call this only if 'target' state is not equal to 'current' state.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
static void qmeshGenericPowerLevelTransitionHandler (
    QMESH_MODEL_MSG_T *model_msg,
    void *context)
{
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context =
        (QMESH_GENERIC_POWERLEVEL_CONTEXT_T *)context;
    int32_t delta_level;
    uint16_t msg_timestamp[3], opcode;

    /*
    1. Check if the 'state' is in transition (if yes, stop/abort) and change the current state to new state
        as per the 'Transition Time'
    2. If msg is reliable, send the 'status' msg back to the 'src' if msg is reliable
    3. If 'state' changed - then send the 'status' msg to the Publish addresses as mentioned in
        section 7.4.1.3 in Model spec.
    */

    /* Check if current state is in transition.If in transition, abort the transition */
    if (model_context->pwr_actual_state.current_pwr_actual !=
        model_context->pwr_actual_state.trgt_pwr_actual)
    {
        QmeshGenericPowerLevelAbortTransition (model_context);
    }

    QmeshMutexLock (&model_context->pwr_lvl_mutex);
    opcode = (uint16_t)model_msg->cmn_msg.opcode;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s:opcode=0x%x\n",
                      __FUNCTION__, opcode);
    /* Store data */
    model_context->pwr_actual_state.trans_data.transition_time =
        model_msg->transition_time;
    model_context->pwr_actual_state.delay = model_msg->delay;
    QmeshMemCpy (&model_context->key_info, &model_msg->key_info,
                 sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));
    QmeshMemCpy (msg_timestamp, model_msg->timestamp, sizeof (msg_timestamp));

    switch (opcode)
    {
        case QMESH_GENERIC_POWER_LEVEL_SET:
        case QMESH_GENERIC_POWER_LEVEL_SET_UNRELIABLE:
        {
            /* Get the target value */
            uint16_t temp = (uint16_t)QMESH_PACK_TWO_BYTE_OPCODE
                          (model_msg->msg[1], model_msg->msg[0]);

            /* If the new value is non-zero then trim the value within the Power Range*/
            if (temp > 0)
                temp = qmeshTrimPowerLevel (temp, model_context);

            model_context->pwr_actual_state.trgt_pwr_actual = temp;
            delta_level = model_context->pwr_actual_state.trgt_pwr_actual -
                          model_context->pwr_actual_state.current_pwr_actual;

            /* Update NVM */
            QmeshGenericPowerLevelUpdateNvm(model_context);

            QmeshUpdateBoundStates (model_context->elm_addr,
                                    QAPP_PWR_LVL_ACTUAL_TRANS_BEGIN,
                                    &model_context->pwr_actual_state.current_pwr_actual);
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                              "%s:Trimmed target value=0x%x, delta =%08d\n", __FUNCTION__,
                              model_context->pwr_actual_state.trgt_pwr_actual, delta_level);
            startPowerLevelTransition (model_context, delta_level, msg_timestamp);
        }
        break;

        default:
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s: \n", __FUNCTION__);
            break;
    }

    QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
}

/*----------------------------------------------------------------------------*
*  NAME
*      qmeshPowerLvlNonLinearTransitionTimerCb
*
*  DESCRIPTION
*      This function is called on the expiry of non linear timer and changes current value to target value
*
*  RETURNS/MODIFIES
*      None
*
*----------------------------------------------------------------------------*/
static void qmeshPowerLvlNonLinearTransitionTimerCb (
    QMESH_TIMER_HANDLE_T timerHandle,
    void *context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s:\n", __FUNCTION__);
    /* Retrieve the context */
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context =
        (QMESH_GENERIC_POWERLEVEL_CONTEXT_T *)context;
    QmeshMutexLock (&model_context->pwr_lvl_mutex);

    if (model_context->pwr_actual_state.trans_data.nl_trans_timer != QMESH_TIMER_INVALID_HANDLE)
    {
        /* Delete Linear Timer */
        QmeshTimerDelete (&model_timer_ghdl,&model_context->pwr_actual_state.trans_data.trans_timer);
    }

    /* Set Current level to Target Level */
    model_context->pwr_actual_state.current_pwr_actual =
        model_context->pwr_actual_state.trgt_pwr_actual;

    /* Update NVM */
    QmeshGenericPowerLevelUpdateNvm(model_context);

    QmeshUpdateBoundStates (model_context->elm_addr,
                            QAPP_PWR_LVL_ACTUAL_UPDATED,
                            &model_context->pwr_actual_state.current_pwr_actual);
    /* Reset transition related variables */
    QmeshGenericPowerLevelContextReset (model_context);
    QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
}


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
static void qmeshGenericPowerLevelPublishRetransTimerCb (
    QMESH_TIMER_HANDLE_T timerHandle,
    void *context)
{
    /* Retrieve the context */
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context =
        (QMESH_GENERIC_POWERLEVEL_CONTEXT_T *)context;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s, cnt=0x%x:\n",
                      __FUNCTION__,
                      model_context->publish_restrans_cnt);
    model_context ->publish_timer = QMESH_TIMER_INVALID_HANDLE;
    /* Send Status Message to Publish Address */
    QmeshGenericPowerLevelStatusPublish (model_context);
    model_context->publish_restrans_cnt--;

    /* Restart the timer */
    if (model_context->publish_restrans_cnt)
    {
        if ((model_context->publish_interval > 0) &&
            (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
            ((model_context->publish_retrans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericPowerLevelPublishRetransTimerCb,
                   (void *)model_context,
                   (model_context->publish_state->retransmit_param.interval_step + 1) * 50))
                   == QMESH_TIMER_INVALID_HANDLE))
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                              "Publish Retransmission Timer Creation Failed or publish data not set\n");
        }
    }
    else
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                          "Publish Retransmission done\n");
    }
}

/*----------------------------------------------------------------------------*
*  NAME
*      qmeshGenericPowerLevelPublishTimerCb
*
*  DESCRIPTION
*      This function sends the status message to assigned publish address on timer expiry
*
*  RETURNS/MODIFIES
*      None
*
*----------------------------------------------------------------------------*/
static void qmeshGenericPowerLevelPublishTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s:\n", __FUNCTION__);
    /* Retrieve the context */
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context =
        (QMESH_GENERIC_POWERLEVEL_CONTEXT_T *)context;
    QmeshMutexLock (&model_context->pwr_lvl_mutex);
    /* Send Status Message to Publish Address */
    QmeshGenericPowerLevelStatusPublish (model_context);

    /* If transition has ended,send the status message to the assigned public address periodically
         after every "Publish_Period" */
    if (model_context->pwr_actual_state.current_pwr_actual ==
        model_context->pwr_actual_state.trgt_pwr_actual)
    {
        /* If publish time is valid (non-zero) start the publish timer */
        if (model_context->publish_interval != 0 &&
            model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
            ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericPowerLevelPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
        {
            DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                               "Publish Timer Creation Failed > 2 secs or Publish data is not set\n");
        }

        model_context->publish_restrans_cnt =
            model_context->publish_state->retransmit_param.count;

        /* Retransmit status Message to the Publish Address periodically
            "publish_restrans_cnt" times at every
             retransmission interval = (Publish Retransmit Interval Steps + 1) * 50 millseconds */
        if (model_context->publish_restrans_cnt &&
           ((model_context->publish_retrans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericPowerLevelPublishRetransTimerCb,
                   (void *)model_context,
                   (model_context->publish_state->retransmit_param.interval_step + 1) * 50))
                   == QMESH_TIMER_INVALID_HANDLE))
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                              "Publish Retransmission Timer Creation Failed\n");
        }
    }

    QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshPowerLevelTransitionTimerCb
 *
 *  DESCRIPTION
 *      This function lineraly increments the value after transition timer
 *      expiry and restarts the transition timer for next linear value change.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
static void qmeshPowerLevelTransitionTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    /* Retrieve the context */
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context =
        (QMESH_GENERIC_POWERLEVEL_CONTEXT_T *)context;
    QmeshMutexLock (&model_context->pwr_lvl_mutex);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s:\n", __FUNCTION__);
    /* Delete  linear transition timer */
    QmeshTimerDelete (&model_timer_ghdl,&timerHandle);

    if (model_context->pwr_actual_state.trans_data.nl_trans_timer != QMESH_TIMER_INVALID_HANDLE)
    {
        /* Increment the level by the 'linear_value' value */
        model_context->pwr_actual_state.current_pwr_actual =
            qmeshTrimPowerLevel (model_context->pwr_actual_state.current_pwr_actual +
                                 model_context->pwr_actual_state.trans_data.linear_value,
                                 model_context);

        /* Update NVM */
        QmeshGenericPowerLevelUpdateNvm(model_context);

        QmeshUpdateBoundStates (model_context->elm_addr, QAPP_PWR_LVL_ACTUAL_UPDATED,
                                &model_context->pwr_actual_state.current_pwr_actual);
        /* Decrement 'trans_time_ms' by 'step_resolution' and 'rem_count' by 1 */
        model_context->pwr_actual_state.trans_data.trans_time_ms -=
            model_context->pwr_actual_state.trans_data.step_resolution;
        model_context->pwr_actual_state.trans_data.rem_count--;

        /* If rem_count is 1,adjust the linear_value or step_resolution according to
             adjust_step_resolution' flag */
        if (model_context->pwr_actual_state.trans_data.rem_count == 1)
        {
            /* If 'adjust_step_resolution' is TRUE, make 'step_resolution' equal to remaining
                 transition time 'trans_time_ms' */
            if (model_context->pwr_actual_state.trans_data.adjust_step_resolution)
            {
                if (model_context->pwr_actual_state.trans_data.step_resolution !=
                    model_context->pwr_actual_state.trans_data.trans_time_ms)
                    model_context->pwr_actual_state.trans_data.step_resolution =
                        model_context->pwr_actual_state.trans_data.trans_time_ms;
            }
            else /* Make 'linear_value' equal to remaining level 'rem_level' */
            {
                int16_t rem_level = model_context->pwr_actual_state.trgt_pwr_actual -
                                  model_context->pwr_actual_state.current_pwr_actual;

                if (model_context->pwr_actual_state.trans_data.linear_value != rem_level)
                    model_context->pwr_actual_state.trans_data.linear_value = rem_level;
            }
        }

        /* If 'rem_count' is non-zero, restart the transition timer for 'step_resolution' time */
        if (model_context->pwr_actual_state.trans_data.rem_count)
        {
            /* Start the timer of time value 'step_resolution' */
            model_context->pwr_actual_state.trans_data.trans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshPowerLevelTransitionTimerCb,
                   (void *)model_context,
                   model_context->pwr_actual_state.trans_data.step_resolution);


            if (model_context->pwr_actual_state.trans_data.trans_timer == QMESH_TIMER_INVALID_HANDLE)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                                  "Repeat Linear Transition Timer Creation Failed\n");
            }
        }
        else  /* If 'rem_count' is zero, transition is complete. Reset transition related variables and
                      send status message to assigned publication address */
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "Linear Transition done\n");
            /* Delete Non Linear Timer */
            QmeshTimerDelete (&model_timer_ghdl,&model_context->pwr_actual_state.trans_data.nl_trans_timer);
            /* Reset transition related variables */
            QmeshGenericPowerLevelContextReset (model_context);

            /* Send Message to Publish Address Immediately todo: get publish address */
            if (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS)
                QmeshGenericPowerLevelStatusPublish (model_context);

            if (model_context ->publish_timer != QMESH_TIMER_INVALID_HANDLE)
                QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

            /* If publish time is valid (non-zero) start the publish timer */
            if (model_context->publish_interval != 0 &&
                model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
                ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericPowerLevelPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
            {
                DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                                   "Publish Timer Creation Failed > 2 secs\n");
            }
        }
    }
    else
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                          "Transition Timer not running\n");
    }

    QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelUpdateNvm
 *
 *  DESCRIPTION
 *      This function writes the current state to NVM.
 *
 *  RETURNS/MODIFIES
 *      None.
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelUpdateNvm(QMESH_GENERIC_POWERLEVEL_CONTEXT_T
                                                     *model_context)
{
#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
    QMESH_MODEL_GEN_PWR_LEVEL_NVM_STATE_T nvm_state;

    nvm_state.current_pwr_actual = model_context->pwr_actual_state.current_pwr_actual;
    nvm_state.trgt_pwr_actual = model_context->pwr_actual_state.trgt_pwr_actual;
    nvm_state.power_last =model_context->power_last;
    nvm_state.power_default =model_context->power_default;
    nvm_state.pwr_range_min =model_context->power_range.pwr_range_min;
    nvm_state.pwr_range_max =model_context->power_range.pwr_range_max;
    nvm_state.last_msg_seq_no =model_context->last_msg_seq_no;
    nvm_state.last_src_addr =model_context->last_src_addr;

    /* Update NVM */
    NVMWrite_ModelGenPowerLevelState((uint8_t*)&nvm_state);
#endif
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
extern void QmeshGenericPowerLevelPublicationHandler (QMESH_MODEL_DATA_T
        *model_data)
{
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context = NULL;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s:\n", __FUNCTION__);
    /* Get the model context data from the composition data */
    model_context = (QMESH_GENERIC_POWERLEVEL_CONTEXT_T *)
                    model_data->model_priv_data;

    if (model_context == NULL)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                          "Context data is NULL \n");
        return;
    }

    model_context->publish_state = model_data->publish_state;

    if (QmeshGenericPowerLevelUpdatePublicationPeriod (model_data))
    {
        if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        {
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
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
                   qmeshGenericPowerLevelPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
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
 *      QmeshGenericOnOffUpdatePublicationPeriod
 *
 *  DESCRIPTION
 *      Calculates the publish interval from the Publish state data for the model.
 *
 *  RETURNS/MODIFIES
 *
 *----------------------------------------------------------------------------*/
extern bool QmeshGenericPowerLevelUpdatePublicationPeriod (
    QMESH_MODEL_DATA_T *model_data)
{
    uint8_t no_of_steps, step_resolution;
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context = NULL;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s:\n", __FUNCTION__);
    /* Get the model context data from the composition data */
    model_context = (QMESH_GENERIC_POWERLEVEL_CONTEXT_T *)
                    model_data->model_priv_data;
     model_context->publish_state = model_data->publish_state;
    /* 6 bits number of steps */
    no_of_steps = model_data->publish_state->publish_period.steps;
    /* 2 bits step resolution */
    step_resolution = model_data->publish_state->publish_period.resolution;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
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
 *      QmeshGenericPowerLevelActualGet
 *
 *  DESCRIPTION
 *      The API returns the current Power Actual state.
 *
 *  RETURNS/MODIFIES
 *      Current Power actual state
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshGenericPowerLevelActualGet (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T
    *model_context)
{
    return model_context->pwr_actual_state.current_pwr_actual;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelActualUpdate
 *
 *  DESCRIPTION
 *      The API sets the Power Actual state to new state value as specificed in the parameters.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelActualSet (QMESH_GENERIC_POWERLEVEL_CONTEXT_T
        *model_context,
        uint8_t transition_time,
        uint8_t delay,
        uint16_t pwr_actual)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s:cur_pwr_actl=0x%x \n",
                      __FUNCTION__, model_context->pwr_actual_state.current_pwr_actual);
    QmeshMutexLock (&model_context->pwr_lvl_mutex);
    /* Note: transition_time and delay params are for future requirements */
    QmeshGenericPowerLevelActualUpdate (model_context, pwr_actual);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelActualUpdate
 *
 *  DESCRIPTION
 *      The API updates the Power Actual state to new state value as specificed in the parameter.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelActualUpdate (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T
    *model_context,
    uint16_t pwr_actual)
{
    uint16_t pwr_lvl = 0;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s:cur_pwr_actl=0x%x \n",
                      __FUNCTION__, model_context->pwr_actual_state.current_pwr_actual);
    QmeshMutexLock (&model_context->pwr_lvl_mutex);

    /* Update the power acutal and publish the new state */
    /* If new value is non-zero then trim the new value as per Range Min and Range Max value */
    if (pwr_actual > 0)
    {
        pwr_lvl = qmeshTrimPowerLevel (pwr_actual, model_context);
    }

    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                      "%s:new Power_actual=0x%x\n",
                      __FUNCTION__, pwr_lvl);
    /* Update the state */
    model_context->pwr_actual_state.current_pwr_actual = pwr_lvl;
    model_context->pwr_actual_state.trgt_pwr_actual = pwr_lvl;
    model_context->pwr_actual_state.initial_pwr_actual = pwr_lvl;

    /* Update power last whenever power actual chages to non-zero value */
    if (model_context->pwr_actual_state.current_pwr_actual)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                          " QmeshGenericPowerLevelActualUpdate Power Last updated to  0x%x\n",
                          model_context->power_last);
        model_context->power_last = model_context->pwr_actual_state.current_pwr_actual;
    }

    /* Update NVM */
    QmeshGenericPowerLevelUpdateNvm(model_context);

    /* If power level is updated due to range then update the bound models */
    if(pwr_actual != pwr_lvl)
    {
        QmeshUpdateBoundStates (model_context->elm_addr,
                    QAPP_PWR_LVL_ACTUAL_UPDATED,
                    &pwr_lvl);
    }

    /* Publish the status message */
    if (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS)
        QmeshGenericPowerLevelStatusPublish (model_context);

    QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                      "QmeshGenericPowerLevelActualUpdate Updated Power Level: 0x%x\n",
                      model_context->pwr_actual_state.current_pwr_actual);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelDefaultGet
 *
 *  DESCRIPTION
 *      The API returns the Power Level Default value
 *
 *  RETURNS/MODIFIES
 *      Returns Power Level Default
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshGenericPowerLevelDefaultGet (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context)
{
    return model_context->power_default;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelDefaultSet
 *
 *  DESCRIPTION
 *      The API sets the Power Level Default value
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelDefaultSet (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context, uint16_t pwr_default)
{
    model_context->power_default = pwr_default;
    /* Update NVM */
    QmeshGenericPowerLevelUpdateNvm(model_context);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelLastGet
 *
 *  DESCRIPTION
 *      The API returns the Power Level Last value.
 *
 *  RETURNS/MODIFIES
 *      Returns Power Level Last
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshGenericPowerLevelLastGet (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context)
{
    return model_context->power_last;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelLastSet
 *
 *  DESCRIPTION
 *      The API sets the Power Level Last value.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelLastSet (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context, uint16_t  pwr_last)
{
    model_context->power_last = pwr_last;
    /* Update NVM */
    QmeshGenericPowerLevelUpdateNvm(model_context);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelMinRangeGet
 *
 *  DESCRIPTION
 *      The API returns the Power Level Range Minimum state value.
 *
 *  RETURNS/MODIFIES
 *      Returns Power Level Range Minimum state value
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshGenericPowerLevelMinRangeGet (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context)
{
    return model_context->power_range.pwr_range_min;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelMaxRangeGet
 *
 *  DESCRIPTION
 *      The API returns the Power Level Range Maximum state value.
 *
 *  RETURNS/MODIFIES
 *      Returns Power Level Range Maximum state value
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshGenericPowerLevelMaxRangeGet (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context)
{
    return model_context->power_range.pwr_range_max;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelStatusSend
 *
 *  DESCRIPTION
 *      This function sends the current Power Level Status message to the remote device as mentioned
 *  in 'msg_cmn' parameter.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelStatusSend (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context,
    QMESH_MODEL_MSG_COMMON_T *msg_cmn)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                      "%s: opcode=0x%x\n", __FUNCTION__,
                      msg_cmn->opcode);
    QmeshMemSet (model_context->status, 0, sizeof (model_context->status));

    switch (msg_cmn->opcode)
    {
        case QMESH_GENERIC_POWER_LEVEL_GET:
        case QMESH_GENERIC_POWER_LEVEL_SET:
        {
            /* Get the level status  */
            getPowerLevelStatus (model_context);

            if (model_context->status[POWER_LEVEL_STATUS_MSG_TIME_OFFSET] == 0)
            {
                data_info.payload_len = POWER_LEVEL_STATUS_NON_TRANS_MSG_LEN;
            }
            else
            {
                data_info.payload_len = POWER_LEVEL_STATUS_TRANS_MSG_LEN;
            }

            /* For POWER_LEVEL_SET message, send the remaining transition
             * time as received in the request message
             */
            if (msg_cmn->opcode == QMESH_GENERIC_POWER_LEVEL_SET)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                                  "%s: Setting Transition Time \n", __FUNCTION__);
                model_context->status[POWER_LEVEL_STATUS_MSG_TIME_OFFSET] =
                    model_context->pwr_actual_state.trans_data.transition_time ;
            }
        }
        break;

        case QMESH_GENERIC_POWER_LAST_GET:
        {
            QmeshMemSet (model_context->status, 0, sizeof (model_context->status));
            getPowerLastStatus (model_context);
            data_info.payload_len = POWER_LEVEL_LAST_STATUS_MSG_LEN;
        }
        break;

        case QMESH_GENERIC_POWER_RANGE_GET:
        case QMESH_GENERIC_POWER_RANGE_SET:
        {
            QmeshMemSet (model_context->status, 0, sizeof (model_context->status));
            getPowerRangeStatus (model_context);
            data_info.payload_len = POWER_LEVEL_RANGE_STATUS_MSG_LEN;
            /* Reset back to success */
            model_context->power_range.status_code = QMESH_MODEL_STATUS_SUCCESS;
        }
        break;

        case QMESH_GENERIC_POWER_DEFAULT_GET:
        case QMESH_GENERIC_POWER_DEFAULT_SET:
        {
            QmeshMemSet (model_context->status, 0, sizeof (model_context->status));
            getPowerDefaultStatus (model_context);
            data_info.payload_len = POWER_LEVEL_DEFAULT_STATUS_MSG_LEN;
        }
        break;

        default:
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                              "%s:\n", __FUNCTION__);
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

    if (QmeshSendMessage (&app_key_info, &data_info) != QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                          "%s: QmeshSendMessage failed\n", __FUNCTION__);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelStatusPublish
 *
 *  DESCRIPTION
 *      This function sends the current Power Level Status message to the Publish address assigned for
 *  this model.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelStatusPublish (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;
    QmeshMemSet (model_context->status, 0, sizeof (model_context->status));
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                      "%s:\n", __FUNCTION__);
    /* Get the level status  */
    getPowerLevelStatus (model_context);

    if (model_context->status[POWER_LEVEL_STATUS_MSG_TIME_OFFSET] == 0)
    {
        data_info.payload_len = POWER_LEVEL_STATUS_NON_TRANS_MSG_LEN;
    }
    else
    {
        data_info.payload_len = POWER_LEVEL_STATUS_TRANS_MSG_LEN;
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
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                          "%s: QmeshSendMessage failed\n", __FUNCTION__);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelServerAppHandler
 *
 *  DESCRIPTION
 *      This method is the Application model callback.
 *  This API will be registered with the model library (via GenericPowerLevelServerAppInit()). This API gets
 *  called from the model library whenever mesh stack sends the model message.
 *  This API will also be called from other extended models which has bound state with this model.
 *
 *  RETURNS/MODIFIES
 *      returns SUCCESS if message is handled sucessfully else FALSE
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericPowerLevelServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len)
{
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context = NULL;
    uint16_t current_time[3];
    QMESH_MODEL_MSG_T model_msg;
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
        {
            /* Get the model data for the GDTT model from the element data */
            QMESH_MODEL_DATA_T *mdl_data = QmeshGetModelData (elem_data,
                                           QMESH_MODEL_GENERIC_POWERONOFF);

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

            return QmeshGenericPowerOnOffSetupServerHandler (nw_hdr, key_info, elem_data,
                    mdl_data,
                    msg, msg_len);
        }

    }

    if (model_context->last_msg_seq_no == nw_hdr->seq_num &&
        model_context->last_src_addr == nw_hdr->src_addr)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                          "%s: seq num 0x%08x processed\n", __FUNCTION__, nw_hdr->seq_num);
        return QMESH_RESULT_SUCCESS;
    }

    QmeshMemSet(&msg_cmn, 0, sizeof(msg_cmn));

    /* Store the common message parameters */
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

    /* Handle Power Level model the messages */
    switch (opcode)
    {
        case QMESH_GENERIC_POWER_LEVEL_GET:
        {
            if (msg_len != POWER_LEVEL_GET_MSG_LEN)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s:dropping msg\n",
                                  __FUNCTION__);
                QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
                return QMESH_RESULT_SUCCESS;
            }
        }
        break;

        case QMESH_GENERIC_POWER_LEVEL_SET:
        case QMESH_GENERIC_POWER_LEVEL_SET_UNRELIABLE:
        {
            if (! (msg_len == POWER_LEVEL_SET_NON_TRANS_MSG_LEN ||
                   msg_len == POWER_LEVEL_SET_TRANS_MSG_LEN))
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s: dropping SET msg\n",
                                  __FUNCTION__);
                QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
                return QMESH_RESULT_SUCCESS;
            }

            /* Get the transaction identifier */
            msg_cmn.tid  = msg[GENERIC_POWERLEVEL_TID_OFFSET];
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                              "%s:QmeshModelCacheAddMsg\n", __FUNCTION__);

            /* Check if the message is in the cache. Returns TRUE if message is new message and
                 cache is updated with the new message */
            if (QmeshModelCacheAddMsg (&msg_cmn, FALSE))
            {
                /* New message is valid - hence consume */
                QmeshMemCpy (&model_msg.cmn_msg, &msg_cmn,
                             sizeof (QMESH_MODEL_MSG_COMMON_T));
                QmeshMemCpy (&model_msg.key_info, key_info,
                             sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));
                QmeshMemCpy (&model_msg.timestamp, current_time, sizeof (current_time));

                model_msg.transition_time = GENERIC_POWER_LEVEL_GET_TRANS_TIME (opcode,
                                            msg, msg_len, elem_data);
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                                  "%s:transition_time = 0x%x, msg_len=%d\n",
                                  __FUNCTION__, model_msg.transition_time, msg_len);

                /* If transition time is invalid, return */
                if (NUM_OF_TRANSITION_STEPS (model_msg.transition_time) ==
                    QMESH_MODEL_INVALID_TRANSITION_TIME)
                {
                    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                                      "%s:Invalid no. of steps\n", __FUNCTION__);
                    QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
                    QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
                    return QMESH_RESULT_SUCCESS;
                }

                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                                  "%s:Current state = 0x%x\n",
                                  __FUNCTION__,
                                  model_context->pwr_actual_state.current_pwr_actual);

                /* Check if new state and current state are same, if so nothing to process just send
                     the status msg */
                if (model_context->pwr_actual_state.current_pwr_actual ==
                    qmeshTrimPowerLevel ((msg[GENERIC_POWERLEVEL_VALUE_OFFSET] |
                                          (msg[GENERIC_POWERLEVEL_VALUE_OFFSET + 1] << 8)), model_context))
                {
                    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                                      "%s:Current state = New state. Send response\n",
                                      __FUNCTION__);
                    /* Copy Transition time */
                    model_context->pwr_actual_state.trans_data.transition_time =
                        model_msg.transition_time;
                    break;
                }

                model_msg.delay = GENERIC_POWER_LEVEL_GET_DELAY_TIME (opcode, msg,
                                  msg_len, elem_data);
#if 0
                /* If Delay is present and non-zero - add the message into 'Delay' cache */
                if (model_msg.delay > 0)
                {
                    if (QmeshAddMsgToDelayCache (&model_msg,
                                                 qmeshGenericPowerLevelTransitionHandler, model_context,msg,msg_len) !=
                        QMESH_RESULT_SUCCESS)
                    {
                        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                                          " Failed to add message into Delay cache\n");
                        QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
                        return QMESH_RESULT_SUCCESS;
                    }
                }
                else
#endif
                {
                    /* Delay is 0, so initiate transition */
                    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                                      "%s: No Delay, model_msg.cmn_msg.src=0x%x\n", __FUNCTION__,
                                      model_msg.cmn_msg.src);
                    /* Alloc for model_msg */
                    model_msg.msg = (uint8_t *)QmeshMalloc (QMESH_MM_SECTION_MODEL_LAYER, msg_len);

                    QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
                    if(model_msg.msg!=NULL)
                    {
                        QmeshMemCpy (model_msg.msg, msg, msg_len);
                        model_msg.msg_len = msg_len;
                        qmeshGenericPowerLevelTransitionHandler (&model_msg, model_context);
                        QmeshFree((void*)model_msg.msg);
                    }
                    QmeshMutexLock (&model_context->pwr_lvl_mutex);
                }
            }
            else
            {
                /*
                Message is in the cache (i.e., within 6 seconds window), it means one of following,
                     1. Duplicate message for the current TID, i.e., same message received within 6 seconds
                     2. Old Message with aborted TID. i.e., message received within 6 seconds but TID is
                         not current TID - aborted messages.
                Either case ignore the message
                */
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s: \n",
                                  __FUNCTION__);
                QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
                return QMESH_RESULT_SUCCESS;
            }
        }
        break;

        case QMESH_GENERIC_POWER_LAST_GET:
        {
            if (msg_len != POWER_LAST_GET_MSG_LEN)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s:dropping msg\n",
                                  __FUNCTION__);
                QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
                return QMESH_RESULT_SUCCESS;
            }
        }
        break;

        case QMESH_GENERIC_POWER_DEFAULT_GET:
        {
            if (msg_len != POWER_DEFAULT_GET_MSG_LEN)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s:dropping msg\n",
                                  __FUNCTION__);
                QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
                return QMESH_RESULT_SUCCESS;
            }
        }
        break;

        case QMESH_GENERIC_POWER_RANGE_GET:
        {
            if (msg_len != POWER_RANGE_GET_MSG_LEN)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER, "%s:dropping msg\n",
                                  __FUNCTION__);
                QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
                return QMESH_RESULT_SUCCESS;
            }
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

    QmeshMutexUnlock (&model_context->pwr_lvl_mutex);
    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelServerAppInit
 *
 *  DESCRIPTION
 *      Initialise Generic Power Level server  model and registers callback with the mesh stack.
 *
 *  RETURNS/MODIFIES
 *      none
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericPowerLevelServerAppInit (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,  "%s: \n", __FUNCTION__);

    /* Check if model_context is valid */
    if (model_context == NULL)
    {
        return QMESH_RESULT_FAILURE;
    }

    /* Create separate mutexes for all instances of generic level server. If
     * mutex creation fails, return failure.
     */
    if (QmeshMutexCreate (&model_context->pwr_lvl_mutex) !=
        QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                           "%s:Creating mutex Failed\n", __FUNCTION__);
        return QMESH_RESULT_FAILURE;
    }

    /* Reset the states */
    model_context->pwr_actual_state.target_time = 0;
    model_context->power_range.status_code = (uint8_t)
            QMESH_MODEL_STATUS_SUCCESS;
    model_context->publish_timer = QMESH_TIMER_INVALID_HANDLE;
    QmeshMemSet (model_context->pwr_actual_state.trgt_time, 0,
                 sizeof (model_context->pwr_actual_state.trgt_time));
    /* Reset transition related variables */
    QmeshGenericPowerLevelContextReset (model_context);

    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelServerAppDeInit
 *
 *  DESCRIPTION
 *      This function De-initializes Generic PowerLevel model state data and Unregisters the model from
 *  the stack.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericPowerLevelServerAppDeInit (
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *model_context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_PWR_LEVEL_SERVER,
                      "GenericPowerLevelServerAppDeInit\n");
    QmeshGenericPowerLevelAbortTransition (model_context);
    QmeshMutexDestroy (&model_context->pwr_lvl_mutex);
}
