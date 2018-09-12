/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*
  * This file contains Composition Data of the Server node device
 */
/*****************************************************************************/
#include "qmesh_demo_composition.h"
#include "qmesh_model_common.h"
#include "qmesh_aksl_list.h"
#include "qmesh_health_server.h"
#include "qmesh_demo_server_config.h"
#include "qmesh_config_server.h"
//#include "model_server_event_handler.h"

/*
 * List of Models supported
 */
const uint16_t server_sig_model_ids[] =  {
                                         QMESH_CONFIG_SERVER_MODEL_ID,
                                         QMESH_HEALTH_SERVER_MODEL_ID,
                                         QMESH_GENERIC_ONOFF_SERVER_MODEL_ID
                               };

/* The subscription list for the models.
     QMESH_MAX_SUB_ADDRS_LIST - maximum number of subscription addresses supported per model */
uint16_t server_model_sub_list[3][QMESH_MAX_SUB_ADDRS_LIST] = {{QMESH_UNASSIGNED_ADDRESS}};

/* Context information specific to a model. This contains the state data relevant to the model */
QMESH_HEALTH_SERVER_STATE_T   g_health_server_state;


/* Test array intialized with some ramdom test error code */
uint8_t test_array[] = {0x12, 0x13};

/* Health model data.
     Intialized Current Fault / Registerd Fault used by the application */
QMESH_HEALTH_MODEL_DATA_T current_fault = {
                                            0x01, /* test id */ 
                                            0x02, /* test arry len */
                                            test_array, /* test array ptr */
                                            0x00 /* attention state */
                                           };

/* Create App Key Lists for all supported Models Statically. */
CREATE_STATIC_APPKEY_LIST(config_server, QMESH_CONFIG_MODEL_KEYLIST_SIZE);
CREATE_STATIC_APPKEY_LIST(health_server, QMESH_MAX_APP_KEYS_LIST);
CREATE_STATIC_APPKEY_LIST(gen_onoff_server, QMESH_MAX_APP_KEYS_LIST);

/* Create Subscription Lists for the bound server models. */
CREATE_STATIC_SUBS_LIST(config_server, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(health_server, QMESH_MAX_SUB_ADDRS_LIST);
CREATE_STATIC_SUBS_LIST(gen_onoff_server, QMESH_MAX_SUB_ADDRS_LIST);


/* On bootup used in PowerOnOff Server model */
uint8 onbootpowerup = 0;

/* Array holding the Publish data for each of the models supported */
QMESH_MODEL_PUBLISH_STATE_T server_publish_state[] =
{
    /* Publish data for Config Server model */
    {
        QMESH_INVALID_KEY_INDEX,                /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,          /* publish_addr */
        NULL,                                                    /* publish_uuid */
        FALSE,                                                  /* use_frnd_key */
        PUBLISH_TTL,                                       /* publish_ttl */
        {0,0},                                                  /* publish_period */
        {0,0},                                                  /* retransmit_param */
        NULL                                                    /* publication handler  */
    },
    /* Publish data for Health Server model */
    {
        QMESH_INVALID_KEY_INDEX,                    /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,              /* publish_addr */
        NULL,                                                        /* publish_uuid */
        FALSE,                                                       /* use_frnd_key */
        PUBLISH_TTL,                                            /* publish_ttl */
        {0,0},                                                        /* publish_period */
        {0,0},                                                        /* retransmit_param */
        QmeshHealthServerPublicationHandler  /* publication handler  */
    },
    /* Publish data for GenericOnOff Server model */
    {
        QMESH_INVALID_KEY_INDEX,                /* app_key_idx */
        QMESH_UNASSIGNED_ADDRESS,          /* publish_addr */
        NULL,                                                    /* publish_uuid */
        FALSE,                                                  /* use_frnd_key */
        PUBLISH_TTL,                                       /* publish_ttl */
        {0,0},                                                  /* publish_period */
        {0,0},                                                  /* retransmit_param */
        QmeshGenericOnOffPublicationHandler  /* publication handler  */
    }
};

/* Array holding the model data for each of the models supported */
QMESH_MODEL_DATA_T server_model_data[] =
{
    /* Initialize the Config server model data */
    {
        &server_publish_state[0],                          /* publish_state */
        GET_STATIC_SUBS_LIST(config_server),          /* subs_list */
        GET_STATIC_APPKEY_LIST(config_server),      /* app_key_list */
        NULL,                                                                    /* model_priv_data */
        QmeshConfigModelServerHandler,                    /* model_handler */
    },
    /* Initialize the Health model data */
    {
        &server_publish_state[1],                      /* publish_state */
        GET_STATIC_SUBS_LIST(health_server),      /* subs_list */
        GET_STATIC_APPKEY_LIST(health_server),  /* app_key_list */
        &g_health_server_state,                                /* model_priv_data */
        QmeshHealthModelServerHandler,                /* model_handler */
    },
    /* Initialize the Generic OnOff server model data */
    {
        &server_publish_state[2],                        /* publish_state */
        GET_STATIC_SUBS_LIST(gen_onoff_server),             /* subs_list */
        GET_STATIC_APPKEY_LIST(gen_onoff_server),    /* app_key_list */
        &g_onoff_model_context,                                                                 /* model_priv_data */
        QmeshGenericOnOffServerHandler,                   /* model_handler */
    }
};

/* Element data on the Server */
QMESH_ELEMENT_DATA_T server_element_data =
{
    START_SEQUENCE_NUMBER,                              /* seq_num */
    LOCAL_UNICAST_ADDRESS,                               /* unicast address */
    server_model_data                                          /* model_data */
};

/* Array holding the element list on the device */
const QMESH_ELEMENT_CONFIG_T server_element_list[NUMER_OF_ELEMENT] =
{
    {
        ELEMENT_LOCATION_FRONT,                        /* Location of the element */
        /* Num of sig models */
        sizeof(server_sig_model_ids)  / sizeof(uint16_t),  
        0,                                                                 /* Num of vendor models */
        server_sig_model_ids,                       /* Sig model ids */
        NULL,                                                           /* Vendor model ids */
        &server_element_data                      /* element_data */
    }
};

/*
 * Composition Data with one element
 */
const QMESH_DEVICE_COMPOSITION_DATA_T server_device_composition =
{ 
    DEVICE_COMPANY_IDENTIFIER,   /* CID */
    DEVICE_PRODUCT_IDENTIFIER,   /* PID */
    DEVICE_VERSION_IDENTIFIER,    /* VID */
    DEVICE_CRPL,                            /* capacity of reply protection list */
    DEVICE_FEATURE,                      /* Features */
    NUMER_OF_ELEMENT,                /* Num of elements */
    server_element_list                  /* Element list */
};

/* Context information specific to a model. This contains the state data relevant to the model */
QMESH_GENERIC_LEVEL_CONTEXT_T g_gen_lvl_context[3];
QMESH_GENERIC_ONOFF_MODEL_CONTEXT_T g_onoff_model_context;
QMESH_GEN_POWERONOFF_DATA_T g_pwronoff_context;
QMESH_GENERIC_DTT_CONTEXT_T g_gen_dtt_context;
QMESH_LIGHTNESS_MODEL_CONTEXT_T g_lightness_server_context;
QMESH_LIGHT_HSL_MODEL_CONTEXT_T g_hsl_server_context; 
QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T    g_hsl_hue_server_context;
QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T    g_hsl_sat_server_context;

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
void DemoCompAppKeyListInit(void)
{
    INIT_STATIC_APPKEY_LIST(config_server);
    INIT_STATIC_APPKEY_LIST(health_server);
    INIT_STATIC_APPKEY_LIST(gen_onoff_server);

    QmeshAddToList(GET_STATIC_APPKEY_LIST(config_server), QMESH_SELF_DEVICE_KEY_IDX);
}

/*----------------------------------------------------------------------------*
 * DemoCompSubsListInit
 */
/*! \brief Creates subscription list for all the supported models
 *
 *  \param none
 *
 *  \returns None.
 */
/*---------------------------------------------------------------------------*/
void DemoCompSubsListInit(void)
{
    INIT_STATIC_SUBS_LIST(config_server);
    INIT_STATIC_SUBS_LIST(health_server);
    INIT_STATIC_SUBS_LIST(gen_onoff_server);
}

/*----------------------------------------------------------------------------*
 * DemoGetConfigClientElemAddr
 */
/*! \brief Returns the Unicast address of the Client device.
 *
 *  \param none
 *
 *  \returns Unicast address of this device.
 */
/*---------------------------------------------------------------------------*/
uint16_t DemoGetConfigClientElemAddr(void)
{
    return server_element_data.unicast_addr;
}
