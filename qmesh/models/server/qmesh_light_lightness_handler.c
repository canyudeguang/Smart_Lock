/******************************************************************************
 *  Copyright 2017-2018 Qualcomm Technologies International, Ltd.
 ******************************************************************************/
/*! \file qmesh_light_lightness_handler.c
 *  \brief defines and functions for SIGmesh Light Lightness model.
 */
/******************************************************************************/


#include "qmesh_model_common.h"
#include "qmesh_generic_onoff_handler.h"
#include "qmesh_generic_level_handler.h"
#include "qmesh_generic_poweronoff_setup_handler.h"
#include "qmesh_generic_poweronoff_handler.h"
#include "qmesh_generic_default_transition_time_handler.h"
#include "qmesh_light_lightness_handler.h"
#include "qmesh_model_nvm.h"
/*============================================================================*
 *  Private Data
 *===========================================================================*/

/* Lightness Status report buffer */
uint8_t lightness_status[LIGHTNESS_STATUS_TRANSITION_REPORT_SIZE];

/* Variable to store transition information if any(from DTT & transition provided)
 * by client
 */
uint8_t g_trans_time = 0;

/*============================================================================*
 *  Private Function Implementations
 *===========================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshLightnessUpdateNvm
 *
 *  DESCRIPTION
 *      This function writes the current state to NVM.
 *
 *  RETURNS/MODIFIES
 *      None.
 *
 *----------------------------------------------------------------------------*/
static void qmeshLightnessUpdateNvm(QMESH_LIGHTNESS_MODEL_CONTEXT_T 
                                                     *model_context)
{
    QMESH_MODEL_LIGHTNESS_NVM_STATE_T nvm_state;
    nvm_state.lightness_linear = model_context->lightness_linear;
    nvm_state.lightness_actual = model_context->lightness_actual;
    nvm_state.lightness_last = model_context->lightness_last; 
    nvm_state.lightness_default = model_context->lightness_default;
    nvm_state.lightness_min_range = model_context->lightness_min_range;
    nvm_state.lightness_max_range = model_context->lightness_max_range;
    nvm_state.last_msg_seq_no = model_context->last_msg_seq_no;   
    nvm_state.last_src_addr = model_context->last_src_addr;
    nvm_state.lightness_target = model_context->lightness_target;
    
    /* Update NVM */
    NVMWrite_ModelLightnessState((uint8_t*)&nvm_state);
} 
/*----------------------------------------------------------------------------*
*  NAME
*      qmeshSqrt
*
*  DESCRIPTION
*      This function computes the square root.
*
*  RETURNS/MODIFIES
*      square root of a given value.
*
*----------------------------------------------------------------------------*/
static uint32_t qmeshSqrt (uint32_t value)
{
    if (value == 0 || value == 1)
        return value;

    uint32_t i = 1, result = 1;

    while (result < value)
    {
        if (result == value)
            return result;

        i++;
        result = i * i;
    }

    if (result == value)
        return i;
    else
        return i - 1;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshLightLightnessPublishRetransTimerCb
 *
 *  DESCRIPTION
 *      This function sends the Light Lightness  message to the publish address for
 *  the Lightness model periodically every 'Publish_Period' until new tranistion
 *  begins.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
static void qmeshLightLightnessPublishRetransTimerCb (QMESH_TIMER_HANDLE_T
        timerHandle,
        void *context)
{
    /* Retrieve the context */
    QMESH_LIGHTNESS_MODEL_CONTEXT_T *model_context =
        (QMESH_LIGHTNESS_MODEL_CONTEXT_T *)context;
    model_context->publish_timer = QMESH_TIMER_INVALID_HANDLE;
    /* Send Status Message to Publish Address */
    QmeshLightLightnessStatusPublish (model_context);
    model_context->publish_restrans_cnt--;

    /* Restart the timer */
    if (model_context->publish_restrans_cnt)
    {
        if ((model_context->publish_interval > 0) &&
            (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
            ((model_context->publish_retrans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightLightnessPublishRetransTimerCb,
                   (void *)model_context,
                   (model_context->publish_state->retransmit_param.interval_step + 1) * 50))
                   == QMESH_TIMER_INVALID_HANDLE))
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                              "Publish Retransmission Timer Creation Failed or publish data not set\n");
        }
    }
}

/*----------------------------------------------------------------------------*
*  NAME
*      qmeshLightLightnessPublishTimerCb
*
*  DESCRIPTION
*      This function sends the Lightness status message to the publish address for
*  the Lightness model periodically every 'Publish_Period' until new tranistion
*  begins.
*
*  RETURNS/MODIFIES
*      None
*
*----------------------------------------------------------------------------*/
static void qmeshLightLightnessPublishTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s:\n", __FUNCTION__);
    /* Retrieve the context */
    QMESH_LIGHTNESS_MODEL_CONTEXT_T *model_context =
        (QMESH_LIGHTNESS_MODEL_CONTEXT_T *)context;
    /* Send Status Message to Publish Address. Enable below code with new stack release */
    QmeshLightLightnessStatusPublish (model_context);
    model_context->publish_timer = QMESH_TIMER_INVALID_HANDLE;

    /* Stop any retransmission of publish message */
    if (model_context->publish_retrans_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

    /* Send Message to Publish Address periodically after every
       "Publish_Period" */
    if ((model_context->publish_interval > 0) &&
        (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
        ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightLightnessPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "Periodic Publish Timer Creation Failed\n");
    }

    model_context->publish_restrans_cnt =
        model_context->publish_state->retransmit_param.count;

    /* Retransmit status Message to the Publish Address periodically
        "publish_restrans_cnt" times at every
         retransmission interval = (Publish Retransmit Interval Steps + 1) * 50 millseconds */
    if (model_context->publish_restrans_cnt &&
        ((model_context->publish_retrans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightLightnessPublishRetransTimerCb,
                   (void *)model_context,
                   (model_context->publish_state->retransmit_param.interval_step + 1) * 50))
                   == QMESH_TIMER_INVALID_HANDLE))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "Publish Retransmission Timer Creation Failed\n");
    }
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      lightStateTransitionTimer
 *
 *  DESCRIPTION
 *      This function lineraly increments the value after transition timer
 *      expiry and restarts the transition timer for next linear value change.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
static void lightStateTransitionTimer (QMESH_TIMER_HANDLE_T timerHandle,
                                       void *context)
{
    /* Extract transition state information and lightness context data */
    QMESH_LIGHTNESS_MODEL_CONTEXT_T *model_data = (QMESH_LIGHTNESS_MODEL_CONTEXT_T
            *)context;
    /* Delete the current transition timer */
    QmeshTimerDelete (&model_timer_ghdl,&model_data->light_trans.timer_handle);
    /* Print that a transition is in progress */
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER, "Transition In Progress\n");
    /* Decrement the step count */
    model_data->light_trans.no_of_steps--;

    /* check what is the present state of the transition & update context*/
    switch (model_data->light_trans.transition_state)
    {
        case QMESH_TRANSITION_BEGIN:
        case QMESH_TRANSITION_IN_PROGRESS:
        {
            /* Update NVM */
            qmeshLightnessUpdateNvm(model_data);
            
            /* Stop publishing status messages */
            if (model_data ->publish_timer != QMESH_TIMER_INVALID_HANDLE)
                QmeshTimerDelete (&model_timer_ghdl,&model_data->publish_timer);

            if (model_data ->publish_retrans_timer != QMESH_TIMER_INVALID_HANDLE)
                QmeshTimerDelete (&model_timer_ghdl,&model_data->publish_retrans_timer);

            QmeshMutexLock (&model_data->mutex_handle);

            /* Check if transition is for Lightness acutal/Linear*/
            if (model_data->light_trans.opcode == QMESH_LIGHT_LIGHTNESS_SET ||
                model_data->light_trans.opcode == QMESH_LIGHT_LIGHTNESS_SET_UNRELIABLE)
            {
                model_data->lightness_actual += model_data->light_trans.linear_value;

                /* Validate if lightness acutal falls within the specified range */
                if ( model_data->lightness_max_range &&  model_data->lightness_min_range)
                {
                    /* Update lightness range */
                    if (model_data->lightness_actual > model_data->lightness_max_range)
                        model_data->lightness_actual = model_data->lightness_max_range;
                    else if (model_data->lightness_actual < model_data->lightness_min_range)
                        model_data->lightness_actual = model_data->lightness_min_range;
                }

                /* Update bound state lightness linear */
                uint32_t lin = model_data->lightness_actual;
                model_data->lightness_linear = ((uint32_t) (lin * lin) / LIGHTNESS_MAX);
                /* Update Bound States for other models */
                QmeshUpdateBoundStates (model_data->light_trans.elm_id,
                                        QAPP_LIGHTNESS_ACTUAL_UPDATED,
                                        &model_data->lightness_actual);
            }
            else
            {
                /* This is for lightness linear */
                model_data->lightness_linear += model_data->light_trans.linear_value;
                /* Compute lightness actual as this is bounded to lightness linear */
                model_data->lightness_actual = qmeshSqrt (model_data->lightness_linear *
                                               LIGHTNESS_MAX);
            }

            QmeshMutexUnlock (&model_data->mutex_handle);

            /* Check whether we have reached the final step */
            if (model_data->light_trans.no_of_steps == 1)
            {
                /* Set the transition state to End */
                model_data->light_trans.transition_state = QMESH_TRANSITION_END;
            }
            else
            {
                /* Set the transition state to in progress */
                model_data->light_trans.transition_state = QMESH_TRANSITION_IN_PROGRESS;
            }

            /* Restart the timer */
            model_data->light_trans.timer_handle = QmeshTimerCreate (&model_timer_ghdl,
                              lightStateTransitionTimer,
                              (void *)model_data,
                               model_data->light_trans.progress_time);
        }
        break;

        case QMESH_TRANSITION_END:
        {
            /* Mark the transition state to idles */
            model_data->light_trans.transition_state = QMESH_TRANSITION_IDLE;
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER, "Transition Ended\n");
            QmeshMutexLock (&model_data->mutex_handle);

            /* Update the Lightness Actual State value */
            if (model_data->light_trans.opcode == QMESH_LIGHT_LIGHTNESS_SET ||
                model_data->light_trans.opcode == QMESH_LIGHT_LIGHTNESS_SET_UNRELIABLE)
            {
                /* Directly update the lightness actual */
                model_data->lightness_actual = model_data->light_trans.target_value;

                /* check whether the actual value fall withing range */
                if ( model_data->lightness_max_range &&  model_data->lightness_min_range)
                {
                    /* Update lightness range */
                    if (model_data->light_trans.target_value > model_data->lightness_max_range)
                        model_data->lightness_actual = model_data->lightness_max_range;
                    else if (model_data->light_trans.target_value < model_data->lightness_min_range)
                        model_data->lightness_actual = model_data->lightness_min_range;
                }

                /* Update the lightness last value */
                if (model_data->light_trans.target_value)
                    model_data->lightness_last = model_data->light_trans.target_value;

                /* Update lightness linear */
                uint32_t lin = model_data->lightness_actual;
                model_data->lightness_linear = ((uint32_t) (lin * lin) / LIGHTNESS_MAX);
            }
            else
            {
                model_data->lightness_linear = model_data->light_trans.target_value;
                /* Update Bound States */
                model_data->lightness_actual = qmeshSqrt (model_data->lightness_linear *
                                               LIGHTNESS_MAX);

                /* Update the lightness last value */
                if (model_data->lightness_actual)
                    model_data->lightness_last = model_data->lightness_actual;
            }

            QmeshMutexUnlock (&model_data->mutex_handle);
            /* reset g_trans_time */
            g_trans_time = 0;
            /* Update Bound States for other models*/
            QmeshUpdateBoundStates (model_data->light_trans.elm_id,
                                    QAPP_LIGHTNESS_ACTUAL_UPDATED,
                                    &model_data->lightness_actual);
            /* Update NVM */
            model_data->lightness_target = 0;
            qmeshLightnessUpdateNvm(model_data);                                    

            /* If publish time is valid (non-zero) start the publish timer */
            if (model_data->publish_interval != 0 &&
                model_data->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
                ((model_data->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightLightnessPublishTimerCb,
                   (void *)model_data,
                   model_data->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "Publish Timer Creation Failed \n");
            }
        }
        break;

        case QMESH_TRANSITION_ABORTING:
        {
            /* No more transition. Update g_trans_time */
            g_trans_time = 0;

            /* Set the transition state back to idle */
            model_data->light_trans.transition_state = QMESH_TRANSITION_IDLE;

            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                              "Transition Aborted\n");
        }
        break;

        default:
            break;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshLightnessTransitionHandler
 *
 *  DESCRIPTION
 *      This function can be called from,
 *      1. Delay cache upon expiry of 'delay' and to start 'Transition' to target state
 *      2. If 'Delay' is 0, will be called from the 'message' handler function of this model.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
static void qmeshLightnessTransitionHandler (QMESH_MODEL_MSG_T *model_msg,
        void *context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                      "Lighntess Transition Handler Called \n");
    /* Retrieve the context information */
    QMESH_LIGHTNESS_MODEL_CONTEXT_T *lightness_context =
        (QMESH_LIGHTNESS_MODEL_CONTEXT_T *)context;

    if (lightness_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&lightness_context->publish_timer);

    if (lightness_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&lightness_context->publish_retrans_timer);

    /* Retreive opcode */
    uint16_t opcode = model_msg->cmn_msg.opcode;

    /* Abort any previous transition if ongoing */
    if (lightness_context->light_trans.transition_state ==
        QMESH_TRANSITION_IN_PROGRESS)
    {
        QmeshLightnessAbortTransition (lightness_context);
    }

    /* Update Bound States - Cancel all other transitions if ongoing in other models */
    QmeshUpdateBoundStates (lightness_context->elm_id,
                            QAPP_LIGHTNESS_ACTUAL_TRANS_BEGIN, NULL);
    /* Prepare for the new transition process */
    lightness_context->light_trans.no_of_steps         = NUM_OF_TRANSITION_STEPS (
                g_trans_time) ;
    lightness_context->light_trans.target_value        = ((
                model_msg->msg[OFFSET_VALUE_BYTE_MSB] << 8) \
            | model_msg->msg[OFFSET_VALUE_BYTE_LSB]);
    lightness_context->light_trans.step_resolution     = STEP_RESOLUTION (
                g_trans_time);
    lightness_context->light_trans.opcode              = opcode;
    lightness_context->light_trans.elm_id              = lightness_context->elm_id;
    lightness_context->light_trans.src_addr            = model_msg->cmn_msg.src;
    lightness_context->light_trans.transition_state    = QMESH_TRANSITION_BEGIN;
    lightness_context->lightness_target = lightness_context->light_trans.target_value;
    
    /* Update NVM with the lightness target value */
    qmeshLightnessUpdateNvm(lightness_context);

    /* Calculate the step resolution in milli seconds */
    switch (lightness_context->light_trans.step_resolution)
    {
        case QMESH_MODEL_STEP_RESOLUTION_100_MSEC:
            lightness_context->light_trans.transition_duration = \
                    (uint32_t)lightness_context->light_trans.no_of_steps * QMESH_100_MILLI_SEC;
            break;

        case QMESH_MODEL_STEP_RESOLUTION_1_SEC:
            lightness_context->light_trans.transition_duration = \
                    (uint32_t)lightness_context->light_trans.no_of_steps * QMESH_SEC;
            break;

        case QMESH_MODEL_STEP_RESOLUTION_10_SEC:
            lightness_context->light_trans.transition_duration = \
                    (uint32_t)lightness_context->light_trans.no_of_steps * QMESH_10_SEC;
            break;

        case QMESH_MODEL_STEP_RESOLUTION_10_MINUTES:
            lightness_context->light_trans.transition_duration = \
                    (uint32_t)lightness_context->light_trans.no_of_steps * QMESH_10_MINUTES;
            break;

        default:
            break;
    }

    /* preserve the context data to be used in transition */
    lightness_context->light_trans.state_data  = (void *)lightness_context;

    /* calculate the interpolated value to be used to increment/decrement in every step. Also
     * identify whether we have to increment/decrement.
     */
    if ((opcode ==  QMESH_LIGHT_LIGHTNESS_SET) ||
        (opcode == QMESH_LIGHT_LIGHTNESS_SET_UNRELIABLE))
    {
        /* Get the interpolated value for lightness actual*/
        lightness_context->light_trans.linear_value  = \
                ((lightness_context->light_trans.target_value - lightness_context-> \
                  lightness_actual ) / lightness_context->light_trans.no_of_steps);
    }
    else
    {
        /* Get the interpolated value for lightness  linear*/
        lightness_context->light_trans.linear_value  = \
                ((lightness_context->light_trans.target_value - lightness_context-> \
                  lightness_linear ) / lightness_context->light_trans.no_of_steps);
    }

    /* Find the progress time for each step based on transition duration and no of steps*/
    lightness_context->light_trans.progress_time = \
            lightness_context->light_trans.transition_duration /
            lightness_context->light_trans.no_of_steps;
    /* Preserve the key info to transmit status information during transition */
    QmeshMemCpy (&lightness_context->light_trans.key_info,
                 &model_msg->key_info,
                 sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T)); // store key info

    /* Send back acknowledgement to client that we have accepted the transition */
    switch (opcode)
    {
        case QMESH_LIGHT_LIGHTNESS_SET:
        case QMESH_LIGHT_LIGHTNESS_SET_UNRELIABLE:
        case QMESH_LIGHT_LIGHTNESS_LINEAR_SET:
        case QMESH_LIGHT_LIGHTNESS_LINEAR_SET_UNRELIABLE:
        {
            /* Send ACK only for reliable messages */
            if (opcode != QMESH_LIGHT_LIGHTNESS_SET_UNRELIABLE &&
                opcode != QMESH_LIGHT_LIGHTNESS_LINEAR_SET_UNRELIABLE)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "Transition Beginning update sent\n");
                /* Update and acknowledge the sender, that transition about to begin */
                QmeshSendLightnessStatusUpdate (lightness_context->elm_id,
                                                model_msg->cmn_msg.src,
                                                opcode,
                                                &model_msg->key_info,
                                                lightness_context);
            }

            /* Begin the transition Process finally */
            lightness_context->light_trans.timer_handle = QmeshTimerCreate (&model_timer_ghdl,
                              lightStateTransitionTimer,
                              (void *)lightness_context,
                              lightness_context->light_trans.progress_time);

       }

        default:
            break;
    }
}
/*============================================================================*
 *  Public Function Implementations
 *===========================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshSendLightnessStatusUpdate
 *
 *  DESCRIPTION
 *      Send Present Lightness Message(based on opcode) with/without transition
 *      information.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshSendLightnessStatusUpdate (uint16_t elem_id,
        uint16_t src_addr,
        uint16_t opcode,
        const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
        QMESH_LIGHTNESS_MODEL_CONTEXT_T *lightness_context)
{
    QMESH_ACCESS_PAYLOAD_INFO_T status_data_info;

    if (lightness_context == NULL)
        return;

    /* Fill in the response payload */
    status_data_info.ttl = QMESH_MODEL_DEFAULT_TTL;
    status_data_info.src_addr = elem_id;
    status_data_info.dst_addr = src_addr;
    status_data_info.trans_ack_required = FALSE;
    status_data_info.trans_mic_size = QMESH_MIC_4_BYTES;
    QmeshMutexLock (&lightness_context->mutex_handle);

    switch (opcode)
    {
        case QMESH_LIGHT_LIGHTNESS_SET:
        case QMESH_LIGHT_LIGHTNESS_GET:
        case QMESH_LIGHT_LIGHTNESS_LINEAR_GET:
        case QMESH_LIGHT_LIGHTNESS_LINEAR_SET:
        {
            if (opcode == QMESH_LIGHT_LIGHTNESS_GET ||
                opcode == QMESH_LIGHT_LIGHTNESS_SET  )
            {
                /* Construct the opcode data in the big endian format */
                lightness_status[0] = (uint8_t) ((QMESH_LIGHT_LIGHTNESS_STATUS & 0xFF00) >> 8);
                lightness_status[1] = (uint8_t) (QMESH_LIGHT_LIGHTNESS_STATUS & 0x00FF);
                /* Construct the payload data in the little endian format */
                lightness_status[2] = (uint8_t) (lightness_context->lightness_actual & 0x00FF);
                lightness_status[3] = (uint8_t) ((lightness_context->lightness_actual & 0xFF00) >>
                                               8);
            }
            else
            {
                /* Construct the opcode data in the big endian format */
                lightness_status[0] = (uint8_t) ((QMESH_LIGHT_LIGHTNESS_LINEAR_STATUS & 0xFF00) >>
                                               8);
                lightness_status[1] = (uint8_t) (QMESH_LIGHT_LIGHTNESS_LINEAR_STATUS & 0x00FF);
                /* Construct the payload data in the little endian format */
                lightness_status[2] = (uint8_t) (lightness_context->lightness_linear & 0x00FF);
                lightness_status[3] = (uint8_t) ((lightness_context->lightness_linear & 0xFF00) >>
                                               8);
            }

            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                              "Lighntess Send Non Trans %d\n", \
                              lightness_context->light_trans.transition_state);

            /* Check if there is a transition in Progress.If so, include transition information */
            if (lightness_context->light_trans.transition_state != QMESH_TRANSITION_IDLE &&
                lightness_context->light_trans.transition_state != QMESH_TRANSITION_ABORTING)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "Lighntess Transition Information %d\n", \
                                  lightness_context->light_trans.transition_state);
                lightness_status[4] = (uint8_t) (lightness_context->light_trans.target_value &
                                               0x00FF);
                lightness_status[5] = (uint8_t) ((lightness_context->light_trans.target_value &
                                                0xFF00) >> 8);

                switch (lightness_context->light_trans.step_resolution)
                {
                    case QMESH_MODEL_STEP_RESOLUTION_100_MSEC:
                    {
                        lightness_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_100_MSEC << 6) | \
                                              lightness_context->light_trans.no_of_steps;
                    }
                    break;

                    case QMESH_MODEL_STEP_RESOLUTION_1_SEC:
                    {
                        lightness_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_1_SEC << 6) | \
                                              lightness_context->light_trans.no_of_steps;
                    }
                    break;

                    case QMESH_MODEL_STEP_RESOLUTION_10_SEC:
                    {
                        lightness_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_SEC << 6) | \
                                              lightness_context->light_trans.no_of_steps;
                    }
                    break;

                    case QMESH_MODEL_STEP_RESOLUTION_10_MINUTES:
                    {
                        lightness_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_MINUTES << 6) | \
                                              lightness_context->light_trans.no_of_steps;
                    }
                    break;
                }

                status_data_info.payload_len = LIGHTNESS_STATUS_TRANSITION_REPORT_SIZE;
                status_data_info.payload = lightness_status;
            }
            else
            {
                status_data_info.payload_len = LIGHTNESS_STATUS_REPORT_SIZE;
                status_data_info.payload = lightness_status;
            }
        }
        break;

        case QMESH_LIGHT_LIGHTNESS_LAST_GET:
        {
            /* Construct the opcode data in the big endian format */
            lightness_status[0] = (uint8_t) ((QMESH_LIGHT_LIGHTNESS_LAST_STATUS & 0xFF00) >>
                                           8);
            lightness_status[1] = (uint8_t) (QMESH_LIGHT_LIGHTNESS_LAST_STATUS & 0x00FF);
            /* Construct the payload data in the little endian format */
            lightness_status[2] = (uint8_t) (lightness_context->lightness_last & 0x00FF);
            lightness_status[3] = (uint8_t) ((lightness_context->lightness_last & 0xFF00) >>
                                           8);
            status_data_info.payload = lightness_status;
            status_data_info.payload_len = LIGHTNESS_STATUS_REPORT_SIZE;
        }
        break;

        case QMESH_LIGHT_LIGHTNESS_DEFAULT_GET:
        case QMESH_LIGHT_LIGHTNESS_DEFAULT_SET:
        {
            /* Construct the opcode data in the big endian format */
            lightness_status[0] = (uint8_t) ((QMESH_LIGHT_LIGHTNESS_DEFAULT_STATUS & 0xFF00)
                                           >> 8);
            lightness_status[1] = (uint8_t) (QMESH_LIGHT_LIGHTNESS_DEFAULT_STATUS & 0x00FF);
            /* Construct the payload data in the little endian format */
            lightness_status[2] = (uint8_t) (lightness_context->lightness_default & 0x00FF);
            lightness_status[3] = (uint8_t) ((lightness_context->lightness_default & 0xFF00)
                                           >> 8);
            status_data_info.payload = lightness_status;
            status_data_info.payload_len = LIGHTNESS_STATUS_REPORT_SIZE;
        }
        break;

        case QMESH_LIGHT_LIGHTNESS_RANGE_GET:
        case QMESH_LIGHT_LIGHTNESS_RANGE_SET:
        {
            /* Construct the opcode data in the big endian format */
            lightness_status[0] = (uint8_t) ((QMESH_LIGHT_LIGHTNESS_RANGE_STATUS & 0xFF00) >>
                                           8);
            lightness_status[1] = (uint8_t) (QMESH_LIGHT_LIGHTNESS_RANGE_STATUS & 0x00FF);
            /* Construct the payload data in the little endian format */
            lightness_status[2] = 0; /* Assume always success */
            lightness_status[3] = (uint8_t) (lightness_context->lightness_min_range & 0x00FF);
            lightness_status[4] = (uint8_t) ((lightness_context->lightness_min_range & 0xFF00)
                                           >> 8);
            lightness_status[5] = (uint8_t) (lightness_context->lightness_max_range & 0x00FF);
            lightness_status[6] = (uint8_t) ((lightness_context->lightness_max_range & 0xFF00)
                                           >> 8);
            status_data_info.payload = lightness_status;
            status_data_info.payload_len = LIGHTNESS_STATUS_REPORT_SIZE + 3;
        }
        break;
    }

    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                      "Lighntess Send Message TransInfo %d\n", \
                      lightness_context->light_trans.transition_state);
    QmeshMutexUnlock (&lightness_context->mutex_handle);
    /* Send the status back */
    QMESH_RESULT_T res = QmeshSendMessage (key_info, &status_data_info);

    if (res != QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "QmeshSendMessage - Status Send Back Failed %d\n", res);
    }

    return;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessServerHandler
 *
 *  DESCRIPTION
 *      Light lightness server event handler. Handles all the model relevant
 *      messages.
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightnessServerHandler (const QMESH_NW_HEADER_INFO_T
        *nw_hdr,
        const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
        const QMESH_ELEMENT_CONFIG_T *elem_data,
        QMESH_MODEL_DATA_T *model_data,
        const uint8_t *msg,
        uint16_t msg_len)
{
    QMESH_MODEL_MSG_COMMON_T msg_cmn;
    QMESH_MODEL_MSG_T cache_msg;
    uint16_t current_time[3];
    uint16_t opcode;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                      "QmeshLightnessServerHandler called \n");
    QMESH_LIGHTNESS_MODEL_CONTEXT_T *lightness_context =
        (QMESH_LIGHTNESS_MODEL_CONTEXT_T *) \
        (model_data->model_priv_data);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                      "Lighntess Transition Information hANDLER %d\n", \
                      lightness_context->light_trans.transition_state);

    if ((lightness_context == NULL) || (!msg || msg_len <= 0))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "QmeshLightnessServerHandler Invalid Model Context \n");
        return QMESH_RESULT_FAILURE;
    }

    /* If the message is the last processed then ignore the message */
    if (lightness_context->last_msg_seq_no == nw_hdr->seq_num &&
        lightness_context->last_src_addr == nw_hdr->src_addr)
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
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER, "%s: Opcode:0x%x\n",
                          __FUNCTION__, opcode);
    }
    else
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "QmeshLightnessServerHandler Invalid opcode format/length \n");
        return QMESH_RESULT_FAILURE;
    }

    /* Get the current time in ms */
    QmeshGetCurrentTimeInMs (current_time);

    switch (opcode)
    {
        case QMESH_LIGHT_LIGHTNESS_GET:
        case QMESH_LIGHT_LIGHTNESS_LINEAR_GET:
        case QMESH_LIGHT_LIGHTNESS_LAST_GET:
        case QMESH_LIGHT_LIGHTNESS_DEFAULT_GET:
        case QMESH_LIGHT_LIGHTNESS_RANGE_GET:
        {
            /* Store the sequence number and src address of the new message */
            lightness_context->last_msg_seq_no = nw_hdr->seq_num;
            lightness_context->last_src_addr = nw_hdr->src_addr;

            QmeshSendLightnessStatusUpdate (lightness_context->elm_id, nw_hdr->src_addr, opcode,
                                            key_info, lightness_context);
            return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_LIGHT_LIGHTNESS_SET:
        case QMESH_LIGHT_LIGHTNESS_LINEAR_SET:
        case QMESH_LIGHT_LIGHTNESS_SET_UNRELIABLE:
        case QMESH_LIGHT_LIGHTNESS_LINEAR_SET_UNRELIABLE:
        {
             /* Store the sequence number and src address of the new message */
             lightness_context->last_msg_seq_no = nw_hdr->seq_num;
             lightness_context->last_src_addr = nw_hdr->src_addr;

            msg += QMESH_OPCODE_FORMAT_TWO_BYTE;
            msg_len -= QMESH_OPCODE_FORMAT_TWO_BYTE;

            if (! (msg_len == LIGHTNESS_SET_NON_TRANS_MSG_LEN ||
                   msg_len == LIGHTNESS_SET_TRANS_MSG_LEN))
                return QMESH_RESULT_SUCCESS;

            QmeshMemSet(&msg_cmn, 0, sizeof(msg_cmn));

             /* Store the message for adding in cache */
            msg_cmn.opcode = opcode;
            msg_cmn.src = nw_hdr->src_addr;
            msg_cmn.dst = nw_hdr->dst_addr;
            msg_cmn.tid = msg[OFFSET_TID];

            /* Check if the message is in the cache. Returns TRUE if message
             *is a new message and cache is updated with the new message
             */
            if (QmeshModelCacheAddMsg (&msg_cmn, FALSE )){

            /* Get default transition time */
            g_trans_time = QmeshGetGenericDTT(elem_data);

            /* Check if this message has a delay/transition time field */
            if ( (msg_len !=  LIGHTNESS_SET_TRANS_MSG_LEN) && (g_trans_time == 0))
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "Lightness Got a non transition message %d\n", msg_len);

                /* Abort any previous transition if ongoing */
                if (lightness_context->light_trans.transition_state ==
                    QMESH_TRANSITION_IN_PROGRESS)
                {
                    QmeshLightnessAbortTransition (lightness_context);
                }

                QmeshMutexLock (&lightness_context->mutex_handle);

                if (opcode == QMESH_LIGHT_LIGHTNESS_SET ||
                    opcode == QMESH_LIGHT_LIGHTNESS_SET_UNRELIABLE)
                {
                    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                      "Lightness Set Received \n");
                    /* Instantaneous Update */
                    lightness_context->lightness_actual = ((msg[OFFSET_VALUE_BYTE_MSB] << 8) |
                                                           msg[OFFSET_VALUE_BYTE_LSB]);
                                                           

                    if ( lightness_context->lightness_min_range &&
                         lightness_context->lightness_max_range)
                    {
                        if (lightness_context->lightness_actual <
                            lightness_context->lightness_min_range)
                        {
                            lightness_context->lightness_actual = lightness_context->lightness_min_range;
                        }

                        if (lightness_context->lightness_actual >
                            lightness_context->lightness_max_range)
                        {
                            lightness_context->lightness_actual = lightness_context->lightness_max_range;
                        }
                    }

                    if (lightness_context->lightness_actual)
                    {
                        lightness_context->lightness_last = lightness_context->lightness_actual;
                    }

                    /* Update lightness linear */
                    uint32_t val = lightness_context->lightness_actual;
                    lightness_context->lightness_linear = ((uint32_t) (val * val) / LIGHTNESS_MAX);
                }
                else
                {
                    /* Instantaneous Update */
                    lightness_context->lightness_linear = ((msg[OFFSET_VALUE_BYTE_MSB] << 8) |
                                                           msg[OFFSET_VALUE_BYTE_LSB]);
                    /* Update actual and last state as well here */
                    lightness_context->lightness_actual = qmeshSqrt (
                            lightness_context->lightness_linear * LIGHTNESS_MAX);

                    if (lightness_context->lightness_actual)
                        lightness_context->lightness_last = lightness_context->lightness_actual;
                }

                QmeshMutexUnlock (&lightness_context->mutex_handle);

                if (opcode == QMESH_LIGHT_LIGHTNESS_SET ||
                    opcode == QMESH_LIGHT_LIGHTNESS_LINEAR_SET)
                {
                    QmeshSendLightnessStatusUpdate (lightness_context->elm_id, 
                                                    nw_hdr->src_addr, 
                                                    opcode,
                                                    key_info, 
                                                    lightness_context);
                }

                /* Update Bound States */
                QmeshUpdateBoundStates (lightness_context->elm_id,
                                        QAPP_LIGHTNESS_ACTUAL_UPDATED, &lightness_context->lightness_actual);
 
                /* Update NVM */
                qmeshLightnessUpdateNvm(lightness_context); 

                if (lightness_context ->publish_timer != QMESH_TIMER_INVALID_HANDLE)
                    QmeshTimerDelete (&model_timer_ghdl,&lightness_context->publish_timer);

                /* If publish time is valid (non-zero) start the publish timer */
                if (lightness_context->publish_interval != 0 &&
                    lightness_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
                    ((lightness_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightLightnessPublishTimerCb,
                   (void *)lightness_context,
                   lightness_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
               {
                    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                      "Lightness Publish Timer Creation Failed \n");
                }
            }
            else
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "Lightness Got a transition message %d\n", msg_len);

                /* Copy 'msg_cmn', 'key_info' and level specific message */
                QmeshMemCpy (&cache_msg.cmn_msg, &msg_cmn, sizeof (QMESH_MODEL_MSG_COMMON_T));
                QmeshMemCpy (&cache_msg.key_info, key_info,
                             sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));

                /* Copy  'current_time' */
                QmeshMemCpy (&cache_msg.timestamp, current_time, sizeof (current_time));

                /* If message includes 'delay' and 'transition_time', store thses value */
                if (msg_len ==  LIGHTNESS_SET_TRANS_MSG_LEN)
                {
                    cache_msg.delay = msg[OFFSET_DELAY];
                    cache_msg.transition_time = msg[OFFSET_TIME];
                    g_trans_time = msg[OFFSET_TIME];
                }
                else
                {
                    cache_msg.delay = 0;
                    cache_msg.transition_time = g_trans_time;
                }

                /* if Delay is non-zero,add the message into 'Delay' cache */
                if (cache_msg.delay > 0)
                {
                    QmeshAddMsgToDelayCache (&cache_msg,
                                             qmeshLightnessTransitionHandler,
                                             (void *)lightness_context,
                                             msg,
                                             msg_len);
                }
                else
                {
                     /* Alloc for model_msg */
                    cache_msg.msg = (uint8_t*)QmeshMalloc (QMESH_MM_SECTION_MODEL_LAYER, msg_len);

                    if(cache_msg.msg!=NULL)
                    {
                        QmeshMemCpy (cache_msg.msg, msg, msg_len);
                        cache_msg.msg_len = msg_len;

                        /* Delay is 0, so initiate transtion or instantaneous state update */
                        qmeshLightnessTransitionHandler (&cache_msg,
                                                         (void *)lightness_context);
                        QmeshFree((void*)cache_msg.msg);
                    }
                }
            }
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
                                  "Model data for Level model is NULL\n");
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
 *      QmeshLightnessActualUpdate
 *
 *  DESCRIPTION
 *      Update Lightness Actual value
 *
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshLightnessActualUpdate (QMESH_LIGHTNESS_MODEL_CONTEXT_T
                                        *lightness_context,
                                        uint16_t light_actual)
{
    /* Update lightness linear */
    uint32_t val = light_actual;
    lightness_context->lightness_actual = light_actual;

    if (light_actual)
        lightness_context->lightness_last = light_actual;

    lightness_context->lightness_linear = ((uint32_t) (val * val)) / LIGHTNESS_MAX;
    
    /* Update NVM */
    qmeshLightnessUpdateNvm(lightness_context);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessActualGet
 *
 *  DESCRIPTION
 *      Returns Lightness Actual value
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshLightnessActualGet (QMESH_LIGHTNESS_MODEL_CONTEXT_T *context)
{
    return context->lightness_actual;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessActualSet
 *
 *  DESCRIPTION
 *      Sets Lightness Actual value.Aborts any transition and updates bound states.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshLightnessActualSet (QMESH_LIGHTNESS_MODEL_CONTEXT_T *context,
                                     uint8_t transition_time,
                                     uint8_t delay,
                                     uint16_t light_actual)
{
    if (context)
    {
        /* Abort any transition */
        QmeshLightnessAbortTransition (context);
        /* Update lightness linear */
        uint32_t val = light_actual;
        context->lightness_actual = light_actual;
        
        if (light_actual)
            context->lightness_last = light_actual;

        context->lightness_linear = ((uint32_t) (val * val)) / LIGHTNESS_MAX;
        /* Update Bound States */
        QmeshUpdateBoundStates (context->elm_id, QAPP_LIGHTNESS_ACTUAL_UPDATED,
                                &context->lightness_actual);
        /* Update NVM */
        qmeshLightnessUpdateNvm(context);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessAbortTransition
 *
 *  DESCRIPTION
 *      Abort ongoing lightness transition if any
 *
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshLightnessAbortTransition (QMESH_LIGHTNESS_MODEL_CONTEXT_T
        *context)
{
    if (context == NULL)
        return;

    if (context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&context->publish_timer);

    /* Abort any previous transition if ongoing */
    if (context->light_trans.transition_state == QMESH_TRANSITION_IN_PROGRESS ||
        context->light_trans.transition_state == QMESH_TRANSITION_BEGIN)
    {
        /* Abort transition.The ongoing transition will abort and clean itself*/
        context->light_trans.transition_state = QMESH_TRANSITION_ABORTING;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessDefaultGet
 *
 *  DESCRIPTION
 *      Return Light Lightness Default value
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshLightnessDefaultGet (QMESH_LIGHTNESS_MODEL_CONTEXT_T
                                        *context)
{
    return context->lightness_default;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessDefaultSet
 *
 *  DESCRIPTION
 *      Sets Light Lightness Default value.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshLightnessDefaultSet (QMESH_LIGHTNESS_MODEL_CONTEXT_T *context,
                                      uint8_t transition_time,
                                      uint8_t delay,
                                      uint16_t light_default)
{
    if (context)
    {
        context->lightness_default = light_default;
        /* Update NVM */
        qmeshLightnessUpdateNvm(context); 
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessDefaultUpdate
 *
 *  DESCRIPTION
 *      Updates Light Lightness Default value.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshLightnessDefaultUpdate (QMESH_LIGHTNESS_MODEL_CONTEXT_T
        *context,
        uint16_t light_default)
{
    if (context)
    {
        context->lightness_default = light_default;
        /* Update NVM */
        qmeshLightnessUpdateNvm(context); 
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessLastGet
 *
 *  DESCRIPTION
 *      returns Light Lightness last value.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshLightnessLastGet (QMESH_LIGHTNESS_MODEL_CONTEXT_T *context)
{
    return context->lightness_last;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessLastSet
 *
 *  DESCRIPTION
 *      Sets Light Lightness last value.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshLightnessLastSet (QMESH_LIGHTNESS_MODEL_CONTEXT_T *model_data,
                                   uint8_t transition_time,
                                   uint8_t delay,
                                   uint16_t light_last)
{
    if (model_data)
    {
        model_data->lightness_last = light_last;
        /* Update NVM */
        qmeshLightnessUpdateNvm(model_data); 
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessLastUpdate
 *
 *  DESCRIPTION
 *      Updates Light Lightness last value.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshLightnessLastUpdate (QMESH_LIGHTNESS_MODEL_CONTEXT_T
                                      *model_data,
                                      uint16_t light_last)
{
    if (model_data)
    {
        model_data->lightness_last = light_last;
        /* Update NVM */
        qmeshLightnessUpdateNvm(model_data); 
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessMinRangeGet
 *
 *  DESCRIPTION
 *      Returns Light Lightness Min Range value.
 *
 *  RETURNS/MODIFIES
 *      Min Range Value
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshLightnessMinRangeGet(QMESH_LIGHTNESS_MODEL_CONTEXT_T* model_data)
{
	return model_data->lightness_min_range;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessMaxRangeGet
 *
 *  DESCRIPTION
 *      Returns Light Lightness Max Range value.
 *
 *  RETURNS/MODIFIES
 *      Max Range Value
 *
 *----------------------------------------------------------------------------*/
extern uint16_t QmeshLightnessMaxRangeGet(QMESH_LIGHTNESS_MODEL_CONTEXT_T* model_data)
{
	return model_data->lightness_max_range;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessServerAppInit
 *
 *  DESCRIPTION
 *      Application calls this API to initialize lightness context
 *
 *  RETURNS/MODIFIES
 *      returns QMESH_RESULT_SUCCESS
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightnessServerAppInit (QMESH_LIGHTNESS_MODEL_CONTEXT_T
        *context)
{
    if (context == NULL)
        return QMESH_RESULT_FAILURE;

    /* During initialization, set the transition state to Idle */
    context->light_trans.transition_state = QMESH_TRANSITION_IDLE;

    if (QmeshMutexCreate (&context->mutex_handle) != QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "%s:Creating mutex Failed\n", __FUNCTION__);
        return QMESH_RESULT_FAILURE;
    }

    /* Initialize transition time to zero */
    g_trans_time = 0;
    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightnessServerAppDeInit
 *
 *  DESCRIPTION
 *      This function De-initializes Light Lightness context
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshLightnessServerAppDeInit (QMESH_LIGHTNESS_MODEL_CONTEXT_T
        *context)
{
    QmeshLightnessAbortTransition (context);
    QmeshMutexDestroy (&context->mutex_handle);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightLightnessStatusPublish
 *
 *  DESCRIPTION
 *      This function sends the current Lightness Status message to the
 *  Publish address assigned for this model.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshLightLightnessStatusPublish (QMESH_LIGHTNESS_MODEL_CONTEXT_T
        *context)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;
    /* Lightness Status report buffer */
    uint8_t light_status[LIGHTNESS_STATUS_TRANSITION_REPORT_SIZE];
    /* Construct the opcode data in the big endian format */
    light_status[0] = (uint8_t) ((QMESH_LIGHT_LIGHTNESS_STATUS & 0xFF00) >> 8);
    light_status[1] = (uint8_t) (QMESH_LIGHT_LIGHTNESS_STATUS & 0x00FF);
    /* Construct the payload data in the little endian format */
    light_status[2] = (uint8_t) (context->lightness_actual & 0x00FF);
    light_status[3] = (uint8_t) ((context->lightness_actual & 0xFF00) >> 8);

    /* Check if there is a transition in Progress.If so, include transition information */
    if (context->light_trans.transition_state != QMESH_TRANSITION_IDLE &&
        context->light_trans.transition_state != QMESH_TRANSITION_ABORTING)
    {
        light_status[4] = (uint8_t) (context->light_trans.target_value & 0x00FF);
        light_status[5] = (uint8_t) ((context->light_trans.target_value & 0xFF00) >> 8);

        switch (context->light_trans.step_resolution)
        {
            case QMESH_MODEL_STEP_RESOLUTION_100_MSEC:
            {
                light_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_100_MSEC << 6) | \
                                  context->light_trans.no_of_steps;
            }
            break;

            case QMESH_MODEL_STEP_RESOLUTION_1_SEC:
            {
                light_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_1_SEC << 6) | \
                                  context->light_trans.no_of_steps;
            }
            break;

            case QMESH_MODEL_STEP_RESOLUTION_10_SEC:
            {
                light_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_SEC << 6) | \
                                  context->light_trans.no_of_steps;
            }
            break;

            case QMESH_MODEL_STEP_RESOLUTION_10_MINUTES:
            {
                light_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_MINUTES << 6) | \
                                  context->light_trans.no_of_steps;
            }
            break;
            data_info.payload_len = LIGHTNESS_STATUS_TRANSITION_REPORT_SIZE;
            data_info.payload = light_status;
        }
    }
    else
    {
        data_info.payload_len = LIGHTNESS_STATUS_REPORT_SIZE;
        data_info.payload = light_status;
    }

    data_info.payload = light_status;
    data_info.ttl = context->publish_state->publish_ttl;
    data_info.src_addr = context->elm_id;
    data_info.dst_addr = context->publish_state->publish_addr;
    data_info.trans_ack_required = FALSE;
    data_info.trans_mic_size = QMESH_MIC_4_BYTES;
    /* key info */
    app_key_info.key_type = QMESH_KEY_TYPE_APP_KEY;
    app_key_info.key_info.prov_net_idx = 0;
    app_key_info.key_info.key_idx = context->publish_state->app_key_idx & 0xFF;
    QMESH_RESULT_T res = QmeshSendMessage (&app_key_info, &data_info);

    if (res != QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "%s %d QmeshSendMessage failed %d\n", __FUNCTION__, __LINE__, res);
    }
    else
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "%s %d Light Lightness Status Published\n", __FUNCTION__, __LINE__);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightLightnessUpdatePublicationPeriod
 *
 *  DESCRIPTION
 *      Calculates the publish interval from the Publish state data for the model.
 *
 *  RETURNS/MODIFIES
 *
 *----------------------------------------------------------------------------*/
extern bool QmeshLightLightnessUpdatePublicationPeriod (
    QMESH_MODEL_DATA_T *model_data)
{
    uint8_t no_of_steps, step_resolution;
    QMESH_LIGHTNESS_MODEL_CONTEXT_T *model_context = NULL;
    /* Get the model context data for the element ID 'elm_addr' from the composition data */
    model_context = (QMESH_LIGHTNESS_MODEL_CONTEXT_T *) model_data->model_priv_data;
    model_context->publish_state = model_data->publish_state;
    /* 6 bits number of steps */
    no_of_steps = model_data->publish_state->publish_period.steps;
    /* 2 bits step resolution */
    step_resolution = model_data->publish_state->publish_period.resolution;

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
 *      QmeshLightLightnessPublicationHandler
 *
 *  DESCRIPTION
 *      Publication configuration handler for Light Lightness Model
 *
 *  RETURNS/MODIFIES
 *      None
 *----------------------------------------------------------------------------*/
extern void QmeshLightLightnessPublicationHandler (QMESH_MODEL_DATA_T
        *model_data)
{
    QMESH_LIGHTNESS_MODEL_CONTEXT_T *model_context = NULL;
    /* Get the model context from the composition data */
    model_context = (QMESH_LIGHTNESS_MODEL_CONTEXT_T *) model_data->model_priv_data;

    if (QmeshLightLightnessUpdatePublicationPeriod (model_data))
    {
        if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        {
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

            /* Start publish timer based on publish interval */
            if ((model_context->publish_interval > 0) &&
                (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
                ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightLightnessPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
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
