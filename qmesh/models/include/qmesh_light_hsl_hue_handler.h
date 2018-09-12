/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/******************************************************************************/
/*! \file qmesh_light_hsl_hue_handler.h
 *  \brief defines and functions for SIGmesh Light HSL Hue Model
 *
 *   This file contains data types and APIs exposed by the Light HSL Hue
 *
 */
/******************************************************************************/
#ifndef __QMESH_LIGHT_HSL_HUE_HANDLER_H__
#define __QMESH_LIGHT_HSL_HUE_HANDLER_H__

/*! \addtogroup Model_Lightness_HSL_Hue_Handler
 * @{
 */

#include "qmesh_hal_ifce.h"
#include "qmesh_types.h"
#include "qmesh_cache_mgmt.h"
#include "qmesh_model_common.h"

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/*!\brief  Lightness Status Report Size */
#define LIGHT_HSL_HUE_TRANSITION_MSG_SIZE       (5)
#define LIGHT_HSL_HUE_NON_TRANSITION_MSG_SIZE   (3)
#define LIGHT_HSL_HUE_TRANSITION_STATUS_SIZE    (7)
#define LIGHT_HSL_HUE_STATUS_REPORT_SIZE        (4)

/*!\brief  Hue Message offsets */
#define OFFSET_VALUE_BYTE_LSB                   (0)
#define OFFSET_VALUE_BYTE_MSB                   (1)
#define OFFSET_HUE_TID                          (2)
#define OFFSET_HUE_TIME                         (3)
#define OFFSET_HUE_DELAY                        (4)

/*!\brief  Transition State Information */
typedef struct
{
    uint16_t          transition_state;         /*!< Transition State Information */
    uint16_t          progress_time;            /*!< Time duration for each step  */
    uint32_t          transition_duration;      /*!< Total transition duration    */
    int16_t           linear_value;             /*!< Value to add for each step   */
    uint16_t          target_value;             /*!< Target value of the state    */
    uint16_t          no_of_steps;              /*!< Total Number of steps        */
    uint16_t          opcode;                   /*!< Model Opcode                 */
    uint16_t          src_addr;                 /*!< Element address              */
    uint8_t           step_resolution;          /*!< Source address               */
    void*           state_data;                 /*!< Step resolution from client  */
    QMESH_TIMER_HANDLE_T  timer_handle;         /*!< Current context information  */
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T key_info;   /*!< Transition timer handler     */
}QMESH_HUE_TRANSITION_INFO;                     /*!< Transport key information    */

/*!\brief  Light HSL Hue Server Context Information */
typedef struct
{
    uint16_t elm_id;                            /*!< Element Address              */
    uint16_t light_hsl_hue;                     /*!< Light HSL Hue                */
    uint32_t    last_msg_seq_no;                /*!< Last processed Seq Number    */
    uint16_t light_target_hue;                  /*!< Light target hue             */  
    uint16_t    last_src_addr;                  /*!< Last Src address processed   */
    QMESH_HUE_TRANSITION_INFO hue_transition;   /*!< Transition Information       */
    QMESH_MUTEX_T mutex_handle;                 /*!< Mutex for sync access        */
    QMESH_TIMER_HANDLE_T publish_timer;         /*!< Publish Timer                */
    QMESH_TIMER_HANDLE_T publish_retrans_timer; /*!< Timer to restransmit to Publish addresses */
    uint8_t       publish_restrans_cnt;         /*!< Publication retransmission count */
    uint32_t      publish_interval;             /*!< Publication interval */
    QMESH_MODEL_PUBLISH_STATE_T  *publish_state;/*!< Pointer to the Model Publication State */
    void *hsl_context;                          /*!< Pointer to the HSL Context */
}QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T;

/*!\brief Light HUE NVM state */
typedef struct __attribute__ ((__packed__))
{
    uint16_t light_hsl_hue;                      /*!<  Light HSL Hue                */
    uint32_t last_msg_seq_no;                    /*!<  Last processed Seq Number    */   
    uint16_t last_src_addr;                      /*!<  Last Src address processed   */ 
    uint16_t light_target_hue;                   /*!<  Light target hue   */  
} QMESH_MODEL_LIGHT_HUE_NVM_STATE_T;

/*----------------------------------------------------------------------------*
 * QmeshLightHSLHueServerHandler
 */
/*! \brief Light HSL Hue Event Handler
 *
 *  \param [in] nw_hdr          Network header \ref QMESH_NW_HEADER_INFO_T
 *  \param [in] key_info        Pointer to Access layer key \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T
 *  \param [in] elem_data       Pointer to element data \ref QMESH_ELEMENT_CONFIG_T
 *  \param [in] model_data      Pointer to Model context \ref QMESH_MODEL_DATA_T
 *  \param [in] msg             Pointer to Message Data
 *  \param [in] msg_len         Message Data Length
 *
 *  \return \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLHueServerHandler(const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
                                                    const QMESH_ELEMENT_CONFIG_T *elem_data,
                                                    QMESH_MODEL_DATA_T *model_data,
                                                    const uint8_t *msg,
                                                    uint16_t msg_len);

/*----------------------------------------------------------------------------*
 * QmeshLightHSLHueServerAppInit
 */
/*! \brief Initializes the Light HSL Hue Server context information
 *
 *  \param [in] context  Pointer to Hue Model Context \ref QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T
 *
 *  \return \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLHueServerAppInit(QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T *context);

/*----------------------------------------------------------------------------*
 * QmeshLightHSLHueServerAppDeInit
 */
/*! \brief DeInitializes the Light HSL Hue server context information
 *
 *  \param [in] context  Pointer to Hue Model Context \ref QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T
 *
 *  \return None
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightHSLHueServerAppDeInit(QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T *context);

/*----------------------------------------------------------------------------*
 * QmeshLightHSLHueAbortTransition
 */
/*! \brief Helper function to abort Light HSL Hue transition (for bound state operations)
 *
 *  \param [in] context  Pointer to Hue Model Context \ref QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightHSLHueAbortTransition(QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T* context);

/*----------------------------------------------------------------------------*
 * QmeshLightHslHueGet
 */
/*! \brief Helper function to get Light HSL Hue value
 *
 *  \param [in] context  Pointer to Hue Model Context \ref QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T
 *
 *  \return HSL Hue Value
 */
/*----------------------------------------------------------------------------*/
extern uint16_t QmeshLightHslHueGet(QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T* context);

/*----------------------------------------------------------------------------*
 * QmeshLightHslHueSet
 */
/*! \brief Helper function to set Light HSL Hue value
 *
 *  \param [in] context  Pointer to Hue Model Context \ref QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T
 *  \param [in] transition_time  transition time uint8_t
 *  \param [in] delay  delay uint8_t
 *  \param [in] hsl_hue HSL hue value uint16_t
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightHslHueSet(QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T* context,
                                uint8_t transition_time,
                                uint8_t delay,
                                uint16_t hsl_hue);

/*----------------------------------------------------------------------------*
 * QmeshLightHslHueUpdate
 */
/*! \brief Helper function to Update Light HSL Hue value
 *
 *  \param [in] context     Pointer to Hue Model Context \ref QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T
 *  \param [in] hsl_hue     HSL hue value uint16_t
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightHslHueUpdate(QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T* context,
                                   uint16_t hsl_hue);

/*----------------------------------------------------------------------------*
 * QmeshLightHslHueUpdatePublicationPeriod
 */
/*! \brief Helper function to update publication period
 *
 *
 *  \param [in] model_data  Pointer to model data \ref QMESH_MODEL_DATA_T
 *
 *  \return bool
 */
/*----------------------------------------------------------------------------*/
bool QmeshLightHslHueUpdatePublicationPeriod(QMESH_MODEL_DATA_T *model_data);

/*----------------------------------------------------------------------------*
 * QmeshLightHslHuePublicationHandler
 */
/*! \brief Publication configuration handler for Light HSL Hue model
 *
 *  \param [in] model_data  Pointer to model data \ref QMESH_MODEL_DATA_T
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightHslHuePublicationHandler(QMESH_MODEL_DATA_T *model_data);

/*----------------------------------------------------------------------------*
 * QmeshLightHslHueStatusPublish
 */
/*! \brief This function sends the current Light HSL Hue status message to the Publish
*          address assigned for this model.
 *
 *  \param [in] model_context  Pointer to model context \ref QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightHslHueStatusPublish(QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T *model_context);
/*!@} */
#endif /* __QMESH_LIGHT_HSL_HANDLER_H__ */