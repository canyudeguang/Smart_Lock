/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_model_common.c
 *  \brief defines and functions for model
 *
 *   This file contains the implementation of common methods used for model server implementations
 */
/******************************************************************************/
#include "qmesh_model_common.h"
#include "qmesh_model_debug.h"

#include "qmesh_generic_onoff_handler.h"
#include "qmesh_generic_level_handler.h"
#include "qmesh_generic_poweronoff_handler.h"
#include "qmesh_generic_default_transition_time_handler.h"
#include "qmesh_light_hsl_setup_handler.h"
#include "qmesh_light_lightness_handler.h"
#include "qmesh_light_hsl_handler.h"
#include "qmesh_light_hsl_hue_handler.h"
#include "qmesh_light_hsl_saturation_handler.h"

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
#include "qmesh_generic_powerlevel_handler.h"
#endif

const QMESH_DEVICE_COMPOSITION_DATA_T* server_composition;

QMESH_TIMER_GROUP_HANDLE_T model_timer_ghdl;     /* Model Timer group handle */
QmeshAppCallback QmeshAppMsgHandler;

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshInitModelCommon
 *
 *  DESCRIPTION
 *      Application calls this API to registers its model messages callback Handler.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
extern void QmeshInitModelCommon (QmeshAppCallback app_model_msg_handler,
                                  const QMESH_DEVICE_COMPOSITION_DATA_T* server_device_composition)
{
    server_composition = server_device_composition;
    QmeshAppMsgHandler= app_model_msg_handler;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGetRemainingTime
 *
 *  DESCRIPTION
 *      This API calcuates the time required to reach the target time ('trgtTime') from this moment.
 *      The API returns the remaining time in 'Generic Default Transition Time State Format' as mentioned in the
 *   Model specification d09r11 (section 3.1.3).
 *
 *  RETURNS/MODIFIES
 *      Remaining Time in uint8_t
 *
 *----------------------------------------------------------------------------*/
extern uint8_t QmeshGetRemainingTime (uint32_t target_time)
{
    uint16_t cur_time_us[3];
    uint32_t cur_time_ms;
    uint32_t time_diff;
    uint8_t rem_time = 0, num_steps;

    if (target_time == UNKNOWN_TARGET_TIME)
        return QMESH_MODEL_UNKNOWN_TRANSITION_TIME;

    if (target_time == 0)
        return 0;

    /* Get the current time in ms */
    QmeshGetCurrentTimeInMs (cur_time_us);
    cur_time_ms = ((uint32_t)cur_time_us[1] << 16) | (uint32_t) (cur_time_us[0]);

    /* Calculate remaining time */
    if (target_time > cur_time_ms)
    {
        time_diff = target_time - cur_time_ms;
    }
    else  /* Corner case */
    {
        time_diff = 100;
    }

    /* Format the time_diff as per Spec into 'Generic Default Transition Time State Format' and return */
    if (time_diff <= QMESH_100_MILLI_SEC_MAX_TIME)
    {
        num_steps = (uint8_t) (time_diff / QMESH_100_MILLI_SEC);
        rem_time =  ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_100_MSEC << 6) | num_steps;
    }
    else if (time_diff <= QMESH_SEC_MAX_TIME)
    {
        num_steps = (uint8_t) (time_diff / QMESH_SEC);
        rem_time =  ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_1_SEC << 6) | num_steps;
    }
    else if (time_diff <= QMESH_10_SEC_MAX_TIME)
    {
        num_steps = (uint8_t) (time_diff / QMESH_10_SEC);
        rem_time =  ((uint8_t) QMESH_MODEL_STEP_RESOLUTION_10_SEC << 6) | num_steps;
    }
    else if (time_diff <= QMESH_10_MINUTES_MAX_TIME)
    {
        num_steps = (uint8_t) (time_diff / QMESH_10_MINUTES);
        rem_time =  ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_MINUTES << 6) | num_steps;
    }

    return rem_time;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshConvertTimeToTransitionTimeFormat
 *
 *  DESCRIPTION
 *      The API returns the remaining time in
 *      'Generic Default Transition Time State Format' as mentioned in the
 *       Model specification.
 *
 *  RETURNS/MODIFIES
 *      Remaining Time in uint8_t
 *
 *----------------------------------------------------------------------------*/
extern uint8_t QmeshConvertTimeToTransitionTimeFormat (uint32_t remaining_time)
{
    uint8_t rem_time = 0, num_steps;

    /* Format the time_diff as per Spec into 'Generic Default Transition Time
     * State Format' and return
     */
    if (remaining_time <= QMESH_100_MILLI_SEC_MAX_TIME)
    {
        num_steps = (uint8_t) (remaining_time / QMESH_100_MILLI_SEC);
        rem_time =  ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_100_MSEC << 6) | num_steps;
    }
    else if (remaining_time <= QMESH_SEC_MAX_TIME)
    {
        num_steps = (uint8_t) (remaining_time / QMESH_SEC);
        rem_time =  ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_1_SEC << 6) | num_steps;
    }
    else if (remaining_time <= QMESH_10_SEC_MAX_TIME)
    {
        num_steps = (uint8_t) (remaining_time / QMESH_10_SEC);
        rem_time =  ((uint8_t) QMESH_MODEL_STEP_RESOLUTION_10_SEC << 6) | num_steps;
    }
    else if (remaining_time <= QMESH_10_MINUTES_MAX_TIME)
    {
        num_steps = (uint8_t) (remaining_time / QMESH_10_MINUTES);
        rem_time =  ((uint8_t)QMESH_MODEL_STEP_RESOLUTION_10_MINUTES << 6) | num_steps;
    }

    return rem_time;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGetStepResolutionInMilliSec
 *
 *  DESCRIPTION
 *      This API returns the step resolution in milliseconds
 *
 *  RETURNS/MODIFIES
 *      Step resolution in milliseconds
 *
 *----------------------------------------------------------------------------*/
uint32_t QmeshGetStepResolutionInMilliSec (uint8_t step_resolution)
{
    uint32_t step_resoln_ms = 0;

    if (step_resolution == QMESH_MODEL_STEP_RESOLUTION_100_MSEC)
        step_resoln_ms = QMESH_100_MILLI_SEC;
    else if (step_resolution == QMESH_MODEL_STEP_RESOLUTION_1_SEC)
        step_resoln_ms = QMESH_SEC;
    else if (step_resolution == QMESH_MODEL_STEP_RESOLUTION_10_SEC)
        step_resoln_ms = QMESH_10_SEC;
    else if (step_resolution == QMESH_MODEL_STEP_RESOLUTION_10_MINUTES)
        step_resoln_ms = QMESH_10_MINUTES;

    return step_resoln_ms;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshTransitionTimeToMilliSec
 *
 *  DESCRIPTION
 *      This API converts the time mentioned in the 'Generic Default Transition Time State Format' as mentioned in the
 *   Model specification d09r11 (section 3.1.3) to milli seconds.
 *
 *  RETURNS/MODIFIES
 *      Time in milliseconds.
 *
 *----------------------------------------------------------------------------*/
uint32_t QmeshTransitionTimeToMilliSec (uint8_t transition_time)
{
    uint32_t milli_sec = 0;
    uint8_t num_steps = 0;
    uint8_t step_resoln = 0;

    if (transition_time == 0)
        return milli_sec;

    num_steps = transition_time &
                QMESH_MODEL_TRANSITION_NUM_OF_TRANSITION_STEPS_BITMASK; /* Number of steps in 'Transition Time' */
    step_resoln = (transition_time & QMESH_MODEL_TRANSITION_STEP_RESOLUTION_BITMASK)
                  >> 6; /* Step Resolution */

    /* Unknown or not determined */
    if (num_steps == QMESH_MODEL_INVALID_TRANSITION_TIME)
    {
#ifdef QMESH_GEN_ONOFF_DEBUG
        DEBUGF (g_debugHdl, "%s %d Number of steps is invalid, transition_time=0x%x\n",
                __FUNCTION__, __LINE__, transition_time);
#endif
        return milli_sec;
    }

    /* convert 'transition' time into 'millisec' */
    milli_sec = (num_steps * QmeshGetStepResolutionInMilliSec (step_resoln));
#ifdef QMESH_GEN_ONOFF_DEBUG
    DEBUGF (g_debugHdl, "%s %d Transition time in  msec=0x%08X \n", __FUNCTION__,
            __LINE__, milli_sec);
#endif
    return milli_sec;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGetModelData
 *
 *  DESCRIPTION
 *      This function returns the model data for a given element
 *
 *  RETURNS/MODIFIES
*       Model Context
*
*-----------------------------------------------------------------------------*/
extern QMESH_MODEL_DATA_T *QmeshGetModelData (const QMESH_ELEMENT_CONFIG_T *elem_config,
                                              uint32_t model_id)
{
    uint8_t mdl_idx;

    if (elem_config)
    {
        for (mdl_idx = 0; mdl_idx < elem_config->num_btsig_models; mdl_idx++ )
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,"Sig Model %d\n",mdl_idx);
            if (elem_config->btsig_model_ids[mdl_idx] == model_id)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,"Found Model ID in QmeshGetModelData\n");
         
                /* Retrieve the model data and return */
                return &elem_config->element_data->model_data[mdl_idx];
            }
        }
    }

    return NULL;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGetDTTFromPrimElement
 *
 *  DESCRIPTION
 *      This function returns the default transition time from primary element
 *
 *  RETURNS/MODIFIES
 *       Default transition time
 *
 *----------------------------------------------------------------------------*/
extern uint8_t QmeshGetDTTFromPrimElement ()
{
    /* Assuming primary elements resides in Index 0 */
    return QmeshGetGenericDTT(&server_composition->elements[0]);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGetGenericDTT
 *
 *  DESCRIPTION
 *      This function returns the default transition time
 *
 *  RETURNS/MODIFIES
 *       Default transition time
 *
 *----------------------------------------------------------------------------*/
extern uint8_t QmeshGetGenericDTT (const QMESH_ELEMENT_CONFIG_T
                                 *elem_data)
{
    uint8_t transition_time = 0;

    /* Get dtt model data */
    QMESH_MODEL_DATA_T *mdl_data  = QmeshGetModelData(elem_data,
                                           QMESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME);

    if (mdl_data != NULL)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                          "Got DTT Context data\n");
                          
        return QmeshGenericDTTGetTransitionTime (mdl_data->model_priv_data);
    }
    else
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_GDTT_SERVER,
                          "Null dtt context data\n");
    }

    return transition_time;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGetModelContextFromComp
 *
 *  DESCRIPTION
 *      This function returns the model context from provided element address
 *      and opcode.
 *  RETURNS/MODIFIES
 *       model context
 *
 *----------------------------------------------------------------------------*/
extern void* QmeshGetModelContextFromComp (uint16_t elem_addr, uint16_t model_id)
{
    uint8_t mdl_idx, elm_idx;
    
    if(server_composition == NULL)
        return NULL;
    
    const QMESH_ELEMENT_CONFIG_T *const elem_config = server_composition->elements;

    for (elm_idx = 0; elm_idx < server_composition->num_elements; elm_idx++)
    {
        if (elem_config[elm_idx].element_data->unicast_addr == elem_addr)
        {
            for (mdl_idx = 0; mdl_idx < elem_config[elm_idx].num_btsig_models; mdl_idx++ )
            {
                if (elem_config[elm_idx].btsig_model_ids[mdl_idx] == model_id)
                {
                    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                      "QmeshGetModelContextFromComp: Got HSL Context \n");
                    /* Retrieve the model data and return */
                    return elem_config[elm_idx].element_data->model_data[mdl_idx].model_priv_data;
                }
            }
        }
    }
    return NULL;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshUpdateBoundStates
 *
 *  DESCRIPTION
 *      This API handles all the bound states operation for models.
 *
 *  RETURNS/MODIFIES
 *       TRUE if handled.
 *
 *----------------------------------------------------------------------------*/
extern bool QmeshUpdateBoundStates (uint16_t elem_addr,
                                    uint16_t app_opcode,
                                    void *data)
{
    QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *onoff_context = NULL;
    QMESH_GENERIC_LEVEL_CONTEXT_T *lvl_context = NULL;
#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE            
    QMESH_GENERIC_POWERLEVEL_CONTEXT_T *pwr_lvl_context = NULL;
#endif    
    QMESH_LIGHTNESS_MODEL_CONTEXT_T *lightness_context = NULL;
    QMESH_LIGHT_HSL_MODEL_CONTEXT_T *hsl_context = NULL;
    QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T *hue_context = NULL;
    QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *sat_context = NULL;
    
                                   
    /* Update relevant bound states based on model opcode */
    switch (app_opcode)
    {
        case QAPP_GET_LVL_RANGE:
        {
            QAPP_GET_LVL_RANGE_T *range = (QAPP_GET_LVL_RANGE_T *) data;

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
            pwr_lvl_context = QmeshGetModelContextFromComp(elem_addr,
                                                           QMESH_MODEL_GENERIC_POWERLEVEL_SERVER);
            if(range)
            {
                range->max_val = QmeshGenericPowerLevelMaxRangeGet(pwr_lvl_context);
                range->min_val = QmeshGenericPowerLevelMinRangeGet(pwr_lvl_context);
            }
            break;
#endif
            if(range)
            {
              range->max_val = MAX_LEVEL;
              range->min_val = MIN_LEVEL;
            }
        }
        break;

        case  QAPP_GEN_ONOFF_TRANS_BEGIN:
        {
            lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_LEVEL);
            lightness_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_LIGHTNESS);
            hsl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_HSL_SERVER);
            hue_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_HSL_HUE_SERVER);
            sat_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_HSL_SATURATION_SERVER);
            
            /* Abort Level Transition */
            if (lvl_context != NULL)
            {
                QmeshGenericLevelAbortTransition (lvl_context);
            }
            
            /* Abort Lightness Transition */
            if(lightness_context)
                QmeshLightnessAbortTransition (lightness_context);
            
            /* Abort Light HSL Transition */
            if(hsl_context)
                QmeshLightHSLAbortTransition (hsl_context);
            
            /* Abort Lightness Hue Transition */
            if(hue_context)
                QmeshLightHSLHueAbortTransition (hue_context);
            
            /* Abort Lightness Sat Transition */
            if(sat_context)
                QmeshLightHSLSatAbortTransition (sat_context);

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE

            pwr_lvl_context = QmeshGetModelContextFromComp(elem_addr,
                                                           QMESH_MODEL_GENERIC_POWERLEVEL_SERVER);
                                                           
            /* Abort PowerLevel Transitions */
            if(pwr_lvl_context)
                QmeshGenericPowerLevelAbortTransition(pwr_lvl_context);
#endif
        }
        break;

        case  QAPP_GEN_ONOFF_UPDATED:
        {
            /* Generic On/Off state updated. Extract the Generic On/Offstate value */
            uint16_t onoff = (* (uint16_t *)data);

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
            uint16_t pwr_lvl_act = 0;
            pwr_lvl_context = QmeshGetModelContextFromComp(elem_addr,
                                                           QMESH_MODEL_GENERIC_POWERLEVEL_SERVER);
            lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_LEVEL);
            
            if(onoff == 0)
            {
                pwr_lvl_act = 0;
                QmeshGenericPowerLevelActualUpdate(pwr_lvl_context, pwr_lvl_act);
            }
            else
            {
                uint16_t pwr_lvl_def = QmeshGenericPowerLevelDefaultGet(pwr_lvl_context);

                if(pwr_lvl_def == 0)
                {
                    pwr_lvl_act = QmeshGenericPowerLevelLastGet(pwr_lvl_context);
                }
                else
                {
                    pwr_lvl_act = pwr_lvl_def;
                }
            }

            /* Update Power Level Acutal */
            if(pwr_lvl_context)
               QmeshGenericPowerLevelActualUpdate(pwr_lvl_context, pwr_lvl_act);
            
            /* Update Level */
            if(lvl_context)
               QmeshGenericLevelUpdate(lvl_context, (pwr_lvl_act - 32768));
#else
            /* Update lightness actual as this is bounded to Generic On/Off */
            uint16_t light_act = 0;
            lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_LEVEL);

            lightness_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_LIGHTNESS);

            /* If Generic On/Off is Zero, set lightness actual to zero */
            if (onoff == 0)
            {
                light_act = 0;
            }
            else
            {
                if(lightness_context)
                {
                    /* Generic on/off is non-zero. Get the lightness default */
                    uint16_t light_def = QmeshLightnessDefaultGet (lightness_context);

                    /* If lightness default is zero, get the lightness last value */
                    if (light_def == 0)
                    {
                        /* For lightness default equal to zero, assign lightness last */
                        light_act = QmeshLightnessLastGet (lightness_context);
                    }
                    else
                    {
                        /* Else assign the lightness default value */
                        light_act = light_def;
                    }
                }
            }

            /* Update lightness actual */
            if(lightness_context)
               QmeshLightnessActualUpdate (lightness_context, light_act);

            /* Update Lightness Level */
            if (lvl_context)
            {
                QmeshGenericLevelUpdate (lvl_context, (light_act - 32768));
            }
            
            if(QmeshAppMsgHandler)
                QmeshAppMsgHandler(elem_addr,app_opcode,data);
#endif
        }
        break;

        case QAPP_LVL_UPDATED:
        {
            /* Get the level data */
            int16_t lvl = (* (int16_t *)data);

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
            
            onoff_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_ONOFF);
            pwr_lvl_context = QmeshGetModelContextFromComp(elem_addr,
                                                           QMESH_MODEL_GENERIC_POWERLEVEL_SERVER);
           /* Update PowerLevel actual */
           if(pwr_lvl_context)
               QmeshGenericPowerLevelActualUpdate(pwr_lvl_context, lvl + 32768);

            /* Update Generic OnOff */
            if(onoff_context)
               QmeshGenericOnOffUpdate(onoff_context, lvl + 32768 > 0? 1 : 0);
#else
            lightness_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_LIGHTNESS);
            hue_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_HSL_HUE_SERVER);
            sat_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_HSL_SATURATION_SERVER);
            onoff_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_ONOFF);
            hsl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_HSL_SERVER);
            
            /* Update lightness actual */
            if(lightness_context)
               QmeshLightnessActualUpdate (lightness_context, lvl + 32768);
            
            /* Update HSL Lightness */
            if(hsl_context)
                QmeshLightHslUpdate(hsl_context,lvl + 32768);

            /* Update Generic OnOff */
            if(onoff_context)
               QmeshGenericOnOffUpdate(onoff_context, lvl + 32768 > 0? 1 : 0);

            /* Update Light Hue */
            if (hue_context)
                QmeshLightHslHueUpdate (hue_context, lvl + 32768);

            /* Update Light Sat */
            if (sat_context)
                QmeshLightHslSatUpdate (sat_context, lvl + 32768);

            if(QmeshAppMsgHandler)
                QmeshAppMsgHandler(elem_addr,app_opcode,data);
#endif
        }
        break;

        case QAPP_LVL_TRANS_BEGIN:
        {
#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
            onoff_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_ONOFF);
            pwr_lvl_context = QmeshGetModelContextFromComp(elem_addr,
                                                           QMESH_MODEL_GENERIC_POWERLEVEL_SERVER);
           /* Cancel any transitions in OnOff server */
           if(onoff_context)
             QmeshGenericOnOffAbortTransition(onoff_context);
           
           /* Cancel any transitions in Power Level server */
           if(pwr_lvl_context)
              QmeshGenericPowerLevelAbortTransition(pwr_lvl_context);
#else
            onoff_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_ONOFF);
            lightness_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_LIGHTNESS);
            
           /* Cancel any transitions in OnOff server */
           if(onoff_context)
              QmeshGenericOnOffAbortTransition(onoff_context);
          
           /* Cancel any transitions on lightness server */
           if(lightness_context)
              QmeshLightnessAbortTransition (lightness_context);
#endif
        }
        break;

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
        case QAPP_PWR_LVL_ACTUAL_TRANS_BEGIN:
        {
            lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_LEVEL);
            onoff_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_ONOFF);  

            /* Cancel Ongoing bound states transition */
            if(onoff_context)
              QmeshGenericOnOffAbortTransition(onoff_context);
            
            if(lvl_context)            
              QmeshGenericLevelAbortTransition(lvl_context);
        }
        break;

        case QAPP_PWR_LVL_ACTUAL_UPDATED:
        {
            uint16_t lvl = (*(uint16_t*)data);
            lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_LEVEL);
            onoff_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_ONOFF);  

            if(onoff_context)            
              QmeshGenericOnOffUpdate(onoff_context, (lvl > 0)? 1 : 0);
            if(lvl_context)
              QmeshGenericLevelUpdate(lvl_context, (int16_t)(lvl - 32768));
        }
        break;
#endif

        case QAPP_LIGHTNESS_ACTUAL_UPDATED:
        {
            /*  Retrieve the lightness actual */
            uint16_t light_actual = (* (uint16_t *)data);
            lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_LEVEL);
            onoff_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_ONOFF);
            hsl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_HSL_SERVER);

            /* Update Generic level */
            if (lvl_context)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "QUB: QAPP_LIGHTNESS_ACTUAL_UPDATED: Got Level Context \n");
                QmeshGenericLevelUpdate (lvl_context, (light_actual - 32768));
            }

            /* Update Generic On/Off */
            if(onoff_context)
              QmeshGenericOnOffUpdate (onoff_context, (light_actual == 0) ? 0 : 1);
            
            /* Update Lightness HSL */
            if(hsl_context)
              QmeshLightHslUpdate (hsl_context, light_actual);
          
            if(QmeshAppMsgHandler)
                QmeshAppMsgHandler(elem_addr,app_opcode,data);          
        }
        break;

        case QAPP_LIGHTNESS_ACTUAL_TRANS_BEGIN:
        {
            lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_LEVEL);
            if (lvl_context != NULL)
            {
               /* Cancel Ongoing bound states transition */
               QmeshGenericLevelAbortTransition (lvl_context);    
            }
        }
        break;

        case QAPP_ONPOWERUP_UPDATED:
        {
            if(QmeshAppMsgHandler)
                QmeshAppMsgHandler(elem_addr,app_opcode,data);
        }
        break;

        case QAPP_ONPOWERUP_BOOTVALUE:
        {
            /* Generic PowerOn/Off OnPowerUp state during boot time is received */
            uint16_t onpowerup = (* (uint16_t *)data);
            
            lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_LEVEL);
            onoff_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_ONOFF);
                                    
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "QUBS: QAPP_ONPOWERUP_BOOTVALUE \n");

            /* Update Generic on/off state based on OnPowerup State */
            if(onpowerup == 2)
            {
                if(onoff_context)
                {
                    if(onoff_context->onoff_state.cur_onoff != 
                       onoff_context->onoff_state.trgt_onoff)
                    {
                        /* If transition was in progress during reset, cur state should be previous target state */
                        QmeshGenericOnOffUpdate(onoff_context, onoff_context->onoff_state.trgt_onoff);
                    }
                }
            }
            else
            {
                if(onoff_context)
                    QmeshGenericOnOffUpdate (onoff_context, (onpowerup == 0) ? 0 : 1);
            }
            
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "QUBS: OnOff Updated \n");

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
            uint16_t pwr_lvl_act = 0;
            pwr_lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_POWERLEVEL_SERVER);

            if(pwr_lvl_context && lvl_context)
            {                
                if(onpowerup == 0)
                {
                    pwr_lvl_act = 0;
                }
                else if(onpowerup == 1)
                {
                    uint16_t pwr_lvl_def = QmeshGenericPowerLevelDefaultGet(pwr_lvl_context);

                    if(pwr_lvl_def == 0)
                    {
                        pwr_lvl_act = QmeshGenericPowerLevelLastGet(pwr_lvl_context);
                    }
                    else
                    {
                        pwr_lvl_act = pwr_lvl_def;
                    }
                }
                else
                {
                    /* If transition was in progress during reset */
                    if(pwr_lvl_context->pwr_actual_state.current_pwr_actual !=
                        pwr_lvl_context->pwr_actual_state.trgt_pwr_actual)
                    {
                        pwr_lvl_act = pwr_lvl_context->pwr_actual_state.trgt_pwr_actual;
                    }
                    else
                    {
                        /* Last known value before reboot */
                        pwr_lvl_act = pwr_lvl_context->pwr_actual_state.current_pwr_actual;
                    }
                }

                /* Update PowerLevel state based on OnPowerup State */
                QmeshGenericPowerLevelActualUpdate(pwr_lvl_context, pwr_lvl_act);
              
                /* Update PowerLevel state based on OnPowerup State */
                QmeshGenericLevelUpdate(lvl_context, (pwr_lvl_act - 32768));
            }
#else
            lightness_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_LIGHTNESS);
            hsl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_HSL_SERVER);
                        
            /* Update lightness actual state based on OnPowerup State */
            uint16_t light_act = 0;
    
            /* If OnPowerUp State is zero, lightness actual should be zero */
            if (onpowerup == 0)
            {
                if(hsl_context)
                {
                  QmeshLightHslUpdate(hsl_context,light_act);
             
                
                  QmeshLightHslHueUpdate (hsl_context->hue_context,
                                        hsl_context->light_hsl_hue_def);
                                        
                  QmeshLightHslSatUpdate (hsl_context->sat_context,hsl_context->light_hsl_sat_def);                                       
                }
                
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "QUBS: HSl Updated \n");
            }
            else if (onpowerup == 1)
            {
                if(lightness_context)
                {
                    /* Get lightness default */
                    uint16_t light_def = QmeshLightnessDefaultGet (lightness_context);

                    if (light_def == 0)
                    {
                        /* Assign lightness last value */
                        light_act = QmeshLightnessLastGet (lightness_context);
                    }
                    else
                    {
                        light_act = light_def;
                    }
                    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "QUBS: lightness context Updated \n");
                }

                if(hsl_context)
                {
                  QmeshLightHslUpdate(hsl_context,light_act);
              
                  QmeshLightHslHueUpdate(hsl_context->hue_context,hsl_context->light_hsl_hue_def);
              
                  QmeshLightHslSatUpdate(hsl_context->sat_context,hsl_context->light_hsl_sat_def);     
                }
                
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "QUBS: onpwrup1 hsl Updated \n");
            }
            else if (onpowerup == 2)
            {
                /* Get the lightness last value */
                if(lightness_context)
                    light_act = QmeshLightnessLastGet (lightness_context);
                
                /* For Lightness ,Hue and Sat it's the last known value */               
            }

            /* If Lightness or HSL were in the middle of transition, restore the
             * target value.
             */
            if(lightness_context && lightness_context->lightness_target!=0)
            {
                 DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "QUBS: lightness_context tgt Updated \n");
                light_act = lightness_context->lightness_target;
                lightness_context->lightness_target = 0;
            }
                
            if(hsl_context && hsl_context->light_target_hsl !=0)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "QUBS: HSL ctee Updated \n");
                QmeshLightHslUpdate(hsl_context,hsl_context->light_target_hsl);
            }
                
            if(hsl_context && hsl_context->hue_context)
            {
                if( hsl_context->hue_context->light_target_hue !=0)
                {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "QUBS: hue_context ttt Updated \n");
                QmeshLightHslHueUpdate(hsl_context->hue_context,
                                       hsl_context->hue_context->light_target_hue);
                }
            }

            if(hsl_context && hsl_context->sat_context)
            {
               if(hsl_context->sat_context->light_target_sat!=0)
               {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "QUBS: sat_context ttt Updated \n");
                QmeshLightHslSatUpdate(hsl_context->sat_context,
                                       hsl_context->sat_context->light_target_sat); 
               }
                           
            }                
  
            /* Update lightness actual */
            if(lightness_context)
            {
                      DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "QUBS: lightness_context root Update \n");
                QmeshLightnessActualUpdate (lightness_context, light_act);
                if(lvl_context)                
                   QmeshGenericLevelUpdate (lvl_context, (light_act - 32768));
                
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "QUBS: lightness_context root Updated \n");
            }
            
            /* Update for the Hue Element */
            if(hue_context)
            {
               lvl_context = QmeshGetModelContextFromComp(hue_context->elm_id,QMESH_MODEL_GENERIC_LEVEL); 
               DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "QUBS: hue_context Updated \n");
               QmeshGenericLevelUpdate (lvl_context, (hue_context->light_hsl_hue - 32768));
      
            }
            
            /* Update for the Saturation Element */
            if(sat_context)
            {
               lvl_context = QmeshGetModelContextFromComp(sat_context->elm_id,QMESH_MODEL_GENERIC_LEVEL);             
               DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "QUBS: sat_context Updated \n");
               QmeshGenericLevelUpdate (lvl_context, (sat_context->light_hsl_sat - 32768));
            }

#endif
        }
        break;

        case QAPP_LIGHT_HSL_UPDATED:
        {
            /*  Retrieve the light HSL value */
            uint16_t light_hsl = (* (uint16_t *)data);
            
            lightness_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_LIGHTNESS);
            lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_LEVEL);
            onoff_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_ONOFF);
            
            /* Update lightness actual */
            if(lightness_context)
               QmeshLightnessActualUpdate (lightness_context, light_hsl);
            
             /* Update Generic level */
            if (lvl_context)
                QmeshGenericLevelUpdate (lvl_context, (light_hsl - 32768));

            /* Update Generic On/Off */
            if(onoff_context)
              QmeshGenericOnOffUpdate (onoff_context, (light_hsl == 0) ? 0 : 1);
                
            if(QmeshAppMsgHandler)
                QmeshAppMsgHandler(elem_addr,app_opcode,data);
        }
        break;

        case QAPP_LIGHT_VALIDATE_HUE:
        {
            /*  Retrieve the light HSL Hue value */
            uint16_t light_hue = * (uint16_t *)data;
            
            hue_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_HSL_HUE_SERVER);
            
            if(hue_context)
              hsl_context = (QMESH_LIGHT_HSL_MODEL_CONTEXT_T*)hue_context->hsl_context;            
            
            if(hsl_context == NULL || hue_context == NULL)
            {
                 DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "Hue updated hsl context null \n");
                                  return FALSE;
            }
            
            /* If both min and max are zero, range is not set by anyone. Skip validating */
            if ( (hsl_context->light_hsl_hue_min == 0) && \
                 (hsl_context->light_hsl_hue_max == 0))
                return TRUE;

            /* Check if they fall within the range */
            if (light_hue < hsl_context->light_hsl_hue_min)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "hsl HUE MIN %d \n",hsl_context->light_hsl_hue_min);
                               
                * (uint16_t *)data = hsl_context->light_hsl_hue_min;
            }
            else if (light_hue > hsl_context->light_hsl_hue_max)
            {
                 DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "hsl HUE max %d \n",hsl_context->light_hsl_hue_max);
                * (uint16_t *)data = hsl_context->light_hsl_hue_max;
            }
        }
        break;

        case QAPP_LIGHT_VALIDATE_SAT:
        {
            /*  Retrieve the light HSL Sat value */
            uint16_t light_sat = * (uint16_t *)data;
            
            sat_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_HSL_SATURATION_SERVER);
            
            if(sat_context)
               hsl_context = (QMESH_LIGHT_HSL_MODEL_CONTEXT_T*)sat_context->hsl_context;
            
            if(hsl_context == NULL || sat_context == NULL)
            {
                 DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                                  "Hue updated hsl context null \n");
                                  return FALSE;
            }
            
            /* If both min and max are zero, range is not set by anyone. Skip validating */
            if ( (hsl_context->light_hsl_sat_min == 0) && \
                 (hsl_context->light_hsl_sat_max == 0))
                return TRUE;

            /* Check if they fall within the range */
            if (light_sat < hsl_context->light_hsl_sat_min)
            {
                * (uint16_t *)data = hsl_context->light_hsl_sat_min;
            }
            else if (light_sat > hsl_context->light_hsl_sat_max)
            {
                * (uint16_t *)data = hsl_context->light_hsl_sat_max;
            }
        }
        break;

        case QAPP_LIGHT_HUE_UPDATED:
        {
            /*  Retrieve the light HSL value */
            uint16_t light_hue = (* (uint16_t *)data);
            lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_LEVEL);
            
            /* Update Level */
            if (lvl_context != NULL)
            {
                QmeshGenericLevelUpdate (lvl_context, (light_hue - 32768));
            }
            
             if(QmeshAppMsgHandler)
                QmeshAppMsgHandler(elem_addr,app_opcode,data);
        }
        break;

        case QAPP_LIGHT_SAT_UPDATED:
        {
            /*  Retrieve the light HSL value */
            uint16_t light_sat = (* (uint16_t *)data);
            lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_LEVEL);
            
            /* Update Level */
            if (lvl_context != NULL)
            {
                /* Update Level */
                QmeshGenericLevelUpdate (lvl_context, (light_sat - 32768));
            }
            
             if(QmeshAppMsgHandler)
                QmeshAppMsgHandler(elem_addr,app_opcode,data);
        }
        break;

        case QAPP_LIGHT_HSL_UPDATE_LIGHT_DEFAULT:
        {
            uint16_t light_def = (* (uint16_t *)data);
            lightness_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_LIGHTNESS);
            
            if(lightness_context)
              QmeshLightnessDefaultUpdate (lightness_context, light_def);
        }
        break;

        case QAPP_LIGHT_HSL_GET_LIGHT_DEFAULT:
        {
            /*  Retrieve the light HSL value */
            uint16_t *light_def = (uint16_t *)data;
            lightness_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_LIGHTNESS);
            
            if(lightness_context)
                *light_def = QmeshLightnessDefaultGet (lightness_context);
        }
        break;

        case QAPP_LIGHT_HSL_TRANS_BEGIN:
        {
            lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_LEVEL);
            onoff_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_ONOFF);
            lightness_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_LIGHT_LIGHTNESS);
                        
            /* Abort onoff Transition */
            if(onoff_context)
              QmeshGenericOnOffAbortTransition (onoff_context);

            /* Abort Level Transition */
            if (lvl_context)
              QmeshGenericLevelAbortTransition (lvl_context);

            /* Abort Lightness Transition */
            if(lightness_context)
              QmeshLightnessAbortTransition (lightness_context);
        }
        break;

        case QAPP_LIGHT_HSL_HUE_TRANS_BEGIN:
        {
            lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_LEVEL);

            /* Abort Level Transition */
            if (lvl_context)
                QmeshGenericLevelAbortTransition (lvl_context);
        }
        break;

        case QAPP_LIGHT_HSL_SAT_TRANS_BEGIN:
        {
            lvl_context = QmeshGetModelContextFromComp(elem_addr,QMESH_MODEL_GENERIC_LEVEL);
                                    
            /* Abort Level Transition */
            if (lvl_context)
                QmeshGenericLevelAbortTransition (lvl_context);
        }
        break;

        default:
            break;
    }    

    return TRUE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      updateElementAddrForModel
 *
 *  DESCRIPTION
 *      This function updates the element id in the model private data with 
 *      unicast address. 
 *  
 *  NOTE
 *      This function updates the following models
 *      1. Generic On Off Server
 *      2. Generic Level Server
 *      3. Power on Off Server
 *      4. Power Level Server
 *      5. Light Lightness Server
 *      6. Light HSL Server
 *      7. Light HSL Hue Server
 *      8. Light HSL Sat Server
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
static void updateElementAddrForModel(uint16_t unicast_addr, const uint16_t model_id, void *model_priv_data)
{
    if(model_priv_data != NULL)
    {
        switch(model_id)
        {
            case QMESH_MODEL_GENERIC_ONOFF:
                ((QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T *)model_priv_data)->elm_addr = unicast_addr;
                break;
            case QMESH_MODEL_GENERIC_LEVEL:
                ((QMESH_GENERIC_LEVEL_CONTEXT_T *)model_priv_data)->elm_addr = unicast_addr;
                break;
            case QMESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME:
                ((QMESH_GENERIC_DTT_CONTEXT_T *)model_priv_data)->elm_addr = unicast_addr;
                break;
            case QMESH_MODEL_GENERIC_POWERONOFF:
                ((QMESH_GEN_POWERONOFF_DATA_T *)model_priv_data)->elm_addr = unicast_addr;
                break;
            case QMESH_MODEL_GENERIC_POWERONOFFSETUPSERVER:
                ((QMESH_GEN_POWERONOFF_DATA_T *)model_priv_data)->elm_addr = unicast_addr;
                break;
#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
            case QMESH_MODEL_GENERIC_POWERLEVEL_SERVER:
                ((QMESH_GENERIC_POWERLEVEL_CONTEXT_T *)model_priv_data)->elm_addr = unicast_addr;
                break;
            case QMESH_MODEL_GENERIC_POWERLEVEL_SETUP_SERVER:
                ((QMESH_GENERIC_POWERLEVEL_CONTEXT_T *)model_priv_data)->elm_addr = unicast_addr;
                break;
#endif
            case QMESH_MODEL_LIGHT_LIGHTNESS:
                ((QMESH_LIGHTNESS_MODEL_CONTEXT_T *)model_priv_data)->elm_id = unicast_addr;
                break;
            case QMESH_MODEL_LIGHT_LIGHTNESS_SETUP_SERVER:
                ((QMESH_LIGHTNESS_MODEL_CONTEXT_T *)model_priv_data)->elm_id = unicast_addr;
                break;
            case QMESH_MODEL_LIGHT_HSL_SERVER:
                ((QMESH_LIGHT_HSL_MODEL_CONTEXT_T *)model_priv_data)->elm_id = unicast_addr;
                break;
            case QMESH_MODEL_LIGHT_HSL_HUE_SERVER:
                ((QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T *)model_priv_data)->elm_id = unicast_addr;
                break;
            case QMESH_MODEL_LIGHT_HSL_SATURATION_SERVER:
                ((QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *)model_priv_data)->elm_id = unicast_addr;
                break;
            case QMESH_MODEL_LIGHT_HSL_SETUP_SERVER:
                ((QMESH_LIGHT_HSL_MODEL_CONTEXT_T *)model_priv_data)->elm_id = unicast_addr;
                break;
            default:
                break;
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshUpdateModelWithUnicastAddr
 *
 *  DESCRIPTION
 *      This function updates the model with unicast address for the correspond elements.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
void QmeshUpdateModelWithUnicastAddr(void)
{
    uint8_t i, j;
    const QMESH_ELEMENT_CONFIG_T *elem_config;
    void *model_priv_data;
    if(server_composition != NULL)
    {
        for(i = 0; i < server_composition->num_elements; i++)
        {
            elem_config = &server_composition->elements[i];
            if(elem_config->element_data->unicast_addr != QMESH_UNASSIGNED_ADDRESS)
            {
                for(j = 0; j < elem_config->num_btsig_models; j++)
                {
                    model_priv_data = QmeshGetModelData(elem_config, 
                                                        elem_config->btsig_model_ids[j])->model_priv_data;
                    updateElementAddrForModel(elem_config->element_data->unicast_addr, 
                                              elem_config->btsig_model_ids[j], 
                                              model_priv_data);
                }
            }
        }
    }
}
