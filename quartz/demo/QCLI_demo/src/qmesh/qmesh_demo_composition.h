/******************************************************************************
Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
#ifndef __QMESH_DEMO_COMPOSITION_H__
#define __QMESH_DEMO_COMPOSITION_H__

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
#include "qmesh_vendor_model_handler.h"

#define START_SEQUENCE_NUMBER           (0x014820u)
#define ELEMENT_LOCATION_FRONT          (0x100)
#define DEVICE_COMPANY_IDENTIFIER       (0x00D7)
#define DEVICE_PRODUCT_IDENTIFIER       (0x001A)
#define DEVICE_VERSION_IDENTIFIER       (0x0003)
#define DEVICE_CRPL                     (0x0003)
#define DEVICE_FEATURE                  (0x0007) /* Relay, Proxy and Friend*/
#define PUBLISH_TTL                     (0x30)

#define QMESH_CONFIG_MODEL_KEYLIST_SIZE     (0x1)
#define QMESH_MAX_APP_KEYS_LIST             (0x06)
#define QMESH_MAX_SUB_ADDRS_LIST            (0x06)

/*****************************************************************************
 * Sample Composition Data for Server Models
******************************************************************************/
/*
 * Primary element for Light HSL
 */
extern const uint16_t server_sig_model_ids_elm_light_hsl[];
/*
* Secondary element for Light Hue
*/
extern const uint16_t server_sig_model_ids_elm_light_hue[];
/*
* Secondary element for Light Saturation
*/
extern const uint16_t server_sig_model_ids_elm_light_saturation[];

extern uint8_t onbootpowerup;

extern QMESH_MODEL_PUBLISH_STATE_T server_elm_light_hsl_publish_state[];

extern QMESH_MODEL_PUBLISH_STATE_T server_elm_light_hue_publish_state[];

extern QMESH_MODEL_PUBLISH_STATE_T server_elm_light_saturation_publish_state[];

extern QMESH_MODEL_DATA_T server_elm_light_hsl_model_data[];

extern QMESH_MODEL_DATA_T server_elm_light_hue_model_data[];
extern QMESH_MODEL_DATA_T server_elm_light_saturation_model_data[];

extern QMESH_ELEMENT_DATA_T server_element_light_hsl_data;


extern QMESH_ELEMENT_DATA_T server_element_light_hue_data;

extern QMESH_ELEMENT_DATA_T server_element_light_saturation_data;

/*
 * Server Element List
 */
extern const QMESH_ELEMENT_CONFIG_T server_element_list[];

/*
 * Composition Data with 3 elements
 */
extern const QMESH_DEVICE_COMPOSITION_DATA_T server_device_composition;

/*****************************************************************************
 * Sample Composition Data for Client Models
******************************************************************************/
/*
 * Primary element for Light HSL Client
 */
extern uint16_t client_sig_model_ids_elm_light_hsl[];
extern uint32_t client_vendor_model_ids[];

extern QMESH_MODEL_PUBLISH_STATE_T client_elm_light_publish_state[];

extern QMESH_MODEL_DATA_T client_elem_light_model_data[];

extern QMESH_ELEMENT_DATA_T client_element_light_data;

/*
 * Element List
 */
extern const QMESH_ELEMENT_CONFIG_T client_ele_list[];

/*
 * Composition Data with 1 element
 */
extern const QMESH_DEVICE_COMPOSITION_DATA_T client_device_composition;

extern QMESH_GENERIC_LEVEL_CONTEXT_T          g_gen_lvl_context[];
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

extern QMESH_HEALTH_SERVER_STATE_T g_health_server_state;
extern QMESH_HEALTH_MODEL_DATA_T current_fault;

void DemoCompAppKeyListInit(void);

void DemoCompSubsListInit(void);

uint16_t DemoGetConfigClientElemAddr(void);

#endif /* __QMESH_DEMO_COMPOSITION_H__ */


