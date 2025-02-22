
/**
 * @file qapi_ble_lls.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Link Loss Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Link Loss Service (LLS) programming interface defines the
 * protocols and procedures to be used to implement the Link Loss
 * Service capabilities.
 */

#ifndef __QAPI_BLE_LLS_H__
#define __QAPI_BLE_LLS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_llstypes.h"  /* QAPI LLS prototypes.                     */

/** @addtogroup qapi_ble_services
@{
*/

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errros.h).                                               */
#define QAPI_BLE_LLS_ERROR_INVALID_PARAMETER             (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_LLS_ERROR_INVALID_BLUETOOTH_STACK_ID    (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_LLS_ERROR_INSUFFICIENT_RESOURCES        (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_LLS_ERROR_SERVICE_ALREADY_REGISTERED    (-1003)
/**< Service is already registered. */
#define QAPI_BLE_LLS_ERROR_INVALID_INSTANCE_ID           (-1004)
/**< Invalid service instance ID. */
#define QAPI_BLE_LLS_ERROR_MALFORMATTED_DATA             (-1005)
/**< Malformatted data. */
#define QAPI_BLE_LLS_ERROR_UNKNOWN_ERROR                 (-1006)
/**< Unknown error. */

/**
 * Structure that contains the attribute handles that will need to be
 * cached by a LLS Client in order to only do service discovery once.
 */
typedef struct qapi_BLE_LLS_Client_Information_s
{
   /**
    * Alert Level attribute handle.
    */
   uint16_t Alert_Level;
} qapi_BLE_LLS_Client_Information_t;

#define QAPI_BLE_LLS_CLIENT_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_LLS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_LLS_Client_Information_t structure.
 */

/**
 * Structure that contains all of the per Client data that will need to
 * be stored by a LLS server.
 */
typedef struct qapi_BLE_LLS_Server_Information_s
{
   /**
    * Alert Level.
    */
   uint16_t Alert_Level;
} qapi_BLE_LLS_Server_Information_t;

#define QAPI_BLE_LLS_SERVER_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_LLS_Server_Information_t))
/**<
 * Size of the #qapi_BLE_LLS_Server_Information_t structure.
 */

/**
 * Enumeration of all the events generated by the LLS
 * Profile. These are used to determine the type of each event
 * generated, and to ensure the proper union element is accessed for the
 * #qapi_BLE_LLS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ET_LLS_ALERT_LEVEL_UPDATE_E /**< Alert level update event. */
} qapi_BLE_LLS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to an LLS server when an LLS client has sent a request to write the LLS
 * Alert Level Characteristic.
 */
typedef struct qapi_BLE_LLS_Alert_Level_Update_Data_s
{
  /**
    * LLS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the LLS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the LLS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the LLS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * Alert Level that has been requested to be written.
    */
   uint8_t                         AlertLevel;
} qapi_BLE_LLS_Alert_Level_Update_Data_t;

#define QAPI_BLE_LLS_ALERT_LEVEL_UPDATE_DATA_SIZE        (sizeof(qapi_BLE_LLS_Alert_Level_Update_Data_t))
/**<
 * Size of the #qapi_BLE_LLS_Alert_Level_Update_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * event data for a LLS instance.
 */
typedef struct qapi_BLE_LLS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_LLS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                  Event_Data_Size;
   union
   {
      /**
       * LLS Alert Level Update event data.
       */
      qapi_BLE_LLS_Alert_Level_Update_Data_t *LLS_Alert_Level_Update_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_LLS_Event_Data_t;

#define QAPI_BLE_LLS_EVENT_DATA_SIZE                     (sizeof(qapi_BLE_LLS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_LLS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the prototype function for
 * an LLS Service Event Receive Data Callback. This function will be called
 * whenever an LLS Service Event occurs that is associated with the
 * specified Bluetooth stack ID.

 * @details
 * The caller should use the contents of the LLS Profile Event Data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, the callback function
 * must copy the data into another data buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e., this
 * function does not have be reentrant). It should be noted, however,
 * that if the same callback is installed more than once, the
 * callbacks will be called serially. Because of this, the processing
 * in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the Thread
 * Context of a Thread that the user does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another LLS Service Event will not be processed while this
 * function call is outstanding).
 *
 * @note1hang
 * This function must not block and wait for events that can only be
 * satisfied by receiving other Bluetooth Stack Events. A Deadlock
 * will occur because other callbacks might not be issued while
 * this function is currently outstanding.
 *
 * @param[in]  BluetoothStackID      Unique identifier assigned to this
 *                                   Bluetooth Protocol Stack on which the
 *                                   event occurred.
 *
 * @param[in]  LLS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value to was supplied
 *                                   as an input parameter from a prior
 *                                   LLS request.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_LLS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_LLS_Event_Data_t *LLS_Event_Data, uint32_t CallbackParameter);

   /* LLS Server API.                                                   */

/**
 * @brief
 * Opens an LLS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one LLS server may be open at a time, per the Bluetooth Stack ID.
 *
 * All Client Requests will be dispatch to the EventCallback function
 * that is specified by the second parameter to this function.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  EventCallback        Callback function that is registered
 *                                  to receive events that are associated
 *                                  with the specified service.
 *
 * @param[in]  CallbackParameter    User-defined parameter that will be
 *                                  passed back to the user in the callback
 *                                  function.
 *
 * @param[out]  ServiceID           Unique GATT service ID of the
 *                                  registered LLS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of LLS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LLS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_LLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LLS_Initialize_Service(uint32_t BluetoothStackID, qapi_BLE_LLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Opens an LLS server on a specified Bluetooth Stack with
 * the ability to control the location of the service in the GATT datallse.
 *
 * @details
 * Only one LLS server may be open at a time, per the Bluetooth Stack ID.
 *
 * All Client Requests will be dispatch to the EventCallback function
 * that is specified by the second parameter to this function.
 *
 * @param[in]      BluetoothStackID      Unique identifier assigned to
 *                                       this Bluetooth Protocol Stack
 *                                       via a call to
 *                                       qapi_BLE_BSC_Initialize().
 *
 * @param[in]      EventCallback         Callback function that is
 *                                       registered to receive events
 *                                       that are associated with the
 *                                       specified service.
 *
 * @param[in]      CallbackParameter     User-defined parameter that
 *                                       will be passed back to the user
 *                                       in the callback function.
 *
 * @param[in]      ServiceID             Unique GATT service ID of the
 *                                       registered LLS service returned
 *                                       from
 *                                       qapi_BLE_GATT_Register_Service()
 *                                       API.
 *
 * @param[in,out]  ServiceHandleRange    Pointer to a Service Handle
 *                                       Range structure that, on input
 *                                       can be used to control the
 *                                       location of the service in the
 *                                       GATT database, and on output,
 *                                       returns the handle range that
 *                                       the service is using in the GATT
 *                                       database.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of LLS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LLS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_LLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LLS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, qapi_BLE_LLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a LLS service instance.
 *
 * @details
 * After this function is called, no other LLS service function can be
 * called until after a successful call to the qapi_BLE_LLS_Initialize_Service()
 * function is performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_LLS_Initialize_Service()
 *                                 function.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LLS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LLS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * This function is responsible for querying the number of attributes
 * that are contained in the LLS Service that is registered with a
 * call to qapi_BLE_LLS_Initialize_Service() or
 * qapi_BLE_LLS_Initialize_Service_Handle_Range().
 *
 * @return   Positive, nonzero, number of attributes that would be
 *           registered by a LLS service instance.
 *
 * @return   Zero on failure.
 */
QAPI_BLE_DECLARATION uint32_t QAPI_BLE_BTPSAPI qapi_BLE_LLS_Query_Number_Attributes(void);

/**
 * @brief
 * Sets the LLS Alert Level
 * for the LLS server.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_LLS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  Alert_Level         Alert Level that will be set if
 *                                 this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LLS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LLS_Set_Alert_Level(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t Alert_Level);

/**
 * @brief
 * Queries the LLS Alert Level
 * for the LLS server.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_LLS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  Alert_Level        Pointer to the Alert Level that
 *                                 will be set if this function is
 *                                 successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LLS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LLS_Query_Alert_Level(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *Alert_Level);

/**
 * @}
 */

#endif

