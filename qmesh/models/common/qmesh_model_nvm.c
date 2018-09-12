
#include "qmesh_model_nvm.h"
#include "qmesh_model_common.h"
#include "qmesh_model_debug.h"



const QMESH_DEVICE_COMPOSITION_DATA_T *nvm_server_composition;

const QMESH_PS_KEY_INFO_T qmesh_model_ps_keys[QMESH_MODEL_PS_KEY_LIST_SIZE] =
{
    {
        QMESH_PS_DATA_SINGLE_FIXED,
        QMESH_PS_KEY_MODEL_GENERIC_ONOFF,
        sizeof(QMESH_MODEL_GEN_ONOFF_NVM_STATE_T),
        1
    },

    {
        QMESH_PS_DATA_SINGLE_FIXED,
        QMESH_PS_KEY_MODEL_GENERIC_LEVEL,
        sizeof(QMESH_MODEL_GEN_LEVEL_NVM_STATE_T) * QMESH_MAX_GENERIC_LEVEL_INSTANCES,
        1
    },

    {
        QMESH_PS_DATA_SINGLE_FIXED,
        QMESH_PS_KEY_MODEL_GENERIC_POWER_ONOFF,
        sizeof(QMESH_MODEL_GEN_PWR_ONOFF_NVM_STATE_T),
        1
    },

    {
        QMESH_PS_DATA_SINGLE_FIXED,
        QMESH_PS_KEY_MODEL_GENERIC_DTT,
        sizeof(QMESH_MODEL_GEN_DTT_NVM_STATE_T),
        1
    },
    
    {
        QMESH_PS_DATA_SINGLE_FIXED,
        QMESH_PS_KEY_MODEL_LIGHTNESS,
        sizeof(QMESH_MODEL_LIGHTNESS_NVM_STATE_T),
        1
    },
    
    {
        QMESH_PS_DATA_SINGLE_FIXED,
        QMESH_PS_KEY_MODEL_LIGHT_HSL,
        sizeof(QMESH_MODEL_LIGHT_HSL_NVM_STATE_T),
        1
    },
    
    {
        QMESH_PS_DATA_SINGLE_FIXED,
        QMESH_PS_KEY_MODEL_LIGHT_HUE,
        sizeof(QMESH_MODEL_LIGHT_HUE_NVM_STATE_T),
        1
    },
    
    {
        QMESH_PS_DATA_SINGLE_FIXED,
        QMESH_PS_KEY_MODEL_LIGHT_SAT,
        sizeof(QMESH_MODEL_LIGHT_SAT_NVM_STATE_T),
        1
    },

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
    {
        QMESH_PS_DATA_SINGLE_FIXED,
        QMESH_PS_KEY_MODEL_GENERIC_POWER_LEVEL,
        sizeof(QMESH_MODEL_GEN_PWR_LEVEL_NVM_STATE_T),
        1
    },
#endif
};

/*----------------------------------------------------------------------------*
 *  NAME
 *      NVMAddModelKeys
 *
 *  DESCRIPTION
 *      This function defines keys for all the models in the NVM
 *
 *  RETURNS/MODIFIES
 *       QmeshResult
 *
 *----------------------------------------------------------------------------*/
QMESH_RESULT_T NVMAddModelKeys (void)
{
    uint16_t i;

    for(i=0; i < sizeof(qmesh_model_ps_keys)/sizeof(qmesh_model_ps_keys[0]); i++)
    {
       if (QmeshPsAddKey (&qmesh_model_ps_keys[i]) == QMESH_RESULT_FAILURE)
            return QMESH_RESULT_FAILURE;
    }

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
    QMESH_MODEL_GEN_PWR_LEVEL_NVM_STATE_T nvm_state;

    /* Initialize the data */
    nvm_state.current_pwr_actual = 2;
    nvm_state.trgt_pwr_actual = 2;
    nvm_state.power_last =2;
    nvm_state.power_default = 2;
    nvm_state.pwr_range_min = 0;
    nvm_state.pwr_range_max = 0;
    nvm_state.last_msg_seq_no = 0;
    nvm_state.last_src_addr = 0;

    /* Update NVM */
    NVMWrite_ModelGenPowerLevelState((uint8_t*)&nvm_state);
#endif
    return QMESH_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NVMModelInit
 *
 *  DESCRIPTION
 *      Application should call this function providing the composition data.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
extern void NVMModelInit (const QMESH_DEVICE_COMPOSITION_DATA_T *server_device_composition)
{
    nvm_server_composition = server_device_composition;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NvmReadOnOffModel
 *
 *  DESCRIPTION
 *      This function read the Generic OnOff Model data from NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
extern void NvmReadOnOffModel(QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T* onoff_model_context)
{
    size_t len = sizeof(QMESH_MODEL_GEN_ONOFF_NVM_STATE_T);
    QMESH_MODEL_GEN_ONOFF_NVM_STATE_T state;
    
    /*Read Generic OnOff server state */
    if (QmeshPsReadKeyData (QMESH_PS_KEY_MODEL_GENERIC_ONOFF, 
                            &state, &len) == QMESH_RESULT_SUCCESS)
    {
        /* Set the Generic OnOff state from NVM data */
        onoff_model_context->onoff_state.cur_onoff = state.cur_onoff;
        onoff_model_context->onoff_state.initial_onoff = state.cur_onoff;
        onoff_model_context->onoff_state.trgt_onoff =state.trgt_onoff;
        onoff_model_context->last_msg_seq_no =state.last_msg_seq_no;
        onoff_model_context->last_src_addr =state.last_src_addr;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NvmReadGenericModelLevelForLightness
 *
 *  DESCRIPTION
 *      This function read the Generic Level model data from NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
extern void NvmReadGenericModelLevel()
{
    uint8_t i = 0;
    uint8_t elm_idx = 0;
    uint8_t mdl_idx = 0;
    QMESH_MODEL_GEN_LEVEL_NVM_STATE_T state[QMESH_MAX_GENERIC_LEVEL_INSTANCES];
    size_t len = sizeof(QMESH_MODEL_GEN_LEVEL_NVM_STATE_T) * QMESH_MAX_GENERIC_LEVEL_INSTANCES;
    /* Read Generic Level server state */
    if (QmeshPsReadKeyData (QMESH_PS_KEY_MODEL_GENERIC_LEVEL, 
                            &state[0], &len) == QMESH_RESULT_SUCCESS)
    {
        const QMESH_ELEMENT_CONFIG_T *const elem_config = nvm_server_composition->elements;

        for (elm_idx = 0; elm_idx < nvm_server_composition->num_elements; elm_idx++)
        {
          for (mdl_idx = 0; mdl_idx < elem_config[elm_idx].num_btsig_models; mdl_idx++ )
          {
                if (elem_config[elm_idx].btsig_model_ids[mdl_idx] == QMESH_MODEL_GENERIC_LEVEL)
                {
                    QMESH_GENERIC_LEVEL_CONTEXT_T *lvl_context = \
                          elem_config[elm_idx].element_data->model_data[mdl_idx].model_priv_data;
      
                   /* Set the Generic Level state from NVM data */
                   lvl_context->cur_level = state[i].cur_level;
                   lvl_context->target_level = state[i].target_level;
                   lvl_context->last_msg_seq_no =state[i].last_msg_seq_no;
                   lvl_context->last_src_addr =state[i].last_src_addr;
                   i++;
                   break;
                }
          }
        }
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NvmReadGenericPowerOnOffState
 *
 *  DESCRIPTION
 *      This function read the Generic Power on off state from NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
extern void NvmReadGenericPowerOnOffState(QMESH_GEN_POWERONOFF_DATA_T* pwronoff_context)
{
    QMESH_MODEL_GEN_PWR_ONOFF_NVM_STATE_T state;
    size_t len = sizeof(QMESH_MODEL_GEN_PWR_ONOFF_NVM_STATE_T);
    /* Read Generic Power OnOff server state */
    if (QmeshPsReadKeyData (QMESH_PS_KEY_MODEL_GENERIC_POWER_ONOFF, 
                            &state, &len) == QMESH_RESULT_SUCCESS)
    {
        /* Set the Generic Power OnOff state from NVM data */
        pwronoff_context->onpowerup = state.onpowerup;
        pwronoff_context->last_msg_seq_no =state.last_msg_seq_no;
        pwronoff_context->last_src_addr =state.last_src_addr;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NvmReadGenericDefaultTimeTransitionServerState
 *
 *  DESCRIPTION
 *      This function read the Generic Default Time Transition model data from NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
extern void NvmReadGenericDefaultTimeTransitionServerState(QMESH_GENERIC_DTT_CONTEXT_T* dtt_context)
{
    QMESH_MODEL_GEN_DTT_NVM_STATE_T state;
    size_t len = sizeof(QMESH_MODEL_GEN_DTT_NVM_STATE_T);
    /* Read Generic Default Time Transition server state */
    if (QmeshPsReadKeyData (QMESH_PS_KEY_MODEL_GENERIC_DTT, 
                            &state, &len) == QMESH_RESULT_SUCCESS)
    {
        /* Set the Generic Default Time Transition state from NVM data */
        dtt_context->transition_time = state.transition_time;
        dtt_context->last_msg_seq_no = state.last_msg_seq_no;
        dtt_context->last_src_addr = state.last_src_addr;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NvmReadLightnessServerState
 *
 *  DESCRIPTION
 *      This function read the Light Lightness model data from NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
extern void NvmReadLightnessServerState(QMESH_LIGHTNESS_MODEL_CONTEXT_T* lightness_context)
{
    size_t len = sizeof(QMESH_MODEL_LIGHTNESS_NVM_STATE_T);
    QMESH_MODEL_LIGHTNESS_NVM_STATE_T nvm_state;

    /* Read Lightness server state */
    if (QmeshPsReadKeyData (QMESH_PS_KEY_MODEL_LIGHTNESS, 
                            &nvm_state, &len) == QMESH_RESULT_SUCCESS)
    {
         lightness_context->lightness_linear = nvm_state.lightness_linear; 
         lightness_context->lightness_actual = nvm_state.lightness_actual; 
         lightness_context->lightness_last = nvm_state.lightness_last; 
         lightness_context->lightness_default = nvm_state.lightness_default; 
         lightness_context->lightness_min_range = nvm_state.lightness_min_range; 
         lightness_context->lightness_max_range = nvm_state.lightness_max_range;
         lightness_context->last_msg_seq_no = nvm_state.last_msg_seq_no;   
         lightness_context->last_src_addr = nvm_state.last_src_addr;
         lightness_context->lightness_target = nvm_state.lightness_target;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NvmReadReadHSLServerState
 *
 *  DESCRIPTION
 *      This function read the Light HSL model data from NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
extern void NvmReadReadHSLServerState(QMESH_LIGHT_HSL_MODEL_CONTEXT_T* hsl_context)
{
    QMESH_MODEL_LIGHT_HSL_NVM_STATE_T nvm_state;
    size_t len = sizeof(QMESH_MODEL_LIGHT_HSL_NVM_STATE_T);

    /* Read HSL server state */
    if (QmeshPsReadKeyData (QMESH_PS_KEY_MODEL_LIGHT_HSL, 
                            &nvm_state, &len) == QMESH_RESULT_SUCCESS)
    {
         hsl_context->light_hsl_lightness = nvm_state.light_hsl_lightness; 
         hsl_context->light_hsl_hue_def = nvm_state.light_hsl_hue_def; 
         hsl_context->light_hsl_hue_min = nvm_state.light_hsl_hue_min; 
         hsl_context->light_hsl_hue_max = nvm_state.light_hsl_hue_max; 
         hsl_context->light_hsl_sat_def = nvm_state.light_hsl_sat_def; 
         hsl_context->light_hsl_sat_min = nvm_state.light_hsl_sat_min;
         hsl_context->light_hsl_sat_max = nvm_state.light_hsl_sat_max;
         hsl_context->last_msg_seq_no = nvm_state.last_msg_seq_no;
         hsl_context->last_src_addr = nvm_state.last_src_addr;
         hsl_context->light_target_hsl = nvm_state.light_target_hsl;
         hsl_context->light_target_hue = nvm_state.light_target_hue;
         hsl_context->light_target_sat = nvm_state.light_target_sat;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NvmReadModelLightHueServerState
 *
 *  DESCRIPTION
 *      This function read the Light Hue model data from NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
extern void NvmReadModelLightHueServerState(QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T* hue_context)
{
    QMESH_MODEL_LIGHT_HUE_NVM_STATE_T nvm_state;
    size_t len = sizeof(QMESH_MODEL_LIGHT_HUE_NVM_STATE_T);

    /* Read Hue server state */
    if (QmeshPsReadKeyData (QMESH_PS_KEY_MODEL_LIGHT_HUE, 
                            &nvm_state, &len) == QMESH_RESULT_SUCCESS)
    {

         hue_context->light_hsl_hue = nvm_state.light_hsl_hue; 
         hue_context->last_msg_seq_no = nvm_state.last_msg_seq_no;
         hue_context->last_src_addr = nvm_state.last_src_addr;
         hue_context->light_target_hue = nvm_state.light_target_hue;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NvmReadSaturationServerState
 *
 *  DESCRIPTION
 *      This function read the Light Saturation model data from NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
extern void NvmReadSaturationServerState(QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T* sat_context)
{
    QMESH_MODEL_LIGHT_SAT_NVM_STATE_T nvm_state;
    size_t len = sizeof(QMESH_MODEL_LIGHT_SAT_NVM_STATE_T);

    /* Read Saturation server state */
    if (QmeshPsReadKeyData (QMESH_PS_KEY_MODEL_LIGHT_SAT, 
                            &nvm_state, &len) == QMESH_RESULT_SUCCESS)
    {
        sat_context->light_hsl_sat = nvm_state.light_hsl_sat; 
        sat_context->last_msg_seq_no = nvm_state.last_msg_seq_no;
        sat_context->last_src_addr = nvm_state.last_src_addr;
        sat_context->light_target_sat = nvm_state.light_target_sat;
    }   
}

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
/*----------------------------------------------------------------------------*
 *  NAME
 *      NvmReadGenericPowerLevelServerState
 *
 *  DESCRIPTION
 *      This function read the Generic Power Level model data from NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
extern void NvmReadGenericPowerLevelServerState(QMESH_GENERIC_POWERLEVEL_CONTEXT_T* pwr_lvl_context)
{
    QMESH_MODEL_GEN_PWR_LEVEL_NVM_STATE_T state;
    size_t len = sizeof(QMESH_MODEL_GEN_PWR_LEVEL_NVM_STATE_T);

    /* Read Generic Power Level server state */
    if (QmeshPsReadKeyData (QMESH_PS_KEY_MODEL_GENERIC_POWER_LEVEL, 
                             &state, &len) == QMESH_RESULT_SUCCESS)
    {
        /* Set the Generic Power Level state from NVM data */
        pwr_lvl_context->pwr_actual_state.initial_pwr_actual = state.current_pwr_actual;
        pwr_lvl_context->pwr_actual_state.current_pwr_actual = state.current_pwr_actual;
        pwr_lvl_context->pwr_actual_state.trgt_pwr_actual = state.trgt_pwr_actual;
        pwr_lvl_context->power_last =state.power_last;
        pwr_lvl_context->power_default =state.power_default;
        pwr_lvl_context->power_range.pwr_range_min =state.pwr_range_min;
        pwr_lvl_context->power_range.pwr_range_max =state.pwr_range_max;
        pwr_lvl_context->last_msg_seq_no =state.last_msg_seq_no;
        pwr_lvl_context->last_src_addr =state.last_src_addr;
    }
}

#endif /* QMESH_POWERLEVEL_SERVER_MODEL_ENABLE */

/*----------------------------------------------------------------------------*
 *  NAME
 *      NVMWrite_ModelGenOnOffState
 *
 *  DESCRIPTION
 *      This function writes the Generic OnOff Model data to NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
void  NVMWrite_ModelGenOnOffState(uint8_t* state)
{
    size_t len = sizeof(QMESH_MODEL_GEN_ONOFF_NVM_STATE_T);
    QmeshPsAddKeyData (QMESH_PS_KEY_MODEL_GENERIC_ONOFF, (uint16_t*)state, &len);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NVMModel_ModelGenWriteLevelState
 *
 *  DESCRIPTION
 *      This function writes the Generic Level model data to NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
extern void NVMWrite_ModelGenWriteLevelState ()
{
    int i = 0;
    uint8_t elm_idx = 0;
    uint8_t mdl_idx = 0;
    
    QMESH_MODEL_GEN_LEVEL_NVM_STATE_T state[QMESH_MAX_GENERIC_LEVEL_INSTANCES];
    
    const QMESH_ELEMENT_CONFIG_T *const elem_config = nvm_server_composition->elements;

    for (elm_idx = 0; elm_idx < nvm_server_composition->num_elements; elm_idx++)
    {
        for (mdl_idx = 0; mdl_idx < elem_config[elm_idx].num_btsig_models; mdl_idx++ )
        {
                if (elem_config[elm_idx].btsig_model_ids[mdl_idx] == QMESH_MODEL_GENERIC_LEVEL)
                {
                    QMESH_GENERIC_LEVEL_CONTEXT_T *lvl_context = \
                          elem_config[elm_idx].element_data->model_data[mdl_idx].model_priv_data;
                          
                    /* Set the Generic Level state from NVM data */
                    state[i].cur_level = lvl_context->cur_level;
                    state[i].target_level = lvl_context->target_level;
                    state[i].last_msg_seq_no = lvl_context->last_msg_seq_no;
                    state[i].last_src_addr = lvl_context->last_src_addr;
                    i++;
                    break;
                }
        }
    }
               
    size_t len = sizeof(QMESH_MODEL_GEN_LEVEL_NVM_STATE_T) * QMESH_MAX_GENERIC_LEVEL_INSTANCES;
    QmeshPsAddKeyData (QMESH_PS_KEY_MODEL_GENERIC_LEVEL, (uint16_t*)state, &len);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NVMWrite_ModelGenPowerOnOffState
 *
 *  DESCRIPTION
 *      This function writes the Generic PowerOnOff model data to NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
void  NVMWrite_ModelGenPowerOnOffState(uint8_t* state)
{
    size_t len = sizeof(QMESH_MODEL_GEN_PWR_ONOFF_NVM_STATE_T);
    QmeshPsAddKeyData (QMESH_PS_KEY_MODEL_GENERIC_POWER_ONOFF, (uint16_t*)state, &len);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NVMWrite_ModelGenDTTState
 *
 *  DESCRIPTION
 *      This function writes the Generic Default Time Transition model data to NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
/*Write the Generic Default Time Transition model state to NVM*/
void  NVMWrite_ModelGenDTTState(uint8_t* state)
{
    size_t len = sizeof(QMESH_MODEL_GEN_DTT_NVM_STATE_T);
    QmeshPsAddKeyData (QMESH_PS_KEY_MODEL_GENERIC_DTT, (uint16_t*)state, &len);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NVMWrite_ModelLightnessState
 *
 *  DESCRIPTION
 *      This function writes the Light Lightness model data to NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
void NVMWrite_ModelLightnessState(uint8_t* state)
{
    size_t len = sizeof(QMESH_MODEL_LIGHTNESS_NVM_STATE_T);
    QmeshPsAddKeyData (QMESH_PS_KEY_MODEL_LIGHTNESS, (uint16_t*)state, &len);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NVMWrite_ModelLightHslState
 *
 *  DESCRIPTION
 *      This function writes the Light HSL model data to NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
void NVMWrite_ModelLightHslState(uint8_t* state)
{
    size_t len = sizeof(QMESH_MODEL_LIGHT_HSL_NVM_STATE_T);
    QmeshPsAddKeyData (QMESH_PS_KEY_MODEL_LIGHT_HSL, (uint16_t*)state, &len);  
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NVMWrite_ModelLightSatState
 *
 *  DESCRIPTION
 *      This function writes the Light Saturation model data to NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
void NVMWrite_ModelLightSatState(uint8_t* state)
{
    size_t len = sizeof(QMESH_MODEL_LIGHT_SAT_NVM_STATE_T);
    QmeshPsAddKeyData (QMESH_PS_KEY_MODEL_LIGHT_SAT, (uint16_t*)state, &len);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      NVMWrite_ModelLightHueState
 *
 *  DESCRIPTION
 *      This function writes the Light Hue model data to NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
void NVMWrite_ModelLightHueState(uint8_t* state)
{
    size_t len = sizeof(QMESH_MODEL_LIGHT_HUE_NVM_STATE_T);
    QmeshPsAddKeyData (QMESH_PS_KEY_MODEL_LIGHT_HUE, (uint16_t*)state, &len);
}

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
/*----------------------------------------------------------------------------*
 *  NAME
 *      NVMWrite_ModelGenPowerLevelState
 *
 *  DESCRIPTION
 *      This function writes the Generic Power Level model data to NVM.
 *
 *  RETURNS/MODIFIES
 *       Nothing
 *
 *----------------------------------------------------------------------------*/
void  NVMWrite_ModelGenPowerLevelState(uint8_t* state)
{
    size_t len = sizeof(QMESH_MODEL_GEN_PWR_LEVEL_NVM_STATE_T);
    QmeshPsAddKeyData (QMESH_PS_KEY_MODEL_GENERIC_POWER_LEVEL, (uint16_t*)state, &len);
}
#endif
