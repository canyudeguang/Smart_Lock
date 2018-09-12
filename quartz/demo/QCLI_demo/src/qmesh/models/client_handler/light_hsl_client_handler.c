/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file light_hsl_client_handler.c
 *      This file contains QCLI command functions definition for
 *      Light_hsl_client model.
 *
******************************************************************************/
/* Note: this is an auto-generated file. */

/*============================================================================*
 *  Library Header Files
 *===========================================================================*/

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/
#include "light_hsl_client_handler.h"
#include "light_hsl_client.h"
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
 *      QcliLightHslClientGet
 *
 *  DESCRIPTION
 *      QCLI command function of LIGHT_HSL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliLightHslClientGet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
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
    status = LightHslClientGet(&header);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliLightHslClientSet
 *
 *  DESCRIPTION
 *      QCLI command function of LIGHT_HSL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliLightHslClientSet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
{
    QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;
    QMESH_MSG_HEADER_T header;
    LIGHT_HSL_CLIENT_SET_T data;
    uint8_t index = 0;
    uint8_t noOfParams = 0;
    uint8_t noOfOptionalParams = 0;

    /* number of parameters */
    noOfParams = 3;  /* nwId, dstAddr, elmtId */
    noOfParams++;  /* reliable/unreliable */
    noOfParams++;  /* hSLLightness */
    noOfParams++;  /* hSLHue */
    noOfParams++;  /* hSLSaturation */
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
        (!parameters[index++].Integer_Is_Valid) /* hSLLightness */ ||
        (!parameters[index++].Integer_Is_Valid) /* hSLHue */ ||
        (!parameters[index++].Integer_Is_Valid) /* hSLSaturation */ ||
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
    QmeshMemSet(&data, 0x00, sizeof(LIGHT_HSL_CLIENT_SET_T));
    /* resetting index to 0 */
    index = 0;

    /* update message header with key info, src & dest addr, 
     * reliable/unreliable and ttl */
    UpdateQmeshMsgHeader(&header, parameters, &index, TRUE);

    /* hSLLightness */
    data.hSLLightness = parameters[index++].Integer_Value;
    /* hSLHue */
    data.hSLHue = parameters[index++].Integer_Value;
    /* hSLSaturation */
    data.hSLSaturation = parameters[index++].Integer_Value;
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
    status = LightHslClientSet(&header, &data);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliLightHslClientHueGet
 *
 *  DESCRIPTION
 *      QCLI command function of LIGHT_HSL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliLightHslClientHueGet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
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
    status = LightHslClientHueGet(&header);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliLightHslClientHueSet
 *
 *  DESCRIPTION
 *      QCLI command function of LIGHT_HSL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliLightHslClientHueSet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
{
    QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;
    QMESH_MSG_HEADER_T header;
    LIGHT_HSL_CLIENT_HUE_SET_T data;
    uint8_t index = 0;
    uint8_t noOfParams = 0;
    uint8_t noOfOptionalParams = 0;

    /* number of parameters */
    noOfParams = 3;  /* nwId, dstAddr, elmtId */
    noOfParams++;  /* reliable/unreliable */
    noOfParams++;  /* hue */
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
        (!parameters[index++].Integer_Is_Valid) /* hue */ ||
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
    QmeshMemSet(&data, 0x00, sizeof(LIGHT_HSL_CLIENT_HUE_SET_T));
    /* resetting index to 0 */
    index = 0;

    /* update message header with key info, src & dest addr, 
     * reliable/unreliable and ttl */
    UpdateQmeshMsgHeader(&header, parameters, &index, TRUE);

    /* hue */
    data.hue = parameters[index++].Integer_Value;
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
    status = LightHslClientHueSet(&header, &data);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliLightHslClientSaturationGet
 *
 *  DESCRIPTION
 *      QCLI command function of LIGHT_HSL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliLightHslClientSaturationGet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
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
    status = LightHslClientSaturationGet(&header);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliLightHslClientSaturationSet
 *
 *  DESCRIPTION
 *      QCLI command function of LIGHT_HSL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliLightHslClientSaturationSet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
{
    QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;
    QMESH_MSG_HEADER_T header;
    LIGHT_HSL_CLIENT_SATURATION_SET_T data;
    uint8_t index = 0;
    uint8_t noOfParams = 0;
    uint8_t noOfOptionalParams = 0;

    /* number of parameters */
    noOfParams = 3;  /* nwId, dstAddr, elmtId */
    noOfParams++;  /* reliable/unreliable */
    noOfParams++;  /* saturation */
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
        (!parameters[index++].Integer_Is_Valid) /* saturation */ ||
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
    QmeshMemSet(&data, 0x00, sizeof(LIGHT_HSL_CLIENT_SATURATION_SET_T));
    /* resetting index to 0 */
    index = 0;

    /* update message header with key info, src & dest addr, 
     * reliable/unreliable and ttl */
    UpdateQmeshMsgHeader(&header, parameters, &index, TRUE);

    /* saturation */
    data.saturation = parameters[index++].Integer_Value;
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
    status = LightHslClientSaturationSet(&header, &data);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliLightHslClientTargetGet
 *
 *  DESCRIPTION
 *      QCLI command function of LIGHT_HSL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliLightHslClientTargetGet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
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
    status = LightHslClientTargetGet(&header);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliLightHslClientDefaultGet
 *
 *  DESCRIPTION
 *      QCLI command function of LIGHT_HSL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliLightHslClientDefaultGet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
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
    status = LightHslClientDefaultGet(&header);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliLightHslClientDefaultSet
 *
 *  DESCRIPTION
 *      QCLI command function of LIGHT_HSL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliLightHslClientDefaultSet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
{
    QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;
    QMESH_MSG_HEADER_T header;
    LIGHT_HSL_CLIENT_DEFAULT_SET_T data;
    uint8_t index = 0;
    uint8_t noOfParams = 0;

    /* number of parameters */
    noOfParams = 3;  /* nwId, dstAddr, elmtId */
    noOfParams++;  /* reliable/unreliable */
    noOfParams++;  /* lightness */
    noOfParams++;  /* hue */
    noOfParams++;  /* saturation */

    /* check num of parameters */
    if(parameters_count != noOfParams) /*common + model parameter count*/
    {
        status = QCLI_STATUS_USAGE_E;
        goto on_error;
    }

    /* check params */
    if ((!parameters[index].Integer_Is_Valid) /* nwId */ ||
        (!parameters[index++].Integer_Is_Valid) /* dstAddr */ ||
        (!parameters[index++].Integer_Is_Valid) /* elmtId */ ||
        (!parameters[index++].Integer_Is_Valid) /* reliable/unreliable */ ||
        (!parameters[index++].Integer_Is_Valid) /* lightness */ ||
        (!parameters[index++].Integer_Is_Valid) /* hue */ ||
        (!parameters[index++].Integer_Is_Valid) /* saturation */ )
    {
        status = QCLI_STATUS_USAGE_E;
        goto on_error;
    }


    QmeshMemSet(&header, 0x00, sizeof(QMESH_MSG_HEADER_T));
    QmeshMemSet(&data, 0x00, sizeof(LIGHT_HSL_CLIENT_DEFAULT_SET_T));
    /* resetting index to 0 */
    index = 0;

    /* update message header with key info, src & dest addr, 
     * reliable/unreliable and ttl */
    UpdateQmeshMsgHeader(&header, parameters, &index, TRUE);

    /* lightness */
    data.lightness = parameters[index++].Integer_Value;
    /* hue */
    data.hue = parameters[index++].Integer_Value;
    /* saturation */
    data.saturation = parameters[index++].Integer_Value;

    /* call model API */
    status = LightHslClientDefaultSet(&header, &data);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliLightHslClientRangeGet
 *
 *  DESCRIPTION
 *      QCLI command function of LIGHT_HSL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliLightHslClientRangeGet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
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
    status = LightHslClientRangeGet(&header);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}
/*----------------------------------------------------------------------------*
 *  NAME
 *      QcliLightHslClientRangeSet
 *
 *  DESCRIPTION
 *      QCLI command function of LIGHT_HSL_CLIENT model.
 *
 *  RETURNS/MODIFIES
 *      QCLI_Command_Status_t - Result of the operation
 *
 *----------------------------------------------------------------------------*/
extern QCLI_Command_Status_t QcliLightHslClientRangeSet(uint32_t parameters_count, QCLI_Parameter_t * parameters)
{
    QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;
    QMESH_MSG_HEADER_T header;
    LIGHT_HSL_CLIENT_RANGE_SET_T data;
    uint8_t index = 0;
    uint8_t noOfParams = 0;

    /* number of parameters */
    noOfParams = 3;  /* nwId, dstAddr, elmtId */
    noOfParams++;  /* reliable/unreliable */
    noOfParams++;  /* hueRangeMin */
    noOfParams++;  /* hueRangeMax */
    noOfParams++;  /* saturationRangeMin */
    noOfParams++;  /* saturationRangeMax */

    /* check num of parameters */
    if(parameters_count != noOfParams) /*common + model parameter count*/
    {
        status = QCLI_STATUS_USAGE_E;
        goto on_error;
    }

    /* check params */
    if ((!parameters[index].Integer_Is_Valid) /* nwId */ ||
        (!parameters[index++].Integer_Is_Valid) /* dstAddr */ ||
        (!parameters[index++].Integer_Is_Valid) /* elmtId */ ||
        (!parameters[index++].Integer_Is_Valid) /* reliable/unreliable */ ||
        (!parameters[index++].Integer_Is_Valid) /* hueRangeMin */ ||
        (!parameters[index++].Integer_Is_Valid) /* hueRangeMax */ ||
        (!parameters[index++].Integer_Is_Valid) /* saturationRangeMin */ ||
        (!parameters[index++].Integer_Is_Valid) /* saturationRangeMax */ )
    {
        status = QCLI_STATUS_USAGE_E;
        goto on_error;
    }


    QmeshMemSet(&header, 0x00, sizeof(QMESH_MSG_HEADER_T));
    QmeshMemSet(&data, 0x00, sizeof(LIGHT_HSL_CLIENT_RANGE_SET_T));
    /* resetting index to 0 */
    index = 0;

    /* update message header with key info, src & dest addr, 
     * reliable/unreliable and ttl */
    UpdateQmeshMsgHeader(&header, parameters, &index, TRUE);

    /* hueRangeMin */
    data.hueRangeMin = parameters[index++].Integer_Value;
    /* hueRangeMax */
    data.hueRangeMax = parameters[index++].Integer_Value;
    /* saturationRangeMin */
    data.saturationRangeMin = parameters[index++].Integer_Value;
    /* saturationRangeMax */
    data.saturationRangeMax = parameters[index++].Integer_Value;

    /* call model API */
    status = LightHslClientRangeSet(&header, &data);
    if (QCLI_STATUS_SUCCESS_E != status) 
    {
        status = QCLI_STATUS_ERROR_E;
    }


on_error: 

    return status;
}

