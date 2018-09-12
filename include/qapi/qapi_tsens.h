#ifndef __QAPI_TSENS_H__
#define __QAPI_TSENS_H__
/**
 * @file qapi_tsens.h
 *
 * Temperature Sensor (TSENS)
 *
 * @addtogroup qapi_tsens
 * @{
 *
 * The temperature sensor is used to monitor the temperature of the SoC
 *          using on-die analog sensors.
 *
 *          This programming interface allows client software to read the
 *          temperature returned by each sensor. The code snippet below shows
 * an example usage.
 *
 * Consult hardware documentation for the placement of sensors on the die.
 *
 * @code {.c}
 *
 *
 *   * The code snippet below demonstates usage of this interface. The example
 *   * below opens TSENS to obtain a handle, gets the number of sensors, reads
 *   * each sensor's temperature, and then closes the handle.
 *
 *   qapi_Status_t status;
 *   qapi_TSENS_Handle_t handle;
 *   uint32_t num_sensors;
 *   uint32_t sensor;
 *   qapi_TSENS_Result_t result;
 *
 *   status = qapi_TSENS_Open(&handle);
 *   if (status != QAPI_OK) { ... }
 *
 *   status = qapi_TSENS_Get_Num_Sensors(handle, &num_sensors);
 *   if (status != QAPI_OK) { ... }
 *
 *   for (sensor = 0; sensor < num_sensors; sensor++)
 *   {
 *      status = qapi_TSENS_Get_Temp(handle, sensor, &result);
 *      if (status != QAPI_OK) { ... }
 *
 *      // result->deg_c is the temperature in degrees Celsius
 *   }
 *
 *   status = qapi_TSENS_Close(handle);
 *   if (status != QAPI_OK) { ... }
 *   handle = NULL;
 *
 * @endcode
 *
 * @}
 */

/*==================================================================================
                   Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
                                All Rights Reserved.
                   Confidential and Proprietary - Qualcomm Technologies, Inc.
==================================================================================*/

/*==================================================================================

                               EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.ioe/1.0/v2/rom/release/api/hwengines/tsens/qapi_tsens.h#3 $

  when        who  what, where, why
  ----------  ---  ---------------------------------------------------------------
  2016-05-06  leo  (TechComm) Edited/added Doxygen comments and markup.
  2015-12-11  jjo  Initial version.

==================================================================================*/

/*----------------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------------*/

/** @addtogroup qapi_tsens
@{ */

/**
* TSENS reading result structure.
*/
typedef struct
{
   int32_t deg_c;      /**< Sensor temperature in degrees Celsius. */
} qapi_TSENS_Result_t;

/**
* @brief TSENS client handle.
*
* @details This handle is used by clients when making calls to TSENS. Clients 
*          are responsible for ensuring they do not lose the handle and for 
*          closing the handle when done with TSENS.
*/
typedef void *qapi_TSENS_Handle_t;

/*----------------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------------*/

/**
*  Opens TSENS.
*
*  @param[out] handle   Pointer to a TSENS handle.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_NO_MEMORY     -- No memory is available to support this operation.
*/
qapi_Status_t qapi_TSENS_Open
(
   qapi_TSENS_Handle_t *handle
);

/**
*  Closes TSENS.
*
*  @param[in] handle   Handle provided by qapi_TSENS_Open().
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t qapi_TSENS_Close
(
   qapi_TSENS_Handle_t handle
);

/**
*  Gets the number of TSENS sensors.
*
*  This function gets the number of TSENS sensors supported
*           by the SoC. The sensor index is zero-based and ranges from
*           0 to the number of sensors minus one.
*
*  @param[in]  handle        Handle provided by qapi_TSENS_Open().
*  @param[out] num_sensors   Number of sensors.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t qapi_TSENS_Get_Num_Sensors
(
   qapi_TSENS_Handle_t handle,
   uint32_t *num_sensors
);

/**
*  Gets a sensor temperature.
*
*  This function reads a TSENS sensor and obtains the sensor
*           temperature. The sensor index is zero-based and ranges from
*           0 to the number of sensors minus one. Consult the hardware
*           documentation for a mapping of the sensor index to the die location.
*
*  @param[in]  handle   Handle provided by qapi_TSENS_Open().
*  @param[in]  sensor   Selected sensor.
*  @param[out] result   Temperature reported by the sensor.
*
*  @return
*  QAPI_OK                -- Call succeeded. \n
*  QAPI_ERROR             -- Call failed. \n
*  QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified. \n
*  QAPI_ERR_TIMEOUT       -- The sensor did not return a reading.
*/
qapi_Status_t qapi_TSENS_Get_Temp
(
   qapi_TSENS_Handle_t handle,
   uint32_t sensor,
   qapi_TSENS_Result_t *result
);

/** @} */ /* end_addtogroup qapi_tsens */

#endif /* #ifndef __QAPI_TSENS_H__ */

