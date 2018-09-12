/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file generic_level_client_handler.c
 *      This file contains QCLI command functions definition for
 *      Generic_level_client model.
 *
******************************************************************************/
/* Note: this is an auto-generated file. */

/*============================================================================*
 *  Library Header Files
 *===========================================================================*/

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/
#include "generic_level_client_handler.h"
#include "generic_level_client.h"
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
 *      QcliGenericLevelClientGet
 *
 *  DESCRIPTION
 *      QCLI command function of GENERIC_LEVEL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliGenericLevelClientGet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
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
    status = GenericLevelClientGet(&header);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliGenericLevelClientSet
 *
 *  DESCRIPTION
 *      QCLI command function of GENERIC_LEVEL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliGenericLevelClientSet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
{
    QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;
    QMESH_MSG_HEADER_T header;
    GENERIC_LEVEL_CLIENT_SET_T data;
    uint8_t index = 0;
    uint8_t noOfParams = 0;
    uint8_t noOfOptionalParams = 0;

    /* number of parameters */
    noOfParams = 3;  /* nwId, dstAddr, elmtId */
    noOfParams++;  /* reliable/unreliable */
    noOfParams++;  /* level */
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
        (!parameters[index++].Integer_Is_Valid) /* level */ ||
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
    QmeshMemSet(&data, 0x00, sizeof(GENERIC_LEVEL_CLIENT_SET_T));
    /* resetting index to 0 */
    index = 0;

    /* update message header with key info, src & dest addr, 
     * reliable/unreliable and ttl */
    UpdateQmeshMsgHeader(&header, parameters, &index, TRUE);

    /* level */
    data.level = parameters[index++].Integer_Value;
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
    status = GenericLevelClientSet(&header, &data);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliGenericLevelClientDeltaSet
 *
 *  DESCRIPTION
 *      QCLI command function of GENERIC_LEVEL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliGenericLevelClientDeltaSet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
{
    QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;
    QMESH_MSG_HEADER_T header;
    GENERIC_LEVEL_CLIENT_DELTA_SET_T data;
    uint8_t index = 0;
    uint8_t noOfParams = 0;
    uint8_t noOfOptionalParams = 0;

    /* number of parameters */
    noOfParams = 3;  /* nwId, dstAddr, elmtId */
    noOfParams++;  /* reliable/unreliable */
    noOfParams++;  /* deltaLevel */
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
        (!parameters[index++].Integer_Is_Valid) /* deltaLevel */ ||
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
    QmeshMemSet(&data, 0x00, sizeof(GENERIC_LEVEL_CLIENT_DELTA_SET_T));
    /* resetting index to 0 */
    index = 0;

    /* update message header with key info, src & dest addr, 
     * reliable/unreliable and ttl */
    UpdateQmeshMsgHeader(&header, parameters, &index, TRUE);

    /* deltaLevel */
    data.deltaLevel = parameters[index++].Integer_Value;
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
    status = GenericLevelClientDeltaSet(&header, &data);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliGenericLevelClientMoveSet
 *
 *  DESCRIPTION
 *      QCLI command function of GENERIC_LEVEL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliGenericLevelClientMoveSet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
{
    QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;
    QMESH_MSG_HEADER_T header;
    GENERIC_LEVEL_CLIENT_MOVE_SET_T data;
    uint8_t index = 0;
    uint8_t noOfParams = 0;
    uint8_t noOfOptionalParams = 0;

    /* number of parameters */
    noOfParams = 3;  /* nwId, dstAddr, elmtId */
    noOfParams++;  /* reliable/unreliable */
    noOfParams++;  /* deltaLevel */
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
        (!parameters[index++].Integer_Is_Valid) /* deltaLevel */ ||
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
    QmeshMemSet(&data, 0x00, sizeof(GENERIC_LEVEL_CLIENT_MOVE_SET_T));
    /* resetting index to 0 */
    index = 0;

    /* update message header with key info, src & dest addr, 
     * reliable/unreliable and ttl */
    UpdateQmeshMsgHeader(&header, parameters, &index, TRUE);

    /* deltaLevel */
    data.deltaLevel = parameters[index++].Integer_Value;
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
    status = GenericLevelClientMoveSet(&header, &data);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}

