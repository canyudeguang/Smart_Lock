/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_db.h
 *  \brief Qmesh Database Structure.
 *
 *   This file contains the Qmesh Database Structures and interface.
 */
 /*****************************************************************************/

#ifndef _QMESH_DB_H_
#define _QMESH_DB_H_

#include "qmesh_private.h"

/*! \brief List Handle. */
/*!< \brief Last successfully processed seq auth */
/*----------------------------------------------------------------------------*
 * <<Function>>
 */
/*! \brief <<description>>
 *
 *  \param <<param>>    <<param description>>
 *
 *  \returns <<return value>>
 */
/*---------------------------------------------------------------------------*/

/*============================================================================*
    Macro Definitions
 *============================================================================*/
#define QMESH_DB_ITERATOR_INVALID   ((QMESH_DB_ITERATOR_T)NULL)

#define ADDRESS_TYPE_FULL_MASK (0xC000)
#define VIRTUAL_ADDRESS_FULL_MASK (0x8000)

/* Test for virtual address. */
#define IS_VIRTUAL_ADDRESS(addr) (((addr) & ADDRESS_TYPE_FULL_MASK) == VIRTUAL_ADDRESS_FULL_MASK)

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/

/*============================================================================*
    Data Type Definitions
 *============================================================================*/
typedef void* QMESH_DB_ITERATOR_T;

typedef struct
{
    uint16_t    lpn_addr;           /*!< LPN Primary Element Address. */
    uint16_t    lpn_counter;        /*!< LPN Counter. */
    uint16_t    frnd_addr;          /*!< FRND Primary Element Address. */
    uint16_t    frnd_counter;       /*!< FRND Counter. */
} QMESH_FRND_KEY_INFO_T;

/*! Virtual address label uuid mapping table */
typedef struct
{
    uint16_t     reference_count;   /*!< \brief Indicates if any model is currently subscribed
                                     *          to this virtual address.
                                     */
    uint16_t     virtual_addr;      /*!< \brief Virtual address */
    QMESH_LABEL_UUID_T  label_uuid; /*!< \brief 128 bit Label UUID. */
} QMESH_VIRTUAL_ADDR_TABLE_T;

typedef struct
{
    uint16_t                    netkey_idx;
    uint8_t                     aid;
    uint8_t                     idx;
    uint16_t                    appkey_idx;
    QMESH_APPKEY_DATA_T       **appkey_list;
    const uint16_t             *app_key;
} QMESH_APPKEY_SEARCH_CNTXT_T;

/*============================================================================*
    API Definitions
 *============================================================================*/
// DB
QMESH_RESULT_T QmeshDBInit(uint8_t max_dev_inst, QMESH_APP_CB_T app_cb);

void QmeshDBDeInit(void);

// Device
QMESH_RESULT_T QmeshDBCreateDeviceInstance(uint8_t dev_instance, QMESH_ROLE_T role,
                                           const QMESH_DEVICE_IDENTIFIER_DATA_T *dev_data,
                                           const QMESH_DEVICE_COMPOSITION_DATA_T *comp_data);

QMESH_RESULT_T QmeshDBSetRole(uint8_t dev_instance, QMESH_ROLE_T role);

QMESH_RESULT_T QmeshDBGetRole(uint8_t dev_instance, QMESH_ROLE_T *role);

bool QmeshDBIsDeviceProvisioned(uint8_t dev_instance);

bool QmeshDBIsProvisioningAllowed(uint8_t dev_instance);

const QMESH_DEVICE_IDENTIFIER_DATA_T *QmeshDBGetDeviceIdData(uint8_t dev_instance);

const QMESH_DEVICE_COMPOSITION_DATA_T *QmeshDBGetCompositionData(uint8_t dev_instance);

QMESH_RESULT_T QmeshDBDestrtoyDeviceInstance(uint8_t dev_instance);

// Device Key
QMESH_RESULT_T QmeshDBSetDeviceKey(uint8_t net_idx, const QMESH_DEVICE_KEY_T dev_key);

const uint16_t *QmeshDBGetDeviceKey(uint8_t net_idx);

uint16_t QmeshDBGetPrimaryElementAddress(uint8_t net_idx);

uint16_t QmeshDBGetNumElements(uint8_t dev_instance);

QMESH_RESULT_T QmeshDBSetElementAddr(uint8_t net_idx, uint16_t prim_addr);

bool QmeshDBIsOwnElementAddress(uint8_t net_idx, uint16_t addr);

// Create a Network.
QMESH_RESULT_T QmeshDBCreateNetwork(uint8_t net_idx);

// Delete a Network.
QMESH_RESULT_T QmeshDBDeleteNetwork(uint8_t net_idx);

// NetKey List Procedures.
/*
 * Network Key List Operations:
 * 1. Add a network key:
 *      a. Check if the network key index already exists.
 *      b. Otherwise add network key.
 * 2. Update a network key:
 *      a. Check if the network key index already exists.
 *      b. Update the network key by adding a new NetKey at same NetKey Index and update the state.
 * 3. Delete a network key:
 *      a. Check if the network key index already exists.
 *      b. Check if there is atleast one another network key.
 *      c. Remove the network key and associated appkeys.
 * 4. Get List of network keys.
 * 5. RX Path:
 *      Find a network key which matches NID and also successfully authenticates the network packet.
 * 6. TX Path:
 *      Find a network key which matches the NetKey Index and state is appropriate.
 * 7. Beacons:
 *      Find the matching Network ID for the Received Beacon.
 */

const QMESH_SUBNET_KEY_INFO_T *QmeshDBGetNetKeyInfo(QMESH_SUBNET_KEY_IDX_INFO_T subnet_key_idx, uint16_t peer_addr);

QMESH_RESULT_T QmeshDBAddNetKey(const QMESH_SUBNET_ADD_INFO_T *subnet_keyinfo);

QMESH_RESULT_T QmeshDBUpdateNetKey(const QMESH_SUBNET_ADD_INFO_T *subnet_keyinfo);

QMESH_RESULT_T QmeshDBDeleteNetKey(QMESH_SUBNET_KEY_IDX_INFO_T subnet_key_idx);

QMESH_RESULT_T QmeshDBGetNetKeyList(uint8_t net_idx, uint8_t *key_list, uint16_t *num_keys);

bool QmeshDBIsValidSubnet(QMESH_SUBNET_KEY_IDX_INFO_T subnet_key_idx);

QMESH_RESULT_T QmeshDBGetNetKey(QMESH_SUBNET_KEY_IDX_INFO_T subnet_idx,
                                QMESH_SUBNET_ADD_INFO_T *subnet_data);


// Subnetwork Procedures.
/*
 * Subnetwork data operations:
 *  1. Key Refresh Phase Get/Set.
 *  2. Node Identity
 *  3. Beacon Informartion and timing.
 */
QMESH_RESULT_T QmeshDBKeyRefreshPhaseGet(QMESH_SUBNET_KEY_IDX_INFO_T subnet_key_idx, QMESH_KEY_REFRESH_PHASE_T *kr_phase);

QMESH_RESULT_T QmeshDBKeyRefreshPhaseSet(QMESH_SUBNET_KEY_IDX_INFO_T subnet_key_idx, QMESH_KEY_REFRESH_PHASE_T kr_phase);

QMESH_RESULT_T QmeshDBNodeIdentityGet(QMESH_SUBNET_KEY_IDX_INFO_T subnet_key_idx, QMESH_NODE_IDENTITY_T *node_identity);

QMESH_RESULT_T QmeshDBNodeIdentitySet(QMESH_SUBNET_KEY_IDX_INFO_T subnet_key_idx, QMESH_NODE_IDENTITY_T node_identity);

// TODO: Beacon Informartion and its timing.

// AppKey List Procedures.
/*
 * Application Key List Operations:
 * 1. Add an Application key:
 *      a. Check if the AppKey index already exists.
 *      b. Otherwise add Application key.
 * 2. Update a Application key:
 *      a. Check if the application key index already exists.
 *      b. Update the application key by adding a new AppKey at same AppKey Index and update the state.
 * 3. Delete an Application key:
 *      a. Check if the Application key index already exists.
 *      c. Remove the application key and their associated bindings.
 * 4. Get List of application keys for a network key.
 * 5. RX Path:
 *      Find a application key which matches AID, Network Key Index and also successfully authenticates the Upper Transport Layer packet.
 * 6. TX Path:
 *      Find a application key which matches the AppKey Index and state is appropriate.
 * 7. Delete all app keys bound to a netkey.
 */

const QMESH_AID_APPKEY_T *QmeshDBGetAppKey(uint8_t net_idx, uint16_t appkey_idx,
                                           uint16_t *subnet_idx);

QMESH_RESULT_T QmeshDBAddAppKey(const QMESH_APPKEY_ADD_INFO_T *app_key);

QMESH_RESULT_T QmeshDBUpdateAppKey(const QMESH_APPKEY_ADD_INFO_T *app_key);

QMESH_RESULT_T QmeshDBUpdateAppKeyFromPS(const QMESH_APPKEY_ADD_INFO_T *app_key);

QMESH_RESULT_T QmeshDBDeleteAppKey(QMESH_SUBNET_KEY_IDX_INFO_T net_key_idx, uint16_t app_key_idx);

QMESH_RESULT_T QmeshDBGetAppKeyList(QMESH_SUBNET_KEY_IDX_INFO_T subnet_key_idx, uint8_t *appkey_list, uint8_t *num_keys);

void QmeshDBInitAppKeySearchCntxt(uint8_t net_idx, QMESH_APPKEY_SEARCH_CNTXT_T *appkey_search);

void QmeshDBGetNextAppKey(QMESH_APPKEY_SEARCH_CNTXT_T *appkey_search);

void QmeshDBDeInitAppKeySearchCntxt(QMESH_APPKEY_SEARCH_CNTXT_T *appkey_search);

// IV

/*
 * IV Update State Machine.
 *  1. NORMAL => IV UPDATE IN PROGRESS
 *     TRIGGERS:
 *      a. SEQ exhaustion and has Primary Subnet => Tirgger IV Update using QemshIncrementIV locally.
 *      b. SNB Received with incremented IV.
 *      c. PTS test mode.
 *     CONDITIONS:
 *      a. Was in NORMAL State for 96hrs atleast.
 *      b. Has Primary => SNB with new IV on primary.
 *      c. No Primary  => SNB on subnet.
 *   2. IV UPDATE IN PROGRESS => NORMAL
 *     TRIGGERS:
 *      a. 144hr timer expried while state is IV UPDATE IN PROGRESS.
 *      b. SNB Received with incremented IV and state NORMAL.
 *      c. PTS test mode.
 *     CONDITIONS:
 *      a. Was in IV UPDATE PROGRESS State for 96hrs atleast.
 *      b. No pending Segmented Message Transmissions.
 *  3. IV RECOVERY PROCEDURE:
 *      a. Received SNB on Primary network.
 *      b. Node is part of primary network.
 *      c. In IV Recovery State.
 */

uint32_t QmeshDBGetCurrentIV(uint8_t net_idx);

QMESH_IV_UPDATE_STATE_T QmeshDBGetIVUpdateState(uint8_t net_idx);

QMESH_RESULT_T QmeshDBSetIV(uint8_t net_idx, uint32_t iv_index, bool no_time_check, QMESH_IV_UPDATE_STATE_T state);

uint32_t QmeshDBGetCurrentTransmitIV(uint8_t net_idx);

QMESH_RESULT_T QmeshDBSetIVRecoveryMode(uint8_t net_idx, bool enable);

bool QmeshDBIsPrimaryNetwork(uint8_t net_idx);

void QmeshDBUpdateDeferedIVState(uint8_t net_idx);

// Network Transmit State
QMESH_RESULT_T QmeshDBGetNetworkTransmitState(uint8_t net_idx, QMESH_TRANSMIT_STATE_T *nw_state);

QMESH_RESULT_T QmeshDBSetNetworkTransmitState(uint8_t net_idx, QMESH_TRANSMIT_STATE_T nw_state);

// Beacon State
QMESH_RESULT_T QmeshDBGetBeaconState(uint8_t net_idx, QMESH_SECURE_NW_BEACON_STATE_T *beacon_state);

QMESH_RESULT_T QmeshDBSetBeaconState(uint8_t net_idx, QMESH_SECURE_NW_BEACON_STATE_T beacon_state);

// Default TTL
QMESH_RESULT_T QmeshDBGetDefaultTTL(uint8_t net_idx, uint8_t *default_ttl);

QMESH_RESULT_T QmeshDBSetDefaultTTL(uint8_t net_idx, uint8_t default_ttl);

// Heartbeat - HB Subscription and Publishing Files need to be cleaned-up.
uint16_t QmeshDBGetActiveFeatureMask(uint8_t net_idx, uint16_t net_key_idx);

uint16_t QmeshDBGetFeatureMask(uint8_t net_idx);

QMESH_RESULT_T QmeshDBGetHeartBeatPublishState(uint8_t net_idx, QMESH_HEARTBEAT_PUBLICATION_STATE_T *hb_pub_state);

QMESH_RESULT_T QmeshDBSetHeartBeatPublishState(uint8_t net_idx, const QMESH_HEARTBEAT_PUBLICATION_STATE_T *hb_pub_state);

uint16_t QmeshDBGetHeartBeatSubscriptionAddr(uint8_t net_idx);

void QmeshDBDisableHeartBeatSubscription(uint8_t net_idx);

QMESH_RESULT_T QmeshDBGetHeartBeatSubscriptionState(uint8_t net_idx, QMESH_HEARTBEAT_SUBSCRIPTION_STATE_T *hb_sub_state);

QMESH_RESULT_T QmeshDBSetHeartBeatSubscriptionState(uint8_t net_idx, uint16_t src, uint16_t dst, uint8_t period_log);

QMESH_RESULT_T QmeshDBGetHeartBeatPublishInfo(uint8_t net_idx, QMESH_HEARTBEAT_PUBLICATION_INFO_T *hb_pub_info);

void QmeshDBUpdateHeartBeatSubscriptionState(uint8_t net_idx, const QMESH_HEARTBEAT_STATUS_T *hb_status);

QMESH_RESULT_T QmeshDBUpdateHeartBeatPublishInfo(uint8_t net_idx, const QMESH_HEARTBEAT_PUBLICATION_INFO_T *pub_info);

// Relay State
QMESH_RESULT_T QmeshDBGetRelayState(uint8_t net_idx, QMESH_RELAY_STATE_INFO_T *relay_state);

QMESH_RESULT_T QmeshDBSetRelayState(uint8_t net_idx, const QMESH_RELAY_STATE_INFO_T *relay_state);

// GATT Proxy - THIS NEEDS TO BE HANDLED IN APPLICATION.
QMESH_GATT_PROXY_STATE_T QmeshDBGetGATTProxyState(uint8_t net_idx);

QMESH_RESULT_T QmeshDBSetGATTProxyState(uint8_t net_idx, QMESH_GATT_PROXY_STATE_T proxy_state);

uint32_t QmeshDBGetAndUpdateSeqNum(uint8_t net_idx, uint16_t src_addr);


void QmeshDBIterNIDListInit(QMESH_DB_ITERATOR_T *handle);

const QMESH_SUBNET_KEY_INFO_T *QmeshDBIterNIDList(QMESH_DB_ITERATOR_T *handle);

void QmeshDBIterNIDListDeInit(QMESH_DB_ITERATOR_T *handle);

bool QmeshDBIsNIDAvailable(uint8_t nid);

bool QmeshDBIsAIDAvailable(uint8_t aid);

QMESH_RESULT_T QmeshDBAddFrndKeys(QMESH_SUBNET_KEY_IDX_INFO_T subnet_idx, bool frnd,
                                  const QMESH_FRND_KEY_INFO_T *frnd_info);

void QmeshDBDeleteFrndKeys(QMESH_SUBNET_KEY_IDX_INFO_T subnet_idx, uint16_t peer_addr);

bool QmeshDBIsFrndEnabled(uint8_t dev_instance);

bool QmeshDBIsLpnEnabled(uint8_t dev_instance);

QMESH_RESULT_T QmeshDBSetFriendState(uint8_t net_idx, QMESH_FRIEND_STATE_T frnd_state);

QMESH_RESULT_T QmeshDBGetFriendState(uint8_t net_idx, QMESH_FRIEND_STATE_T *frnd_state);

extern QMESH_RESULT_T QmeshDBGetModelPublicationState(uint8_t net_idx,
                                                      uint16_t ele_addr,
                                                      QMESH_MODEL_TYPE_T model_type,
                                                      uint32_t model_id,
                                                      uint8_t *resp_buf);

extern QMESH_RESULT_T QmeshDBSetModelPublicationState(uint8_t net_idx,
                                                      QMESH_PUBLICATION_CONFIG_T *pub_config,
                                                      uint16_t *encoded_vr);

extern QMESH_RESULT_T QmeshDBUpdateModelSubscriptionList(uint8_t net_idx,
                                                         QMESH_SUBSCRIPTION_CONFIG_T *sub_config,
                                                         uint16_t opcode,
                                                         uint16_t *encoded_vr);

extern QMESH_RESULT_T QmeshDBGetModelSubscriptionList(uint8_t net_idx,
                                                      uint16_t ele_addr,
                                                      QMESH_MODEL_TYPE_T model_type,
                                                      uint32_t model_id,
                                                      uint8_t *addr_buf,
                                                      uint16_t *num_addrs);

extern QMESH_RESULT_T QmeshDBModelAppKeyBind(uint8_t net_idx,
                                             const QMESH_MODEL_APPKEY_BIND_INFO_T *bind_info,
                                             bool bind);

extern QMESH_RESULT_T QmeshDBGetModelAppKeyList(uint8_t net_idx, uint16_t elem_addr,
                                                QMESH_MODEL_TYPE_T model_type, uint32_t model_id,
                                                uint8_t *akl_list, uint16_t *num_keys);

extern QMESH_RESULT_T QmeshDBUpdateVirtualTable(const QMESH_LABEL_UUID_T label_uuid,
                                                uint16_t sub_addr,
                                                bool add,
                                                uint16_t *virtual_table_key);

extern const uint16_t *QmeshDBGetVirtualIdxAndUUID(uint16_t vr_addr, uint16_t *vr_idx);

QMESH_RESULT_T QmeshDBGetSecureNetworkBeaconData(QMESH_SUBNET_KEY_IDX_INFO_T subnet_idx,
                                                 uint8_t *beacon_buf);

bool QmeshDBValidateBeacon(const uint8_t *snb_data, QMESH_SUBNET_KEY_IDX_INFO_T *subnet_idx,
                           QMESH_KEY_IDX_T *key_state);

uint16_t QmeshDBGetFrndAddress(QMESH_SUBNET_KEY_IDX_INFO_T subnet_idx);

void QmeshDBEnableIVTestMode(bool enable);

QMESH_RESULT_T QmeshDBSendIVTestModeSignal(uint8_t net_idx, QMESH_IV_UPDATE_STATE_T signal);

bool QmeshDBGetPTSMode(void);

const QMESH_NETKEY_INFO_T *QmeshDBGetNetworkKeyInfo(QMESH_SUBNET_KEY_IDX_INFO_T subnet_idx);

extern void QmeshClearCompositionData(uint8_t net_idx);

#endif /* _QMESH_DB_H_ */

