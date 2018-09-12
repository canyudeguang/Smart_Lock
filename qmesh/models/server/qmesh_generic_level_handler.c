/******************************************************************************
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_generic_level_handler.c
 *  \brief defines and functions for SIGmesh Generic Level Handler
 *
 *   This file contains the implementation on Generic Level Handler
 */
/******************************************************************************/

/*=============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "qmesh_generic_level_handler.h"
#include "qmesh_model_nvm.h"

/*=============================================================================*
 *  Private Data
 *============================================================================*/

/* Message Timestamp Difference (6000 ms) */
/*static const uint16_t g_message_interval[3] = {0x1770, 0x0000, 0x0000};*/

/* Flag to check if generic delta set messages are called for the first time */
static bool g_first_iteration = TRUE;

/*=============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/* This function resets the state context values related to a transition */
static void resetStateTransitionContext (QMESH_GENERIC_LEVEL_CONTEXT_T
        *model_context);

/* This function sends the status message to assigned publish address on timer
 * expiry
 */
static void levelPublishTimerCb (QMESH_TIMER_HANDLE_T timerHandle, void *context);

/* This function is called on expiry of linear timer and changes current value
 * to target value linearly
 */
static void levelLinearTransitionTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context);

/* This function starts a new transition or updates the level state
 * instantaneously
 */
static void updateLevel (QMESH_GENERIC_LEVEL_CONTEXT_T *model_context,
                         int32_t delta_level);

/* The function sets the current level value */
static void setCurrentLevel (QMESH_GENERIC_LEVEL_CONTEXT_T *model_context,
                             int16_t new_level);

/*============================================================================*
*  Private Function Definitions
*============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshLevelUpdateNvm
 *
 *  DESCRIPTION
 *      This function writes the current state to NVM.
 *
 *  RETURNS/MODIFIES
 *      None.
 *
 *----------------------------------------------------------------------------*/
static void qmeshLevelUpdateNvm(void)
{
   /* Update NVM */
   NVMWrite_ModelGenWriteLevelState(); 
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      resetStateTransitionContext
 *
 *  DESCRIPTION
 *      This function resets the state context values related to a transition.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void resetStateTransitionContext (QMESH_GENERIC_LEVEL_CONTEXT_T
        *model_context)
{
    model_context->trans_data.trans_timer = QMESH_TIMER_INVALID_HANDLE;
    model_context->trans_data.rem_count = 0;
    model_context->trans_data.step_resolution = 0;
    model_context->trans_data.linear_value = 0;
    model_context->trans_data.trans_time_ms = 0;
    model_context->trans_data.adjust_step_resolution = FALSE;
    model_context->trans_data.transition_time =
        QMESH_MODEL_TRANSITION_NOT_INPROGRESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      adjustDeltaLevel
 *
 *  DESCRIPTION
 *      This function updates the level state
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static inline int32_t adjustDeltaLevel (int16_t cur_lvl, int32_t delta_lvl, int16_t max,
                                      int16_t min)
{
    int32_t trgt_lvl = (int32_t)cur_lvl + delta_lvl;
    return ((trgt_lvl > (int32_t) max) ?
            ((int32_t)max - (int32_t)cur_lvl) :
            ((trgt_lvl < (int32_t) min) ?
             ((int32_t)min - (int32_t)cur_lvl) :
             delta_lvl));
}

/*----------------------------------------------------------------------------*
*  NAME
*      qmeshGenericLevelPublishRetransTimerCb
*
*  DESCRIPTION
*      This function sends the level status message to the publish address for
*  the level model periodically every 'Publish_Period' until new tranistion
*  begins.
*
*  RETURNS/MODIFIES
*      None
*
*----------------------------------------------------------------------------*/
static void qmeshGenericLevelPublishRetransTimerCb (QMESH_TIMER_HANDLE_T
        timerHandle, void *context)
{
    /* Retrieve the context */
    QMESH_GENERIC_LEVEL_CONTEXT_T *model_context =
        (QMESH_GENERIC_LEVEL_CONTEXT_T *)context;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER, "%s, cnt=0x%x:\n",
                      __FUNCTION__,
                      model_context->publish_restrans_cnt);
    model_context ->publish_timer = QMESH_TIMER_INVALID_HANDLE;
    /* Send Status Message to Publish Address */
    QmeshGenericLevelStatusPublish (model_context);
    model_context->publish_restrans_cnt--;

    /* Restart the timer */
    if (model_context->publish_restrans_cnt)
    {
        if ((model_context->publish_interval > 0) &&
            (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
            ((model_context->publish_retrans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshGenericLevelPublishRetransTimerCb,
                   (void *)model_context,
                   (model_context->publish_state->retransmit_param.interval_step + 1) * 50))
                   == QMESH_TIMER_INVALID_HANDLE))
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                              "Publish Retransmission Timer Creation Failed or publish data not set\n");
        }
    }
    else
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                          "Publish Retransmission done\n");
    }
}


/*----------------------------------------------------------------------------*
*  NAME
*      levelPublishTimerCb
*
*  DESCRIPTION
*      This function sends the status message to assigned publish address on
*      timer expiry
*
*  RETURNS/MODIFIES
*      None
*
*----------------------------------------------------------------------------*/
static void levelPublishTimerCb (QMESH_TIMER_HANDLE_T timerHandle, void *context)
{
    /* Delete timer */
    QmeshTimerDelete (&model_timer_ghdl,&timerHandle);
    /* Retrieve the context */
    QMESH_GENERIC_LEVEL_CONTEXT_T *model_context =
        (QMESH_GENERIC_LEVEL_CONTEXT_T *)context;
    /* Lock the mutex */
    QmeshMutexLock (&model_context->level_mutex);
    /* Send Status Message to Publish Address */
    QmeshGenericLevelStatusPublish (model_context);
    /* Initialise the timer to 'QMESH_TIMER_INVALID_HANDLE' */
    model_context ->publish_timer = QMESH_TIMER_INVALID_HANDLE;

    /* Stop any retransmission of publish message */
    if (model_context->publish_retrans_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

    /* If transition has ended,send the status message to the assigned public
     * address periodically after every "Publish_Period"
     */
    if (model_context->cur_level ==  model_context->target_level)
    {
        /* If publish time is valid (non-zero) start the publish timer */
        if (model_context->publish_interval != 0 &&
            model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
            ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   levelPublishTimerCb,
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
                   qmeshGenericLevelPublishRetransTimerCb,
                   (void *)model_context,
                   (model_context->publish_state->retransmit_param.interval_step + 1) * 50))
                   == QMESH_TIMER_INVALID_HANDLE))
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                              "Publish Retransmission Timer Creation Failed\n");
        }
    }

    /* Unlock the mutex */
    QmeshMutexUnlock (&model_context->level_mutex);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *     levelLinearTransitionTimerCb
 *
 *  DESCRIPTION
 *      This function is called on the expiry of linear timer and changes
 *      current value to target value linearly
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
static void levelLinearTransitionTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    QAPP_GET_LVL_RANGE_T range = {MAX_LEVEL, MIN_LEVEL};
    /* Retrieve the context */
    QMESH_GENERIC_LEVEL_CONTEXT_T *model_context =
        (QMESH_GENERIC_LEVEL_CONTEXT_T *)context;
    /* Lock the mutex */
    QmeshMutexLock (&model_context->level_mutex);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER, "Linear Timer Expired \n");
    /* Increment the level by the 'linear_value' value */
    setCurrentLevel (model_context, (model_context->cur_level +
                                     model_context->trans_data.linear_value));

    /* For "MOVE_SET" messages, value should be changed indefinitely and
     * should stop changing on reaching MAX or MIN values
     */
    if (model_context->trans_data.rem_count == 0xFF)
    {
        /* Call application callback handler to adjust level value as per
        * extended model min and max values.
        */
        QmeshUpdateBoundStates (model_context->elm_addr, QAPP_GET_LVL_RANGE,
                                &range);
        /* Check if current value has reached its max or min level */
        int32_t temp = (int32_t) model_context->cur_level +
                     model_context->trans_data.linear_value;

        if ((temp > (int32_t) range.max_val) ||
            (temp < (int32_t) range.min_val))
        {
            /* As next linear value change can result in value going beyond
             * MAX and MIN levels, stop changing now
             */
            model_context->trans_data.rem_count = 0;
        }
    }
    else /* For other messages, decrement the rem_count and adjust the
          * linear_value or step_resolution if rem_count is 1
          */
    {
        /* Decrement 'trans_time_ms' by 'step_resolution' and
         * 'rem_count' by 1
         */
        model_context->trans_data.trans_time_ms -=
            model_context->trans_data.step_resolution;
        model_context->trans_data.rem_count--;

        /* If rem_count is 1,adjust the linear_value or step_resolution
         * according to 'adjust_step_resolution' flag
         */
        if (model_context->trans_data.rem_count == 1)
        {
            /* If 'adjust_step_resolution' is TRUE, make 'step_resolution'
             * equal to remaining transition time 'trans_time_ms'
             */
            if (model_context->trans_data.adjust_step_resolution)
            {
                if (model_context->trans_data.step_resolution !=
                    model_context->trans_data.trans_time_ms)
                {
                    model_context->trans_data.step_resolution =
                        model_context->trans_data.trans_time_ms;
                }
            }
            else /* Make 'linear_value' equal to remaining level 'rem_level'
                  */
            {
                int16_t rem_level = model_context->target_level -
                                  model_context->cur_level;

                if (model_context->trans_data.linear_value != rem_level)
                    model_context->trans_data.linear_value = rem_level;
            }
        }
    }

    /* If 'rem_count' is non-zero, restart the transition timer for
     * 'step_resolution' time
     */
    if (model_context->trans_data.rem_count)
    {
       /* Start the timer of time value 'step_resolution'*/
        model_context->trans_data.trans_timer = QmeshTimerCreate (&model_timer_ghdl,
                                                                  levelLinearTransitionTimerCb,
                                                                  (void *)model_context,
                                                                  model_context->trans_data.step_resolution);

        if(model_context->trans_data.trans_timer == QMESH_TIMER_INVALID_HANDLE)
        {
            DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                               "Linear Transition Timer Creation Failed\n");
        }
    }
    else /* If 'rem_count' is zero, transition is complete. Reset transition
          * related variables and send status message to assigned
          * publication address
          */
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                          "Linear Transition complete\n");
        /* Reset transition related variables */
        resetStateTransitionContext (model_context);

        /* Publish status message after transition is complete */
        if (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS)
            QmeshGenericLevelStatusPublish (model_context);

        if (model_context ->publish_timer != QMESH_TIMER_INVALID_HANDLE)
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

        /* If publish time is valid (non-zero) start the publish timer */
        if (model_context->publish_interval != 0 &&
            model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
            ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   levelPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
        {
            DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                               "Publish Timer Creation Failed > 2 secs\n");
        }
    }

    /* Unlock the mutex */
    QmeshMutexUnlock (&model_context->level_mutex);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      updateLevel
 *
 *  DESCRIPTION
 *      The function starts a new transition or updates the level state
 *      instantaneously
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void updateLevel (QMESH_GENERIC_LEVEL_CONTEXT_T *model_context,
                         int32_t delta_level)
{
    uint8_t num_steps, step_resolution;
    /* Retrieve 'num_steps' from 'transition_time' */
    num_steps =
        NUM_OF_TRANSITION_STEPS (model_context->trans_data.transition_time);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                      "Num steps: %x\n", num_steps);
    /* Retrieve 'step_resolution' from 'transition_time' */
    step_resolution =
        STEP_RESOLUTION (model_context->trans_data.transition_time);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                      "Resolution: %x\n", step_resolution);

    if (num_steps == 0) /* If 'num_steps' is 0,it is instantaneous update */
    {
        /* Update 'cur_level' by delta level */
        setCurrentLevel (model_context, model_context->target_level);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                          "cur level: %d\n", model_context->cur_level);

        /* Publish status message*/
        if (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS)
            QmeshGenericLevelStatusPublish (model_context);

        if (model_context ->publish_timer != QMESH_TIMER_INVALID_HANDLE)
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

        /* If publish time is valid (non-zero) start the publish timer */
        if (model_context->publish_interval != 0 &&
            model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
            ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   levelPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
        {
            DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                               "Publish Timer Creation Failed > 2 secs\n");
        }
    }
    else  /* Start the transition timer for state update */
    {
        /* Calculate the transition time in milliseconds */
        model_context->trans_data.step_resolution =
            QmeshGetStepResolutionInMilliSec (step_resolution);
        model_context->trans_data.trans_time_ms =
            num_steps * (model_context->trans_data.step_resolution);

        /* Set transition values for'MOVE_SET' messages.As 'MOVE_SET is
         * indefinite, rem_count is set to 0xFF
         */
        if (model_context->trans_data.rem_count == INVALID_INDEX)
        {
            /* Set 'step_resolution' equal to 'trans_time_ms'in milliseconds */
            model_context->trans_data.step_resolution =
                model_context->trans_data.trans_time_ms;
            /* Set 'linear_value' equal to 'delta_level' */
            model_context->trans_data.linear_value = delta_level;
        }
        else /* Set transition values for other level messages */
        {
            if (ABS_VAL (delta_level) >= num_steps)
            {
                model_context->trans_data.rem_count = num_steps;
                model_context->trans_data.linear_value = delta_level / num_steps;
                model_context->trans_data.adjust_step_resolution = FALSE;
            }
            else
            {
                model_context->trans_data.rem_count =  ABS_VAL (delta_level);
                model_context->trans_data.linear_value =
                    (delta_level < 0 ? -1 : 1);
                model_context->trans_data.step_resolution *=
                    (num_steps / ABS_VAL (delta_level));
                model_context->trans_data.adjust_step_resolution = TRUE;
            }
        }

        /* Start the transition timer of time 'step_resolution' for
         * 'rem_count' number of times
         */
        model_context->trans_data.trans_timer = QmeshTimerCreate(&model_timer_ghdl,
                                                       levelLinearTransitionTimerCb, (void *)model_context,
                                                       model_context->trans_data.step_resolution);

        if (model_context->trans_data.trans_timer == QMESH_TIMER_INVALID_HANDLE)
        {
            DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                               "Linear Transition Timer Creation Failed\n");
        }

        /* Send additional status message if transition time is greater than or equal to 2 secs.
         See Model specificaiton section 7.4.1.3*/
        if (model_context->trans_data.trans_time_ms >=
            TRANSITION_TIME_PUBLISH_THRESHOLD)
        {
            /* Send additional status message*/
            if (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS)
                QmeshGenericLevelStatusPublish (model_context);
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      setCurrentLevel
 *
 *  DESCRIPTION
 *      The function sets the current level value
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
static void setCurrentLevel (QMESH_GENERIC_LEVEL_CONTEXT_T *model_context,
                             int16_t new_level)
{
    /* Change 'cur_level' */
    model_context->cur_level = new_level;
    /* Update NVM */
    qmeshLevelUpdateNvm();
    /* Call Application Callback handler. Send the event to the hardware */
    QmeshUpdateBoundStates (model_context->elm_addr, QAPP_LVL_UPDATED,
                            (void *) (&model_context->cur_level));
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      getGenericLevelStatus
 *
 *  DESCRIPTION
 *      This function returns the level status
 *
 *  RETURNS/MODIFIES
 *      Level Status
 *
 *----------------------------------------------------------------------------*/
static void getGenericLevelStatus (QMESH_GENERIC_LEVEL_CONTEXT_T
                                   *model_context)
{
    /* Get the opcode, present level and target level */
    model_context->status[0] = (uint8_t) (QMESH_GENERIC_LEVEL_STATUS >> 8);
    model_context->status[1] = (uint8_t)QMESH_GENERIC_LEVEL_STATUS;
    model_context->status[2] = (uint8_t)model_context->cur_level;
    model_context->status[3] = (uint8_t) (model_context->cur_level >> 8);
    model_context->status[4] = (uint8_t)model_context->target_level;
    model_context->status[5] = (uint8_t) (model_context->target_level >> 8);

    /* Transition NOT in progress */
    if (model_context->cur_level ==  model_context->target_level)
    {
        /* Remaining time should be 0 */
        model_context->status[6] = 0;
    }
    else  /* Transition in progress */
    {
        if (model_context->trans_data.rem_count == INVALID_INDEX)
        {
            model_context->status[6] = QMESH_MODEL_UNKNOWN_TRANSITION_TIME;
        }
        else
        {
            model_context->status[6] =
                QmeshConvertTimeToTransitionTimeFormat (
                    model_context->trans_data.trans_time_ms);
        }
    }

    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                      "Level status response: %d %d %d %d %d %d\n",
                      model_context->status[0], model_context->status[1],
                      model_context->status[2], model_context->status[3],
                      model_context->status[4], model_context->status[5],
                      model_context->status[6]);
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericLevelStatusSend
 *
 *  DESCRIPTION
 *      This function sends the level status message in response to reliable
 *      GET/SET message
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericLevelStatusSend (QMESH_GENERIC_LEVEL_CONTEXT_T
        *model_context,
        QMESH_MODEL_MSG_COMMON_T
        *msg_cmn)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    /* Check for validity of context data */
    if (!model_context || !msg_cmn)
    {
        DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                           "\nNull model context/Null common msg\n");
        return;
    }

    if (msg_cmn->opcode == QMESH_GENERIC_LEVEL_SET ||
        msg_cmn->opcode == QMESH_GENERIC_DELTA_SET ||
        msg_cmn->opcode == QMESH_GENERIC_MOVE_SET ||
        msg_cmn->opcode == QMESH_GENERIC_LEVEL_GET)
    {
        /* Get the level status  */
        getGenericLevelStatus (model_context);
        /* Check if 'target_level' and 'remaining time' are present in the
         * response.
         */
        data_info.payload = model_context->status;

        if (model_context->status[GENERIC_LEVEL_STATUS_TRANS_TIME_OFFSET]
            == 0)
        {
            data_info.payload_len = GENERIC_LEVEL_STATUS_NON_TRANS_MSG_LEN;
        }
        else
        {
            data_info.payload_len = GENERIC_LEVEL_STATUS_TRANS_MSG_LEN;
        }

        /* Fill the 'data_info' */
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
        (void)QmeshSendMessage (&app_key_info, &data_info);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericLevelStatusPublish
 *
 *  DESCRIPTION
 *      This function sends the level status message to assigned publish address
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericLevelStatusPublish (QMESH_GENERIC_LEVEL_CONTEXT_T
        *model_context)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;

    /* Check for validity of context data */
    if (!model_context)
    {
        DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                           "\nNull level model context\n");
        return;
    }

    /* Get the level status  */
    getGenericLevelStatus (model_context);
    /* Check if 'target_level' and 'remaining time' are present in the
     * response.
     */
    data_info.payload = (uint8_t *)model_context->status;

    if (model_context->status[GENERIC_LEVEL_STATUS_TRANS_TIME_OFFSET] == 0)
    {
        data_info.payload_len = GENERIC_LEVEL_STATUS_NON_TRANS_MSG_LEN;
    }
    else
    {
        data_info.payload_len = GENERIC_LEVEL_STATUS_TRANS_MSG_LEN;
    }

    /* Fill the 'data_info' */
    data_info.ttl = model_context->publish_state->publish_ttl;
    data_info.src_addr = model_context->elm_addr;
    data_info.dst_addr = model_context->publish_state->publish_addr;
    data_info.trans_ack_required = FALSE;
    data_info.trans_mic_size = QMESH_MIC_4_BYTES;
    // Check if publish address is valid and send message : TODO
    /* key info */
    app_key_info.key_type = QMESH_KEY_TYPE_APP_KEY;
    app_key_info.key_info.prov_net_idx = 0;
    app_key_info.key_info.key_idx = model_context->publish_state->app_key_idx &
                                    0xFF;

    if (QmeshSendMessage (&app_key_info, &data_info) != QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                          "%s %d QmeshSendMessage failed\n", __FUNCTION__, __LINE__);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericLevelServerAppInit
 *
 *  DESCRIPTION
 *      This function initialises all instances of Generic Level model state
 *      data
 *
 *  RETURNS/MODIFIES
 *      QMESH_RESULT_T
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericLevelServerAppInit (QMESH_GENERIC_LEVEL_CONTEXT_T
        *model_context)
{
    /* Check if model_context is valid */
    if (model_context == NULL)
    {
        DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                           "Null level model context\n");
        return QMESH_RESULT_FAILURE;
    }

    if (QmeshMutexCreate (&model_context->level_mutex) !=
        QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                           "QmeshGenericLevelServerAppInit:Creating mutex Failed\n");
        return QMESH_RESULT_FAILURE;
    }

    model_context->publish_interval = 0;
    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericLevelServerAppDeInit
 *
 *  DESCRIPTION
 *      This function de-initialises Generic Level model state data
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericLevelServerAppDeInit (QMESH_GENERIC_LEVEL_CONTEXT_T
        *model_context)
{
    /* Abort any on going transition */
    QmeshGenericLevelAbortTransition (model_context);
    /* Destroy the mutex for all instances of generic level */
    QmeshMutexDestroy (&model_context->level_mutex);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                      "QmeshGenericLevelServerAppDeInit\n");
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericLevelServerAppHandler
 *
 *  DESCRIPTION
 *      This function handles events for Generic Level server
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenericLevelServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len
)
{
    QMESH_MODEL_MSG_COMMON_T msg_cmn;// = {};
    QMESH_MODEL_MSG_T cache_msg;// = {};
    QMESH_GENERIC_LEVEL_CONTEXT_T *model_context = NULL;
    bool transactional = FALSE;
    uint16_t current_time[3], src_addr;
    uint32_t opcode, seq_no;
    /* Get the current time in ms */
    QmeshGetCurrentTimeInMs (current_time);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER, "%s:\n", __FUNCTION__);
    /* Get level context */
    model_context = (QMESH_GENERIC_LEVEL_CONTEXT_T *) model_data->model_priv_data;

    if ((model_context == NULL) ||
        (!msg || msg_len <= 0))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                          "Context data is NULL or msg is invalid \n");
        return QMESH_RESULT_FAILURE;
    }

    if (model_context->last_msg_seq_no == nw_hdr->seq_num &&
        model_context->last_src_addr == nw_hdr->src_addr)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                          "%s: seq num 0x%08x processed\n", __FUNCTION__, nw_hdr->seq_num);
        return QMESH_RESULT_SUCCESS;
    }

    /* Validate if the opcode has the correct length */
    if ((get_opcode_format (msg[MODEL_OPCODE_OFFSET]) ==
         QMESH_OPCODE_FORMAT_TWO_BYTE))
    {
        opcode = QMESH_PACK_TWO_BYTE_OPCODE (msg[0], msg[1]);
        msg += QMESH_OPCODE_FORMAT_TWO_BYTE;
        msg_len -= QMESH_OPCODE_FORMAT_TWO_BYTE;
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                          "Src Addr: %x Dst Addr: %x Opcode: %x, Msg Len: %d\n",
                          nw_hdr->src_addr, nw_hdr->dst_addr, opcode, msg_len);
    }
    else
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                          "Incorrect Opcode Length\n");
        return QMESH_RESULT_FAILURE;
    }

    QmeshMemSet(&msg_cmn, 0, sizeof(msg_cmn));

    /* Lock the mutex */
    QmeshMutexLock (&model_context->level_mutex);
    /* Store the common message parameters */
    QmeshMemCpy (&model_context->key_info,
                 key_info,
                 sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));

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
        case QMESH_GENERIC_LEVEL_GET:
        {
            if (msg_len != GENERIC_LEVEL_GET_MSG_LEN)
            {
                QmeshMutexUnlock (&model_context->level_mutex);
                return QMESH_RESULT_SUCCESS;
            }

            QmeshGenericLevelStatusSend (model_context, &msg_cmn);
            /* Unlock the mutex */
            QmeshMutexUnlock (&model_context->level_mutex);
            return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_GENERIC_DELTA_SET:
        case QMESH_GENERIC_DELTA_SET_UNRELIABLE:
        {
            if (! (msg_len == GENERIC_LEVEL_DELTA_TRANS_MSG_LEN ||
                   msg_len == GENERIC_LEVEL_DELTA_NON_TRANS_MSG_LEN))
            {
                QmeshMutexUnlock (&model_context->level_mutex);
                return QMESH_RESULT_SUCCESS;
            }

            /* Update transactional flag as TRUE */
            msg_cmn.tid = msg[GENERIC_LEVEL_DELTA_TID_OFFSET];
            transactional = TRUE;
        }
        break;

        case QMESH_GENERIC_LEVEL_SET:
        case QMESH_GENERIC_LEVEL_SET_UNRELIABLE:
        case QMESH_GENERIC_MOVE_SET:
        case QMESH_GENERIC_MOVE_SET_UNRELIABLE:
        {
            if (! (msg_len == GENERIC_LEVEL_TRANS_MSG_LEN ||
                   msg_len == GENERIC_LEVEL_NON_TRANS_MSG_LEN))
            {
                QmeshMutexUnlock (&model_context->level_mutex);
                return QMESH_RESULT_SUCCESS;
            }

            msg_cmn.tid = msg[GENERIC_LEVEL_TID_OFFSET];
        }
        break;

        default:
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                              "Opcode did not match\n");
            /* Retore the previous sequence number and src address - since the message is not processed */
            model_context->last_msg_seq_no = seq_no;
            model_context->last_src_addr = src_addr;
            QmeshMutexUnlock (&model_context->level_mutex);
            return QMESH_RESULT_FAILURE;
        }
    }

    /* Check if the message is in the cache. Returns TRUE if message is
     * a new message and cache is updated with the new message
     */
    if (QmeshModelCacheAddMsg (&msg_cmn, transactional))
    {
        /* New message is valid - hence consume */
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                          "New Message\n");
        /* Store message parameters to be added in cache */
        QmeshMemCpy (&cache_msg.cmn_msg,
                     &msg_cmn,
                     sizeof (QMESH_MODEL_MSG_COMMON_T));
        QmeshMemCpy (&cache_msg.key_info,
                     key_info,
                     sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));
        QmeshMemCpy (&cache_msg.timestamp,
                     current_time,
                     sizeof (current_time));
        /* If Transition and delay fields are available,
         * store the values
         */
        cache_msg.transition_time =
            GENERIC_LEVEL_GET_TRANS_TIME (opcode, msg, msg_len,
                                          elem_data);

        /* If transition time is invalid, return */
        if (NUM_OF_TRANSITION_STEPS (cache_msg.transition_time) ==
            QMESH_MODEL_INVALID_TRANSITION_TIME)
        {
            /* Unlock the mutex */
            QmeshMutexUnlock (&model_context->level_mutex);
            DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                               "Invalid Transition Time\n");
            return QMESH_RESULT_SUCCESS;
        }

        cache_msg.delay =
            GENERIC_LEVEL_GET_DELAY (opcode, msg, msg_len);

        /* If Delay is non-zero, add the message into 'Delay' cache */
        if (cache_msg.delay > 0)
        {
            if (QmeshAddMsgToDelayCache (&cache_msg,
                                         QmeshGenericLevelTransitionHandler,
                                         (void *)model_context,
                                         &msg[GENERIC_LEVEL_VALUE_OFFSET],
                                         GENERIC_LEVEL_GET_LEVEL_SIZE (opcode))
                != QMESH_RESULT_SUCCESS)
            {
                DEBUG_MODEL_ERROR (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                                   "Adding message to delay cache failed\n");
                /* Unlock the mutex */
                QmeshMutexUnlock (&model_context->level_mutex);
                return QMESH_RESULT_SUCCESS;
            }
        }
        else
        {
            /* Delay is 0, so initiate transtion or
             * instantaneous state update
             */
            /* Unlock the mutex */
            QmeshMutexUnlock (&model_context->level_mutex);

             /* Alloc for model_msg */
             cache_msg.msg = (uint8_t *)QmeshMalloc (QMESH_MM_SECTION_MODEL_LAYER, msg_len);
             if(cache_msg.msg!=NULL)
             {
               QmeshMemCpy (cache_msg.msg, msg, msg_len);
               cache_msg.msg_len = msg_len;
               QmeshGenericLevelTransitionHandler (&cache_msg,
                                                   (void *)model_context);
               QmeshFree((void*)cache_msg.msg);
             }
             return QMESH_RESULT_SUCCESS;
        }
    }
    else
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                          "Duplicate Message\n");
        /* Unlock the mutex */
        QmeshMutexUnlock (&model_context->level_mutex);
        return QMESH_RESULT_SUCCESS;
    }

    /* Unlock the mutex */
    QmeshMutexUnlock (&model_context->level_mutex);
    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericLevelTransitionHandler
 *
 *  DESCRIPTION
 *      This function can be called from,
 *      1. Delay cache upon expiry of 'delay' and to start 'Transition' to
 *         target state
 *      2. If 'Delay' is 0, will be called from the 'message' handler function
 *         sof this model.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericLevelTransitionHandler (QMESH_MODEL_MSG_T *model_msg,
        void *context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                      "QmeshGenericLevelTransitionHandler\n");
    int32_t delta_level = 0;
    uint16_t msg_timestamp[3], time_diff[3], opcode;
    QAPP_GET_LVL_RANGE_T range = {MAX_LEVEL, MIN_LEVEL};
    /* Retreive the level context */
    QMESH_GENERIC_LEVEL_CONTEXT_T *model_context =
        (QMESH_GENERIC_LEVEL_CONTEXT_T *)context;
    /* Check if current state is in transition.If in transition,
     * abort the transition
     */
    QmeshGenericLevelAbortTransition (model_context);
    /* Lock the mutex */
    QmeshMutexLock (&model_context->level_mutex);
    /* Retreive opcode , transition time , delay, Key Info and Timestamp */
    opcode = model_msg->cmn_msg.opcode;
    model_context->trans_data.transition_time = model_msg->transition_time;
    model_context->delay = model_msg->delay;
    QmeshMemCpy (&model_context->key_info, &model_msg->key_info,
                 sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));
    QmeshMemCpy (msg_timestamp, model_msg->timestamp, sizeof (msg_timestamp));
    /* Call application callback handler to get level range as per
     * extended model min and max values.
     */
    QmeshUpdateBoundStates (model_context->elm_addr, QAPP_GET_LVL_RANGE,
                            &range);

    switch (opcode)
    {
        case QMESH_GENERIC_LEVEL_SET:
        case QMESH_GENERIC_LEVEL_SET_UNRELIABLE:
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                              "QMESH_GENERIC_LEVEL_SET\n");
            /* Retrieve 'target_level' */
            model_context->target_level =
                QMESH_PACK_TWO_BYTE_LEVEL (model_msg->msg[0],
                                           model_msg->msg[1]);
            /* Calculate 'delta_level' */
            delta_level = (int32_t) model_context->target_level -
                          (int32_t) model_context->cur_level;
            /* Adjust delta level value */
            delta_level = adjustDeltaLevel (model_context->cur_level,
                                            delta_level, range.max_val,
                                            range.min_val);
        }
        break;

        case QMESH_GENERIC_DELTA_SET:
        case QMESH_GENERIC_DELTA_SET_UNRELIABLE:
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                              "QMESH_GENERIC_DELTA_LEVEL_SET\n");
            bool new_transaction = TRUE;

            /* If tid,src and dst are same as received in the last message, get
             * the timestamp difference between the current timestamp and
             * timestamp of the last message
             */
            if ((!g_first_iteration) &&
                (model_context->delta_data.tid == model_msg->cmn_msg.tid) &&
                (model_context->delta_data.src_addr == model_msg->cmn_msg.src) &&
                (model_context->delta_data.dst_addr == model_msg->cmn_msg.dst))
            {
                new_transaction = FALSE;
                QmeshTimeSub (time_diff, model_msg->timestamp,
                              model_context->delta_data.timestamp);
            }

            /* Update timestamp */
            QmeshMemCpy (model_context->delta_data.timestamp, model_msg->timestamp,
                         sizeof (model_msg->timestamp));
            /* Retrieve delta level */
            delta_level = QMESH_PACK_FOUR_BYTE_DELTA_LEVEL (model_msg->msg[0],
                          model_msg->msg[1],
                          model_msg->msg[2],
                          model_msg->msg[3]);

            /* For delta set messages,if tid is new or time difference is
             * greater than equal to 6 secs, a new transaction is there
             */
            if ((new_transaction)/* ||
           (!QmeshTimeCmp(time_diff, (uint16_t*)g_message_interval))*/)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                                  "Non Transactional\n");
                /* Make g_first_iteration as FALSE */
                g_first_iteration = FALSE;
                /* Store TID , Source Address and Destination Address */
                model_context->delta_data.tid = model_msg->cmn_msg.tid;
                model_context->delta_data.src_addr = model_msg->cmn_msg.src;
                model_context->delta_data.dst_addr = model_msg->cmn_msg.dst;
                /* Set 'initial_level' equal to 'cur_level' */
                model_context->delta_data.initial_level =
                    model_context->cur_level;
                /* Check if delta level falls within minimum and maximum value
                 */
                delta_level = adjustDeltaLevel (model_context->cur_level,
                                                delta_level, range.max_val,
                                                range.min_val);
                model_context->target_level = model_context->cur_level +
                                              delta_level;
            }
            else  /* It is a part of transactional message */
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                                  "Transactional\n");
                /* Check if delta level falls within minimum and maximum value
                 */
                adjustDeltaLevel (model_context->delta_data.initial_level,
                                  delta_level, range.max_val,
                                  range.min_val);
                model_context->target_level =
                    model_context->delta_data.initial_level + delta_level;
                /* Calculate new delta level */
                delta_level = (int32_t)model_context->target_level -
                              (int32_t)model_context->cur_level;
            }
        }
        break;

        case QMESH_GENERIC_MOVE_SET:
        case QMESH_GENERIC_MOVE_SET_UNRELIABLE:
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                              "QMESH_GENERIC_MOVE_SET\n");
            int16_t move_set_level;
            /* Retrieve delta level */
            move_set_level = QMESH_PACK_TWO_BYTE_LEVEL (model_msg->msg[0],
                             model_msg->msg[1]);

            /* If transition time is 0, do nothing */
            if ((NUM_OF_TRANSITION_STEPS (
                     model_context->trans_data.transition_time)) &&
                (move_set_level != 0))
            {
                /* Set 'rem_count' as 0xFF as it is indefinite */
                model_context->trans_data.rem_count = INVALID_INDEX;
                /* Set 'delta_level' value */
                delta_level = (int32_t) move_set_level;
                /* Adjust delta level value */
                adjustDeltaLevel (model_context->cur_level, delta_level,
                                  range.max_val, range.min_val);

                /* Set 'target_level' as MIN or MAX value */
                if (delta_level > 0)
                    model_context->target_level = MAX_LEVEL;
                else if (delta_level < 0)
                    model_context->target_level = MIN_LEVEL;
            }
        }
        break;

        default:
            break;
    }

    /* If 'delta_level' is zero, do nothing */
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                      "Delta level: %ld\n", delta_level);
    /* Call the application callback handler for transition started event */
    QmeshUpdateBoundStates (model_context->elm_addr, QAPP_LVL_TRANS_BEGIN,
                            NULL);

    /* Update NVM */
    qmeshLevelUpdateNvm();

    /* Update level */
    if (delta_level != 0)
    {
        updateLevel (model_context, delta_level);
    }

    /* Send the level status */
    QmeshGenericLevelStatusSend (model_context, &model_msg->cmn_msg);
    /* Unlock the mutex */
    QmeshMutexUnlock (&model_context->level_mutex);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericLevelAbortTransition
 *
 *  DESCRIPTION
 *      This function aborts the current running transition and
 *      resets transition related variables.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericLevelAbortTransition (QMESH_GENERIC_LEVEL_CONTEXT_T
        *model_context)
{
    /* Lock the mutex */
    QmeshMutexLock (&model_context->level_mutex);

    if (model_context->cur_level != model_context->target_level)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                          "Transition aborted\n");

        /* Delete the currently running transition timer */
        if (model_context->trans_data.trans_timer != QMESH_TIMER_INVALID_HANDLE)
        {
            /* Delete the linear transition timer */
            QmeshTimerDelete (&model_timer_ghdl,&model_context->trans_data.trans_timer);
            /* Delete the publication timer */
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);
            /* Delete the publication retransmission timer */
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);
            model_context->target_level = model_context->cur_level;
            /* Update NVM */
            qmeshLevelUpdateNvm();
            /* Reset the transition related variables */
            resetStateTransitionContext (model_context);
        }
    }

    /* Unlock the mutex */
    QmeshMutexUnlock (&model_context->level_mutex);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericLevelUpdate
 *
 *  DESCRIPTION
 *      This function updates the generic level value changed due to bound states
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericLevelUpdate (QMESH_GENERIC_LEVEL_CONTEXT_T
                                     *model_context,
                                     int16_t new_level)
{
    /* Lock the mutex */
    QmeshMutexLock (&model_context->level_mutex);
    /* Update 'cur_level' to 'new_level'.Send the level change event to the
     * hardware
     */
    model_context->cur_level = new_level;
    /* Make 'target_level' equal to 'cur_level' */
    model_context->target_level = model_context->cur_level;
    /* Update NVM */
    qmeshLevelUpdateNvm();
    /* Unlock the mutex */
    QmeshMutexUnlock (&model_context->level_mutex);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                      "Cur Level: %d\n", model_context->cur_level);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericLevelSet
 *
 *  DESCRIPTION
 *      This function sets the generic level value.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshGenericLevelSet (QMESH_GENERIC_LEVEL_CONTEXT_T *model_context,
                                  uint8_t transition_time, uint8_t delay, int16_t new_level)
{
    /* Note: transition_time and delay params are for future requirements */
    /* Abort any ongoing transition */
    QmeshGenericLevelAbortTransition(model_context);

    /* Call the bound states handler for transition started event */
    QmeshUpdateBoundStates (model_context->elm_addr, QAPP_LVL_TRANS_BEGIN,
                            NULL);
    /* Lock the mutex */
    QmeshMutexLock (&model_context->level_mutex);
    /* Update 'cur_level' to 'new_level'.Send the level change event to the
     * hardware
     */
    model_context->cur_level = new_level;
    /* Make 'target_level' equal to 'cur_level' */
    model_context->target_level = model_context->cur_level;
    /* Update NVM */
    qmeshLevelUpdateNvm();
    /* Call Application Callback handler. Send the event to the hardware */
    QmeshUpdateBoundStates (model_context->elm_addr, QAPP_LVL_UPDATED,
                            (void *) (&model_context->cur_level));
    /* Unlock the mutex */
    QmeshMutexUnlock (&model_context->level_mutex);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                      "Cur Level: %d\n", model_context->cur_level);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericLevelGet
 *
 *  DESCRIPTION
 *      This function sets the generic level value changed due to bound states
 *
 *  RETURNS/MODIFIES
 *      Current level state
 *
 *----------------------------------------------------------------------------*/
extern int16_t QmeshGenericLevelGet (QMESH_GENERIC_LEVEL_CONTEXT_T *model_context)
{
    return model_context->cur_level;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericLevelUpdatePublicationPeriod
 *
 *  DESCRIPTION
 *      Calculates the publish interval from the Publish state data for the model.
 *
 *  RETURNS/MODIFIES
 *
 *----------------------------------------------------------------------------*/
extern bool QmeshGenericLevelUpdatePublicationPeriod (QMESH_MODEL_DATA_T
        *model_data)
{
    uint8_t no_of_steps, step_resolution;
    QMESH_GENERIC_LEVEL_CONTEXT_T *model_context = NULL;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER, "%s:\n", __FUNCTION__);
    /* Get the model context data from the composition data */
    model_context = (QMESH_GENERIC_LEVEL_CONTEXT_T *) model_data->model_priv_data;
     model_context->publish_state = model_data->publish_state;
    /* 6 bits number of steps */
    no_of_steps = model_data->publish_state->publish_period.steps;
    /* 2 bits step resolution */
    step_resolution = model_data->publish_state->publish_period.resolution;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
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
 *      QmeshGenericLevelPublicationHandler
 *
 *  DESCRIPTION
 *      Publication configuration handler for Generic Level model
 *
 *  RETURNS/MODIFIES
 *      None
 *----------------------------------------------------------------------------*/
extern void QmeshGenericLevelPublicationHandler (QMESH_MODEL_DATA_T *model_data)
{
    QMESH_GENERIC_LEVEL_CONTEXT_T *model_context = NULL;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER, "%s:\n", __FUNCTION__);
    /* Get the model context data from the composition data */
    model_context = (QMESH_GENERIC_LEVEL_CONTEXT_T *) model_data->model_priv_data;

    if (model_context == NULL)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
                          "Context data is NULL \n");
        return;
    }

    model_context->publish_state = model_data->publish_state;

    if (QmeshGenericLevelUpdatePublicationPeriod (model_data))
    {
        if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        {
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
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
                   levelPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_LEVEL_SERVER,
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


