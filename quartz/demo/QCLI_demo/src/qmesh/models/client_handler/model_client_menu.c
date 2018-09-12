/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/

#include "qcli_api.h"

/* Model Headers */
#include "generic_onoff_client_handler.h"
#include "generic_power_onoff_client_handler.h"
#include "generic_power_level_client_handler.h"
#include "generic_default_transition_time_client_handler.h"
#include "generic_level_client_handler.h"
#include "light_lightness_client_handler.h"
#include "light_hsl_client_handler.h"

/* Enabled Models */
#define QMESH_GENERIC_ONOFF_CLIENT_ENABLE 1
#define QMESH_GENERIC_POWER_ONOFF_CLIENT_ENABLE 1
#define QMESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_ENABLE 1
#define QMESH_GENERIC_POWER_LEVEL_CLIENT_ENABLE 1
#define QMESH_GENERIC_LEVEL_CLIENT_ENABLE 1
#define QMESH_LIGHT_LIGHTNESS_CLIENT_ENABLE 1
#define QMESH_LIGHT_HSL_CLIENT_ENABLE 1
/* QMesh Model Client Commands */
const QCLI_Command_t qmesh_model_cmd_list[] =
{
   /* {
    *   cmd_function,
    *   start_thread,
    *   cmd_string,
    *   usage_string,
    *   description
    * }
    */

/* Generic Model APIs */
#if (QMESH_GENERIC_ONOFF_CLIENT_ENABLE == 1)
    /* Generic onoff client model messages */
    {
        QcliGenericOnoffClientGet,
        false,
        "GenericOnoffClientGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]) ",
        " Get the current on/off state of the device"
    },
    {
        QcliGenericOnoffClientSet,
        false,
        "GenericOnoffClientSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [onOff(1)] [Tid(1)] [TransTime(1) optional] [Delay(1) conditional]" ,
        "Set the current on/off state of the device"
    },
#endif

#if (QMESH_GENERIC_POWER_ONOFF_CLIENT_ENABLE == 1)
    /* Generic power onoff client model messages */
    {
        QcliGenericPowerOnoffClientOnpowerupGet,
        false,
        "GenericPowerOnoffClientOnpowerupGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]",
        " Get the current power on/off state of the device"
    },
    {
        QcliGenericPowerOnoffClientOnpowerupSet,
        false,
        "GenericPowerOnoffClientOnpowerupSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [onPowUp(1)] ",
        " Set the current power on/off state of the device"
    },
#endif

#if (QMESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_ENABLE == 1)
    /* Generic default transisiton time client model messages */
    {
        QcliGenericDefaultTransitionTimeClientGet,
        false,
        "GenericDefaultTransitionTimeClientGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]",
        " Get default transistion time state of the device"
    },
    {
        QcliGenericDefaultTransitionTimeClientSet,
        false,
        "GenericDefaultTransitionTimeClientSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [trans time(1)]",
        " Set default transistion time state of the device"
    },
#endif

#if (QMESH_GENERIC_LEVEL_CLIENT_ENABLE == 1)
    /* Generic level client model messages */
    {
        QcliGenericLevelClientGet,
        false,
        "GenericLevelClientGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]",
        "Get the generic level of the device"
    },
    {
        QcliGenericLevelClientSet,
        false,
        "GenericLevelClientSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [Level(1)] [tid(1)][trns time(1) optional] [delay(1) conditional] ",
        "Set the generic level of the device"
    },
    {
        QcliGenericLevelClientDeltaSet,
        false,
        "GenericLevelClientDeltaSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [DeltaLevel(1)] [tid(1)][trns time(1) optional] [delay(1) conditional]",
        "Set the generic delta level of the device"
    },
    {
        QcliGenericLevelClientMoveSet,
        false,
        "GenericLevelClientMoveSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [Level(1)] [tid(1)][trns time(1) optional] [delay(1) conditional] ",
        "Set the generic move level of the device"
    },
#endif

#if (QMESH_GENERIC_POWER_LEVEL_CLIENT_ENABLE == 1)
    /* Generic power level client model messages */
    {
        QcliGenericPowerLevelClientGet,
        false,
        "GenericPowerLevelClientGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]",
        " Get the generic power level of the device"
    },
    {
        QcliGenericPowerLevelClientSet,
        false,
        "GenericPowerLevelClientSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [power(2)] [tid(1)] [transTime(1) optional] [delay(1) conditional] ",
        " Set the generic power Level of the device"
    },
    {
        QcliGenericPowerLevelClientLastGet,
        false,
        "GenericPowerLevelClientLastGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]",
        " Get the generic power last state of the device"
    },
    {
        QcliGenericPowerLevelClientDefaultGet,
        false,
        "GenericPowerLevelClientDefaultGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]",
        " Get the generic power default state of the device"
    },
    {
        QcliGenericPowerLevelClientDefaultSet,
        false,
        "GenericPowerLevelClientDefaultSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [power(2)] ",
        " Set the generic power default state of the device"
    },
    {
        QcliGenericPowerLevelClientRangeGet,
        false,
        "GenericPowerLevelClientRangeGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]",
        " Get the generic power range of the device"
    },
    {
        QcliGenericPowerLevelClientRangeSet,
        false,
        "GenericPowerLevelClientRangeSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [RangeMin(2)] [RangeMax(2)]",
        " Set the power range of the device"
    },
#endif

#if (QMESH_LIGHT_LIGHTNESS_CLIENT_ENABLE == 1)
    /* Light lightness client model mesages */
    {
        QcliLightLightnessClientGet,
        false, "LightLightnessClientGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]",
        " Get light lightness state"
    },
    {
        QcliLightLightnessClientSet,
        false, "LightLightnessClientSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [lightness(2)] [tid(1)] [transTime(1) optional] [delay(1) conditional] ",
        " Set the light lightness state"
    },
    {
        QcliLightLightnessClientLinearGet,
        false, "LightLightnessClientLinearGet",
        "[appID(2)] [destID(2)] [srcAddr(2)] ",
        " Get the linear light state"
    },
    {
        QcliLightLightnessClientLinearSet,
        false, "LightLightnessClientLinearSet",
        "[[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [lightness(2)] [tid(1)] [transTime(1) optional] [delay(1) conditional] ",
        "  Set the light linear state"
    },
    {
        QcliLightLightnessClientLastGet,
        false, "LightLightnessClientLastGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]",
        " Get last light state"
    },
    {
        QcliLightLightnessClientDefaultGet,
        false, "LightLightnessClientDefaultGet",
        "[appID(2)] [destID(2)] [srcAddr(2)] ",
        " Get default ligh state"
    },
    {
        QcliLightLightnessClientDefaultSet,
        false, "LightLightnessClientDefaultSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [lightness(2)]",
        " Set default light state"
    },
    {
        QcliLightLightnessClientRangeGet,
        false, "LightLightnessClientRangeGet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [min(2)] [max(2)]",
        " Get light range"
    },
    {
        QcliLightLightnessClientRangeSet,
        false, "LightLightnessClientRangeSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [min(2)] [max (2)]",
        " Set light range"
    },
#endif

#if (QMESH_LIGHT_HSL_CLIENT_ENABLE == 1)
    /* Light Hsl client model mesages */
    {
        QcliLightHslClientGet,
        false, "LightHslClientGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]",
        " Get light hsl state"
    },
    {
        QcliLightHslClientSet,
        false, "LightHslClientSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [lightness(2)] [hue(2)] [saturation(2)] [tid(1)] [transTime(1) optional] [delay(1) conditional] ",
        " Set the light hsl state"
    },
    {
        QcliLightHslClientHueGet,
        false, "LightHslClientHueGet",
        "[appID(2)] [destID(2)] [srcAddr(2)] ",
        " Get the light hsl hue state"
    },
    {
        QcliLightHslClientHueSet,
        false, "LightHslClientHueSet",
        "[[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [hue(2)] [tid(1)] [transTime(1) optional] [delay(1) conditional] ",
        " Set the light hsl hue state"
    },
    {
        QcliLightHslClientSaturationGet,
        false, "LightHslClientSaturationGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]",
        " Get the light hsl saturation state"
    },
    {
        QcliLightHslClientSaturationSet,
        false, "LightHslClientSaturationSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [saturation(2)] [tid(1)] [transTime(1) optional] [delay(1) conditional] ",
        " Set the light hsl saturation state"
    },
    {
        QcliLightHslClientTargetGet,
        false, "LightHslClientTargetGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]",
        " Get the light hsl target state"
    },
    {
        QcliLightHslClientDefaultGet,
        false, "LightHslClientDefaultGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]",
        " Get the light hsl default state"
    },
    {
        QcliLightHslClientDefaultSet,
        false, "LightHslClientDefaultSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [lightness(2)] [hue(2)] [saturation(2)]",
        " Set the light hsl default state"
    },
    {
        QcliLightHslClientRangeGet,
        false, "LightHslClientRangeGet",
        "[appID(2)] [destID(2)] [srcAddr(2)]",
        " Get the light hsl range state"
    },
    {
        QcliLightHslClientRangeSet,
        false, "LightHslClientRangeSet",
        "[appID(2)] [destID(2)] [srcAddr(2)] [reliable(1)] [hue min(2)] [hue max (2)] [saturation min(2)] [saturation max (2)]",
        " Set the light hsl range state"
    },
#endif
};

/* Command group for SIG Model Clients */
const QCLI_Command_Group_t qmesh_Model_group =
{
    "QMesh Model client",
    (sizeof(qmesh_model_cmd_list) / sizeof(qmesh_model_cmd_list[0])),
    qmesh_model_cmd_list
};
