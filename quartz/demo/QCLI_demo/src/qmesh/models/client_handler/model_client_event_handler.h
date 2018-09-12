/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/

#ifndef __MODEL_CLIENT_EVENT_HANDLER_H__
#define __MODEL_CLIENT_EVENT_HANDLER_H__

#include "model_client_common.h"
#include "qmesh_types.h"

/** \addtogroup model_client_event_handler
 * @{
 * \JAVADOC_AUTOBRIEF 
 * \file model_client_event_handler.h
 * \brief   
 *
 * \details 
 */

/* Model Opcode offset in message */
#define MODEL_OPCODE_OFFSET                               (0UL)

/* One byte opcode messages mask */
#define QMESH_OPCODE_FORMAT_ONE_BYTE                      (0x00)
/* Two bytes opcode messages mask */
#define QMESH_OPCODE_FORMAT_TWO_BYTE                      (0x02)
/* Three bytes opcode messages mask */
#define QMESH_OPCODE_FORMAT_THREE_BYTE                    (0x03)

#define get_opcode_format(opcode)                         (( (opcode) >> 6 ) & 3 )

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
                                    uint16_t msg_len);

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
                                    uint16_t msg_len);

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
                                    uint16_t msg_len);

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
extern QmeshResult QmeshGenericDefaultTransitionTimeClientHandler (const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *app_key_info,
                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                    QMESH_MODEL_DATA_T *model_data,
                                    const uint8_t *msg,
                                    uint16_t msg_len);

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
extern QmeshResult QmeshGenericPowerLevelClientHandler (const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *app_key_info,
                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                    QMESH_MODEL_DATA_T *model_data,
                                    const uint8_t *msg,
                                    uint16_t msg_len);

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
extern QmeshResult QmeshLightLightnessClientHandler (const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *app_key_info,
                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                    QMESH_MODEL_DATA_T *model_data,
                                    const uint8_t *msg,
                                    uint16_t msg_len);

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
extern QmeshResult QmeshLightHslClientHandler (const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *app_key_info,
                                    const QMESH_ELEMENT_CONFIG_T *elem_config,
                                    QMESH_MODEL_DATA_T *model_data,
                                    const uint8_t *msg,
                                    uint16_t msg_len);
                                    
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
                                    uint16_t msg_len);
/*!@} */
#endif /* __MODEL_CLIENT_EVENT_HANDLER_H__ */

