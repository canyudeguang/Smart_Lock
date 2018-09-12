/******************************************************************************
 *  Copyright 2017-2018 Qualcomm Technologies International, Ltd.
 ******************************************************************************/
/*! \file qmesh_light_hsl_saturation_handler.c
 *  \brief defines and functions for SIGMesh Light HSL Saturation model.
 */
/******************************************************************************/

#include "qmesh_model_common.h"
#include "qmesh_light_hsl_saturation_handler.h"
#include "qmesh_generic_level_handler.h"
#include "qmesh_model_nvm.h"
/*============================================================================*
 *  Private Data
 *===========================================================================*/

/* Lightness Status report buffer */
uint8_t light_hsl_sat_status[LIGHT_HSL_SAT_TRANSITION_STATUS_SIZE];

/* Variable to store transition information if any(from DTT & transition provided)
 * by client
 */
uint8_t g_sattrans_time = 0;

/*============================================================================*
 *  Private Function Implementations
 *===========================================================================*/
 
/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshLightSatUpdateNvm
 *
 *  DESCRIPTION
 *      This function writes the current state to NVM.
 *
 *  RETURNS/MODIFIES
 *      None.
 *
 *----------------------------------------------------------------------------*/
static void qmeshLightSatUpdateNvm(QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T 
                                                     *model_context)
{
    QMESH_MODEL_LIGHT_SAT_NVM_STATE_T nvm_state;

    nvm_state.light_hsl_sat = model_context->light_hsl_sat; 
    nvm_state.last_msg_seq_no = model_context->last_msg_seq_no;
    nvm_state.last_src_addr = model_context->last_src_addr;  
    nvm_state.light_target_sat = model_context->light_target_sat; 

    /* Update NVM */
    NVMWrite_ModelLightSatState((uint8_t*)&nvm_state);
} 
/*----------------------------------------------------------------------------*
*  NAME
*      qmeshLightHslSatPublishRetransTimerCb
*
*  DESCRIPTION
*      This function sends the Light HSL sat message to the publish address for
*  the HSL model periodically every 'Publish_Period' until new tranistion
*  begins.
*
*  RETURNS/MODIFIES
*      None
*
*----------------------------------------------------------------------------*/
static void qmeshLightHslSatPublishRetransTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    /* Retrieve the context */
    QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *model_context =
        (QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *)context;
    model_context->publish_timer = QMESH_TIMER_INVALID_HANDLE;
    /* Send Status Message to Publish Address */
    QmeshLightHslSatStatusPublish (model_context);
    model_context->publish_restrans_cnt--;

    /* Restart the timer */
    if (model_context->publish_restrans_cnt)
    {
        if ((model_context->publish_interval > 0) &&
            (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
            ((model_context->publish_retrans_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightHslSatPublishRetransTimerCb,
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
*      qmeshLightHslSatPublishTimerCb
*
*  DESCRIPTION
*      This function sends the HSL Hue status message to the publish address for
*  the HSl model periodically every 'Publish_Period' until new tranistion
*  begins.
*
*  RETURNS/MODIFIES
*      None
*
*----------------------------------------------------------------------------*/
static void qmeshLightHslSatPublishTimerCb (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GEN_ONOFF_SERVER, "%s:\n", __FUNCTION__);
    /* Retrieve the context */
    QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *model_context =
        (QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *)context;
    /* Send Status Message to Publish Address. Enable below code with new stack release */
    QmeshLightHslSatStatusPublish (model_context);
    model_context ->publish_timer = QMESH_TIMER_INVALID_HANDLE;

    /* Stop any retransmission of publish message */
    if (model_context->publish_retrans_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

    /* Send Message to Publish Address periodically after every
       "Publish_Period" */
    if ((model_context->publish_interval > 0) &&
        (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
        ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightHslSatPublishTimerCb,
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
                   qmeshLightHslSatPublishRetransTimerCb,
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
 *      lightHslSatServerTransitionTimer
 *
 *  DESCRIPTION
 *      This function lineraly increments the value after transition timer
 *      expiry and restarts the transition timer for next linear value change.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
static void lightHslSatServerTransitionTimer (QMESH_TIMER_HANDLE_T timerHandle,
        void *context)
{
    /* Extract transition state information and lightness context data */
    QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *model_context = \
            (QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *)context;
    QMESH_SAT_TRANSITION_INFO *p_info = (QMESH_SAT_TRANSITION_INFO *) (
                                            &model_context->sat_transition);
    /* Delete the current transition timer */
    QmeshTimerDelete (&model_timer_ghdl,&p_info->timer_handle);
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
            qmeshLightSatUpdateNvm(model_context); 
            /* Stop publishing status messages */
            if (model_context ->publish_timer != QMESH_TIMER_INVALID_HANDLE)
                QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

            if (model_context ->publish_retrans_timer != QMESH_TIMER_INVALID_HANDLE)
                QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

            QmeshMutexLock (&model_context->mutex_handle);
            model_context->light_hsl_sat = (int16_t) (model_context->light_hsl_sat) + \
                                           p_info->linear_value;
            QmeshMutexUnlock (&model_context->mutex_handle);
            /* Update Bound States */
            QmeshUpdateBoundStates (model_context->elm_id, QAPP_LIGHT_VALIDATE_SAT,
                                    &model_context->light_hsl_sat);

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
                                                     lightHslSatServerTransitionTimer,
                                                     (void *)model_context,
                                                     p_info->progress_time);

            QmeshUpdateBoundStates (model_context->elm_id, QAPP_LIGHT_SAT_UPDATED,
                                    &model_context->light_hsl_sat);
        }
        break;

        case QMESH_TRANSITION_END:
        {
            /* Mark the transition state to idle */
            p_info->transition_state = QMESH_TRANSITION_IDLE;
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER, "Transition Ended\n");
            QmeshMutexLock (&model_context->mutex_handle);
            model_context->light_hsl_sat = p_info->target_value;
            QmeshMutexUnlock (&model_context->mutex_handle);
            /* Update Bound States */
            QmeshUpdateBoundStates (model_context->elm_id, QAPP_LIGHT_VALIDATE_SAT,
                                    &model_context->light_hsl_sat);
            /* reset g_sattrans_time */
            g_sattrans_time = 0;
            /* Update Bound States for other models*/
            QmeshUpdateBoundStates (model_context->elm_id, QAPP_LIGHT_SAT_UPDATED,
                                    &model_context->light_hsl_sat);
             model_context->light_target_sat = 0;
             qmeshLightSatUpdateNvm(model_context);                                      

            /* If publish time is valid (non-zero) start the publish timer */
            if (model_context->publish_interval != 0 &&
                model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
                ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightHslSatPublishTimerCb,
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
            g_sattrans_time = 0;

            /* Set the transition state back to idle */
            model_context->sat_transition.transition_state = QMESH_TRANSITION_IDLE;

            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                              "Sat Transition Aborted\n");
        }
        break;

        default:
            break;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      qmeshSendLightHSLSatServerStatusUpdate
 *
 *  DESCRIPTION
 *      Send Present Light HSL Saturation Message(based on opcode) with/without transition
 *      information.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
static void qmeshSendLightHSLSatServerStatusUpdate (uint16_t elem_id,
        uint16_t src_addr,
        uint16_t opcode,
        const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
        QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *context)
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
        case QMESH_LIGHT_HSL_SAT_GET:
        case QMESH_LIGHT_HSL_SAT_SET:
        {
            /* Construct the opcode data in the big endian format */
            light_hsl_sat_status[0] = (uint8_t) ((QMESH_LIGHT_HSL_SAT_STATUS & 0xFF00) >> 8);
            light_hsl_sat_status[1] = (uint8_t) (QMESH_LIGHT_HSL_SAT_STATUS & 0x00FF);
            /* Construct the payload data in the little endian format */
            light_hsl_sat_status[2] = (uint8_t) (context->light_hsl_sat & 0x00FF);
            light_hsl_sat_status[3] = (uint8_t) ((context->light_hsl_sat & 0xFF00) >> 8);

            /* Check if there is a transition in Progress.If so, include transition information */
            if (context->sat_transition.transition_state != QMESH_TRANSITION_IDLE &&
                context->sat_transition.transition_state != QMESH_TRANSITION_ABORTING)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                                  "Filling Transition Information\n");
                light_hsl_sat_status[4] = (uint8_t) (context->sat_transition.target_value &
                                                   0x00FF);
                light_hsl_sat_status[5] = (uint8_t) ((context->sat_transition.target_value &
                                                    0xFF00) >> 8);

                switch (context->sat_transition.step_resolution)
                {
                    case QMESH_MODEL_STEP_RESOLUTION_100_MSEC:
                    {
                        light_hsl_sat_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_100_MSEC << 6) | \
                                                  context->sat_transition.no_of_steps;
                    }
                    break;

                    case QMESH_MODEL_STEP_RESOLUTION_1_SEC:
                    {
                        light_hsl_sat_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_1_SEC << 6) | \
                                                  context->sat_transition.no_of_steps;
                    }
                    break;

                    case QMESH_MODEL_STEP_RESOLUTION_10_SEC:
                    {
                        light_hsl_sat_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_SEC << 6) | \
                                                  context->sat_transition.no_of_steps;
                    }
                    break;

                    case QMESH_MODEL_STEP_RESOLUTION_10_MINUTES:
                    {
                        light_hsl_sat_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_MINUTES << 6) |
                                                  \
                                                  context->sat_transition.no_of_steps;
                    }
                    break;
                }

                status_data_info.payload_len = LIGHT_HSL_SAT_TRANSITION_STATUS_SIZE;
                status_data_info.payload = light_hsl_sat_status;
            }
            else
            {
                status_data_info.payload_len = LIGHT_HSL_SAT_STATUS_REPORT_SIZE;
                status_data_info.payload = light_hsl_sat_status;
            }
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
 *      qmeshLightHSLSatServerTransitionHandler
 *
 *  DESCRIPTION
 *      This function initiates transition process for Light HSL Saturation State.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
static void qmeshLightHSLSatServerTransitionHandler (QMESH_MODEL_MSG_T
        *model_msg, void *context)
{
    /* Retrieve the context information */
    QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *model_context =
        (QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *) \
        (context);
    /* Retreive opcode */
    uint16_t opcode = model_msg->cmn_msg.opcode;

    /* Abort any previous transition if ongoing */
    if (model_context->sat_transition.transition_state ==
        QMESH_TRANSITION_IN_PROGRESS)
    {
        QmeshLightHSLSatAbortTransition (model_context);
    }

    if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

    if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_retrans_timer);

    /* Update Bound States - Cancel all other transitions if ongoing in other models */
    //QmeshUpdateBoundStates(model_msg->cmn_msg.elem_id,QAPP_LIGHTNESS_ACTUAL_TRANS_BEGIN,NULL);
    /* Prepare for the new transition process */
    model_context->sat_transition.no_of_steps         = NUM_OF_TRANSITION_STEPS (
                g_sattrans_time) ;
    /* Get the target value for Light Hue. */
    model_context->sat_transition.target_value        = ((
                model_msg->msg[OFFSET_VALUE_BYTE_MSB] << 8) \
            | model_msg->msg[OFFSET_VALUE_BYTE_LSB]);
    model_context->light_target_sat =   model_context->sat_transition.target_value;   

    /* Update NVM with Saturation Context */
    qmeshLightSatUpdateNvm(model_context);  
    
    model_context->sat_transition.step_resolution     = STEP_RESOLUTION (
                g_sattrans_time);
    model_context->sat_transition.opcode              = opcode;
    model_context->sat_transition.src_addr            = model_msg->cmn_msg.src;
    model_context->sat_transition.transition_state    = QMESH_TRANSITION_BEGIN;

    /* Calculate the step resolution in milli seconds */
    switch (model_context->sat_transition.step_resolution)
    {
        case QMESH_MODEL_STEP_RESOLUTION_100_MSEC:
            model_context->sat_transition.transition_duration = \
                   (uint32_t)model_context->sat_transition.no_of_steps * QMESH_100_MILLI_SEC;
            break;

        case QMESH_MODEL_STEP_RESOLUTION_1_SEC:
            model_context->sat_transition.transition_duration = \
                    (uint32_t)model_context->sat_transition.no_of_steps * QMESH_SEC;
            break;

        case QMESH_MODEL_STEP_RESOLUTION_10_SEC:
            model_context->sat_transition.transition_duration = \
                    (uint32_t)model_context->sat_transition.no_of_steps * QMESH_10_SEC;
            break;

        case QMESH_MODEL_STEP_RESOLUTION_10_MINUTES:
            model_context->sat_transition.transition_duration = \
                    (uint32_t)model_context->sat_transition.no_of_steps * QMESH_10_MINUTES;
            break;

        default:
            break;
    }

    /* preserve the context data to be used in transition */
    model_context->sat_transition.state_data          = (void *)model_context;
    /* Calculate interpolated value for HSL Light State */
    model_context->sat_transition.linear_value  = \
            ((model_context->sat_transition.target_value - model_context-> \
              light_hsl_sat) / model_context->sat_transition.no_of_steps);
    /* Find the progress time for each step based on transition duration and no of steps*/
    model_context->sat_transition.progress_time = \
            model_context->sat_transition.transition_duration /
            model_context->sat_transition.no_of_steps;
    /* Preserve the key info to transmit status information during transition */
    QmeshMemCpy (&model_context->sat_transition.key_info,
                 &model_msg->key_info,
                 sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T)); // store key info

    /* Send back acknowledgement to client that we have accepted the transition */
    if (opcode == QMESH_LIGHT_HSL_SAT_SET)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "Transition Beginning update sent\n");
        /* Update and acknowledge the sender, that transition about to begin */
        qmeshSendLightHSLSatServerStatusUpdate (model_context->elm_id,
                                                model_msg->cmn_msg.src,
                                                opcode,
                                                &model_msg->key_info,
                                                model_context);
    }

    /* Begin the transition process finally */
    model_context->sat_transition.timer_handle = QmeshTimerCreate (&model_timer_ghdl,
                       lightHslSatServerTransitionTimer,
                       (void *)model_context,
                       model_context->sat_transition.progress_time);

}
/*============================================================================*
 *  Public Function Implementations
 *===========================================================================*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHSLSaturationServerHandler
 *
 *  DESCRIPTION
 *      Light HSL Saturation Server event handler.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLSaturationServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
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
    QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *model_context =
        (QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *) \
        (model_data->model_priv_data);

    if ((model_context == NULL) || (!msg || msg_len <= 0))
    {
        return QMESH_RESULT_FAILURE;
    }

    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                      "%s %d Saturation Handler\n", __FUNCTION__, __LINE__);

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

    /* Store the sequence number and src address of the new message */
    model_context->last_msg_seq_no = nw_hdr->seq_num;
    model_context->last_src_addr = nw_hdr->src_addr;

    /* Get the current time in ms */
    QmeshGetCurrentTimeInMs (current_time);
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                      "%s %d Saturation Opcode Processing\n", __FUNCTION__, __LINE__);

    switch (opcode)
    {
        case QMESH_LIGHT_HSL_SAT_GET:
        {
            qmeshSendLightHSLSatServerStatusUpdate (model_context->elm_id,
                                                    nw_hdr->src_addr,
                                                    opcode,
                                                    key_info,
                                                    model_context);
            return QMESH_RESULT_SUCCESS;
        }
        break;

        case  QMESH_LIGHT_HSL_SAT_SET:
        case  QMESH_LIGHT_HSL_SAT_SET_UNRELIABLE:
        {
            msg += QMESH_OPCODE_FORMAT_TWO_BYTE;
            msg_len -= QMESH_OPCODE_FORMAT_TWO_BYTE;

            /* Message Validation */
            if (! (msg_len == LIGHT_HSL_SAT_TRANSITION_MSG_SIZE ||
                   msg_len == LIGHT_HSL_SAT_NON_TRANSITION_MSG_SIZE))
                return QMESH_RESULT_SUCCESS;

            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                              "%s %d SAT Received correct Opcode\n", __FUNCTION__, __LINE__);

            QmeshMemSet(&msg_cmn, 0, sizeof(msg_cmn));

            /* Store the message for adding in cache */
            msg_cmn.opcode = opcode;
            msg_cmn.src = nw_hdr->src_addr;
            msg_cmn.dst = nw_hdr->dst_addr;
            msg_cmn.tid = msg[OFFSET_SAT_TID];

            /* Check if the message is in the cache. Returns TRUE if message
             *is a new message and cache is updated with the new message
             */
            if (QmeshModelCacheAddMsg (&msg_cmn, FALSE )){
            /* Check if DTT has any valid values */
            g_sattrans_time = QmeshGetDTTFromPrimElement();
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER, "DTT g_sattrans_time %d\n",
                              g_sattrans_time);

            /* Check if this message has a delay/transition time field */
            if (msg_len !=  LIGHT_HSL_SAT_TRANSITION_MSG_SIZE && (g_sattrans_time == 0))
            {
                /* Abort any previous transition if ongoing */
                if (model_context->sat_transition.transition_state ==
                    QMESH_TRANSITION_IN_PROGRESS)
                {
                    QmeshLightHSLSatAbortTransition (model_context);
                }

                QmeshMutexLock (&model_context->mutex_handle);
                /* Instantaneous Update */
                model_context->light_hsl_sat = ((msg[OFFSET_VALUE_BYTE_MSB] << 8) |
                                                msg[OFFSET_VALUE_BYTE_LSB]);
                model_context->light_target_sat = model_context->light_hsl_sat;
                QmeshMutexUnlock (&model_context->mutex_handle);
                /* Update Bound States */
                QmeshUpdateBoundStates (model_context->elm_id, QAPP_LIGHT_VALIDATE_SAT,
                                        &model_context->light_hsl_sat);

                if (opcode == QMESH_LIGHT_HSL_SAT_SET)
                {
                    qmeshSendLightHSLSatServerStatusUpdate (model_context->elm_id, 
                                                            nw_hdr->src_addr,
                                                            opcode, key_info, 
                                                            model_context);
                }

                /* Update Bound States */
                QmeshUpdateBoundStates (model_context->elm_id, QAPP_LIGHT_SAT_UPDATED,
                                        &model_context->light_hsl_sat);

                 qmeshLightSatUpdateNvm(model_context);

                if (model_context ->publish_timer != QMESH_TIMER_INVALID_HANDLE)
                    QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

                /* If publish time is valid (non-zero) start the publish timer */
                if (model_context->publish_interval != 0 &&
                    model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS &&
                    ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightHslSatPublishTimerCb,
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
                /* Copy 'msg_cmn', 'key_info' and level specific message */
                QmeshMemCpy (&cache_msg.cmn_msg, &msg_cmn, sizeof (QMESH_MODEL_MSG_COMMON_T));
                QmeshMemCpy (&cache_msg.key_info, key_info,
                             sizeof (QMESH_ACCESS_PAYLOAD_KEY_INFO_T));

                /* Copy  'current_time' */
                QmeshMemCpy (&cache_msg.timestamp, current_time, sizeof (current_time));

                /* If message includes 'delay' and 'transition_time', store thses value */
                if (msg_len ==  LIGHT_HSL_SAT_TRANSITION_MSG_SIZE)
                {
                    cache_msg.delay = msg[OFFSET_SAT_DELAY];
                    cache_msg.transition_time = msg[OFFSET_SAT_TIME];
                    g_sattrans_time = msg[OFFSET_SAT_TIME];
                }
                else
                {
                    cache_msg.delay = 0;
                    cache_msg.transition_time = g_sattrans_time;
                }

                /* if Delay is non-zero,add the message into 'Delay' cache */
                if (cache_msg.delay > 0)
                {
                    QmeshAddMsgToDelayCache (&cache_msg,
                                             qmeshLightHSLSatServerTransitionHandler,
                                             (void *)model_context,
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

                        /* Delay is 0, so initiate transtion  */
                        qmeshLightHSLSatServerTransitionHandler (&cache_msg,
                                                                 (void *)model_context);
                        QmeshFree((void*)cache_msg.msg);
                    }
                }
            }
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
 *      QmeshLightHSLSatAbortTransition
 *
 *  DESCRIPTION
 *      Abort ongoing light HSL Saturation transition
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
void QmeshLightHSLSatAbortTransition (QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T
                                      *context)
{
    if (context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&context->publish_timer);

    if (context->publish_retrans_timer != QMESH_TIMER_INVALID_HANDLE)
        QmeshTimerDelete (&model_timer_ghdl,&context->publish_retrans_timer);

      /* Abort any previous transition if ongoing */
    if (context->sat_transition.transition_state == QMESH_TRANSITION_IN_PROGRESS ||
        context->sat_transition.transition_state == QMESH_TRANSITION_BEGIN)
    {
      context->sat_transition.transition_state = QMESH_TRANSITION_ABORTING;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHSLSatServerAppInit
 *
 *  DESCRIPTION
 *      Application calls this API to initialize Light HSL Saturation context.
 *
 *  RETURNS/MODIFIES
 *      returns QMESH_RESULT_SUCCESS
 *
 *----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLSatServerAppInit (
    QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *context)
{
    if (context)
        context->sat_transition.transition_state = QMESH_TRANSITION_IDLE;

    if (QmeshMutexCreate (&context->mutex_handle) != QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "%s:Creating mutex Failed\n", __FUNCTION__);
        return QMESH_RESULT_FAILURE;
    }

    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHSLSatServerAppDeInit
 *
 *  DESCRIPTION
 *      This function De-initializes Light HSL Saturation Context.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshLightHSLSatServerAppDeInit (QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T
        *context)
{
    if (context)
    {
        context->sat_transition.transition_state = QMESH_TRANSITION_IDLE;
        QmeshMutexDestroy (&context->mutex_handle);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHslSatGet
 *
 *  DESCRIPTION
 *      This function returns the Light HSL Saturation value.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
uint16_t QmeshLightHslSatGet (QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *context)
{
    return context->light_hsl_sat;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHslSatSet
 *
 *  DESCRIPTION
 *      This function sets the Light HSL Saturation value. It aborts any ongoing
 *  transition and updates bound states.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
void QmeshLightHslSatSet (QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *context,
                          uint8_t transition_time,
                          uint8_t delay,
                          uint16_t hsl_sat)
{
    if (context)
    {
        /* Abort any ongoing transition */
        QmeshLightHSLSatAbortTransition (context);
        /* Set the HSL saturation value */
        context->light_hsl_sat =    hsl_sat;
        /* Update Bound States for other models*/
        QmeshUpdateBoundStates (context->elm_id, QAPP_LIGHT_SAT_UPDATED,
                                &context->light_hsl_sat);

        qmeshLightSatUpdateNvm(context);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHslSatUpdate
 *
 *  DESCRIPTION
 *      This function updates Light HSL Saturation State value.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshLightHslSatUpdate (QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T
                                    *context,
                                    uint16_t hsl_sat)
{
    if (context)
    {
        /* Set the HSL saturation value */
        context->light_hsl_sat =    hsl_sat;
        qmeshLightSatUpdateNvm(context);               
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHslSatStatusPublish
 *
 *  DESCRIPTION
 *      This function sends the current Light HSL Status message to the
 *  Publish address assigned for this model.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
extern void QmeshLightHslSatStatusPublish (QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T
        *context)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;
    /* Lightness Status report buffer */
    uint8_t sat_status[LIGHT_HSL_SAT_TRANSITION_STATUS_SIZE];
    /* Construct the opcode data in the big endian format */
    sat_status[0] = (uint8_t) ((QMESH_LIGHT_HSL_SAT_STATUS & 0xFF00) >> 8);
    sat_status[1] = (uint8_t) (QMESH_LIGHT_HSL_SAT_STATUS & 0x00FF);
    /* Construct the payload data in the little endian format */
    sat_status[2] = (uint8_t) (context->light_hsl_sat & 0x00FF);
    sat_status[3] = (uint8_t) ((context->light_hsl_sat & 0xFF00) >> 8);

    /* Check if there is a transition in Progress.If so, include transition information */
    if (context->sat_transition.transition_state != QMESH_TRANSITION_IDLE &&
        context->sat_transition.transition_state != QMESH_TRANSITION_ABORTING)
    {
        sat_status[4] = (uint8_t) (context->sat_transition.target_value & 0x00FF);
        sat_status[5] = (uint8_t) ((context->sat_transition.target_value & 0xFF00) >> 8);

        switch (context->sat_transition.step_resolution)
        {
            case QMESH_MODEL_STEP_RESOLUTION_100_MSEC:
            {
                sat_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_100_MSEC << 6) | \
                                context->sat_transition.no_of_steps;
            }
            break;

            case QMESH_MODEL_STEP_RESOLUTION_1_SEC:
            {
                sat_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_1_SEC << 6) | \
                                context->sat_transition.no_of_steps;
            }
            break;

            case QMESH_MODEL_STEP_RESOLUTION_10_SEC:
            {
                sat_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_SEC << 6) | \
                                context->sat_transition.no_of_steps;
            }
            break;

            case QMESH_MODEL_STEP_RESOLUTION_10_MINUTES:
            {
                sat_status[6] = ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_MINUTES << 6) | \
                                context->sat_transition.no_of_steps;
            }
            break;
        }

        data_info.payload_len = LIGHT_HSL_SAT_TRANSITION_STATUS_SIZE;
        data_info.payload = sat_status;
    }
    else
    {
        data_info.payload_len = LIGHT_HSL_SAT_STATUS_REPORT_SIZE;
        data_info.payload = sat_status;
    }

    data_info.payload = sat_status;
    data_info.ttl = context->publish_state->publish_ttl;
    data_info.src_addr = context->elm_id;
    data_info.dst_addr = context->publish_state->publish_addr;
    data_info.trans_ack_required = FALSE;
    data_info.trans_mic_size = QMESH_MIC_4_BYTES;
    /* key info */
    app_key_info.key_type = QMESH_KEY_TYPE_APP_KEY;
    app_key_info.key_info.prov_net_idx = 0;
    app_key_info.key_info.key_idx = context->publish_state->app_key_idx & 0xFF;

    if (QmeshSendMessage (&app_key_info, &data_info) != QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_LIGHTNESS_SERVER,
                          "%s %d QmeshSendMessage failed\n", __FUNCTION__, __LINE__);
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHslSatUpdatePublicationPeriod
 *
 *  DESCRIPTION
 *      Calculates the publish interval from the Publish state data for the model.
 *
 *  RETURNS/MODIFIES
 *
 *----------------------------------------------------------------------------*/
extern bool QmeshLightHslSatUpdatePublicationPeriod (QMESH_MODEL_DATA_T
        *model_data)
{
    uint8_t no_of_steps, step_resolution;
    QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *model_context = NULL;
    /* Get the model context data for the element ID 'elm_addr' from the composition data */
    model_context = (QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *)
                    model_data->model_priv_data;
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
 *      QmeshLightHslSatPublicationHandler
 *
 *  DESCRIPTION
 *      Publication configuration handler for Light HSL Hue Model
 *
 *  RETURNS/MODIFIES
 *      None
 *----------------------------------------------------------------------------*/
extern void QmeshLightHslSatPublicationHandler (QMESH_MODEL_DATA_T *model_data)
{
    QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *model_context = NULL;
    /* Get the model context from the composition data */
    model_context = (QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *)
                    model_data->model_priv_data;

    if (QmeshLightHslSatUpdatePublicationPeriod (model_data))
    {
        if (model_context->publish_timer != QMESH_TIMER_INVALID_HANDLE)
        {
            QmeshTimerDelete (&model_timer_ghdl,&model_context->publish_timer);

            /* Start publish timer based on publish interval */
            if ((model_context->publish_interval > 0) &&
                (model_context->publish_state->publish_addr != QMESH_UNASSIGNED_ADDRESS) &&
                ((model_context->publish_timer = QmeshTimerCreate (&model_timer_ghdl,
                   qmeshLightHslSatPublishTimerCb,
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