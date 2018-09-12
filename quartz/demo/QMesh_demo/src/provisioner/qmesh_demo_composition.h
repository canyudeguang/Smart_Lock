/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*
  * This file contains datatypes used for the defining Composition Data.
 */
/*****************************************************************************/
#ifndef __QMESH_DEMO_COMPOSITION_H__
#define __QMESH_DEMO_COMPOSITION_H__

/* Model headers */
#include "qmesh_config_server.h"
#include "qmesh_config_client.h"
#include "qmesh_health_server.h"
#include "qmesh_health_client.h"
#include "qmesh_generic_onoff_handler.h"
#include "qmesh_generic_default_transition_time_handler.h"
#include "qmesh_generic_poweronoff_handler.h"
#include "qmesh_generic_poweronoff_setup_handler.h"
#include "qmesh_generic_level_handler.h"
#include "qmesh_light_hsl_handler.h"
#include "qmesh_light_hsl_hue_handler.h"
#include "qmesh_light_hsl_saturation_handler.h"
#include "qmesh_light_hsl_setup_handler.h"
#include "qmesh_light_lightness_handler.h"
#include "qmesh_light_lightness_setup_handler.h"
#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
#include "qmesh_generic_powerlevel_handler.h"
#include "qmesh_generic_powerlevel_setup_handler.h"
#endif

/* Starting sequence number to use in the outgoing mesh packets */
#define START_SEQUENCE_NUMBER               (0x014820u)
/* Bluetooth SIG defined location descriptor value */
#define ELEMENT_LOCATION_FRONT              (0x100)
/* Company Identifier value */
#define DEVICE_COMPANY_IDENTIFIER         (0x00D7)
/* Vendor assigned product identifier */
#define DEVICE_PRODUCT_IDENTIFIER          (0x001A)
/* Vendor assigned product version identifier */
#define DEVICE_VERSION_IDENTIFIER           (0x0003)
/* Minimum number of replay protection list items on the device */
#define DEVICE_CRPL                                        (0x0003)
/* Bit field of the device features. See QMESH_DEVICE_FEATURE_MASK_T for more details */
#define DEVICE_FEATURE                                 (0x0003) /* Relay and Proxy supported */
/* Time-To-Live for publish messages */
#define PUBLISH_TTL                                        (0x30)
/* Number of elements supported on this device */
#define NUMER_OF_ELEMENT                           (1)
/* Primary element address of the this device */
#define LOCAL_UNICAST_ADDRESS           (0x77)
/* Maximum number of subscription address per model */
#define QMESH_MAX_SUB_ADDRS_LIST            4
/* Maximum number of Application keys that can bound to a model */
#define QMESH_MAX_APP_KEYS_LIST               6

#define QMESH_CONFIG_MODEL_KEYLIST_SIZE     (0x1)

/*****************************************************************************
 * Sample Composition Data for Server Models
******************************************************************************/
/* SIG model IDs supported on this device */
extern const uint16_t provisioner_sig_model_ids[];
/* Subscription list */
extern uint16_t provisioner_model_sub_list[3][QMESH_MAX_SUB_ADDRS_LIST];
extern uint8_t onbootpowerup;
/* Array holding the model data for each of the models supported */
extern QMESH_MODEL_PUBLISH_STATE_T provisioner_publish_state[];
/* Array holding the model data for each of the models supported */
extern QMESH_MODEL_DATA_T provisioner_model_data[];
/* Element data on the Provisioner */
extern QMESH_ELEMENT_DATA_T provisioner_element_data;
/* Server Element List */
extern const QMESH_ELEMENT_CONFIG_T provisioner_element_list[NUMER_OF_ELEMENT];
/* Composition Data with one element */
extern const QMESH_DEVICE_COMPOSITION_DATA_T server_device_composition;

/* Model contexts referred by server implementations */
extern QMESH_GENERIC_LEVEL_CONTEXT_T          g_gen_lvl_context[3];
extern QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T    g_onoff_model_context;
extern QMESH_GEN_POWERONOFF_DATA_T            g_pwronoff_context;
extern QMESH_LIGHTNESS_MODEL_CONTEXT_T        g_lightness_server_context;
extern QMESH_LIGHT_HSL_MODEL_CONTEXT_T        g_hsl_server_context; 
extern QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T    g_hsl_hue_server_context;
extern QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T    g_hsl_sat_server_context;
extern QMESH_GENERIC_DTT_CONTEXT_T            g_gen_dtt_context;
#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
extern QMESH_GENERIC_POWERLEVEL_CONTEXT_T g_pwr_lvl_context;
#endif

/*----------------------------------------------------------------------------*
 * DemoCompAppKeyListInit
 */
/*! \brief Creates Application Key list for all the supported models
 *
 *  \param none
 *
 *  \returns None.
 */
/*---------------------------------------------------------------------------*/
void DemoCompAppKeyListInit(void);

/*----------------------------------------------------------------------------*
 * DemoGetConfigClientElemAddr
 */
/*! \brief Returns the Unicast address of the Provisioner device.
 *
 *  \param none
 *
 *  \returns Unicast address of this device.
 */
/*---------------------------------------------------------------------------*/
void DemoCompSubsListInit(void);

/*----------------------------------------------------------------------------*
 * DemoGetConfigClientElemAddr
 */
/*! \brief Returns the Unicast address of the Provisioner device.
 *
 *  \param none
 *
 *  \returns Unicast address of this device.
 */
/*---------------------------------------------------------------------------*/
uint16_t DemoGetConfigClientElemAddr(void);
#endif /* __QMESH_DEMO_COMPOSITION_H__ */
