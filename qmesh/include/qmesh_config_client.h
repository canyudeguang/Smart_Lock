/*=============================================================================
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_config_client.h
 *
 *  \brief Qmesh Configuration Client API.
 *
 *   This file contains the different data types and API definitions for
 *   Configuration client.
 *
 */
 /*****************************************************************************/

#ifndef _QMESH_CONFIG_CLIENT_H_
#define _QMESH_CONFIG_CLIENT_H_

#include "qmesh_types.h"

/*! \addtogroup Core_Config_Client
* @{
*/

/*============================================================================*
   Public Definitions
 *============================================================================*/

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/
/*! \brief Configuration client commands. */
typedef enum
{
    QMESH_CONFIG_CLI_BEACON_GET,  /*!< Gets the beacon state of a node. See \ref QMESH_CONFIG_BEACON_STATUS_EVENT. */
    QMESH_CONFIG_CLI_BEACON_SET,  /*!< Sets beacon state on a node. See \ref QMESH_CONFIG_BEACON_STATUS_EVENT. */
    QMESH_CONFIG_CLI_COMPOSITION_DATA_GET,  /*!< Reads composition data from the configuration server. See \ref QMESH_CONFIG_COMPOSITION_DATA_STATUS_EVENT. */
    QMESH_CONFIG_CLI_DEFAULT_TTL_GET,  /*!< Gets the default TTL value from the node. See \ref QMESH_CONFIG_DEFAULT_TTL_STATUS_EVENT. */
    QMESH_CONFIG_CLI_DEFAULT_TTL_SET,  /*!< Sets the default TTL value from the node. See \ref QMESH_CONFIG_DEFAULT_TTL_STATUS_EVENT. */
    QMESH_CONFIG_CLI_RELAY_GET,  /*!< Gets the relay state of the server. See \ref QMESH_CONFIG_RELAY_STATUS_EVENT. */
    QMESH_CONFIG_CLI_RELAY_SET,  /*!< Sets the relay state of the server. See \ref QMESH_CONFIG_RELAY_STATUS_EVENT.*/
    QMESH_CONFIG_CLI_NETWORK_KEY_ADD,  /*!< Adds a network key on the node. See \ref QMESH_CONFIG_NET_KEY_STATUS_EVENT. */
    QMESH_CONFIG_CLI_NETWORK_KEY_UPDATE,  /*!< Updates a network key on the node. See \ref QMESH_CONFIG_NET_KEY_STATUS_EVENT. */
    QMESH_CONFIG_CLI_NETWORK_KEY_DELETE,  /*!< Deletes a network key on the node. See \ref QMESH_CONFIG_NET_KEY_STATUS_EVENT. */
    QMESH_CONFIG_CLI_NETWORK_KEY_GET,  /*!< Gets all global net key indices for this network. See \ref QMESH_CONFIG_NET_KEY_LIST_EVENT. */
    QMESH_CONFIG_CLI_APP_KEY_ADD,  /*!< Adds an application key to a node. See \ref QMESH_CONFIG_APP_KEY_STATUS_EVENT. */
    QMESH_CONFIG_CLI_APP_KEY_UPDATE,  /*!< Updates an application key on a node. See \ref QMESH_CONFIG_APP_KEY_STATUS_EVENT. */
    QMESH_CONFIG_CLI_APP_KEY_DELETE,  /*!< Deletes an application key on a node. See \ref QMESH_CONFIG_APP_KEY_STATUS_EVENT. */
    QMESH_CONFIG_CLI_APP_KEY_GET,  /*!< Gets the application bound to the net key from the configuration server. See \ref QMESH_CONFIG_APP_KEY_LIST_EVENT. */
    QMESH_CONFIG_CLI_KEY_REFRESH_PHASE_GET,  /*!< Gets the key refresh state of a node. See \ref QMESH_CONFIG_KEY_REFRESH_PHASE_STATUS_EVENT. */
    QMESH_CONFIG_CLI_KEY_REFRESH_PHASE_SET,  /*!< Sets the key refresh state on a node. See \ref QMESH_CONFIG_KEY_REFRESH_PHASE_STATUS_EVENT. */
    QMESH_CONFIG_CLI_GATT_PROXY_STATE_GET,  /*!< Gets the proxy state of the server. See \ref QMESH_CONFIG_GATT_PROXY_STATUS_EVENT. */
    QMESH_CONFIG_CLI_GATT_PROXY_STATE_SET,  /*!< Sets the proxy state of the server. See \ref QMESH_CONFIG_GATT_PROXY_STATUS_EVENT. */
    QMESH_CONFIG_CLI_NODE_IDENTITY_SET,  /*!< Sets the node identity state of a node. See \ref QMESH_CONFIG_NODE_IDENTITY_STATUS_EVENT. */
    QMESH_CONFIG_CLI_NODE_IDENTITY_GET,  /*!< Gets the node identity state of a node. See \ref QMESH_CONFIG_NODE_IDENTITY_STATUS_EVENT. */
    QMESH_CONFIG_CLI_PUBLICATION_GET,  /*!< Gets the configuration model publication state of a node. See \ref QMESH_CONFIG_MODEL_PUB_STATUS_EVENT. */
    QMESH_CONFIG_CLI_PUBLICATION_SET,  /*!< Sets the configuration model publication state of a node. See \ref QMESH_CONFIG_MODEL_PUB_STATUS_EVENT. */
    QMESH_CONFIG_CLI_PUBLICATION_VIRTUAL_ADDR_SET,  /*!< Sets the configuration model publication virtual
                                               * address state of a node. See \ref QMESH_CONFIG_MODEL_PUB_STATUS_EVENT.
                                               */
    QMESH_CONFIG_CLI_SUBSCRIPTION_SET,  /*!< Adds, deletes, or overwrites an address
                                         * to the subscription list of a node. See \ref QMESH_CONFIG_MODEL_SUB_STATUS_EVENT.
                                         */
    QMESH_CONFIG_CLI_SUBSCRIPTION_DEL_ALL,  /*!< Deletes all addresses
                                             * to the subscription list of a node. See \ref QMESH_CONFIG_MODEL_SUB_STATUS_EVENT.
                                             */
    QMESH_CONFIG_CLI_SUBSCRIPTION_VIRTUAL_ADDR_SET,  /*!< Adds, deletes, or overwrites
                                                      * a virtual address to the subscription
                                                      * list of a node. See \ref QMESH_CONFIG_MODEL_SUB_STATUS_EVENT.
                                                      */
    QMESH_CONFIG_CLI_SUBSCRIPTION_GET,  /*!< Gets the model subscription list of a SIG model
                                         * within an element on a node.
                                         */
    QMESH_CONFIG_CLI_VENDOR_SUBSCRIPTION_GET,  /*!< Gets the subscription list of
                                                * a vendor model within an element on a node. See \ref QMESH_CONFIG_VENDOR_MODEL_SUBSCRIBE_LIST_EVENT.
                                                */
    QMESH_CONFIG_CLI_APP_BIND,  /*!< Binds a model with an application key on the server. See \ref QMESH_CONFIG_MODEL_APP_STATUS_EVENT. */
    QMESH_CONFIG_CLI_APP_UNBIND,  /*!< Unbinds a model with an application key on the server. See \ref QMESH_CONFIG_MODEL_APP_STATUS_EVENT. */
    QMESH_CONFIG_CLI_SIG_MODEL_APP_GET,  /*!< Gets all the application keys bound with the SIG model on the server. See \ref QMESH_CONFIG_SIG_MODEL_APP_LIST_EVENT. */
    QMESH_CONFIG_CLI_VENDOR_MODEL_APP_GET,  /*!< Gets all the application keys bound with the vendor model on the server. See \ref QMESH_CONFIG_VENDOR_MODEL_APP_LIST_EVENT. */
    QMESH_CONFIG_CLI_NODE_RESET,  /*!< Resets a node. See \ref QMESH_CONFIG_NODE_RESET_STATUS_EVENT. */
    QMESH_CONFIG_CLI_FRIEND_GET,  /*!< Gets the friend state of a server. See \ref QMESH_CONFIG_FRIEND_STATUS_EVENT. */
    QMESH_CONFIG_CLI_FRIEND_SET,  /*!< Sets the friend state on a server. See \ref QMESH_CONFIG_FRIEND_STATUS_EVENT. */
    QMESH_CONFIG_CLI_HEART_BEAT_PUBLIC_GET,  /*!< Gets the heart beat publication state of a node. See \ref QMESH_CONFIG_HEART_BEAT_PUB_STATUS_EVENT. */
    QMESH_CONFIG_CLI_HEART_BEAT_PUBLIC_SET,  /*!< Sets the heart beat publication state of a node. See \ref QMESH_CONFIG_HEART_BEAT_PUB_STATUS_EVENT. */
    QMESH_CONFIG_CLI_HEART_BEAT_SUBCRIBE_GET,  /*!< Gets the heart beat subscription state of a node. See \ref QMESH_CONFIG_HEART_BEAT_SUB_STATUS_EVENT. */
    QMESH_CONFIG_CLI_HEART_BEAT_SUBCRIBE_SET,  /*!< Sets the heart beat subscription state of a node. See \ref QMESH_CONFIG_HEART_BEAT_SUB_STATUS_EVENT. */
    QMESH_CONFIG_CLI_LPN_POLL_TIMEOUT_GET,  /*!< Gets the current value of the poll timeout timer of the low
                                       * power node within a friend node. See \ref QMESH_CONFIG_LPN_POLL_TIMEOUT_STATUS_EVENT.
                                       */
    QMESH_CONFIG_CLI_NETWORK_TRANSMIT_GET,  /*!< Gets the current network transmit state of a node. See \ref QMESH_CONFIG_NETWORK_TRANSMIT_STATUS_EVENT. */
    QMESH_CONFIG_CLI_NETWORK_TRANSMIT_SET,  /*!< Sets the current network transmit state of a node. See \ref QMESH_CONFIG_NETWORK_TRANSMIT_STATUS_EVENT. */
} QMESH_CONFIG_CLIENT_CMD_T;

/*! \brief Configuration model subscription operation code. */
typedef enum
{
    QMESH_MODEL_SUB_OPERATION_ADD,        /*!< Adds an address in the subscription list. */
    QMESH_MODEL_SUB_OPERATION_DELETE,     /*!< Deletes an address in the subscription list. */
    QMESH_MODEL_SUB_OPERATION_OVERWRITE,  /*!< Overwrites an address in the subscription list. */
    QMESH_MODEL_SUB_OPERATION_DELETE_ALL  /*!< Deletes a complete subscription list. */
} QMESH_MODEL_SUB_OPERATION_T;

/*============================================================================*
    Data Type Definitions
 *============================================================================*/

/*================= Configuration Client Command Data Structures ===================*/

/*! \brief 12-bit global net/application key index as defined by the spec.
 */
typedef uint16_t QMESH_NET_KEY_INDEX_T;
typedef uint16_t QMESH_APP_KEY_INDEX_T;

/*! \brief Device address information. */
typedef union
{
    uint16_t addr;                  /*!< Value of the publish/subscribe address. */
    QMESH_LABEL_UUID_T label_uuid;  /*!< Label UUID represented as an array of
                                     *   half words in little endian format.
                                     */
} QMESH_ADDR_T;

/*! \brief Element information. */
typedef struct
{
    uint16_t            ele_addr;   /*!< Element address of a node to which this model belongs. */
    QMESH_MODEL_TYPE_T  model_type; /*!< SIG or vendor model type. See \ref QMESH_MODEL_TYPE_T. */
    uint32_t            model_id;   /*!< Model ID array of 2 bytes for a SIG model and 4 bytes for a vendor model. */
} QMESH_ELEMENT_INFO_T;

/*! \brief Configuration network key add. */
typedef struct
{
    QMESH_NET_KEY_INDEX_T   net_key_idx; /*!< 12-bit network key index. */
    QMESH_NETWORK_KEY_T     net_key;     /*!< Network key in packed little endian format. @newpagetable */
} QMESH_CONFIG_NET_KEY_ADD_T;

/*! \brief Configuration application key add. */
typedef struct
{
    QMESH_NET_KEY_INDEX_T   net_key_idx; /*!< 12-bit network key index. */
    QMESH_APP_KEY_INDEX_T   app_key_idx; /*!< 12-bit application key index. */
    QMESH_APPLICATION_KEY_T app_key;     /*!< Application key in packed little endian format. */
} QMESH_CONFIG_APP_KEY_ADD_T;

/*! \brief Configuration application key delete. */
typedef struct
{
    QMESH_NET_KEY_INDEX_T net_key_idx; /*!< 12-bit network key index. */
    QMESH_APP_KEY_INDEX_T app_key_idx; /*!< 12-bit application key index. */
} QMESH_CONFIG_APP_KEY_DELETE_T;

/*! \brief Configuration set key refresh phase. */
typedef struct
{
    QMESH_NET_KEY_INDEX_T   net_key_idx; /*!< 12-bit network key index. */
    uint8_t                 phase;       /*!< Key refresh phase to set for phase values. */
} QMESH_CONFIG_KEY_REFRESH_PHASE_SET_T;

/*! \brief Configuration set node identity state. */
typedef struct
{
    QMESH_NET_KEY_INDEX_T   net_key_idx;    /*!< 12-bit network key index. */
    uint8_t                 identity_state; /*!< Identity state to set (range 0x00 to 0x02).*/
} QMESH_CONFIG_NODE_IDENTITY_SET_T;

/*! \brief Configuration model subscription status. */
typedef struct
{
    QMESH_ELEMENT_INFO_T        ele_info;   /*!< Element information of the peer device being configured. */
    QMESH_MODEL_SUB_OPERATION_T operation;  /*!< Add, delete, or overwrite. See \ref QMESH_MODEL_SUB_OPERATION_T. */
    QMESH_ADDR_T                sub_addr;   /*!< Subscription address */
} QMESH_CONFIG_SUBSCRIPTION_CFG_T;

/*! \brief Configuration set application key bind/unbind with model state. */
typedef struct
{
    QMESH_ELEMENT_INFO_T    ele_info;       /*!< Element information of the peer device being configured. */
    QMESH_APP_KEY_INDEX_T   app_key_idx;    /*!< 12-bit application key index. */
} QMESH_CONFIG_APPKEY_BIND_UNBIND_T;

/*! \brief Configuration set heart beat subscription state. */
typedef struct
{
    uint16_t    src_addr;   /*!< Source address for the heart beat messages. */
    uint16_t    dest_addr;  /*!< Destination address for the heart beat messages. */
    uint8_t     period_log; /*!< Period for the heart beat messages. */
} QMESH_CONFIG_HEART_BEAT_SUB_SET_T;

/*! \brief Configuration set relay state. */
typedef struct
{
    uint8_t relay_state;            /*!< New relay state for the node (range 0x00 to 0x02). */
    uint8_t relay_rtx_cnt:3;        /*!< Number of retransmissions on the advertising bearer for each network PDU relayed by the node. */
    uint8_t relay_rtx_int_stp:5;    /*!< Number of 10-millisecond steps between retransmissions. */
} QMESH_CONFIG_RELAY_SET_T;

/*! \brief The unicast address of the low power node. */
typedef uint16_t QMESH_CONFIG_LPN_POLL_TIMEOUT_GET_T;

/*! \brief Configuration set network transmit state. */
typedef struct
{
    uint8_t nw_tx_count:3;      /*!< Number of transmissions for each network PDU originating from the node. */
    uint8_t nw_tx_int_step:5;   /*!< Number of 10-millisecond steps between transmissions. */
} QMESH_CONFIG_NETWORK_TX_SET_T;

/*! \brief Heart beat publication state. */
typedef struct
{
    uint16_t dest_addr;    /*!< Destination address of the heart beat messages. */
    uint16_t features;     /*!< Bit field indicating features that trigger heart beat messages when changed. */
    uint16_t net_key_idx;  /*!< Net key index. */
    uint8_t  count_log;    /*!< Number of heart beat messages to be sent, where count_log is a
                            *   least possible value for which 2^(count_log - 1) is greater
                            *   than or equal to the count value. Currently, values between 0x01 and 0x11 are valid.
                            */
    uint8_t  period_log;   /*!< Period in seconds for sending heart beat messages, where
                            *   period_log is the least possible value for which
                            *   2^(period_log -1) is greater than or equal to the period.
                            *   Currently, values between 0x01 and 0x11 are valid.
                            */
    uint8_t  ttl;          /*!< TTL to be used when sending heart beat messages. */
} QMESH_HEART_BEAT_PUBLICATION_T;

/*! \brief Configuration model publication state and virtual address state. */
typedef struct
{
    uint16_t            ele_addr;           /*!< Address of the element. */
    uint16_t            app_key_idx;        /*!< Index of the application key. */
    bool                credential_flag;    /*!< Value of the friendship credential flag. */
    uint8_t             publish_ttl;        /*!< Default TTL value for the outgoing messages. */
    uint8_t             publish_period;     /*!< Period for periodic status publishing. */
    uint8_t             pub_rtx_cnt:3;      /*!< Number of retransmissions for each published message. */
    uint8_t             pub_rtx_int_stp:5;  /*!< Number of 50-millisecond steps between retransmissions. */
    QMESH_MODEL_TYPE_T  model_type;         /*!< SIG or vendor model type. See \ref QMESH_MODEL_TYPE_T. */
    uint32_t            model_id;           /*!< SIG model ID or vendor model ID. */
    QMESH_ADDR_T        pub_addr;           /*!< Publication address. */
} QMESH_CONFIG_MODEL_PUBLICATION_T;

/*! \brief Beacon state to set on a node (range 0x00 to 0x01). */
typedef uint8_t QMESH_CONFIG_BEACON_SET_T;

/*! \brief Friend state to set on a node (range 0x00 to 0x02). */
typedef uint8_t QMESH_CONFIG_FRIEND_SET_T;

/*! \brief Page number of the composition data to get; only page 0 is defined. */
typedef uint8_t QMESH_CONFIG_COMPOSITION_DATA_GET_T;

/*! \brief Default TTL value to set on a node (range 0x00, 0x02 to 0x7F). */
typedef uint8_t QMESH_CONFIG_DEFAULT_TTL_SET_T;

/*! \brief GATT proxy state to set on a node (range 0x01 to 0x02). */
typedef uint8_t QMESH_CONFIG_GATT_PROXY_SET_T;

/*! \brief Configuration net key delete status. */
typedef QMESH_NET_KEY_INDEX_T QMESH_CONFIG_NET_KEY_DELETE_T;

/*! \brief Configuration application key get message. */
typedef QMESH_NET_KEY_INDEX_T QMESH_CONFIG_APP_KEY_GET_T;

/*! \brief Configuration key refresh phase get message. */
typedef QMESH_NET_KEY_INDEX_T QMESH_CONFIG_KEY_REFRESH_PHASE_GET_T;

/*! \brief Configuration node identity get message. */
typedef QMESH_NET_KEY_INDEX_T QMESH_CONFIG_NODE_IDENTITY_GET_T;

/*! \brief Configuration application key update message. */
typedef QMESH_CONFIG_APP_KEY_ADD_T QMESH_CONFIG_APP_KEY_UPDATE_T;

/*! \brief Configuration network key update message. */
typedef QMESH_CONFIG_NET_KEY_ADD_T QMESH_CONFIG_NET_KEY_UPDATE_T;

/*! \brief Configuration publication get message. */
typedef QMESH_ELEMENT_INFO_T QMESH_CONFIG_PUBLICATION_GET_T;

/*! \brief Configuration model publication set message. */
typedef QMESH_CONFIG_MODEL_PUBLICATION_T QMESH_CONFIG_MODEL_PUB_SET_T;

/*! \brief Configuration model publication virtual address set message. */
typedef QMESH_CONFIG_MODEL_PUBLICATION_T QMESH_CONFIG_MODEL_PUB_VIR_ADDR_SET_T;

/*! \brief Configuration subscription set message. */
typedef QMESH_CONFIG_SUBSCRIPTION_CFG_T QMESH_CONFIG_SUBSCRIPTION_SET_T;

/*! \brief Configuration subscription virtual address set message. */
typedef QMESH_CONFIG_SUBSCRIPTION_CFG_T QMESH_CONFIG_SUBSCRIPTION_VIR_SET_T;

/*! \brief Configuration subscription delete all message. */
typedef QMESH_ELEMENT_INFO_T QMESH_CONFIG_SUBSCRIPTION_DEL_ALL_T;

/*! \brief Configuration subscription get message. */
typedef QMESH_ELEMENT_INFO_T QMESH_CONFIG_SUBSCRIPTION_GET_T;

/*! \brief Configuration application get message. */
typedef QMESH_ELEMENT_INFO_T QMESH_CONFIG_APP_GET_T;

/*================= Configuration Client Status Data Structures ====================*/
/*! \brief QMesh configuration model net key status. */
typedef struct
{
    uint8_t    status;             /*!< Status from the server. See \ref QMESH_RESULT_T. */
    uint16_t   glob_net_key_idx;   /*!< Requested global net key index. */
} QMESH_CONFIG_MODEL_NET_KEY_STATUS_T;

/*! \brief QMesh configuration model application key status. */
typedef struct
{
    uint8_t    status;             /*!< Status from the server. See \ref QMESH_RESULT_T. */
    uint16_t   glob_net_key_idx;   /*!< Requested global net key index. */
    uint16_t   glob_app_key_idx;   /*!< Requested global application key index. */

} QMESH_CONFIG_MODEL_APP_KEY_STATUS_T;

/*! \brief QMesh configuration model node identity status. */
typedef struct
{
    uint16_t   glob_net_key_idx;   /*!< Global net key index. */
    uint8_t    status;             /*!< Status from the server. See \ref QMESH_RESULT_T. */
    uint8_t    identity_state;     /*!< Identity state of the server. */
} QMESH_CONFIG_MODEL_NODE_IDENTITY_STATUS_T;

/*! \brief QMesh configuration model application key list. */
typedef struct
{
    const uint16_t  *app_key_index;     /*!< List of global application key indices. */
    uint16_t        glob_net_key_idx;   /*!< Global net key index. */
    uint16_t        num_keys;           /*!< Number of application keys. */
    uint8_t         status;             /*!< Status from the server. See \ref QMESH_RESULT_T. */
} QMESH_CONFIG_MODEL_APP_KEY_LIST_STATUS_T;

/*! \brief QMesh configuration model net key list. */
typedef struct
{
    const uint16_t  *net_key_index;    /*!< List of global net key indices. */
    uint8_t         num_keys;          /*!< Number of net keys. */
} QMESH_CONFIG_MODEL_NET_KEY_LIST_STATUS_T;

/*! \brief Heart beat publication status. */
typedef struct
{
    uint16_t   dest_addr;      /*!< Destination address of heart beat publication messages. */
    uint16_t   features;       /*!< Features bitmask to be published. */
    uint16_t   net_key_idx;    /*!< Global net key index to be used to encrypt heart beat messages. */
    uint8_t    status;         /*!< Status from the server. See \ref QMESH_RESULT_T. */
    uint8_t    count_log;      /*!< Count log of the number of heart beat messages to be sent from a server, where count_log is the least possible value for which 2^(count_log - 1) is greater than or equal to the count value. */
    uint8_t    period;         /*!< Log of the time in seconds for sending heart beat messages. */
    uint8_t    ttl;            /*!< TTL value being used for heart beat messages. */
} QMESH_CONFIG_MODEL_HEART_BEAT_PUB_STATUS_T;

/*! \brief Heart beat subscription status. */
typedef struct
{
    uint16_t   dest_addr;     /*!< Destination address of heart beat publication messages. */
    uint16_t   src_addr;      /*!< Source address of heart beat publication messages. */
    uint8_t    status;        /*!< Status from the server. See \ref QMESH_RESULT_T. */
    uint8_t    count_log;     /*!< Count log of the number of heart beat messages to be received from a server, where count_log is the least possible value for which 2^(count_log - 1) is greater than or equal to the count value. */
    uint8_t    period_log;    /*!< Time in seconds in which the heart beat messages are to be processed, where period_log is the least possible value for which 2^(period_log - 1) is greater than or equal to the period value. */
    uint8_t    min_hops;      /*!< Minimum hops between two devices. */
    uint8_t    max_hops;      /*!< Maximum hops between two devices. */
} QMESH_CONFIG_MODEL_HEART_BEAT_SUB_STATUS_T;

/*! \brief Model app status. */
typedef struct
{
    QMESH_MODEL_TYPE_T  model_type;    /*!< SIG or vendor model. See \ref QMESH_MODEL_TYPE_T. */
    uint32_t            model_id;      /*!< Model ID. */
    uint16_t            ele_addr;      /*!< Element address of a node in which this model is defined. */
    uint16_t            app_key_idx;   /*!< Index of the application key to be bound with the model. */
    uint8_t             status;        /*!< Status from the server. See \ref QMESH_RESULT_T. */
} QMESH_CONFIG_MODEL_APP_BIND_STATUS_T;

/*! \brief Composition data. */
typedef struct
{
    uint8_t         page_num;       /*!< Page number of this composition information. */
    uint16_t        cid;            /*!< 16-bit company identifier assigned by the Bluetooth SIG. */
    uint16_t        pid;            /*!< Contains a 16-bit vendor-assigned product identifier. */
    uint16_t        vid;            /*!< Contains a 16-bit vendor-assigned product version identifier. */
    uint16_t        crpl;           /*!< 16-bit value representing the minimum number of replay protection list entries in a device. */
    uint16_t        features;       /*!< Contains a bit field indicating the device features. */
    const uint8_t   *ele_list;      /*!< Element list. */
    uint8_t         ele_list_len;   /*!< Element list length in bytes. */
} QMESH_CONFIG_COMPOSITION_DATA_STATUS_T;

/*! \brief Key refresh phase status. */
typedef struct
{
    uint16_t   glob_net_key_index;     /*!< Global net key index. */
    uint8_t    phase;                  /*!< Key refresh phase of a remote node. See \ref QMESH_KEY_REFRESH_PHASE_T. */
    uint8_t    status;                 /*!< Status from the server. See \ref QMESH_RESULT_T. */
} QMESH_CONFIG_KEY_REFRESH_PHASE_STATUS_T;

/*! \brief Model application list. */
typedef struct
{
    QMESH_MODEL_TYPE_T  type;           /*!< SIG model or vendor model. See \ref QMESH_MODEL_TYPE_T. */
    uint32_t            model_id;       /*!< Model ID. */
    uint16_t            ele_addr;       /*!< Element address on which this model is defined. */
    uint8_t             num_keys;       /*!< Number of keys. */
    uint8_t             status;         /*!< Status from the server. See \ref QMESH_RESULT_T. */
    const uint16_t      *app_idx_list;  /*!< Global application key indices list. */
} QMESH_CONFIG_MODEL_APP_LIST_T;

/*! \brief Model subscription list. */
typedef struct
{
    QMESH_MODEL_TYPE_T  type;            /*!< SIG model or vendor model. See \ref QMESH_MODEL_TYPE_T. */
    uint32_t            model_id;        /*!< Model ID. */
    uint16_t            ele_addr;        /*!< Element address. */
    uint8_t             status;          /*!< Status from the server. See \ref QMESH_RESULT_T. */
    uint8_t             num_addr;        /*!< Number of addresses. */
    const uint16_t      *subscribe_list; /*!< Subscription address list. */
} QMESH_CONFIG_MODEL_SUBSCRIPTION_LIST_T;

/*! \brief Model subscription status. */
typedef struct
{
    QMESH_MODEL_TYPE_T  type;           /*!< SIG model or vendor model. See \ref QMESH_MODEL_TYPE_T. */
    uint32_t            model_id;       /*!< SIG model ID or vendor model ID. */
    uint16_t            elm_addr;       /*!< Address of the element. */
    uint16_t            sub_addr;       /*!< Value of the address. */
    uint8_t             status;         /*!< Status code for the requesting message. */
} QMESH_CONFIG_MODEL_SUBSCRIPTION_STATUS_T;

/*! \brief Model publication status. */
typedef struct
{
    QMESH_MODEL_TYPE_T type; /*!< SIG model or vendor model. See \ref QMESH_MODEL_TYPE_T. */
    uint32_t model_id;       /*!< SIG model ID or vendor model ID. */
    uint16_t elm_addr;       /*!< Address of the element. */
    uint16_t pub_addr;       /*!< Value of the publish address. */
    uint16_t app_key_idx;    /*!< Index of the application key. */
    bool credential_flag;    /*!< Value of the friendship credential flag. */
    uint8_t pub_ttl;         /*!< Default TTL value for the outgoing messages. */
    uint8_t pub_period;      /*!< Period for periodic status publishing. */
    uint8_t pub_rtx_cnt;     /*!< Number of retransmissions for each published message. */
    uint8_t pub_rtx_int_stp; /*!< Number of 50-millisecond steps between retransmissions.*/
    uint8_t status;          /*!< Status code for the requesting message. */
} QMESH_CONFIG_MODEL_PUBLICATION_STATUS_T;

/*! \brief LPN poll timeout status. */
typedef struct
{
    uint16_t lpn_address;    /*!< Unicast address of the low power node. */
    uint32_t poll_timeout;   /*!< Current value of the poll timeout timer of the low power node. */
} QMESH_CONFIG_MODEL_LPN_TIMEOUT_STATUS_T;

/*! \brief Network transmit status. */
typedef struct
{
    uint8_t nw_tx_cnt; /*!< Number of transmissions for each network PDU originating from the node. */
    uint8_t nw_tx_int_step; /*!< Number of 10-millisecond steps between transmissions. */
} QMESH_CONFIG_MODEL_NW_TRANSMIT_STATUS_T;

/*! \brief Configuration relay status. */
typedef struct
{
    uint8_t relay_state;       /*!< Relay state for the node. */
    uint8_t relay_rtx_cnt;     /*!< Number of retransmissions on an advertising bearer
                                *   for each network PDU relayed by the node.
                                */
    uint8_t relay_rtx_int_stp; /*!< Number of 10-millisecond steps between retransmissions. */
} QMESH_CONFIG_MODEL_RELAY_STATUS_T;

/*! \brief Configuration beacon status. */
typedef struct
{
    QMESH_SECURE_NW_BEACON_STATE_T beacon_state; /*!< Secure Network Beacon State. See \ref QMESH_SECURE_NW_BEACON_STATE_T */
} QMESH_CONFIG_MODEL_BEACON_STATUS_T;

/*! \brief Configuration Default TTL status. */
typedef struct
{
    uint8_t ttl_state; /*!< Default TTL State */
} QMESH_CONFIG_MODEL_DEFAULT_TTL_STATUS_T;

/*! \brief Configuration GATT Proxy status. */
typedef struct
{
    QMESH_GATT_PROXY_STATE_T gatt_proxy_state; /*!< GATT proxy state. See \ref QMESH_GATT_PROXY_STATE_T */
} QMESH_CONFIG_MODEL_GATT_PROXY_STATUS_T;

/*! \brief Configuration Friend status. */
typedef struct
{
    uint8_t friend_state; /*!< Friend State */
} QMESH_CONFIG_MODEL_FRIEND_STATUS_T;


/*! \brief Configuration client command data. */
typedef union
{
    QMESH_CONFIG_BEACON_SET_T beacon_state;
    QMESH_CONFIG_COMPOSITION_DATA_GET_T page_num;
    QMESH_CONFIG_DEFAULT_TTL_SET_T default_ttl;
    QMESH_CONFIG_RELAY_SET_T relay_set;
    QMESH_CONFIG_NET_KEY_ADD_T net_key_add;
    QMESH_CONFIG_NET_KEY_UPDATE_T net_key_update;
    QMESH_CONFIG_NET_KEY_DELETE_T net_key_delete;
    QMESH_CONFIG_APP_KEY_ADD_T app_key_add;
    QMESH_CONFIG_APP_KEY_UPDATE_T app_key_update;
    QMESH_CONFIG_APP_KEY_DELETE_T app_key_delete;
    QMESH_CONFIG_APP_KEY_GET_T app_key_get;
    QMESH_CONFIG_KEY_REFRESH_PHASE_GET_T key_refresh_phase_get;
    QMESH_CONFIG_KEY_REFRESH_PHASE_SET_T key_refresh_phase_set;
    QMESH_CONFIG_GATT_PROXY_SET_T gatt_proxy_state;
    QMESH_CONFIG_NODE_IDENTITY_GET_T node_identity_get;
    QMESH_CONFIG_NODE_IDENTITY_SET_T node_identity_set;
    QMESH_CONFIG_PUBLICATION_GET_T pub_get;
    QMESH_CONFIG_MODEL_PUB_SET_T pub_set;
    QMESH_CONFIG_MODEL_PUB_VIR_ADDR_SET_T pub_vir_set;
    QMESH_CONFIG_SUBSCRIPTION_SET_T sub_set;
    QMESH_CONFIG_SUBSCRIPTION_VIR_SET_T vir_sub_set;
    QMESH_CONFIG_SUBSCRIPTION_DEL_ALL_T sub_del_all;
    QMESH_CONFIG_SUBSCRIPTION_GET_T sub_get;
    QMESH_CONFIG_APPKEY_BIND_UNBIND_T app_key_bind;
    QMESH_CONFIG_APP_GET_T app_get;
    QMESH_CONFIG_FRIEND_SET_T friend_state;
    QMESH_HEART_BEAT_PUBLICATION_T heart_beat_pub_set;
    QMESH_CONFIG_HEART_BEAT_SUB_SET_T heart_beat_sub_set;
    QMESH_CONFIG_LPN_POLL_TIMEOUT_GET_T lpn_poll_timeout_get;
    QMESH_CONFIG_NETWORK_TX_SET_T network_tx_set;
} QMESH_CONFIG_CLIENT_CMD_DATA_T;

/*! \brief Configuration client event information. */
typedef union
{
    QMESH_CONFIG_COMPOSITION_DATA_STATUS_T comp_data;               /*!< Parameter to \ref QMESH_CONFIG_COMPOSITION_DATA_STATUS_EVENT */
    QMESH_CONFIG_MODEL_PUBLICATION_STATUS_T pub_status;             /*!< Parameter to \ref QMESH_CONFIG_MODEL_PUB_STATUS_EVENT */
    QMESH_CONFIG_MODEL_SUBSCRIPTION_STATUS_T sub_status;            /*!< Parameter to \ref QMESH_CONFIG_MODEL_SUB_STATUS_EVENT */
    QMESH_CONFIG_MODEL_SUBSCRIPTION_LIST_T model_sub_list;          /*!< Parameter to \ref QMESH_CONFIG_SIG_MODEL_SUBSCRIBE_LIST_EVENT
                                                                     *   and \ref QMESH_CONFIG_VENDOR_MODEL_SUBSCRIBE_LIST_EVENT
                                                                     */
    QMESH_CONFIG_MODEL_NET_KEY_STATUS_T net_key_status;             /*!< Parameter to \ref QMESH_CONFIG_NET_KEY_STATUS_EVENT */
    QMESH_CONFIG_MODEL_NET_KEY_LIST_STATUS_T net_key_list;          /*!< Parameter to \ref QMESH_CONFIG_NET_KEY_LIST_EVENT */
    QMESH_CONFIG_MODEL_APP_KEY_STATUS_T app_key_status;             /*!< Parameter to \ref QMESH_CONFIG_APP_KEY_STATUS_EVENT */
    QMESH_CONFIG_MODEL_APP_KEY_LIST_STATUS_T app_key_list;          /*!< Parameter to \ref QMESH_CONFIG_APP_KEY_LIST_EVENT */
    QMESH_CONFIG_MODEL_NODE_IDENTITY_STATUS_T node_identity_status; /*!< Parameter to \ref QMESH_CONFIG_NODE_IDENTITY_STATUS_EVENT */
    QMESH_CONFIG_MODEL_APP_BIND_STATUS_T mdl_app_status;            /*!< Parameter to \ref QMESH_CONFIG_MODEL_APP_STATUS_EVENT */
    QMESH_CONFIG_MODEL_HEART_BEAT_PUB_STATUS_T hb_pub_status;       /*!< Parameter to \ref QMESH_CONFIG_HEART_BEAT_PUB_STATUS_EVENT */
    QMESH_CONFIG_MODEL_HEART_BEAT_SUB_STATUS_T hb_sub_status;       /*!< Parameter to \ref QMESH_CONFIG_HEART_BEAT_SUB_STATUS_EVENT */
    QMESH_CONFIG_MODEL_APP_LIST_T model_app_list;                   /*!< Parameter to \ref QMESH_CONFIG_SIG_MODEL_APP_LIST_EVENT
                                                                     *   and \ref QMESH_CONFIG_VENDOR_MODEL_APP_LIST_EVENT
                                                                     */
    QMESH_CONFIG_KEY_REFRESH_PHASE_STATUS_T key_refresh_status;     /*!< Parameter to \ref QMESH_CONFIG_KEY_REFRESH_PHASE_STATUS_EVENT */
    QMESH_CONFIG_MODEL_LPN_TIMEOUT_STATUS_T poll_timeout_status;    /*!< Parameter to \ref QMESH_CONFIG_LPN_POLL_TIMEOUT_STATUS_EVENT */
    QMESH_CONFIG_MODEL_NW_TRANSMIT_STATUS_T nw_transmit_status;     /*!< Parameter to \ref QMESH_CONFIG_NETWORK_TRANSMIT_STATUS_EVENT */
    QMESH_CONFIG_MODEL_RELAY_STATUS_T relay_status;                 /*!< Parameter to \ref QMESH_CONFIG_RELAY_STATUS_EVENT */
    QMESH_CONFIG_MODEL_BEACON_STATUS_T beacon_status;               /*!< Parameter to \ref QMESH_CONFIG_BEACON_STATUS_EVENT */
    QMESH_CONFIG_MODEL_DEFAULT_TTL_STATUS_T default_ttl_status;     /*!< Parameter to \ref QMESH_CONFIG_DEFAULT_TTL_STATUS_EVENT */
    QMESH_CONFIG_MODEL_GATT_PROXY_STATUS_T gatt_proxy_status;       /*!< Parameter to \ref QMESH_CONFIG_GATT_PROXY_STATUS_EVENT */
    QMESH_CONFIG_MODEL_FRIEND_STATUS_T friend_status;               /*!< Parameter to \ref QMESH_CONFIG_FRIEND_STATUS_EVENT */
} QMESH_CONFIG_CLIENT_EVT_INFO_T;

/*! \brief Configuration client event data. */
typedef struct
{
    QMESH_SUBNET_KEY_IDX_INFO_T    net_key_info;  /*!< Network key information. */
    const QMESH_NW_HEADER_INFO_T   *nw_hdr;       /*!< Network header information. See \ref QMESH_NW_HEADER_INFO_T. */
    QMESH_CONFIG_CLIENT_EVT_INFO_T event_info;    /*!< Config Client Event information. See \ref QMESH_CONFIG_CLIENT_EVT_INFO_T*/
} QMESH_CONFIG_CLIENT_EVT_DATA_T;

/*! \brief Configuration client header information. */
typedef struct
{
    QMESH_SUBNET_KEY_IDX_INFO_T  key_info;    /*!< Network key information. See \ref QMESH_SUBNET_KEY_IDX_INFO_T */
    uint16_t                     dst_addr;    /*!< Destination address. */
    uint16_t                     src_addr;    /*!< Unicast Address of the Element with Configuration Client Model. */
    uint8_t                      ttl;         /*!< TTL of the message. */
} QMESH_CONFIG_CLIENT_MSG_INFO_T;

/*============================================================================*
    Public API
 *============================================================================*/

#ifdef __cplusplus
 extern "C" {
#endif

/*----------------------------------------------------------------------------*
 * QmeshConfigSendClientCmd
 */
/*! \brief Sends a configuration client command to the node.
  *        A command response is provided in \ref QMESH_CONFIG_CLIENT_EVT_DATA_T.
  *
  *  \param[in] msg_info   Network layer information needed to send the command.
  *                        See \ref QMESH_CONFIG_CLIENT_MSG_INFO_T.
  *  \param[in] cmd       Command to be sent. See \ref QMESH_CONFIG_CLIENT_CMD_T.
  *  \param[in] cmd_data  Pointer to the data relevant for the command.
  *                       See \ref QMESH_CONFIG_CLIENT_CMD_DATA_T.
  *
  *  \returns
  *      return \ref QMESH_RESULT_T.
  *
  *  NOTE:
  *  1) This API queues the message for transmission, and the status it returns
  *     indicates whether the queueing was successful or not.
  *  2) A separate event QMESH_APP_PAYLOAD_EVENT with the appropriate status will
  *     be sent when the message is actually attempted for transmission.
  */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshConfigSendClientCmd(const QMESH_CONFIG_CLIENT_MSG_INFO_T *msg_info,
                                               QMESH_CONFIG_CLIENT_CMD_T cmd,
                                               const QMESH_CONFIG_CLIENT_CMD_DATA_T *cmd_data);


/*----------------------------------------------------------------------------*
 * QmeshConfigModelClientHandler
 */
/*! \brief Handler for configuration model client messages.
  *
  *  \param[in] nw_hdr      Network header information.
  *                         See \ref QMESH_NW_HEADER_INFO_T.
  *  \param[in] key_info    Application payload security information.
  *                         See \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T.
  *  \param[in] elem_config Data specific to each element.
  *                         See \ref QMESH_ELEMENT_CONFIG_T.
  *  \param[in] model_data  Model data. See \ref QMESH_MODEL_DATA_T.
  *  \param[in] msg         Pointer to the config model data payload.
  *  \param[in] msg_len     Config model data payload length.
  *
  *  \returns
  *      return \ref QMESH_RESULT_T.
  */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshConfigModelClientHandler(const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
                                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                                    QMESH_MODEL_DATA_T *model_data,
                                                    const uint8_t *msg,
                                                    uint16_t msg_len);

#ifdef __cplusplus
 }
#endif
/*!@} */
#endif /* __QMESH_CONFIG_CLIENT_H__ */


