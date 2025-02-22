
/**
 * @file qapi_ble_bas.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Battery Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Battery Service (BAS)programming interface defines the
 * protocols and procedures to be used to implement Battery
 * Service capabilities.
 */

#ifndef __QAPI_BLE_BAS_H__
#define __QAPI_BLE_BAS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_bastypes.h"  /* QAPI BAS prototypes.                     */

/** @addtogroup qapi_ble_services
@{
*/

   /** @name Error Return Codes

      Error codes that are smaller than these (less than -1000) are
      related to the Bluetooth Protocol Stack itself (see
      qapi_ble_errors.h).
      @{ */
#define QAPI_BLE_BAS_ERROR_INVALID_PARAMETER                      (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_BAS_ERROR_INVALID_BLUETOOTH_STACK_ID             (-1001)
/**< Invalid Bluetooth stack ID. */
#define QAPI_BLE_BAS_ERROR_INSUFFICIENT_RESOURCES                 (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_BAS_ERROR_SERVICE_ALREADY_REGISTERED             (-1003)
/**< Service is already registered. */
#define QAPI_BLE_BAS_ERROR_INVALID_INSTANCE_ID                    (-1004)
/**< Invalid service instance ID. */
#define QAPI_BLE_BAS_ERROR_MALFORMATTED_DATA                      (-1005)
/**< Malformatted data. */
#define QAPI_BLE_BAS_ERROR_MAXIMUM_NUMBER_OF_INSTANCES_REACHED    (-1006)
/**< Maximum number of instances was reached. */
#define QAPI_BLE_BAS_ERROR_UNKNOWN_ERROR                          (-1007)
/**< Unknown error. */

/** @} */ /* end namegroup */

/**
 * Structure that represents the format for the Presentation Format
 * Descriptor.
 */
typedef struct qapi_BLE_BAS_Presentation_Format_Data_s
{
   /**
    * Determines how a characteristic value is formatted.
    */
   uint8_t  Format;

   /**
    * Used with interger data types to determine how the characteristic
    * value is further formatted. The actual value = (characteristic
    * value * 10^Exponent).
    */
   uint8_t  Exponent;

   /**
    * Unit of measurement for the characteristic value.
    */
   uint16_t Unit;

   /**
    * Organization that is responsible for defining the
    * enumerations for the description field.
    */
   uint8_t  NameSpace;

   /**
    * Enumerated value that is used to uniquely identitfy a
    * characteristic value
    */
   uint16_t Description;
} qapi_BLE_BAS_Presentation_Format_Data_t;

#define QAPI_BLE_BAS_PRESENTATION_FORMAT_DATA_SIZE               (sizeof(qapi_BLE_BAS_Presentation_Format_Data_t))
/**< Size of the #qapi_BLE_BAS_Presentation_Format_Data_t structure. */

/**
 * Enumeration of the valid Read Request types that a server
 * may receive in a
 * QAPI_BLE_ET_BAS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E or
 * QAPI_BLE_ET_BAS_SERVER_CLIENT_CONFIGURATION_UPDATE_E event.
 *
 * For each event it is up to the application to return (or write) the
 * correct Client Configuration descriptor based on this value.
 */
typedef enum qapi_BLE_BAS_Characteristic_Type_s
{
   QAPI_BLE_CT_BATTERY_LEVEL_E /**< Battery level characteristic. */
} qapi_BLE_BAS_Characteristic_Type_t;

/**
 * Enumeration of all events generated by the BAS
 * profile. These are used to determine the type of each event
 * generated and to ensure the proper union element is accessed for the
 * #qapi_BLE_BAS_Event_Data_t structure.
 */
typedef enum qapi_BLE_BAS_Event_Type_s
{
   QAPI_BLE_ET_BAS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_ET_BAS_SERVER_CLIENT_CONFIGURATION_UPDATE_E,
   /**< Write CCCD request event. */
   QAPI_BLE_ET_BAS_SERVER_READ_BATTERY_LEVEL_REQUEST_E
   /**< Read Battery Level request event. */
} qapi_BLE_BAS_Event_Type_t;

/**
 * Structure that contains the handles that must be cached by a
 * BAS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_BAS_Client_Information_s
{
   /**
    * BAS Battery Level attribute handle.
    */
   uint16_t Battery_Level;

   /**
    * BAS Battery Level CCCD attribute handle.
    */
   uint16_t Battery_Level_Client_Configuration;

   /**
    * BAS Battery Level Presentation Format Descriptor attribute
    * handle.
    */
   uint16_t Battery_Level_Presentation_Format;
} qapi_BLE_BAS_Client_Information_t;

#define QAPI_BLE_BAS_CLIENT_INFORMATION_DATA_SIZE                (sizeof(qapi_BLE_BAS_Client_Information_t))
/**< Size of the #qapi_BLE_BAS_Client_Information_t structure. */

/**
 * Structure that contains all of the per client data is to
 * be stored by a BAS Server.
 */
typedef struct qapi_BLE_BAS_Server_Information_s
{
   /**
    * BAS Battery Level CCCD.
    */
   uint16_t Battery_Level_Client_Configuration;
} qapi_BLE_BAS_Server_Information_t;

#define QAPI_BLE_BAS_SERVER_INFORMATION_DATA_SIZE                (sizeof(qapi_BLE_BAS_Server_Information_t))
/**< Size of the #qapi_BLE_BAS_Server_Information_t structure. */

/**
 * Structure that represents the format for the data that is dispatched
 * to a BAS server when a BAS client has sent a request to read a BAS
 * characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_BAS_Read_Client_Configuration_Response() function.
 */
typedef struct qapi_BLE_BAS_Read_Client_Configuration_Data_s
{
   /**
    * BAS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the BAS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the BAS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the BAS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the BAS characteristic
    * type.
    */
   qapi_BLE_BAS_Characteristic_Type_t ClientConfigurationType;
} qapi_BLE_BAS_Read_Client_Configuration_Data_t;

#define QAPI_BLE_BAS_READ_CLIENT_CONFIGURATION_DATA_SIZE         (sizeof(qapi_BLE_BAS_Read_Client_Configuration_Data_t))
/**<
 * Size of the #qapi_BLE_BAS_Read_Client_Configuration_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a BAS server when a BAS client has sent a request to write a BAS
 * characteristic's CCCD.
 */
typedef struct qapi_BLE_BAS_Client_Configuration_Update_Data_s
{
   /**
    * BAS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the BAS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the BAS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the BAS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the BAS characteristic
    * type.
    */
   qapi_BLE_BAS_Characteristic_Type_t ClientConfigurationType;

   /**
    * Flags if notifications have been enabled for the requested CCCD.
    */
   boolean_t                          Notify;
} qapi_BLE_BAS_Client_Configuration_Update_Data_t;

#define QAPI_BLE_BAS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE       (sizeof(qapi_BLE_BAS_Client_Configuration_Update_Data_t))
/**<
 * Size of the #qapi_BLE_BAS_Client_Configuration_Update_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a BAS server when a BAS client has sent a request to read the BAS
 * Battery Level characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the qapi_BLE_BAS_Battery_Level_Read_Request_Response()
 * or qapi_BLE_BAS_Battery_Level_Read_Request_Error_Response() function.
 */
typedef struct qapi_BLE_BAS_Read_Battery_Level_Data_s
{
   /**
    * BAS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the BAS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the BAS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the BAS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_BAS_Read_Battery_Level_Data_t;

#define QAPI_BLE_BAS_READ_BATTERY_LEVEL_DATA_SIZE                 (sizeof(qapi_BLE_BAS_Read_Battery_Level_Data_t))
/**<
 * Size of the #qapi_BLE_BAS_Read_Battery_Level_Data_t
 * structure.
 */

/**
 * Structure the represents the container structure for holding all the
 * event data for a BAS instance.
 */
typedef struct qapi_BLE_BAS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_BAS_Event_Type_t  Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                   Event_Data_Size;
   union
   {
      /**
       * BAS Read Client Configuration data.
       */
      qapi_BLE_BAS_Read_Client_Configuration_Data_t   *BAS_Read_Client_Configuration_Data;

      /**
       * BAS client Configuration Update data.
       */
      qapi_BLE_BAS_Client_Configuration_Update_Data_t *BAS_Client_Configuration_Update_Data;

      /**
       * ANS Read Battery Level data.
       */
      qapi_BLE_BAS_Read_Battery_Level_Data_t          *BAS_Read_Battery_Level_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_BAS_Event_Data_t;

#define QAPI_BLE_BAS_EVENT_DATA_SIZE                             (sizeof(qapi_BLE_BAS_Event_Data_t))
/**< Size of the #qapi_BLE_BAS_Event_Data_t structure. */

/**
 * @brief
 * This declared type represents the prototype function for
 * a BAS Profile Event Receive Data Callback. This function will be called
 * whenever a BAS Profile Event occurs that is associated with the
 * specified Bluetooth stack ID.

 * @details
 * The caller can use the contents of the BAS Profile Event Data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, the callback function
 * must copy the data into another data buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e., this
 * function does not have be reentrant). Note, however,
 * that if the same callback is installed more than once, the
 * callbacks will be called serially. Because of this, the processing
 * in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the Thread
 * Context of a Thread that the user does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another BAS Profile Event will not be processed while this
 * function call is outstanding).
 *
 * @note1hang
 * This function must npt block and wait for events that can only be
 * satisfied by receiving other Bluetooth Stack Events. A Deadlock
 * will occur because other callbacks might not be issued while
 * this function is currently outstanding.
 *
 * @param[in]  BluetoothStackID      Unique identifier assigned to this
 *                                   Bluetooth Protocol Stack on which the
 *                                   event occurred.
 *
 * @param[in]  BAS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that was supplied
 *                                   as an input parameter from a prior
 *                                   BAS request.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_BAS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_BAS_Event_Data_t *BAS_Event_Data, uint32_t CallbackParameter);

   /* BAS Server API.                                                   */

/**
 * @brief
 * Opens a BAS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one BAS server may be open at a time, per the Bluetooth Stack ID.
 *
 * All client requests will be dispatched to the EventCallback function
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
 *                                  registered BAS service returned from the
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of the BAS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BAS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BAS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BAS_ERROR_MAXIMUM_NUMBER_OF_INSTANCES_REACHED \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BAS_Initialize_Service(uint32_t BluetoothStackID, qapi_BLE_BAS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Opens a BAS server on a specified Bluetooth Stack with
 * the ability to control the location of the service in the GATT database.
 *
 * @details
 * Only one BAS server may be open at a time, per the Bluetooth Stack ID.
 *
 * All client requests will be dispatched to the EventCallback function
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
 *                                       registered BAS service returned
 *                                       from the
 *                                       qapi_BLE_GATT_Register_Service()
 *                                       API.
 *
 * @param[in,out]  ServiceHandleRange    Pointer to a Service Handle
 *                                       Range structure, that on input
 *                                       can be used to control the
 *                                       location of the service in the
 *                                       GATT database, and on output
 *                                       returns the handle range that
 *                                       the service is using in the GATT
 *                                       database.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of the BAS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BAS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BAS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BAS_ERROR_MAXIMUM_NUMBER_OF_INSTANCES_REACHED \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BAS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, qapi_BLE_BAS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a BAS service instance.
 *
 * @details
 * After this function is called, no other BAS service function can be
 * called until after a successful call to the qapi_BLE_BAS_Initialize_Service()
 * function is performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_BAS_Initialize_Service()
 *                                 function.
 *
 * @return      Zero if successful.

 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BAS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BAS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BAS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the BAS service that is registered with a
 * call to qapi_BLE_BAS_Initialize_Service() or
 * qapi_BLE_BAS_Initialize_Service_Handle_Range().
 *
 * @return   Positive, nonzero number of attributes that can be
 *           registered by a BAS service instance. \n
 *           Zero on failure.
 */
QAPI_BLE_DECLARATION uint32_t QAPI_BLE_BTPSAPI qapi_BLE_BAS_Query_Number_Attributes(void);

/**
 * @brief
 * Allows a mechanism for a BAS service to
 * successfully respond to a received read client configuration request.
 *
 * @details
 * Possible events: \n
 *    QAPI_BLE_ET_BAS_SERVER_READ_BATTERY_LEVEL_REQUEST_E
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  TransactionID       Transaction ID of the original read
 *                                 request. This value was received in
 *                                 the
 *                                 QAPI_BLE_ET_BAS_SERVER_READ_BATTERY_LEVEL_REQUEST_E
 *                                 event.
 *
 * @param[in]  BatteryLevel        Battery level value to send to the
 *                                 remote device.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BAS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_TRANSACTION_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BAS_Battery_Level_Read_Request_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t BatteryLevel);

/**
 * @brief
 * Responds to BAS Read Battery Level
 * Request when an error occurred.
 *
 * @details
 * Possible events: \n
 *    QAPI_BLE_ET_BAS_SERVER_READ_BATTERY_LEVEL_REQUEST_E
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  TransactionID       Transaction ID of the original read
 *                                 request. This value was received in
 *                                 the
 *                                 QAPI_BLE_ET_BAS_SERVER_READ_BATTERY_LEVEL_REQUEST_E
 *                                 event.
 *
 * @param[in]  ErrorCode          Error code that occurred during the read operation.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BAS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_TRANSACTION_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BAS_Battery_Level_Read_Request_Error_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Gets the presentation format of the
 * device battery level on the specified BAS instance. It is supported
 * only when multiple service instances exist.
 *
 * @param[in]   BluetoothStackID                    Unique identifier
 *                                                  assigned to this
 *                                                  Bluetooth Protocol
 *                                                  Stack via a call to
 *                                                  qapi_BLE_BSC_Initialize().
 *
 * @param[in]   InstanceID                          Service instance
 *                                                  ID to close. This is
 *                                                  the value that was
 *                                                  returned from the
 *                                                  qapi_BLE_BAS_Initialize_Service()
 *                                                  function.
 *
 * @param[out]  CharacteristicPresentationFormat    Pointer to store
 *                                                  the Battery Level
 *                                                  presentation format
 *                                                  of the specified BAS
 *                                                  instance
 *
 * @return       Zero if successful.
 *
 * @return       An error code if negative; one of the following values:
 *               @par
 *                  QAPI_BLE_BAS_ERROR_INVALID_PARAMETER \n
 *                  QAPI_BLE_BAS_ERROR_INVALID_INSTANCE_ID \n
 *                  QAPI_BLE_BTPS_ERROR_FEATURE_NOT_AVAILABLE
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BAS_Query_Characteristic_Presentation_Format(uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_BAS_Presentation_Format_Data_t *CharacteristicPresentationFormat);

/**
 * @brief
 * Parses a value received from a remote
 * BAS server, interpreting it as characteristic presentation format of
 * Battery Level.
 *
 * It is supported only when multiple service instances exists.
 *
 * @param[in]  BluetoothStackID                    Unique identifier
 *                                                 assigned to this
 *                                                 Bluetooth Protocol
 *                                                 Stack via a call to
 *                                                 qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID                          Service instance
 *                                                 ID to close. This is
 *                                                 the value that was
 *                                                 returned from the
 *                                                 qapi_BLE_BAS_Initialize_Service()
 *                                                 function.
 *
 * @param[in]  CharacteristicPresentationFormat    Pointer to store the
 *                                                 Battery Level
 *                                                 presentation format of
 *                                                 the specified BAS
 *                                                 instance
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BAS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BAS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BTPS_ERROR_FEATURE_NOT_AVAILABLE
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BAS_Set_Characteristic_Presentation_Format(uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_BAS_Presentation_Format_Data_t *CharacteristicPresentationFormat);

/**
 * @brief
 * Allows a mechanism for a BAS service to
 * successfully respond to a received read client configuration request.
 *
 * @details
 * Possible events: \n
 *    QAPI_BLE_ET_BAS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E
 *
 * @param[in]  BluetoothStackID        Unique identifier assigned to this
 *                                     Bluetooth Protocol Stack via a
 *                                     call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID              Service instance ID to close.
 *                                     This is the value that was returned
 *                                     from the qapi_BLE_BAS_Initialize_Service()
 *                                     function.
 *
 * @param[in]  TransactionID           Transaction ID of the original
 *                                     read request. This value was
 *                                     received in the
 *                                     QAPI_BLE_ET_BAS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E
 *                                     event.
 *
 * @param[in]  Client_Configuration    Specifies a client characteristic
 *                                     configuration descriptor to send
 *                                     to a remote device.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BAS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BAS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BAS_Read_Client_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t Client_Configuration);

/**
 * @brief
 * Sends a Battery Level Status
 * notification to a specified remote device.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_BAS_Initialize_Service()
 *                                 function.
 *
 * @param[in]  ConnectionID        ConnectionID of the remote device to which
 *                                 to send the notification.
 *
 * @param[in]  BatteryLevel        Battery level value to send to the
 *                                 remote device.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BAS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BAS_ERROR_UNKNOWN_ERROR \n
 *                 QAPI_BLE_BAS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_HANDLE_VALUE \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_CONNECTION_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BAS_Notify_Battery_Level(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint8_t BatteryLevel);

   /* BAS Client API                                                    */

/**
 * @brief
 * Parses a value received from a remote
 * BAS server, interpreting it as characteristic presentation format of
 * Battery Level. It is supported only when multiple service instances
 * exists.
 *
 * @param[in]   ValueLength                         Specifies the length
 *                                                  of the value returned
 *                                                  by the remote BAS
 *                                                  server.
 *
 * @param[in]   Value                               Pointer to
 *                                                  the data returned by
 *                                                  the remote BAS
 *                                                  server.
 *
 * @param[out]  CharacteristicPresentationFormat    Pointer to store
 *                                                  the parsed Battery
 *                                                  Level presentation
 *                                                  format of the
 *                                                  specified BAS
 *                                                  instance.
 *
 * @return       Zero if successful.
 *
 * @return       An error code if negative; one of the following values:
 *               @par
 *                  QAPI_BLE_BAS_ERROR_INVALID_PARAMETER \n
 *                  QAPI_BLE_BTPS_ERROR_FEATURE_NOT_AVAILABLE
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BAS_Decode_Characteristic_Presentation_Format(uint32_t ValueLength, uint8_t *Value, qapi_BLE_BAS_Presentation_Format_Data_t *CharacteristicPresentationFormat);

/** @} */

#endif

