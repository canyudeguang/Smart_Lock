/******************************************************************************
 *  Copyright 2017 Qualcomm Technologies International, Ltd.
 ******************************************************************************/
/*! \file qmesh_light_hsl_handler.c
 *  \brief defines and functions for SIGMesh Light HSL model.
 */
/******************************************************************************/
#include "qmesh_model_common.h"
#include "qmesh_light_hsl_handler.h"
#include "qmesh_generic_onoff_handler.h"
#include "qmesh_generic_level_handler.h"
#include "qmesh_generic_poweronoff_handler.h"
#include "qmesh_light_lightness_handler.h"
#include "qmesh_model_nvm.h"
/*============================================================================*
 *  Private Data
 *===========================================================================*/

/* Lightness Status report buffer */
uint8_t light_hsl_status[LIGHT_HSL_RANGE_STATUS_SIZE];

/* Variable to store transition information if any(from DTT & transition provided)
 * by client
 */
uint8_t g_hsltrans_time = 0;

/*============================================================================*
 *  Private Function Implementations
 *===========================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshLightHslUpdateNvm
 *
 *  DESCRIPTION
 *      This function writes the current state to NVM.
 *
 *  RETURNS/MODIFIES
 *      None.
 *
 *----------------------------------------------------------------------------*/
static void qmeshLightHslUpdateNvm(QMESH_LIGHT_HSL_MODEL_CONTEXT_T 
                                                     *model_context)
{
    QMESH_MODEL_LIGHT_HSL_NVM_STATE_T nvm_state;
    QMESH_MODEL_LIGHT_SAT_NVM_STATE_T sat_nvm_state;
    QMESH_MODEL_LIGHT_HUE_NVM_STATE_T hue_nvm_state;

    nvm_state.light_hsl_lightness = model_context->light_hsl_lightness;
    nvm_state.light_hsl_hue_def = model_context->light_hsl_hue_def;
    nvm_state.light_hsl_hue_min = model_context->light_hsl_hue_min;
    nvm_state.light_hsl_hue_max = model_context->light_hsl_hue_max;
    nvm_state.light_hsl_sat_def = model_context->light_hsl_sat_def;
    nvm_state.light_hsl_sat_min = model_context->light_hsl_sat_min;
    nvm_state.light_hsl_sat_max = model_context->light_hsl_sat_max;
    nvm_state.last_msg_seq_no = model_context->last_msg_seq_no;
    nvm_state.last_src_addr = model_context->last_src_addr;  
    nvm_state.light_target_hsl = model_context->light_target_hsl;
    nvm_state.light_target_hue = model_context->light_target_hue;
    nvm_state.light_target_sat = model_context->light_target_sat;
    
    /* Update NVM */
    NVMWrite_ModelLightHslState((uint8_t*)&nvm_state);

    hue_nvm_state.light_hsl_hue = model_context->hue_context->light_hsl_hue; 
    hue_nvm_state.last_msg_seq_no = model_context->hue_context->last_msg_seq_no;
    hue_nvm_state.last_src_addr = model_context->hue_context->last_src_addr; 
    hue_nvm_state.light_target_hue = model_context->light_target_hue;    

    /* Update NVM */
    NVMWrite_ModelLightHueState((uint8_t*)&hue_nvm_state);

    sat_nvm_state.light_hsl_sat = model_context->sat_context->light_hsl_sat; 
    sat_nvm_state.last_msg_seq_no = model_context->sat_context->last_msg_seq_no;
    sat_nvm_state.last_src_addr = model_context->sat_context->last_src_addr;  
    sat_nvm_state.light_target_sat = model_context->light_target_sat;    

    /* Update NVM */
    NVMWrite_ModelLightSatState((uint8_t*)&sat_nvm_state);
} 
/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshLightHslPublishRetransTimerCb
 *
 *  DESCRIPTION
 *      This function sends the Light HSL message to the publish address for
 *  the HSL model periodically every 'Publish_Period' until new tranistion
 *  begins.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
static void qmeshLightHslPublishRetransTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    /* Retrieve the context */
    QMESH_LIGHT_HSL_MODEL_CONTEXT_T *model_context =
        (QMESH_LIGHT_HSL_MODEL_CONTEXT_T *)context;
    model_context ->publish_timer = QMESH_TIMER_INVALID_HANDLE;
    /* Send Status Message to Publish Address */
    QmeshLightHslStatusPublish (model_context);
    model_context->publish_restrans_cnt--;

    /* Restart the timer */
    if (model_context->publish_restrans_cnt)
    {
        if ((model_context->publish_interval > 0) &&
            (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
            ((model_context->publish_retrans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightHslPublishRetransTimerCb,
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
*      qmeshLightHslPublishTimerCb
*
*  DESCRIPTION
*      This function sends the HSL status message to the publish address for
*  the HSl model periodically every 'Publish_Period' until new tranistion
*  begins.
*
*  RETURNS/MODIFIES
*      None
*
*----------------------------------------------------------------------------*/
static void qmeshLightHslPublishTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s:\n", __FUNCTION__);
    /* Retrieve the context */
    QMESH_LIGHT_HSL_MODEL_CONTEXT_T *model_context =
        (QMESH_LIGHT_HSL_MODEL_CONTEXT_T *)context;
    /* Send Status Message to Publish Address. Enable below code with new stack release */
    QmeshLightHslStatusPublish (model_context);
    model_context ->publish_timer = QMESH_TIMER_INVALID_HANDLE;

    /* Stop any retransmission of publish message */
    if (model_context->publish_retrans_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

    /* Send Message to Publish Address periodically after every
       "Publish_Period" */
    if ((model_context->publish_interval > 0) &&
        (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
        ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightHslPublishTimerCb,
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
                   qmeshLightHslPublishRetransTimerCb,
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
 *      lightHslServerTransitionTimer
 *
 *  DESCRIPTION
 *      This function lineraly increments the value after transition timer
 *      expiry and restarts the transition timer for next linear value change.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
static void lightHslServerTransitionTimer (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    /* Extract transition state information and lightness context data */
    QMESH_LIGHT_HSL_MODEL_CONTEXT_T *model_context = \
            (QMESH_LIGHT_HSL_MODEL_CONTEXT_T *)context;
    QMESH_HSL_SERVER_TRANSITION_INFO *p_info = \
            (QMESH_HSL_SERVER_TRANSITION_INFO *) (&model_context->hsl_transition);
    /* Delete the current transition timer */
    QmeshTimerDelete(&model_timer_ghdl,&p_info->timer_handle);
    /* Print that a transition is in progress */
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER, "Transition In Progress\n");
    /* Decrement the step count */
    p_info->no_of_steps--;

    /* check what is the present state of the transition & update context*/
    switch (p_info->transition_state)
    {
        case QMESH_TRANSITION_BEGIN:
        case QMESH_TRANSITION_IN_PROGRESS:
        {
            /* Update NVM with transition information */
            qmeshLightHslUpdateNvm(model_context);
             
            /* Stop publishing status messages */
            if (model_context ->publish_timer != QMESH_TIMER_INVALID_HANDLE)
                QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

            if (model_context ->publish_retrans_timer != QMESH_TIMER_INVALID_HANDLE)
                QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

            /* Initiate transition to the target state and send 'status' */
            QmeshMutexLock (&model_context->mutex_handle);
            model_context->light_hsl_lightness = (int16_t) (
                    model_context->light_hsl_lightness) + \
                                                 p_info->linear_value[0];
            model_context->hue_context->light_hsl_hue = (int16_t) (
                        model_context->hue_context->light_hsl_hue) + \
                    p_info->linear_value[1];
            model_context->sat_context->light_hsl_sat = (int16_t) (
                        model_context->sat_context->light_hsl_sat) + \
                    p_info->linear_value[2];
            QmeshMutexUnlock (&model_context->mutex_handle);

            /* Check whether we have reached the final step */
            if (p_info->no_of_steps == 1)
            {
                /* Set the transition state to End */
                p_info->transition_state = QMESH_TRANSITION_END;
            }
            else
            {
                /* Set the transition state to in progress */
                p_info->transition_state = QMESH_TRANSITION_IN_PROGRESS;
            }

            /* Restart the timer */
            p_info->timer_handle = QmeshTimerCreate (&model_timer_ghdl,
                                                     lightHslServerTransitionTimer,
                                                     (void *)model_context,
                                                     p_info->progress_time);

            if(p_info->timer_handle == QMESH_TIMER_INVALID_HANDLE)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER, "Transition Timer Failed\n");
            }

            /* Update Bound States for other models*/
            QmeshUpdateBoundStates (model_context->elm_id,
                                    QAPP_LIGHT_HSL_UPDATED,
                                    &model_context->light_hsl_lightness);
        }
        break;

        case QMESH_TRANSITION_END:
        {
            /* Mark the transition state to idles */
            p_info->transition_state = QMESH_TRANSITION_IDLE;
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER, "Transition Ended\n");
            QmeshMutexLock (&model_context->mutex_handle);
            model_context->light_hsl_lightness = p_info->target_value[0];
            QmeshLightHslHueUpdate (model_context->hue_context, p_info->target_value[1]);
            QmeshLightHslSatUpdate (model_context->sat_context, p_info->target_value[2]);
            QmeshMutexUnlock (&model_context->mutex_handle);
            /* reset g_hsltrans_time */
            g_hsltrans_time = 0;
            /* Update Bound States for other models*/
            QmeshUpdateBoundStates (model_context->elm_id,
                                    QAPP_LIGHT_HSL_UPDATED,
                                    &model_context->light_hsl_lightness);
            /* Reset all the target values to zero as transition is complete */
            model_context->light_target_hsl = 0;            
            model_context->light_target_hue = 0;            
            model_context->light_target_sat = 0;            
            qmeshLightHslUpdateNvm(model_context);                                    

            /* If publish time is valid (non-zero) start the publish timer */
            if (model_context->publish_interval != 0 &&
                model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
                ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightHslPublishTimerCb,
                   (void *)model_context,
                   model_context->publish_interval))
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
            g_hsltrans_time = 0;

            /* Set the transition state back to idle */
            model_context->hsl_transition.transition_state = QMESH_TRANSITION_IDLE;

            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                              "HSL Transition Aborted\n");
        }
        break;

        default:
            break;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshSendLightHSLServerStatusUpdate
 *
 *  DESCRIPTION
 *      Send Present Light HSL Message(based on opcode) with/without transition
 *      information.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
static void qmeshSendLightHSLServerStatusUpdate (uint16_t elem_id,
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
    QmeshMutexLock (&context->mutex_handle);

    switch (opcode)
    {
        case QMESH_LIGHT_HSL_GET:
        case QMESH_LIGHT_HSL_SET:
        {
            /* Construct the opcode data in the big endian format */
            light_hsl_status[0] = (uint8_t) ((QMESH_LIGHT_HSL_STATUS & 0xFF00) >> 8);
            light_hsl_status[1] = (uint8_t) (QMESH_LIGHT_HSL_STATUS & 0x00FF);
            /* Construct the payload data in the little endian format */
            light_hsl_status[2] = (uint8_t) (context->light_hsl_lightness & 0x00FF);
            light_hsl_status[3] = (uint8_t) ((context->light_hsl_lightness & 0xFF00) >> 8);
            light_hsl_status[4] = (uint8_t) (context->hue_context->light_hsl_hue & 0x00FF);
            light_hsl_status[5] = (uint8_t) ((context->hue_context->light_hsl_hue & 0xFF00) >>
                                           8);
            light_hsl_status[6] = (uint8_t) (context->sat_context->light_hsl_sat & 0x00FF);
            light_hsl_status[7] = (uint8_t) ((context->sat_context->light_hsl_sat & 0xFF00) >>
                                           8);

            /* Check if there is a transition in Progress.If so, include transition information */
            if (context->hsl_transition.transition_state != QMESH_TRANSITION_IDLE &&
                context->hsl_transition.transition_state != QMESH_TRANSITION_ABORTING)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "qmeshSendLightHSLServerStatusUpdate transition filling \n");

                switch (context->hsl_transition.step_resolution)
                {
                    case QMESH_MODEL_STEP_RESOLUTION_100_MSEC:
                    {
                        light_hsl_status[8] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_100_MSEC << 6) | \
                                              context->hsl_transition.no_of_steps;
                    }
                    break;

                    case QMESH_MODEL_STEP_RESOLUTION_1_SEC:
                    {
                        light_hsl_status[8] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_1_SEC << 6) | \
                                              context->hsl_transition.no_of_steps;
                    }
                    break;

                    case QMESH_MODEL_STEP_RESOLUTION_10_SEC:
                    {
                        light_hsl_status[8] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_SEC << 6) | \
                                              context->hsl_transition.no_of_steps;
                    }
                    break;

                    case QMESH_MODEL_STEP_RESOLUTION_10_MINUTES:
                    {
                        light_hsl_status[8] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_MINUTES << 6) | \
                                              context->hsl_transition.no_of_steps;
                    }
                    break;
                }

                status_data_info.payload_len = LIGHT_HSL_TRANSITION_STATUS_SIZE;
                status_data_info.payload = light_hsl_status;
            }
            else
            {
                status_data_info.payload_len = LIGHT_HSL_STATUS_REPORT_SIZE;
                status_data_info.payload = light_hsl_status;
            }
        }
        break;

        case QMESH_LIGHT_HSL_TARGET_GET:
        {
            /* Construct the opcode data in the big endian format */
            light_hsl_status[0] = (uint8_t) ((QMESH_LIGHT_HSL_TARGET_STATUS & 0xFF00) >> 8);
            light_hsl_status[1] = (uint8_t) (QMESH_LIGHT_HSL_TARGET_STATUS & 0x00FF);
            /* Construct the payload data in the little endian format */
            light_hsl_status[2] = (uint8_t) (context->hsl_transition.target_value[0] &
                                           0x00FF);
            light_hsl_status[3] = (uint8_t) ((context->hsl_transition.target_value[0] &
                                            0xFF00) >> 8);
            light_hsl_status[4] = (uint8_t) (context->hsl_transition.target_value[1] &
                                           0x00FF);
            light_hsl_status[5] = (uint8_t) ((context->hsl_transition.target_value[1] &
                                            0xFF00) >> 8);
            light_hsl_status[6] = (uint8_t) (context->hsl_transition.target_value[2] &
                                           0x00FF);
            light_hsl_status[7] = (uint8_t) ((context->hsl_transition.target_value[2] &
                                            0xFF00) >> 8);

            /* Check if there is a transition in Progress.If so, include transition information */
            if (context->hsl_transition.transition_state != QMESH_TRANSITION_IDLE &&
                context->hsl_transition.transition_state != QMESH_TRANSITION_ABORTING)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "Filling Transition Information\n");

                switch (context->hsl_transition.step_resolution)
                {
                    case QMESH_MODEL_STEP_RESOLUTION_100_MSEC:
                    {
                        light_hsl_status[8] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_100_MSEC << 6) | \
                                              context->hsl_transition.no_of_steps;
                    }
                    break;

                    case QMESH_MODEL_STEP_RESOLUTION_1_SEC:
                    {
                        light_hsl_status[8] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_1_SEC << 6) | \
                                              context->hsl_transition.no_of_steps;
                    }
                    break;

                    case QMESH_MODEL_STEP_RESOLUTION_10_SEC:
                    {
                        light_hsl_status[8] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_SEC << 6) | \
                                              context->hsl_transition.no_of_steps;
                    }
                    break;

                    case QMESH_MODEL_STEP_RESOLUTION_10_MINUTES:
                    {
                        light_hsl_status[8] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_MINUTES << 6) | \
                                              context->hsl_transition.no_of_steps;
                    }
                    break;
                }

                status_data_info.payload_len = LIGHT_HSL_TRANSITION_STATUS_SIZE;
                status_data_info.payload = light_hsl_status;
            }
            else
            {
                status_data_info.payload_len = LIGHT_HSL_STATUS_REPORT_SIZE;
                status_data_info.payload = light_hsl_status;
            }
        }
        break;

        case QMESH_LIGHT_HSL_DEFAULT_GET:
        {
             /* Construct the payload data in the little endian format */
            uint16_t light_def = 0;
            QmeshUpdateBoundStates (context->elm_id, QAPP_LIGHT_HSL_GET_LIGHT_DEFAULT,
                                    &light_def);

            /* Construct the opcode data in the big endian format */
            light_hsl_status[0] = (uint8_t) ((QMESH_LIGHT_HSL_DEFAULT_STATUS & 0xFF00) >> 8);
            light_hsl_status[1] = (uint8_t) (QMESH_LIGHT_HSL_DEFAULT_STATUS & 0x00FF);
            /* Construct the payload data in the little endian format */
            light_hsl_status[2] = (uint8_t) (light_def & 0x00FF);
            light_hsl_status[3] = (uint8_t) ((light_def & 0xFF00) >> 8);
            light_hsl_status[4] = (uint8_t) (context->light_hsl_hue_def & 0x00FF);
            light_hsl_status[5] = (uint8_t) ((context->light_hsl_hue_def & 0xFF00) >>
                                           8);
            light_hsl_status[6] = (uint8_t) (context->light_hsl_sat_def & 0x00FF);
            light_hsl_status[7] = (uint8_t) ((context->light_hsl_sat_def  & 0xFF00) >>
                                           8);
            status_data_info.payload_len = LIGHT_HSL_DEFAULT_STATUS_SIZE;
            status_data_info.payload = light_hsl_status;
        }
        break;

        case QMESH_LIGHT_HSL_RANGE_GET:
        {
            /* Construct the opcode data in the big endian format */
            light_hsl_status[0] = (uint8_t) ((QMESH_LIGHT_HSL_RANGE_STATUS & 0xFF00) >> 8);
            light_hsl_status[1] = (uint8_t) (QMESH_LIGHT_HSL_RANGE_STATUS & 0x00FF);
            light_hsl_status[2] = (uint8_t) (context->light_range_status);
            /* Construct the payload data in the little endian format */
            light_hsl_status[3] = (uint8_t) (context->light_hsl_hue_min & 0x00FF);
            light_hsl_status[4] = (uint8_t) ((context->light_hsl_hue_min & 0xFF00) >> 8);
            light_hsl_status[5] = (uint8_t) (context->light_hsl_hue_max & 0x00FF);
            light_hsl_status[6] = (uint8_t) ((context->light_hsl_hue_max & 0xFF00) >> 8);
            light_hsl_status[7] = (uint8_t) (context->light_hsl_sat_min & 0x00FF);
            light_hsl_status[8] = (uint8_t) ((context->light_hsl_sat_min & 0xFF00) >> 8);
            light_hsl_status[9] = (uint8_t) (context->light_hsl_sat_max & 0x00FF);
            light_hsl_status[10] = (uint8_t) ((context->light_hsl_sat_max & 0xFF00) >> 8);
            status_data_info.payload_len = LIGHT_HSL_RANGE_STATUS_SIZE;
            status_data_info.payload = light_hsl_status;
        }
        break;
    }

    QmeshMutexUnlock (&context->mutex_handle);
    /* Send the status back */
    QmeshSendMessage (key_info, &status_data_info);
    return;
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
static void qmeshLightHSLServerTransitionHandler (QMESH_MODEL_MSG_T *model_msg,
        void *context)
{
    /* Retrieve the context information */
    QMESH_LIGHT_HSL_MODEL_CONTEXT_T *model_context =
        (QMESH_LIGHT_HSL_MODEL_CONTEXT_T *) \
        (context);
    /* Retreive opcode */
    uint16_t opcode = model_msg->cmn_msg.opcode;

    /* Abort any previous transition if ongoing */
    if (model_context->hsl_transition.transition_state ==
        QMESH_TRANSITION_IN_PROGRESS)
    {
        QmeshLightHSLAbortTransition (model_context);
    }

    if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

    if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

    /* Update Bound States - Cancel all other transitions if ongoing in other models */
    QmeshUpdateBoundStates (model_context->elm_id, QAPP_LIGHT_HSL_TRANS_BEGIN,
                            NULL);
    /* Prepare for the new transition process */
    model_context->hsl_transition.no_of_steps         = NUM_OF_TRANSITION_STEPS (
                g_hsltrans_time) ;
    /* Get the target value for Light HSL. */
    model_context->hsl_transition.target_value[0]     = ((
                model_msg->msg[OFFSET_VALUE_BYTE_MSB] << 8) \
            | model_msg->msg[OFFSET_VALUE_BYTE_LSB]);
     model_context->light_target_hsl = model_context->hsl_transition.target_value[0];
    
    /* Get the target value for HSL Hue */
    model_context->hsl_transition.target_value[1]     = ((
                model_msg->msg[OFFSET_VALUE_BYTE_MSB + 2] << 8) \
            | model_msg->msg[OFFSET_VALUE_BYTE_LSB + 2]);
    model_context->light_target_hue =  model_context->hsl_transition.target_value[1];   
    
    /* Get the target value for HSL Sat */
    model_context->hsl_transition.target_value[2]     = ((
                model_msg->msg[OFFSET_VALUE_BYTE_MSB + 4] << 8) \
            | model_msg->msg[OFFSET_VALUE_BYTE_LSB + 4]);
            
    model_context->light_target_sat =  model_context->hsl_transition.target_value[2];   

    /* Update HSL,Hue and Sat target value in the NVM */
    qmeshLightHslUpdateNvm(model_context);
    
    model_context->hsl_transition.step_resolution     = STEP_RESOLUTION (
                g_hsltrans_time);
    model_context->hsl_transition.opcode              = opcode;
    model_context->hsl_transition.src_addr            = model_msg->cmn_msg.src;
    model_context->hsl_transition.transition_state    = QMESH_TRANSITION_BEGIN;

    /* Calculate the step resolution in milli seconds */
    switch (model_context->hsl_transition.step_resolution)
    {
        case QMESH_MODEL_STEP_RESOLUTION_100_MSEC:
            model_context->hsl_transition.transition_duration = \
                    (uint32_t)model_context->hsl_transition.no_of_steps * QMESH_100_MILLI_SEC;
            break;

        case QMESH_MODEL_STEP_RESOLUTION_1_SEC:
            model_context->hsl_transition.transition_duration = \
                    (uint32_t)model_context->hsl_transition.no_of_steps * QMESH_SEC;
            break;

        case QMESH_MODEL_STEP_RESOLUTION_10_SEC:
            model_context->hsl_transition.transition_duration = \
                   (uint32_t) model_context->hsl_transition.no_of_steps * QMESH_10_SEC;
            break;

        case QMESH_MODEL_STEP_RESOLUTION_10_MINUTES:
            model_context->hsl_transition.transition_duration = \
                   (uint32_t) model_context->hsl_transition.no_of_steps * QMESH_10_MINUTES;
            break;

        default:
            break;
    }

    /* preserve the context data to be used in transition */
    model_context->hsl_transition.state_data          = (void *)model_context;
    /* Calculate interpolated value for HSL Light State */
    model_context->hsl_transition.linear_value[0]  = \
            ((model_context->hsl_transition.target_value[0] - model_context-> \
              light_hsl_lightness ) / model_context->hsl_transition.no_of_steps);
    /* Calculate interpolated value for HSL Hue State */
    model_context->hsl_transition.linear_value[1]  = \
            ((model_context->hsl_transition.target_value[1] - model_context-> \
              hue_context->light_hsl_hue ) / model_context->hsl_transition.no_of_steps);
    /* Calculate interpolated value for HSL Sat State */
    model_context->hsl_transition.linear_value[2]  = \
            ((model_context->hsl_transition.target_value[2] - model_context-> \
              sat_context->light_hsl_sat ) / model_context->hsl_transition.no_of_steps);
    /* Find the progress time for each step based on transition duration and no of steps*/
    model_context->hsl_transition.progress_time = \
            model_context->hsl_transition.transition_duration /
            model_context->hsl_transition.no_of_steps;
    /* Preserve the key info to transmit status information during transition */
    QmeshMemCpy (&model_context->hsl_transition.key_info,
                 &model_msg->key_info,
                 sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T)); // store key info

    /* Send back acknowledgement to client that we have accepted the transition */

    if (opcode == QMESH_LIGHT_HSL_SET)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "Transition Beginning update sent\n");
        /* Update and acknowledge the sender, that transition about to begin */
        qmeshSendLightHSLServerStatusUpdate (model_context->elm_id,
                                             model_msg->cmn_msg.src,
                                             opcode,
                                             &model_msg->key_info,
                                             model_context);
    }

    /* Begin the transition Process finally */
    model_context->hsl_transition.timer_handle = QmeshTimerCreate (&model_timer_ghdl,
                      lightHslServerTransitionTimer,
                      (void *)model_context,
                      model_context->hsl_transition.progress_time);
}

/*============================================================================*
 *  Public Function Implementations
 *===========================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHSLServerAppHandler
 *
 *  DESCRIPTION
 *      Light lightness server event handler. Handles all the model relevant
 *      messages.
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLServerHandler (const QMESH_NW_HEADER_INFO_T
        *nw_hdr,
        const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
        const QMESH_ELEMENT_CONFIG_T  *elem_data,
        QMESH_MODEL_DATA_T *model_data,
        const uint8_t *msg,
        uint16_t msg_len)
{
    QMESH_MODEL_MSG_COMMON_T msg_cmn;
    QMESH_MODEL_MSG_T cache_msg;
    uint16_t current_time[3];
    uint16_t opcode;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                      "QmeshLightHSLServerHandler Called \n");
    QMESH_LIGHT_HSL_MODEL_CONTEXT_T *model_context =
        (QMESH_LIGHT_HSL_MODEL_CONTEXT_T *) \
        (model_data->model_priv_data);

    if ((model_context == NULL) || (!msg || msg_len <= 0))
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "QmeshLightHSLServerHandler Invalid Model Context \n");
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
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER, "%s: Opcode:0x%x\n",
                          __FUNCTION__, opcode);
    }
    else
    {
        return QMESH_RESULT_FAILURE;
    }

    /* Get the current time in ms */
    QmeshGetCurrentTimeInMs (current_time);

    switch (opcode)
    {
        case QMESH_LIGHT_HSL_GET:
        case QMESH_LIGHT_HSL_TARGET_GET:
        case QMESH_LIGHT_HSL_DEFAULT_GET:
        case QMESH_LIGHT_HSL_RANGE_GET:
        {
           /* Store the sequence number and src address of the new message */
           model_context->last_msg_seq_no = nw_hdr->seq_num;
           model_context->last_src_addr = nw_hdr->src_addr;

            qmeshSendLightHSLServerStatusUpdate (model_context->elm_id,
                                                 nw_hdr->src_addr,
                                                 opcode,
                                                 key_info,
                                                 model_context);
            return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_LIGHT_HSL_SET:
        case QMESH_LIGHT_HSL_SET_UNRELIABLE:
        {
           /* Store the sequence number and src address of the new message */
           model_context->last_msg_seq_no = nw_hdr->seq_num;
           model_context->last_src_addr = nw_hdr->src_addr;

            msg += QMESH_OPCODE_FORMAT_TWO_BYTE;
            msg_len -= QMESH_OPCODE_FORMAT_TWO_BYTE;

            /* Message Validation */
            if (! (msg_len == LIGHT_HSL_NON_TRANSITION_MSG_SIZE ||
                   msg_len == LIGHT_HSL_TRANSITION_MSG_SIZE))
                return QMESH_RESULT_SUCCESS;

            QmeshMemSet(&msg_cmn, 0, sizeof(msg_cmn));

             /* Store the message for adding in cache */
             msg_cmn.opcode = opcode;
             msg_cmn.src = nw_hdr->src_addr;
             msg_cmn.dst = nw_hdr->dst_addr;
             msg_cmn.tid = msg[OFFSET_HSL_TID];

             /* Check if the message is in the cache. Returns TRUE if message
              *is a new message and cache is updated with the new message
              */
            if (QmeshModelCacheAddMsg (&msg_cmn, FALSE )){

            /* Check if DTT has any valid values */
            g_hsltrans_time = QmeshGetGenericDTT(elem_data);
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER, "DTT g_hsltrans_time %d\n",
                              g_hsltrans_time);

            /* Check if this message has a delay/transition time field */
            if (msg_len !=  LIGHT_HSL_TRANSITION_MSG_SIZE && (g_hsltrans_time == 0))
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "HSL Non transition Msg received %d\n", msg_len);

                /* Abort any previous transition if ongoing */
                if (model_context->hsl_transition.transition_state ==
                    QMESH_TRANSITION_IN_PROGRESS)
                {
                    QmeshLightHSLAbortTransition (model_context);
                }

                QmeshMutexLock (&model_context->mutex_handle);
                /* Instantaneous Update */
                model_context->light_hsl_lightness = ((msg[OFFSET_VALUE_BYTE_MSB] << 8) |
                                                      msg[OFFSET_VALUE_BYTE_LSB]);
               
                model_context->hue_context->light_hsl_hue = ((msg[OFFSET_VALUE_BYTE_MSB + 2] <<
                        8) | msg[OFFSET_VALUE_BYTE_LSB + 2]);
                model_context->sat_context->light_hsl_sat = ((msg[OFFSET_VALUE_BYTE_MSB + 4] <<
                        8) | msg[OFFSET_VALUE_BYTE_LSB + 4]);
                QmeshMutexUnlock (&model_context->mutex_handle);

                if (opcode == QMESH_LIGHT_HSL_SET)
                {
                    qmeshSendLightHSLServerStatusUpdate (model_context->elm_id, 
                                                         nw_hdr->src_addr, 
                                                         opcode,
                                                         key_info, 
                                                         model_context);
                }

                /* Update Bound States */
                QmeshUpdateBoundStates (model_context->elm_id, QAPP_LIGHT_HSL_UPDATED,
                                        &model_context->light_hsl_lightness);
                
                qmeshLightHslUpdateNvm(model_context);                                        

                if (model_context ->publish_timer != QMESH_TIMER_INVALID_HANDLE)
                    QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

                /* If publish time is valid (non-zero) start the publish timer */
                if (model_context->publish_interval != 0 &&
                    model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
                    ((model_context->publish_retrans_timer = QmeshTimerCreate (&model_timer_ghdl,
                                                                               qmeshLightHslPublishTimerCb,
                                                                               (void *)model_context,
                                                                               model_context->publish_interval))
                   == QMESH_TIMER_INVALID_HANDLE))
                {
                    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                      "Lightness HSL Publish Timer Creation Failed \n");
                }
            }
            else
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "HSL transition Msg received %d\n", msg_len);

                /* Copy 'msg_cmn', 'key_info' and level specific message */
                QmeshMemCpy (&cache_msg.cmn_msg, &msg_cmn, sizeof (QMESH_MODEL_MSG_COMMON_T));
                QmeshMemCpy (&cache_msg.key_info, key_info,
                             sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));

                /* Copy  'current_time' */
                QmeshMemCpy (&cache_msg.timestamp, current_time, sizeof (current_time));

                /* If message includes 'delay' and 'transition_time', store thses value */
                if (msg_len ==  LIGHT_HSL_TRANSITION_MSG_SIZE)
                {
                    cache_msg.delay = msg[OFFSET_HSL_DELAY];
                    cache_msg.transition_time = msg[OFFSET_HSL_TIME];
                    g_hsltrans_time = msg[OFFSET_HSL_TIME];
                }
                else
                {
                    cache_msg.delay = 0;
                     cache_msg.transition_time = g_hsltrans_time;
                }

                /* if Delay is non-zero,add the message into 'Delay' cache */
                if (cache_msg.delay > 0)
                {
                QmeshAddMsgToDelayCache (&cache_msg,
                                         qmeshLightHSLServerTransitionHandler,
                                         (void *)model_context,
                                          msg,
                                          msg_len);
                }
                else
                {
                    /* Alloc for model_msg */
                    cache_msg.msg = (uint8_t *)QmeshMalloc (QMESH_MM_SECTION_MODEL_LAYER, msg_len);

                    if(cache_msg.msg!=NULL)
                    {
                        QmeshMemCpy (cache_msg.msg, msg, msg_len);
                        cache_msg.msg_len = msg_len;

                        /* Delay is 0, so initiate transtion  */
                        qmeshLightHSLServerTransitionHandler (&cache_msg,
                                                              (void *)model_context);
                        QmeshFree((void*)cache_msg.msg);
                    }

                }
            }
            }
            return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_GENERIC_POWERONOFF_GET: /* Generic Power OnOff Messages */
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

        case QMESH_GENERIC_ONOFF_GET: /* Generic On/Off Messages */
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
                                  "Model data for GDTT model is NULL\n");
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
                                  "Model data for Level model is NULL\n");
            }

            return QMESH_RESULT_SUCCESS;
        }
        break;

        case QMESH_LIGHT_LIGHTNESS_SET: /* Lightness Messages */
        case QMESH_LIGHT_LIGHTNESS_GET:
        case QMESH_LIGHT_LIGHTNESS_SET_UNRELIABLE:
        case QMESH_LIGHT_LIGHTNESS_LINEAR_GET:
        case QMESH_LIGHT_LIGHTNESS_LINEAR_SET:
        case QMESH_LIGHT_LIGHTNESS_LINEAR_SET_UNRELIABLE:
        case QMESH_LIGHT_LIGHTNESS_LAST_GET:
        case QMESH_LIGHT_LIGHTNESS_DEFAULT_GET:
        case QMESH_LIGHT_LIGHTNESS_RANGE_GET:
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
                                  "Model data for Lightness model is NULL\n");
            }

            return QMESH_RESULT_SUCCESS;
        }
        break;

        default:
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "Unknown Message HSL handler\n");
            return QMESH_RESULT_FAILURE;
        }
        break;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshAbortLightHSLTransition
 *
 *  DESCRIPTION
 *      Abort ongoing light HSL transition if any
 *
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshLightHSLAbortTransition (QMESH_LIGHT_HSL_MODEL_CONTEXT_T
        *model_context)
{
    if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

    if (model_context->publish_retrans_timer != QMESH_TIMER_INVALID_HANDLE)
       QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

    /* Abort any previous transition if ongoing */
    if (model_context->hsl_transition.transition_state == QMESH_TRANSITION_IN_PROGRESS ||
        model_context->hsl_transition.transition_state == QMESH_TRANSITION_BEGIN)
    {
      model_context->hsl_transition.transition_state = QMESH_TRANSITION_ABORTING;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHSLServerAppInit
 *
 *  DESCRIPTION
 *      Application calls this API to initialize the Light HSL context information
 *
 *  RETURNS/MODIFIES
 *      returns QMESH_RESULT_SUCCESS
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLServerAppInit (QMESH_LIGHT_HSL_MODEL_CONTEXT_T
        *context)
{
    if (context)
        context->hsl_transition.transition_state = QMESH_TRANSITION_IDLE;

    if (QmeshMutexCreate (&context->mutex_handle) != QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "%s:Creating mutex Failed\n", __FUNCTION__);
        return QMESH_RESULT_FAILURE;
    }

    return QMESH_RESULT_SUCCESS;
}

/*--------------------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHSLServerAppDeInit
 *
 *  DESCRIPTION
 *      Application calls this API to deinitialize the Light HSL context information
 *
 *  RETURNS/MODIFIES
 *      returns QMESH_RESULT_SUCCESS
 *-------------------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLServerAppDeInit (
    QMESH_LIGHT_HSL_MODEL_CONTEXT_T *context)
{
    if (context)
    {
        QmeshLightHSLAbortTransition (context);
        QmeshMutexDestroy (&context->mutex_handle);
    }

    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGetLightHsl
 *
 *  DESCRIPTION
 *      Application calls this API to get light HSL value
 *
 *  RETURNS/MODIFIES
 *      returns light HSL value
 *
 *
 *----------------------------------------------------------------------------*/
uint16_t QmeshLightHslGet (QMESH_LIGHT_HSL_MODEL_CONTEXT_T *context)
{
    return context->light_hsl_lightness;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHslSet
 *
 *  DESCRIPTION
 *      Application calls this API to set light HSL value
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *----------------------------------------------------------------------------*/
void QmeshLightHslSet (QMESH_LIGHT_HSL_MODEL_CONTEXT_T *context,
                       uint8_t  transition_time,
                       uint8_t  delay,
                       uint16_t light_hsl)
{
    /* Note: transition_time and delay params are for future requirements */
    if (context)
    {
        /* Abort any ongoing transition */
        QmeshLightHSLAbortTransition (context);
        /* Update light hsl value */
        context->light_hsl_lightness = light_hsl;
        
        /* Update Bound States */
        QmeshUpdateBoundStates (context->elm_id,
                                QAPP_LIGHT_HSL_UPDATED,
                                &context->light_hsl_lightness);
        qmeshLightHslUpdateNvm(context);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHslUpdate
 *
 *  DESCRIPTION
 *      Application calls this API to update light HSL value
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *----------------------------------------------------------------------------*/
extern void QmeshLightHslUpdate (QMESH_LIGHT_HSL_MODEL_CONTEXT_T *context,
                                 uint16_t light_hsl)
{
    if (context)
    {
        context->light_hsl_lightness = light_hsl;
        qmeshLightHslUpdateNvm(context);   
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHslStatusPublish
 *
 *  DESCRIPTION
 *      This function sends the current Light HSL Status message to the
 *  Publish address assigned for this model.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshLightHslStatusPublish (QMESH_LIGHT_HSL_MODEL_CONTEXT_T
                                        *model_context)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;
    /* Lightness Status report buffer */
    uint8_t hsl_status[LIGHT_HSL_TRANSITION_MSG_SIZE];
    /* Construct the opcode data in the big endian format */
    hsl_status[0] = (uint8_t) ((QMESH_LIGHT_HSL_STATUS & 0xFF00) >> 8);
    hsl_status[1] = (uint8_t) (QMESH_LIGHT_HSL_STATUS & 0x00FF);
    /* Construct the payload data in the little endian format */
    hsl_status[2] = (uint8_t) (model_context->light_hsl_lightness & 0x00FF);
    hsl_status[3] = (uint8_t) ((model_context->light_hsl_lightness & 0xFF00) >> 8);
    hsl_status[4] = (uint8_t) (model_context->hue_context->light_hsl_hue & 0x00FF);
    hsl_status[5] = (uint8_t) ((model_context->hue_context->light_hsl_hue & 0xFF00) >>
                             8);
    hsl_status[6] = (uint8_t) (model_context->sat_context->light_hsl_sat & 0x00FF);
    hsl_status[7] = (uint8_t) ((model_context->sat_context->light_hsl_sat & 0xFF00) >>
                             8);

    if (model_context->hsl_transition.transition_state != QMESH_TRANSITION_IDLE)
    {
        data_info.payload_len = LIGHT_HSL_STATUS_REPORT_SIZE;
    }
    else
    {
        switch (model_context->hsl_transition.step_resolution)
        {
            case QMESH_MODEL_STEP_RESOLUTION_100_MSEC:
            {
                hsl_status[8] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_100_MSEC << 6) | \
                                model_context->hsl_transition.no_of_steps;
            }
            break;

            case QMESH_MODEL_STEP_RESOLUTION_1_SEC:
            {
                hsl_status[8] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_1_SEC << 6) | \
                                model_context->hsl_transition.no_of_steps;
            }
            break;

            case QMESH_MODEL_STEP_RESOLUTION_10_SEC:
            {
                hsl_status[8] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_SEC << 6) | \
                                model_context->hsl_transition.no_of_steps;
            }
            break;

            case QMESH_MODEL_STEP_RESOLUTION_10_MINUTES:
            {
                hsl_status[8] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_MINUTES << 6) | \
                                model_context->hsl_transition.no_of_steps;
            }
            break;
        }

        data_info.payload_len = LIGHT_HSL_TRANSITION_STATUS_SIZE;
    }

    data_info.payload = hsl_status;
    data_info.ttl = model_context->publish_state->publish_ttl;
    data_info.src_addr = model_context->elm_id;
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
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "%s %d QmeshSendMessage failed\n", __FUNCTION__, __LINE__);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHslUpdatePublicationPeriod
 *
 *  DESCRIPTION
 *      Calculates the publish interval from the Publish state data for the model.
 *
 *  RETURNS/MODIFIES
 *
 *----------------------------------------------------------------------------*/
extern bool QmeshLightHslUpdatePublicationPeriod (QMESH_MODEL_DATA_T
        *model_data)
{
    uint8_t no_of_steps, step_resolution;
    QMESH_LIGHT_HSL_MODEL_CONTEXT_T *model_context = NULL;
    /* Get the model context data for the element ID 'elm_addr' from the composition data */
    model_context = (QMESH_LIGHT_HSL_MODEL_CONTEXT_T *) model_data->model_priv_data;
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
 *      QmeshLightHslPublicationHandler
 *
 *  DESCRIPTION
 *      Publication configuration handler for Light HSL Model
 *
 *  RETURNS/MODIFIES
 *      None
 *----------------------------------------------------------------------------*/
extern void QmeshLightHslPublicationHandler (QMESH_MODEL_DATA_T *model_data)
{
    QMESH_LIGHT_HSL_MODEL_CONTEXT_T *model_context = NULL;
    /* Get the model context from the composition data */
    model_context = (QMESH_LIGHT_HSL_MODEL_CONTEXT_T *) model_data->model_priv_data;

    if (QmeshLightHslUpdatePublicationPeriod (model_data))
    {
        if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        {
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

            /* Start publish timer based on publish interval */
            if ((model_context->publish_interval > 0) &&
                (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
                ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightHslPublishTimerCb,
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
