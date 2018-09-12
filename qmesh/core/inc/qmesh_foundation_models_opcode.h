/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_foundation_models_opcode.h
 *  \brief Foundation Model Message Opcode definitions
 *
 *   This file contains Message opcodes of all the Foundation Models
 */
/*****************************************************************************/

#ifndef __QMESH_FOUNDATION_MODEL_H__
#define __QMESH_FOUNDATION_MODEL_H__

/*! \brief One byte opcode messages mask */
#define QMESH_OPCODE_FORMAT_ONE_BYTE      0x00
/*! \brief Two byte opcode messages mask */
#define QMESH_OPCODE_FORMAT_TWO_BYTE     0x02
/*! \brief Three byte opcode messages mask */
#define QMESH_OPCODE_FORMAT_THREE_BYTE  0x03

/*! \brief config model message opcodes */

#define APP_KEY_ADD                     0x00
#define APP_KEY_DELETE                  0x8000
#define APP_KEY_GET                     0x8001
#define APP_KEY_LIST                    0x8002
#define APP_KEY_STATUS                  0x8003
#define APP_KEY_UPDATE                  0x01

#define ATTENTION_GET                   0x8004
#define ATTENTION_SET                   0x8005
#define ATTENTION_SET_UNREL             0x8006
#define ATTENTION_STATUS                0x8007

#define COMPOSITION_DATA_GET            0x8008
#define COMPOSITION_DATA_STATUS         0x02

#define CONFIG_BEACON_GET               0x8009
#define CONFIG_BEACON_SET               0x800A
#define CONFIG_BEACON_STATUS            0x800B

#define CONFIG_DEFAULT_TTL_GET          0x800C
#define CONFIG_DEFAULT_TTL_SET          0x800D
#define CONFIG_DEFUALT_TTL_STATUS       0x800E

#define CONFIG_FRND_GET                 0x800F
#define CONFIG_FRND_SET                 0x8010
#define CONFIG_FRND_STATUS              0x8011

#define CONFIG_GATT_PROXY_GET           0x8012
#define CONFIG_GATT_PROXY_SET           0x8013
#define CONFIG_GATT_PROXY_STATUS        0x8014

#define CONFIG_KEY_REFRESH_PHASE_GET    0x8015
#define CONFIG_KEY_REFRESH_PHASE_SET    0x8016
#define CONFIG_KEY_REFRESH_PHASE_STATUS 0x8017

#define CONFIG_MODEL_PUBLICATION_GET    0x8018
#define CONFIG_MODEL_PUBLICATION_SET    0x03
#define CONFIG_MODEL_PUBLICATION_STATUS             0x8019
#define CONFIG_MODEL_PUBLICATION_VIRTUAL_ADDR_SET   0x801A

#define CONFIG_MODEL_SUBSCRIPTION_ADD                   0x801B
#define CONFIG_MODEL_SUBSCRIPTION_DELETE                0x801C
#define CONFIG_MODEL_SUBSCRIPTION_DELETE_ALL            0x801D
#define CONFIG_MODEL_SUBSCRIPTION_OW                    0x801E
#define CONFIG_MODEL_SUBSCRIPTION_STATUS                0x801F
#define CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDR_ADD      0x8020
#define CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDR_DELETE   0x8021
#define CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDR_OW       0x8022

#define CONFIG_RELAY_GET                0x8026
#define CONFIG_RELAY_SET                0x8027
#define CONFIG_RELAY_STATUS             0x8028

#define CONFIG_SIG_MODEL_SUBSCRIPTION_GET 0x8029
#define CONFIG_SIG_MODEL_SUBSCRIPTION_LIST 0x802A

#define CONFIG_VENDOR_MODEL_SUBSCRIPTION_GET 0x802B
#define CONFIG_VENDOR_MODEL_SUBSCRIPTION_LIST 0x802C

#define LPN_POLL_TIMEOUT_GET            0x802D
#define LPN_POLL_TIMEOUT_STATUS         0x802E

#define CONFIG_HEART_BEAT_PUBLICATION_GET      0x8038
#define CONFIG_HEART_BEAT_PUBLICATION_SET      0x8039
#define CONFIG_HEART_BEAT_PUBLICATION_STATUS   0x06

#define CONFIG_HEART_BEAT_SUBSCRIBE_GET        0x803A
#define CONFIG_HEART_BEAT_SUBSCRIBE_SET        0x803B
#define CONFIG_HEART_BEAT_SUBSCRIBE_STATUS     0x803C

#define MODEL_APP_BIND                  0x803D
#define MODEL_APP_STATUS                0x803E
#define MODEL_APP_UNBIND                0x803F

#define NET_KEY_ADD                     0x8040
#define NET_KEY_DELETE                  0x8041
#define NET_KEY_GET                     0x8042
#define NET_KEY_LIST                    0x8043
#define NET_KEY_STATUS                  0x8044
#define NET_KEY_UPDATE                  0x8045

#define NODE_IDENTITY_GET               0x8046
#define NODE_IDENTITY_SET               0x8047
#define NODE_IDENTITY_STATUS            0x8048

#define NODE_RESET                      0x8049
#define NODE_RESET_STATUS               0x804A

#define SIG_MODEL_APP_GET               0x804B
#define SIG_MODEL_APP_LIST              0x804C

#define VENDOR_MODEL_APP_GET            0x804D
#define VENDOR_MODEL_APP_LIST           0x804E

#define NETWORK_TRANSMIT_GET            0x8023
#define NETWORK_TRANSMIT_SET            0x8024
#define NETWORK_TRANSMIT_STATUS         0x8025

/* End of Config Model Message opcodes  */

/*! \brief Size of Company ID (BT SIG assigned) */
#define QMESH_COMP_ID_SIZE_IN_BYTES         0x0002

/*! \brief Health Model Message Opcodes */
#define HEALTH_CURRENT_STATUS               0x04
#define HEALTH_ATTENTION_GET                0x8004
#define HEALTH_ATTENTION_SET                0x8005
#define HEALTH_ATTENTION_SET_UNACK          0x8006
#define HEALTH_ATTENTION_STATUS             0x8007
#define HEALTH_FAULT_CLEAR                  0x802F
#define HEALTH_FAULT_CLEAR_UNACK            0x8030
#define HEALTH_FAULT_GET                    0x8031
#define HEALTH_FAULT_STATUS                 0x05
#define HEALTH_FAULT_TEST                   0x8032
#define HEALTH_FAULT_TEST_UNACK             0x8033
#define HEALTH_PERIOD_GET                   0x8034
#define HEALTH_PERIOD_SET                   0x8035
#define HEALTH_PERIOD_SET_UNACK             0x8036
#define HEALTH_PERIOD_STATUS                0x8037

/* End of Health Model Message opcodes  */

#endif  /* #ifndef __QMESH_FOUNDATION_MODEL_H__ */
