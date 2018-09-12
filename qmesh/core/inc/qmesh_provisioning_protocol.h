/******************************************************************************
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_provisioning_protocol.h
 *  \brief Qmesh provisioning functions
 *
 *   This file contains provisioning protocol function prototypes
 *
 */
 /*****************************************************************************/

#ifndef __QMESH_PROVISIONING_PROTOCOL_H__
#define __QMESH_PROVISIONING_PROTOCOL_H__

#include "qmesh_config.h"
#include "qmesh_types.h"
#include "qmesh_security_ifce.h"
#include "qmesh_soft_timers.h"

/* Length of provisioning protocol opcode */
#define PROV_OPCODE_LEN  1

/* Identify states */
#define MESH_IDENTIFY_STATE_OFF 0x00

/* |-- Code (1 Octet) --|-- Data (1 to 22 or 64 Octets) --| */
/* Provisioning protocol oprational buffer length */
#define MESH_PP_MAX_MSG_LENGTH              65

/* Prov Invite (1 Octet)
     + Prov capability(11 Octets)
     + Prov Start (5 Octets)
     + Provisioner public key (64 Octets)
     + Device public key (64 Octets) */
#define MESH_PP_CONFIRMATION_INPUT_LEN      145

/* Provision protocol message types */
#define PROVISIONING_INVITE         0x00
#define PROVISIONING_CAPABILITIES   0x01
#define PROVISIONING_START          0x02
#define PROVISIONING_PUBLIC_KEY     0x03
#define PROVISIONING_INPUT_COMPLETE 0x04
#define PROVISIONING_CONFIRMATION   0x05
#define PROVISIONING_RANDOM         0x06
#define PROVISIONING_DATA           0x07
#define PROVISIONING_COMPLETE       0x08
#define PROVISIONING_FAILED         0x09

/* Packet lengths */
#define PROVISIONING_INVITE_OFFSET                0
#define PROVISIONING_INVITE_LEN_OCTETS            1

#define PROVISIONING_CAPABILITIES_OFFSET          (PROVISIONING_INVITE_LEN_OCTETS)
#define PROVISIONING_CAPABILITIES_LEN_OCTETS      11

#define PROVISIONING_START_OFFSET                 (PROVISIONING_CAPABILITIES_OFFSET + \
                                                    PROVISIONING_CAPABILITIES_LEN_OCTETS)
#define PROVISIONING_START_LEN_OCTETS             5

#define PROVISIONING_PUBLIC_KEY_OFFSET            (PROVISIONING_START_OFFSET + \
                                                    PROVISIONING_START_LEN_OCTETS)
#define PROVISIONING_PUBLIC_KEY_LEN_OCTETS        64

#define PROVISIONING_INPUT_COMPLETE_OFFSET        (PROVISIONING_PUBLIC_KEY_OFFSET + \
                                                    PROVISIONING_PUBLIC_KEY_LEN_OCTETS)
#define PROVISIONING_INPUT_COMPLETE_LEN_OCTETS    0

#define PROVISIONING_CONFIRMATION_OFFSET          (PROVISIONING_INPUT_COMPLETE_OFFSET + \
                                                    PROVISIONING_INPUT_COMPLETE_LEN_OCTETS)
#define PROVISIONING_CONFIRMATION_LEN_OCTETS      16
#define PROVISIONING_CONFIRMATION_LEN_WORDS       8

/* Provisioning Random */
#define PROVISIONING_RANDOM_OFFSET                (PROVISIONING_CONFIRMATION_OFFSET + \
                                                    PROVISIONING_CONFIRMATION_LEN_OCTETS)
#define PROVISIONING_RANDOM_LEN_OCTETS            16
#define PROVISIONING_RANDOM_LEN_WORDS              8

/* Provisioning complete */
#define PROVISIONING_COMPLETE_OFFSET              (PROVISIONING_RANDOM_OFFSET + \
                                                    PROVISIONING_RANDOM_LEN_OCTETS)
#define PROVISIONING_COMPLETE_LEN_OCTETS          0

/* Provisioning Failed */
#define PROVISIONING_FAILED_LEN_OCTETS            1
#define PROVISIONING_DATA_MAC_OCTETS              8
#define PROVISIONING_DATA_LEN_OCTETS        (25 + PROVISIONING_DATA_MAC_OCTETS)

#define PROVISIONING_CONF_RANDOM_SIZE_OCTETS        16

/* Provisioning Salt offset */
#define PROVISIONING_SALT_CONF_SALT_OFFSET          0
#define PROVISIONING_SALT_CONF_SALT_SIZE            16
#define PROVISIONING_SALT_CONF_SALT_SIZE_HWORDS     8

#define PROVISIONING_SALT_PROV_RANDOM_OFFSET        (PROVISIONING_SALT_CONF_SALT_OFFSET + \
                                                    PROVISIONING_SALT_CONF_SALT_SIZE)
#define PROVISIONING_SALT_PROV_RANDOM_SIZE          PROVISIONING_CONF_RANDOM_SIZE_OCTETS

#define PROVISIONING_SALT_DEV_RANDOM_OFFSET         (PROVISIONING_SALT_PROV_RANDOM_OFFSET + \
                                                    PROVISIONING_SALT_PROV_RANDOM_SIZE)
#define PROVISIONING_SALT_DEV_RANDOM_SIZE           PROVISIONING_CONF_RANDOM_SIZE_OCTETS

#define PROVISIONING_SALT_INPUT_SIZE                (PROVISIONING_SALT_DEV_RANDOM_OFFSET + \
                                                    PROVISIONING_SALT_DEV_RANDOM_SIZE)

#define PROVISIONING_CONF_AUTH_SIZE_OCTETS          16
#define PROVISIONING_CONF_INPUT_SIZE_OCTETS         (PROVISIONING_CONF_RANDOM_SIZE_OCTETS + \
                                                     PROVISIONING_CONF_AUTH_SIZE_OCTETS) /* Random (16) + Auth (16) */

#define PP_REQ_TIMEOUT                      (60 * QMESH_SECOND)

/* Consts for use by the rest of the system */
#define ECC_P256_KEY_BITS                   256
#define ECC_P256_KEY_WORDS                  (ECC_P256_KEY_BITS/16)
#define ECC_P256_KEY_OCTETS                 (ECC_P256_KEY_BITS/8)

#define ECC_P256_KEY_SIZE                   (ECC_P256_KEY_OCTETS+1)
#define ECC_P256_KEY_SIZE_WORDS             (ECC_P256_KEY_WORDS+1)

#define ECC_P256_PUBLIC_KEY_SIZE            (ECC_P256_KEY_SIZE * 2)
#define ECC_P256_PUBLIC_KEY_SIZE_WORDS      (ECC_P256_KEY_SIZE_WORDS * 2)

#define DHMKEY_SIZE_IN_OCTETS               (ECC_P256_KEY_SIZE * 2)
#define DHMKEY_SIZE_IN_WORDS                (ECC_P256_KEY_SIZE_WORDS * 2)

#define PP_EVENT_ACK            0
#define PP_EVENT_RESET          1

#define SHARED_SECRET_SIZE_HWORDS            (16U)

#define PP_CAPABILITY_PUB_KEY_TYPE_MASK     (0xFE)

typedef enum
{
    QMESH_PROVISIONING_ERROR_PROHIBITED,
    QMESH_PROVISIONING_ERROR_INVALID_PDU,
    QMESH_PROVISIONING_ERROR_INVALID_FORMAT,
    QMESH_PROVISIONING_ERROR_UNEXPECTED_PDU,
    QMESH_PROVISIONING_ERROR_CONFIRMATION_FAILED,
    QMESH_PROVISIONING_ERROR_OUT_OF_RESOURCE,
    QMESH_PROVISIONING_ERROR_DECRYPTION_FAILED,
    QMESH_PROVISIONING_ERROR_UNEXPECTED_ERROR,
    QMESH_PROVISIONING_ERROR_CANNOT_ASSIGN_ADDR,
} QMESH_PROVISIONING_ERROR_T;

/* Provisioning Error Codes, refer to section 5.4.1.10
 *  Table 5.38: Provisioning error codes */

typedef enum
{
    QMESH_PP_STATE_IDLE,
    QMESH_PP_STATE_READY,
    QMESH_PP_STATE_INVITE,
    QMESH_PP_STATE_CAPABILITY,
    QMESH_PP_STATE_START,
    QMESH_PP_STATE_PUBLIC_KEY,
    QMESH_PP_STATE_OOB_ACTION,
    QMESH_PP_STATE_CONFIRMATION,
    QMESH_PP_STATE_INPUT_COMP,
    QMESH_PP_STATE_RANDOM,
    QMESH_PP_STATE_DATA,
    QMESH_PP_STATE_PROV_DATA,
    QMESH_PP_STATE_FAILED,
    QMESH_PP_STATE_INVALID
} QMESH_PP_STATE_T;

/* Placeholder for all the gobal data used in provisioning,
    to facilitate the dynamic memory allocation and de-allocation */
typedef struct
{
    /* Device random number used for provisioning */
    uint8_t dev_random[PROVISIONING_RANDOM_LEN_OCTETS];
    uint8_t prov_random[PROVISIONING_RANDOM_LEN_OCTETS];
    uint16_t ConfirmationKey[PROVISIONING_CONFIRMATION_LEN_WORDS];
    uint16_t prov_conf_received[PROVISIONING_CONFIRMATION_LEN_WORDS];
    uint16_t device_confirmation[PROVISIONING_CONFIRMATION_LEN_WORDS];
    uint8_t pp_global_tx_buf[MESH_PP_MAX_MSG_LENGTH];
    uint8_t pp_global_rx_buf[MESH_PP_MAX_MSG_LENGTH];
    /* Prov capability(14 Octets)
        + Prov Start (7 Octets)
        + Provisioner public key (64 Octets)
        + Device public key (64 Octets) */
    uint8_t confInputNProvSaltInput[MESH_PP_CONFIRMATION_INPUT_LEN];
} QMESH_PP_BUFFER_T;

/* Session key and session nonce for encryption of
 * provisioning data
 */
typedef struct
{
    QMESH_SEC_KEY_T SessionKey;
    QMESH_SEC_KEY_T SessionNonce;
    QMESH_SEC_KEY_T ProvisioningSalt;
    uint16_t        shared_secret[SHARED_SECRET_SIZE_HWORDS];
} QMESH_PP_DATA_SESSION_ENC_T;

typedef struct
{
    uint16                  self_priv_key[EC256_KEY_BLOCK_SIZE];    /*!< Device's Private Key for Asymmetric Key Exchange */
    EC_256_AFFINE_POINT_T   self_pub_key;                           /*!< Device's own public key. Both for provisioner and node */
    EC_256_AFFINE_POINT_T   rem_pub_key;                            /*!< Remote device's public key */
    EC_256_AFFINE_POINT_T   dh_shared_secret;                       /*!< Diffe-helman shared secret */
    QMESH_PP_BUFFER_T       buffer;                                 /*!< Other Global data used during key exchange */
} QMESH_PP_KEY_EXCHANGE_DATA_T;

typedef struct
{
    // TODO: Get rid of this first two bearer parameters.
    QMESH_BEARER_HANDLE_T                   bearer_handle;          /*!< Bearer Handle for Provisioning. */
    bool                                    proxy;                  /*!< Bearer is a Proxy Bearer. */
    const QMESH_DEVICE_IDENTIFIER_DATA_T   *p_device;               /*!< Device Information */
    uint8_t                                 dev_instance;           /*!< Device Instance for which provisioning protocol is running. */
    QMESH_PP_STATE_T                        state;                  /*!< Current state of PP procedure */
    QMESH_ROLE_T                            prov_role;              /*!< Holds the current role of provisioning protocol */
    QMESH_PP_KEY_EXCHANGE_DATA_T            *p_key;                 /*!< Key exchange data */
    QMESH_PROV_SELECTED_CAPS_T              auth_info;              /*!< OOB authentication information */
    uint8_t                                 auth_value[PROVISIONING_CONF_AUTH_SIZE_OCTETS];
    QMESH_TIMER_HANDLE_T                    msg_retry_tid;          /*!< Message retry Timer */
    bool                                    oob_action_pending;     /*!< Oob action pending flag */
    bool                                    cfm_rcvd;               /*!< Flag to indicate that confirmation is received and buffered */
    uint8_t                                 *conf_buf;              /*!< Buffer to hold confirmation message if OOB action is pending */
    QmeshResult                             result;                 /*!< Holds the result of provision failure */
#if (QMESH_SUPPORT_PROVISIONER_ROLE == 1)
    QMESH_SUBNET_KEY_IDX_INFO_T             nw_key_info;            /*!< Local Network key index */
    uint16_t                                prim_ele_addr;          /*!< Primary element address to assign */
    uint8_t                                 identity_duration;      /*!< Period for which the device has to perform identity action */
    uint16_t                                *device_key;            /*!< Pointer to generated device key */
    bool                                    discovery_enabled;      /*!< Device discovery enabled? */
    QMESH_UUID_T                            peer_uuid;              /*!< UUID of peer device to be provisioned */
#endif
} QMESH_PP_CONTEXT_T;


extern QMESH_PP_CONTEXT_T g_pp_context;

#define DEV_RANDOM            g_pp_context.p_key->buffer.dev_random
#define PROV_RANDOM           g_pp_context.p_key->buffer.prov_random
#define CONF_KEY              g_pp_context.p_key->buffer.ConfirmationKey
#define PROV_CONF_RCVD        g_pp_context.p_key->buffer.prov_conf_received
#define DEV_CONF              g_pp_context.p_key->buffer.device_confirmation
#define GBL_TX_BUF            g_pp_context.p_key->buffer.pp_global_tx_buf
#define GBL_RX_BUF            g_pp_context.p_key->buffer.pp_global_rx_buf
#define CONF_IP_N_PROV_SLT_IP g_pp_context.p_key->buffer.confInputNProvSaltInput

extern void PpHandleUnProvDeviceBeacon(const uint8_t *msg, uint8_t msg_len);
extern QmeshResult PpInit(const QMESH_DEVICE_IDENTIFIER_DATA_T *p_device);
extern QmeshResult PpInviteDev(void);
extern void PpProcessMsg(uint8_t *msg, uint16_t msg_length);
extern void PpEvtHdlr(uint8_t event);

#endif /* __QMESH_PROVISIONING_PROTOCOL_H__ */

