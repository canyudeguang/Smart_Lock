/*=============================================================================
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_config_server.h
 *  \brief Configuration Server Model functionality for Qmesh Library.
 *
 *   This file contains the function that implements handling of Qmesh
 *   Configuration Server Model.
 */
/*****************************************************************************/

#ifndef __QMESH_CONFIG_SERVER_H__
#define __QMESH_CONFIG_SERVER_H__

#include "qmesh_types.h"

/*! \addtogroup Core_Config_Server
 * @{
 */

/*============================================================================*
   Public Definitions
 *============================================================================*/

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/
/*! \brief Configuration model subscription set operation. */
typedef enum
{
    QMESH_MODEL_SUB_EVENT_ADD        = 0, /*!< Subscription add event. */
    QMESH_MODEL_SUB_EVENT_DELETE     = 1, /*!< Subscription delete event. */
    QMESH_MODEL_SUB_EVENT_OVERWRITE  = 2, /*!< Subscription overwrite event. This is
                                           *   equivalent to QMESH_MODEL_SUB_EVENT_DELETE_ALL
                                           *   followed by QMESH_MODEL_SUB_EVENT_ADD.
                                           */
    QMESH_MODEL_SUB_EVENT_DELETE_ALL = 3, /*!< Subscription delete all event. */
} QMESH_MODEL_SUB_OPCODE_T;

/*! \brief Configuration model app bind or unbind event. */
typedef enum
{
    QMESH_CONFIG_APP_BIND           = 0, /*!< App bind event. */
    QMESH_CONFIG_APP_UNBIND         = 1, /*!< App unBind event. */
} QMESH_CONFIG_APP_BIND_STATE_T;

/*============================================================================*
    Data Type Definitions
 *============================================================================*/
/*! \brief Element and model for which configuration parameters have changed
 *         via config model commands.
 */
typedef struct
{
    const QMESH_ELEMENT_CONFIG_T *p_ele;    /*!< Pointer to the element for which configuration parameters have changed.
                                             */
    const QMESH_MODEL_DATA_T *p_model;      /*!< Pointer to the model for which configuration parameters have changed.
                                             */
    QMESH_MODEL_TYPE_T model_type;          /*!< SIG model ID or vendor model ID. */
    uint32_t model_id;                      /*!< Model ID. */
} QMESH_ELEMENT_MODEL_INFO_T;

/*! \brief App bind or unbind event information. */
typedef struct
{
    QMESH_ELEMENT_MODEL_INFO_T ele_model_info; /*!< Element and model for which the app bind state has changed.
                                                */
    QMESH_CONFIG_APP_BIND_STATE_T bind_state;  /*!< App key bind state. */
    uint16 app_key_idx;                        /*!< Global app key index. */
} QMESH_CONFIG_APP_BIND_EVENT_T;

/*! \brief Configuration model subscription set event information. */
typedef struct
{
    QMESH_ELEMENT_MODEL_INFO_T ele_model_info; /*!< Element and model for which a subscription state has changed.
                                                */
    QMESH_MODEL_SUB_OPCODE_T sub_operation;    /*!< Subscription operation.
                                                *          See \ref QMESH_MODEL_SUB_OPERATION_T.
                                                */
    uint16_t sub_addr;
/*!< Subscription address. This will be the actual address for a nonvirtual address. In the case
*   of a virtual address, this will have the encoded virtual address. The application can
*   use QmeshGetVirtualAddressUuid() to get the actual virtual address. When the
*   operation is QMESH_MODEL_SUB_EVENT_DELETE_ALL, the application must ignore the subscription
*   address and label_uuid.
*/
   const uint16_t *label_uuid;                 /*!< Pointer to the label UUID. This will be NULL for a nonvirtual address.
                                                */
} QMESH_CONFIG_SUB_SET_EVENT_T;

/*! \brief Configuration model publication set event information. */
typedef QMESH_ELEMENT_MODEL_INFO_T QMESH_CONFIG_PUBLICATION_SET_EVENT_T;

/*! \brief Configuration model network key event information. */
typedef QMESH_KEY_IDX_INFO_T QMESH_CONFIG_NETKEY_EVENT_T;

/*============================================================================*
    Public API
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * QmeshConfigModelServerHandler
 */
/*! \brief Handler for configuration model server messages.
  *
  *  \param[in] nw_hdr      Network header information.
  *                         See \ref QMESH_NW_HEADER_INFO_T.
  *  \param[in] key_info    App payload security information.
  *                         See \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T.
  *  \param[in] elem_config Data specific to each element.
  *                         See \ref QMESH_ELEMENT_CONFIG_T.
  *  \param[in] model_data  Model data. See \ref QMESH_MODEL_DATA_T.
  *  \param[in] msg         Pointer to the config model data payload.
  *  \param[in] msg_len     Config model data payload length.
  *
  *  \returns
  *      returns \ref QMESH_RESULT_T.
  */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshConfigModelServerHandler(const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
                                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                                    QMESH_MODEL_DATA_T *model_data,
                                                    const uint8_t *msg,
                                                    uint16_t msg_len);

/*!@} */
#endif /* __QMESH_CONFIG_SERVER_H__ */

