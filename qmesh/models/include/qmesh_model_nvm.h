/*=============================================================================
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/******************************************************************************/
/*! \file qmesh_model_nvm.h
 *  \brief
 *      Defines the NVM for the models.
 *
 */
/******************************************************************************/
#ifndef __QMESH_MODEL_NVM_H__
#define __QMESH_MODEL_NVM_H__

#include "qmesh_hal_ifce.h"
#include "qmesh_types.h"
#include "qmesh_model_config.h"
#include "qmesh_model_debug.h"
#include "qmesh.h"
#include "qmesh_ps.h"
#include "qmesh_soft_timers.h"

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

#ifdef __cplusplus
extern "C" {
#endif

/* Model NVM Utilities */
typedef enum {
    QMESH_PS_KEY_MODEL_GENERIC_ONOFF= QMESH_PS_KEY_MODEL_GROUP, /* Generic OnOff Server state */
    QMESH_PS_KEY_MODEL_GENERIC_LEVEL, /* Generic Level Server state */
    QMESH_PS_KEY_MODEL_GENERIC_POWER_ONOFF, /* Generic Power OnOff Server state */
    QMESH_PS_KEY_MODEL_GENERIC_DTT, /* Generic Default Time Transition Server state */
    QMESH_PS_KEY_MODEL_GENERIC_POWER_LEVEL, /* Generic PowerLevel Server state */
    QMESH_PS_KEY_MODEL_LIGHTNESS, /* Lightness Server state */
    QMESH_PS_KEY_MODEL_LIGHT_HSL, /* HSL Server state */
    QMESH_PS_KEY_MODEL_LIGHT_HUE, /* Hue Server state */
    QMESH_PS_KEY_MODEL_LIGHT_SAT /* Saturation Server state */
}MODEL_NVM_KEYS_T;

/* Number of Qmesh Model Keys */
#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
#define QMESH_MODEL_PS_KEY_LIST_SIZE                       (9)
#else
#define QMESH_MODEL_PS_KEY_LIST_SIZE                       (8)
#endif /* QMESH_POWERLEVEL_SERVER_MODEL_ENABLE */

extern const QMESH_PS_KEY_INFO_T qmesh_model_ps_keys[QMESH_MODEL_PS_KEY_LIST_SIZE];

/*----------------------------------------------------------------------------*
 * NVMAddModelKeys
 */
/*! \brief This function defines keys for all the models in the NVM
 *
 *  \return \ref QmeshResult
 */
/*---------------------------------------------------------------------------*/
QmeshResult NVMAddModelKeys (void);

/*----------------------------------------------------------------------------*
 * NVMModelInit
 */
/*! \brief Application call this function providing composition data.
 *
 *  \return \ref QmeshResult
 */
/*---------------------------------------------------------------------------*/
extern void NVMModelInit (const QMESH_DEVICE_COMPOSITION_DATA_T *server_device_composition);

/*----------------------------------------------------------------------------*
 * NvmReadOnOffModel
 */
/*! \brief This function reads on off model data from the NVM
 *
 *  \return \ref QmeshResult
 */
/*---------------------------------------------------------------------------*/
extern void NvmReadOnOffModel(QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T* onoff_model_context);

/*----------------------------------------------------------------------------*
 * NvmReadGenericModelLevel
 */
/*! \brief This function reads  level data from the NVM
 *
 *  \return \ref QmeshResult
 */
/*---------------------------------------------------------------------------*/
extern void NvmReadGenericModelLevel(void);


/*----------------------------------------------------------------------------*
 * NvmReadGenericPowerOnOffState
 */
/*! \brief This function reads poweronoff data from the NVM
 *
 *  \return \ref QmeshResult
 */
/*---------------------------------------------------------------------------*/
extern void NvmReadGenericPowerOnOffState(QMESH_GEN_POWERONOFF_DATA_T* pwronoff_context);

/*----------------------------------------------------------------------------*
 * NvmReadGenericDefaultTimeTransitionServerState
 */
/*! \brief This function reads DTT Model data from the NVM
 *
 *  \return \ref QmeshResult
 */
/*---------------------------------------------------------------------------*/
extern void NvmReadGenericDefaultTimeTransitionServerState(QMESH_GENERIC_DTT_CONTEXT_T* dtt_context);

/*----------------------------------------------------------------------------*
 * NvmReadLightnessServerState
 */
/*! \brief This function reads lightness data from the NVM
 *
 *  \return \ref QmeshResult
 */
/*---------------------------------------------------------------------------*/
extern void NvmReadLightnessServerState(QMESH_LIGHTNESS_MODEL_CONTEXT_T* lightness_context);

/*----------------------------------------------------------------------------*
 * NvmReadReadHSLServerState
 */
/*! \brief This function reads HSL Server data from the NVM
 *
 *  \return \ref QmeshResult
 */
/*---------------------------------------------------------------------------*/
extern void NvmReadReadHSLServerState(QMESH_LIGHT_HSL_MODEL_CONTEXT_T* hsl_context);

/*----------------------------------------------------------------------------*
 * NvmReadModelLightHueServerState
 */
/*! \brief This function reads Hue Data the NVM
 *
 *  \return \ref QmeshResult
 */
/*---------------------------------------------------------------------------*/
extern void NvmReadModelLightHueServerState(QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T* hue_context);

/*----------------------------------------------------------------------------*
 * NvmReadSaturationServerState
 */
/*! \brief This function reads saturation data from the NVM
 *
 *  \return \ref QmeshResult
 */
/*---------------------------------------------------------------------------*/
extern void NvmReadSaturationServerState(QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T* sat_context);

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
/*----------------------------------------------------------------------------*
 * NvmReadGenericPowerLevelServerState
 */
/*! \brief This function reads the powerlevel data from the NVM
 *
 *  \return \ref QmeshResult
 */
/*---------------------------------------------------------------------------*/
extern void NvmReadGenericPowerLevelServerState(QMESH_GENERIC_POWERLEVEL_CONTEXT_T* pwr_lvl_context);
#endif

/*----------------------------------------------------------------------------*
 * NVMWrite_ModelGenOnOffState
 */
/*! \brief This function writes the Generic OnOff Model data to NVM.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
void  NVMWrite_ModelGenOnOffState(uint8_t* state);

/*----------------------------------------------------------------------------*
 * NVMWrite_ModelGenWriteLevelState
 */
/*! \brief Writes the level data to NVM.
 *
 *  \return nothing
 */
/*---------------------------------------------------------------------------*/
extern void NVMWrite_ModelGenWriteLevelState (void);

/*----------------------------------------------------------------------------*
 * NVMWrite_ModelGenPowerOnOffState
 */
/*! \brief This function writes the Generic Power OnOff Model data to NVM.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
void  NVMWrite_ModelGenPowerOnOffState(uint8_t* state);

/*----------------------------------------------------------------------------*
 * NVMWrite_ModelGenDTTState
 */
/*! \brief This function writes the Generic Default Time Transition Model data to NVM.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
void  NVMWrite_ModelGenDTTState(uint8_t* state);

/*----------------------------------------------------------------------------*
 * NVMWrite_ModelLightnessState
 */
/*! \brief This function writes the Light Lightness Model data to NVM.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
void NVMWrite_ModelLightnessState(uint8_t* state);

/*----------------------------------------------------------------------------*
 * NVMWrite_ModelLightHslState
 */
/*! \brief This function writes the Light HSL Model data to NVM.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
void NVMWrite_ModelLightHslState(uint8_t* state);

/*----------------------------------------------------------------------------*
 * NVMWrite_ModelLightSatState
 */
/*! \brief This function writes the Light Saturation Model data to NVM.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
void NVMWrite_ModelLightSatState(uint8_t* state);

/*----------------------------------------------------------------------------*
 * NVMWrite_ModelLightHueState
 */
/*! \brief This function writes the Generic Light Hue Model data to NVM.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
void NVMWrite_ModelLightHueState(uint8_t* state);

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
/*----------------------------------------------------------------------------*
 * NVMWrite_ModelGenPowerLevelState
 */
/*! \brief This function writes the Generic PowerLevel Model data to NVM.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
void  NVMWrite_ModelGenPowerLevelState(uint8_t* state);
#endif

#ifdef __cplusplus
}
#endif

/*!@} */

#endif /* __QMESH_MODEL_COMMON_H__ */
