/******************************************************************************
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_frnd_common.h
 *  \brief defines and functions for qmesh_frnd_common.c
 *
 *   This file contains the implementation of transport layer functionality. The
 *   transport layer uses AES-CCM to encrypt and authenticate application data. It
 *   also defines how application messages are segmented and reassembled into multi-
 *   ple transport PDUs.
 */
 /*****************************************************************************/

#ifndef __QMESH_FRND_COMMON_H__
#define __QMESH_FRND_COMMON_H__

#include "qmesh_data_types.h"
#include "qmesh_result.h"
#include "qmesh_friendship.h"

#define CTRL_MSG_OPCODE_LEN     1
#define PRIM_ELM_ID             0
#define FRND_UPDATE_MD_IDX 5

/* Enumeration type for MD bit format */
typedef enum
{
    frnd_queue_empty,
    frnd_queue_not_empty,
} more_data_field_t;


#define FRND_POLL_MSG_LEN               (0x01)
#define FRND_UPDATE_MSG_LEN             (0x06)
#define FRND_REQUEST_MSG_LEN            (0x0A)
#define FRND_OFFER_MSG_LEN              (0x06)
#define FRND_SBU_LIST_CFM_MSG_LEN       (0x01)
#define FRND_CLR_MSG_LEN                (0x04)
#define FRND_CLEAR_CFM_MSG_LEN          (0x04)


#endif /* __QMESH_FRND_COMMON_H__ */

