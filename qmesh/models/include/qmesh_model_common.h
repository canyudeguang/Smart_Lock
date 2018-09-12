/*=============================================================================
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/******************************************************************************/
/*! \file qmesh_model_common.h
 *  \brief
 *      Defines the common data structures common across all the models
 *
 */
/******************************************************************************/
#ifndef __QMESH_MODEL_COMMON_H__
#define __QMESH_MODEL_COMMON_H__

#include "qmesh_hal_ifce.h"
#include "qmesh_types.h"
#include "qmesh_model_config.h"
#include "qmesh_model_debug.h"
#include "qmesh.h"
#include "qmesh_ps.h"
#include "qmesh_soft_timers.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup Model_Common
 * @{
 */

/* Delay resolution (5 ms) */
#define QMESH_MODEL_DELAY_RESOLUTION                      (5)

/* Transition time related macros */
#define QMESH_MODEL_INVALID_TRANSITION_TIME               (0x3F)
#define QMESH_MODEL_UNKNOWN_TRANSITION_TIME               (0x3F)
#define QMESH_MODEL_TRANSITION_NOT_INPROGRESS             (0)

/* Transition step resolution and number of steps bitmasks */
#define QMESH_MODEL_TRANSITION_STEP_RESOLUTION_BITMASK    (0xC0)
#define QMESH_MODEL_TRANSITION_NUM_OF_TRANSITION_STEPS_BITMASK          (0x3F)
#define STEP_RESOLUTION(val)                              ((val & QMESH_MODEL_TRANSITION_STEP_RESOLUTION_BITMASK) >> 6)
#define NUM_OF_TRANSITION_STEPS(val)                                    (val & QMESH_MODEL_TRANSITION_NUM_OF_TRANSITION_STEPS_BITMASK)

/* Transition step resolution values */
#define QMESH_MODEL_STEP_RESOLUTION_100_MSEC              (0x0)
#define QMESH_MODEL_STEP_RESOLUTION_1_SEC                 (0x1)
#define QMESH_MODEL_STEP_RESOLUTION_10_SEC                (0x2)
#define QMESH_MODEL_STEP_RESOLUTION_10_MINUTES            (0x3)

/* Transition step resolution value in milliseconds */
#define QMESH_100_MILLI_SEC                               (100)
#define QMESH_SEC                                         (1000)
#define QMESH_10_SEC                                      (10000)
#define QMESH_10_MINUTES                                  (600000)

/* Maximum transition time supported by each step resolution value */
#define QMESH_MAX_TRANSITION_STEPS                        (62)
#define QMESH_100_MILLI_SEC_MAX_TIME                      (6200)
#define QMESH_SEC_MAX_TIME                                (62000)
#define QMESH_10_SEC_MAX_TIME                             (620000)
#define QMESH_10_MINUTES_MAX_TIME                         (37200000)

/* Publish time macros in milliseconds */
#define TRANSITION_TIME_PUBLISH_THRESHOLD                  (2000)
#define PUBLISH_TIME                                       (1000)

/* Target time in case of indefinite transition */
#define UNKNOWN_TARGET_TIME                                (0xFFFFFFFF)

/* Model Opcode offset in message */
#define MODEL_OPCODE_OFFSET                               (0UL)

/* One byte opcode messages mask */
#define QMESH_OPCODE_FORMAT_ONE_BYTE                      (0x00)
/* Two bytes opcode messages mask */
#define QMESH_OPCODE_FORMAT_TWO_BYTE                      (0x02)
/* Three bytes opcode messages mask */
#define QMESH_OPCODE_FORMAT_THREE_BYTE                    (0x03)

/* Size of opcodes */
#define CONFIG_MESSAGE_ONEB_OPCODE_SIZE                   (1UL)
#define CONFIG_MESSAGE_TWOB_OPCODE_SIZE                   (2UL)
#define CONFIG_MESSAGE_THREEB_OPCODE_SIZE                 (3UL)

#define QMESH_MODEL_DELAY_STEP_IN_MSEC                      (5)
/* Opcode Format */
#define get_opcode_format(opcode)                         (( (opcode) >> 6 ) & 3 )

/* Two bytes Opcode value */
#define QMESH_PACK_TWO_BYTE_OPCODE(opc_b1, opc_b2)        ((((uint16_t)opc_b1) << 8) | (opc_b2) )

/* Three bytes Opcode value */
#define QMESH_PACK_THREE_BYTE_OPCODE(opc_b1, opc_b2, opc_b3)    ((((uint32_t)opc_b1) << 16) | (((uint32_t)opc_b2) << 8) | (opc_b3))


/* Default TTL value */
#define QMESH_MODEL_DEFAULT_TTL                           (10)

/* MIC values */
#define QMESH_MIC_4_BYTES                                 (4)
#define QMESH_MIC_8_BYTES                                 (8)

/* Retrieve absolute value */
#define ABS_VAL(a)                                        (((a) < 0) ? (-a) : (a))


/* Maximum and Minimum level values supported */
#define MAX_LEVEL                                         (32767)
#define MIN_LEVEL                                         (-32768)


/* App event callback Handler. */
typedef void (*QMESH_MODEL_APP_CALLBACK_T) (const QMESH_NW_HEADER_INFO_T
        *nw_hdr,
        const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
        const QMESH_ELEMENT_DATA_T *elem_data,
        QMESH_MODEL_DATA_T *model_data,
        const uint8_t *msg,
        uint16_t msg_len);

/* Server Model IDs */
#define QMESH_MODEL_GENERIC_ONOFF                         (0X1000)
#define QMESH_MODEL_GENERIC_LEVEL                         (0X1002)
#define QMESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME       (0x1004)
#define QMESH_MODEL_GENERIC_POWERONOFF                    (0x1006)
#define QMESH_MODEL_GENERIC_POWERONOFFSETUPSERVER         (0x1007)
#define QMESH_MODEL_GENERIC_POWERLEVEL_SERVER             (0x1009)
#define QMESH_MODEL_GENERIC_POWERLEVEL_SETUP_SERVER       (0x100A)
#define QMESH_MODEL_LIGHT_LIGHTNESS                       (0x1300)
#define QMESH_MODEL_LIGHT_LIGHTNESS_SETUP_SERVER          (0x1301)
#define QMESH_MODEL_LIGHT_HSL_SERVER                      (0x1307)
#define QMESH_MODEL_LIGHT_HSL_HUE_SERVER                  (0x130A)
#define QMESH_MODEL_LIGHT_HSL_SATURATION_SERVER           (0x130B)
#define QMESH_MODEL_LIGHT_HSL_SETUP_SERVER                (0x1308)

/* Vender Server Model IDs */
#define QMESH_VENDOR_MODEL_SERVER                         (0x003F002A)

/* Client Model IDs */
#define QMESH_MODEL_GENERIC_ONOFF_CLIENT                   (0x1001)
#define QMESH_MODEL_GENERIC_POWERONOFF_CLIENT              (0x1008)
#define QMESH_MODEL_GENERIC_LEVEL_CLIENT                   (0x1003)
#define QMESH_MODEL_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT (0x1005)
#define QMESH_MODEL_GENERIC_POWERLEVEL_CLIENT              (0x100B)
#define QMESH_MODEL_LIGHT_LIGHTNESS_CLIENT                 (0x1302)
#define QMESH_MODEL_LIGHT_HSL_CLIENT                       (0x1309)

/* Vender Client Model IDs */
#define QMESH_VENDOR_MODEL_CLIENT                          (0x003F002B)

#define QMESH_NUM_MODEL_SOFT_TIMERS                        (35)
extern QMESH_TIMER_GROUP_HANDLE_T model_timer_ghdl;     /* Model Timer group handle */

#define QMESH_MODELS_TOTAL_POOL_SIZE                       (MODEL_CACHE_ENTRY_SIZE * NUM_DELAY_CACHE_ENTRIES)
#define QMESH_MODELS_TOTAL_POOL_ENTRY                      (1)
#define QMESH_MODELS_TOTAL_POOLS                           (NUM_DELAY_CACHE_ENTRIES)
#define QMESH_MODELS_BUFFER_SIZE                           QMESH_MM_BUFFER_SIZE(QMESH_MODELS_TOTAL_POOL_SIZE,   \
                                                                                QMESH_MODELS_TOTAL_POOL_ENTRY,  \
                                                                                QMESH_MODELS_TOTAL_POOLS)

/*!\brief  Defines status code for messages containing status parameter */
typedef enum
{
    QMESH_MODEL_STATUS_SUCCESS = 0x0,
    QMESH_MODEL_STATUS_RANGE_MIN_SET_FAILURE = 0x1,
    QMESH_MODEL_STATUS_RANGE_MAX_SET_FAILURE = 0x2
} QMESH_MODEL_STATUS_CODE;

/*!\brief  Transition State Machine */
typedef enum
{
    QMESH_TRANSITION_BEGIN,
    QMESH_TRANSITION_IN_PROGRESS,
    QMESH_TRANSITION_END,
    QMESH_TRANSITION_IDLE,
    QMESH_TRANSITION_ABORTING
} QMESH_TRANSITION_STATE;


/*!\brief  Model message opcodes  */
typedef enum
{
    QMESH_GENERIC_ONOFF_GET                                 =   0X8201,
    QMESH_GENERIC_ONOFF_SET                                 =   0X8202,
    QMESH_GENERIC_ONOFF_SET_UNRELIABLE                      =   0X8203,
    QMESH_GENERIC_ONOFF_STATUS                              =   0X8204,
    QMESH_GENERIC_LEVEL_GET                                 =   0X8205,
    QMESH_GENERIC_LEVEL_SET                                 =   0X8206,
    QMESH_GENERIC_LEVEL_SET_UNRELIABLE                      =   0X8207,
    QMESH_GENERIC_LEVEL_STATUS                              =   0X8208,
    QMESH_GENERIC_DELTA_SET                                 =   0X8209,
    QMESH_GENERIC_DELTA_SET_UNRELIABLE                      =   0X820A,
    QMESH_GENERIC_MOVE_SET                                  =   0X820B,
    QMESH_GENERIC_MOVE_SET_UNRELIABLE                       =   0X820C,
    QMESH_GENERIC_DEFAULT_TRANSITION_TIME_GET               =   0X820D,
    QMESH_GENERIC_DEFAULT_TRANSITION_TIME_SET               =   0X820E,
    QMESH_GENERIC_DEFAULT_TRANSITION_TIME_SET_UNRELIABLE    =   0X820F,
    QMESH_GENERIC_DEFAULT_TRANSITION_TIME_STATUS            =   0X8210,
    QMESH_GENERIC_POWERONOFF_GET                            =   0x8211,
    QMESH_GENERIC_POWERONOFF_SET                            =   0x8213,
    QMESH_GENERIC_POWERONOFF_SET_UNRELIABLE                 =   0x8214,
    QMESH_GENERIC_POWERONOFF_STATUS                         =   0x8212,

    /* Generic Power Level model */
    QMESH_GENERIC_POWER_LEVEL_GET                           =   0x8215,
    QMESH_GENERIC_POWER_LEVEL_SET                           =   0x8216,
    QMESH_GENERIC_POWER_LEVEL_SET_UNRELIABLE                =   0x8217,
    QMESH_GENERIC_POWER_LEVEL_STATUS                        =   0x8218,
    QMESH_GENERIC_POWER_LAST_GET                            =   0x8219,
    QMESH_GENERIC_POWER_LAST_STATUS                         =   0x821A,
    QMESH_GENERIC_POWER_DEFAULT_GET                         =   0x821B,
    QMESH_GENERIC_POWER_DEFAULT_STATUS                      =   0x821C,
    QMESH_GENERIC_POWER_RANGE_GET                           =   0x821D,
    QMESH_GENERIC_POWER_RANGE_STATUS                        =   0x821E,

    /* Generic Power Level Setup model */
    QMESH_GENERIC_POWER_DEFAULT_SET                         =   0x821F,
    QMESH_GENERIC_POWER_DEFAULT_SET_UNRELIABLE              =   0x8220,
    QMESH_GENERIC_POWER_RANGE_SET                           =   0x8221,
    QMESH_GENERIC_POWER_RANGE_SET_UNRELIABLE                =   0x8222,
    QMESH_LIGHT_LIGHTNESS_GET                               =   0x824B,
    QMESH_LIGHT_LIGHTNESS_SET                               =   0x824C,
    QMESH_LIGHT_LIGHTNESS_SET_UNRELIABLE                    =   0x824D,
    QMESH_LIGHT_LIGHTNESS_STATUS                            =   0x824E,
    QMESH_LIGHT_LIGHTNESS_LINEAR_GET                        =   0x824F,
    QMESH_LIGHT_LIGHTNESS_LINEAR_SET                        =   0x8250,
    QMESH_LIGHT_LIGHTNESS_LINEAR_SET_UNRELIABLE             =   0x8251,
    QMESH_LIGHT_LIGHTNESS_LINEAR_STATUS                     =   0x8252,
    QMESH_LIGHT_LIGHTNESS_LAST_GET                          =   0x8253,
    QMESH_LIGHT_LIGHTNESS_LAST_STATUS                       =   0x8254,
    QMESH_LIGHT_LIGHTNESS_DEFAULT_GET                       =   0x8255,
    QMESH_LIGHT_LIGHTNESS_DEFAULT_STATUS                    =   0x8256,
    QMESH_LIGHT_LIGHTNESS_RANGE_GET                         =   0x8257,
    QMESH_LIGHT_LIGHTNESS_RANGE_STATUS                      =   0x8258,
    QMESH_LIGHT_LIGHTNESS_DEFAULT_SET                       =   0x8259,
    QMESH_LIGHT_LIGHTNESS_DEFAULT_SET_UNRELIABLE            =   0x825A,
    QMESH_LIGHT_LIGHTNESS_RANGE_SET                         =   0x825B,
    QMESH_LIGHT_LIGHTNESS_RANGE_SET_UNRELIABLE              =   0x825C,
    QMESH_LIGHT_HSL_GET                                     =   0X826D,
    QMESH_LIGHT_HSL_HUE_GET                                 =   0X826E,
    QMESH_LIGHT_HSL_HUE_SET                                 =   0X826F,
    QMESH_LIGHT_HSL_HUE_SET_UNRELIABLE                      =   0X8270,
    QMESH_LIGHT_HSL_HUE_STATUS                              =   0X8271,
    QMESH_LIGHT_HSL_SAT_GET                                 =   0X8272,
    QMESH_LIGHT_HSL_SAT_SET                                 =   0X8273,
    QMESH_LIGHT_HSL_SAT_SET_UNRELIABLE                      =   0X8274,
    QMESH_LIGHT_HSL_SAT_STATUS                              =   0X8275,
    QMESH_LIGHT_HSL_SET                                     =   0X8276,
    QMESH_LIGHT_HSL_SET_UNRELIABLE                          =   0X8277,
    QMESH_LIGHT_HSL_STATUS                                  =   0X8278,
    QMESH_LIGHT_HSL_TARGET_GET                              =   0X8279,
    QMESH_LIGHT_HSL_TARGET_STATUS                           =   0X827A,
    QMESH_LIGHT_HSL_DEFAULT_GET                             =   0X827B,
    QMESH_LIGHT_HSL_DEFAULT_STATUS                          =   0X827C,
    QMESH_LIGHT_HSL_RANGE_GET                               =   0X827D,
    QMESH_LIGHT_HSL_RANGE_STATUS                            =   0X827E,
    QMESH_LIGHT_HSL_DEFAULT_SET                             =   0X827F,
    QMESH_LIGHT_HSL_DEFAULT_SET_UNRELIABLE                  =   0X8280,
    QMESH_LIGHT_HSL_RANGE_SET                               =   0X8281,
    QMESH_LIGHT_HSL_RANGE_SET_UNRELIABLE                    =   0X8282
} QMESH_MODEL_EVENT_T;

/*!\brief  Application event opcodes  */
typedef enum
{
    QAPP_GET_LVL_RANGE,
    QAPP_LVL_UPDATED,
    QAPP_LVL_TRANS_BEGIN,
    QAPP_LIGHTNESS_ACTUAL_UPDATED,
    QAPP_LIGHTNESS_ACTUAL_TRANS_BEGIN,
    QAPP_GEN_ONOFF_TRANS_BEGIN,
    QAPP_GEN_ONOFF_UPDATED,
    QAPP_ONOFF_BOOTVALUE,
    QAPP_ONPOWERUP_BOOTVALUE,
    QAPP_ONPOWERUP_UPDATED,
    QAPP_PWR_LVL_ACTUAL_TRANS_BEGIN,
    QAPP_PWR_LVL_ACTUAL_UPDATED,
    QAPP_LIGHT_HSL_UPDATED,
    QAPP_LIGHT_HUE_UPDATED,
    QAPP_LIGHT_SAT_UPDATED,
    QAPP_LIGHT_VALIDATE_SAT,
    QAPP_LIGHT_VALIDATE_HUE,
    QAPP_LIGHT_HSL_TRANS_BEGIN,
    QAPP_LIGHT_HSL_HUE_TRANS_BEGIN,
    QAPP_LIGHT_HSL_SAT_TRANS_BEGIN,
    QAPP_LIGHT_HSL_UPDATE_LIGHT_DEFAULT,
    QAPP_LIGHT_HSL_GET_LIGHT_DEFAULT
} QMESH_APPLICATION_OPCODES_T;


/*!\brief structure to message min max range of level value */
typedef struct
{
    int16_t max_val;            /*!< Maximum value of level.*/
    int16_t min_val;            /*!< Minimum value of level.*/
} QAPP_GET_LVL_RANGE_T;

/*!\brief Message details stored in the cache */
typedef struct
{
    uint32_t    opcode;         /*!< Message opcode - 1 octet or 2 or 3 octects */
    uint16_t    elem_id;        /*!< Destination element index to which this message belongs to */
    uint16_t    src;            /*!< Source element address */
    uint16_t    dst;            /*!< Source element address */
    uint8_t     tid;            /*!< Transaction Identifier */
} QMESH_MODEL_MSG_COMMON_T;

typedef void (*QmeshAppCallback)(uint16_t elem_addr,uint16_t app_opcode,void *data);

/*----------------------------------------------------------------------------*
 * QmeshInitModelCommon
 */
/*! \brief  This API registers the application callback with the model common code.
 *      
 *
 *  \param [in] appcallback   Pointer to an application callback function
 *  \param [in] server_device_composition   Pointer to composition data
 *
 *  \return None
 */
/*---------------------------------------------------------------------------*/
extern void QmeshInitModelCommon (QmeshAppCallback appcallback,
                                  const QMESH_DEVICE_COMPOSITION_DATA_T* server_device_composition);

/*----------------------------------------------------------------------------*
 * QmeshGetModelContextFromComp
 */
/*! \brief This API returns the model data.
 *
 *  \param [in] elem_addr     Element address
 *  \param [in] model_id      Model identifier
 *
 *  \return Model context.
 */
/*---------------------------------------------------------------------------*/
extern void *QmeshGetModelContextFromComp (uint16_t elem_addr, uint16_t model_id);

/*----------------------------------------------------------------------------*
 * QmeshGetRemainingTime
 */
/*! \brief  This API calcuates the time required to reach the target time ('trgtTime') from this moment.
 *      The API returns the remaining time in 'Generic Default Transition Time State Format' as mentioned in the
 *      Model specification v1.0 (section 3.1.3).
 *
 *  \param [in] trgt_time           Target time
 *
 *  \return None
 */
/*---------------------------------------------------------------------------*/
extern uint8_t QmeshGetRemainingTime (uint32_t trgt_time);

/*----------------------------------------------------------------------------*
 * QmeshConvertTimeToTransitionTimeFormat
 */
/*! \brief  The API returns the remaining time in 'Generic Default Transition Time State Format' as
 *          mentioned in the Model specification
 *
 *  \param [in] remaining_time      Remaining time
 *
 *  \return Remaining Time in uint8_t
 */
/*---------------------------------------------------------------------------*/
extern uint8_t QmeshConvertTimeToTransitionTimeFormat (uint32_t remaining_time);

/*----------------------------------------------------------------------------*
 * QmeshTransitionTimeToMilliSec
 */
/*! \brief  This API converts the time mentioned in the 'Generic Default Transition Time State Format' as mentioned in the
 *          Model specification d09r11 (section 3.1.3) to milli seconds.
 *
 *  \param [in] transition_time   Transition time
 *
 *  \return Time in milliseconds.
 */
/*---------------------------------------------------------------------------*/
uint32_t QmeshTransitionTimeToMilliSec (uint8_t transition_time);

/*----------------------------------------------------------------------------*
 * QmeshGetStepResolutionInMilliSec
 */
/*! \brief  This API returns the step resolution in milliseconds
 *
 *  \param [in] step_resolution   Step resolution time
 *
 *  \return Step resolution in milliseconds
 */
/*---------------------------------------------------------------------------*/
uint32_t QmeshGetStepResolutionInMilliSec (uint8_t step_resolution);

/*----------------------------------------------------------------------------*
 * QmeshGetGenericDTT
 */
/*! \brief  This function returns the default transition time
 *
 *  \param [in] elem_data     Element Configuration data
 *
 *  \return Default transition time
 */
/*---------------------------------------------------------------------------*/
extern uint8_t QmeshGetGenericDTT (const QMESH_ELEMENT_CONFIG_T *elem_data);

/*----------------------------------------------------------------------------*
 * QmeshGetDTTFromPrimElement
 */
/*! \brief  This function returns the default transition time from primary element
 *
 *
 *  \return Default transition time
 */
/*---------------------------------------------------------------------------*/
extern uint8_t QmeshGetDTTFromPrimElement(void);

/*----------------------------------------------------------------------------*
 * QmeshUpdateBoundStates
 */
/*! \brief This API handles all the bound states operation for models.
 *
 *  \param [in] elem_addr     Element address
 *  \param [in] app_opcode    Opcode
 *  \param [in] data          data
 *
 *  \return TRUE if handled.
 */
/*---------------------------------------------------------------------------*/
extern bool QmeshUpdateBoundStates (uint16_t elem_addr,
                                    uint16_t app_opcode,
                                    void *data);

/*----------------------------------------------------------------------------*
 * QmeshGetModelData
 */
/*! \brief This API returns the model data.
 *
 *  \param [in] elem_config     Element configuration data
 *  \param [in] model_id        Model identifier
 *
 *  \return \ref QMESH_MODEL_DATA_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_MODEL_DATA_T *QmeshGetModelData (const QMESH_ELEMENT_CONFIG_T *elem_config,
                                              uint32_t model_id);

/*----------------------------------------------------------------------------*
 * QmeshUpdateModelWithUnicastAddr
 */
/*! \brief This API updates the model with unicast address for the correspond elements.
 *
 *  \return Nothing
 */
/*---------------------------------------------------------------------------*/
extern void QmeshUpdateModelWithUnicastAddr(void);

#ifdef __cplusplus
}
#endif

/*!@} */

#endif /* __QMESH_MODEL_COMMON_H__ */
