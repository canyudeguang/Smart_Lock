/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_helper.h
 *  \brief Helper functions for Qmesh Library.
 *
 *   This file contains the helper function used in Qmesh stack lbrary.
 */
/*****************************************************************************/

#ifndef __QMESH_HELPER_H__
#define __QMESH_HELPER_H__

#include "qmesh_types.h"

/*===========================================================================*
 *  Public Definitions
 *===========================================================================*/
/* Macro to check if the IV State is within the valid range. */
#define QmeshIsIvStateValid(x) ((x) <= QMESH_IV_UPDATE_STATE_IN_PROGRESS)

/* Macro to check if the Key Refresh State is within the valid range. */
#define QmeshIsKeyRefreshStateValid(x) ((x) <= QMESH_KEY_REFRESH_PHASE_3)

/* Macro to check if the Beacon State is within the valid range. */
#define QmeshIsBeaconStateValid(x) ((x) <= QMESH_SECURE_NW_BEACON_STATE_START)

/* Macro to check if the Default TTL is within the valid range. */
#define QmeshIsDefaultTTLValid(x) ((0 == ((x) & 0x80)) && (1 != (x)))

/* Macro to check if the Relay State is within the valid range. */
#define QmeshIsRelayStateValid(x) ((x) <= QMESH_RELAY_STATE_NOT_SUPPORTED)

/* Macro to check if the GATT Proxy State is within the valid range. */
#define QmeshIsGattProxyStateValid(x) ((x) <= QMESH_GATT_PROXY_STATE_NOT_SUPPORTED)

/* Macro to check if the global network/app key is index is valid. */
#define QmeshIsGlobalKeyIndexValid(x) (0 == ((x) & 0xF000))

/* Macro to check if the Node Identity State is valid. */
#define QmeshIsNodeIdentityStateValid(x) ((x) <= QMESH_NODE_IDENTITY_NOT_SUPPORTED)

/* Macro to check if the element address is valid. */
#define QmeshIsElementAddressValid(x) (((x) & 0x8000) == 0 && \
                                     (x) != QMESH_UNASSIGNED_ADDRESS)

/* Macro to check if the Model Type is valid. */
#define QmeshIsModelTypeValid(x) ((x) <= QMESH_MODEL_TYPE_VENDOR)

/* Macro to check if Publish TTL is valid. Valid range is 0x00 - 0x7F and 0xFF. */
#define QmeshIsPublishTTLValid(x) (((x) < 0x80) || ((x) == 0xFF))

/* Macro to check if Access Layer TTL is valid. Valid range is 0x00 - 0x7F and 0xFF. */
#define QmeshIsAccessTTLValid(x) (((x) < 0x80) || ((x) == 0xFF))

/* Macro to check if Subscription address is valid. The value of the Address field
 * shall not be an unassigned address, unicast address, all-nodes address
 * or virtual address. (See Section 4.3.2.19)
 */
#define QmeshIsSubscriptionAddressValid(x) ((((x) & 0xC000) == 0xC000) && \
                                            ((x) != QMESH_ADDRESS_ALL_NODES))

/* Macro to check if the Friend State is valid. */
#define QmeshIsFriendStateValid(x) ((x) <= QMESH_FRIEND_STATE_NOT_SUPPORTED)

/*===========================================================================*
 *  Public Function Prototypes
 *===========================================================================*/
/*----------------------------------------------------------------------------*
 * QmeshMemCpyUnpackSwapEndianess
 */
/*! \brief Swap endianness of the buffer and unpack.
 *
 *  \param  dest    Destination buffer.
 *  \param  src     Source buffer. Its a 16 bit array.
 *  \param  len     length of source in bytes.
 */
/*---------------------------------------------------------------------------*/
extern void QmeshMemCpyUnpackSwapEndianess(uint8_t *dest, const uint16_t *src, uint16_t len);

/*----------------------------------------------------------------------------*
 * QmeshMemCpyU16SwapEndian
 */
/*! \brief Reverse the long words and also, swap the bytes inside a word.
 *
 *  \param  dest    Destination buffer.
 *  \param  src     Source buffer. Its a 16 bit array.
 *  \param  len     length of source in bytes.
 */
/*---------------------------------------------------------------------------*/
extern void QmeshMemCpyU16SwapEndian(uint16_t *dest, const uint16_t *src, uint16_t len);


#endif /* __QMESH_HELPER_H__ */

