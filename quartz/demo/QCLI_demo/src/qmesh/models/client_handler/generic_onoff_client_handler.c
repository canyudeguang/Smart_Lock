/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file generic_onoff_client_handler.c
 *      This file contains QCLI command functions definition for
 *      Generic_onoff_client model.
 *
******************************************************************************/
/* Note: this is an auto-generated file. */

/*============================================================================*
 *  Library Header Files
 *===========================================================================*/

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/
#include "generic_onoff_client_handler.h"
#include "generic_onoff_client.h"
#include "model_client_handler_utils.h"

/*============================================================================*
 *  Private Data Type
 *===========================================================================*/

/*============================================================================*
 *  Private Function Prototypes
 *===========================================================================*/

/*============================================================================*
 *  Private Function Implementations
 *===========================================================================*/

/*============================================================================*
 *  Public Function Implementations
 *===========================================================================*/
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliGenericOnoffClientGet
 *
 *  DESCRIPTION
 *      QCLI command function of GENERIC_ONOFF_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliGenericOnoffClientGet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
{
    QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;
    QMESH_MSG_HEADER_T header;
    uint8_t index = 0;
    uint8_t noOfParams = 0;

    /* number of parameters */
    noOfParams = 3;  /* nwId, dstAddr, elmtId */

    /* check num of parameters */
    if(parameters_count != noOfParams) /*common + model parameter count*/
    {
        status = QCLI_STATUS_USAGE_E;
        goto on_error;
    }

    /* check params */
    if ((!parameters[index].Integer_Is_Valid) /* nwId */ ||
        (!parameters[index++].Integer_Is_Valid) /* dstAddr */ ||
        (!parameters[index++].Integer_Is_Valid) /* elmtId */ )
    {
        status = QCLI_STATUS_USAGE_E;
        goto on_error;
    }


    QmeshMemSet(&header, 0x00, sizeof(QMESH_MSG_HEADER_T));
    /* resetting index to 0 */
    index = 0;

    /* update message header with key info, src & dest addr and ttl */
    UpdateQmeshMsgHeader(&header, parameters, &index, FALSE);


    /* call model API */
    status = GenericOnoffClientGet(&header);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliGenericOnoffClientSet
 *
 *  DESCRIPTION
 *      QCLI command function of GENERIC_ONOFF_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliGenericOnoffClientSet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
{
    QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;
    QMESH_MSG_HEADER_T header;
    GENERIC_ONOFF_CLIENT_SET_T data;
    uint8_t index = 0;
    uint8_t noOfParams = 0;
    uint8_t noOfOptionalParams = 0;

    /* number of parameters */
    noOfParams = 3;  /* nwId, dstAddr, elmtId */
    noOfParams++;  /* reliable/unreliable */
    noOfParams++;  /* onOff */
    noOfParams++;  /* tid */
    noOfOptionalParams++;  /* transitionTime */
    noOfOptionalParams++;  /* delay */

    /* check num of parameters */
    if((parameters_count != noOfParams) && 
        (parameters_count != (noOfParams + noOfOptionalParams)))/*common + model parameter count*/
    {
        status = QCLI_STATUS_USAGE_E;
        goto on_error;
    }

    /* check params */
    if ((!parameters[index].Integer_Is_Valid) /* nwId */ ||
        (!parameters[index++].Integer_Is_Valid) /* dstAddr */ ||
        (!parameters[index++].Integer_Is_Valid) /* elmtId */ ||
        (!parameters[index++].Integer_Is_Valid) /* reliable/unreliable */ ||
        (!parameters[index++].Integer_Is_Valid) /* onOff */ ||
        (!parameters[index++].Integer_Is_Valid) /* tid */ )
    {
        status = QCLI_STATUS_USAGE_E;
        goto on_error;
    }

    /* check optional params */
    if ((parameters_count == (noOfParams + noOfOptionalParams) && 
        ((!parameters[index++].Integer_Is_Valid) /* transitionTime */ ||
        (!parameters[index++].Integer_Is_Valid) /* delay */)))
    {
        status = QCLI_STATUS_USAGE_E;
        goto on_error;  
    }

    QmeshMemSet(&header, 0x00, sizeof(QMESH_MSG_HEADER_T));
    QmeshMemSet(&data, 0x00, sizeof(GENERIC_ONOFF_CLIENT_SET_T));
    /* resetting index to 0 */
    index = 0;

    /* update message header with key info, src & dest addr, 
     * reliable/unreliable and ttl */
    UpdateQmeshMsgHeader(&header, parameters, &index, TRUE);

    /* onOff */
    data.onOff = parameters[index++].Integer_Value;
    /* tid */
    data.tid = parameters[index++].Integer_Value;
    /* optional params */
    if (parameters_count == (noOfParams + noOfOptionalParams))
    {
        /* transitionTime */
        data.transitionTime = parameters[index++].Integer_Value;
        /* delay */
        data.delay = parameters[index++].Integer_Value;
        /* update transition time validity flag */
        data.validTransitionTime = TRUE;
    }

    /* call model API */
    status = GenericOnoffClientSet(&header, &data);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}

