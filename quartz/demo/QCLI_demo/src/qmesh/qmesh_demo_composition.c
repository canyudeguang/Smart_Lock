/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/

#include "qmesh_demo_composition.h"
#include "qmesh_model_common.h"

#include "qmesh_aksl_list.h"
#include "qmesh_health_server.h"
#include "qmesh_config_client.h"
#include "model_client_event_handler.h"

/*****************************************************************************
 * Sample Composition Data for Server Models
******************************************************************************/
/*
 * Primary element for Light HSL
 */
const uint16_t server_sig_model_ids_elm_light_hsl[] =  {
                                         QMESH_CONFIG_SERVER_MODEL_ID,
                                         QMESH_HEALTH_SERVER_MODEL_ID,
                                         QMESH_MODEL_GENERIC_ONOFF,
                                         QMESH_MODEL_GENERIC_LEVEL,
                                         QMESH_MODEL_GENERIC_POWERONOFF,
                                         QMESH_MODEL_GENERIC_POWERONOFFSETUPSERVER,
                                         QMESH_MODEL_LIGHT_LIGHTNESS,
                                         QMESH_MODEL_LIGHT_LIGHTNESS_SETUP_SERVER,
                                         QMESH_MODEL_LIGHT_HSL_SERVER,
                                         QMESH_MODEL_LIGHT_HSL_SETUP_SERVER,
                                         QMESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME
                               };
/*
* Secondary element for Light Hue
*/
const uint16_t server_sig_model_ids_elm_light_hue[] = {
                               QMESH_MODEL_GENERIC_LEVEL,
                               QMESH_MODEL_LIGHT_HSL_HUE_SERVER
                               };
/*
* Secondary element for Light Saturation
*/
const uint16_t server_sig_model_ids_elm_light_saturation[] = {
                               QMESH_MODEL_GENERIC_LEVEL,
                               QMESH_MODEL_LIGHT_HSL_SATURATION_SERVER
                               };
/*
 * Primary element for Vendor model
 */
uint32_t server_vendor_model_ids_elm_light_hsl[] =  {
                                QMESH_VENDOR_MODEL_SERVER
                               };

/* Context information specific to a model. This contains the state data relevent to that model */
QMESH_GENERIC_LEVEL_CONTEXT_T g_gen_lvl_context[3];
QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T g_onoff_model_context;
QMESH_GEN_POWERONOFF_DATA_T g_pwronoff_context;
QMESH_GENERIC_DTT_CONTEXT_T g_gen_dtt_context;
QMESH_LIGHTNESS_MODEL_CONTEXT_T g_lightness_server_context;
QMESH_LIGHT_HSL_MODEL_CONTEXT_T g_hsl_server_context;
QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T    g_hsl_hue_server_context;
QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T    g_hsl_sat_server_context;
QMESH_HEALTH_SERVER_STATE_T   g_health_server_state;


#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
QMESH_GENERIC_POWERLEVEL_CONTEXT_T g_pwr_lvl_context;
#endif
QMESH_VENDOR_MODEL_DATA_T g_vendor_model_context;
/* Test array intialized with some ramdom test error code */
uint8_t test_array[2] = {0x12, 0x13};

/* Intialized Current Fault / Registerd Fault used by the application */
QMESH_HEALTH_MODEL_DATA_T current_fault = {
                                            0x01, /* test id */
                                            0x02, /* test arry len */
                                            test_array, /* test array ptr */
                                            0x00 /* attention state */
                                           };

uint8_t onbootpowerup = 0;

/* Create App Key Lists for all Server Models Statically. */
CREATE_STATIC_APPKEY_LIST(config_server, QMESH_CONFIG_MODEL_KEYLIST_SIZE);
CREATE_STATIC_APPKEY_LIST(health_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(gen_onoff_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(gen_level_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(gen_power_onoff_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(gen_power_onoff_setup_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(light_lightness_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(light_lightness_setup_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(light_hsl_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(light_hsl_setup_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(gen_default_tt_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(hue_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(hue_gen_level_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(saturation_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(saturation_gen_level_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(vendor_server, QMESH_MAX_APP_KEYS_LIST);

/* Create Subscription Lists for bound server models. */
CREATE_STATIC_SUBS_LIST(config_server, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(health_server, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(gen_power_onoff_server, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(light_lightness_server, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(hue_server, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(saturation_server, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(gen_default_tt_server, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(vendor_server, QMESH_MAX_SUB_ADDRS_LIST);


QMESH_MODEL_PUBLISH_STATE_T server_elm_light_hsl_publish_state[] =
{
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        NULL                        /* publication handler  */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        QmeshHealthServerPublicationHandler /* publication handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        QmeshGenericOnOffPublicationHandler /* publication handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        QmeshGenericLevelPublicationHandler /* publication handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        QmeshGenericPowerOnOffPublicationHandler  /* publication handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        QmeshGenericPowerOnOffPublicationHandler  /* publication handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* *publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        QmeshLightLightnessPublicationHandler /* publication handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        NULL                        /* publication handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        QmeshLightHslPublicationHandler  /* publication handler  */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        NULL                        /* publication handler  */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        QmeshGenericDTTPublicationHandler  /* publication handler */
    }
};

QMESH_MODEL_PUBLISH_STATE_T server_elm_vendor_publish_state[] =
{
    { /* Vendor publish state */
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* *publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        NULL                        /* publication handler */
    }
};

QMESH_MODEL_PUBLISH_STATE_T server_elm_light_hue_publish_state[] =
{
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        QmeshGenericLevelPublicationHandler /* publication handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        QmeshLightHslHuePublicationHandler /* publication handler */
    }
};

QMESH_MODEL_PUBLISH_STATE_T server_elm_light_saturation_publish_state[] =
{
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        QmeshGenericLevelPublicationHandler /* publication handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        QmeshLightHslSatPublicationHandler /* publication handler */
    }
};


QMESH_MODEL_DATA_T server_elm_light_hsl_model_data[] =
{
    /* Initialize the Config model data */
    {
        &server_elm_light_hsl_publish_state[0],      /* publish_state */
        GET_STATIC_SUBS_LIST(config_server),                   /* subs_list */
        GET_STATIC_APPKEY_LIST(config_server),       /* app_key_list */
        NULL,                                        /* model_priv_data */
        QmeshConfigModelServerHandler,               /* model_handler */
    },
    /* Initialize the Health model data */
    {
        &server_elm_light_hsl_publish_state[1],      /* publish_state */
        GET_STATIC_SUBS_LIST(health_server),      /* subs_list */
        GET_STATIC_APPKEY_LIST(health_server),       /* app_key_list */
        &g_health_server_state,                      /* model_priv_data */
        QmeshHealthModelServerHandler,               /* model_handler */
    },
    /* Initialize the Generic OnOff model data */
    {
        &server_elm_light_hsl_publish_state[2],      /* publish_state */
        GET_STATIC_SUBS_LIST(light_lightness_server),      /* subs_list */
        GET_STATIC_APPKEY_LIST(gen_onoff_server),    /* app_key_list */
        &g_onoff_model_context,                      /* model_priv_data */
        QmeshGenericOnOffServerHandler,              /* model_handler */
    },
    /* Initialize the Generic level model data */
        {
        &server_elm_light_hsl_publish_state[3],      /* publish_state */
        GET_STATIC_SUBS_LIST(light_lightness_server),      /* subs_list */
        GET_STATIC_APPKEY_LIST(gen_level_server),    /* app_key_list */
        &g_gen_lvl_context[0],                       /* model_priv_data */
        QmeshGenericLevelServerHandler,              /* model_handler */
    },
    /* Initialize the Generic power on off model data */
    {
        &server_elm_light_hsl_publish_state[4],           /* publish_state */
        GET_STATIC_SUBS_LIST(gen_power_onoff_server),           /* subs_list */
        GET_STATIC_APPKEY_LIST(gen_power_onoff_server),   /* app_key_list */
        &g_pwronoff_context,                              /* model_priv_data */
        QmeshGenericPowerOnOffServerHandler,              /* model_handler */
    },
    /* Initialize the Generic power on off setup server model data */
    {
        &server_elm_light_hsl_publish_state[5],                 /* publish_state */
        GET_STATIC_SUBS_LIST(gen_power_onoff_server),                 /* subs_list */
        GET_STATIC_APPKEY_LIST(gen_power_onoff_setup_server),   /* app_key_list */
        &g_pwronoff_context,                                    /* model_priv_data */
        QmeshGenericPowerOnOffSetupServerHandler,               /* model_handler */
    },
    /* Initialize the Lightness server model data */
    {
        &server_elm_light_hsl_publish_state[6],      /* publish_state */
        GET_STATIC_SUBS_LIST(light_lightness_server),      /* subs_list */
        GET_STATIC_APPKEY_LIST(light_lightness_server),   /* app_key_list */
        &g_lightness_server_context,                 /* model_priv_data */
        QmeshLightnessServerHandler,                 /* model_handler */
    },
    /* Initialize the Lightness setup server model data */
    {
        &server_elm_light_hsl_publish_state[7],      /* publish_state */
        GET_STATIC_SUBS_LIST(light_lightness_server),      /* subs_list */
        GET_STATIC_APPKEY_LIST(light_lightness_setup_server),   /* app_key_list */
        &g_lightness_server_context,                 /* model_priv_data */
        QmeshLightnessSetupServerHandler,            /* model_handler */
    },
    /* Initialize the Lightness HSL server model data */
    {
        &server_elm_light_hsl_publish_state[8],      /* publish_state */
        GET_STATIC_SUBS_LIST(light_lightness_server),      /* subs_list */
        GET_STATIC_APPKEY_LIST(light_hsl_server),   /* app_key_list */
        &g_hsl_server_context,                       /* model_priv_data */
        QmeshLightHSLServerHandler,                  /* model_handler */
    },
    /* Initialize the Lightness HSL Setup server model data */
    {
        &server_elm_light_hsl_publish_state[9],      /* publish_state */
        GET_STATIC_SUBS_LIST(light_lightness_server),      /* subs_list */
        GET_STATIC_APPKEY_LIST(light_hsl_setup_server),   /* app_key_list */
        &g_hsl_server_context,                       /* model_priv_data */
        QmeshLightHSLSetupServerHandler,             /* model_handler */
    },
    /* Initialize the Generic Default Transition Time server model data */
    {
        &server_elm_light_hsl_publish_state[10],      /* publish_state */
        GET_STATIC_SUBS_LIST(gen_default_tt_server),      /* subs_list */
        GET_STATIC_APPKEY_LIST(gen_default_tt_server),    /* app_key_list */
        &g_gen_dtt_context,                           /* model_priv_data */
        QmeshGenericDTTServerHandler,                 /* model_handler */
    },
    /* Initialize the Vendor model data */    
    {
        &server_elm_vendor_publish_state[0],      /* publish_state */
        GET_STATIC_SUBS_LIST(vendor_server),      /* subs_list */
        GET_STATIC_APPKEY_LIST(vendor_server),   /* app_key_list */
        &g_vendor_model_context,              /* model_priv_data */
        QmeshVendorModelServerHandler,         /* model_handler */
    }
};

QMESH_MODEL_DATA_T server_elm_light_hue_model_data[] =
{
    /* Initialize the Generic Level server model data */
    {
        &server_elm_light_hue_publish_state[0],      /* publish_state */
        GET_STATIC_SUBS_LIST(hue_server),      /* subs_list */
        GET_STATIC_APPKEY_LIST(hue_server),          /* app_key_list */
        &g_gen_lvl_context[1],                       /* model_priv_data */
        QmeshGenericLevelServerHandler,              /* model_handler */
    },
    /* Initialize the Light HSL hue Level server model data */
    {
        &server_elm_light_hue_publish_state[1],      /* publish_state */
        GET_STATIC_SUBS_LIST(hue_server),             /* subs_list */
        GET_STATIC_APPKEY_LIST(hue_gen_level_server),          /* app_key_list */
        &g_hsl_hue_server_context,                   /* model_priv_data */
        QmeshLightHSLHueServerHandler,               /* model_handler */
    },

};

QMESH_MODEL_DATA_T server_elm_light_saturation_model_data[] =
{
    /* Initialize the Generic Level server model data */
    {
        &server_elm_light_saturation_publish_state[0],      /* publish_state */
        GET_STATIC_SUBS_LIST(saturation_server),      /* subs_list */
        GET_STATIC_APPKEY_LIST(saturation_server),   /* app_key_list */
        &g_gen_lvl_context[2],                              /* model_priv_data */
        QmeshGenericLevelServerHandler,                     /* model_handler */
    },
    /* Initialize the Light HSL Saturation server model data */
    {
        &server_elm_light_saturation_publish_state[1],      /* publish_state */
        GET_STATIC_SUBS_LIST(saturation_server),      /* subs_list */
        GET_STATIC_APPKEY_LIST(saturation_gen_level_server),   /* app_key_list */
        &g_hsl_sat_server_context,                          /* model_priv_data */
        QmeshLightHSLSaturationServerHandler,               /* model_handler */
    }
};

QMESH_ELEMENT_DATA_T server_element_light_hsl_data =
{
    START_SEQUENCE_NUMBER,            /* seq_num */
    QMESH_UNASSIGNED_ADDRESS,         /* unicast_addr */
    server_elm_light_hsl_model_data   /* model_data */
};


QMESH_ELEMENT_DATA_T server_element_light_hue_data =
{
    START_SEQUENCE_NUMBER,          /* seq_num */
    QMESH_UNASSIGNED_ADDRESS,       /* unicast_addr */
    server_elm_light_hue_model_data /* model_data */
};

QMESH_ELEMENT_DATA_T server_element_light_saturation_data =
{
    START_SEQUENCE_NUMBER,          /* seq_num */
    QMESH_UNASSIGNED_ADDRESS,       /* unicast_addr */
    server_elm_light_saturation_model_data   /* model_data */
};

/*
 * Server Element List
 */
const QMESH_ELEMENT_CONFIG_T server_element_list[] =
{
    {
        ELEMENT_LOCATION_FRONT,                                         /* Location of the element */
        sizeof(server_sig_model_ids_elm_light_hsl)  / sizeof(uint16_t),   /* Num of sig models */
        sizeof(server_vendor_model_ids_elm_light_hsl )/sizeof(uint32_t),  /* Num of vendor models */
        server_sig_model_ids_elm_light_hsl,                             /* Sig model ids */
        server_vendor_model_ids_elm_light_hsl,                                                           /* Vendor model ids */
        &server_element_light_hsl_data                                  /* element_data */
    },
    {
        ELEMENT_LOCATION_FRONT,                                 /* Location of the element */
        sizeof(server_sig_model_ids_elm_light_hue) / sizeof(uint16_t),     /* Num of sig models */
        0,                                                      /* Num of vendor models */
        server_sig_model_ids_elm_light_hue,                     /* Sig model ids */
        NULL,                                                   /* Vendor model ids */
        &server_element_light_hue_data                          /* element_data */
    },
    {
        ELEMENT_LOCATION_FRONT,                                 /* Location of the element */
        sizeof(server_sig_model_ids_elm_light_saturation) / sizeof(uint16_t),     /* Num of sig models */
        0,                                                      /* Num of vendor models */
        server_sig_model_ids_elm_light_saturation,              /* Sig model ids */
        NULL,                                                   /* Vendor model ids */
        &server_element_light_saturation_data                 /* element_data */
    }
};


/*
 * Composition Data with 3 elements
 */
const QMESH_DEVICE_COMPOSITION_DATA_T server_device_composition =
{
    DEVICE_COMPANY_IDENTIFIER,   /* CID */
    DEVICE_PRODUCT_IDENTIFIER,   /* PID */
    DEVICE_VERSION_IDENTIFIER,   /* VID */
    DEVICE_CRPL,                 /* capacity of reply protection list */
    DEVICE_FEATURE,              /* Features */
    3,                           /* Num of elements */
    server_element_list          /* Element list */
};

/*****************************************************************************
 * Sample Composition Data for Client Models
******************************************************************************/
/*
 * Primary element for Light HSL Client
 */
uint16_t client_sig_model_ids_elm_light_hsl[] =  {QMESH_CONFIG_SERVER_MODEL_ID,
                                                   QMESH_CONFIG_CLIENT_MODEL_ID,
                                                   QMESH_HEALTH_SERVER_MODEL_ID,
                                                   QMESH_MODEL_GENERIC_ONOFF_CLIENT,
                                                   QMESH_MODEL_GENERIC_POWERONOFF_CLIENT,
                                                   QMESH_MODEL_GENERIC_LEVEL_CLIENT,
                                                   QMESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT,
                                                   QMESH_MODEL_GENERIC_POWERLEVEL_CLIENT,
                                                   QMESH_MODEL_LIGHT_LIGHTNESS_CLIENT,
                                                   QMESH_MODEL_LIGHT_HSL_CLIENT};

uint32_t client_vendor_model_ids[] =  {QMESH_VENDOR_MODEL_CLIENT};

/* Create App Key Lists for all Client Models Statically. */
CREATE_STATIC_APPKEY_LIST(cl_config_server, QMESH_CONFIG_MODEL_KEYLIST_SIZE);
CREATE_STATIC_APPKEY_LIST(cl_health_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(config_client, QMESH_CONFIG_MODEL_KEYLIST_SIZE);
CREATE_STATIC_APPKEY_LIST(gen_onoff_client, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(gen_level_client, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(gen_power_onoff_client, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(gen_power_level_client, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(light_lightness_client, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(light_hsl_client, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(gen_default_tt_client, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(vendor_client, QMESH_MAX_APP_KEYS_LIST);

/* Create Subscription Lists for all Client Models Statically. */
CREATE_STATIC_SUBS_LIST(cl_health_server, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(gen_onoff_client, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(gen_level_client, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(gen_power_onoff_client, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(gen_power_level_client, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(light_lightness_client, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(light_hsl_client, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(gen_default_tt_client, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(vendor_client, QMESH_MAX_SUB_ADDRS_LIST);

QMESH_MODEL_PUBLISH_STATE_T client_elm_light_publish_state[] =
{
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* *publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                       /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        NULL                        /* Config handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* *publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                       /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        NULL                        /* Config handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        QmeshHealthServerPublicationHandler /* publication handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* *publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                       /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        NULL                        /* Config handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* *publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                       /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        NULL                        /* Config handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* *publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                       /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        NULL                        /* Config handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* *publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                       /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        NULL                        /* Config handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* *publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                       /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        NULL                        /* Config handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* *publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                       /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        NULL                        /* Config handler */
    },
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* *publish_uuid */
        FALSE,                      /* use_frnd_key */
        0x30,                       /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        NULL                        /* Config handler */
    }
};

QMESH_MODEL_PUBLISH_STATE_T client_vendor_model_publish_state[] =
{
    {
        QMESH_INVALID_KEY_INDEX,    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,   /* publish_addr */
        NULL,                       /* *publish_uuid */
        FALSE,                      /* use_frnd_key */
        PUBLISH_TTL,                       /* publish_ttl */
        {0,0},                      /* publish_period */
        {0,0},                      /* retransmit_param */
        NULL                        /* Config handler */
    }
};

QMESH_MODEL_DATA_T client_elem_light_model_data[] =
{
    /* Initialize the Config model data */
    {
        &client_elm_light_publish_state[0],      /* publish_state */
        QMESH_LIST_HANDLE_INVALID,      /* subs_list */
        GET_STATIC_APPKEY_LIST(cl_config_server),   /* app_key_list */
        NULL,                                        /* model_priv_data */
        QmeshConfigModelServerHandler,               /* model_handler */
    },
    {
        &client_elm_light_publish_state[1],             /* publish_state */
        QMESH_LIST_HANDLE_INVALID,             /* subs_list */
        GET_STATIC_APPKEY_LIST(config_client),    /* app_key_list */
        NULL,                                           /* model_priv_data */
        QmeshConfigModelClientHandler,                  /* model_handler */
    },
    /* Initialize the Health model data */
    {
        &client_elm_light_publish_state[2],      /* publish_state */
        GET_STATIC_SUBS_LIST(cl_health_server),      /* subs_list */
        GET_STATIC_APPKEY_LIST(cl_health_server),       /* app_key_list */
        &g_health_server_state,                      /* model_priv_data */
        QmeshHealthModelServerHandler,               /* model_handler */
    },
    {
        &client_elm_light_publish_state[3],             /* publish_state */
        GET_STATIC_SUBS_LIST(gen_onoff_client),             /* subs_list */
        GET_STATIC_APPKEY_LIST(gen_onoff_client),    /* app_key_list */
        NULL,                                           /* model_priv_data */
        QmeshGenericOnOffClientHandler,                 /* model_handler */
    },
    {
        &client_elm_light_publish_state[4],             /* publish_state */
        GET_STATIC_SUBS_LIST(gen_power_onoff_client),             /* subs_list */
        GET_STATIC_APPKEY_LIST(gen_power_onoff_client),    /* app_key_list */
        NULL,                                           /* model_priv_data */
        QmeshGenericPowerOnOffClientHandler,            /* model_handler */
    },
    {
        &client_elm_light_publish_state[5],             /* publish_state */
        GET_STATIC_SUBS_LIST(gen_level_client),             /* subs_list */
        GET_STATIC_APPKEY_LIST(gen_level_client),    /* app_key_list */
        NULL,                                           /* model_priv_data */
        QmeshGenericLevelClientHandler,                 /* model_handler */
    },
    {
        &client_elm_light_publish_state[6],             /* publish_state */
        GET_STATIC_SUBS_LIST(gen_default_tt_client),             /* subs_list */
        GET_STATIC_APPKEY_LIST(gen_default_tt_client),    /* app_key_list */
        NULL,                                           /* model_priv_data */
        QmeshGenericDefaultTransitionTimeClientHandler,  /* model_handler */
    },
    {
        &client_elm_light_publish_state[7],             /* publish_state */
        GET_STATIC_SUBS_LIST(gen_power_level_client),             /* subs_list */
        GET_STATIC_APPKEY_LIST(gen_power_level_client),    /* app_key_list */
        NULL,                                           /* model_priv_data */
        QmeshGenericPowerLevelClientHandler,            /* model_handler */
    },
    {
        &client_elm_light_publish_state[8],             /* publish_state */
        GET_STATIC_SUBS_LIST(light_lightness_client),             /* subs_list */
        GET_STATIC_APPKEY_LIST(light_lightness_client),    /* app_key_list */
        NULL,                                           /* model_priv_data */
        QmeshLightLightnessClientHandler,               /* model_handler */
    },
    {
        &client_elm_light_publish_state[9],             /* publish_state */
        GET_STATIC_SUBS_LIST(light_hsl_client),             /* subs_list */
        GET_STATIC_APPKEY_LIST(light_hsl_client),    /* app_key_list */
        NULL,                                           /* model_priv_data */
        QmeshLightHslClientHandler,                     /* model_handler */
    },
    {
        &client_vendor_model_publish_state[0],             /* publish_state */
        GET_STATIC_SUBS_LIST(vendor_client),             /* subs_list */
        GET_STATIC_APPKEY_LIST(vendor_client),    /* app_key_list */
        NULL,                                           /* model_priv_data */
        QmeshVendorClientHandler,                     /* model_handler */
    }
};

QMESH_ELEMENT_DATA_T client_element_light_data =
{
    0x014820u,                          /* seq_num */
    0x0465,                             /* unicast_addr */
    client_elem_light_model_data        /* model_data */
};

/*
 * Element List
 */
const QMESH_ELEMENT_CONFIG_T client_ele_list[] =
{
    {
        0x20,                                /* Loc */
        sizeof(client_sig_model_ids_elm_light_hsl) / sizeof(uint16_t),     /* Num of sig models */
        sizeof(client_vendor_model_ids) / sizeof(uint32_t),       /* Num of vendor models */
        client_sig_model_ids_elm_light_hsl,  /* Sig model ids */
        client_vendor_model_ids, /* Vendor model ids */
        &client_element_light_data           /* element_data */
    }
};

/*
 * Composition Data with 1 element
 */
const QMESH_DEVICE_COMPOSITION_DATA_T client_device_composition =
{
    DEVICE_COMPANY_IDENTIFIER,   /* CID */
    DEVICE_PRODUCT_IDENTIFIER,   /* PID */
    DEVICE_VERSION_IDENTIFIER,   /* VID */
    DEVICE_CRPL,                 /* capacity of reply protection list */
    DEVICE_FEATURE,              /* Features */
    1,                           /* Num of elements */
    client_ele_list              /* Element list */
};

void DemoCompAppKeyListInit(void)
{
    INIT_STATIC_APPKEY_LIST(config_server);
    INIT_STATIC_APPKEY_LIST(health_server);
    INIT_STATIC_APPKEY_LIST(gen_onoff_server);
    INIT_STATIC_APPKEY_LIST(gen_level_server);
    INIT_STATIC_APPKEY_LIST(gen_power_onoff_server);
    INIT_STATIC_APPKEY_LIST(gen_power_onoff_setup_server);
    INIT_STATIC_APPKEY_LIST(light_lightness_server);
    INIT_STATIC_APPKEY_LIST(light_lightness_setup_server);
    INIT_STATIC_APPKEY_LIST(light_hsl_server);
    INIT_STATIC_APPKEY_LIST(light_hsl_setup_server);
    INIT_STATIC_APPKEY_LIST(gen_default_tt_server);
    INIT_STATIC_APPKEY_LIST(hue_server);
    INIT_STATIC_APPKEY_LIST(hue_gen_level_server);
    INIT_STATIC_APPKEY_LIST(saturation_server);
    INIT_STATIC_APPKEY_LIST(saturation_gen_level_server);
    INIT_STATIC_APPKEY_LIST(vendor_server);

    QmeshAddToList(GET_STATIC_APPKEY_LIST(config_server), QMESH_SELF_DEVICE_KEY_IDX);

    INIT_STATIC_APPKEY_LIST(cl_config_server);
    INIT_STATIC_APPKEY_LIST(cl_health_server);
    INIT_STATIC_APPKEY_LIST(config_client);
    INIT_STATIC_APPKEY_LIST(gen_onoff_client);
    INIT_STATIC_APPKEY_LIST(gen_level_client);
    INIT_STATIC_APPKEY_LIST(gen_power_onoff_client);
    INIT_STATIC_APPKEY_LIST(gen_power_level_client);
    INIT_STATIC_APPKEY_LIST(light_lightness_client);
    INIT_STATIC_APPKEY_LIST(light_hsl_client);
    INIT_STATIC_APPKEY_LIST(gen_default_tt_client);
    INIT_STATIC_APPKEY_LIST(vendor_client);

    QmeshAddToList(GET_STATIC_APPKEY_LIST(cl_config_server), QMESH_SELF_DEVICE_KEY_IDX);
    QmeshAddToList(GET_STATIC_APPKEY_LIST(config_client), QMESH_PEER_DEVICE_KEY_IDX);
}

void DemoCompSubsListInit(void)
{
    INIT_STATIC_SUBS_LIST(config_server);
    INIT_STATIC_SUBS_LIST(health_server);
    INIT_STATIC_SUBS_LIST(gen_power_onoff_server);
    INIT_STATIC_SUBS_LIST(light_lightness_server);
    INIT_STATIC_SUBS_LIST(hue_server);
    INIT_STATIC_SUBS_LIST(saturation_server);
    INIT_STATIC_SUBS_LIST(gen_default_tt_server);
    INIT_STATIC_SUBS_LIST(cl_health_server);
    INIT_STATIC_SUBS_LIST(vendor_server);

    INIT_STATIC_SUBS_LIST(gen_onoff_client);
    INIT_STATIC_SUBS_LIST(gen_level_client);
    INIT_STATIC_SUBS_LIST(gen_power_onoff_client);
    INIT_STATIC_SUBS_LIST(gen_power_level_client);
    INIT_STATIC_SUBS_LIST(light_lightness_client);
    INIT_STATIC_SUBS_LIST(light_hsl_client);
    INIT_STATIC_SUBS_LIST(gen_default_tt_client);
    INIT_STATIC_SUBS_LIST(vendor_client);
}

uint16_t DemoGetConfigClientElemAddr(void)
{
    return client_element_light_data.unicast_addr;
}

