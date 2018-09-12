/*=============================================================================
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_ps_db_interface.h
 *  \brief Qmesh Persistent Storage API.
 *
 *   This file contains the API for Qmesh Persistent Storage - DB interfaces.
 *
 */
 /*****************************************************************************/

#ifndef _QMESH_PS_DB_INTERFACE_H_
#define _QMESH_PS_DB_INTERFACE_H_

#include "qmesh_types.h"
#include "qmesh_data_types.h"
#include "qmesh_result.h"
#include "qmesh_private.h"
#include "qmesh_ps.h"

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/

typedef enum
{
    QMESH_RESET_STATE_IN_PROGRESS    = 0, /* Device reset in progress */
    QMESH_RESET_STATE_COMPLETE       = 1  /* Device reset complete */
} QMESH_RESET_STATE_T;

/*============================================================================*
    Data Type Definitions
 *============================================================================*/
typedef struct
{
    uint8_t                        prov_net_idx;
    QMESH_RESET_STATE_T            reset_state;
} QMESH_RESET_STATE_PS_DATA_T;

typedef struct
{
    uint8_t                        prov_net_idx;
    uint8_t                        ttl;
} QMESH_TTL_PS_DATA_T;

typedef struct
{
    uint8_t                         prov_net_idx;
    QMESH_TRANSMIT_STATE_T          network_retransmit;
} QMESH_NET_RETRANSMIT_PS_DATA_T;

typedef struct
{
    uint8_t                         prov_net_idx;
   QMESH_RELAY_STATE_INFO_T         relay_state_info;
} QMESH_RELAY_INFO_PS_DATA_T;

typedef struct
{
    uint8_t                         prov_net_idx;
    QMESH_SECURE_NW_BEACON_STATE_T  beacon_state;
} QMESH_BEACON_STATE_PS_DATA_T;

typedef struct
{
    uint8_t                         prov_net_idx;
    QMESH_FRIEND_STATE_T            friend_state;
} QMESH_FRIEND_STATE_PS_DATA_T;

typedef struct
{
    QMESH_KEY_REFRESH_PHASE_T       kr_phase;
    QMESH_SUBNET_KEY_IDX_INFO_T     key_idx;
    QMESH_NETWORK_KEY_T             cur_key;
    QMESH_NETWORK_KEY_T             new_key;
} QMESH_NET_KEY_PS_DATA_T;

typedef struct
{
    QMESH_KEY_REFRESH_PHASE_T       kr_phase;
    QMESH_APP_KEY_IDX_INFO_T        app_key_idx;
    QMESH_SUBNET_KEY_IDX_INFO_T     nw_key_idx;
    QMESH_APPLICATION_KEY_T         cur_key;
    QMESH_APPLICATION_KEY_T         new_key;
} QMESH_APP_KEY_PS_DATA_T;

typedef struct
{
    uint8_t                         prov_net_idx;
    QMESH_DEVICE_KEY_T              key;
} QMESH_DEV_KEY_PS_DATA_T;

typedef struct
{
    uint8_t                         prov_net_idx;
    QMESH_IV_UPDATE_STATE_T         state;
    uint32_t                        iv_index;
} QMESH_IVI_KEY_PS_DATA_T;

typedef struct
{
    uint8_t                             prov_net_idx;
    QMESH_HEARTBEAT_PUBLICATION_STATE_T hb_pub_state;
} QMESH_HB_PUB_KEY_PS_DATA_T;

typedef union
{
    QMESH_RESET_STATE_PS_DATA_T    reset_state;
    QMESH_TTL_PS_DATA_T            ttl;
    QMESH_NET_RETRANSMIT_PS_DATA_T net_retransmit_info;
    QMESH_RELAY_INFO_PS_DATA_T     relay_info;
    QMESH_NET_KEY_PS_DATA_T        net_key;
    QMESH_APP_KEY_PS_DATA_T        app_key;
    QMESH_DEV_KEY_PS_DATA_T        dev_key;
    QMESH_IVI_KEY_PS_DATA_T        ivi;
    QMESH_BEACON_STATE_PS_DATA_T   beacon_state;
    QMESH_FRIEND_STATE_PS_DATA_T   friend_state;
    QMESH_HB_PUB_KEY_PS_DATA_T     hb_pub_state;
} QMESH_PS_DATA_T;

typedef QMESH_RESULT_T (*QMESH_PS_KEY_READ_HANDLER_T) (const QMESH_PS_DATA_T *ps_data);

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
    API Definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * QmeshRestoreFromPS
 */
/*! \brief Restores DB from PS.
 *
 *  \param net_idx  Provisioned net index.
 *
 *  \returns \ref QMESH_RESULT_T
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshRestoreFromPS(uint8_t net_idx);

/*----------------------------------------------------------------------------*
 * QmeshDeviceDataDeleteHandler
 */
/*! \brief Clears the data for all the keys in PS.
 *
 *  \param net_idx  Provisioned net index.
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
extern void QmeshDeviceDataDeleteHandler(uint8_t net_idx);

/*----------------------------------------------------------------------------*
 * QmesResetStatePsWriteHandler
 */
/*! \brief Stores Device Reset State
 *
 *  \param net_idx       Provisioned network index.
 *  \param reset_state   Device reset state.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshResetStatePsWriteHandler(uint8_t net_idx, QMESH_RESET_STATE_T reset_state);

/*----------------------------------------------------------------------------*
 * QmeshTTLPsWriteHandler
 */
/*! \brief Stores TTL to PS
 *
 *  \param net_idx  Provisioned network index.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshTTLPsWriteHandler(uint8_t net_idx, uint8_t ttl);

/*----------------------------------------------------------------------------*
 * QmeshTTLPsDeleteHandler
 */
/*! \brief Delete TTL info in PS.
 *
 *  \param net_idx  Provisioned network index.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshTTLPsDeleteHandler(uint8_t net_idx);

/*----------------------------------------------------------------------------*
 * QmeshNetRetransmitPsWriteHandler
 */
/*! \brief Stores Network Retransmit Interval & count to PS
 *
 *  \param net_idx  Provisioned network index.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshNetRetransmitPsWriteHandler(uint8_t net_idx, QMESH_TRANSMIT_STATE_T nw_state);

/*----------------------------------------------------------------------------*
 * QmeshNetRetransmitPsDeleteHandler
 */
/*! \brief Delete Network Retransmit Interval & count in PS.
 *
 *  \param net_idx  Provisioned network index.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshNetRetransmitPsDeleteHandler(uint8_t net_idx);

/*----------------------------------------------------------------------------*
 * QmeshRelayPsWriteHandler
 */
/*! \brief Stores Relay Info to PS
 *
 *  \param net_idx  Provisioned network index.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshRelayPsWriteHandler(uint8_t net_idx, const QMESH_RELAY_STATE_INFO_T *relay_state);

/*----------------------------------------------------------------------------*
 * QmeshRelayPsDeleteHandler
 */
/*! \brief Delete Relay Info in PS.
 *
 *  \param net_idx  Provisioned network index.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshRelayPsDeleteHandler(uint8_t net_idx);

/*----------------------------------------------------------------------------*
 * QmeshIviPsWriteHandler
 */
/*! \brief Stores IVI to PS
 *
 *  \param net_idx     Provisioned network index.
 *  \param state       IV state.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshIviPsWriteHandler(uint8_t net_idx, uint32_t iv_index,
                                             QMESH_IV_UPDATE_STATE_T state);

/*----------------------------------------------------------------------------*
 * QmeshIviPsDeleteHandler
 */
/*! \brief Delete IVI in PS.
 *
 *  \param net_idx  Provisioned network index.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshIviPsDeleteHandler(uint8_t net_idx);

/*----------------------------------------------------------------------------*
 * QmeshDevKeyPsWriteHandler
 */
/*! \brief Stores dev key to PS
 *
 *  \param prov_net_idx  Provisioned network index.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshDevKeyPsWriteHandler(uint8_t net_idx, const QMESH_DEVICE_KEY_T dev_key);

/*----------------------------------------------------------------------------*
 * QmeshDevKeyPsDeleteHandler
 */
/*! \brief Delete dev key in PS.
 *
 *  \param net_idx  Provisioned network index.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshDevKeyPsDeleteHandler(uint8_t net_idx);

/*----------------------------------------------------------------------------*
 * QmeshNetKeyPsWriteHandler
 */
/*! \brief Stores net key in PS.
 *
 *  \param sub_net  Network key data.
 *  \old_key        Old Key when the key refresh state is other than QMESH_KEY_REFRESH_NORMAL_OPERATION
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshNetKeyPsWriteHandler(const QMESH_SUBNET_ADD_INFO_T *subnet_keyinfo,
                                                const uint16_t *current_network_key);

/*----------------------------------------------------------------------------*
 * QmeshNetKeyPsDeleteHandler
 */
/*! \brief Delete net key in PS.
 *
 *  \param net_key_idx  Net key index to be deleted.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshNetKeyPsDeleteHandler(QMESH_SUBNET_KEY_IDX_INFO_T net_key_idx);

/*----------------------------------------------------------------------------*
 * QmeshAppKeyPsWriteHandler
 */
/*! \brief Stores app key in PS.
 *
 *  \param app_key  Application key data.
 *  \param key_state Key State
 *  \param old_key Old Key when the key refresh state is other than QMESH_KEY_REFRESH_NORMAL_OPERATION
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshAppKeyPsWriteHandler(const QMESH_APPKEY_ADD_INFO_T *app_key,
                                                QMESH_KEY_REFRESH_PHASE_T kr_phase,
                                                uint16_t *old_key);

/*----------------------------------------------------------------------------*
 * QmeshAppKeyPsDeleteHandler
 */
/*! \brief Delete app key in PS.
 *
 *  \param app_key_idx  App key index to be deleted.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshAppKeyPsDeleteHandler(QMESH_SUBNET_KEY_IDX_INFO_T app_key_idx);

/*----------------------------------------------------------------------------*
 * QmeshBeaconStatePsWriteHandler
 */
/*! \brief Stores Beacon State in PS.
 *
 *  \param prov_net_idx  Provisioned network index.
 *  \param state         Beacon state.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshBeaconStatePsWriteHandler(uint8_t net_idx, QMESH_SECURE_NW_BEACON_STATE_T beacon_state);

/*----------------------------------------------------------------------------*
 * QmeshBeaconStatePsDeleteHandler
 */
/*! \brief Delete Beacon State in PS.
 *
 *  \param net_idx  Provisioned network index.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshBeaconStatePsDeleteHandler(uint8_t net_idx);

/*----------------------------------------------------------------------------*
 * QmeshFriendStatePsWriteHandler
 */
/*! \brief Stores Friend State in PS.
 *
 *  \param net_idx    Provisioned network index.
 *  \param state      Friend state.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshFriendStatePsWriteHandler(uint8_t net_idx, const QMESH_FRIEND_STATE_T friend_state);

/*----------------------------------------------------------------------------*
 * QmeshFriendStatePsDeleteHandler
 */
/*! \brief Delete Friend State in PS.
 *
 *  \param net_idx  Provisioned network index.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshFriendStatePsDeleteHandler(uint8_t net_idx);

/*----------------------------------------------------------------------------*
 * QmeshHBPubPsWriteHandler
 */
/*! \brief Stores Heartbeat Publication state to PS
 *
 *  \param net_idx  Provisioned network index.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshHBPubPsWriteHandler(uint8_t net_idx,
                                               const QMESH_HEARTBEAT_PUBLICATION_STATE_T *hb_pub_state);

/*----------------------------------------------------------------------------*
 * QmeshHBPubPsDeleteHandler
 */
/*! \brief Delete Heartbeat Publication state in PS.
 *
 *  \param net_idx  Provisioned network index.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshHBPubPsDeleteHandler(uint8_t net_idx);


#ifdef __cplusplus
}
#endif

#endif /* _QMESH_PS_DB_INTERFACE_H_ */

