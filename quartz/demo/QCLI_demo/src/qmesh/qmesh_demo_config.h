/******************************************************************************
Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/

#ifndef __QMESH_DEMO_CONFIG_H__
#define __QMESH_DEMO_CONFIG_H__

/* Proxy feature bit in the Features field of Composition data. */
#define PROXY_FEATURE_MASK 0x2

#define QCLI_DFLT_TTL  10
#define DEBUG   1

#ifdef DEBUG
//#define DEBUG_GAP_GATT_BEARER 
#endif

#define QMESH_PS_IFCE_ENABLED 1
/******************************************************************************
 * GATT And Bearer Data
 *****************************************************************************/

#define MAX_NUM_OF_PROVISIONED_NETWORK                1
#define MAX_NUM_OF_SUB_NETWORK                        4  /* 4 Network Keys */
#define MAX_NUM_OF_APP_KEY_PER_NETWORK                4  /* 4 App Keys Per Network */
#define GATT_PROXY_SRVC_CONN_ADVERT_INTERVAL_IN_MS    2 * 1000  /* 2 Seconds  */
#define GATT_PROXY_SRVC_CONN_ADVERT_DURATION          60 * 1000 /* 60 Seconds */
#define UNPROV_CONN_ADVRT_DISCOVERY_DUARTION          30 * 1000 /* 30 Seconds */
#define PROVISIONER_ROLE  0
#define NODE_ROLE         1

/*
  The MTU value chosen here need to be between QAPI_BLE_ATT_PROTOCOL_MTU_MINIMUM_LE
  and QAPI_BLE_ATT_PROTOCOL_MTU_MAXIMUM
*/
#define PROV_MAX_ATT_MTU_SIZE                         69
#define PROX_MAX_ATT_MTU_SIZE                         33


#define LE_SCAN_INTERVAL                              (uint32_t)(0x64)
#define LE_SCAN_WINDOW                                (uint32_t)(0x3C)
#define UNPROVISIONED_DEVICE_FOUND_LIST               20
#define PROXY_DEVICE_FOUND_LIST                       20

#define QMESH_MAX_NUM_OF_PROVISIONABLE_DEVICES (3)

#define MAX_NUM_OF_GATT_CONN                          4  /* 4 GATT Connections (Should be same as
                                                          * QMESH_MAX_PROXY_BEARERS in QMesh 
                                                          * Stack support ) */
#define MAX_NUM_OF_ADV_BEARER                         2  /* 2 ADV Bearer (Provisioning and Network) */

/* Some MACROs for accessing little-endian unaligned values.         */
#define READ_UNALIGNED_BYTE_LITTLE_ENDIAN(_x) (((uint8_t *)(_x))[0])
#define READ_UNALIGNED_WORD_LITTLE_ENDIAN(_x) \
  ((uint16_t)((((uint16_t)(((uint8_t *)(_x))[1])) << 8) | \
  ((uint16_t)(((uint8_t *)(_x))[0]))))

#define ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(_x, _y)        \
{                                                                       \
  ((uint8_t *)(_x))[0] = ((uint8_t)(((uint16_t)(_y)) & 0xFF));          \
  ((uint8_t *)(_x))[1] = ((uint8_t)((((uint16_t)(_y)) >> 8) & 0xFF));   \
}


/* Advertisement Timer for sending device identification */
#define UNPROV_DEV_BEACON_TIME (1 * QMESH_SECOND)
#define URI_HASH_LEN (4)

#define QCLI_ARR_LOGI(m, a, l) \
    {\
        int i;\
        QCLI_LOGI (mesh_group, " [");\
        for (i = 0; i < l; i++ )\
        {\
            QCLI_LOGI (m, "%d ", a[i]);\
        }\
        QCLI_LOGI (mesh_group, " ]\n");\
    }\

#endif