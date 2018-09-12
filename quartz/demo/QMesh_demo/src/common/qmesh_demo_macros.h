/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*
  * This file contains various macros used in the Application
 */
/*****************************************************************************/
#ifndef __QMESH_DEMO_MACROS_H__
#define __QMESH_DEMO_MACROS_H__

/* Client data buffer size in octets for sending config model messages */
#define CONFIG_CLI_DATA_BUFFER_SIZE                                0x20

/* 'Config Model Application Key Add' message related macros */
#define APP_KEY_ADD                                                                 0x00
#define CONFIG_MODEL_APP_KEY_ADD__OPCODE_IDX         0
#define CONFIG_MODEL_APP_KEY_ADD__OPCODE_LEN         1
#define CONFIG_MODEL_APP_KEY_ADD__KEY_IDX__IDX      (CONFIG_MODEL_APP_KEY_ADD__OPCODE_IDX + \
                                                CONFIG_MODEL_APP_KEY_ADD__OPCODE_LEN)
#define CONFIG_MODEL_APP_KEY_ADD__KEY_IDX__LEN      3
#define CONFIG_MODEL_APP_KEY_ADD__APP_KEY__IDX      (CONFIG_MODEL_APP_KEY_ADD__KEY_IDX__IDX + \
                                                CONFIG_MODEL_APP_KEY_ADD__KEY_IDX__LEN)
#define CONFIG_MODEL_APP_KEY_ADD__APP_KEY__LEN      16
#define CONFIG_MODEL_APP_KEY_ADD__MSG_LEN                 (CONFIG_MODEL_APP_KEY_ADD__OPCODE_LEN + \
                                           CONFIG_MODEL_APP_KEY_ADD__KEY_IDX__LEN + \
                                           CONFIG_MODEL_APP_KEY_ADD__APP_KEY__LEN)

/* 'Config Model App Bind' message related macros */
#define MODEL_APP_BIND                                                  0x803D
#define CONFIG_MODEL_APP_BIND__OPCODE__IDX      0
#define CONFIG_MODEL_APP_BIND__OPCODE__LEN      2
#define CONFIG_MODEL_APP_BIND__ELE_ADDR__IDX   (CONFIG_MODEL_APP_BIND__OPCODE__IDX + \
                                              CONFIG_MODEL_APP_BIND__OPCODE__LEN)
#define CONFIG_MODEL_APP_BIND__ELE_ADDR__LEN   2
#define CONFIG_MODEL_APP_BIND__KEY_ID__IDX        (CONFIG_MODEL_APP_BIND__ELE_ADDR__IDX + \
                                            CONFIG_MODEL_APP_BIND__ELE_ADDR__LEN)
#define CONFIG_MODEL_APP_BIND__KEY_ID__LEN        2
#define CONFIG_MODEL_APP_BIND__MODEL_ID_IDX     (CONFIG_MODEL_APP_BIND__KEY_ID__IDX + \
                                             CONFIG_MODEL_APP_BIND__KEY_ID__LEN)
/* Message length excluding the model id */
#define CONFIG_MODEL_APP_BIND__MSG_LEN               (CONFIG_MODEL_APP_BIND__OPCODE__LEN + \
                                        CONFIG_MODEL_APP_BIND__ELE_ADDR__LEN + \
                                        CONFIG_MODEL_APP_BIND__KEY_ID__LEN)

/* Utility macros */
#define GET_MSB_OF_UINT16(x) (((x) >> 8) & 0xFF)
#define GET_LSB_OF_UINT16(x) ((x) & 0xFF)
#define GET_BYTE_OF_UINT32(x, idx) ((uint8_t)(((uint32_t)(x) >> (uint16_t)(idx << 3)) & 0xFF))
#define UPPER_NIBBLE_MASK                                        (0xF0)
#define LOWER_NIBBLE_MASK                                       (0x0F)
#define TRANS_MIC_SIZE_4_BYTES                              (0x04)
/* Upper 4 bits of net idx represents provisioned net ID */
#define PROVISIONED_NET_ID_MASK 0xF000
/* Lower 12 bits of net idx represents 12 bit global app/net key ID */
#define KEY_ID_MASK 0x0FFF
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
#endif
