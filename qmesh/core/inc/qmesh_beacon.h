/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_beacon.h
 *  \brief Qmesh Beacon API.
 *
 *   This file contains beacon API.
 */
 /*****************************************************************************/

#ifndef _QMESH_BEACON_H_
#define _QMESH_BEACON_H_

#include "qmesh_data_types.h"
#include "qmesh_result.h"
#include "qmesh_types.h"


typedef enum
{
    QMESH_UNPROVISIONED_DEVICE_BEACON = 0,
    QMESH_SECURE_NETWORK_BEACON       = 1
} QMESH_BEACON_TYPE_T;


#define UNPROVISIONED_DEVICE_BEACON_LEN_OCTETS          19
#define UNPROVISIONED_DEVICE_BEACON_URI_LEN_OCTETS      23
#define SECURE_NETWORK_BROADCAST_BEACON_LEN_OCTETS      22
#define SEC_NET_BROADCAST_BEACON_AUTH_VALUE_LEN_OCTETS  8


QMESH_RESULT_T QmeshSendUnProvisionedBeacon(uint16_t oobInfo, uint8_t *uriHash);
QMESH_RESULT_T QmeshBeaconReceive(const uint8_t *beacon, uint8_t beacon_len);

#endif /* _QMESH_BEACON_H_ */


