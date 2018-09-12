/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/******************************************************************************/
/*! \file qmesh_light_hsl_handler.h
 *  \brief defines and functions for SIGmesh Light HSL Handler
 *
 *   This file contains data types and APIs exposed by the Light HSL Server
 *
 */
/******************************************************************************/
#ifndef __QMESH_LIGHT_HSL_HANDLER_H__
#define __QMESH_LIGHT_HSL_HANDLER_H__

/*! \addtogroup Model_Lightness_HSL_Handler
 * @{
 */

#include "qmesh_hal_ifce.h"
#include "qmesh_types.h"
#include "qmesh_cache_mgmt.h"
#include "qmesh_model_common.h"
#include "qmesh_light_hsl_hue_handler.h"
#include "qmesh_light_hsl_saturation_handler.h"

/*============================================================================*
 *  Public Definitions
 *============================================================================*/

/*!\brief  Lightness Status Report Size */
#define LIGHT_HSL_RANGE_STATUS_SIZE             (11)
#define LIGHT_HSL_TRANSITION_MSG_SIZE           (9)
#define LIGHT_HSL_NON_TRANSITION_MSG_SIZE       (7)
#define LIGHT_HSL_TRANSITION_STATUS_SIZE        (9)
#define LIGHT_HSL_STATUS_REPORT_SIZE            (8)
#define LIGHT_HSL_DEFAULT_STATUS_SIZE           (8)

/*!\brief  Lightness Message offsets */
#define OFFSET_VALUE_BYTE_LSB                   (0)
#define OFFSET_VALUE_BYTE_MSB                   (1)
#define OFFSET_HSL_TID                          (6)
#define OFFSET_HSL_TIME                         (7)
#define OFFSET_HSL_DELAY                        (8)

/*!\brief  Lightness Range Error defines */
#define LIGHTHSL_RANGE_SET_SUCCESS             (0)
#define LIGHTHSL_CANNOT_SET_RANGE_MIN          (1)
#define LIGHTHSL_CANNOT_SET_RANGE_MAX          (2)

/*!\brief  Lightness Max Value for Actual/Linear */
#define LIGHTNESS_MAX                           (65535)

/*!\brief  Transition State Information */
typedef struct
{
  uint16_t  transition_state;               /*!< Transition State Information */
  uint16_t  progress_time;                  /*!<  Time duration for each step  */
  uint32_t  transition_duration;            /*!<  Total transition duration    */
  int16_t   linear_value[3];                /*!<  Value to add for each step   */
  uint16_t  target_value[3];                /*!<  Target value of the state    */
  uint16_t  no_of_steps;                    /*!<  Total Number of steps        */
  uint16_t  opcode;                         /*!<  Model Opcode                 */
  uint16_t  elm_id;                         /*!<  Element address              */
  uint16_t  src_addr;                       /*!<  Source address               */
  uint8_t   step_resolution;                /*!<  Step resolution from client  */
  void*   state_data;                       /*!<  Current context information  */
  QMESH_TIMER_HANDLE_T  timer_handle;       /*!<  Transition timer handler     */
  QMESH_ACCESS_PAYLOAD_KEY_INFO_T key_info; /*!<  Transport key information    */
}QMESH_HSL_SERVER_TRANSITION_INFO;

/*!\brief  Light HSL Server Context Information */
typedef struct
{
  uint16_t elm_id;                              /*!<  Element address              */
  uint16_t light_hsl_lightness;                 /*!<  Light HSL Lightness State    */
  uint16_t light_hsl_hue_def;                   /*!<  Light HSL Hue Default state  */
  uint16_t light_hsl_hue_min;                   /*!<  Light HSL Hue Range-Min State*/
  uint16_t light_hsl_hue_max;                   /*!<  Light HSL Hue Range-Max State*/
  uint16_t light_hsl_sat_def;                   /*!<  Light HSL Sat Default State  */
  uint16_t light_hsl_sat_min;                   /*!<  Light HSL Sat Range-Min State*/
  uint16_t light_hsl_sat_max;                   /*!<  Light HSL Sat Range-Max State*/
  uint8_t  light_range_status;                  /*!<  Light HSL Hue Range Status   */
  uint16_t light_target_hsl;                    /*!<  Light target HSL   */
  uint16_t light_target_sat;                   /*!<   Light target Sat   */ 
  uint16_t light_target_hue;                   /*!<   Light target Hue   */ 
  uint32_t    last_msg_seq_no;                  /*!<  Last processed Seq Number    */
  uint16_t    last_src_addr;                    /*!<  Last Src address processed   */
  QMESH_LIGHT_HSL_HUE_MODEL_CONTEXT_T* hue_context;/*!<  Light Hue Context Info */
  QMESH_LIGHT_HSL_SAT_MODEL_CONTEXT_T* sat_context;/*!<  Light Sat Context Info */
  QMESH_HSL_SERVER_TRANSITION_INFO hsl_transition; /*!<  Transition Information */
  QMESH_MUTEX_T mutex_handle;                   /*!<  Mutex for sync access        */

  QMESH_TIMER_HANDLE_T publish_timer;           /*!<  Publish Timer                */
  QMESH_TIMER_HANDLE_T publish_retrans_timer;   /*!<  Timer to restransmit to Publish addresses */
  uint8_t       publish_restrans_cnt;           /*!<  Publication retransmission count */
  uint32_t      publish_interval;               /*!<  Publication interval */
  QMESH_MODEL_PUBLISH_STATE_T  *publish_state;  /*!<  Pointer to the Model Publication State */
}QMESH_LIGHT_HSL_MODEL_CONTEXT_T;

/*!\brief Light HSL NVM state */
typedef struct __attribute__ ((__packed__))
{
  uint16_t light_hsl_lightness;                /*!<  Light HSL Lightness State    */  
  uint16_t light_hsl_hue_def;                  /*!<  Light HSL Hue Default state  */ 
  uint16_t light_hsl_hue_min;                  /*!<  Light HSL Hue Range-Min State*/ 
  uint16_t light_hsl_hue_max;                  /*!<  Light HSL Hue Range-Max State*/   
  uint16_t light_hsl_sat_def;                  /*!<  Light HSL Sat Default State  */ 
  uint16_t light_hsl_sat_min;                  /*!<  Light HSL Sat Range-Min State*/ 
  uint16_t light_hsl_sat_max;                  /*!<  Light HSL Sat Range-Max State*/ 
  uint32_t last_msg_seq_no;                    /*!<  Last processed Seq Number    */   
  uint16_t last_src_addr;                      /*!<  Last Src address processed   */ 
  uint16_t light_target_hsl;                   /*!<  Target HSL State   */ 
  uint16_t light_target_hue;                   /*!<  Target Hue State   */ 
  uint16_t light_target_sat;                   /*!<  Target Sat State   */ 
} QMESH_MODEL_LIGHT_HSL_NVM_STATE_T;

/*----------------------------------------------------------------------------*
 * QmeshLightHSLServerAppHandler
 */
/*! \brief Light HSL Server Event Handler
 *  \param [in] nw_hdr                  Network header \ref QMESH_NW_HEADER_INFO_T
 *  \param [in] key_info                Pointer to Access layer key \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T
 *  \param [in] elem_data               Pointer to element data \ref QMESH_ELEMENT_CONFIG_T
 *  \param [in] model_data              Pointer to Model context \ref QMESH_MODEL_DATA_T
 *  \param [in] msg                     Pointer to Message Data
 *  \param [in] msg_len                 Message Data Length
 *
 *  \return \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLServerHandler(const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                                 const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
                                                 const QMESH_ELEMENT_CONFIG_T  *elem_data,
                                                 QMESH_MODEL_DATA_T *model_data,
                                                 const uint8_t *msg,
                                                 uint16_t msg_len);

/*----------------------------------------------------------------------------*
 * QmeshLightHSLServerAppInit
 */
/*! \brief Initializes the Light HSL server
 *
 *  \param [in] context                 Model context information
 *
 *  \return \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLServerAppInit(QMESH_LIGHT_HSL_MODEL_CONTEXT_T* context);

/*----------------------------------------------------------------------------*
 * QmeshLightHSLServerAppDeInit
 */
/*! \brief Deinitializes the Light HSL server
 *
 *  \param [in] context                 Model context information
 *
 *  \return \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightHSLServerAppDeInit(QMESH_LIGHT_HSL_MODEL_CONTEXT_T* context);

/*----------------------------------------------------------------------------*
 * QmeshLightHSLAbortTransition
 */
/*! \brief Helper function to abort Light HSL transition (for bound state operations)
 *
 *  \param [in] model_context         Model context information
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightHSLAbortTransition(QMESH_LIGHT_HSL_MODEL_CONTEXT_T* model_context);

/*----------------------------------------------------------------------------*
 * QmeshLightHslGet
 */
/*! \brief Helper function to get Light HSL Hue value
 *
 *  \param [in] context                 Model context information
 *
 *  \return HSL Hue Value
 */
/*----------------------------------------------------------------------------*/
extern uint16_t QmeshLightHslGet(QMESH_LIGHT_HSL_MODEL_CONTEXT_T* context);

/*----------------------------------------------------------------------------*
 * QmeshLightHslSet
 */
/*! \brief Helper function to set Light HSL Hue value. This Aborts any ongoing
 *         transition and also updates bound states.
 *  \param [in] context                 Model context information
 *  \param [in] transition_time         transition time
 *  \param [in] delay                   Delayed execution time
 *  \param [in] light_hsl               Light HSL value
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightHslSet(QMESH_LIGHT_HSL_MODEL_CONTEXT_T* context,
                             uint8_t transition_time,
                             uint8_t delay,
                             uint16_t light_hsl);
/*----------------------------------------------------------------------------*
 * QmeshLightHslUpdate
 */
/*! \brief Helper function to update Light HSL value
 *
 *  \param [in] model_context           Model context information
 *  \param [in] light_hsl               Light HSL value
 *
 *  \return nothing
 */
 /*----------------------------------------------------------------------------*/
extern void QmeshLightHslUpdate(QMESH_LIGHT_HSL_MODEL_CONTEXT_T *model_context,
                                uint16_t light_hsl);

/*----------------------------------------------------------------------------*
 * QmeshLightHslUpdatePublicationPeriod
 */
/*! \brief  Updates the publication period
 *
 *
 *  \param [in] model_data           Model context information
 *
 *  \return bool
 */
 /*----------------------------------------------------------------------------*/
bool QmeshLightHslUpdatePublicationPeriod(QMESH_MODEL_DATA_T *model_data);

/*----------------------------------------------------------------------------*
 * QmeshLightHslPublicationHandler
 */
/*! \brief  HSL publication handler
 *
 *  \param [in] model_data           Model context information
 *
 *  \return nothing
 */
 /*----------------------------------------------------------------------------*/
extern void QmeshLightHslPublicationHandler(QMESH_MODEL_DATA_T *model_data);

/*----------------------------------------------------------------------------*
 * QmeshLightHslStatusPublish
 */
/*! \brief  This functions publishes the HSL status.
 *
 *  \param [in] model_context           Model context information
 *
 *  \return nothing
 */
 /*----------------------------------------------------------------------------*/
extern void QmeshLightHslStatusPublish(QMESH_LIGHT_HSL_MODEL_CONTEXT_T *model_context);

/*!@} */
#endif /* __QMESH_LIGHT_HSL_HANDLER_H__ */
