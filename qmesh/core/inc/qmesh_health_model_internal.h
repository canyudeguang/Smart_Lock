/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_health_model_internal.h
 *  \brief Health Model functionality for Qmesh Library.
 *
 *   This file contains the data types and helper macros required for Qmesh
 *   Health Server & Client Models.
 */
/*****************************************************************************/

#ifndef _QMESH_HEALTH_MODEL_INTERNAL_H_
#define _QMESH_HEALTH_MODEL_INTERNAL_H_

#include "qmesh_types.h"

/*============================================================================*
   Public Definitions
 *============================================================================*/
/*! \brief Size of Test ID (Identifier of most recently performed test) */
#define QMESH_TEST_ID_SIZE_IN_BYTES                     0x0001u

/************************ Health Server Definitions **************************/
/*! \brief Health Period Divisor value MIN */
#define HEALTH_PERIOD_DIVISOR_MIN                       0x00u

/*! \brief Health Period Divisor value MAX */
#define HEALTH_PERIOD_DIVISOR_MAX                       0x0Fu

/*! \brief Health Period Divisor value INVALID */
#define HEALTH_PERIOD_DIVISOR_INVALID                   (HEALTH_PERIOD_DIVISOR_MAX + 1)

/*! \brief Health Publication period Disabled */
#define HEALTH_PUB_PERIOD_DISABLED                      0x00000000u

/*! \brief Health Message Size in Bytes */
#define QMESH_HEALTH_CURRENT_STATUS_HDR_SIZE_IN_BYTES   0x0004u
#define QMESH_HEALTH_FAULT_STATUS_HDR_SIZE_IN_BYTES     0x0004u
#define QMESH_HEALTH_PERIOD_STATUS_SIZE_IN_BYTES        0x0003u
#define QMESH_HEALTH_ATTENTION_STATUS_SIZE_IN_BYTES     0x0003u

/*! \brief Health Status message min in Bytes */
#define QMESH_HEALTH_STATUS_MIN_SIZE_IN_BYTES           QMESH_ONE_BYTE_OPCODE_SIZE  + \
                                                        QMESH_TEST_ID_SIZE_IN_BYTES + \
                                                        QMESH_COMP_ID_SIZE_IN_BYTES + \

/*! \brief Publication Step resolution values */
#define QMESH_PUB_STEP_RESOLUTION_0                     0x00u
#define QMESH_PUB_STEP_RESOLUTION_1                     0x01u
#define QMESH_PUB_STEP_RESOLUTION_2                     0x02u
#define QMESH_PUB_STEP_RESOLUTION_3                     0x03u

/*! \brief Company ID Pack */
#define HEALTH_COMP_ID_PACK(a, b)                       (a | (b << 8))

/*! \brief Health model message retransmit interval */
#define HEALTH_MSG_RETRANSMIT_INTERVAL                  (150 * QMESH_MILLISECOND)

/*! \brief Health model message retransmit interval */
#define HEALTH_MODEL_RETRANSMIT_COUNT                   (10)

/************************ Health Client Definitions **************************/
/*! \brief Health Client Message Max Size in Bytes */
#define QMESH_HEALTH_CLI_MAX_MSG_SIZE_IN_BYTES          QMESH_TWO_BYTES_OPCODE_SIZE + \
                                                        QMESH_TEST_ID_SIZE_IN_BYTES + \
                                                        QMESH_COMP_ID_SIZE_IN_BYTES

#define QMESH_HEALTH_STATUS_TEST_ID_IDX                 (0u)
#define QMESH_HEALTH_STATUS_TEST_ID_LEN                 (1u)
#define QMESH_HEALTH_STATUS_COMP_ID_IDX                 (QMESH_HEALTH_STATUS_TEST_ID_IDX + \
                                                         QMESH_HEALTH_STATUS_TEST_ID_LEN)
#define QMESH_HEALTH_STATUS_COMP_ID_LEN                 (2u)
#define QMESH_HEALTH_STATUS_TEST_ARR_IDX                (QMESH_HEALTH_STATUS_COMP_ID_IDX + \
                                                         QMESH_HEALTH_STATUS_COMP_ID_LEN)
#define QMESH_HEALTH_ATTENTION_STATUS_IDX               (0u)
#define QMESH_HEALTH_FAST_PERIOD_DIVISOR_IDX            (0u)

/*!@} */
#endif /* _QMESH_HEALTH_MODEL_INTERNAL_H_ */

