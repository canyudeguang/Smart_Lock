/*=============================================================================
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_pts_test.h
 *  \brief Qmesh APIs for PTS Test Mode.
 *
 *   This file contains the API for Qmesh APIs for PTS Test Mode.
 */
/******************************************************************************/

#ifndef __QMESH_PTS_TEST_H__
#define __QMESH_PTS_TEST_H__

#include "qmesh_types.h"
/*! \addtogroup PTS_Test
* @{
*/


/*============================================================================*
    Macro Definitions
 *============================================================================*/

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/

/*============================================================================*
    Data Type Definitions
 *============================================================================*/

/*! \brief Test Information: Access layer data information. */
typedef struct
{
    uint16_t          payload_len;    /*!< Payload length. */
    const uint8_t    *payload;        /*!< Payload buffer. */
} QMESH_TEST_PTS_ACCESS_PDU_T;

/*============================================================================*
    Public Function Implementations
 *============================================================================*/
/*----------------------------------------------------------------------------*
 * QmeshSetPTSMode
 */
/*! \brief Enables or disables PTS mode. This mode is useful for PTS
 *         testing to control the working Mesh core stack. APIs defined in this file
 *         work only when this is enabled.
 *
 * \param[in] enable If TRUE, enable PTS-related changes, otherwise disable PTS changes.
 *
 * \return None.
 */
/*----------------------------------------------------------------------------*/
extern void QmeshSetPTSMode(bool enable);

/*----------------------------------------------------------------------------*
 * QmeshEnableIVTestMode
 */
/*! \brief Enables the IV test mode. This bypasses the 96 hour check for IV
 *         state change.
 *  \note1hang PTS Mode needs to be enabled for IV test mode to be enabled.
 *             See \ref QmeshSetPTSMode
 *
 * \param[in] enable If TRUE, IV Test mode is enabled and disabled when FALSE.
 *
 * \return None.
 */
/*----------------------------------------------------------------------------*/
void QmeshEnableIVTestMode(bool enable);

/*----------------------------------------------------------------------------*
 * QmeshSendIVTestModeSignal
 */
/*! \brief This API transitions the IV state to requested state. When requested
 *         state is \ref QMESH_IV_UPDATE_STATE_IN_PROGRESS current IV will be
 *         incremented and state will be transitioned to IV update in progress.
 *
 *  \note1hang IV test mode needs to be enabled for sending IV Test signals.
 *             See \ref QmeshEnableIVTestMode
 *
 * \param[in] net_idx  Provisioned Network Index.
 * \param[in] signal   IV Update state to transition. See \ref QMESH_IV_UPDATE_STATE_T
 *
 * \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
QMESH_RESULT_T QmeshSendIVTestModeSignal(uint8_t net_idx, QMESH_IV_UPDATE_STATE_T signal);

/*----------------------------------------------------------------------------*
* QmeshSetSecureNetworkBeaconPeriod
*/
/*! \brief Changes the secure network beacon broadcast period. Note that
 *         randomness is added between the period selected. This API
 *         can be used for testing key refresh over secure network beacon test
 *         cases when PTS does not wait long enough as the default/dynamically-computed
 *         beacon period. \n
 *         This API works only when PTS Test mode is enabled via \ref QmeshSetPTSMode.
 *
 * \param[in] net_key_info   Provisioned network and Subnetwork Information.
 *                       See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 * \param[in] beacon_period  Beacon period in seconds.
 *
 * \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSetSecureNetworkBeaconPeriod(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info,
                                                        uint16_t beacon_period);

/*----------------------------------------------------------------------------*
* QmeshAddDevKey
*/
/*! \brief Adds a device key.
 *
 *  \param[in] net_key_info  Provisioned network and subnetwork information.
 *                           See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *  \param[in] dev_key       Pointer to the device key.
 *
 * \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshAddDevKey(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info,
                                     const QMESH_DEVICE_KEY_T dev_key);

/*----------------------------------------------------------------------------*
 * QmeshUpdateModelAppBind
 */
/*! \brief Binds or unbinds an application key
 *         with a model on an element.
 *
 *         This function is only supported in a provisioner role
 *
 *  \param[in] prov_net_id  Provisioned Network Index.
 *  \param[in] bind_info    Pointer to Model Appkey binding information
 *                          See \ref QMESH_MODEL_APPKEY_BIND_INFO_T.
 *  \param[in] bind         TRUE to bind, FALSE to unbind.
 *
 *  \return \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshUpdateModelAppBind(uint8_t prov_net_id,
                                              const QMESH_MODEL_APPKEY_BIND_INFO_T *bind_info,
                                              bool bind);

/*----------------------------------------------------------------------------*
 * QmeshClearRPL
 */
/*! \brief Clears the replay protection list.
 *
 *  Removes all entries from the RPL.
 *
 *  \param[in] prov_net_id     Provisioned Network Index.
 *
 *  \return \ref QMESH_RESULT_T
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshClearRPL(uint8_t prov_net_id);

/*----------------------------------------------------------------------------*
 * QmeshSetMaxSubnet
 */
/*! \brief Sets the maximum number of subnetworks supported for the specified
 *         provisioned network.
 *
 *  \param[in] prov_net_id     Provisioned Network Index.
 *  \param[in] max_sub_net     Maximum number of supported subnetworks per provisioned network.
 *
 *  \return \ref QMESH_RESULT_T
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSetMaxSubnet(uint8_t prov_net_id, uint16_t max_sub_net);
/*!@} */
#endif /* __QMESH_PTS_TEST_H__ */

