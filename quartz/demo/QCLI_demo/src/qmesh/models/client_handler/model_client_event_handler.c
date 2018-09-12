/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*! \file model_client_event_handler.c
 *      This file contains function definition for function processing
 *      SIG Mesh Client Models events.
 *
******************************************************************************/

/*============================================================================*
 *  Library Header Files
 *===========================================================================*/
#include "qcli_api.h"

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/
#include "model_client_event_handler.h"
#include "qmesh_model_common.h"
#include "qmesh_vendor_model_handler.h"

/*============================================================================*
 *  Private Data Type
 *===========================================================================*/
extern QCLI_Group_Handle_t mesh_model_group;

#ifdef ENABLE_THROUGHPUT_TESTING
static uint32_t count = 0;
#endif /* ENABLE_THROUGHPUT_TESTING */

#define  QCLI_LOGI(m,...) QCLI_Printf(m,__VA_ARGS__)
/*============================================================================*
 *  Private Function Prototypes
 *===========================================================================*/

/*============================================================================*
 *  Private Function Implementations
 *===========================================================================*/

/*============================================================================*
 *  Public Function Implementations
 *===========================================================================*/
static uint32_t getMessageOpcode(const uint8_t *msg, uint16_t *offset)
{
    uint32_t opcode = 0x00;
    /* Retrieve the opcode from the message */
    switch (get_opcode_format(msg[MODEL_OPCODE_OFFSET]))
    {
        case QMESH_OPCODE_FORMAT_ONE_BYTE:
            {
                opcode = msg[0];
                *offset = 1;
            }
            break;
            
        case QMESH_OPCODE_FORMAT_TWO_BYTE:
            {
                opcode = QMESH_PACK_TWO_BYTE_OPCODE(msg[0], msg[1]);
                *offset = 2;
            }
            break;
        
        case QMESH_OPCODE_FORMAT_THREE_BYTE:
            {
                opcode = QMESH_PACK_THREE_BYTE_OPCODE(msg[0], msg[1], msg[2]);
                *offset = 3;
            }
            break;
    }

    return opcode;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericOnOffClientHandler
 *
 *  DESCRIPTION
 *      This function handles Generic On Off client model events. 
 *
 *  RETURNS/MODIFIES
 *      QmeshResult
 *
 *----------------------------------------------------------------------------*/
QmeshResult QmeshGenericOnOffClientHandler (const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *app_key_info,
                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                    QMESH_MODEL_DATA_T *model_data,
                                    const uint8_t *msg,
                                    uint16_t msg_len)
{
    uint32_t opcode = 0x00;
    uint16_t offset = 0;
    QmeshResult result = QMESH_RESULT_SUCCESS;

    if(!msg || msg_len<=0)
    {
        return QMESH_RESULT_FAILURE;
    }

    /* Retrieve the opcode from the message */
    opcode = getMessageOpcode(msg, &offset);

    if (opcode == QMESH_GENERIC_ONOFF_CLIENT_STATUS)
    {
        QCLI_LOGI(mesh_model_group,"%s\n", __FUNCTION__);
        QCLI_LOGI(mesh_model_group,"\tElm Addr : 0x%x\n", elem_config->element_data->unicast_addr);
        QCLI_LOGI(mesh_model_group,"\tKey Idx : 0x%x\n", app_key_info->key_info.key_idx);
        QCLI_LOGI(mesh_model_group,"\tSrc Addr : 0x%x\n", nw_hdr->src_addr);
    }

    switch(opcode)
    {
        case QMESH_GENERIC_ONOFF_CLIENT_STATUS:
            {
                uint8_t presentOnOff = 0;
                /* Optional Fields */
                uint8_t targetOnOff = 0;
                uint8_t remainingTime = 0;
                
                presentOnOff = READ_uint8_t(msg+offset); /* presentOnOff */
                offset += sizeof(uint8_t);
                
                if (msg_len > offset)
                {
                    targetOnOff = READ_uint8_t(msg+offset); /* targetOnOff */
                    offset += sizeof(uint8_t);
                }
                if (msg_len > offset)
                {
                    remainingTime = READ_uint8_t(msg+offset); /* remainingTime */
                    offset += sizeof(uint8_t);
                }

                QCLI_LOGI (mesh_model_group, "\tReceived GENERIC_ONOFF_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tpresentOnOff : 0x%x\n",presentOnOff);
                QCLI_LOGI (mesh_model_group, "\ttargetOnOff : 0x%x\n",targetOnOff);
                QCLI_LOGI (mesh_model_group, "\tremainingTime : 0x%x\n",remainingTime);
            }
            break;

        default:
            result = QMESH_RESULT_FAILURE;
            break;
    }

    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerOnOffClientHandler
 *
 *  DESCRIPTION
 *      This function handles Generic Power On Off client model events. 
 *
 *  RETURNS/MODIFIES
 *      QmeshResult
 *
 *----------------------------------------------------------------------------*/
QmeshResult QmeshGenericPowerOnOffClientHandler (const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *app_key_info,
                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                    QMESH_MODEL_DATA_T *model_data,
                                    const uint8_t *msg,
                                    uint16_t msg_len)
{
    uint32_t opcode = 0x00;
    uint16_t offset = 0;
    QmeshResult result = QMESH_RESULT_SUCCESS;

    if(!msg || msg_len<=0)
    {
        return QMESH_RESULT_FAILURE;
    }

    /* Retrieve the opcode from the message */
    opcode = getMessageOpcode(msg, &offset);

    if (opcode == QMESH_GENERIC_POWER_ONOFF_CLIENT_STATUS)
    {
        QCLI_LOGI(mesh_model_group,"%s\n", __FUNCTION__);
        QCLI_LOGI(mesh_model_group,"\tElm Addr : 0x%x\n", elem_config->element_data->unicast_addr);
        QCLI_LOGI(mesh_model_group,"\tKey Idx : 0x%x\n", app_key_info->key_info.key_idx);
        QCLI_LOGI(mesh_model_group,"\tSrc Addr : 0x%x\n", nw_hdr->src_addr);
    }

    switch(opcode)
    {
        case QMESH_GENERIC_POWER_ONOFF_CLIENT_STATUS:
            {
                uint8_t onPowerUp = 0;
                /* Optional Fields */
                
                onPowerUp = READ_uint8_t(msg+offset); /* onPowerUp */
                offset += sizeof(uint8_t);
                

                QCLI_LOGI (mesh_model_group, "\tReceived GENERIC_ONPOWERUP_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tonPowerUp : 0x%x\n",onPowerUp);
            }
            break;

        default:
            result = QMESH_RESULT_FAILURE;
            break;
    }

    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericLevelClientHandler
 *
 *  DESCRIPTION
 *      This function handles Generic Level client model events. 
 *
 *  RETURNS/MODIFIES
 *      QmeshResult
 *
 *----------------------------------------------------------------------------*/
QmeshResult QmeshGenericLevelClientHandler (const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *app_key_info,
                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                    QMESH_MODEL_DATA_T *model_data,
                                    const uint8_t *msg,
                                    uint16_t msg_len)
{
    uint32_t opcode = 0x00;
    uint16_t offset = 0;
    QmeshResult result = QMESH_RESULT_SUCCESS;

    if(!msg || msg_len<=0)
    {
        return QMESH_RESULT_FAILURE;
    }

    /* Retrieve the opcode from the message */
    opcode = getMessageOpcode(msg, &offset);

    if (opcode == QMESH_GENERIC_LEVEL_CLIENT_STATUS)
    {
        QCLI_LOGI(mesh_model_group,"%s\n", __FUNCTION__);
        QCLI_LOGI(mesh_model_group,"\tElm Addr : 0x%x\n", elem_config->element_data->unicast_addr);
        QCLI_LOGI(mesh_model_group,"\tKey Idx : 0x%x\n", app_key_info->key_info.key_idx);
        QCLI_LOGI(mesh_model_group,"\tSrc Addr : 0x%x\n", nw_hdr->src_addr);
    }

    switch(opcode)
    {
        case QMESH_GENERIC_LEVEL_CLIENT_STATUS:
            {
                int16_t presentLevel = 0;
                /* Optional Fields */
                int16_t targetLevel = 0;
                uint8_t remainingTime = 0;
                
                presentLevel = READ_int16_t(msg+offset); /* presentLevel */
                offset += sizeof(int16_t);
                
                if (msg_len > offset)
                {
                    targetLevel = READ_int16_t(msg+offset); /* targetLevel */
                    offset += sizeof(int16_t);
                }
                if (msg_len > offset)
                {
                    remainingTime = READ_uint8_t(msg+offset); /* remainingTime */
                    offset += sizeof(uint8_t);
                }

                QCLI_LOGI (mesh_model_group, "\tReceived GENERIC_LEVEL_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tpresentLevel : 0x%x\n",presentLevel);
                QCLI_LOGI (mesh_model_group, "\ttargetLevel : 0x%x\n",targetLevel);
                QCLI_LOGI (mesh_model_group, "\tremainingTime : 0x%x\n",remainingTime);
            }
            break;

        default:
            result = QMESH_RESULT_FAILURE;
            break;
    }

    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericDefaultTransitionTimeClientHandler
 *
 *  DESCRIPTION
 *      This function handles Generic Default Transition Time client model events. 
 *
 *  RETURNS/MODIFIES
 *      QmeshResult
 *
 *----------------------------------------------------------------------------*/
QmeshResult QmeshGenericDefaultTransitionTimeClientHandler (const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *app_key_info,
                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                    QMESH_MODEL_DATA_T *model_data,
                                    const uint8_t *msg,
                                    uint16_t msg_len)
{
    uint32_t opcode = 0x00;
    uint16_t offset = 0;
    QmeshResult result = QMESH_RESULT_SUCCESS;

    if(!msg || msg_len<=0)
    {
        return QMESH_RESULT_FAILURE;
    }

    /* Retrieve the opcode from the message */
    opcode = getMessageOpcode(msg, &offset);

    if (opcode == QMESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_STATUS)
    {
        QCLI_LOGI(mesh_model_group,"%s\n", __FUNCTION__);
        QCLI_LOGI(mesh_model_group,"\tElm Addr : 0x%x\n", elem_config->element_data->unicast_addr);
        QCLI_LOGI(mesh_model_group,"\tKey Idx : 0x%x\n", app_key_info->key_info.key_idx);
        QCLI_LOGI(mesh_model_group,"\tSrc Addr : 0x%x\n", nw_hdr->src_addr);
    }

    switch(opcode)
    {
        case QMESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_STATUS:
            {
                uint8_t transitionTime = 0;
                /* Optional Fields */
                
                transitionTime = READ_uint8_t(msg+offset); /* transitionTime */
                offset += sizeof(uint8_t);
                

                QCLI_LOGI (mesh_model_group, "\tReceived GENERIC_DEFAULT_TRANSITION_TIME_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\ttransitionTime : 0x%x\n",transitionTime);
            }
            break;

        default:
            result = QMESH_RESULT_FAILURE;
            break;
    }

    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshGenericPowerLevelClientHandler
 *
 *  DESCRIPTION
 *      This function handles Generic Power Level client model events. 
 *
 *  RETURNS/MODIFIES
 *      QmeshResult
 *
 *----------------------------------------------------------------------------*/
QmeshResult QmeshGenericPowerLevelClientHandler (const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *app_key_info,
                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                    QMESH_MODEL_DATA_T *model_data,
                                    const uint8_t *msg,
                                    uint16_t msg_len)
{
    uint32_t opcode = 0x00;
    uint16_t offset = 0;
    QmeshResult result = QMESH_RESULT_SUCCESS;

    if(!msg || msg_len<=0)
    {
        return QMESH_RESULT_FAILURE;
    }

    /* Retrieve the opcode from the message */
    opcode = getMessageOpcode(msg, &offset);

    if ((opcode == QMESH_GENERIC_POWER_LEVEL_CLIENT_STATUS) ||
        (opcode == QMESH_GENERIC_POWER_LEVEL_CLIENT_LAST_STATUS) ||
        (opcode == QMESH_GENERIC_POWER_LEVEL_CLIENT_DEFAULT_STATUS) ||
        (opcode == QMESH_GENERIC_POWER_LEVEL_CLIENT_RANGE_STATUS))
    {
        QCLI_LOGI(mesh_model_group,"%s\n", __FUNCTION__);
        QCLI_LOGI(mesh_model_group,"\tElm Addr : 0x%x\n", elem_config->element_data->unicast_addr);
        QCLI_LOGI(mesh_model_group,"\tKey Idx : 0x%x\n", app_key_info->key_info.key_idx);
        QCLI_LOGI(mesh_model_group,"\tSrc Addr : 0x%x\n", nw_hdr->src_addr);
    }

    switch(opcode)
    {
        case QMESH_GENERIC_POWER_LEVEL_CLIENT_STATUS:
            {
                int16_t presentPower = 0;
                /* Optional Fields */
                int16_t targetPower = 0;
                uint8_t remainingTime = 0;
                
                presentPower = READ_int16_t(msg+offset); /* presentPower */
                offset += sizeof(int16_t);
                
                if (msg_len > offset)
                {
                    targetPower = READ_int16_t(msg+offset); /* targetPower */
                    offset += sizeof(int16_t);
                }
                if (msg_len > offset)
                {
                    remainingTime = READ_uint8_t(msg+offset); /* remainingTime */
                    offset += sizeof(uint8_t);
                }

                QCLI_LOGI (mesh_model_group, "\tReceived GENERIC_POWER_LEVEL_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tpresentPower : 0x%x\n",presentPower);
                QCLI_LOGI (mesh_model_group, "\ttargetPower : 0x%x\n",targetPower);
                QCLI_LOGI (mesh_model_group, "\tremainingTime : 0x%x\n",remainingTime);
            }
            break;

        case QMESH_GENERIC_POWER_LEVEL_CLIENT_LAST_STATUS:
            {
                int16_t power = 0;
                /* Optional Fields */
                
                power = READ_int16_t(msg+offset); /* power */
                offset += sizeof(int16_t);
                

                QCLI_LOGI (mesh_model_group, "\tReceived GENERIC_POWER_LAST_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tpower : 0x%x\n",power);
            }
            break;

        case QMESH_GENERIC_POWER_LEVEL_CLIENT_DEFAULT_STATUS:
            {
                int16_t power = 0;
                /* Optional Fields */
                
                power = READ_int16_t(msg+offset); /* power */
                offset += sizeof(int16_t);
                

                QCLI_LOGI (mesh_model_group, "\tReceived GENERIC_POWER_DEFAULT_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tpower : 0x%x\n",power);
            }
            break;

        case QMESH_GENERIC_POWER_LEVEL_CLIENT_RANGE_STATUS:
            {
                uint8_t status = 0;
                uint16_t rangeMin = 0;
                uint16_t rangeMax = 0;
                /* Optional Fields */
                
                status = READ_uint8_t(msg+offset); /* status */
                offset += sizeof(uint8_t);
                rangeMin = READ_uint16_t(msg+offset); /* rangeMin */
                offset += sizeof(uint16_t);
                rangeMax = READ_uint16_t(msg+offset); /* rangeMax */
                offset += sizeof(uint16_t);
                

                QCLI_LOGI (mesh_model_group, "\tReceived GENERIC_POWER_RANGE_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tstatus : 0x%x\n",status);
                QCLI_LOGI (mesh_model_group, "\trangeMin : 0x%x\n",rangeMin);
                QCLI_LOGI (mesh_model_group, "\trangeMax : 0x%x\n",rangeMax);
            }
            break;

        default:
            result = QMESH_RESULT_FAILURE;
            break;
    }

    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightLightnessClientHandler
 *
 *  DESCRIPTION
 *      This function handles Light Lightness client model events. 
 *
 *  RETURNS/MODIFIES
 *      QmeshResult
 *
 *----------------------------------------------------------------------------*/
QmeshResult QmeshLightLightnessClientHandler (const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *app_key_info,
                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                    QMESH_MODEL_DATA_T *model_data,
                                    const uint8_t *msg,
                                    uint16_t msg_len)
{
    uint32_t opcode = 0x00;
    uint16_t offset = 0;
    QmeshResult result = QMESH_RESULT_SUCCESS;

    if(!msg || msg_len<=0)
    {
        return QMESH_RESULT_FAILURE;
    }

    /* Retrieve the opcode from the message */
    opcode = getMessageOpcode(msg, &offset);

    if ((opcode == QMESH_LIGHT_LIGHTNESS_CLIENT_STATUS) ||
        (opcode == QMESH_LIGHT_LIGHTNESS_CLIENT_LINEAR_STATUS) ||
        (opcode == QMESH_LIGHT_LIGHTNESS_CLIENT_LAST_STATUS) ||
        (opcode == QMESH_LIGHT_LIGHTNESS_CLIENT_DEFAULT_STATUS) ||
        (opcode == QMESH_LIGHT_LIGHTNESS_CLIENT_RANGE_STATUS))
    {
        QCLI_LOGI(mesh_model_group,"%s\n", __FUNCTION__);
        QCLI_LOGI(mesh_model_group,"\tElm Addr : 0x%x\n", elem_config->element_data->unicast_addr);
        QCLI_LOGI(mesh_model_group,"\tKey Idx : 0x%x\n", app_key_info->key_info.key_idx);
        QCLI_LOGI(mesh_model_group,"\tSrc Addr : 0x%x\n", nw_hdr->src_addr);
    }

    switch(opcode)
    {
        case QMESH_LIGHT_LIGHTNESS_CLIENT_STATUS:
            {
                uint16_t presentLightness = 0;
                /* Optional Fields */
                uint16_t targetLightness = 0;
                uint8_t remainingTime = 0;
                
                presentLightness = READ_uint16_t(msg+offset); /* presentLightness */
                offset += sizeof(uint16_t);
                
                if (msg_len > offset)
                {
                    targetLightness = READ_uint16_t(msg+offset); /* targetLightness */
                    offset += sizeof(uint16_t);
                }
                if (msg_len > offset)
                {
                    remainingTime = READ_uint8_t(msg+offset); /* remainingTime */
                    offset += sizeof(uint8_t);
                }

                QCLI_LOGI (mesh_model_group, "\tReceived LIGHT_LIGHTNESS_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tpresentLightness : 0x%x\n",presentLightness);
                QCLI_LOGI (mesh_model_group, "\ttargetLightness : 0x%x\n",targetLightness);
                QCLI_LOGI (mesh_model_group, "\tremainingTime : 0x%x\n",remainingTime);
            }
            break;

        case QMESH_LIGHT_LIGHTNESS_CLIENT_LINEAR_STATUS:
            {
                uint16_t presentLightness = 0;
                /* Optional Fields */
                uint16_t targetLightness = 0;
                uint8_t remainingTime = 0;
                
                presentLightness = READ_uint16_t(msg+offset); /* presentLightness */
                offset += sizeof(uint16_t);
                
                if (msg_len > offset)
                {
                    targetLightness = READ_uint16_t(msg+offset); /* targetLightness */
                    offset += sizeof(uint16_t);
                }
                if (msg_len > offset)
                {
                    remainingTime = READ_uint8_t(msg+offset); /* remainingTime */
                    offset += sizeof(uint8_t);
                }

                QCLI_LOGI (mesh_model_group, "\tReceived LIGHT_LIGHTNESS_LINEAR_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tpresentLightness : 0x%x\n",presentLightness);
                QCLI_LOGI (mesh_model_group, "\ttargetLightness : 0x%x\n",targetLightness);
                QCLI_LOGI (mesh_model_group, "\tremainingTime : 0x%x\n",remainingTime);
            }
            break;

        case QMESH_LIGHT_LIGHTNESS_CLIENT_LAST_STATUS:
            {
                uint16_t lightness = 0;
                /* Optional Fields */
                
                lightness = READ_uint16_t(msg+offset); /* lightness */
                offset += sizeof(uint16_t);
                

                QCLI_LOGI (mesh_model_group, "\tReceived LIGHT_LIGHTNESS_LAST_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tlightness : 0x%x\n",lightness);
            }
            break;

        case QMESH_LIGHT_LIGHTNESS_CLIENT_DEFAULT_STATUS:
            {
                uint16_t lightness = 0;
                /* Optional Fields */
                
                lightness = READ_uint16_t(msg+offset); /* lightness */
                offset += sizeof(uint16_t);
                

                QCLI_LOGI (mesh_model_group, "\tReceived LIGHT_LIGHTNESS_DEFAULT_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tlightness : 0x%x\n",lightness);
            }
            break;

        case QMESH_LIGHT_LIGHTNESS_CLIENT_RANGE_STATUS:
            {
                uint8_t statusCode = 0;
                uint16_t rangeMin = 0;
                uint16_t rangeMax = 0;
                /* Optional Fields */
                
                statusCode = READ_uint8_t(msg+offset); /* statusCode */
                offset += sizeof(uint8_t);
                rangeMin = READ_uint16_t(msg+offset); /* rangeMin */
                offset += sizeof(uint16_t);
                rangeMax = READ_uint16_t(msg+offset); /* rangeMax */
                offset += sizeof(uint16_t);
                

                QCLI_LOGI (mesh_model_group, "\tReceived LIGHT_LIGHTNESS_RANGE_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tstatusCode : 0x%x\n",statusCode);
                QCLI_LOGI (mesh_model_group, "\trangeMin : 0x%x\n",rangeMin);
                QCLI_LOGI (mesh_model_group, "\trangeMax : 0x%x\n",rangeMax);
            }
            break;

        default:
            result = QMESH_RESULT_FAILURE;
            break;
    }

    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshLightHslClientHandler
 *
 *  DESCRIPTION
 *      This function handles Light HSL client model events. 
 *
 *  RETURNS/MODIFIES
 *      QmeshResult
 *
 *----------------------------------------------------------------------------*/
QmeshResult QmeshLightHslClientHandler (const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *app_key_info,
                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                    QMESH_MODEL_DATA_T *model_data,
                                    const uint8_t *msg,
                                    uint16_t msg_len)
{
    uint32_t opcode = 0x00;
    uint16_t offset = 0;
    QmeshResult result = QMESH_RESULT_SUCCESS;

    if(!msg || msg_len<=0)
    {
        return QMESH_RESULT_FAILURE;
    }

    /* Retrieve the opcode from the message */
    opcode = getMessageOpcode(msg, &offset);

    if ((opcode == QMESH_LIGHT_HSL_CLIENT_STATUS) ||
        (opcode == QMESH_LIGHT_HSL_CLIENT_HUE_STATUS) ||
        (opcode == QMESH_LIGHT_HSL_CLIENT_SATURATION_STATUS) ||
        (opcode == QMESH_LIGHT_HSL_CLIENT_TARGET_STATUS) ||
        (opcode == QMESH_LIGHT_HSL_CLIENT_DEFAULT_STATUS) ||
        (opcode == QMESH_LIGHT_HSL_CLIENT_RANGE_STATUS))
    {
        QCLI_LOGI(mesh_model_group,"%s\n", __FUNCTION__);
        QCLI_LOGI(mesh_model_group,"\tElm Addr : 0x%x\n", elem_config->element_data->unicast_addr);
        QCLI_LOGI(mesh_model_group,"\tKey Idx : 0x%x\n", app_key_info->key_info.key_idx);
        QCLI_LOGI(mesh_model_group,"\tSrc Addr : 0x%x\n", nw_hdr->src_addr);
    }

    switch(opcode)
    {
        case QMESH_LIGHT_HSL_CLIENT_STATUS:
            {
                uint16_t hSLLightness = 0;
                uint16_t hSLHue = 0;
                uint16_t hSLSaturation = 0;
                /* Optional Fields */
                uint8_t remainingTime = 0;
                
                hSLLightness = READ_uint16_t(msg+offset); /* hSLLightness */
                offset += sizeof(uint16_t);
                hSLHue = READ_uint16_t(msg+offset); /* hSLHue */
                offset += sizeof(uint16_t);
                hSLSaturation = READ_uint16_t(msg+offset); /* hSLSaturation */
                offset += sizeof(uint16_t);
                
                if (msg_len > offset)
                {
                    remainingTime = READ_uint8_t(msg+offset); /* remainingTime */
                    offset += sizeof(uint8_t);
                }

                QCLI_LOGI (mesh_model_group, "\tReceived LIGHT_HSL_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\thSLLightness : 0x%x\n",hSLLightness);
                QCLI_LOGI (mesh_model_group, "\thSLHue : 0x%x\n",hSLHue);
                QCLI_LOGI (mesh_model_group, "\thSLSaturation : 0x%x\n",hSLSaturation);
                QCLI_LOGI (mesh_model_group, "\tremainingTime : 0x%x\n",remainingTime);
            }
            break;

        case QMESH_LIGHT_HSL_CLIENT_HUE_STATUS:
            {
                uint16_t presentHue = 0;
                /* Optional Fields */
                uint16_t targetHue = 0;
                uint8_t remainingTime = 0;
                
                presentHue = READ_uint16_t(msg+offset); /* presentHue */
                offset += sizeof(uint16_t);
                
                if (msg_len > offset)
                {
                    targetHue = READ_uint16_t(msg+offset); /* targetHue */
                    offset += sizeof(uint16_t);
                }
                if (msg_len > offset)
                {
                    remainingTime = READ_uint8_t(msg+offset); /* remainingTime */
                    offset += sizeof(uint8_t);
                }

                QCLI_LOGI (mesh_model_group, "\tReceived LIGHT_HSL_HUE_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tpresentHue : 0x%x\n",presentHue);
                QCLI_LOGI (mesh_model_group, "\ttargetHue : 0x%x\n",targetHue);
                QCLI_LOGI (mesh_model_group, "\tremainingTime : 0x%x\n",remainingTime);
            }
            break;

        case QMESH_LIGHT_HSL_CLIENT_SATURATION_STATUS:
            {
                uint16_t presentSaturation = 0;
                /* Optional Fields */
                uint16_t targetSaturation = 0;
                uint8_t remainingTime = 0;
                
                presentSaturation = READ_uint16_t(msg+offset); /* presentSaturation */
                offset += sizeof(uint16_t);
                
                if (msg_len > offset)
                {
                    targetSaturation = READ_uint16_t(msg+offset); /* targetSaturation */
                    offset += sizeof(uint16_t);
                }
                if (msg_len > offset)
                {
                    remainingTime = READ_uint8_t(msg+offset); /* remainingTime */
                    offset += sizeof(uint8_t);
                }

                QCLI_LOGI (mesh_model_group, "\tReceived LIGHT_HSL_SATURATION_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tpresentSaturation : 0x%x\n",presentSaturation);
                QCLI_LOGI (mesh_model_group, "\ttargetSaturation : 0x%x\n",targetSaturation);
                QCLI_LOGI (mesh_model_group, "\tremainingTime : 0x%x\n",remainingTime);
            }
            break;

        case QMESH_LIGHT_HSL_CLIENT_TARGET_STATUS:
            {
                uint16_t hSLLightnessTarget = 0;
                uint16_t hSLHueTarget = 0;
                uint16_t hSLSaturationTarget = 0;
                /* Optional Fields */
                uint8_t remainingTime = 0;
                
                hSLLightnessTarget = READ_uint16_t(msg+offset); /* hSLLightnessTarget */
                offset += sizeof(uint16_t);
                hSLHueTarget = READ_uint16_t(msg+offset); /* hSLHueTarget */
                offset += sizeof(uint16_t);
                hSLSaturationTarget = READ_uint16_t(msg+offset); /* hSLSaturationTarget */
                offset += sizeof(uint16_t);
                
                if (msg_len > offset)
                {
                    remainingTime = READ_uint8_t(msg+offset); /* remainingTime */
                    offset += sizeof(uint8_t);
                }

                QCLI_LOGI (mesh_model_group, "\tReceived LIGHT_HSL_TARGET_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\thSLLightnessTarget : 0x%x\n",hSLLightnessTarget);
                QCLI_LOGI (mesh_model_group, "\thSLHueTarget : 0x%x\n",hSLHueTarget);
                QCLI_LOGI (mesh_model_group, "\thSLSaturationTarget : 0x%x\n",hSLSaturationTarget);
                QCLI_LOGI (mesh_model_group, "\tremainingTime : 0x%x\n",remainingTime);
            }
            break;

        case QMESH_LIGHT_HSL_CLIENT_DEFAULT_STATUS:
            {
                uint16_t lightness = 0;
                uint16_t hue = 0;
                uint16_t saturation = 0;
                /* Optional Fields */
                
                lightness = READ_uint16_t(msg+offset); /* lightness */
                offset += sizeof(uint16_t);
                hue = READ_uint16_t(msg+offset); /* hue */
                offset += sizeof(uint16_t);
                saturation = READ_uint16_t(msg+offset); /* saturation */
                offset += sizeof(uint16_t);
                

                QCLI_LOGI (mesh_model_group, "\tReceived LIGHT_HSL_DEFAULT_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tlightness : 0x%x\n",lightness);
                QCLI_LOGI (mesh_model_group, "\thue : 0x%x\n",hue);
                QCLI_LOGI (mesh_model_group, "\tsaturation : 0x%x\n",saturation);
            }
            break;

        case QMESH_LIGHT_HSL_CLIENT_RANGE_STATUS:
            {
                uint8_t statusCode = 0;
                uint16_t hueRangeMin = 0;
                uint16_t hueRangeMax = 0;
                uint16_t saturationRangeMin = 0;
                uint16_t saturationRangeMax = 0;
                /* Optional Fields */
                
                statusCode = READ_uint8_t(msg+offset); /* statusCode */
                offset += sizeof(uint8_t);
                hueRangeMin = READ_uint16_t(msg+offset); /* hueRangeMin */
                offset += sizeof(uint16_t);
                hueRangeMax = READ_uint16_t(msg+offset); /* hueRangeMax */
                offset += sizeof(uint16_t);
                saturationRangeMin = READ_uint16_t(msg+offset); /* saturationRangeMin */
                offset += sizeof(uint16_t);
                saturationRangeMax = READ_uint16_t(msg+offset); /* saturationRangeMax */
                offset += sizeof(uint16_t);
                

                QCLI_LOGI (mesh_model_group, "\tReceived LIGHT_HSL_RANGE_STATUS Event\n");
                QCLI_LOGI (mesh_model_group, "\tstatusCode : 0x%x\n",statusCode);
                QCLI_LOGI (mesh_model_group, "\thueRangeMin : 0x%x\n",hueRangeMin);
                QCLI_LOGI (mesh_model_group, "\thueRangeMax : 0x%x\n",hueRangeMax);
                QCLI_LOGI (mesh_model_group, "\tsaturationRangeMin : 0x%x\n",saturationRangeMin);
                QCLI_LOGI (mesh_model_group, "\tsaturationRangeMax : 0x%x\n",saturationRangeMax);
            }
            break;

        default:
            result = QMESH_RESULT_FAILURE;
            break;
    }

    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshVendorClientHandler
 *
 *  DESCRIPTION
 *      This function handles Vendor client model events. 
 *
 *  RETURNS/MODIFIES
 *      QmeshResult
 *
 *----------------------------------------------------------------------------*/
QmeshResult QmeshVendorClientHandler (const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *app_key_info,
                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                    QMESH_MODEL_DATA_T *model_data,
                                    const uint8_t *msg,
                                    uint16_t msg_len)
{
    uint32_t opcode = 0x00;
    uint16_t offset = 0;
    QmeshResult result = QMESH_RESULT_SUCCESS;

    if(!msg || (msg_len < QMESH_OPCODE_FORMAT_THREE_BYTE))
    {
        return QMESH_RESULT_FAILURE;
    }

    /* Retrieve the opcode from the message */
    opcode = getMessageOpcode(msg, &offset);
#ifdef ENABLE_THROUGHPUT_TESTING
    count++;
    QCLI_LOGI(mesh_model_group,"\n%s:Opcode=0x%06x, src=0x%x, seq_num=0x%06x, count=%d\n", 
                                __FUNCTION__, opcode, nw_hdr->src_addr, nw_hdr->seq_num, count);
#endif /* ENABLE_THROUGHPUT_TESTING */

    switch(opcode)
    {
        case QMESH_VENDOR_MODEL_OPCODE_STATUS_1:
        {
            QCLI_LOGI (mesh_model_group, "\tReceived QMESH_VENDOR_MODEL_OPCODE_STATUS_1 Event\n");
        }
        break;
    }

    return result;
}

