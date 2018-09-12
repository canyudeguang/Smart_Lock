/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/

#include "model_client_handler_utils.h"
#include "qmesh_demo_config.h"

/*----------------------------------------------------------------------------*
 *  NAME
 *      UpdateQmeshMsgHeader
 *
 *  DESCRIPTION
 *      Updates the message header with parameters passed in 'parameters' and increments
 *      the 'index' with number of values read from 'parameters'.
 *
 *  RETURNS/MODIFIES
 *      Nothing
 *
 *----------------------------------------------------------------------------*/
void UpdateQmeshMsgHeader (QMESH_MSG_HEADER_T *header,
                                    QCLI_Parameter_t * parameters,
                                    uint8_t *index,
                                    bool hasReliableParam)
{
    uint8_t idx = 0;
    /* AppKey Info */
    header->keyInfo.prov_net_idx = (parameters[idx].Integer_Value & 0xF000) >> 12;
    header->keyInfo.key_idx = parameters[idx++].Integer_Value & 0x0FFF;
    header->use_frnd_keys = FALSE;
    /* Dst Addr */
    header->dstAddr = parameters[idx++].Integer_Value;
    /* Src Addr */
    header->srcAddr = parameters[idx++].Integer_Value;
    /* Reliable / Unreliable */
    if (hasReliableParam)
    {
        header->reliable = (parameters[idx++].Integer_Value == 1) ? TRUE : FALSE;
    }
    /* TTL */
    header->ttl = QCLI_DFLT_TTL;
    /* Update no of QCLI paramter read */
    *index = idx;
}


