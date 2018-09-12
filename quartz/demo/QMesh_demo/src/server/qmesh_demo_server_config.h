/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*
  * This file contains various configurations parameters for the Server node application
 */
/*****************************************************************************/
#ifndef __QMESH_DEMO_SERVER_CONFIG_H__
#define __QMESH_DEMO_SERVER_CONFIG_H__

#include "qmesh_types.h"

/* Maximum number of mesh network */
#define QMESH_APP_MAX_NETWORKS          (1)
/* SIG defined model ID value for Generic OnOff Server Model */
#define QMESH_GENERIC_ONOFF_SERVER_MODEL_ID                         (0x1000)
/* SIG defined model ID value for Generic OnOff Client Model */
#define QMESH_GENERIC_ONOFF_CLIENT_MODEL_ID                          (0x1001)
/* Primary Element address of Client device to provision */
#define QMESH_DEMO_CLIENT_ELEMENT_ADDRESS                              (0x004E)
/* Primary Element address of Server device to provision */
#define QMESH_DEMO_SERVER_ELEMENT_ADDRESS                             (0x004F)

/* Device type used while provisioning */
#define QMESH_APP_REMOTE_DEVICE_TYPE_CLIENT                             (0)
#define QMESH_APP_REMOTE_DEVICE_TYPE_SERVER                         (1)

/* Proxy feature bit in the Features field of Composition data. */
#define PROXY_FEATURE_MASK                                          0x2
/* Default TTL value used for sending mesh packets */
#define QCLI_DFLT_TTL                                                       10
/* Flag to enable logging */
#define DEBUG                                                                       1
/* Flag to enable persistent store to store mesh data */
#define QMESH_PS_IFCE_ENABLED                                    0
/* Maximum number of sub network supported */
#define MAX_NUM_OF_SUB_NETWORK                              1  /* 1 Network */
#define NODE_ROLE                                                              1
/* Maximum number of devices this device can provision */
#define QMESH_MAX_NUM_OF_PROVISIONABLE_DEVICES (2)
/* Attention duration during provisioning in seconds */
#define QMESH_APP_ATTENTION_DURATION                (10) /* In seconds */
/* Device instance value */
#define DEVICE_INSTANCE                                                 (0)

/*
  The MTU value of packet over GATT bearer. The value need to be between 
  QAPI_BLE_ATT_PROTOCOL_MTU_MINIMUM_LE and QAPI_BLE_ATT_PROTOCOL_MTU_MAXIMUM
*/
#define PROV_MAX_ATT_MTU_SIZE                         69
#define PROX_MAX_ATT_MTU_SIZE                         33
#endif
