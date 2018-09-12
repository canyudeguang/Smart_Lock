/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_health_server.h
 *  \brief Health Model functionality for Qmesh Library.
 *
 *   This file contains the data types required for Qmesh
 *   Health Server Model.
 */
/*****************************************************************************/

#ifndef __QMESH_HEALTH_SERVER_H__
#define __QMESH_HEALTH_SERVER_H__

#include "qmesh_types.h"
#include "qmesh_soft_timers.h"

/*============================================================================*
    Data Type Definitions
 *============================================================================*/

/*! \brief Health Model Server state.
 *         This should be used as model data for HEALTH server model.
 */
typedef struct
{
    uint8_t                          fast_period_divisor;  /*!< \brief fast period divisor for health status msg */
    uint16_t                         company_id;           /*!< \brief company id */
    uint32_t                         pub_interval;         /*!< \brief Publication interval for health status msg */
    QMESH_TIMER_HANDLE_T             pub_tid;              /*!< \brief Publication timer id */
    QMESH_HEALTH_MODEL_DATA_T        *fault;               /*!< \brief Fault array */
    QMESH_MODEL_PUBLISH_STATE_T      *pub_info;            /*!< \brief Publication info */
} QMESH_HEALTH_SERVER_STATE_T;

/*============================================================================*
   Public function prototype
 *============================================================================*/
/*! \addtogroup Core_Health_Server
* @{
*/

/*----------------------------------------------------------------------------*
 * QmeshHealthServerPublicationHandler
 */
/*! \brief Publication handler for the health model server.
 *
 *  \param[in] model_data  Model data information.
 *                         See \ref QMESH_MODEL_DATA_T.
 *
 *  \return None.
 */
/*----------------------------------------------------------------------------*/
extern void QmeshHealthServerPublicationHandler(QMESH_MODEL_DATA_T *model_data);

/*----------------------------------------------------------------------------*
 * QmeshHealthModelServerHandler
 */
/*! \brief Handler for health model server messages.
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
 *  \returns
 *      returns \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshHealthModelServerHandler(const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
                                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                                    QMESH_MODEL_DATA_T *model_data,
                                                    const uint8_t *msg,
                                                    uint16_t msg_len);

/*!@} */
#endif /* __QMESH_HEALTH_SERVER_H__ */

