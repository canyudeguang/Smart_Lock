/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/******************************************************************************/
/*! \file qmesh_light_hsl_saturation_handler.h
 *  \brief defines and functions for SIGmesh Light HSL Saturation Handler
 *
 *   This file contains data types and APIs exposed by the Light HSL Saturation
 *   Handler
 */
/******************************************************************************/
#ifndef __QMESH_LIGHT_HSL_SATURATION_HANDLER_H__
#define __QMESH_LIGHT_HSL_SATURATION_HANDLER_H__

/*! \addtogroup Model_Lightness_HSL_Saturation_Handler
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
#define LIGHT_HSL_SAT_TRANSITION_MSG_SIZE       (5)
#define LIGHT_HSL_SAT_NON_TRANSITION_MSG_SIZE   (3)
#define LIGHT_HSL_SAT_TRANSITION_STATUS_SIZE    (7)
#define LIGHT_HSL_SAT_STATUS_REPORT_SIZE        (4)

/*!\brief  Hue Message offsets */
#define OFFSET_VALUE_BYTE_LSB                   (0)
#define OFFSET_VALUE_BYTE_MSB                   (1)
#define OFFSET_SAT_TID                          (2)
#define OFFSET_SAT_TIME                         (3)
#define OFFSET_SAT_DELAY                        (4)

/*!\brief  Transition State Information */
typedef struct
{
    uint16_t          transition_state;         /*!< Transition State Information */
    uint32_t          transition_duration;      /*!< Total transition duration    */
    uint16_t          progress_time;            /*!< Time duration for each step  */
    int16_t           linear_value;             /*!< Value to add for each step   */
    uint16_t          target_value;             /*!< Target value of the state    */
    uint16_t          no_of_steps;              /*!< Total Number of steps        */
    uint16_t          opcode;                   /*!< Model Opcode                 */
    uint16_t          elm_id;                   /*!< Element address              */
    uint16_t          src_addr;                 /*!< Source address               */
    uint8_t           step_resolution;          /*!< Step resolution from client  */
    void*           state_data;                 /*!< Current context information  */
    QMESH_TIMER_HANDLE_T  timer_handle;         /*!< Transition timer handler     */
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T key_info;   /*!< Transport key information    */
}QMESH_SAT_TRANSITION_INFO;

/*!\brief  Light HSL Server Context Information */
typedef struct
{
    uint16_t elm_id;                             /*!< Element Address              */
    uint16_t light_hsl_sat;                      /*!< Light HSL Saturation State   */
    uint32_t    last_msg_seq_no;                 /*!< Last processed Seq Number    */
    uint16_t    last_src_addr;                   /*!< Last Src address processed   */
    uint16_t    light_target_sat;                /*!< Light target sat             */
    QMESH_SAT_TRANSITION_INFO sat_transition;    /*!< Transition Information       */
    QMESH_MUTEX_T mutex_handle;                  /*!< Mutex for sync access        */
    QMESH_TIMER_HANDLE_T publish_timer;          /*!< Publish Timer                */
    QMESH_TIMER_HANDLE_T publish_retrans_timer;  /*!< Timer to restransmit to Publish addresses */
    uint8_t       publish_restrans_cnt;          /*!< Publication retransmission count */
    uint32_t      publish_interval;              /*!< Publication interval */
    QMESH_MODEL_PUBLISH_STATE_T  *publish_state; /*!< Pointer to the Model Publication State */
    void                         *hsl_context;   /*!< Pointer to the HSL Context */
}QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T;

/*!\brief Light Sat NVM state */
typedef struct __attribute__ ((__packed__))
{
  uint16_t light_hsl_sat;                      /*!<  Light HSL Sat                */
  uint32_t last_msg_seq_no;                    /*!<  Last processed Seq Number    */   
  uint16_t last_src_addr;                      /*!<  Last Src address processed   */ 
  uint16_t    light_target_sat;                /*!< Light target sat             */
} QMESH_MODEL_LIGHT_SAT_NVM_STATE_T;

/*----------------------------------------------------------------------------*
 * QmeshLightHSLSaturationServerHandler
 */
/*! \brief Light HSL Saturation Event Handler
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
extern QMESH_RESULT_T QmeshLightHSLSaturationServerHandler(const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                                    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
                                                    const QMESH_ELEMENT_CONFIG_T *elem_data,
                                                    QMESH_MODEL_DATA_T *model_data,
                                                    const uint8_t *msg,
                                                    uint16_t msg_len);

/*----------------------------------------------------------------------------*
 * QmeshLightHSLSatServerAppInit
 */
/*! \brief Initializes the Light HSL Saturation Server
 *
 *  \param [in] context         Model Context information
 *
 *  \return \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLSatServerAppInit(QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *context);

/*----------------------------------------------------------------------------*
 * QmeshLightHSLSatServerAppDeInit
 */
/*! \brief This function de-initializes Light HSL Saturation Server
 *
 *  \param [in] context         Model Context information
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightHSLSatServerAppDeInit(QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *context);

/*----------------------------------------------------------------------------*
 * QmeshLightHSLSatAbortTransition
 */
/*! \brief Helper function to abort Light HSL Saturation transition (for bound state operations)
 *
 *  \param [in] context         Model Context information
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightHSLSatAbortTransition(QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *context);

/*----------------------------------------------------------------------------*
 * QmeshLightHslSatGet
 */
/*! \brief Helper function to get Light HSL Sat value
 *
 *  \param [in] context         Model Context information
 *
 *  \return HSL Sat Value
 */
/*----------------------------------------------------------------------------*/
extern uint16_t QmeshLightHslSatGet(QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *context);

/*----------------------------------------------------------------------------*
 * QmeshLightHslSatSet
 */
/*! \brief Helper function to set Light HSL Sat value
 *
 *  \param [in] context             Model Context information
 *  \param [in] transition_time     Transition time
 *  \param [in] delay               Delayed execution time
 *  \param [in] hsl_sat             HSL saturation value
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightHslSatSet(QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T* context,
                                uint8_t transition_time,
                                uint8_t delay,
                                uint16_t hsl_sat);

/*----------------------------------------------------------------------------*
 * QmeshLightHslSatUpdate
 */
/*! \brief Helper function to Update Light HSL Sat value
 *
 *  \param [in] context             Model Context information
 *  \param [in] hsl_sat             HSL saturation value
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightHslSatUpdate(QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T* context,
                                   uint16_t hsl_sat);

/*----------------------------------------------------------------------------*
 * QmeshLightHslSatUpdatePublicationPeriod
 */
/*! \brief Publication configuration handler for Light HSL Saturation Server.
 *
 *  \param [in] model_data          Model Context information
 *
 *  \return bool
 */
/*----------------------------------------------------------------------------*/
bool QmeshLightHslSatUpdatePublicationPeriod(QMESH_MODEL_DATA_T *model_data);

/*----------------------------------------------------------------------------*
 * QmeshLightHslSatPublicationHandler
 */
/*! \brief Publication configuration handler for Light HSL Saturation model
 *
 *  \param [in] model_data          Model Context information
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightHslSatPublicationHandler(QMESH_MODEL_DATA_T *model_data);

/*----------------------------------------------------------------------------*
 * QmeshLightHslSatStatusPublish
 */
/*! \brief This function sends the current Light HSL Sat status message to the
 *      Publish address assigned for this model.
 *
 *  \param [in] model_context       Model Context information
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightHslSatStatusPublish(QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T *model_context);
/*!@} */
#endif /* __QMESH_LIGHT_HSL_SATURATION_HANDLER_H__ */