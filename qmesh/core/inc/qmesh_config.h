/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_config.h
 *  \brief Qmesh library configuration parameters
 *
 *   This file contains the library configuration parameters
 */
 /*****************************************************************************/
#ifndef __QMESH_CONFIG_H__
#define __QMESH_CONFIG_H__

#include "qmesh_data_types.h"

/*! \addtogroup Core
 * @{
 */


/*! \brief  Enable Provisioner role related functionality.
*/
#ifndef QMESH_SUPPORT_PROVISIONER_ROLE
#define QMESH_SUPPORT_PROVISIONER_ROLE           (1)
#endif

/*! \brief  Sequence number threshold for IV update
*/
#define MESH_SEQ_NUM_THRESHOLD                   (50)

#define QMESH_DEFAULT_TTL_MAX                    (127)

/* Size of << SRC, SQN >> cache Table per provisioned network */
#define SRC_SQN_LOOKUP_TABLE_SIZE                (16)

/* Number of application key bind per sub network network */
#define MAX_NO_OF_APP_KEY_BIND_SUPPORT_PER_NETWORK      (4)

/* Number of different app key (keys related to different permissions) supported
*  per provisioned network. Each network key represent different levels of permissions.
*/
#define MAX_NUM_SUB_NET_PER_PROVISIONING         (4)

/* Maximum Provisioned Networks or Device Instances supported.
 * NOTE: We can support maximum of 16 only, so don't change to
 * a higher value. On CSR10xx it is 1.
 */
#ifdef PLATFORM_CSR10XX
#define QMESH_MAX_DEVICE_INSTANCES               (1)
#else
#define QMESH_MAX_DEVICE_INSTANCES               (16)
#endif /* PLATFORM_CSR10XX */

#define QMESH_MAX_NETWORKS                       (QMESH_MAX_DEVICE_INSTANCES)

#define QMESH_MAX_SUBNETS                        (4)
#define QMESH_MAX_APPKEYS                        (8)
#define QMESH_MAX_FRNDKEYS                       (8)

/* Number of provisioned network supported */
#define MAX_NO_OF_PROVISIONED_NETWORK            (1)

/* Number of proxy filter addresses Supported */
#define MAX_PROXY_FILTER_ADDRESS                 (10)

/*! Maximum composition data page num defined currently */
#define QMESH_COMP_DATA_MAX_PAGE                 (0)

/*! Maximum Number of Proxy Bearers. */
#define QMESH_MAX_PROXY_BEARERS                  (4)

/*!@} */

#endif /* __QMESH_CONFIG_H__ */

