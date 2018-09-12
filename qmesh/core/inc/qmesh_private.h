/******************************************************************************
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_private.h
 *  \brief This file contains prototypes for mesh internal functionality
 */
/*****************************************************************************/

#ifndef __QMESH_PRIVATE_H__
#define __QMESH_PRIVATE_H__

#include "qmesh_types.h"
#include "qmesh_config.h"
#include "qmesh_soft_timers.h"

#define QMESH_NUM_SOFT_TIMERS               20

/*! \brief Max retransmit count  */
#define MAX_MSG_RETRANSMIT_COUNT        7
/*! \brief Least retransmit Interval, 10ms  */
#define LEAST_MSG_RETRANSMIT_INTERVAL   0

/*! \brief Default retransmit count  */
#define DEFAULT_MSG_RETRANSMIT_COUNT      1
/*! \brief Default retransmit Interval, 10ms  */
#define DEFAULT_MSG_RETRANSMIT_INTERVAL   0

/*! \brief Initial default TTL value, this will be used in cases when the 
 *         default TTL is not set by the config client 
 */
#define DEFAULT_ACCESS_TTL_INIT_VALUE        20

/*! \brief Default relay retransmit count  */
#define DEFAULT_MSG_RELAY_REPEAT_COUNT      1
/*! \brief Default relay retransmit Interval, 10ms  */
#define DEFAULT_MSG_RELAY_REPEAT_INTERVAL   0

/* Use this macro on unused local variables that cannot be removed (such as
 * unused function parameters). This will quell warnings from certain compilers
 * and static code analysis tools like Lint and Valgrind.
 */
#define QMESH_UNUSED(x) ((void) (x))

/* Needs to be moved into sig mesh config header file */

#define QMESH_PS_SANITY_WORD                0x516D  /* Q=0x53 m=0x6D Qm->Qmesh.
                                                     * This value will be checked at stack intialization.
                                                     * If this value is present on PS,the stack variables
                                                     * are intialized with the values read from PS.
                                                     * Otherwise stack variables are reset and
                                                     * stored on the PS.
                                                     */

#define MAX_IV_INDEX_LEN_IN_OCTETS          (4)

#define MAX_NO_OF_NET_ID_SUPPORT            (MAX_NO_OF_PROVISIONED_NETWORK * MAX_NUM_SUB_NET_PER_PROVISIONING)

#define MAX_NO_OF_APP_ID_SUPPORT            (MAX_NO_OF_NET_ID_SUPPORT * MAX_NO_OF_APP_KEY_BIND_SUPPORT_PER_NETWORK)

#define QMESH_MAX_VIRTUAL_ADDRESS           (0x05)

/* Fixed group addresses. */
#define QMESH_ADDRESS_ALL_PROXIES           (0xFFFC)
#define QMESH_ADDRESS_ALL_FRIENDS           (0xFFFD)
#define QMESH_ADDRESS_ALL_RELAYS            (0xFFFE)
#define QMESH_ADDRESS_ALL_NODES             (0xFFFF)

#define UPPER_NIBBLE_MASK (0xF0)
#define LOWER_NIBBLE_MASK (0x0F)

#define MESH_ADDR_LEN_IN_BYTES          (2UL)

/* Unprovisioned Device Beacon */
#define UNPROVISIONED_DEVICE_BEACON__BEACON_TYPE__IDX (0)
#define UNPROVISIONED_DEVICE_BEACON__BEACON_TYPE__LEN (1)

#define UNPROVISIONED_DEVICE_BEACON__UUID__IDX (UNPROVISIONED_DEVICE_BEACON__BEACON_TYPE__IDX + \
                                                UNPROVISIONED_DEVICE_BEACON__BEACON_TYPE__LEN)
#define UNPROVISIONED_DEVICE_BEACON__UUID__LEN (16)

#define UNPROVISIONED_DEVICE_BEACON__OOB__IDX (UNPROVISIONED_DEVICE_BEACON__UUID__IDX + \
                                               UNPROVISIONED_DEVICE_BEACON__UUID__LEN)
#define UNPROVISIONED_DEVICE_BEACON__OOB__LEN (2)

#define UNPROVISIONED_DEVICE_BEACON__URI_HASH__IDX (UNPROVISIONED_DEVICE_BEACON__OOB__IDX + \
                                                    UNPROVISIONED_DEVICE_BEACON__OOB__LEN)
#define UNPROVISIONED_DEVICE_BEACON__URI_HASH__LEN (4)

#define UNPROVISIONED_DEVICE_BEACON__PACKET_LEN (UNPROVISIONED_DEVICE_BEACON__BEACON_TYPE__LEN + \
                                                 UNPROVISIONED_DEVICE_BEACON__UUID__LEN + \
                                                 UNPROVISIONED_DEVICE_BEACON__OOB__LEN + \
                                                 UNPROVISIONED_DEVICE_BEACON__URI_HASH__LEN)

/* Secure Network Beacon */

#define SECURE_NETWORK_BEACON__BEACON_TYPE__IDX (0)
#define SECURE_NETWORK_BEACON__BEACON_TYPE__LEN (1)

#define SECURE_NETWORK_BEACON__FLAGS__IDX (SECURE_NETWORK_BEACON__BEACON_TYPE__IDX + \
                                           SECURE_NETWORK_BEACON__BEACON_TYPE__LEN)
#define SECURE_NETWORK_BEACON__FLAGS__LEN (1)

#define SECURE_NETWORK_BEACON__NETWORK_ID__IDX (SECURE_NETWORK_BEACON__FLAGS__IDX + \
                                                SECURE_NETWORK_BEACON__FLAGS__LEN)
#define SECURE_NETWORK_BEACON__NETWORK_ID__LEN (8)

#define SECURE_NETWORK_BEACON__IV_IDX__IDX (SECURE_NETWORK_BEACON__NETWORK_ID__IDX + \
                                            SECURE_NETWORK_BEACON__NETWORK_ID__LEN)
#define SECURE_NETWORK_BEACON__IV_IDX__LEN (4)

#define SECURE_NETWORK_BEACON_DATA_LENGTH  (SECURE_NETWORK_BEACON__FLAGS__LEN + \
                                            SECURE_NETWORK_BEACON__NETWORK_ID__LEN + \
                                            SECURE_NETWORK_BEACON__IV_IDX__LEN)

#define SECURE_NETWORK_BEACON__AUTH_VAL__IDX (SECURE_NETWORK_BEACON__IV_IDX__IDX + \
                                              SECURE_NETWORK_BEACON__IV_IDX__LEN)
#define SECURE_NETWORK_BEACON__AUTH_VAL__LEN (8)

#define SECURE_NETWORK_BEACON__PACKET_LEN (SECURE_NETWORK_BEACON__BEACON_TYPE__LEN + \
                                           SECURE_NETWORK_BEACON_DATA_LENGTH + \
                                           SECURE_NETWORK_BEACON__AUTH_VAL__LEN)

/* Key Refresh & IV Update bit positions in Secure Network Beacon Flag field. */
#define KEY_REFRESH_FLAG_MASK              (1 << 0)
#define IV_UPDATE_FLAG_MASK                (1 << 1)

#define GET_MSB_OF_UINT16(x) (((x) >> 8) & 0xFF)
#define GET_LSB_OF_UINT16(x) ((x) & 0xFF)
#define GET_BYTE_OF_UINT32(x, idx) ((uint8_t)(((uint32_t)(x) >> (uint16_t)(idx << 3)) & 0xFF))

/*!< \brief Macros to get U16/U24/U32 value from U8 array in little/big endian format */
#define GET_U16_FROM_U8_LE(u8_arr, idx) (((uint16_t) (u8_arr)[(idx) + 0])        | \
                                         ((uint16_t) (u8_arr)[(idx) + 1] << 8u))

#define GET_U16_FROM_U8_BE(u8_arr, idx) (((uint16_t) (u8_arr)[(idx) + 1])        | \
                                         ((uint16_t) (u8_arr)[(idx) + 0] << 8u))

#define GET_U24_FROM_U8_LE(u8_arr, idx) (((uint32_t) (u8_arr)[(idx) + 0])        | \
                                         ((uint32_t) (u8_arr)[(idx) + 1] << 8u)  | \
                                         ((uint32_t) (u8_arr)[(idx) + 2] << 16u))

#define GET_U24_FROM_U8_BE(u8_arr, idx) (((uint32_t) (u8_arr)[(idx) + 2])        | \
                                         ((uint32_t) (u8_arr)[(idx) + 1] << 8u)  | \
                                         ((uint32_t) (u8_arr)[(idx) + 0] << 16u))

#define GET_U32_FROM_U8_LE(u8_arr, idx) (((uint32_t) (u8_arr)[(idx) + 0])        | \
                                         ((uint32_t) (u8_arr)[(idx) + 1] << 8u)  | \
                                         ((uint32_t) (u8_arr)[(idx) + 2] << 16u) | \
                                         ((uint32_t) (u8_arr)[(idx) + 3] << 24u))

#define GET_U32_FROM_U8_BE(u8_arr, idx) (((uint32_t) (u8_arr)[(idx) + 3])        | \
                                         ((uint32_t) (u8_arr)[(idx) + 2] << 8u)  | \
                                         ((uint32_t) (u8_arr)[(idx) + 1] << 16u) | \
                                         ((uint32_t) (u8_arr)[(idx) + 0] << 24u))

#define PACK_U8_INTO_U16(u8_src_lsb, u8_src_msb) (((uint16_t) (u8_src_msb) << 8) | (u8_src_lsb))

/* Unpack IV index in big endian */
#define QMESH_UNPACK_U32_BE(u8_ptr, u32_val) \
    do { \
         *((uint8_t *)(u8_ptr) + 3) = ((u32_val) & 0xFF); \
         *((uint8_t *)(u8_ptr) + 2) = ((u32_val) >> 8)  & 0xFF; \
         *((uint8_t *)(u8_ptr) + 1) = ((u32_val) >> 16) & 0xFF; \
         *((uint8_t *)(u8_ptr) + 0) = ((u32_val) >> 24) & 0xFF; \
    } while(0)

#define QMESH_UNPACK_U24_BE(u8_ptr, u32_val) \
    do { \
         *((uint8_t *)(u8_ptr) + 2) = ((u32_val) & 0xFF); \
         *((uint8_t *)(u8_ptr) + 1) = ((u32_val) >> 8)  & 0xFF; \
         *((uint8_t *)(u8_ptr) + 0) = ((u32_val) >> 16) & 0xFF; \
    } while(0)

#define QMESH_UNPACK_U16_BE(u8_ptr, u16_val) \
    do { \
         *((uint8_t *)(u8_ptr) + 1) = ((u16_val) & 0xFF); \
         *((uint8_t *)(u8_ptr) + 0) = ((u16_val) >> 8)  & 0xFF; \
    } while(0)

#define QMESH_UNPACK_U16_LE(u8_ptr, u16_val) \
    do { \
         *((uint8_t *)(u8_ptr) + 0) = ((u16_val) & 0xFF); \
         *((uint8_t *)(u8_ptr) + 1) = ((u16_val) >> 8)  & 0xFF; \
    } while(0)


/* Marco to Check if the address is valid unicast address or not */
#define IS_VALID_UNICAST_ADDR(addr) ((((addr) == 0x0000) || ((addr) & 0x8000)) ? FALSE : TRUE)

/*! \brief State of the Key. */
typedef enum
{
    QMESH_KEY_IDX_CURRENT  = 0,
    QMESH_KEY_IDX_NEW      = 1,
    QMESH_KEY_IDX_MAX      = 2
} QMESH_KEY_IDX_T;

/*! \brief Nonce type. */
typedef enum
{
    QMESH_NETWORK_NONCE = 0x00,
    QMESH_APP_NONCE     = 0x01,
    QMESH_DEV_NONCE     = 0x02,
    QMESH_PROXY_NONCE   = 0x03
} QMESH_NONCE_T;

/*!< \brief Network Layer Message Types. */
typedef enum
{
    QMESH_NETWORK_MESG_ACCESS       = 0,    /*!< Access Message. */
    QMESH_NETWORK_MESG_CONTROL      = 1,    /*!< Transport Control Message. */
} QMESH_NETWORK_MESG_T;

/*! \brief Heart Beat Subsciption state */
typedef struct
{
    uint16_t                src_addr;       /*!< Source address. */
    uint16_t                dst_addr;       /*!< Destination address. */
    uint8_t                 min_hops;       /*!< Minimum hops. */
    uint8_t                 max_hops;       /*!< Maximum hops. */
    uint16_t                count;          /*!< Heart beat messages count for receive */
    uint16_t                period;         /*!< Heart beat receive timer period in seconds */
    QMESH_TIMER_HANDLE_T    hb_tid;         /*!< Timer ID of timer for receiving Heart Beat Messages */
} QMESH_HEARTBEAT_SUBSCRIPTION_INFO_T;

/*! \brief Heart Beat Publication State */
typedef struct
{
    uint16_t               dst_addr;       /*!< Destination address. */
    uint8_t                ttl;            /*!< TTL to be used for a heart beat message. */
    uint16_t               feature_mask;   /*!< Publish reature mask. When any feature state changes and the bit field is set, it triggers a publish message. See \ref QMESH_DEVICE_FEATURE_MASK_T. */
    uint16_t               net_key_idx;    /*!< Globally identified network key index. */
    uint32_t               count;          /*!< Count of out going Heart Beat Messages */
    uint16_t               period;         /*!< Period derived from Period Log */
    QMESH_TIMER_HANDLE_T   hb_tid;         /*!< Timer ID of Heart Beat Periodic timer */
} QMESH_HEARTBEAT_PUBLICATION_INFO_T;

/*! \brief IV Index State. */
typedef struct
{
    uint32_t                        iv_index;                   /*!< IV Index. */
    QMESH_IV_UPDATE_STATE_T         iv_update_state;            /*!< IV Update State. */
    QMESH_TIMER_HANDLE_T            iv_update_tid;              /*!< Timer for 96hr and 144hr check. */
    bool                            defer_iv_update;            /*!< Flag to indicate if IV update has to be
                                                                 *   deffered as Segmented Tx is in progress */
    bool                            recovery_mode;              /*!< Flag to see if IV Recovery Mode is On. */
} QMESH_IV_INDEX_INFO_T;

typedef struct
{
    QMESH_SUBNET_KEY_IDX_INFO_T         net_key_idx;                /*!< 4-bit network index and 12-bit Global Network Key Index. */
    QMESH_KEY_REFRESH_PHASE_T           kr_phase;                   /*!< Key Refresh state of the subnetwork. */
    QMESH_KEY_IDX_T                     key_state;                  /*!< Key State. */
    QMESH_NETWORK_KEY_T                 network_key;                /*!< Network Key. */
} QMESH_SUBNET_ADD_INFO_T;

/*! \brief Secondary Key information generated from network key. */
typedef struct _QMESH_SUBNET_KEY_INFO_T
{
    uint8_t                             nid;                        /*!< 7-bit NID. */
    QMESH_SEC_KEY_T                     encrypt_key;                /*!< Encryption Key. */
    QMESH_SEC_KEY_T                     privacy_key;                /*!< Privacy Key. */
    QMESH_SUBNET_KEY_IDX_INFO_T         net_key_idx;                /*!< 4-bit network index and 12-bit Global Network Key Index. */
    uint16_t                            frnd_lpn_addr;              /*!< Primary Element Address of FRND or LPN in case of Friend Keys.
                                                                         Otherwise it is QMESH_UNASSIGNED_ADDRESS. */
    struct _QMESH_SUBNET_KEY_INFO_T    *next;                       /*!< Pointer to the next entry in the NID list. */
} QMESH_SUBNET_KEY_INFO_T;

/*! \brief Network Key and it's derived information. */
typedef struct
{
    QMESH_NETWORK_KEY_T                 network_key;                /*!< Network Key. */
    QMESH_SEC_KEY_T                     beacon_key;                 /*!< Beacon Key. */
    QMESH_NETWORK_ID_T                  network_id;                 /*!< 64-bit Network ID. */
    QMESH_SUBNET_KEY_INFO_T            *subnet_key_info;            /*!< Pointer Subnetwork Information. */
} QMESH_NETKEY_INFO_T;

/*! \brief Subnetwork information that is unique to a subnetwork. */
typedef struct
{
    QMESH_SUBNET_KEY_IDX_INFO_T         net_key_idx;                            /*!< 4-bit network index and 12-bit Global Network Key Index. */
    QMESH_KEY_REFRESH_PHASE_T           kr_phase;                               /*!< Key Refresh state of the subnetwork. */
    QMESH_TIMER_HANDLE_T                beacon_obs_tid;                         /*!< Beacon Observation Timer Handle. */
    QMESH_TIMER_HANDLE_T                beacon_tx_tid;                          /*!< Beacon Transmission Timer Handle. */
    uint16_t                            num_obs_beacon;                         /*!< Number of Beacons observed on the subnet. */
    uint16_t                            beacon_obs_period;                      /*!< Beacon Observation period in seconds. */
    QMESH_NETKEY_INFO_T                *net_keys[QMESH_KEY_IDX_MAX];            /*!< Current and New Keys. */
} QMESH_SUBNET_INFO_T;

/*! \brief Friendship Key Information. */
typedef struct
{
    uint16_t                     peer_addr;
    uint16_t                     frnd_counter;
    uint16_t                     lpn_counter;
    const QMESH_SUBNET_INFO_T   *subnet_info;
    QMESH_SUBNET_KEY_INFO_T     *subnet_keys[QMESH_KEY_IDX_MAX];
} QMESH_FRND_KEY_DATA_T;

/*! \brief Relay State. */
typedef struct
{
    QMESH_RELAY_STATE_T             relay_state;        /*! Relay State. */
    QMESH_TRANSMIT_STATE_T          relay_retx_info;    /*! Relay Retransmit Information. */
} QMESH_RELAY_STATE_INFO_T;

typedef struct
{
    uint8_t                         aid;                    /*!< 6-bit AID. */
    QMESH_APPLICATION_KEY_T         app_key;                /*!< Application Key. */
} QMESH_AID_APPKEY_T;

/*! \brief Application Key Data. */
typedef struct
{
    uint16_t                        app_key_idx;                    /*!< 12-bit Global Application Key Index. */
    QMESH_AID_APPKEY_T             *app_keys[QMESH_KEY_IDX_MAX];    /*!< Pointers to the App Keys. */
    const QMESH_SUBNET_INFO_T      *net_key_info;                   /*!< Pointer to the Network Key Information. */
} QMESH_APPKEY_DATA_T;

/*! \brief Application Key Add/Update/Sotre Data Structure. */
typedef struct
{
    QMESH_KEY_IDX_T                 key_state;
    QMESH_SUBNET_KEY_IDX_INFO_T     net_key_idx;         /*!< 4-bit network index and 12-bit Global Network Key Index. */
    uint16_t                        app_key_idx;         /*!< 12-bit Global Application Key Index. */
    QMESH_APPLICATION_KEY_T         app_key;             /*!< Application Key. */
} QMESH_APPKEY_ADD_INFO_T;

/*! \brief Network Data Structure */
typedef struct
{
    QMESH_DEVICE_KEY_T                      dev_key;            /*!< Device Key. */
    QMESH_LIST_HANDLE_T                     rpl;                /*!< Replay Protection List. */
    QMESH_SUBNET_INFO_T                   **net_idx_list;       /*!< NetKey Index list. */
    QMESH_APPKEY_DATA_T                   **appkey_list;        /*!< AppKey List. */
    QMESH_IV_INDEX_INFO_T                   iv_index_info;      /*!< Intialization Vector State. */
    QMESH_TRANSMIT_STATE_T                  nw_tx_state;        /*!< Network Transmit State. */
    QMESH_SECURE_NW_BEACON_STATE_T          beacon_state;       /*!< Beacon State. */
    uint8_t                                 default_ttl;        /*!< Default TTL State. */
    QMESH_HEARTBEAT_PUBLICATION_INFO_T      hb_pub_state;       /*!< Heartbeat Publication State. */
    QMESH_HEARTBEAT_SUBSCRIPTION_INFO_T     hb_sub_state;       /*!< Heartbeat Subscription State. */
    QMESH_RELAY_STATE_INFO_T                relay_state;        /*!< Relay State and Retransmit Information. */
    QMESH_FRIEND_STATE_T                    frnd_state;         /*!< Friend State. */
    // TODO: Add Friend and LPN states.
} QMESH_NETWORK_T;

/*! \brief Device Data Structure */
typedef struct
{
    const QMESH_DEVICE_IDENTIFIER_DATA_T      *dev_data;       /*!< Device Identity Data. */
    const QMESH_DEVICE_COMPOSITION_DATA_T     *comp_data;      /*!< Device Composition Data. */
    QMESH_ROLE_T                               role;           /*!<  Device Role. */
    // TODO: Do we need a provisioning context here?
    QMESH_NETWORK_T                           *network;        /*!< Network Information. */
} QMESH_DEVICE_T;

/*! \brief Heartbeat status. */
typedef struct
{
    uint16_t    src_addr;
    uint16_t    dst_addr;
    uint16_t    features;
    uint8_t     hops;
} QMESH_HEARTBEAT_STATUS_T;

typedef struct
{
#ifdef PLATFORM_MULTITHREAD_SUPPORT
    QMESH_MUTEX_T prov_mutex;
#endif
    QMESH_TIMER_GROUP_HANDLE_T timer_ghdl; /* Timer group handle */
} MESH_DATA_T;

extern void NotifyAppCallbackEvent(QMESH_EVENT_T event, QMESH_RESULT_T status, void* data);

// TODO: Get rid of this.
/* Mesh Data */
extern MESH_DATA_T g_mesh_data;

#endif /* __QMESH_PRIVATE_H__ */

