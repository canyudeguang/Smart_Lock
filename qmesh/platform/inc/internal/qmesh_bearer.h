/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_bearer.h
 *  \brief Qmesh bearer APIs for QMESH library
 *
 *   This file contains the functions to manage mesh bearer.
 */
/******************************************************************************/

#ifndef __QMESH_BEARER_H__
#define __QMESH_BEARER_H__

#include "qmesh.h"

#ifdef __cplusplus
 extern "C" {
#endif

/*! \addtogroup Platform_Bearer
 *  @{
 */
/*============================================================================*
    Public Definitions
 *============================================================================*/

/*! \brief Advertising data type that contains the provisioning protocol messages */
#define QMESH_AD_TYPE_MESH_PB_ADV           (0x29u)

/*! \brief Advertising data type that contains the provisioned network messages */
#define QMESH_AD_TYPE_MESH_MESSAGE          (0x2Au)

/*! \brief Advertising data type that contains the
 *         Secure Network and Unprovisioned device beacons
 */
#define QMESH_AD_TYPE_MESH_BEACON           (0x2Bu)

/*============================================================================*
    Data Type Definitions
 *============================================================================*/

/*! \brief Advert bearer parameters */
typedef struct
{
    uint8_t retransmit_count;       /*!< \brief Retransmit count. 0 means no retransmit */
    uint8_t retransmit_interval;    /*!< \brief Retransmit interval in units of 10 millisecond
                                     *          0 means 10 millisecond, 1 means 20 millisecond and so on */
} QMESH_ADV_BEARER_MSG_PARAM_T;


/*! \brief Bearer specific parameter */
typedef union
{
    QMESH_ADV_BEARER_MSG_PARAM_T adv_params; /*!< \brief Advert bearer specific transmit parameter */
} QMESH_BEARER_MSG_PARAM_T;


/*============================================================================*
    Public Function Prototypes
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * QmeshBearerIsMeshAdType
 */
/*! \brief This function is a callback to the stack when the bearer receives
 *         a message that belongs to the stack
 *
 *  \returns Returns non-zero value if advert type is MESH related.
 */
/*---------------------------------------------------------------------------*/
#define QmeshBearerIsMeshAdType(ad_type) ((ad_type) == QMESH_AD_TYPE_MESH_PB_ADV  || \
                                          (ad_type) == QMESH_AD_TYPE_MESH_MESSAGE || \
                                          (ad_type) == QMESH_AD_TYPE_MESH_BEACON)

/*----------------------------------------------------------------------------*
 * QmeshBearerReceive
 */
/*! \brief This function is a callback to the stack when the bearer receives
 *         a message that belongs to the stack.
 *
 *  \param[in] bearer_hdr  Bearer header information.
 *  \param[in] msg         Pionter to the message. <br>
 *                         The message must contain the complete data as received
 *                         on the bearer. It should start with the AD_TYPE field
 *                         in case of LE advertising bearer and the proxy header
 *                         in case of the data received on proxy bearer.
 *  \param[in] len         Length of the payload.
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
extern void QmeshBearerReceive(QMESH_BEARER_HEADER_T *bearer_hdr,
                               uint8_t *msg,
                               uint8_t len);

/*----------------------------------------------------------------------------*
 * QmeshBearerSend
 */
/*! \brief Qmesh stack calls this API to send mesh messages to Bearer
 *
 *  \param[in] context        Context data provided by application while
 *                            calling \ref QmeshAddBearer.
 *  \param[in] use_adv_bearer TRUE when packet needs to be sent only on LE Advert Bearer.
 *  \param[in] bearer_param   Bearer specific parameter. \ref QMESH_BEARER_MSG_PARAM_T.
 *  \param[in] msg            Pointer to the message to be transmitted.<br>
 *                            The mesage contains complete packet to be
 *                            transmitted on the bearer.<br>
 *             - If it a advertising type message, it will contian the
 *               AD Type as the first octet of the msg.
 *               The bearer must prefix the AD Length when forming the ADV Report.
 *             - The Proxy bearer messages will contain the complete payload
 *               including the Mesh Proxy or Provisioning PDU headers. The
 *               stack takes care of segmentation of the messages when they are
 *               larger than Bearer MTU.
 *  \param[in] len            Length of the message.
 *                            It includes the AD_TYPE field in case of ADV type message.
 *
 *  \returns Refer to \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshBearerSend(QMESH_BEARER_CONTEXT_T context,
                                      bool use_adv_bearer,
                                      const QMESH_BEARER_MSG_PARAM_T *bearer_param,
                                      uint8_t *msg, uint16_t len);

/*!@} */

#ifdef __cplusplus
 }
#endif

#endif /* __QMESH_BEARER_H__ */

