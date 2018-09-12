/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/******************************************************************************/
/*! \file qmesh_light_lightness_handler.h
 *  \brief defines and functions for SIGmesh Light Lightness Handler
 *
 *   This file contains data types and APIs exposed by the Light Lightness
 *
 */
/******************************************************************************/
#ifndef __QMESH_LIGHT_LIGHTNESS_HANDLER_H__
#define __QMESH_LIGHT_LIGHTNESS_HANDLER_H__

/*! \addtogroup  Model_Light_Lightness_Handler
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
#define LIGHTNESS_STATUS_TRANSITION_REPORT_SIZE (7)
#define LIGHTNESS_STATUS_REPORT_SIZE            (4)

/*!\brief  Lightness Message offsets */
#define OFFSET_VALUE_BYTE_LSB                   (0)
#define OFFSET_VALUE_BYTE_MSB                   (1)
#define OFFSET_TID                              (2)
#define OFFSET_TIME                             (3)
#define OFFSET_DELAY                            (4)

/*!\brief  Lightness Range Error defines */
#define LIGHTNESS_RANGE_SET_SUCCESS             (0)
#define LIGHTNESS_CANNOT_SET_RANGE_MIN          (1)
#define LIGHTNESS_CANNOT_SET_RANGE_MAX          (2)

#define LIGHTNESS_GET_MSG_LEN                      (0)
#define LIGHTNESS_SET_NON_TRANS_MSG_LEN            (3)
#define LIGHTNESS_SET_TRANS_MSG_LEN                (5)
#define LIGHTNESS_DEFAULT_SET_MSG_LEN              (2)
#define LIGHTNESS_RANGE_SET_MSG_LEN                (4)

/*!\brief  Lightness Max Value for Actual/Linear */
#define LIGHTNESS_MAX                           (65535)

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
}QMESH_TRANSITION_INFO;

/*!\brief  Lightness Server model data */
typedef struct
{
    uint16_t elm_id;                            /*!< Element Address              */
    uint16_t lightness_linear;                  /*!< Lightness Linear State       */
    uint16_t lightness_actual;                  /*!< Lightness Actual State       */
    uint16_t lightness_last;                    /*!< Lightness Last State         */
    uint16_t lightness_default;                 /*!< Lightness default  State     */
    uint16_t lightness_target;                  /*!< Lightness target   State     */
    uint16_t lightness_min_range;               /*!< Lightness Range Min State    */
    uint16_t lightness_max_range;               /*!< Lightness Range Max State    */
    uint32_t last_msg_seq_no;                   /*!< Last processed Seq Number    */
    uint16_t last_src_addr;                     /*!< Last Src address processed   */
    QMESH_TRANSITION_INFO light_trans;          /*!< Transition Information       */
    QMESH_MUTEX_T mutex_handle;                 /*!< Mutex for sync access        */
    QMESH_TIMER_HANDLE_T publish_timer;         /*!< Publish Timer                */
    QMESH_TIMER_HANDLE_T publish_retrans_timer; /*!< Timer to restransmit to Publish addresses */
    uint8_t       publish_restrans_cnt;         /*!< Publication retransmission count */
    uint32_t      publish_interval;             /*!< Publication interval */
    QMESH_MODEL_PUBLISH_STATE_T  *publish_state;/*!< Pointer to the Model Publication State */
}QMESH_LIGHTNESS_MODEL_CONTEXT_T;

/*!\brief Lightness NVM state */
typedef struct __attribute__ ((__packed__))
{
    uint16_t lightness_linear;                  /*!<  Lightness Linear State       */
    uint16_t lightness_actual;                  /*!<  Lightness Actual State       */  
    uint16_t lightness_last;                    /*!<  Lightness Last State         */
    uint16_t lightness_default;                 /*!<  Lightness default  State     */ 
    uint16_t lightness_min_range;               /*!<  Lightness Range Min State    */
    uint16_t lightness_max_range;               /*!<  Lightness Range Max State    */
    uint32_t last_msg_seq_no;                   /*!<  Last processed Seq Number    */   
    uint16_t last_src_addr;                     /*!<  Last Src address processed   */
    uint16_t lightness_target;                  /*!<  Lightness target value       */
} QMESH_MODEL_LIGHTNESS_NVM_STATE_T;

/*----------------------------------------------------------------------------*
 * QmeshLightnessServerHandler
 */
/*! \brief Lightness Event Handler
 *
 *  \param [in] nw_hdr          Network header \ref QMESH_NW_HEADER_INFO_T
 *  \param [in] key_info        Pointer to Access layer key \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T
 *  \param [in] elem_data       Pointer to element data \ref QMESH_ELEMENT_CONFIG_T
 *  \param [in] model_data      Pointer to Model context \ref QMESH_MODEL_DATA_T
 *  \param [in] msg             Pointer to Message Data
 *  \param [in] msg_len         Message Data Length
 *
 *  \return QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightnessServerHandler(const QMESH_NW_HEADER_INFO_T *nw_hdr,
                                        const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
                                        const QMESH_ELEMENT_CONFIG_T  *elem_data,
                                        QMESH_MODEL_DATA_T *model_data,
                                        const uint8_t *msg,
                                        uint16_t msg_len);

/*----------------------------------------------------------------------------*
 * QmeshLightnessServerAppInit
 */
/*! \brief Initializes the Lightness Server
 *
 *  \param [in] context         Model context information
 *
 *  \return \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLightnessServerAppInit(QMESH_LIGHTNESS_MODEL_CONTEXT_T *context);

/*----------------------------------------------------------------------------*
 * QmeshLightnessServerAppDeInit
 */
/*! \brief DeInitializes the Lightness Server
 *
 *  \param [in] context
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightnessServerAppDeInit(QMESH_LIGHTNESS_MODEL_CONTEXT_T *context);

/*----------------------------------------------------------------------------*
 * QmeshLightnessActualUpdate
 */
/*! \brief Helper function to update Lightness Actual
 *
 *  \param [in] model_data      Model context information
 *  \param [in] light_actual    Light actual value
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightnessActualUpdate(QMESH_LIGHTNESS_MODEL_CONTEXT_T* model_data,
                                       uint16_t light_actual);

/*----------------------------------------------------------------------------*
 * QmeshLightnessActualGet
 */
/*! \brief Helper function to retrieve Lightness Actual
 *
 *  \param [in] model_data      Model context information
 *
 *  \return uint16_t
 */
/*----------------------------------------------------------------------------*/
extern uint16_t QmeshLightnessActualGet(QMESH_LIGHTNESS_MODEL_CONTEXT_T* model_data);

/*----------------------------------------------------------------------------*
 * QmeshLightnessActualSet
 */
/*! \brief Helper function to Set Lightness Actual. This function aborts any ongoing
 *         transition and updates bound states.
 *
 *  \param [in] model_data          Model context information
 *  \param [in] transition_time     Transition time
 *  \param [in] delay               Delayed execution time
 *  \param [in] light_actual        Light actual value
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightnessActualSet(QMESH_LIGHTNESS_MODEL_CONTEXT_T* model_data,
                                      uint8_t transition_time,
                                      uint8_t delay,
                                      uint16_t light_actual);

/*----------------------------------------------------------------------------*
 * QmeshLightnessAbortTransition
 */
/*! \brief Helper function to abort Lightness transition (for bound state operations)
 *
 *  \param [in] context             Model context information
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightnessAbortTransition(QMESH_LIGHTNESS_MODEL_CONTEXT_T* context);

/*----------------------------------------------------------------------------*
 * QmeshLightnessDefaultGet
 */
/*! \brief Helper function to get Lightness default (for bound state operations)
 *
 *  \param [in] context             Model context information
 *
 *  \return uint16_t
 */
/*----------------------------------------------------------------------------*/
extern uint16_t QmeshLightnessDefaultGet(QMESH_LIGHTNESS_MODEL_CONTEXT_T* context);

/*----------------------------------------------------------------------------*
 * QmeshLightnessDefaultSet
 */
/*! \brief Helper function to set Lightness default. This function aborts any
 *         transition and updates bound states.
 *
 *  \param [in] context             Model context information
 *  \param [in] transition_time     Transition time
 *  \param [in] delay               Dealyed execution time
 *  \param [in] light_default       Light default value
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightnessDefaultSet(QMESH_LIGHTNESS_MODEL_CONTEXT_T* context,
                                     uint8_t transition_time,
                                     uint8_t delay,
                                     uint16_t light_default);

/*----------------------------------------------------------------------------*
 * QmeshLightnessDefaultUpdate
 */
/*! \brief Helper function to update Lightness Default
 *
 *  \param [in] model_data          Model context information
 *  \param [in] light_default       Light default value
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightnessDefaultUpdate(QMESH_LIGHTNESS_MODEL_CONTEXT_T* model_data,
                                        uint16_t light_default);

/*----------------------------------------------------------------------------*
 * QmeshLightnessLastGet
 */
/*! \brief Helper function to get Lightness Last value (for bound state operations)
 *
 *  \param [in] model_data          Model context information
 *
 *  \return uint16_t
 */
/*----------------------------------------------------------------------------*/
extern uint16_t QmeshLightnessLastGet(QMESH_LIGHTNESS_MODEL_CONTEXT_T* model_data);

/*----------------------------------------------------------------------------*
 * QmeshLightnessLastSet
 */
/*! \brief Helper function to set Lightness Last value.This function aborts any
 *         transition and updates bound states.
 *
 *  \param [in] model_data          Model context information
 *  \param [in] transition_time     Transition time
 *  \param [in] delay               Delayed execution time
 *  \param [in] light_last          Light last value
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightnessLastSet(QMESH_LIGHTNESS_MODEL_CONTEXT_T* model_data,
                                    uint8_t transition_time,
                                    uint8_t delay,
                                    uint16_t light_last);

/*----------------------------------------------------------------------------*
 * QmeshLightnessLastUpdate
 */
/*! \brief Helper function to update Lightness Last value (for bound state operations)
 *
 *  \param [in] model_data          Model context information
 *  \param [in] light_last          Light last value
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightnessLastUpdate(QMESH_LIGHTNESS_MODEL_CONTEXT_T* model_data,
                                     uint16_t light_last);

/*----------------------------------------------------------------------------*
 * QmeshLightnessMinRangeGet
 */
/*! \brief Helper function to get Lightness Min Range value (for bound state operations)
 *
 *  \param [in] model_data          Model context information
 *
 *  \return uint16_t
 */
/*----------------------------------------------------------------------------*/
extern uint16_t QmeshLightnessMinRangeGet(QMESH_LIGHTNESS_MODEL_CONTEXT_T* model_data);

/*----------------------------------------------------------------------------*
 * QmeshLightnessMaxRangeGet
 */
/*! \brief Helper function to get Lightness Max Range value (for bound state operations)
 *
 *  \param [in] model_data          Model context information
 *
 *  \return uint16_t
 */
/*----------------------------------------------------------------------------*/
extern uint16_t QmeshLightnessMaxRangeGet(QMESH_LIGHTNESS_MODEL_CONTEXT_T* model_data);

/*----------------------------------------------------------------------------*
 * QmeshLightLightnessPublicationPeriod
 */
/*! \brief Publication configuration handler for Light Lightness Server.
 *
 *  \param [in] model_data          Model context information
 *
 *  \return bool
 */
/*----------------------------------------------------------------------------*/
bool QmeshLightLightnessUpdatePublicationPeriod(QMESH_MODEL_DATA_T *model_data);

/*----------------------------------------------------------------------------*
 * QmeshLightLightnessPublicationHandler
 */
/*! \brief Publication configuration handler for Light Lightness model
 *
 *  \param [in] model_data          Model context information
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightLightnessPublicationHandler(QMESH_MODEL_DATA_T *model_data);

/*----------------------------------------------------------------------------*
 * QmeshLightLightnessStatusPublish
 */
/*! \brief This function sends the current Light Lightness status message to the
 *      Publish address assigned for this model.
 *
 *  \param [in] model_context       Model context information
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshLightLightnessStatusPublish(QMESH_LIGHTNESS_MODEL_CONTEXT_T *model_context);

/*----------------------------------------------------------------------------*
 * QmeshSendLightnessStatusUpdate
 */
/*! \brief Helper function to send lightness status response back.
 *
 *  \param [in] elem_id             Element address
 *  \param [in] src_addr            Source address
 *  \param [in] opcode              Op Code of the message
 *  \param [in] key_info            Key information
 *  \param [in] lightness_context   Light context
 *
 *  \return nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshSendLightnessStatusUpdate (uint16_t elem_id,
        uint16_t src_addr,
        uint16_t opcode,
        const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
        QMESH_LIGHTNESS_MODEL_CONTEXT_T *lightness_context);
/*!@} */
#endif /* __QMESH_LIGHTNESS_HANDLER_H__ */
