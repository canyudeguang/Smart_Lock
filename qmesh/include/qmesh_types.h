/*=============================================================================
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_types.h
 *  \brief Qmesh library data types
 *
 *   This file contains the different data types used in Qmesh library.
 *
 */
/******************************************************************************/

#ifndef _QMESH_TYPES_H_
#define _QMESH_TYPES_H_

#include "qmesh_data_types.h"
#include "qmesh_result.h"
#include "qmesh_aksl_list.h"

/*! \addtogroup Core_General
 * @{
 */

/*============================================================================*
    Macro Definitions
 *============================================================================*/

/*! \brief Macro to convert a number in half words to a number in octets. */
#define QMESH_CONVERT_HWORDS_TO_OCTETS(hw)              ((hw) * 2)

/*! \brief Macro to convert a number in octets to a number in half words. */
#define QMESH_CONVERT_OCTETS_TO_HWORDS(o)               (((o) + 1) / 2)

/*! \brief Macro to convert a number in words to a number in octets. */
#define QMESH_CONVERT_WORDS_TO_OCTETS(w)                ((w) * 4)

/*! \brief Macro to convert a number in octets to a number in words. */
#define QMESH_CONVERT_OCTETS_TO_WORDS(o)                (((o) + 3) / 4)

/*! \brief UUID size in octets. */
#define QMESH_UUID_SIZE_OCTETS                  (16)

/*! \brief UUID size in half words. */
#define QMESH_UUID_SIZE_HWORDS              (QMESH_CONVERT_OCTETS_TO_HWORDS(QMESH_UUID_SIZE_OCTETS))

/*! \brief Mesh security key size in octets. */
#define QMESH_SECURITY_KEY_OCTETS               (16)

/*! \brief Mesh security key size in half words. */
#define QMESH_SECURITY_KEY_HWORDS           (QMESH_CONVERT_OCTETS_TO_HWORDS(QMESH_SECURITY_KEY_OCTETS))

/*! \brief Qmesh network key size in octets. */
#define QMESH_NETWORK_KEY_SIZE_OCTETS       (QMESH_SECURITY_KEY_OCTETS)

/*! \brief Qmesh network key size in half words. */
#define QMESH_NETWORK_KEY_SIZE_HWORDS       (QMESH_SECURITY_KEY_HWORDS)

/*! \brief Qmesh application key size in octets. */
#define QMESH_APP_KEY_SIZE_OCTETS           (QMESH_SECURITY_KEY_OCTETS)

/*! \brief Qmesh application key size in half words. */
#define QMESH_APP_KEY_SIZE_HWORDS           (QMESH_SECURITY_KEY_HWORDS)

/*! \brief Network ID Size in Octets and Halfwords. */
#define QMESH_NETWORK_ID_OCTETS             (8)
#define QMESH_NETWORK_ID_SIZE_HWORDS        (QMESH_CONVERT_OCTETS_TO_HWORDS(QMESH_NETWORK_ID_OCTETS))

/*! \brief Invalid bearer handle. */
#define QMESH_BEARER_HANDLE_INVALID         ((QMESH_BEARER_HANDLE_T) NULL)

/*! \brief SIG model ID length. */
#define QMESH_SIG_MODEL_ID_LENGTH           (2)

/*! \brief Vendor model ID length. */
#define QMESH_VENDOR_MODEL_ID_LENGTH        (4)

#define TRANSPORT_PDU_MIN_SIZE_IN_BYTES     (5)
#define TRANSPORT_PDU_MAX_SIZE_IN_BYTES     (16)
#define TRANS_SEG_PDU_LEN_BYTES             (12)

/*!\brief Unassigned address */
#define QMESH_UNASSIGNED_ADDRESS            (0x0000u)

/*!\brief Unassigned key index */
#define QMESH_INVALID_KEY_INDEX             (0xFFFFu)

/*! \brief Primary element index. */
#define QMESH_PRIMARY_ELEMENT_INDEX         (0)

/*! \brief Configuration server model ID. */
#define QMESH_CONFIG_SERVER_MODEL_ID                    0x0000u

/*! \brief Configuration client model ID. */
#define QMESH_CONFIG_CLIENT_MODEL_ID                    0x0001u

/*! \brief Health server model ID. */
#define QMESH_HEALTH_SERVER_MODEL_ID                    0x0002u

/*! \brief Health client model ID. */
#define QMESH_HEALTH_CLIENT_MODEL_ID                    0x0003u

/*! \brief Mask to force the stack to use Default TTL. */
#define QMESH_USE_DEFAULT_TTL               0xFF

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/

/*! \brief Role of the device */
typedef enum
{
    QMESH_ROLE_DEVICE                        = 0, /*!< Node */
    QMESH_ROLE_PROVISIONER                   = 1, /*!< Provisioner */
} QMESH_ROLE_T;

/*! \brief Bearer types. */
typedef enum
{
    QMESH_BEARER_TYPE_LE_ADV_PROVISIONING       = 0,      /*!< Provisioning LE Advert Bearer. */
    QMESH_BEARER_TYPE_LE_ADV_NETWORK            = 1,      /*!< Network LE Advert Bearer. */
    QMESH_BEARER_TYPE_PROXY_PROVISIONING_SERVER = 2,      /*!< Provisioning Proxy bearer in server role. */
    QMESH_BEARER_TYPE_PROXY_NETWORK_SERVER      = 3,      /*!< Network Proxy bearer in server role. */
    QMESH_BEARER_TYPE_PROXY_PROVISIONING_CLIENT = 4,      /*!< Provisioning Proxy bearer in client role. */
    QMESH_BEARER_TYPE_PROXY_NETWORK_CLIENT      = 5,      /*!< Network Proxy bearer in client role. */
    QMESH_BEARER_TYPE_MAX
} QMESH_BEARER_TYPE_T;

/*! \brief Mesh proxy filter type */
typedef enum
{
    QMESH_PROXY_FILTER_TYPE_WHITELIST = 0,  /*!< Proxy whitelist filter type. */
    QMESH_PROXY_FILTER_TYPE_BLACKLIST = 1   /*!< Proxy blacklist filter type. */
} QMESH_PROXY_FILTER_TYPE_T;

/*! \brief Mesh proxy service data type used in the advertising. */
typedef enum
{
    QMESH_PROXY_ADV_ID_TYPE_NETWORK = 0,    /*!< Proxy service data in advertising is the network ID type. */
    QMESH_PROXY_ADV_ID_TYPE_NODE    = 1,    /*!< Proxy service data in advertising is the node identity type. */
} QMESH_PROXY_ADV_ID_TYPE_T;

/*! \brief Relay state. */
typedef enum
{
    QMESH_RELAY_STATE_DISABLED      = 0, /*!< Node support relay feature is disabled. */
    QMESH_RELAY_STATE_ENABLED       = 1, /*!< Node support relay feature is enabled. */
    QMESH_RELAY_STATE_NOT_SUPPORTED = 2  /*!< Relay feature is not supported. */
} QMESH_RELAY_STATE_T;

/*! \brief Key refresh phases. */
typedef enum
{
    QMESH_KEY_REFRESH_PHASE_0   = 0,    /*!< Distribution of the new keys. */
    QMESH_KEY_REFRESH_NORMAL_OPERATION = QMESH_KEY_REFRESH_PHASE_0,
    QMESH_KEY_REFRESH_PHASE_1   = 1,    /*!< Use the old key to transmit and either old or new key to receive. */
    QMESH_KEY_REFRESH_PHASE_2   = 2,    /*!< Use the new key to transmit and both keys to receive. */
    QMESH_KEY_REFRESH_PHASE_3   = 3     /*!< Revoke old keys and move to the normal phase. */
} QMESH_KEY_REFRESH_PHASE_T;

/*! \brief IV update state.
 *  \note1hang See Mesh Profile Specification Section-3.10.5.
 */
typedef enum
{
    QMESH_IV_UPDATE_STATE_NORMAL        = 0,    /*!< IV update is not in progress. */
    QMESH_IV_UPDATE_STATE_IN_PROGRESS   = 1     /*!< IV update is in progress. */
} QMESH_IV_UPDATE_STATE_T;

/*! \brief Secure network beacon broadcast state. */
typedef enum
{
    QMESH_SECURE_NW_BEACON_STATE_STOP   = 0,    /*!< Node stops broadcasting secure network beacons. */
    QMESH_SECURE_NW_BEACON_STATE_START  = 1     /*!< Node broadcasts secure network beacons. */
} QMESH_SECURE_NW_BEACON_STATE_T;

/*! \brief Type of model. */
typedef enum
{
    QMESH_MODEL_TYPE_SIG,       /*!< SIG defined model. */
    QMESH_MODEL_TYPE_VENDOR,    /*!< Vendor-specific model. */
    QMESH_MODEL_TYPE_ANY        /*!< Any model type. */
} QMESH_MODEL_TYPE_T;

/*! \brief OOB information mask is used to help drive the provisioning process by
 *         indicating the availability of OOB data, such as a public key of the device.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Table-3.5.4.
 */
typedef enum
{
    QMESH_OOB_INFO_MASK_NONE                = 0,       /*!< No OOB information.              */
    QMESH_OOB_INFO_MASK_OTHER               = 1 << 0,  /*!< Other.                           */
    QMESH_OOB_INFO_MASK_ELECTRONICURI       = 1 << 1,  /*!< Electronic/URI.                  */
    QMESH_OOB_INFO_MASK_QRCODE              = 1 << 2,  /*!< 2D machine-readable code.        */
    QMESH_OOB_INFO_MASK_BARCODE             = 1 << 3,  /*!< Bar code.                        */
    QMESH_OOB_INFO_MASK_NFC                 = 1 << 4,  /*!< Near field communication (NFC).  */
    QMESH_OOB_INFO_MASK_NUMBER              = 1 << 5,  /*!< Mask number.                     */
    QMESH_OOB_INFO_MASK_STRING              = 1 << 6,  /*!< Mask string.                     */
    /* 4 RFU Bits. */
    QMESH_OOB_INFO_MASK_ONBOX               = 1 << 11,  /*!< On the box.                     */
    QMESH_OOB_INFO_MASK_INBOX               = 1 << 12,  /*!< Inside the box.                 */
    QMESH_OOB_INFO_MASK_ONPAPER             = 1 << 13,  /*!< On a piece of paper.            */
    QMESH_OOB_INFO_MASK_INMANUAL            = 1 << 14,  /*!< Inside a manual.                */
    QMESH_OOB_INFO_MASK_ONDEVICE            = 1 << 15   /*!< On the device.                  */
} QMESH_OOB_INFO_MASK_T;

/*! \brief Algorithm supported for the provisioning mask.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Table-5.18.
 */
typedef enum
{
    QMESH_PROV_ALGO_MASK_NONE               = 0,        /*!< None.                      */
    QMESH_PROV_ALGO_MASK_FIPSP256EC         = 1 << 0    /*!< FIPS P-256 elliptic curve. */
} QMESH_PROV_ALGO_MASK_T;

/*! \brief Public key type mask.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Table-5.19.
 */
typedef enum
{
    QMESH_PUBLIC_KEY_MASK_NONE              = 0,        /*!< None.                                 */
    QMESH_PUBLIC_KEY_MASK_OOB               = 1 << 0    /*!< Public key OOB information available. */
} QMESH_PUBLIC_KEY_MASK_T;

/*! \brief Static OOB type mask.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Table-5.20.
 */
typedef enum
{
    QMESH_STATIC_MASK_NONE                  = 0,        /*!< None.                                */
    QMESH_STATIC_MASK_OOB_AVAILABLE         = 1 << 0    /*!< Static OOB information is available. */
} QMESH_STATIC_OOB_MASK_T;

/*! \brief Output OOB action mask.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Table-5.22.
 */
typedef enum
{
    QMESH_OUTPUT_OOB_ACTION_MASK_NONE           = 0,        /*!< None.                   */
    QMESH_OUTPUT_OOB_ACTION_MASK_BLINK          = 1 << 0,   /*!< Blink.                  */
    QMESH_OUTPUT_OOB_ACTION_MASK_BEEP           = 1 << 1,   /*!< Beep.                   */
    QMESH_OUTPUT_OOB_ACTION_MASK_VIBRATE        = 1 << 2,   /*!< Vibrate.                */
    QMESH_OUTPUT_OOB_ACTION_MASK_NUMERIC        = 1 << 3,   /*!< Output is numeric.      */
    QMESH_OUTPUT_OOB_ACTION_MASK_ALPHANUMERIC   = 1 << 4    /*!< Output is alphanumeric. */
} QMESH_OUTPUT_OOB_ACTION_MASK_T;

/*! \brief Input OOB action mask.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Table-5.24.
 */
typedef enum
{
    QMESH_INPUT_OOB_ACTION_MASK_NONE            = 0,        /*!< None.                  */
    QMESH_INPUT_OOB_ACTION_MASK_PUSH            = 1 << 0,   /*!< Push.                  */
    QMESH_INPUT_OOB_ACTION_MASK_TWIST           = 1 << 1,   /*!< Twist.                 */
    QMESH_INPUT_OOB_ACTION_MASK_NUMBER          = 1 << 2,   /*!< Input is a number.     */
    QMESH_INPUT_OOB_ACTION_MASK_ALPHANUMERIC    = 1 << 3    /*!< Input is alphanumeric. */
} QMESH_INPUT_OOB_ACTION_MASK_T;

/*! \brief Algorithm supported for the provisioning mask.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Table-5.18.
 */
typedef enum
{
    QMESH_PROV_ALGO_FIPSP256EC              = 0,    /*!< FIPS P-256 elliptic curve.       */
    QMESH_PROV_ALGO_INVALID,                        /*!< Any value above this is invalid. */
} QMESH_PROV_ALGO_T;

/*! \brief Public key type mask.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Table-5.19.
 */
typedef enum
{
    QMESH_PUBLIC_KEY_NO_OOB             = 0,    /*!< No public key OOB information is available. */
    QMESH_PUBLIC_KEY_OOB                = 1,    /*!< Public key OOB information is available.    */
    QMESH_PUBLIC_KEY_INVALID,                   /*!< Any value above this is invalid.            */
} QMESH_PUBLIC_KEY_T;

/*! \brief Output OOB action.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Table-5.22.
 */
typedef enum
{
    QMESH_OUTPUT_OOB_ACTION_BLINK           = 0,    /*!< Blink --   Numeric data.                  */
    QMESH_OUTPUT_OOB_ACTION_BEEP            = 1,    /*!< Beep --    Numeric data.                  */
    QMESH_OUTPUT_OOB_ACTION_VIBRATE         = 2,    /*!< Vibrate -- Numeric data.                  */
    QMESH_OUTPUT_OOB_ACTION_NUMERIC         = 3,    /*!< Output is numeric -- Numeric data.        */
    QMESH_OUTPUT_OOB_ACTION_ALPHANUMERIC    = 4,    /*!< Output is alphanumeric - Array of octets. */
    QMESH_OUTPUT_OOB_ACTION_INVALID                 /*!< Any value above this is invalid.          */
} QMESH_OUTPUT_OOB_ACTION_T;

/*! \brief Input OOB Action.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Table-5.24.
 */
typedef enum
{
    QMESH_INPUT_OOB_ACTION_PUSH             = 0,    /*!< Push                 --   Numeric data.  */
    QMESH_INPUT_OOB_ACTION_TWIST            = 1,    /*!< Twist                --   Numeric data.  */
    QMESH_INPUT_OOB_ACTION_NUMBER           = 2,    /*!< Input is a number    --   Numeric data.  */
    QMESH_INPUT_OOB_ACTION_ALPHANUMERIC     = 3,    /*!< Input is alphanumeric -- Array of octets */
    QMESH_INPUT_OOB_ACTION_INVALID                  /*!< Any value above these is invalid.        */
} QMESH_INPUT_OOB_ACTION_T;

/*! \brief Authentication method to be used during provisioning.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Table-5.28.
 */
typedef enum
{
    QMESH_AUTH_METHOD_OOB_NONE              = 0,    /*!< No OOB authentication is used.     */
    QMESH_AUTH_METHOD_OOB_STATIC            = 1,    /*!< Static OOB authentication is used. */
    QMESH_AUTH_METHOD_OOB_OUTPUT            = 2,    /*!< Output OOB authentication is used. */
    QMESH_AUTH_METHOD_OOB_INPUT             = 3,    /*!< Input OOB authentication is used.  */
    QMESH_AUTH_METHOD_INVALID                       /*!< Any value above thess is invalid.  */
} QMESH_AUTH_METHOD_T;

/*! \brief Device features supported.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Table-4.3.
 */
typedef enum
{
    QMESH_DEVICE_FEATURE_MASK_NONE          = 0,        /*!< None of the features are supported. */
    QMESH_DEVICE_FEATURE_MASK_RELAY         = 1 << 0,   /*!< Relay feature bitmask.         */
    QMESH_DEVICE_FEATURE_MASK_PROXY         = 1 << 1,   /*!< Proxy feature bitmask.         */
    QMESH_DEVICE_FEATURE_MASK_FRIEND        = 1 << 2,   /*!< Friend feature bitmask.        */
    QMESH_DEVICE_FEATURE_MASK_LOWPOWER      = 1 << 3    /*!< Low power feature bitmask.     */
} QMESH_DEVICE_FEATURE_MASK_T;

/*! \brief Mesh provision status values. */
typedef enum
{
    QMESH_PROV_STATUS_START,         /*!< Provision start received. */
    QMESH_PROV_STATUS_CAPABILITIES,  /*!< Provision capabilities received. */
    QMESH_PROV_STATUS_PUB_KEY,       /*!< Provision public key received. */
    QMESH_PROV_STATUS_IP_COMP,       /*!< Provision input complete received. */
    QMESH_PROV_STATUS_CONFIRMATION,  /*!< Provision confirmation received. */
    QMESH_PROV_STATUS_RANDOM,        /*!< Provision random received. */
    QMESH_PROV_STATUS_DATA,          /*!< Provision data received. */
    QMESH_PROV_STATUS_MAX
} QMESH_PROV_STATUS_T;

/*! \brief Node identity values. */
typedef enum
{
    QMESH_NODE_IDENTITY_STOPPED,        /*!< Node identity for a subnet is stopped. */
    QMESH_NODE_IDENTITY_RUNNING,        /*!< Node identity for a subnet is running. */
    QMESH_NODE_IDENTITY_NOT_SUPPORTED,  /*!< Node identity is not supported. */
    QMESH_NODE_IDENTITY_PROHIBITED      /*!< Node identity value is prohibited. */
} QMESH_NODE_IDENTITY_T;

/*! \brief Qmesh event types related to the local changes or due to any
 *         network event, such as provisioning, key, network IV, or device ID updates
 */
typedef enum
{
    QMESH_PROVISION_EVENT_GROUP_ID = 0x0000,    /*!< Provisioning procedure related events. */
    QMESH_UNPROVISIONED_DEVICE_EVENT,           /*!< Sent when the device is enabled to support
                                                 *          the provisioner role and discover devices
                                                 *          has been initiated. See QmeshStartDeviceDiscovery(). \n
                                                 *          Parameter: \ref QMESH_UUID_T (i.e., uint16s) in big endian format
                                                 */
    QMESH_PROVISION_INVITE_EVENT,               /*!< Unprovisioned device will receive this event
                                                 *          when a provisioner starts provisioning the device. \n
                                                 *          Parameter: \ref QMESH_PROV_INVITE_DATA_T
                                                 */
    QMESH_PROV_LINK_OPEN_EVENT,                 /*!< Indicates that a provisioning link is opened. \n
                                                 *          Parameter: None
                                                 */
    QMESH_PROV_LINK_CLOSE_EVENT,                /*!< Indicates that the provisioning link is closed. \n
                                                 *          Parameter: None
                                                 */

    QMESH_DEVICE_CAPABILITY_EVENT,              /*!< Received the device capabilities of a remote device. \n
                                                 *          Parameter: See \ref QMESH_REMOTE_NODE_CAPS_DATA_T
                                                 */
    QMESH_DEVICE_PUBLIC_KEY_ACTION_EVENT,       /*!< Requests a public key for a peer device; see QmeshSetDevicePublicKey().
                                                 *          Parameter: None
                                                 */
    QMESH_OOB_ACTION_EVENT,                     /*!< Indicates that an OOB action is selected for OOB
                                                 *          authentication during provisioning; see QmeshSetOobData().
                                                 *          Parameter: \ref QMESH_PROV_SELECTED_CAPS_T
                                                 */

    QMESH_PROVISION_COMPLETE_EVENT,             /*!< Indicates that provisioning is complete. The associated
                                                 *          error code indicates the result of the operation. The event
                                                 *          is received at both the provisioner and the node. \n
                                                 *          Parameter: \ref QMESH_PROV_COMPLETE_DATA_T
                                                 */

    QMESH_PROVISION_STATUS_EVENT,               /*!< Indicates the status of an ongoing provisioning procedure. \n
                                                 *          Parameter: \ref QMESH_PROV_STATUS_T
                                                 */

    QMESH_PUBLIC_KEY_AVAILABLE_EVENT,           /*!< Indicates that public-private key generation is complete.
                                                 *          Sent after calling \ref QmeshSetProvisioningRole if
                                                 *          role is \ref QMESH_ROLE_DEVICE and result code is
                                                 *          \ref QMESH_RESULT_INPROGRESS.
                                                 *          Parameter: None
                                                 */

    QMESH_GENERIC_EVENT_GROUP_ID = 0x0100,      /*!< Generic MESH stack events. */
    QMESH_PROXY_FILTER_STATUS_EVENT,            /*!< Received in response to a proxy filter configuration message. \n
                                                 *          Parameter: \ref QMESH_PROXY_FILTER_STATUS_T
                                                 */
    QMESH_APP_PAYLOAD_EVENT,                    /*!< Indicates that application data is received. */
    QMESH_TEST_PTS_ACCESS_PDU_EVT,
    QMESH_GET_DEVICE_KEY_EVENT,                 /*!< In the provisioner role, this event is used to fetch
                                                 *          the device key for the specified element address. \n
                                                 *          Parameter: \ref QMESH_DEVICE_KEY_INFO_T
                                                 */
    QMESH_PROXY_DISCONNECT_EVENT,               /*!< Event is received when proxy server/client
                                                 *          encountered an error upon specification expects
                                                 *          a disconnection. \n
                                                 *          Parameter: \ref QMESH_PROXY_DISCONNECT_EVENT_DATA_T.
                                                 */

    QMESH_EVENT_CONFIG_MODEL_GROUP_ID = 0x0200, /*!< Configuration model events. */
    QMESH_CONFIG_NET_KEY_STATUS_EVENT,          /*!< The event is received as a response to a \ref QMESH_CONFIG_CLI_NETWORK_KEY_ADD,
                                                 *          \ref QMESH_CONFIG_CLI_NETWORK_KEY_UPDATE, or \ref QMESH_CONFIG_CLI_NETWORK_KEY_DELETE
                                                 *          command request to the QmeshConfigSendClientCmd() function.
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_NET_KEY_STATUS_T.
                                                 */
    QMESH_CONFIG_BEACON_STATUS_EVENT,           /*!< Received as a response to a \ref QMESH_CONFIG_CLI_BEACON_SET or
                                                 *          \ref QMESH_CONFIG_CLI_BEACON_GET command request to
                                                 *          the QmeshConfigSendClientCmd() function. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_BEACON_STATUS_T.
                                                 */
    QMESH_CONFIG_DEFAULT_TTL_STATUS_EVENT,      /*!< Received as a response to a \ref QMESH_CONFIG_CLI_DEFAULT_TTL_GET
                                                 *          or \ref QMESH_CONFIG_CLI_DEFAULT_TTL_SET command to
                                                 *          the QmeshConfigSendClientCmd() function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_DEFAULT_TTL_STATUS_T.
                                                 */
    QMESH_CONFIG_RELAY_STATUS_EVENT,            /*!< Received as a response to a \ref QMESH_CONFIG_CLI_RELAY_GET or
                                                 *          \ref QMESH_CONFIG_CLI_RELAY_SET command in the QmeshConfigSendClientCmd()
                                                 *          function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_RELAY_STATUS_T.
                                                 */
    QMESH_CONFIG_APP_KEY_STATUS_EVENT,          /*!< Indicates that the target device responded to a \ref QMESH_CONFIG_CLI_APP_KEY_ADD,
                                                 *          \ref QMESH_CONFIG_CLI_APP_KEY_UPDATE, or \ref QMESH_CONFIG_CLI_APP_KEY_DELETE
                                                 *          command in the QmeshConfigSendClientCmd() function call.
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          as \ref QMESH_CONFIG_MODEL_APP_KEY_STATUS_T.
                                                 */
    QMESH_CONFIG_FRIEND_STATUS_EVENT,           /*!< Received in response to a \ref QMESH_CONFIG_CLI_FRIEND_GET or
                                                 *          \ref QMESH_CONFIG_CLI_FRIEND_SET command in the
                                                 *          QmeshConfigSendClientCmd() function call.
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_FRIEND_STATUS_T.
                                                 */
    QMESH_CONFIG_GATT_PROXY_STATUS_EVENT,       /*!< Received in response to a \ref QMESH_CONFIG_CLI_GATT_PROXY_STATE_GET or
                                                 *          \ref QMESH_CONFIG_CLI_GATT_PROXY_STATE_SET command in the QmeshConfigSendClientCmd()
                                                 *          function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_GATT_PROXY_STATUS_T.
                                                 */
    QMESH_CONFIG_NODE_IDENTITY_STATUS_EVENT,    /*!< Node response to a \ref QMESH_CONFIG_CLI_NODE_IDENTITY_GET or
                                                 *          \ref QMESH_CONFIG_CLI_NODE_IDENTITY_SET command in
                                                 *          the QmeshConfigSendClientCmd function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_NODE_IDENTITY_STATUS_T.
                                                 */
    QMESH_CONFIG_MODEL_PUB_STATUS_EVENT,        /*!< Response to a \ref QMESH_CONFIG_CLI_PUBLICATION_GET,
                                                 *          \ref QMESH_CONFIG_CLI_PUBLICATION_SET, or
                                                 *          QMESH_CONFIG_CLI_PUBLICATION_VIRTUAL_ADDR_SET command in
                                                 *          the QmeshConfigSendClientCmd() function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_PUBLICATION_STATUS_T.
                                                 */
    QMESH_CONFIG_MODEL_SUB_STATUS_EVENT,        /*!< Response received for a \ref QMESH_CONFIG_CLI_SUBSCRIPTION_SET,
                                                 *          \ref QMESH_CONFIG_CLI_SUBSCRIPTION_VIRTUAL_ADDR_SET, or
                                                 *          \ref QMESH_CONFIG_CLI_SUBSCRIPTION_DEL_ALL command in
                                                 *          the QmeshConfigSendClientCmd() function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_SUBSCRIPTION_STATUS_T.
                                                 */
    QMESH_CONFIG_NODE_RESET_STATUS_EVENT,       /*!< Indicates the target node was reset as a result of a
                                                 *          \ref QMESH_CONFIG_CLI_NODE_RESET command in
                                                 *          the QmeshConfigSendClientCmd() function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T to
                                                 *          be ignored.
                                                 */
    QMESH_CONFIG_APP_KEY_LIST_EVENT,            /*!< App key list event generated for a \ref QMESH_CONFIG_CLI_APP_KEY_GET
                                                 *          command in the QmeshConfigSendClientCmd() function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_APP_KEY_LIST_STATUS_T.
                                                 */
    QMESH_CONFIG_NET_KEY_LIST_EVENT,            /*!< Net key list event generated for a \ref QMESH_CONFIG_CLI_NETWORK_KEY_GET
                                                 *          command in the QmeshConfigSendClientCmd() function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_NET_KEY_LIST_STATUS_T.
                                                 */
    QMESH_CONFIG_HEART_BEAT_PUB_STATUS_EVENT,   /*!< Heart beat publication status generated for a \ref QMESH_CONFIG_CLI_HEART_BEAT_PUBLIC_GET or
                                                 *          \ref QMESH_CONFIG_CLI_HEART_BEAT_PUBLIC_SET command in
                                                 *          the QmeshConfigSendClientCmd() function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_HEART_BEAT_PUB_STATUS_T.
                                                 */
    QMESH_CONFIG_HEART_BEAT_SUB_STATUS_EVENT,   /*!< Response received for a \ref QMESH_CONFIG_CLI_HEART_BEAT_SUBCRIBE_GET or
                                                 *          \ref QMESH_CONFIG_CLI_HEART_BEAT_SUBCRIBE_SET command in
                                                 *          the QmeshConfigSendClientCmd() function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_HEART_BEAT_SUB_STATUS_T.
                                                 */
    QMESH_CONFIG_MODEL_APP_STATUS_EVENT,        /*!< Model app status generated for a \ref QMESH_CONFIG_CLI_APP_BIND or
                                                 *          \ref QMESH_CONFIG_CLI_APP_UNBIND command in
                                                 *          the QmeshConfigSendClientCmd() function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_APP_BIND_STATUS_T.
                                                 */
    QMESH_CONFIG_COMPOSITION_DATA_STATUS_EVENT, /*!< Response to a \ref QMESH_CONFIG_CLI_COMPOSITION_DATA_GET command in
                                                 *          the QmeshConfigSendClientCmd() function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_COMPOSITION_DATA_STATUS_T.
                                                 */
    QMESH_CONFIG_KEY_REFRESH_PHASE_STATUS_EVENT,/*!< Key refresh phase status generated for a \ref QMESH_CONFIG_CLI_KEY_REFRESH_PHASE_GET
                                                 *          or \ref QMESH_CONFIG_CLI_KEY_REFRESH_PHASE_SET command in
                                                 *          the QmeshConfigSendClientCmd() function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_KEY_REFRESH_PHASE_STATUS_T.
                                                 */
    QMESH_CONFIG_SIG_MODEL_APP_LIST_EVENT,      /*!< Response to a \ref QMESH_CONFIG_CLI_SIG_MODEL_APP_GET command in
                                                 *          the QmeshConfigSendClientCmd() function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_APP_LIST_T.
                                                 */
    QMESH_CONFIG_VENDOR_MODEL_APP_LIST_EVENT,   /*!< Response to a \ref QMESH_CONFIG_CLI_VENDOR_MODEL_APP_GET command in
                                                 *          the QmeshConfigSendClientCmd() function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_APP_LIST_T.
                                                 */
    QMESH_CONFIG_SIG_MODEL_SUBSCRIBE_LIST_EVENT, /*!< Response received for a \ref QMESH_CONFIG_CLI_SUBSCRIPTION_GET command in
                                                  *         the QmeshConfigSendClientCmd() function call. \n
                                                  *         Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                  *         \ref QMESH_CONFIG_MODEL_SUBSCRIPTION_LIST_T.
                                                  */
    QMESH_CONFIG_VENDOR_MODEL_SUBSCRIBE_LIST_EVENT, /*!< Response to a \ref QMESH_CONFIG_CLI_VENDOR_SUBSCRIPTION_GET command in
                                                     *      the QmeshConfigSendClientCmd() function call. \n
                                                     *      Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                     *      \ref QMESH_CONFIG_MODEL_SUBSCRIPTION_LIST_T.
                                                     */
    QMESH_CONFIG_IDENTITY_SET_EVENT,            /*!< Configuration identity set event received on the node.
                                                 *          Parameter: \ref QMESH_CONFIG_NODE_IDENTITY_INFO_T
                                                 */
    QMESH_CONFIG_IDENTITY_GET_EVENT,            /*!< Configuration identity get event received on the node.
                                                 *          Parameter: \ref QMESH_CONFIG_NODE_IDENTITY_INFO_T
                                                 */
    QMESH_CONFIG_GATT_PROXY_SET_EVENT,          /*!< Configuration GATT proxy set event received on the node.
                                                 *          Parameter: \ref QMESH_CONFIG_GATT_PROXY_INFO_T
                                                 */
    QMESH_CONFIG_GATT_PROXY_GET_EVENT,          /*!< Configuration GATT proxy get event received on the node.
                                                 *          Parameter: \ref QMESH_CONFIG_GATT_PROXY_INFO_T
                                                 */
    QMESH_CONFIG_LPN_POLL_TIMEOUT_STATUS_EVENT, /*!< Response to a \ref QMESH_CONFIG_CLI_LPN_POLL_TIMEOUT_GET command in
                                                  *         the QmeshConfigSendClientCmd() function call. \n
                                                  *         Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                  *         \ref QMESH_CONFIG_MODEL_LPN_TIMEOUT_STATUS_T.
                                                  *         NOTE: This event is currently not supported.
                                                  */
    QMESH_CONFIG_NETWORK_TRANSMIT_STATUS_EVENT, /*!< Response to a \ref QMESH_CONFIG_CLI_NETWORK_TRANSMIT_GET or
                                                 *          \ref QMESH_CONFIG_CLI_NETWORK_TRANSMIT_SET command in
                                                 *          the QmeshConfigSendClientCmd() function call. \n
                                                 *          Parameter: \ref QMESH_CONFIG_CLIENT_EVT_DATA_T with \ref QMESH_CONFIG_CLIENT_EVT_INFO_T as
                                                 *          \ref QMESH_CONFIG_MODEL_NW_TRANSMIT_STATUS_T.
                                                 */
    QMESH_NODE_RESET_EVENT,                     /*!< Node has received a reset event on the node.
                                                 *          Parameter: uint8_t stores the Provisioned network index.
                                                 */
    QMESH_CONFIG_APP_BIND_UNBIND_EVENT,         /*!< Configuration model app bind or unbind event was received on the node. \n
                                                 *          Parameter: \ref QMESH_CONFIG_APP_BIND_EVENT_T
                                                 */
    QMESH_CONFIG_SUBSCRIPTION_SET_EVENT,        /*!< Configuration model subscription set event. \n
                                                 *          Parameter: \ref QMESH_CONFIG_SUB_SET_EVENT_T
                                                 */
    QMESH_CONFIG_PUBLICATION_SET_EVENT,         /*!< Configuration model publication set event. \n
                                                 *          Parameter: \ref QMESH_CONFIG_PUBLICATION_SET_EVENT_T
                                                 */
    QMESH_CONFIG_NETKEY_ADD_EVENT,              /*!< Configuration model network key add event on the node.
                                                 *          Parameter: \ref QMESH_CONFIG_NETKEY_EVENT_T
                                                 */
    QMESH_CONFIG_NETKEY_DELETE_EVENT,           /*!< Configuration model network key delete event on the node.
                                                 *          Parameter: \ref QMESH_CONFIG_NETKEY_EVENT_T
                                                 */

    QMESH_EVENT_HEALTH_MODEL_GROUP_ID = 0x0300, /*!< Health model events. */
    QMESH_HEALTH_FAULT_GET_EVENT,               /*!< Indicates that the application is to update the current fault state. \n
                                                 *      Parameter: None
                                                 */
    QMESH_HEALTH_FAULT_CLEAR_EVENT,             /*!< Indicates that the application is to clear all fault states. \n
                                                 *      Parameter: None
                                                 */
    QMESH_HEALTH_FAULT_TEST_EVENT,              /*!< Indicates that the application is to start a specifc test. \n
                                                 *      Parameter: \ref QMESH_HEALTH_TEST_ID_INFO_T
                                                 */
    QMESH_HEALTH_ATTENTION_SET_EVENT,           /*!< Indicates that the application is to trigger the attention procedure with
                                                 *      a specified attention value. \n
                                                 *      Parameter: uint8_t stores the attention state/time.
                                                 */
    QMESH_HEALTH_ATTENTION_GET_EVENT,           /*!< Indicates that the application is to update the current attention value. \n
                                                 *      Parameter: None
                                                 */
    QMESH_HEALTH_FAULT_STATUS_EVENT,            /*!< Event is received as part of a health model client operation. \n
                                                 *      Parameter: \ref QMESH_HEALTH_CLIENT_EVT_DATA_T with \ref QMESH_HEALTH_CLIENT_EVT_INFO_T as
                                                 *      \ref QMESH_HEALTH_FAULT_STATUS.
                                                 */
    QMESH_HEALTH_CURRENT_STATUS_EVENT,          /*!< Event is received to indicate the publication procedure. \n
                                                 *      Parameter: \ref QMESH_HEALTH_CLIENT_EVT_DATA_T with \ref QMESH_HEALTH_CLIENT_EVT_INFO_T as
                                                 *      \ref QMESH_HEALTH_FAULT_STATUS.
                                                 */
    QMESH_HEALTH_PERIOD_STATUS_EVENT,           /*!< Event is received as part of a health period get/set operation. \n
                                                 *      Parameter: \ref QMESH_HEALTH_CLIENT_EVT_DATA_T with \ref QMESH_HEALTH_CLIENT_EVT_INFO_T as
                                                 *      \ref QMESH_HEALTH_PERIOD_STATUS.
                                                 */
    QMESH_HEALTH_ATTENTION_STATUS_EVENT,        /*!< Event is received as part of an attention get/set operation.
                                                 *      Parameter: \ref QMESH_HEALTH_CLIENT_EVT_DATA_T with \ref QMESH_HEALTH_CLIENT_EVT_INFO_T as
                                                 *      \ref QMESH_HEALTH_ATTENTION_STATUS.
                                                 */
    QMESH_IV_RECOVERY_COMPLETE_EVENT,           /*!< Event to inform application about IV recovery complete.
                                                 */

    QMESH_EVENT_LPN_FRIEND_GROUP_ID = 0x0400,   /*!< LPN/Friend events  */
    QMESH_LPN_SLEEP_EVENT,                      /*!< Indicates that the application can to go to sleep */
    QMESH_LPN_SUB_LIST_CFM_EVENT,               /*!< Subscription list confirmation event */
    QMESH_FRND_OFFER_EVENT,                     /*!< Friend offer event */
    QMESH_FRND_ESTABLISHED_EVENT,               /*!< Friend Established event */
    QMESH_FRND_TERMINATED_EVENT,                /*!< Friendship Terminated event */
    QMESH_FRND_REMOVE_CFM_EVENT,                /*!< Friendship has been removed */
    QMESH_LPN_WAKE_EVENT,                       /*!< Indicates that the application should wakeup from sleep */
    QMESH_INVALID_EVENT = 0xFFFF                /*!< Invalid event. Not used. */
} QMESH_EVENT_T;

/*! \brief Operations allowed on subscription lists.
 */
typedef enum
{
    QMESH_SUBSCRIBE_ADD,    /*!< Add to a subscription list. */
    QMESH_SUBSCRIBE_DEL,    /*!< Delete address from a subscription list. */
    QMESH_SUBSCRIBE_OW,     /*!< Overwrite in a subscription list. */
    QMESH_SUBSCRIBE_DEL_ALL /*!< Delete all addresses from a subscription list. Used only when
                             *          the device is in a provisioner role.
                             */
} QMESH_SUBSCRIBE_OPCODE_T;

/*! \brief Qmesh health test ID status type. */
typedef enum
{
    QMESH_HEALTH_VALID_TEST_ID,    /**< Test ID is valid. */
    QMESH_HEALTH_INVALID_TEST_ID   /**< Test ID is invalid. */
} QMESH_TEST_ID_STATUS_T;

/*! \brief Qmesh GATT proxy state. */
typedef enum
{
    QMESH_GATT_PROXY_STATE_DISABLED        = 0,     /*!< Mesh proxy service is running, but the proxy feature is disabled. */
    QMESH_GATT_PROXY_STATE_RUNNING         = 1,     /*!< Mesh proxy service is running, and the proxy feature is enabled. */
    QMESH_GATT_PROXY_STATE_NOT_SUPPORTED   = 2,     /*!< Mesh proxy servive is not supported because the proxy feature is not supported. */
} QMESH_GATT_PROXY_STATE_T;

/*! \brief Advertising message types. */
typedef enum
{
    QMESH_ADV_MESG_INVALID          = 0,            /*!< Invalid advertising message, or it is not an advertisement (e.g., a GATT bearer message). */
    QMESH_ADV_MESG_CSRMESH_MASP     = 1,            /*!< MASP CSRmesh message. */
    QMESH_ADV_MESG_CSRMESH_MCP      = 2,            /*!< MCP CSRmesh message. */
    QMESH_ADV_MESG_CSRMESH_UNKNOWN  = 3,            /*!< Unknown CSRmesh message. */
    QMESH_ADV_MESG_USER             = 4,            /*!< Bluetooth application message. */
    QMESH_ADV_MESG_SIG_NETWORK      = 5,            /*!< Mesh profile network layer message. */
    QMESH_ADV_MESG_SIG_RELAY        = 6,            /*!< Mesh profile relay message. */
    QMESH_ADV_MESG_SIG_BEACON       = 7,            /*!< Mesh profile unprovisioned device beacon/secure network beacon message. */
    QMESH_ADV_MESG_SIG_PBADV        = 8             /*!< Mesh profile provsioning bearer advertisement message. */
} QMESH_ADV_MESG_T;

/*! \brief Mesh security key types. */
typedef enum
{
    QMESH_KEY_TYPE_APP_KEY              = 0,  /*!< Mesh key type Application key */
    QMESH_KEY_TYPE_DEVICE_KEY_SELF      = 1,  /*!< Mesh key type Device key. Typically used in server role. */
    QMESH_KEY_TYPE_DEVICE_KEY_PEER      = 2   /*!< Mesh key type Device key. Typically used in client role. */
} QMESH_ACCESS_PAYLOAD_KEY_TYPE_T;

typedef enum
{
    QMESH_FRIEND_STATE_DISABLED         = 0,
    QMESH_FRIEND_STATE_ENABLED          = 1,
    QMESH_FRIEND_STATE_NOT_SUPPORTED    = 2
} QMESH_FRIEND_STATE_T;

 /*!< \brief Transport Layer MIC Size. */
typedef enum
{
    QMESH_TRANS_SZMIC_RFU           = 0,                          /*!< SZMIC is RFU for Control PDU. */
    QMESH_TRANS_SZMIC_32BIT         = 4,                          /*!< 32-bit Transport MIC Size. */
    QMESH_TRANS_SZMIC_64BIT         = 8                           /*!< 64-bit Transport MIC Size. */
} QMESH_TRANS_SZMIC_T;

/*!< \brief Proxy failure reason. */
typedef enum
{
    QMESH_PROXY_REASSEMBLY_TIMEOUT  = 0,        /*!< Proxy PDU reassembly timedout. */
    QMESH_PROXY_REASSEMBLY_FAILURE  = 1         /*!< Proxy PDU reassembly encountered unexpected message. */
} QMESH_PROXY_FAILURE_REASON_T;


/*============================================================================*
    Data Type Definitions
 *============================================================================*/

/*! \brief Uniform resource indicator string. */
typedef const char* QMESH_URI_T;

/*! \brief 128-bit UUID represented as an array of half words in big endian format. */
typedef uint16_t    QMESH_UUID_T[QMESH_UUID_SIZE_HWORDS];

/*! \brief 128-bit device UUID represented as an array of half words in big endian format. */
typedef QMESH_UUID_T QMESH_DEVICE_UUID_T;

/*! \brief 128-bit label UUID represented as an array of half words in little endian format. */
typedef QMESH_UUID_T QMESH_LABEL_UUID_T;

/*! \brief Bearer handle. */
typedef void* QMESH_BEARER_HANDLE_T;

/*! \brief Bearer context. */
typedef void* QMESH_BEARER_CONTEXT_T;

/*! \brief Mesh security represented as an array of half words in little endian format. */
typedef uint16_t QMESH_SEC_KEY_T[QMESH_SECURITY_KEY_HWORDS];

/*! \brief Network key. */
typedef QMESH_SEC_KEY_T QMESH_NETWORK_KEY_T;

/*! \brief Device key. */
typedef QMESH_SEC_KEY_T QMESH_DEVICE_KEY_T;

/*! \brief Network key. */
typedef QMESH_SEC_KEY_T QMESH_APPLICATION_KEY_T;

/*! \brief Key index data. */
typedef struct
{
    uint16_t    prov_net_idx :  4;  /*!< Provisioned network index. */
    uint16_t    key_idx      : 12;  /*!< 12-bit global network or application key index for the provisioned network. */
} QMESH_KEY_IDX_INFO_T;

/*! \brief Security information used to encrpypt/decrypt the Application payload
 *         while sending/receiving respectively.
 */
typedef struct
{
    QMESH_ACCESS_PAYLOAD_KEY_TYPE_T key_type;  /*!< Key type to use for Transport Layer Encrypted
                                                *   Authentication. See \ref QMESH_ACCESS_PAYLOAD_KEY_TYPE_T.
                                                */
    QMESH_KEY_IDX_INFO_T key_info;             /*!< If key_type is QMESH_KEY_TYPE_APP_KEY then it indicates
                                                *   Provisioned network and Global Application Key Index.
                                                *   Else Provisioned network and Global Network Key Index.
                                                */
    bool                 use_frnd_keys;        /*!< This is used by LPN to indicate the stack to send message
                                                *   with Friendship Security Material on the subnet. This is
                                                *   ignored if the device is not a Low Power Node (LPN).
                                                */
} QMESH_ACCESS_PAYLOAD_KEY_INFO_T;


/*! \brief Application key index data. */
typedef QMESH_KEY_IDX_INFO_T QMESH_APP_KEY_IDX_INFO_T;

/*! \brief Subnetwork key index data. */
typedef QMESH_KEY_IDX_INFO_T QMESH_SUBNET_KEY_IDX_INFO_T;

/*! \brief Mesh Network ID. */
typedef uint16_t QMESH_NETWORK_ID_T[QMESH_NETWORK_ID_SIZE_HWORDS];

/*! \brief Network header information. */
typedef struct
{
    uint32_t    seq_num;    /*!< 24-bit sequence number with which the message was received. */
    uint16_t    src_addr;   /*!< 16-bit source address of the message. */
    uint16_t    dst_addr;   /*!< 16-bit destination address of the message. */
    uint8_t     ttl;        /*!< Received TTL of the message. */
} QMESH_NW_HEADER_INFO_T;

/*! \brief Device provisioning capabilities.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Table-5.16.
 */
typedef struct
{
    uint8_t     pubkey_oob_mask;        /*!< Mask of public key information available. See \ref QMESH_PUBLIC_KEY_MASK_T. */
    uint8_t     static_oob_mask;        /*!< Mask of static OOB information available. See \ref QMESH_STATIC_OOB_MASK_T. */
    uint8_t     output_oob_size;        /*!< Output OOB data size in octets. Refer to the Mesh Profile Specification, Table-5.21. */
    uint8_t     input_oob_size;         /*!< Input OOB data size in octets. Refer to the Mesh Profile Specification, Table-5.23. */
    uint16_t    output_action_mask;     /*!< Mask of output OOB actions supported. See \ref QMESH_OUTPUT_OOB_ACTION_MASK_T. */
    uint16_t    input_action_mask;      /*!< Mask of input OOB actions supported. See \ref QMESH_INPUT_OOB_ACTION_MASK_T. */
    uint16_t    prov_algos_mask;        /*!< Mask of provisioning algorithms supported. See \ref QMESH_PROV_ALGO_MASK_T. */
} QMESH_DEV_CAPS_DATA_T;

/*! \brief Device provisioning capabilities.
 */
typedef struct
{
    uint8_t                 num_elements;       /*!< Number of elements. */
    QMESH_DEV_CAPS_DATA_T   caps;               /*!< Device capability. */
} QMESH_REMOTE_NODE_CAPS_DATA_T;

/*! \brief OOB action. */
typedef union
{
    QMESH_INPUT_OOB_ACTION_T    input_oob_action;      /*!< Input OOB action.  */
    QMESH_OUTPUT_OOB_ACTION_T   output_oob_action;     /*!< Output OOB action. */
} QMESH_OOB_ACTION_T;

/*! \brief Provision selected capabilities. */
typedef struct
{
    QMESH_PUBLIC_KEY_T      pub_key_method;     /*!< Device public key exchange method. See \ref QMESH_PUBLIC_KEY_T. */
    QMESH_AUTH_METHOD_T     auth_method;        /*!< Authentication method. See \ref QMESH_AUTH_METHOD_T. */
    QMESH_OOB_ACTION_T      action;             /*!< Action selected. See \ref QMESH_OOB_ACTION_T. */
    uint8_t                 auth_data_size;     /*!< Authentication data size. */
} QMESH_PROV_SELECTED_CAPS_T;

/*! \brief Provision status event */
typedef struct
{
    uint8_t     att_duration; /*!< Time in seconds for which the node is expected to attract
                               *          human attention.
                               */
} QMESH_PROV_INVITE_DATA_T;


/*! \brief Data recieved upon successful provisioning of the device. */
typedef struct
{
    QMESH_SUBNET_KEY_IDX_INFO_T key_info;   /*!< Provisioned network and subnetwork identifier. */
    uint16_t                    elm_addr;   /*!< Element address of the primary element. */
    uint16_t                   *device_key; /*!< Device key generated during provisioning
                                             * in packed little endian format.
                                             * Valid only on the provisioner. Will be NULL
                                             * in the case of the device.
                                             */
} QMESH_PROV_COMPLETE_DATA_T;

/*! \brief Device key list is managed by the application in the provisioner role.
 *  The stack uses QMESH_GET_DEVICE_KEY_EVENT to get the device
 *  key for the specified element address.
 */
typedef struct
{
    uint8_t                     prov_net_id;    /*!< Provisioned network identifier. */
    uint16_t                    elm_addr;       /*!< Element address of the primary element. */
    uint16_t                   *device_key;     /*!< Device key generated during provisioning
                                                 * in packed little endian format.
                                                 * Valid only on the provisioner. Will be NULL
                                                 * in the case of the device.
                                                 */
} QMESH_DEVICE_KEY_INFO_T;

/*! \brief Device data. */
typedef struct
{
    QMESH_DEVICE_UUID_T     uuid;  /*!< UUID identifying the device in big endian format. */
    uint16_t                uri_length;  /*!< URI length in octets. */
    QMESH_URI_T             uri;  /*!< Uniform resource indicator for the device. */
    uint16_t                oob_info_mask;  /*!< Mask of OOB information availability bits. See \ref QMESH_OOB_INFO_MASK_T. */
    QMESH_DEV_CAPS_DATA_T   dev_caps;  /*!< Device capabilities for provisioning. */
    uint16_t         *const oob_pub_key;  /*!< OOB public key in big endian format. The stack generates and copies here for application reference */
} QMESH_DEVICE_IDENTIFIER_DATA_T;

/*! \brief Publish retransmit state.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Sections 4.2.2.6 and 4.2.2.7.
 */
typedef struct
{
    uint8_t             count : 3;    /*!< Retransmit count. */
    uint8_t     interval_step : 5;    /*!< Retransmit interval in steps of 50 milliseconds. */
} QMESH_PUBLISH_RETRANSMIT_STATE_T;

/*! \brief Publish period.
 *
 *    \note1hang Refer to the Mesh Profile Specification, Section 4.2.2.2.
 */
typedef struct
{
    uint8_t     resolution : 2;        /*!< Resolution of the period. */
    uint8_t          steps : 6;        /*!< Number of period steps. */
} QMESH_PUBLISH_PERIOD_T;

typedef struct _QMESH_MODEL_DATA_T __QMESH_MODEL_DATA_T;
typedef struct _QMESH_ELEMENT_CONFIG_T __QMESH_ELEMENT_CONFIG_T;

/*! \brief model callback. */
typedef QMESH_RESULT_T (*QMESH_MODEL_CB_T)(const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                           const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
                                           const __QMESH_ELEMENT_CONFIG_T *elem_config,
                                           __QMESH_MODEL_DATA_T *model_data,
                                           const uint8_t *msg,
                                           uint16_t msg_len);

/*! \brief Model publication handler */
typedef void (*QMESH_PUBLISH_CONF_HANDLER_T)(__QMESH_MODEL_DATA_T *model_data);

/*! \brief Model publish state.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Section 4.2.2.
 */
typedef struct
{
    uint16_t                          app_key_idx;          /*!< Application key index to be used for publishing. */
    uint16_t                          publish_addr;         /*!< Publish address. */
    const QMESH_LABEL_UUID_T          *publish_uuid;        /*!< Label UUID when publish address is a virtual address. */
    bool                              use_frnd_key;         /*!< TRUE if friend security material iss to be used. Applicable only for a low power node. */
    uint8_t                           publish_ttl;          /*!< Publish TTL. When the value is 0xFF, it uses the default TTL. */
    QMESH_PUBLISH_PERIOD_T            publish_period;       /*!< Period of publishing mssages. See \ref QMESH_PUBLISH_PERIOD_T. */
    QMESH_PUBLISH_RETRANSMIT_STATE_T  retransmit_param;     /*!< Publish the retransmit state. See \ref QMESH_PUBLISH_RETRANSMIT_STATE_T. */
    QMESH_PUBLISH_CONF_HANDLER_T      conf_handler;         /*!< Publish the configuration handler. */
} QMESH_MODEL_PUBLISH_STATE_T;

/*! \brief Model data. */
typedef struct _QMESH_MODEL_DATA_T
{
    QMESH_MODEL_PUBLISH_STATE_T  *const publish_state;      /*!< Pointer to the model publication state. NULL if publication is not supported by the model. */
    QMESH_LIST_HANDLE_T                 subs_list;          /*!< Model subscription list. */
    QMESH_LIST_HANDLE_T                 app_key_list;       /*!< Bound application key list. */
    void                         *const model_priv_data;    /*!< Pointer model's private information. This data will not be interpreted or accessed by the stack. */
    QMESH_MODEL_CB_T                    model_handler;      /*!< Model handler. See \ref QMESH_MODEL_CB_T. */
}QMESH_MODEL_DATA_T;

/*! \brief Data specific to each element. */
typedef struct
{
    uint32_t            seq_num;            /*!< 24-bit element sequence number. */
    uint16_t            unicast_addr;       /*!< Unicast address of the element. */
    QMESH_MODEL_DATA_T  *const model_data;  /*!< Constant pointer to the model data list array. Model order must follow the model identifiers order in \ref QMESH_ELEMENT_CONFIG_T.
                                             *          The model data array must follow the order of the Bluetooth SIG models followed by the vendor models.
                                             */
} QMESH_ELEMENT_DATA_T;

/*! \brief Element configuration information. */
typedef struct _QMESH_ELEMENT_CONFIG_T
{
    uint16_t                       loc;                 /*!< Bluetooth SIG defined location descriptor. Refer to <a href="https://www.bluetooth.com/specifications/assigned-numbers/gatt-namespace-descriptors">location descriptor</a>. */
    uint16_t                       num_btsig_models;    /*!< Number of Bluetooth SIG defined models on the device. */
    uint16_t                       num_vendor_models;   /*!< Number of vendor defined models on the device. */
    const uint16_t          *const btsig_model_ids;     /*!< Pointer to the array of Bluetooth SIG defined model identifiers. */
    const uint32_t          *const vendor_model_ids;    /*!< Pointer to the array of vendor defined model identifiers. */
    QMESH_ELEMENT_DATA_T    *const element_data;        /*!< Constant pointer to the element data. */
} QMESH_ELEMENT_CONFIG_T;

/*! \brief Model to AppKey bind information. */
typedef struct
{
    uint16_t            elm_addr;           /*!< Element address. */
    uint16_t            app_key_idx;        /*!< Application key index. */
    uint32_t            model_id;           /*!< ID of the model to bind. */
    QMESH_MODEL_TYPE_T  model_type;         /*!< type of the model. */
} QMESH_MODEL_APPKEY_BIND_INFO_T;

/*! \brief Device composition data.
 *
 *  \note1hang Refer to the Mesh Profile Specification, Section 4.2.1.
 */
typedef struct
{
    uint16_t    cid;
   /*!< Bluetooth SIG defined company identifier. Refer to <a href="https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers">Company Identifiers</a>. */
    uint16_t    pid;            /*!< Vendor assigned product identifier. */
    uint16_t    vid;            /*!< Vendor assigned product version identifier. */
    uint16_t    crpl;           /*!< Minimum number of replay protection list items on the device. */
    uint16_t    feature_mask;   /*!< Bit field of the device features. See \ref QMESH_DEVICE_FEATURE_MASK_T. */
    uint8_t     num_elements;   /*!< Number of elements on the device. */

    const QMESH_ELEMENT_CONFIG_T *const elements;   /*!< Pointer to the array of element configuration (See \ref QMESH_ELEMENT_CONFIG_T).
                                                                The primary element must be the first one. */
} QMESH_DEVICE_COMPOSITION_DATA_T;

/*! \brief QMesh app event data. Provides event, status, and app data to the application. */
typedef struct
{
    QMESH_EVENT_T   event;  /*!< QMesh event. */
    QMESH_RESULT_T  status;  /*!< QMesh operation status. */
    void *param;  /*!< Pointer to a buffer containing the parameters associated with the event. */
} QMESH_APP_EVENT_DATA_T;

/*! \brief Proxy Disconnect Event. */
typedef struct
{
    QMESH_PROXY_FAILURE_REASON_T     reason;    /*!< Proxy failure reason. See \ref QMESH_PROXY_FAILURE_REASON_T. */
    QMESH_BEARER_HANDLE_T            handle;    /*!< Proxy Bearer Handle. See \ref QMESH_BEARER_HANDLE_T. */
    QMESH_BEARER_CONTEXT_T           cntxt;     /*!< Proxy Bearer Context. See \ref QMESH_BEARER_CONTEXT_T. */
} QMESH_PROXY_DISCONNECT_EVENT_DATA_T;

/*! \brief Proxy filter status event data. */
typedef struct
{
    QMESH_PROXY_FILTER_TYPE_T    type;           /*!< Proxy filter type.               */
    uint16_t                     src_addr;       /*!< Source device address.           */
    uint16_t                     num_addresses;  /*!< Number of address in the filter. */
    QMESH_SUBNET_KEY_IDX_INFO_T  subnet_idx;     /*!< Subnet Key Index. */
    QMESH_BEARER_HANDLE_T        bearer_handle;  /*!< Bearer Handle. */
} QMESH_PROXY_FILTER_STATUS_T;

/*! \brief Bearer Header. */
typedef struct
{
    QMESH_BEARER_HANDLE_T       handle;      /*!< Bearer Handle. */
    uint32_t                    timestamp;   /*!< Time stamp in milliseconds. */
    int8_t                      rssi;        /*!< Received Signal Strength Indicator. */
    // TODO: Bluetooth Device Address and Address type.
    // TODO: Do we need bearer context data here?
} QMESH_BEARER_HEADER_T;

/*================== Friend Data Structures ===================*/

/*================= Config Model Structures ===================*/

/*!\brief Application callback function for events. */
typedef void (*QMESH_APP_CB_T) (QMESH_APP_EVENT_DATA_T eventDataCallback);

/*================== Publication/Subscription configuration data structures =================*/

/*!\brief  Publication configuration information. */
typedef struct
{
    uint16_t                ele_addr;            /*!< Address of the element. */
    QMESH_MODEL_TYPE_T      model_type;          /*!< SIG model ID or vendor model ID. */
    uint32_t                model_id;            /*!< Model ID. */
    uint16_t                pub_addr;            /*!< Publish address. The app uses this field to pass a
                                                      unicast or group address. */
    uint16_t                app_key_idx;         /*!< 12-bit global index of the application key. */
    bool                    credential_flag;     /*!< Value of the friendship credential flag. */
    uint8_t                 pub_period;          /*!< Period for periodic status publishing. */
    uint8_t                 pub_ttl;             /*!< Default TTL value for the outgoing messages. */
    uint8_t                 pub_rtx_cnt;         /*!< Number of retransmissions for each published message. */
    uint8_t                 pub_rtx_int_stp;     /*!< Number of 50-millisecond steps between retransmissions.*/
    const uint16_t         *label_uuid;          /*!< 128-bit label UUID. This field is needed in the case of a virtual address. */
} QMESH_PUBLICATION_CONFIG_T;

/*!\brief  Subscription configuration information. */
typedef struct
{
    uint16_t                ele_addr;       /*!< Address of the element. */
    QMESH_MODEL_TYPE_T      model_type;     /*!< SIG model ID or vendor model ID. */
    uint32_t                model_id;       /*!< Model ID. */
    uint16_t                sub_addr;       /*!< Subscription address. The app uses this field to pass a unicast or group address. */
    const uint16_t         *label_uuid;     /*!< 128-bit label UUID. This field is needed in the case of virtual a address. */
} QMESH_SUBSCRIPTION_CONFIG_T;


/*! \brief Access Layer Payload Information. */
typedef struct
{
    uint16_t   src_addr;
    /*!< Source address. */
    uint16_t   dst_addr;
    /*!< Destination address. */
    uint16_t   payload_len;
    /*!< Payload length. */
    uint8_t    *payload;
    /*!< Pointer to the access layer payload. */
    uint8_t    ttl;
    /*!< TTL. */
    QMESH_TRANS_SZMIC_T    trans_mic_size;
    /*!< MIC size to be used; 4 or 8. */
    bool       trans_ack_required; /*!< Applicable for (app_payload_len + trans_mic_size) <= 12, which can be sent either as an unsegmented or a single segmented transport PDU. */
} QMESH_ACCESS_PAYLOAD_INFO_T;

/*! \brief Health model data. */
typedef struct
{
    uint8_t       test_id;            /*!< Test ID of a currently running test. */
    uint8_t       len_fault_arr;      /*!< Length of the fault array. */
    uint8_t       *fault_arr;         /*!< Fault array is stored in the application. */
    uint8_t       attention_state;    /*!< Attention state is stored in the application. */
} QMESH_HEALTH_MODEL_DATA_T;

/*! \brief Health model test ID information. */
typedef struct
{
    uint8_t                   test_id;    /*!< Current test ID. */
    QMESH_TEST_ID_STATUS_T    status;     /*!< Application supported test ID status. */
} QMESH_HEALTH_TEST_ID_INFO_T;

/*! \brief Config GATT proxy information. */
typedef struct
{
    QMESH_GATT_PROXY_STATE_T gp_state;  /*!< GATT proxy states. */
} QMESH_CONFIG_GATT_PROXY_INFO_T;

/*! \brief Node identity information. */
typedef struct
{
    uint8_t                        status;         /*!< Status code for the node identity procedure. */
    QMESH_SUBNET_KEY_IDX_INFO_T    net_key_index;  /*!< Subnet key index. */
    QMESH_NODE_IDENTITY_T          identity;       /*!< Current node identity state for the global net key index. */
} QMESH_CONFIG_NODE_IDENTITY_INFO_T;

/*! \brief Network transmit state. */
typedef struct
{
    uint8_t    transmit_count    : 3;   /*!< Network transmit count. */
    uint8_t    transmit_interval : 5;   /*!< Network transmit interval. */
} QMESH_TRANSMIT_STATE_T;

/*! \brief Network Transmit State. */
typedef QMESH_TRANSMIT_STATE_T QMESH_NETWORK_TRANSMIT_STATE_T;

/*! \brief Heartbeat subscription state. */
typedef struct
{
    uint16_t      src_addr;               /*!< Source address. */
    uint16_t      dst_addr;               /*!< Destination address. */
    uint8_t       period_log;             /*!< Period log. */
    uint8_t       count_log;              /*!< Count log. */
    uint8_t       min_hops;               /*!< Minimum hops. */
    uint8_t       max_hops;               /*!< Maximum hops. */
} QMESH_HEARTBEAT_SUBSCRIPTION_STATE_T;

/*! \brief Heartbeat Publication State. */
typedef struct
{
    uint16_t      dst_addr;      /*!< Destination address. */
    uint8_t       period_log;    /*!< Period log. */
    uint8_t       count_log;     /*!< Count log. */
    uint8_t       ttl;           /*!< TTL to be used for a heart beat message. */
    uint16_t      feature_mask;  /*!< Publish reature mask. When any feature state changes and the bit field is set, it triggers a publish message. See \ref QMESH_DEVICE_FEATURE_MASK_T. */
    uint16_t      net_key_idx;   /*!< Globally identified network key index. */
} QMESH_HEARTBEAT_PUBLICATION_STATE_T;

/*@} */
#endif /* _QMESH_TYPES_H_ */

