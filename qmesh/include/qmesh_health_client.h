/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_health_client.h
 *  \brief Health Model functionality for Qmesh Library.
 *
 *   This file contains the data types required for Qmesh
 *   Health Client Model.
 */
/*****************************************************************************/

#ifndef __QMESH_HEALTH_CLIENT_H__
#define __QMESH_HEALTH_CLIENT_H__

#include "qmesh_types.h"

/*! \addtogroup Core_Health_Client
* @{
*/

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/
/*! \brief Health Client Commands. */
typedef enum
{
    QMESH_HEALTH_CLI_FAULT_GET,             /*!< \brief Gets the fault state of a node \ref QMESH_HEALTH_FAULT_STATUS */
    QMESH_HEALTH_CLI_FAULT_CLEAR,           /*!< \brief Clears the fault array of a node \ref QMESH_HEALTH_FAULT_STATUS */
    QMESH_HEALTH_CLI_FAULT_CLEAR_UNACK,     /*!< \brief Clears the fault array of a node */
    QMESH_HEALTH_CLI_ATTENTION_GET,         /*!< \brief Gets the attention state of a node \ref QMESH_HEALTH_ATTENTION_STATUS */
    QMESH_HEALTH_CLI_PERIOD_GET,            /*!< \brief Gets the health fast period divisor field of a node \ref QMESH_HEALTH_PERIOD_STATUS */
    QMESH_HEALTH_CLI_ATTENTION_SET,         /*!< \brief Sets the attention state of a node \ref QMESH_HEALTH_ATTENTION_STATUS */
    QMESH_HEALTH_CLI_ATTENTION_SET_UNACK,   /*!< \brief Sets the attention state of a node */
    QMESH_HEALTH_CLI_FAULT_TEST,            /*!< \brief Invokes self-test procedure of a node \ref QMESH_HEALTH_FAULT_STATUS */
    QMESH_HEALTH_CLI_FAULT_TEST_UNACK,      /*!< \brief Invokes self-test procedure of a node */
    QMESH_HEALTH_CLI_PERIOD_SET,            /*!< \brief Sets the health fast period divisor field of a node \ref QMESH_HEALTH_PERIOD_STATUS */
    QMESH_HEALTH_CLI_PERIOD_SET_UNACK       /*!< \brief Sets the health fast period divisor field of a node */
}QMESH_HEALTH_CLIENT_CMD_T;

/*============================================================================*
    Data Type Definitions
 *============================================================================*/
/*! \brief Health fault get type. */
typedef struct
{
    uint16_t   company_id; /*!< 16-bit Bluetooth assigned company identifier. */
} QMESH_HEALTH_FAULT_GET;

/*! \brief Health fault clear type. */
typedef struct
{
    uint16_t   company_id; /*!< 16-bit Bluetooth assigned company identifier. */
} QMESH_HEALTH_FAULT_CLEAR;

/*! \brief Health fault test type. */
typedef struct
{
    uint8_t    test_id;    /*!< Identifier of a specific test to be performed. */
    uint16_t   company_id; /*!< 16-bit Bluetooth assigned company identifier. */
} QMESH_HEALTH_FAULT_TEST;

/*! \brief Health period set type. */
typedef struct
{
    uint8_t    fast_period_divisor;    /*!< Divider of a publish period. */
} QMESH_HEALTH_PERIOD_SET;

/*! \brief Health attention set type. */
typedef struct
{
    uint8_t    attention;      /*!< Value of the attention timer. */
} QMESH_HEALTH_ATTENTION_SET;

/*! \brief Health fault status type. */
typedef struct
{
    uint8_t        test_id;        /*!< Identifier of a specific test to be performed. */
    uint16_t       company_id;     /*!< 16-bit Bluetooth assigned company identifier. */
    uint8_t        fault_arry_len; /*!< Fault array length. */
    const uint8_t  *fault_array;   /*!< Fault array fields contain a sequence of fault values. */
} QMESH_HEALTH_FAULT_STATUS;

/*! \brief Health attention status type. */
typedef struct
{
    uint8_t    attention;      /*!< Value of the attention timer. */
} QMESH_HEALTH_ATTENTION_STATUS;

/*! \brief Health period status type. */
typedef struct
{
    uint8_t    fast_period_divisor; /*!< Divider of a publish period. */
} QMESH_HEALTH_PERIOD_STATUS;

/*! \brief Health client command data. */
typedef union
{
    QMESH_HEALTH_FAULT_TEST         fault_test;
    QMESH_HEALTH_FAULT_GET          fault_get;
    QMESH_HEALTH_FAULT_CLEAR        fault_clear;
    QMESH_HEALTH_PERIOD_SET         period_set;
    QMESH_HEALTH_ATTENTION_SET      attention_set;
} QMESH_HEALTH_CLIENT_CMD_DATA_T;

/*! \brief Health client event information. */
typedef union
{
    QMESH_HEALTH_FAULT_STATUS               fault_status;
    QMESH_HEALTH_ATTENTION_STATUS           attention_status;
    QMESH_HEALTH_PERIOD_STATUS              period_status;
} QMESH_HEALTH_CLIENT_EVT_INFO_T;

/*! \brief Health client event data. */
typedef struct
{
    const QMESH_NW_HEADER_INFO_T   *nw_hdr;
    QMESH_SUBNET_KEY_IDX_INFO_T     key_info;
    QMESH_HEALTH_CLIENT_EVT_INFO_T  event_info;
} QMESH_HEALTH_CLIENT_EVT_DATA_T;

/*! \brief Health client command information. */
typedef struct
{
    QMESH_SUBNET_KEY_IDX_INFO_T  key_info;   /*!< Application key information. */
    uint16_t                     dst_addr;   /*!< Destination address. */
    uint16_t                     src_addr;   /*!< Unicast Address of the element with Health Client Model. */
    uint8_t                      ttl;        /*!< TTL of the message. */
} QMESH_HEALTH_CLIENT_CMD_INFO_T;

/*============================================================================*
    Public API
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * QmeshSendHealthClientCmd
 */
/*! \brief Sends a health client command to the node. The
  *        command response is provided in QMESH_HEALTH_CLIENT_EVT_T.
  *
  *  \param[in] cmd_info   Command information needed to send the command.
  *                        See \ref QMESH_HEALTH_CLIENT_CMD_INFO_T.
  *  \param[in] cmd        Command to be sent. See \ref QMESH_HEALTH_CLIENT_CMD_DATA_T.
  *  \param[in] cmd_data   Pointer to the data relevant for the command.
  *
  *  \return \ref QMESH_RESULT_T.
  */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSendHealthClientCmd(const QMESH_HEALTH_CLIENT_CMD_INFO_T *cmd_info,
                                               QMESH_HEALTH_CLIENT_CMD_T cmd,
                                               const QMESH_HEALTH_CLIENT_CMD_DATA_T *cmd_data);

/*----------------------------------------------------------------------------*
 * QmeshHealthModelClientHandler
 */
/*! \brief Handler for health model client messages.
  *
  *  \param[in] nw_hdr      Network header information.
  *                         See \ref QMESH_NW_HEADER_INFO_T.
  *  \param[in] key_info    App payload security information.
  *                         See \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T.
  *  \param[in] elem_config Data specific to each element.
  *                         See \ref QMESH_ELEMENT_CONFIG_T.
  *  \param[in] model_data  Model data. See \ref QMESH_MODEL_DATA_T.
  *  \param[in] msg         Pointer to the health model data payload.
  *  \param[in] msg_len     Health model data payload length.
  *
  *  \return \ref QMESH_RESULT_T.
  */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshHealthModelClientHandler(const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
                                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                                    QMESH_MODEL_DATA_T *model_data,
                                                    const uint8_t *msg,
                                                    uint16_t msg_len);
/*!@} */
#endif /* __QMESH_HEALTH_CLIENT_H__ */

