/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "qapi_otp_tlv.h"
#include "qurt_timer.h"    /* Timer for Throughput Calculation.         */
#include "qcli_util.h"

#include "qapi_fs.h"
#include "qapi_persist.h"
#include "qapi_ble_errors.h"

#include "qmesh_ble_coex.h"

#include "qmesh_demo_config.h"
#include "qmesh_demo_debug.h"

#include "qmesh_types.h"
#include "qmesh.h"
#include "qmesh_ble_gap.h"
#include "qmesh_ble_gatt.h"

#include "qmesh_sched.h"
#include "qmesh_qca402x_sched.h"
#include "qmesh_hal_ifce.h"

#include "qurt_error.h"
#include "qurt_thread.h"
#include "qmesh_private.h"

#include "qmesh_gw_debug.h"

   /* Demo Constants.                                                   */
#define QAPI_BLE_COMPARE_QMCT_SERVICE_UUID_TO_UUID_16(_x)    QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x12, 0x34)
#define QAPI_BLE_COMPARE_QMCT_PRIM_SRVC_UUID_TO_UUID_16(_x)    QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x11, 0x11)
#define QMCT_THREAD_STACK_SIZE                           (1024)
#define QMCT_MESH_THREAD_STACK_SIZE                      (4*1024)
#define QMCT_THREAD_PRIORITY                             (10)

   /* Some MACROs for accessing little-endian unaligned values.         */
#define READ_UNALIGNED_BYTE_LITTLE_ENDIAN(_x)  (((uint8_t *)(_x))[0])
#define READ_UNALIGNED_WORD_LITTLE_ENDIAN(_x)  ((uint16_t)((((uint16_t)(((uint8_t *)(_x))[1])) << 8) | ((uint16_t)(((uint8_t *)(_x))[0]))))

#define ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(_x, _y)        \
{                                                                       \
  ((uint8_t *)(_x))[0] = ((uint8_t)(((uint16_t)(_y)) & 0xFF));          \
  ((uint8_t *)(_x))[1] = ((uint8_t)((((uint16_t)(_y)) >> 8) & 0xFF));   \
}

#define ASSIGN_COEX_SERVICE_UUID_16(_x)                 QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x12, 0x34)

#define CONVERT_TO_BASEBAND_SLOTS(_x)                             ((unsigned long)((((8000L * ((unsigned long)(_x))) / 500L) + 5L)/10L))

   /* Determine the Name we will use for this compilation.              */
#define DEVICE_FRIENDLY_NAME                       "ble-coex"

   /* The following MACRO is used to convert an ASCII character into the*/
   /* equivalent decimal value.  The MACRO converts lower case          */
   /* characters to upper case before the conversion.                   */
#define ToInt(_x)                                  (((_x) > 0x39)?(((_x) & ~0x20)-0x37):((_x)-0x30))

   /* Generic Access Profile (GAP) Constants.                           */

#define DEFAULT_IO_CAPABILITY      (QAPI_BLE_LIC_NO_INPUT_NO_OUTPUT_E)
                                                         /* Denotes the       */
                                                         /* default I/O       */
                                                         /* Capability that is*/
                                                         /* used with Pairing.*/

#define DEFAULT_MITM_PROTECTION                  (TRUE)  /* Denotes the       */
                                                         /* default value used*/
                                                         /* for Man in the    */
                                                         /* Middle (MITM)     */
                                                         /* protection used   */
                                                         /* with Secure Simple*/
                                                         /* Pairing.          */

#define DEFAULT_SECURE_CONNECTIONS               (TRUE)  /* Denotes the       */
                                                         /* default value used*/
                                                         /* for Secure        */
                                                         /* Connections used  */
                                                         /* with Secure Simple*/
                                                         /* Pairing.          */


   /* The following MACRO is used to calculate if an ASCII              */
   /* characteristic is valid.                                          */
#define CHECK_ASCII_VALID(_x)                         (((_x) >= ' ') && ((_x) <= '~'))

   /* Generic Access Profile (GAP) structures.                          */

   /* Structure used to hold all of the GAP LE Parameters.              */
typedef struct _tagGAPLE_Parameters_t
{
   qapi_BLE_GAP_LE_Connectability_Mode_t ConnectableMode;
   qapi_BLE_GAP_Discoverability_Mode_t   DiscoverabilityMode;
   qapi_BLE_GAP_LE_IO_Capability_t       IOCapability;
   boolean_t                             MITMProtection;
   boolean_t                             SecureConnections;
   boolean_t                             OOBDataPresent;
} GAPLE_Parameters_t;

#define GAPLE_PARAMETERS_DATA_SIZE                       (sizeof(GAPLE_Parameters_t))


   /* Generic Access Profile (GAPLE) Internal Variables.                */
static qapi_BLE_BD_ADDR_t  LocalBD_ADDR;            /* Holds the BD_ADDR of the        */
                                                    /* local device.                   */

static qapi_BLE_Encryption_Key_t ER = {0x28, 0xBA, 0xE1, 0x37, 0x13, 0xB2, 0x20, 0x45, 0x16, 0xB2, 0x19, 0xD0, 0x80, 0xEE, 0x4A, 0x51};
                                                    /* The Encryption Root Key should  */
                                                    /* be generated in such a way as   */
                                                    /* to guarantee 128 bits of        */
                                                    /* entropy.                        */

static qapi_BLE_Encryption_Key_t IR = {0x41, 0x09, 0xA0, 0x88, 0x09, 0x6B, 0x70, 0xC0, 0x95, 0x23, 0x3C, 0x8C, 0x48, 0xFC, 0xC9, 0xFE};
                                                    /* The Identity Root Key should    */
                                                    /* be generated in such a way as   */
                                                    /* to guarantee 128 bits of        */
                                                    /* entropy.                        */

static qapi_BLE_Encryption_Key_t DHK;               /* The DHK key can be              */
                                                    /* regenerated on the fly using the*/
                                                    /* constant IR and ER keys and     */
                                                    /* are used globally, for all      */
                                                    /* devices.                        */

static qapi_BLE_Encryption_Key_t IRK;               /* The IRK key can be              */
                                                    /* regenerated on the fly using the*/
                                                    /* constant IR and ER keys and     */
                                                    /* are used globally, for all      */
                                                    /* devices.                        */


   /* The following bit mask values may be used for the Flags field of  */
   /* the BLEParameters_t structure.                                    */
#define BLE_PARAMETERS_FLAGS_ADVERTISING_PARAMETERS_VALID   0x00000001
#define BLE_PARAMETERS_FLAGS_SCAN_PARAMETERS_VALID          0x00000002
#define BLE_PARAMETERS_FLAGS_CONNECTION_PARAMETERS_VALID    0x00000004

   /* The following structure is used to hold the Scan Window and       */
   /* Interval parameters for LE Scanning.                              */
typedef struct _tagBLEScanParameters_t
{
   uint16_t ScanInterval;
   uint16_t ScanWindow;
} BLEScanParameters_t;

   /* The following structure is used to hold information on the        */
   /* configured Scan/Advertising/Connection Parameters.                */
typedef struct _tagBLEParameters_t
{
   unsigned long                            Flags;
   qapi_BLE_GAP_LE_Advertising_Parameters_t AdvertisingParameters;
   qapi_BLE_GAP_LE_Connection_Parameters_t  ConnectionParameters;
   BLEScanParameters_t                      ScanParameters;
} BLEParameters_t;


#define  QMCTBufferLength    256

static uint8_t QMCTBuffer[QMCTBufferLength + 1];

#define  MAX_NUM_OF_COEX_TIMERS    5

#define  MAX_NUM_OF_DEVICES    5

static qapi_BLE_BD_ADDR_t QMCT_BD_ADDR [MAX_NUM_OF_DEVICES];

#define  MESH_PKT_SEND_INTERVAL_MIN    50     /* 50 ms */

static int QMCTMeshSendInterval;

static int QMCTMeshSendElementAddr;

static int QMCTMeshSrcElementAddr;

static int QMCTMeshPktTTL;

static int QMCTMeshDuration;

static int QMCTMeshMsgCount;

static bool QMCTMeshRunning;

static int QMCTConnInterval;

static int QMCTWriteInterval;

static int QMCTGattDuration;

static int QMCTConnectionCount;

static int DeviceStatus;

static bool QMCTGattRunning;

#define GATT_PKT_MAX_QUEUE_SIZE     5

static bool DevGattBufferFull;


   /* The following defines the structure that holds all of the DUMMY   */
   /* Characteristic Handles that need to be cached by a DUMMY Client.  */
typedef struct _tagQMCT_Client_Info_t
{
   uint16_t Tx_Characteristic;
   uint16_t Tx_Client_Configuration_Descriptor;
   /*uint16_t Rx_Characteristic;
   uint16_t Tx_Credit_Characteristic;
   uint16_t Rx_Credit_Characteristic;
   uint16_t Rx_Credit_Client_Configuration_Descriptor;*/
} QMCT_Client_Info_t;


   /* Generic Access Profile Service (GAPS) structures.                 */

   /* The following structure represents the information we will store  */
   /* on a Discovered GAP Service.                                      */
typedef struct _tagGAPS_Client_Info_t
{
   uint16_t DeviceNameHandle;
   uint16_t DeviceAppearanceHandle;
} GAPS_Client_Info_t;

   /* The following structure holds information on known Device         */
   /* Appearance Values.                                                */
typedef struct _tagGAPS_Device_Appearance_Mapping_t
{
   uint16_t  Appearance;
   char     *String;
} GAPS_Device_Appearance_Mapping_t;

   /* Remote Device Information structure.                              */

   /* The following bit mask values may be used for the Flags field of  */
   /* the DeviceInfo_t structure.                                       */
#define DEVICE_INFO_FLAGS_LTK_VALID                         0x01
#define DEVICE_INFO_FLAGS_SPPLE_SERVER                      0x02
#define DEVICE_INFO_FLAGS_SPPLE_CLIENT                      0x04
#define DEVICE_INFO_FLAGS_SERVICE_DISCOVERY_OUTSTANDING     0x08
#define DEVICE_INFO_FLAGS_IRK_VALID                         0x10
#define DEVICE_INFO_FLAGS_ADDED_TO_WHITE_LIST               0x20
#define DEVICE_INFO_FLAGS_ADDED_TO_RESOLVING_LIST           0x40




/* The following structure is used to track the sending and receiving*/
/* of data for the throughput test.                                  */
typedef struct _tagXferInfo_t
{
   uint64_t  RxCount;
   boolean_t TimingStarted;
   uint64_t  FirstTime;
   uint64_t  LastTime;
} XferInfo_t;

   /* The following structure holds the information that needs to be    */
   /* stored for a connected remote device.                             */
   /* * NOTE * If the local device pairs with the remote device, then   */
   /*          the LTK MUST ve valid, and the remote device information */
   /*          MUST persist between connections.  If the local device   */
   /*          does NOT pair with the remote device, then the LTK will  */
   /*          NOT be valid, and the remote device information will be  */
   /*          deleted when the remote device is disconnected.          */
   /* * NOTE * The ConnectionID will be used to indicate that a remote  */
   /*          device is currently connected.  Otherwise it will be set */
   /*          to zero to indicate that the remote device is currently  */
   /*          disconnected.                                            */
   /* * NOTE * The SelectedRemoteBD_ADDR will correspond to the         */
   /*          RemoteAddress field of the remote device that is         */
   /*          currently connected.                                     */
typedef struct _tagDeviceInfo_t
{
   uint8_t                                Flags;
   unsigned int                           ConnectionID;
   boolean_t                              RemoteDeviceIsMaster;
   qapi_BLE_BD_ADDR_t                     RemoteAddress;
   qapi_BLE_GAP_LE_Address_Type_t         RemoteAddressType;
   qapi_BLE_GAP_LE_Address_Type_t         IdentityAddressType;
   qapi_BLE_BD_ADDR_t                     IdentityAddressBD_ADDR;
   uint8_t                                EncryptionKeySize;
   qapi_BLE_Long_Term_Key_t               LTK;
   qapi_BLE_Encryption_Key_t              IRK;
   qapi_BLE_Random_Number_t               Rand;
   uint16_t                               EDIV;
   qapi_BLE_GAP_LE_White_List_Entry_t     WhiteListEntry;
   qapi_BLE_GAP_LE_Resolving_List_Entry_t ResolvingListEntry;
   GAPS_Client_Info_t                     GAPSClientInfo;
   QMCT_Client_Info_t                     QMCTClientInfo;
   struct _tagDeviceInfo_t               *NextDeviceInfoInfoPtr;
} DeviceInfo_t;

#define DEVICE_INFO_DATA_SIZE                            (sizeof(DeviceInfo_t))

typedef struct _tagPersistentRemoteDeviceData_t
{
   uint8_t                        Flags;
   qapi_BLE_BD_ADDR_t             LastAddress;
   qapi_BLE_GAP_LE_Address_Type_t LastAddressType;
   qapi_BLE_BD_ADDR_t             IdentityAddress;
   qapi_BLE_GAP_LE_Address_Type_t IdentityAddressType;
   uint8_t                        EncryptionKeySize;
   qapi_BLE_Long_Term_Key_t       LTK;
   qapi_BLE_Encryption_Key_t      IRK;
} PersistentRemoteDeviceData_t;

#define PERSISTENT_REMOTE_DEVICE_DATA_SIZE               (sizeof(PersistentRemoteDeviceData_t))

#define PERSISTENT_REMOTE_DEVICE_DATA_FLAG_LTK_VALID        0x01
#define PERSISTENT_REMOTE_DEVICE_DATA_FLAG_IDENTITY_VALID   0x02

typedef struct _tagPersistentData_t
{
   qapi_BLE_BD_ADDR_t LocalAddress;
   uint8_t NumberRemoteDevices;
   PersistentRemoteDeviceData_t RemoteDevices[1];
} PersistentData_t;

#define PERSISTENT_DATA_SIZE(_x)                         (QAPI_BLE_BTPS_STRUCTURE_OFFSET(PersistentData_t, RemoteDevices) + (PERSISTENT_REMOTE_DEVICE_DATA_SIZE * (_x)))


QMESH_TIMER_GROUP_HANDLE_T coex_timer_ghdl = NULL;


   /* Internal Variables to this Module (Remember that all variables    */
   /* declared static are initialized to 0 automatically by the         */
   /* compiler as part of standard C/C++).                              */

QCLI_Group_Handle_t   mesh_coex_group;          /* Handle for our main QCLI Command*/
                                                    /* Group.                          */

static uint32_t            BluetoothStackID;        /* Variable which holds the Handle */
                                                    /* of the opened Bluetooth Protocol*/
                                                    /* Stack.                          */

static boolean_t           CoExBluetoothInit;        /* Flag which holds Bluetooth */
                                                    /* Initialization Status */

static uint32_t            ScanTimerID;             /* Scan Timer ID.                  */

static uint32_t            AdvertTimerID;           /* Advert Timer ID.                */

/* GATT Server Implementaion */
/* CoEx Service Internal Variables */
static uint32_t            CoExInstanceID;          /* The following holds the CoEx    */
                                                    /* Instance ID.                    */

#define MAX_NUM_OF_COEX_GATT_CONN    5

typedef struct
{
    qapi_BLE_BD_ADDR_t         devAddr;     /* Holds remote device BD_ADDR*/
    uint32_t                   connectionId;   /* Holds Proxy/Provisioning service connection ID*/
    uint8_t                    mtu;            /* Holds Proxy/Provisioning service MTU size */
    bool                       notifyEnabled;  /* Holds Proxy/Provisioning service Notification Status */
    uint8_t                    ccdHndlValue[2]; /* Holds Proxy/Provisioning service CCD Handle Value*/
} COEX_GATT_DEVICE_INFO_T;

typedef struct
{
    qapi_BLE_GATT_Attribute_Handle_Group_t  hndleDetails;   /*Holds registered service handle details*/
    int32_t                                 srvcId;         /*Holds Proxy/Provisioning service ID*/
    COEX_GATT_DEVICE_INFO_T                 devInfo[MAX_NUM_OF_COEX_GATT_CONN];     /*Hold remote device information*/
} COEX_GATT_SERVER_DATA_T;

COEX_GATT_SERVER_DATA_T *coexGATTServerData;

/* CoEx Service UUID */
#define COEX_SERVICE_UUID                      {0x34, 0x12}
/* CoEx Service Characteristic UUIDS */
#define COEX_DATA_IN_UUID                      {0x11, 0x11}
#define COEX_DATA_OUT_UUID                     {0x12, 0x11}

/* CoEx Service Declaration                                */
static const qapi_BLE_GATT_Primary_Service_16_Entry_t CoEx_Service_UUID =
{
   COEX_SERVICE_UUID
} ;

/* CoEx Data In Characteristic Declaration.            */
static const qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t CoEx_Data_In =
{
    (QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_READ|QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_WRITE_WITHOUT_RESPONSE),
    COEX_DATA_IN_UUID,
} ;


/* CoEx Data In Out Characteristic Value.                  */
static const qapi_BLE_GATT_Characteristic_Value_16_Entry_t  CoEx_Data_In_Value =
{
   COEX_DATA_IN_UUID,
   0,
   NULL
} ;

/* CoEx Data Out Characteristic Declaration.                        */
static const qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t CoEx_Data_Out =
{
   (QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_READ|QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_NOTIFY),
   COEX_DATA_OUT_UUID
} ;

/* CoEx Data Out Characteristic Value.                              */
static const qapi_BLE_GATT_Characteristic_Value_16_Entry_t CoEx_Data_Out_Value =
{
   COEX_DATA_OUT_UUID,
   0,
   NULL
} ;

/* Client Characteristic Configuration Descriptor.                   */
static qapi_BLE_GATT_Characteristic_Descriptor_16_Entry_t Client_Characteristic_Configuration =
{
   QAPI_BLE_GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_BLUETOOTH_UUID_CONSTANT,
   QAPI_BLE_GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH,
   NULL
};

   /* The following defines the Mesh Proxy Service that is registered with   */
   /* the GATT_Register_Service function call.                          */
   /* * NOTE * This array will be registered with GATT in the call to   */
   /*          GATT_Register_Service.                                   */
const qapi_BLE_GATT_Service_Attribute_Entry_t CoEx_Service[] =
{
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE,          QAPI_BLE_AET_PRIMARY_SERVICE_16_E,            (uint8_t *)&CoEx_Service_UUID             },
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE,          QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_16_E, (uint8_t *)&CoEx_Data_In                  },
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, QAPI_BLE_AET_CHARACTERISTIC_VALUE_16_E,       (uint8_t *)&CoEx_Data_In_Value            },
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE,          QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_16_E, (uint8_t *)&CoEx_Data_Out                 },
   { 0,                                               QAPI_BLE_AET_CHARACTERISTIC_VALUE_16_E,       (uint8_t *)&CoEx_Data_Out_Value           },
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, QAPI_BLE_AET_CHARACTERISTIC_DESCRIPTOR_16_E,  (uint8_t *)&Client_Characteristic_Configuration }
} ;

#define COEX_SERVICE_ATTR_COUNT               (sizeof(CoEx_Service)/sizeof(qapi_BLE_GATT_Service_Attribute_Entry_t))

#define COEX_DATA_IN_CHAR_ATT_OFFSET            2
#define COEX_DATA_OUT_CHAR_ATT_OFFSET           4
#define COEX_DATA_OUT_CHAR_CCD_ATT_OFFSET       5

#define CHAR_CCD_WRITE_VALUE_LENGTH             2


static unsigned int        ConnectionCount;         /* Holds the number of connected   */
                                                    /* remote devices.                 */

static DeviceInfo_t       *DeviceInfoList;          /* Holds the list head for the     */
                                                    /* remote device info list         */

typedef char               BoardStr_t[16];          /* User to represent a structure to*/
                                                    /* hold a BD_ADDR return from      */
                                                    /* BD_ADDRToStr.                   */

   /* Generic Access Profile (GAPLE) Internal Variables.                */

static BLEParameters_t     BLEParameters;           /* Variable which is used to hold  */
                                                    /* the BLE Scan/Advertising/...    */
                                                    /* Connection Parameters that have */
                                                    /* been configured at the CLI.     */

static qapi_BLE_BD_ADDR_t  SelectedRemoteBD_ADDR;   /* Holds the BD_ADDR of the        */
                                                    /* connected remote device that is */
                                                    /* currently selected.             */

static boolean_t           ScanInProgress;          /* A boolean flag to show if a scan*/
                                                    /* is in process                   */

static boolean_t           LocalDeviceIsMaster;     /* Variable which indicates if the */
                                                    /* local device is the master      */
                                                    /* of the connection.              */

static qapi_BLE_GAP_LE_Address_Type_t  RemoteAddressType;
                                                    /* Variable which holds the remote */
                                                    /* address type for a connected    */
                                                    /* remote device until it can be   */
                                                    /* stored in the remote device     */
                                                    /* information.                    */


   /* Generic Access Profile Service (GAPS) Internal Variables.         */
static QCLI_Group_Handle_t gaps_group;              /* Handle for our GAPS Command     */
                                                    /* Group.                          */

static boolean_t           DisplayAdvertisingEventData; /* Flag to indicate if we      */
                                                    /* should have verbose adv report  */
                                                    /* outputs.                        */

   /* The following is used to map from ATT Error Codes to a printable  */
   /* string.                                                           */
static char *ErrorCodeStr[] =
{
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_NO_ERROR",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_INVALID_HANDLE",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_READ_NOT_PERMITTED",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_WRITE_NOT_PERMITTED",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_INVALID_PDU",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_INSUFFICIENT_AUTHENTICATION",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_REQUEST_NOT_SUPPORTED",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_INVALID_OFFSET",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_INSUFFICIENT_AUTHORIZATION",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_PREPARE_QUEUE_FULL",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_ATTRIBUTE_NOT_FOUND",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_ATTRIBUTE_NOT_LONG",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_INSUFFICIENT_ENCRYPTION_KEY_SIZE",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_INVALID_ATTRIBUTE_VALUE_LENGTH",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_INSUFFICIENT_ENCRYPTION",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_UNSUPPORTED_GROUP_TYPE",
   "QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_INSUFFICIENT_RESOURCES"
} ;

#define NUMBER_OF_ERROR_CODES     (sizeof(ErrorCodeStr)/sizeof(char *))

   /* The following array is used to map Device Appearance Values to    */
   /* strings.                                                          */
static GAPS_Device_Appearance_Mapping_t AppearanceMappings[] =
{
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_UNKNOWN,                        "Unknown"                   },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_PHONE,                  "Generic Phone"             },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_COMPUTER,               "Generic Computer"          },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_WATCH,                  "Generic Watch"             },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_SPORTS_WATCH,                   "Sports Watch"              },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_CLOCK,                  "Generic Clock"             },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_DISPLAY,                "Generic Display"           },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_GENERIC_REMOTE_CONTROL, "Generic Remote Control"    },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_EYE_GLASSES,            "Eye Glasses"               },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_TAG,                    "Generic Tag"               },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_KEYRING,                "Generic Keyring"           },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_MEDIA_PLAYER,           "Generic Media Player"      },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_BARCODE_SCANNER,        "Generic Barcode Scanner"   },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_THERMOMETER,            "Generic Thermometer"       },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_THERMOMETER_EAR,                "Ear Thermometer"           },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_HEART_RATE_SENSOR,      "Generic Heart Rate Sensor" },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_BELT_HEART_RATE_SENSOR,         "Belt Heart Rate Sensor"    },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_BLOOD_PRESSURE,         "Generic Blood Pressure"    },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_BLOOD_PRESSURE_ARM,             "Blood Pressure: ARM"       },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_BLOOD_PRESSURE_WRIST,           "Blood Pressure: Wrist"     },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_HUMAN_INTERFACE_DEVICE,         "Human Interface Device"    },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_HID_KEYBOARD,                   "HID Keyboard"              },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_HID_MOUSE,                      "HID Mouse"                 },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_HID_JOYSTICK,                   "HID Joystick"              },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_HID_GAMEPAD,                    "HID Gamepad"               },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_HID_DIGITIZER_TABLET,           "HID Digitizer Tablet"      },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_HID_CARD_READER,                "HID Card Reader"           },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_HID_DIGITAL_PEN,                "HID Digitizer Pen"         },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_HID_BARCODE_SCANNER,            "HID Bardcode Scanner"      },
   { QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_GLUCOSE_METER,          "Generic Glucose Meter"     }
} ;

#define NUMBER_OF_APPEARANCE_MAPPINGS     (sizeof(AppearanceMappings)/sizeof(GAPS_Device_Appearance_Mapping_t))

   /* This function is used to register the Qmesh CoEx Command Group with QCLI */
void Initialize_Qmesh_CoEx(void);

static int32_t OpenStack(qapi_BLE_HCI_DriverInformation_t *HCI_DriverInformation);
static boolean_t CloseStack(void);
static void GenerateRandomKeys(void);

   /* The following function is responsible for converting the specified string    */
   /* into data of type BD_ADDR.  The first parameter of this function  */
   /* is the BD_ADDR string to be converted to a BD_ADDR.  The second   */
   /* parameter of this function is a pointer to the BD_ADDR in which   */
   /* the converted BD_ADDR String is to be stored.                     */
static void StrToBD_ADDR(char *BoardStr, qapi_BLE_BD_ADDR_t *Board_Address);

   /* Demo helper functions.                                            */
static DeviceInfo_t *CreateNewDeviceInfoEntry(DeviceInfo_t **ListHead, qapi_BLE_BD_ADDR_t RemoteAddress);
static DeviceInfo_t *SearchDeviceInfoEntryByBD_ADDR(DeviceInfo_t **ListHead, qapi_BLE_BD_ADDR_t RemoteAddress);
static DeviceInfo_t *SearchDeviceInfoEntryByConnectionID(DeviceInfo_t **ListHead, unsigned int ConnectionID);
static DeviceInfo_t *DeleteDeviceInfoEntry(DeviceInfo_t **ListHead, qapi_BLE_BD_ADDR_t RemoteAddress);


static void BD_ADDRToStr(qapi_BLE_BD_ADDR_t Board_Address, BoardStr_t BoardStr);
static unsigned int StringToUnsignedInteger(char *StringInteger);

static QCLI_Command_Status_t InitializeCoExBluetooth(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t DeInitializeCoExBluetooth(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t GetLocalAddress(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t AdvertiseLE(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t ScanLE(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t ConnectLE(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t DisconnectLE(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t CancelConnectLE(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t RegisterCoExService(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t DeregisterCoExService(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t SelectRemoteDevice(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t DisplayRemoteDevices(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t SendMeshPackets(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t SendMeshPacketsWithoutThread(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t SendGattPackets(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t SendMeshAndGattPackets(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t StopTestRunning(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);


   /* Generic Access Profile (GAP) helper functions.                  */
static int StartScan(uint32_t BluetoothStackID, qapi_BLE_GAP_LE_Filter_Policy_t FilterPolicy, unsigned int ScanDuration);
static int StopScan(uint32_t BluetoothStackID);
static int ConnectLEDevice(uint32_t BluetoothStackID, boolean_t UseWhiteList, qapi_BLE_BD_ADDR_t *BD_ADDR, qapi_BLE_GAP_LE_Address_Type_t AddressType);
static int DisconnectLEDevice(qapi_BLE_BD_ADDR_t BD_ADDR);
static int DiscoverLEDeviceServices(qapi_BLE_BD_ADDR_t  BD_ADDR);
static QCLI_Command_Status_t CancelLECreateConnection(void);

static void DisplayUUID(qapi_BLE_GATT_UUID_t *UUID);

/* Generic Attribute Profile (GATT) QCLI command functions.          */
static QCLI_Command_Status_t DiscoverServices(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

/* Generic Access Profile Service (GAPS) helper functions.           */
static void GAPSPopulateHandles(GAPS_Client_Info_t *ClientInfo, qapi_BLE_GATT_Service_Discovery_Indication_Data_t *ServiceInfo);
static boolean_t AppearanceToString(uint16_t Appearance, char **String);

static void QMCTPopulateHandles(QMCT_Client_Info_t *ClientInfo, qapi_BLE_GATT_Service_Discovery_Indication_Data_t *ServiceInfo);

/* BSC Timer Callback function prototype.                            */
static void BSC_Timer_Callback(uint32_t BluetoothStackID, uint32_t TimerID, uint32_t CallbackParameter);

/* Advert Timer Callback function prototype.                            */
static void Advert_Timer_Callback(uint32_t BluetoothStackID, uint32_t TimerID, uint32_t CallbackParameter);

/* Generic Access Profile (GAPLE) Event Callback function prototypes.*/
static void QAPI_BLE_BTPSAPI BLE_COEX_GAP_LE_Event_Callback(uint32_t BluetoothStackID, qapi_BLE_GAP_LE_Event_Data_t *GAP_LE_Event_Data, uint32_t CallbackParameter);


/* Generic Attribute Profile (GATT) Event Callback function  prototypes        */
extern void QAPI_BLE_BTPSAPI BLE_COEX_GATT_Connection_Event_Callback(uint32_t BluetoothStackID, qapi_BLE_GATT_Connection_Event_Data_t *GATT_Connection_Event_Data, uint32_t CallbackParameter);
static void QAPI_BLE_BTPSAPI BLE_COEX_GATT_Service_Discovery_Event_Callback(uint32_t BluetoothStackID, qapi_BLE_GATT_Service_Discovery_Event_Data_t *GATT_Service_Discovery_Event_Data, uint32_t CallbackParameter);


/* Generic Attribute Profile (GATT) Service Event Callback function prototypes (Server) */
static void QAPI_BLE_BTPSAPI GATT_ClientEventCallback_GAPS(uint32_t BluetoothStackID, qapi_BLE_GATT_Client_Event_Data_t *GATT_Client_Event_Data, uint32_t CallbackParameter);

static void QAPI_BLE_BTPSAPI CoExGapLeEventCallback(uint32_t btStackId, 
        qapi_BLE_GAP_LE_Event_Data_t *GAP_LE_Event_Data, uint32_t CallbackParameter);

static void QAPI_BLE_BTPSAPI CoExGattServerEventCallback(uint32_t btStackID,
    qapi_BLE_GATT_Server_Event_Data_t *GattServerEventData, uint32_t CallbackParameter);

/* Generic Attribute Profile (GATT) Service Event Callback function prototypes (Client) */

/*********************************************************************/
/* qcli Command Group Definitions                                           */
/*********************************************************************/

const QCLI_Command_t qmesh_coex_cmd_list[] =
{
   // cmd_function             start_thread      cmd_string                 usage_string description
   { InitializeCoExBluetooth,        false, "InitializeCoExBluetooth",      "", "Initialize Bluetooth (for standalone testing)"},
   { DeInitializeCoExBluetooth,      false, "DeInitializeCoExBluetooth",      "", "DeInitialize Bluetooth (If initialized from CoEx)"},
   { GetLocalAddress,                false, "GetLocalAddress",               "",                              "Querys Bluetooth Address of local Bluetooth controller."                      },
   { AdvertiseLE,                    false, "AdvertiseLE",                   "[(0 = Disable, 1 = Enable)] [Duration (if enabling advert) (seconds, optional)]",     "Starts/Stops Advertising Process.[Advert Interval Min 100 ms & Advert Interval Max 200 ms]"                                          },
   { ScanLE,                         false, "ScanLE",                        "[(0 = Disable, 1 = Enable)] [Duration (if enabling scan) (seconds, optional)]", "Starts/Stops Scan Process."     },
   { ConnectLE,                      false, "ConnectLE",                     "[Use White List (0=False, 1=True)] [BD_ADDR (If Use White List=0)] [ADDR Type (0 = Public, 1 = Random, 2 = Public Identity, 3 = Random Identity) (If Use White List=0)] ", "Sends an LE connection request to a remote device."  },
   { DisconnectLE,                   false, "DisconnectLE",                  "",                                                                                                                                   "Disconnects active LE connection."                                            },
   { CancelConnectLE,                false, "CancelConnectLE",               "",                                                                                                                                   "Cancels active LE connection process."                                        },
   { DiscoverServices,               false, "DiscoverServices",              "",                                                                                                                                   "Starts process to discover all GATT Service on active LE connection."         },
   { RegisterCoExService,            false, "RegisterCoExService",           "",                              "Register CoEx Service."                      },
   { DeregisterCoExService,          false, "DeregisterCoExService",         "",                              "Deregister CoEx Service."                      },
   { SelectRemoteDevice,             false, "SelectRemoteDevice",            "[Connection ID]",                                                                                                                    "Selects a remote device based on the GATT Connection ID."                     },
   { DisplayRemoteDevices,           false, "DisplayRemoteDevices",          "",                                                                                                                                   "Displays information for paired devices."                                     },

   { SendMeshPackets,                false, "SendMeshPackets",             "[Duration] [DST Element Addr] [SRC Element Addr] [Mesh Send Interval (.> 50 ms)] [TTL]",                                                                                     "Send Data over Mesh for X duration " },
   { SendMeshPacketsWithoutThread,   false, "SendMeshPacketsWithoutThread", "[Duration] [DST Element Addr] [SRC Element Addr] [Mesh Send Interval (.> 50 ms)] [TTL]",                                                                                     "Send Data over Mesh for X duration " },
   { SendGattPackets,                false, "SendGattPackets",             "[Num Of Dev (max upto 5)] [BD ADDR 9as per num of dev)] [CONN INTRVL] [Duration] [Gatt Send Interval] ",                             "Send Data over multiple Gatt Connection for X duration " },
   { SendMeshAndGattPackets,         false, "SendMeshAndGattPackets",      "[Num Of Dev (max upto 5)] [BD ADDR 9as per num of dev)] [CONN INTRVL] [Duration] [Gatt Send Interval] [Element Addr] [Mesh Send Interval]",                   "Send Data over multiple Gatt Connection and  Mesh for X duration " },
   { StopTestRunning,                false, "StopTestRunning",               "",                                                                                                                                 "Stop Test Running" },


};

const QCLI_Command_Group_t qmesh_coex_cmd_group =
{
    "Qmesh CoEx",
    (sizeof(qmesh_coex_cmd_list) / sizeof(qmesh_coex_cmd_list[0])),
    qmesh_coex_cmd_list
};


static qapi_BLE_HCI_DriverInformation_t HCI_DriverInformation;
static QMESH_TIMER_HANDLE_T g_mesh_tx_dur_tmr;
static QMESH_TIMER_HANDLE_T g_mesh_tx_tmr;
MESH_DATA_T g_mesh_data;

   /* Demo helper functions.                                            */

   /* The following function will create a device information entry and */
   /* add it to the specified List.  This function, if successful, will */
   /* return a pointer to the Entry that has been created and added to  */
   /* the specified list.  This function will return NULL if NO Entry   */
   /* was added.  This can occur if the element passed in was deemed    */
   /* invalid or the actual List Head was invalid.                      */
   /* ** NOTE ** This function does not insert duplicate entries into   */
   /*            the list.  An element is considered a duplicate if the */
   /*            RemoteAddress already exists for an entry.  When this  */
   /*            occurs, this function returns NULL.                    */
static DeviceInfo_t *CreateNewDeviceInfoEntry(DeviceInfo_t **ListHead, qapi_BLE_BD_ADDR_t RemoteAddress)
{
   DeviceInfo_t *ret_val = NULL;
   boolean_t     Result;

   /* Verify that the passed in parameters seem semi-valid.             */
   if((ListHead) && (!QAPI_BLE_COMPARE_NULL_BD_ADDR(RemoteAddress)))
   {
      /* Allocate the memory for the entry.                             */
      if((ret_val = malloc(sizeof(DeviceInfo_t))) != NULL)
      {
         /* Initialize the entry.                                       */
         memset(ret_val, 0, sizeof(DeviceInfo_t));

         /* Store the remote device address.                            */
         ret_val->RemoteAddress = RemoteAddress;

         Result = qapi_BLE_BSC_AddGenericListEntry_Actual(QAPI_BLE_EK_BD_ADDR_T_E, QAPI_BLE_BTPS_STRUCTURE_OFFSET(DeviceInfo_t, RemoteAddress), QAPI_BLE_BTPS_STRUCTURE_OFFSET(DeviceInfo_t, NextDeviceInfoInfoPtr), (void **)(ListHead), (void *)(ret_val));
         if(!Result)
         {
            /* Failed to add to list so we should free the memory that  */
            /* we allocated for the entry.                              */
            free(ret_val);
         }
      }
   }

   return(ret_val);
}

   /* The following function searches the specified List for the        */
   /* specified Connection BD_ADDR.  This function returns NULL if      */
   /* either the List Head is invalid, the BD_ADDR is invalid, or the   */
   /* Connection BD_ADDR was NOT found.                                 */
static DeviceInfo_t *SearchDeviceInfoEntryByBD_ADDR(DeviceInfo_t **ListHead, qapi_BLE_BD_ADDR_t RemoteAddress)
{
   DeviceInfo_t *ret_val = NULL;
   DeviceInfo_t *DeviceInfo;

   /* Verify the list head.                                             */
   if(ListHead)
   {
      /* Loop through the device information.                           */
      DeviceInfo = *ListHead;
      while(DeviceInfo)
      {
         /* If the BD_ADDR is a match then we found the remote device   */
         /* information.                                                */
         if(QAPI_BLE_COMPARE_BD_ADDR(DeviceInfo->RemoteAddress, RemoteAddress))
         {
            /* Set the remote device information pointer to the return  */
            /* value and break since we are done.                       */
            ret_val = DeviceInfo;
            break;
         }
         DeviceInfo = DeviceInfo->NextDeviceInfoInfoPtr;
      }
   }

   return(ret_val);
}

   /* The following function searches the specified List for the        */
   /* specified GATT Connection ID.  This function returns NULL if      */
   /* either the List Head is invalid, the Connection ID is invalid, or */
   /* the Connection ID was NOT found.                                  */
static DeviceInfo_t *SearchDeviceInfoEntryByConnectionID(DeviceInfo_t **ListHead, unsigned int ConnectionID)
{
   return(qapi_BLE_BSC_SearchGenericListEntry(QAPI_BLE_EK_UNSIGNED_INTEGER_E, (void *)(&ConnectionID), QAPI_BLE_BTPS_STRUCTURE_OFFSET(DeviceInfo_t, ConnectionID), QAPI_BLE_BTPS_STRUCTURE_OFFSET(DeviceInfo_t, NextDeviceInfoInfoPtr), (void **)(ListHead)));
}

   /* The following function searches the specified Key Info List for   */
   /* the specified BD_ADDR and removes it from the List.  This function*/
   /* returns NULL if either the List Head is invalid, the BD_ADDR is   */
   /* invalid, or the specified Entry was NOT present in the list.  The */
   /* entry returned will have the Next Entry field set to NULL, and    */
   /* the caller is responsible for deleting the memory associated with */
   /* this entry by calling the FreeKeyEntryMemory() function.          */
static DeviceInfo_t *DeleteDeviceInfoEntry(DeviceInfo_t **ListHead, qapi_BLE_BD_ADDR_t RemoteAddress)
{
   return(qapi_BLE_BSC_DeleteGenericListEntry(QAPI_BLE_EK_BD_ADDR_T_E, (void *)(&RemoteAddress), QAPI_BLE_BTPS_STRUCTURE_OFFSET(DeviceInfo_t, RemoteAddress), QAPI_BLE_BTPS_STRUCTURE_OFFSET(DeviceInfo_t, NextDeviceInfoInfoPtr), (void **)(ListHead)));
}

   /* The following function is responsible for converting data of type */
   /* BD_ADDR to a string.  The first parameter of this function is the */
   /* BD_ADDR to be converted to a string.  The second parameter of this*/
   /* function is a pointer to the string in which the converted BD_ADDR*/
   /* is to be stored.                                                  */
static void BD_ADDRToStr(qapi_BLE_BD_ADDR_t Board_Address, BoardStr_t BoardStr)
{
   snprintf((char *)BoardStr, (sizeof(BoardStr_t)/sizeof(char)), "0x%02X%02X%02X%02X%02X%02X", Board_Address.BD_ADDR5, Board_Address.BD_ADDR4, Board_Address.BD_ADDR3, Board_Address.BD_ADDR2, Board_Address.BD_ADDR1, Board_Address.BD_ADDR0);
}

   /* The following function is responsible for converting number       */
   /* strings to there unsigned integer equivalent.  This function can  */
   /* handle leading and tailing white space, however it does not handle*/
   /* signed or comma delimited values.  This function takes as its     */
   /* input the string which is to be converted.  The function returns  */
   /* zero if an error occurs otherwise it returns the value parsed from*/
   /* the string passed as the input parameter.                         */
static unsigned int StringToUnsignedInteger(char *StringInteger)
{
   int          IsHex;
   unsigned int Index;
   unsigned int ret_val = 0;

   /* Before proceeding make sure that the parameter that was passed as */
   /* an input appears to be at least semi-valid.                       */
   if((StringInteger) && (strlen(StringInteger)))
   {
      /* Initialize the variable.                                       */
      Index = 0;

      /* Next check to see if this is a hexadecimal number.             */
      if(strlen(StringInteger) > 2)
      {
         if((StringInteger[0] == '0') && ((StringInteger[1] == 'x') || (StringInteger[1] == 'X')))
         {
            IsHex = 1;

            /* Increment the String passed the Hexadecimal prefix.      */
            StringInteger += 2;
         }
         else
            IsHex = 0;
      }
      else
         IsHex = 0;

      /* Process the value differently depending on whether or not a    */
      /* Hexadecimal Number has been specified.                         */
      if(!IsHex)
      {
         /* Decimal Number has been specified.                          */
         while(1)
         {
            /* First check to make sure that this is a valid decimal    */
            /* digit.                                                   */
            if((StringInteger[Index] >= '0') && (StringInteger[Index] <= '9'))
            {
               /* This is a valid digit, add it to the value being      */
               /* built.                                                */
               ret_val += (StringInteger[Index] & 0xF);

               /* Determine if the next digit is valid.                 */
               if(((Index + 1) < strlen(StringInteger)) && (StringInteger[Index+1] >= '0') && (StringInteger[Index+1] <= '9'))
               {
                  /* The next digit is valid so multiply the current    */
                  /* return value by 10.                                */
                  ret_val *= 10;
               }
               else
               {
                  /* The next value is invalid so break out of the loop.*/
                  break;
               }
            }

            Index++;
         }
      }
      else
      {
         /* Hexadecimal Number has been specified.                      */
         while(1)
         {
            /* First check to make sure that this is a valid Hexadecimal*/
            /* digit.                                                   */
            if(((StringInteger[Index] >= '0') && (StringInteger[Index] <= '9')) || ((StringInteger[Index] >= 'a') && (StringInteger[Index] <= 'f')) || ((StringInteger[Index] >= 'A') && (StringInteger[Index] <= 'F')))
            {
               /* This is a valid digit, add it to the value being      */
               /* built.                                                */
               if((StringInteger[Index] >= '0') && (StringInteger[Index] <= '9'))
                  ret_val += (StringInteger[Index] & 0xF);
               else
               {
                  if((StringInteger[Index] >= 'a') && (StringInteger[Index] <= 'f'))
                     ret_val += (StringInteger[Index] - 'a' + 10);
                  else
                     ret_val += (StringInteger[Index] - 'A' + 10);
               }

               /* Determine if the next digit is valid.                 */
               if(((Index + 1) < strlen(StringInteger)) && (((StringInteger[Index+1] >= '0') && (StringInteger[Index+1] <= '9')) || ((StringInteger[Index+1] >= 'a') && (StringInteger[Index+1] <= 'f')) || ((StringInteger[Index+1] >= 'A') && (StringInteger[Index+1] <= 'F'))))
               {
                  /* The next digit is valid so multiply the current    */
                  /* return value by 16.                                */
                  ret_val *= 16;
               }
               else
               {
                  /* The next value is invalid so break out of the loop.*/
                  break;
               }
            }

            Index++;
         }
      }
   }

   return(ret_val);
}

/* The following function is a utility function which is used to     */
/* generate random values for the ER and the IR.                     */
static void GenerateRandomKeys(void)
{
   uint8_t Status;
   uint32_t MaxSize;
   qapi_BLE_Random_Number_t RandomNumber;

   /* Initialize the max size.                                          */
   MaxSize = (sizeof(qapi_BLE_Random_Number_t) > (sizeof(qapi_BLE_Encryption_Key_t) / 2)) ? \
                (sizeof(qapi_BLE_Encryption_Key_t) / 2) : sizeof(qapi_BLE_Random_Number_t);

   /* Initialize the ER and IR to random values.                        */
   if((!qapi_BLE_HCI_LE_Rand(BluetoothStackID,  &Status,  &RandomNumber)) && (!Status))
      memcpy(&ER, &RandomNumber, MaxSize);

   if((!qapi_BLE_HCI_LE_Rand(BluetoothStackID,  &Status,  &RandomNumber)) && (!Status))
      memcpy(&(((int8_t *)&ER)[sizeof(qapi_BLE_Encryption_Key_t) / 2]), &RandomNumber, MaxSize);

   if((!qapi_BLE_HCI_LE_Rand(BluetoothStackID,  &Status,  &RandomNumber)) && (!Status))
      memcpy(&IR, &RandomNumber, MaxSize);

   if((!qapi_BLE_HCI_LE_Rand(BluetoothStackID,  &Status,  &RandomNumber)) && (!Status))
      memcpy(&(((int8_t *)&IR)[sizeof(qapi_BLE_Encryption_Key_t) / 2]), &RandomNumber, MaxSize);
}


/* The following function is responsible for opening the SS1         */
/* Bluetooth Protocol Stack.  This function accepts a pre-populated  */
/* HCI Driver Information structure that contains the HCI Driver     */
/* Transport Information.  This function returns zero on successful  */
/* execution and a negative value on all errors.                     */
static int32_t OpenStack(qapi_BLE_HCI_DriverInformation_t *HCI_DriverInformation)
{
   int32_t                 Result;
   int32_t                 ret_val = 0;
   char                    BluetoothAddress[16];
   uint32_t                serviceID;

   /* First check to see if the Stack has already been opened.          */
   if(!BluetoothStackID)
   {
      /* Next, makes sure that the Driver Information passed appears to */
      /* be semi-valid.                                                 */
      if(HCI_DriverInformation)
      {
         QCLI_LOGE (mesh_coex_group, "OpenStack()\n");
         /* Set the HCI driver information.                             */
         QAPI_BLE_HCI_DRIVER_SET_COMM_INFORMATION(HCI_DriverInformation, 1, 115200, 
                                                    QAPI_BLE_COMM_PROTOCOL_UART_E);
         /* Initialize the Stack                                        */
         Result = qapi_BLE_BSC_Initialize(HCI_DriverInformation, 0);

         /* Next, check the return value of the initialization to see if*/
         /* it was successful.                                          */
         if(Result > 0)
         {
            /* The Stack was initialized successfully, inform the user  */
            /* and set the return value of the initialization function  */
            /* to the Bluetooth Stack ID.                               */
            BluetoothStackID = Result;
            QCLI_LOGE (mesh_coex_group, "Bluetooth Stack ID: %d.\n", BluetoothStackID);

            /* Let's output the Bluetooth Device Address so that the    */
            /* user knows what the Device Address is.                   */
            if(!qapi_BLE_GAP_Query_Local_BD_ADDR(BluetoothStackID, &LocalBD_ADDR))
            {
               BD_ADDRToStr(LocalBD_ADDR, BluetoothAddress);
               QCLI_LOGE (mesh_coex_group, "BD_ADDR: %s\n", BluetoothAddress);
            }

            /*Shall be enabled later for GATT connection*/
            /* Generate some random values for IR and ER (normally would*/
            /* be in flash).                                            */
            GenerateRandomKeys();
            QCLI_LOGE (mesh_coex_group, "Random keys successfully generated\n");
            /* Regenerate IRK and DHK from the constant Identity Root   */
            /* Key.                                                     */
            qapi_BLE_GAP_LE_Diversify_Function(BluetoothStackID, (qapi_BLE_Encryption_Key_t *)(&IR), 1,0, &IRK);
            qapi_BLE_GAP_LE_Diversify_Function(BluetoothStackID, (qapi_BLE_Encryption_Key_t *)(&IR), 3, 0, &DHK);

            /* Initialize the GATT Service.                                */
            if((Result = qapi_BLE_GATT_Initialize(BluetoothStackID, 
                          (QAPI_BLE_GATT_INITIALIZATION_FLAGS_SUPPORT_LE | QAPI_BLE_GATT_INITIALIZATION_FLAGS_DISABLE_SERVICE_CHANGED_CHARACTERISTIC), 
                          BLE_COEX_GATT_Connection_Event_Callback, 0)) == 0)
            {
                uint32_t maxNumDataPkt = GATT_PKT_MAX_QUEUE_SIZE;
                /* Set Queuing Parameters */
                Result = qapi_BLE_GATT_Set_Queuing_Parameters(BluetoothStackID, maxNumDataPkt,
                                                            maxNumDataPkt-1, TRUE);
                if (Result != 0)
                {
                    QCLI_LOGE(mesh_coex_group, "GATT Set Queuing Parameters failed, error %d\n", Result);
                }
                /* Initialize the GAPS Service.                             */
                Result = qapi_BLE_GAPS_Initialize_Service(BluetoothStackID, &serviceID);
                if(Result > 0)
                {
                    QCLI_LOGE (mesh_coex_group, "GAP Service Id %x, Result %x\n", serviceID, Result);
                    /* Set the GAP Device Name and Device Appearance.        */
                   if (qapi_BLE_GAPS_Set_Device_Name(BluetoothStackID, serviceID, DEVICE_FRIENDLY_NAME) > 0)
                   {
                        QCLI_LOGE (mesh_coex_group, "GAP Service Name %s\n", DEVICE_FRIENDLY_NAME);
                   }
                   if (qapi_BLE_GAPS_Set_Device_Appearance(BluetoothStackID, 
                                               serviceID, 
                                               QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_COMPUTER) > 0)
                   {
                        QCLI_LOGE (mesh_coex_group, "GAP Device Appearence Set %x\n", 
                            QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_COMPUTER);
                   }
                }
            }

            /* Let's enable address resolution in the controller.       */
            /* * NOTE * This function MUST be called to enable the use  */
            /*          of the resolving list in the controller.        */
            Result = qapi_BLE_GAP_LE_Set_Address_Resolution_Enable(BluetoothStackID, TRUE);
            if(!Result)
            {
               /* Let's set the Resolvable Private Address (RPA) Timeout*/
               /* to 60 seconds.                                        */
               /* * NOTE * An new RPA will be generated and used for a  */
               /*          remote device when the timeout occurs.       */
               Result = qapi_BLE_GAP_LE_Set_Resolvable_Private_Address_Timeout(BluetoothStackID, 60);
               if(Result)
               {
                    QCLI_LOGE (mesh_coex_group, "Error - set resolvable private address returned %d.\n", Result);
               }
            }
            else
            {
                QCLI_LOGE (mesh_coex_group, "Error - set addr resolution enable returned %d.\n", Result);
            }
            /* Let's set the default data length to the maximum.        */
            Result = qapi_BLE_GAP_LE_Set_Default_Data_Length(BluetoothStackID, 251, 2120);
            if(Result)
            {
                QCLI_LOGE (mesh_coex_group, "qapi_BLE_GAP_LE_Set_Default_Data_Length %d \n", Result);
            }
         }
         else
         {
            /* The Stack was NOT initialized successfully, inform the   */
            /* user and set the return value of the initialization      */
            /* function to an error.                                    */
            QCLI_LOGE (mesh_coex_group, "qapi_BLE_BSC_Initialize() %d", Result);
            BluetoothStackID = 0;

            ret_val = -1;
         }
      }
      else
      {
         /* One or more of the necessary parameters are invalid.        */
         ret_val = -1;
      }
   }
   else
   {
      /* Stack is already opened.                                       */
      ret_val = 0;
   }

   return(ret_val);
}


static boolean_t CloseStack(void)
{
    boolean_t    ret_val = FALSE;

    /* First check to see if the Stack has been opened.                  */
    if(BluetoothStackID)
    {
        /* Cleanup GATT Module.                                           */
        qapi_BLE_GATT_Cleanup(BluetoothStackID);

        /* Simply close the Stack                                         */
        qapi_BLE_BSC_Shutdown(BluetoothStackID);

        /* Inform the user that the stack has been shut down.             */
        QCLI_LOGE (mesh_coex_group, "Stack Shutdown.\n");

        /* Flag that the Stack is no longer initialized.                  */
        BluetoothStackID = 0;

        /* Flag success to the caller.                                    */
        ret_val          = TRUE;
    }
    else
    {
        QCLI_LOGE (mesh_coex_group, "Bluetooth Not Initialized\n");
    }

    return(ret_val);
}


static QCLI_Command_Status_t InitializeCoExBluetooth(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;

    if (!BluetoothStackID)
    {
        BluetoothStackID = GetBluetoothStackId();
    }

    if (!BluetoothStackID)
    {
        /* Enable the debug logs */
        QmeshDebugHandleSet (&QCLI_Printf, mesh_coex_group);
        /* Initialize bluetooth */
        if (OpenStack(&HCI_DriverInformation) != 0)
        {
           QCLI_LOGE (mesh_coex_group, "Bluetooth initialization failed!!\n");
           return QCLI_STATUS_ERROR_E;
        }
        CoExBluetoothInit = TRUE;
        QCLI_LOGI (mesh_coex_group, "Bluetooth Stack Initialized\n");
    }
    else
    {
        QCLI_LOGI (mesh_coex_group, "Bluetooth is already initialized\n");
    }

    return(ret_val);
}

static QCLI_Command_Status_t DeInitializeCoExBluetooth(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;

    if (CoExBluetoothInit)
    {
        /* DeInitialize bluetooth */
        if (!CloseStack())
        {
            QCLI_LOGE (mesh_coex_group, "Bluetooth Deinitialization failed!!\n");
            return QCLI_STATUS_ERROR_E;
        }
        CoExBluetoothInit = FALSE;
        QCLI_LOGI (mesh_coex_group, "Bluetooth DeInitialized\n");
    }
    else
    {
        QCLI_LOGE (mesh_coex_group, "Bluetooth Not Initialized from CoEx\n");
        ret_val = QCLI_STATUS_ERROR_E;
    }

    return(ret_val);
}


   /* The following function is responsible for querying the Bluetooth  */
   /* Device Address of the local Bluetooth Device.  This function      */
   /* returns zero on successful execution and a negative value on all  */
   /* errors.                                                           */
static QCLI_Command_Status_t GetLocalAddress(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   int                   Result;
   BoardStr_t            BoardStr;
   qapi_BLE_BD_ADDR_t    BD_ADDR;
   QCLI_Command_Status_t ret_val;

   if (!BluetoothStackID)
       BluetoothStackID = GetBluetoothStackId();
   /* First, check that valid Bluetooth Stack ID exists.                */
   if(BluetoothStackID)
   {
      /* Attempt to submit the command.                                 */
      Result = qapi_BLE_GAP_Query_Local_BD_ADDR(BluetoothStackID, &BD_ADDR);

      /* Check the return value of the submitted command for success.   */
      if(!Result)
      {
         BD_ADDRToStr(BD_ADDR, BoardStr);

         QCLI_LOGE(mesh_coex_group, "BD_ADDR of Local Device is: %s.\n", BoardStr);

         /* Flag success to the caller.                                 */
         ret_val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         /* Display a message indicating that an error occurred while   */
         /* attempting to query the Local Device Address.               */
         QCLI_LOGE(mesh_coex_group, "GAP_Query_Local_BD_ADDR() Failure: %d.\n", Result);

         ret_val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      /* No valid Bluetooth Stack ID exists.                            */
      QCLI_LOGE (mesh_coex_group, "Bluetooth Not Initialized\n");
      ret_val = QCLI_STATUS_ERROR_E;
   }

   return(ret_val);
}

   /* The following function is responsible for starting an LE scan     */
   /* procedure.  This function returns zero if successful and a        */
   /* negative value if an error occurred.                              */
static QCLI_Command_Status_t ScanLE(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   unsigned int                    Duration;
   QCLI_Command_Status_t           ret_val;
   qapi_BLE_GAP_LE_Filter_Policy_t FilterPolicy;

   if (!BluetoothStackID)
       BluetoothStackID = GetBluetoothStackId();
   /* First, check that valid Bluetooth Stack ID exists.                */
   if(BluetoothStackID)
   {
      /* Verify the command line parameters.                            */
      if((Parameter_Count >= 1) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid) && ((Parameter_List[0].Integer_Value == 0) || (Parameter_List[0].Integer_Value == 1)))
      {
         /* Check to see if we are enabling or disabling scanning.      */
         if(Parameter_List[0].Integer_Value == 0)
         {
            /* Check to see if we are in fact in the process of         */
            /* scanning.                                                */
            if(ScanInProgress)
            {
               /* Simply stop scanning.                                 */
               if(!StopScan(BluetoothStackID))
               {
                  /* Flag that scanning is not in progess.              */
                  ScanInProgress = FALSE;

                  /* Return success to the caller.                      */
                  ret_val        = QCLI_STATUS_SUCCESS_E;
               }
               else
                  ret_val = QCLI_STATUS_ERROR_E;
            }
            else
            {
               QCLI_LOGE (mesh_coex_group, "Scanning is not in progress.\n");

               ret_val = QCLI_STATUS_SUCCESS_E;
            }
         }
         else
         {
            /* Check to see if scanning is in progress.                 */
            if(!ScanInProgress)
            {
               FilterPolicy = QAPI_BLE_FP_NO_FILTER_E;
               /* Check to see if an optional duration is specified. */
               if((Parameter_Count >= 2) && (Parameter_List) && (Parameter_List[1].Integer_Is_Valid))
                  Duration = Parameter_List[1].Integer_Value;
               else
                  Duration = 0;

               /* Simply start scanning.                                */
               if(!StartScan(BluetoothStackID, FilterPolicy, Duration))
               {
                  /* Flag that scanning is in progess.                  */
                  ScanInProgress = TRUE;

                  /* Return success to the caller.                      */
                  ret_val        = QCLI_STATUS_SUCCESS_E;
               }
               else
                  ret_val = QCLI_STATUS_ERROR_E;
            }
            else
            {
               QCLI_LOGE(mesh_coex_group, "Scan already in progress.\n");

               ret_val = QCLI_STATUS_SUCCESS_E;
            }
         }
      }
      else
         ret_val = QCLI_STATUS_USAGE_E;
   }
   else
      ret_val = QCLI_STATUS_ERROR_E;

   return(ret_val);
}

   /* The following function is responsible for connecting to a remote  */
   /* LE device.  This function returns zero if successful and a        */
   /* negative value if an error occurred.                              */
static QCLI_Command_Status_t ConnectLE(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_BLE_BD_ADDR_t    BD_ADDR;
   QCLI_Command_Status_t ret_val;
   boolean_t             UseWhiteList;

   QCLI_LOGE(mesh_coex_group, "%s\n",__FUNCTION__);
   if (!BluetoothStackID)
       BluetoothStackID = GetBluetoothStackId();
   /* First, check that valid Bluetooth Stack ID exists.                */
   if(BluetoothStackID)
   {
      /* Next, make sure that a valid device address exists.            */
      if((Parameter_Count >= 1) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid) && (Parameter_List[0].Integer_Value >= 0) && (Parameter_List[0].Integer_Value <= 1))
      {
         /* Store if the user has requested to use the White List in the*/
         /* controller.                                                 */
         if(Parameter_List[0].Integer_Value == 1)
            UseWhiteList = TRUE;
         else
            UseWhiteList = FALSE;

         /* Check to see if scanning is in progress, if so cancel it.   */
         if(ScanInProgress)
         {
            /* Attempt to stop scanning.                                */
            if(!StopScan(BluetoothStackID))
               ScanInProgress = FALSE;

            QCLI_LOGE(mesh_coex_group, "Scan stopped before making LE Connection\n");
         }

         /* If we are NOT using the White List in the controller, then  */
         /* the local device will send a connection request when a      */
         /* connectable advertisement is received from a remote device  */
         /* in the White List.                                          */
         /* * NOTE * A remote device and address type MUST have         */
         /*          previously been added to the White List in the     */
         /*          controller.                                        */
         if(UseWhiteList)
         {
            /* Attempt to connect using the White List in the           */
            /* controller.                                              */
            /* * NOTE * The remote address and address type are NOT     */
            /*          used.                                           */
            if(!ConnectLEDevice(BluetoothStackID, UseWhiteList, NULL, 0))
               ret_val = QCLI_STATUS_SUCCESS_E;
            else
               ret_val = QCLI_STATUS_ERROR_E;
         }
         else
         {
            /* Make sure the remote address and address type have been  */
            /* specified.                                               */
            if((Parameter_Count >= 3) && (Parameter_List[1].String_Value) && (strlen((char *)(Parameter_List[1].String_Value)) == (sizeof(qapi_BLE_BD_ADDR_t) * 2)) && (Parameter_List[2].Integer_Is_Valid) && (Parameter_List[2].Integer_Value >= QAPI_BLE_LAT_PUBLIC_E) && (Parameter_List[2].Integer_Value <= QAPI_BLE_LAT_RANDOM_IDENTITY_E))
            {
               /* Convert the parameter to a Bluetooth Device Address.  */
               StrToBD_ADDR((char *)(Parameter_List[1].String_Value), &BD_ADDR);

               /* Attempt to connect to the specified remote address and*/
               /* address type.                                         */
               if(!ConnectLEDevice(BluetoothStackID, UseWhiteList, &BD_ADDR, (qapi_BLE_GAP_LE_Address_Type_t)Parameter_List[2].Integer_Value))
                  ret_val = QCLI_STATUS_SUCCESS_E;
               else
                  ret_val = QCLI_STATUS_ERROR_E;
            }
            else
               ret_val = QCLI_STATUS_USAGE_E;
         }
      }
      else
         ret_val = QCLI_STATUS_USAGE_E;
   }
   else
      ret_val = QCLI_STATUS_ERROR_E;

   return(ret_val);
}

   /* The following function is responsible for disconnecting to an LE  */
   /* device.  This function returns zero if successful and a negative  */
   /* value if an error occurred.                                       */
static QCLI_Command_Status_t DisconnectLE(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t ret_val;
   if (!BluetoothStackID)
       BluetoothStackID = GetBluetoothStackId();
   /* First, check that valid Bluetooth Stack ID exists.                */
   if(BluetoothStackID)
   {
      /* Next, check to make sure we are currently connected.           */
      /* * NOTE * If we are connected, then the a remote device MUST be */
      /*          selected.                                             */
      if(ConnectionCount)
      {
         /* Disconnect the selected remote device.                      */
         ret_val = qapi_BLE_GAP_LE_Disconnect(BluetoothStackID, SelectedRemoteBD_ADDR);
         if(!ret_val)
            QCLI_LOGE(mesh_coex_group, "Disconnect Request successful.\n");

         else
            QCLI_LOGE(mesh_coex_group, "Unable to disconnect device: %d.\n", ret_val);

         ret_val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         QCLI_LOGE(mesh_coex_group, "Device is not connected.\n");

         ret_val = QCLI_STATUS_SUCCESS_E;
      }
   }
   else
      ret_val = QCLI_STATUS_ERROR_E;

   return(ret_val);
}

   /* The following function is responsible for canceling an active LE  */
   /* connection process to an LE device.  This function returns zero if*/
   /* successful and a negative value if an error occurred.             */
static QCLI_Command_Status_t CancelConnectLE(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   int                   Result;
   QCLI_Command_Status_t ret_val;

   /* First, check that valid Bluetooth Stack ID exists.                */
   if(BluetoothStackID)
   {
      /* Attempt to cancel the connection process.                      */
      Result = qapi_BLE_GAP_LE_Cancel_Create_Connection(BluetoothStackID);
      if(!Result)
         ret_val = QCLI_STATUS_SUCCESS_E;
      else
      {
         QCLI_LOGE(mesh_coex_group, "Unable to cancel LE connection process: %d.\n", Result);
         ret_val = QCLI_STATUS_ERROR_E;
      }
   }
   else
      ret_val = QCLI_STATUS_ERROR_E;

   return(ret_val);
}

static QCLI_Command_Status_t CancelLECreateConnection(void)
{
   int                   Result;
   QCLI_Command_Status_t ret_val;

   /* First, check that valid Bluetooth Stack ID exists.                */
   if(BluetoothStackID)
   {
      /* Attempt to cancel the connection process.                      */
      Result = qapi_BLE_GAP_LE_Cancel_Create_Connection(BluetoothStackID);
      if(!Result)
         ret_val = QCLI_STATUS_SUCCESS_E;
      else
      {
         QCLI_LOGE(mesh_coex_group, "Unable to cancel LE connection process: %d.\n", Result);
         ret_val = QCLI_STATUS_ERROR_E;
      }
   }
   else
      ret_val = QCLI_STATUS_ERROR_E;

   return(ret_val);
}


   /* The following function is respoinsible for selecting a remote     */
   /* device.  This is necessary if multiple remote devices are         */
   /* connected.                                                        */
   /* * NOTE * The selected remote device may be changed during a       */
   /*          pairing/security re-establishment process since the      */
   /*          SecurityRemoteBD_ADDR is used to store the remote device */
   /*          address during this process.                             */
static QCLI_Command_Status_t SelectRemoteDevice(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   int           ret_val;
   DeviceInfo_t *DeviceInfo;
   BoardStr_t    BoardStr;

   if (!BluetoothStackID)
       BluetoothStackID = GetBluetoothStackId();
   /* First, check that valid Bluetooth Stack ID exists.                */
   if(BluetoothStackID)
   {
      /* Next, check to make sure we are currently connected.           */
      /* * NOTE * If we are connected, then the a remote device MUST be */
      /*          selected.                                             */
      if(ConnectionCount)
      {
         /* Verify that the input parameters are semi-valid.            */
         if((Parameter_Count >= 1) && (Parameter_List) && (Parameter_List[0].Integer_Is_Valid) && (Parameter_List[0].Integer_Value > 0))
         {
            /* Lock the Bluetooth stack.                                */
            if(!qapi_BLE_BSC_LockBluetoothStack(BluetoothStackID))
            {
               /* Get the device info for the remote device.            */
               if((DeviceInfo = SearchDeviceInfoEntryByConnectionID(&DeviceInfoList, (unsigned int)(Parameter_List[0].Integer_Value))) != NULL)
               {
                  /* Simply update the address of the selected remote   */
                  /* device.                                            */
                  SelectedRemoteBD_ADDR = DeviceInfo->RemoteAddress;

                  /* Inform the user of the selected remote device.     */
                  QCLI_LOGE(mesh_coex_group, "\nSelected Remote Device:\n");
                  BD_ADDRToStr(SelectedRemoteBD_ADDR, BoardStr);
                  QCLI_LOGE(mesh_coex_group, "   Address:  %s\n", BoardStr);
                  QCLI_LOGE(mesh_coex_group, "   ID:       %u\n", DeviceInfo->ConnectionID);

                  ret_val               = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  QCLI_LOGE(mesh_coex_group, "No device info for the specified GATT Connection ID.\n");

                  ret_val = QCLI_STATUS_ERROR_E;
               }

               /* Un-lock the Bluetooth Stack.                          */
               qapi_BLE_BSC_UnLockBluetoothStack(BluetoothStackID);
            }
            else
            {
               QCLI_LOGE(mesh_coex_group, "Unable to acquire Bluetooth Stack Lock.\n");

               ret_val = QCLI_STATUS_ERROR_E;
            }
         }
         else
            ret_val = QCLI_STATUS_USAGE_E;
      }
      else
      {
         QCLI_LOGE(mesh_coex_group, "No Connection Established.\n");

         /* Flag success to the caller.                                 */
         ret_val = QCLI_STATUS_SUCCESS_E;
      }
   }
   else
      ret_val = QCLI_STATUS_ERROR_E;

   return(ret_val);
}

   /* The following function is responsible for displaying all remote   */
   /* device information that is stored by the local device.            */
   /* * NOTE * A remote device information entry may exist for a remote */
   /*          device even if we are NOT connected.  This will occur if */
   /*          we have previously paired with the remote device.  In    */
   /*          this case, the remote device information entry will NOT  */
   /*          be deleted when the remote device is disconnected.  If   */
   /*          the remote device is NOT connected, then the GATT        */
   /*          Connection ID will be zero.                              */
static QCLI_Command_Status_t DisplayRemoteDevices(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   BoardStr_t             BoardStr;
   unsigned int           Index;
   DeviceInfo_t          *DeviceInfo;
   QCLI_Command_Status_t  ret_val;

   if (!BluetoothStackID)
       BluetoothStackID = GetBluetoothStackId();
   /* Lock the Bluetooth stack.                                         */
   if(!qapi_BLE_BSC_LockBluetoothStack(BluetoothStackID))
   {
      /* Loop through the device information list.                      */
      DeviceInfo = DeviceInfoList;
      Index      = 0;
      while(DeviceInfo)
      {
         /* Display the information for the remote device.              */
         QCLI_LOGE(mesh_coex_group, "\nDevice Information Entry (%u):\n", Index++);
         QCLI_LOGE(mesh_coex_group, "   Selected:   %s\n", ((QAPI_BLE_COMPARE_BD_ADDR(SelectedRemoteBD_ADDR, DeviceInfo->RemoteAddress)) ? "Yes" : "No"));
         BD_ADDRToStr(DeviceInfo->RemoteAddress, BoardStr);
         QCLI_LOGE(mesh_coex_group, "   Address:    %s\n", BoardStr);
         QCLI_LOGE(mesh_coex_group, "   Type:       ");
         switch(DeviceInfo->RemoteAddressType)
         {
            case QAPI_BLE_LAT_PUBLIC_E:
               QCLI_LOGE(mesh_coex_group, "QAPI_BLE_LAT_PUBLIC_E\n");
               break;
            case QAPI_BLE_LAT_RANDOM_E:
               QCLI_LOGE(mesh_coex_group, "QAPI_BLE_LAT_RANDOM_E\n");
               break;
            case QAPI_BLE_LAT_PUBLIC_IDENTITY_E:
               QCLI_LOGE(mesh_coex_group, "QAPI_BLE_LAT_PUBLIC_IDENTITY_E\n");
               break;
            case QAPI_BLE_LAT_RANDOM_IDENTITY_E:
               QCLI_LOGE(mesh_coex_group, "QAPI_BLE_LAT_RANDOM_IDENTITY_E\n");
               break;
            default:
               QCLI_LOGE(mesh_coex_group, "Invalid\n");
               break;
         }
         QCLI_LOGE(mesh_coex_group, "   Connected:  %s\n", (DeviceInfo->ConnectionID) ? "Yes" : "No");
         QCLI_LOGE(mesh_coex_group, "   ID:         %u\n", DeviceInfo->ConnectionID);
         QCLI_LOGE(mesh_coex_group, "   LTK:        %s\n", (DeviceInfo->Flags & (uint8_t)DEVICE_INFO_FLAGS_LTK_VALID) ? "Yes" : "No");

         if(DeviceInfo->Flags & (uint8_t)DEVICE_INFO_FLAGS_LTK_VALID)
         {
            QCLI_LOGE(mesh_coex_group, "   LTK:        0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n", DeviceInfo->LTK.Long_Term_Key0,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key1,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key2,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key3,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key4,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key5,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key6,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key7,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key8,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key9,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key10,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key11,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key12,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key13,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key14,
                                                                                                                          DeviceInfo->LTK.Long_Term_Key15);


         }

         QCLI_LOGE(mesh_coex_group, "   IRK:        %s\n", (DeviceInfo->Flags & (uint8_t)DEVICE_INFO_FLAGS_IRK_VALID) ? "Yes" : "No");

         if(DeviceInfo->Flags & (uint8_t)DEVICE_INFO_FLAGS_IRK_VALID)
         {
            BD_ADDRToStr(DeviceInfo->IdentityAddressBD_ADDR, BoardStr);
            QCLI_LOGE(mesh_coex_group, "   Identity Address:      %s\n", BoardStr);

            switch(DeviceInfo->IdentityAddressType)
            {
               case QAPI_BLE_LAT_PUBLIC_E:
                  QCLI_LOGE(mesh_coex_group, "   Identity Address Type: %s\n", "Public");
                  break;
               case QAPI_BLE_LAT_RANDOM_E:
                  QCLI_LOGE(mesh_coex_group, "   Identity Address Type: %s\n", "Random");
                  break;
               case QAPI_BLE_LAT_PUBLIC_IDENTITY_E:
                  QCLI_LOGE(mesh_coex_group, "   Identity Address Type: %s\n", "Public Identity");
                  break;
               case QAPI_BLE_LAT_RANDOM_IDENTITY_E:
                  QCLI_LOGE(mesh_coex_group, "   Identity Address Type: %s\n", "Random Identity");
                  break;
               default:
                  QCLI_LOGE(mesh_coex_group, "   Identity Address Type: %s\n", "Default");
                  break;
            }
         }

         DeviceInfo = DeviceInfo->NextDeviceInfoInfoPtr;
      }

      /* Un-lock the Bluetooth Stack.                                   */
      qapi_BLE_BSC_UnLockBluetoothStack(BluetoothStackID);

      /* Return success to the caller.                                  */
      ret_val = QCLI_STATUS_SUCCESS_E;
   }
   else
   {
      QCLI_LOGE(mesh_coex_group, "Unable to acquire Bluetooth Stack Lock.\n");

      ret_val = QCLI_STATUS_ERROR_E;
   }

   return(ret_val);
}


static void SendMeshDataThread(void *Param)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;
    int msg_count = 0;
    uint8_t data[] = {0xC0,0x00,0x04,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
    int result = -1;
    uint32_t interval = QMCTMeshSendInterval;

    QCLI_LOGE(mesh_coex_group, "SendMeshDataThread : START\n");

    app_key_info.key_type = QMESH_KEY_TYPE_APP_KEY;
    app_key_info.key_info.prov_net_idx = 0;
    app_key_info.key_info.key_idx = 0;

    data_info.dst_addr = QMCTMeshSendElementAddr;
    data_info.src_addr = QMCTMeshSrcElementAddr;
    data_info.ttl = QMCTMeshPktTTL;
    data_info.payload_len = 10;
    data_info.payload = data;

    data_info.trans_mic_size = 4;
    data_info.trans_ack_required = 0;

    while(QMCTMeshRunning)
    {
        result = QmeshSendMessage(&app_key_info, &data_info);
        if(result == QMESH_RESULT_SUCCESS)
        {
            msg_count++;
            QCLI_LOGE(mesh_coex_group, "SendMeshDataThread Msg Count = %d\n", msg_count);
        }
        else if (QMESH_RESULT_INSUFFICIENT_RESOURCES != result)
        {
             QCLI_LOGE(mesh_coex_group, "QmeshSendMessage failed, ret=0x%x\n", result);
        }
        qurt_thread_sleep(interval/10);
    }

    QCLI_LOGE(mesh_coex_group, "SendMeshDataThread : END\n");
    QCLI_Display_Prompt();

    qurt_thread_stop();
}

static void SendMeshData(void)
{
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T app_key_info;
    QMESH_ACCESS_PAYLOAD_INFO_T data_info;
    uint8_t data[] = {0xC0,0x00,0x04,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};

    int result = -1;

    app_key_info.key_type = QMESH_KEY_TYPE_APP_KEY;
    app_key_info.key_info.prov_net_idx = 0;
    app_key_info.key_info.key_idx = 0;

    data_info.dst_addr = QMCTMeshSendElementAddr;
    data_info.src_addr = QMCTMeshSrcElementAddr;
    data_info.ttl = QMCTMeshPktTTL;
    data_info.payload_len = 10;
    data_info.payload = data;

    data_info.trans_mic_size = 4;
    data_info.trans_ack_required = 0;

    result = QmeshSendMessage(&app_key_info, &data_info);
    if(result == QMESH_RESULT_SUCCESS)
    {
        QMCTMeshMsgCount++;
        QCLI_LOGE(mesh_coex_group, "SendMeshData Msg Count= %d\n", QMCTMeshMsgCount);
    }
    else if (QMESH_RESULT_INSUFFICIENT_RESOURCES != result)
    {
         QCLI_LOGE(mesh_coex_group, "QmeshSendMessage failed, ret=0x%x\n", result);
    }
}

static void SendGattDataThread(void *Param)
{
    qapi_BLE_BD_ADDR_t BD_ADDR;
    DeviceInfo_t          *DeviceInfo;
    int          ByteSent = 0;
    int          Result;
    boolean_t    Done = FALSE;
    unsigned int DataCount = 20;
    int DataIndex = 0;
    int duration = 0;
    BoardStr_t   BoardStr;
    DevGattBufferFull = FALSE;

    memcpy(&BD_ADDR, Param, sizeof(qapi_BLE_BD_ADDR_t));

    BD_ADDRToStr(BD_ADDR, BoardStr);
    QCLI_LOGE(mesh_coex_group, "SendGattDataThread [%s]: START\n",BoardStr);

    /* Verify that there is a connection that is established.         */
    if(QMCTConnectionCount)
    {
        /* Get the device info for the connection device.        */
        if((DeviceInfo = SearchDeviceInfoEntryByBD_ADDR(&DeviceInfoList, BD_ADDR)) != NULL)
        {
            while((duration < QMCTGattDuration) && (!Done) && (QMCTGattRunning))
            {
                DataCount = 20;
                /* We are acting as SPPLE Client, so write to the Rx        */
                /* Characteristic.                                          */
                if(DeviceInfo->QMCTClientInfo.Tx_Characteristic)
                {
                    if (!DevGattBufferFull)
                    {
                        Result = qapi_BLE_GATT_Write_Without_Response_Request(BluetoothStackID, DeviceInfo->ConnectionID, 
                                           DeviceInfo->QMCTClientInfo.Tx_Characteristic, 
                                           (uint16_t)DataCount, 
                                           &QMCTBuffer[DataIndex]);
                    }
                    else
                    {
                        qurt_thread_sleep(8);
                        continue;
                    }
                }
                else
                {
                   /* We have not discovered the Tx Characteristic, so exit */
                   /* the loop.                                             */
                   Done = TRUE;
                }

                /* Check to see if any data was written.                       */
                if(!Done)
                {
                    /* Check to see if the data was written successfully.       */
                    if(Result >= 0)
                    {
                        ByteSent += Result;
                        DataIndex += Result;
                        if ((DataIndex + DataCount) >= QMCTBufferLength)
                            DataIndex = 0;
                        QCLI_LOGE(mesh_coex_group, "SendGattDataThread : Bytes Sent %d to Conn Id %d\n", ByteSent, DeviceInfo->ConnectionID);
                    }
                    else
                    {
                        if (QAPI_BLE_BTPS_ERROR_INSUFFICIENT_BUFFER_SPACE == Result)
                        {
                            QCLI_LOGE(mesh_coex_group, "SendGattDataThread : SEND failed with error GATT BUFFER FULL to Conn Id %d\n", DeviceInfo->ConnectionID);
                            DevGattBufferFull = TRUE;
                        }
                        else if (QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID == Result)
                        {
                            QCLI_LOGE(mesh_coex_group, "SendGattDataThread : SEND failed with error Invalid Connection ID to Conn Id %d\n", DeviceInfo->ConnectionID);
                            Done = TRUE;
                        }
                        else
                            QCLI_LOGE(mesh_coex_group, "SendGattDataThread : SEND failed with error %d to Conn Id %d\n", Result, DeviceInfo->ConnectionID);
                    }
                }
                if (QMCTWriteInterval == 20) 
                {
                    qurt_thread_sleep(1);
                    duration += 10;
                }
                else if (QMCTWriteInterval == 100)
                {
                    qurt_thread_sleep(8);
                    duration += 80;
                }
                else
                {
                    qurt_thread_sleep(QMCTWriteInterval/10);
                    duration += QMCTWriteInterval;
                }
                QCLI_Display_Prompt();
            }
            /* Get the device info for the connected devices.        */
            if((DeviceInfo = SearchDeviceInfoEntryByBD_ADDR(&DeviceInfoList, BD_ADDR)) != NULL)
            {
                /* Add respective delay to disconnect the device */
                qurt_thread_sleep(5);
                /* Trigger Disconnect */
                DisconnectLEDevice(DeviceInfo->RemoteAddress);
            }
        }
        else
        {
            QCLI_LOGE(mesh_coex_group, "SendGattDataThread : No Device Info.\n");
        }
    }
    else
    {
        QCLI_LOGE(mesh_coex_group, "SendGattDataThread : No Connection Established\n");
    }

    QCLI_LOGE(mesh_coex_group, "SendGattDataThread [%s]: END\n",BoardStr);
    QCLI_Display_Prompt();
    /* Stop Thread */
    qurt_thread_stop();
}


static int DisconnectLEDevice(qapi_BLE_BD_ADDR_t BD_ADDR)
{
    int          Result;
    BoardStr_t   BoardStr;

    BD_ADDRToStr(BD_ADDR, BoardStr);
    QCLI_LOGE(mesh_coex_group, "DisconnectLEDevice : %s \n",BoardStr);
    /* Disconnect the selected remote device.                      */
    Result = qapi_BLE_GAP_LE_Disconnect(BluetoothStackID, BD_ADDR);
    if(!Result)
        QCLI_LOGE(mesh_coex_group, "Disconnect Request successful.\n");
    else
        QCLI_LOGE(mesh_coex_group, "Unable to disconnect device: %d.\n", Result);

    return Result;
}


static int DiscoverLEDeviceServices(qapi_BLE_BD_ADDR_t  BD_ADDR)
{
    int                                    Result;
    DeviceInfo_t                           *DeviceInfo;
    int                                    ret_val;
    BoardStr_t                             BoardStr;

    /* Get the device info for the connection device.              */
    if((DeviceInfo = SearchDeviceInfoEntryByBD_ADDR(&DeviceInfoList, BD_ADDR)) != NULL)
    {
        /* Verify that no service discovery is outstanding for this */
        /* device.                                                  */
        if(!(DeviceInfo->Flags & DEVICE_INFO_FLAGS_SERVICE_DISCOVERY_OUTSTANDING))
        {
            SelectedRemoteBD_ADDR = BD_ADDR;
            QCLI_LOGE(mesh_coex_group, "\nSelected Remote Device:\n");
            BD_ADDRToStr(SelectedRemoteBD_ADDR, BoardStr);
            QCLI_LOGE(mesh_coex_group, "   Address:  %s\n", BoardStr);
            Result = qapi_BLE_GATT_Start_Service_Discovery(BluetoothStackID, 
                            DeviceInfo->ConnectionID, 
                            0, NULL, 
                            BLE_COEX_GATT_Service_Discovery_Event_Callback, 0);
            if(!Result)
            {
                /* Display success message.                           */
                QCLI_LOGE(mesh_coex_group, "qapi_BLE_GATT_Service_Discovery_Start() success.\n");

                /* Flag that a Service Discovery Operation is         */
                /* outstanding.                                       */
                DeviceInfo->Flags |= DEVICE_INFO_FLAGS_SERVICE_DISCOVERY_OUTSTANDING;

                ret_val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
                /* An error occur so just clean-up.                   */
                QCLI_LOGE(mesh_coex_group, "Error - GATT_Service_Discovery_Start returned %d.\n", Result);

                ret_val = QCLI_STATUS_ERROR_E;
            }
        }
        else
        {
            QCLI_LOGE(mesh_coex_group, "Service Discovery Operation Outstanding for Device.\n");

            ret_val = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        QCLI_LOGE(mesh_coex_group, "No Device Info.\n");

        ret_val = QCLI_STATUS_ERROR_E;
    }
    return(ret_val);
}

   /* Dummy Service (UUID = 0x1234) helper functions.                   */

   /* The following function is a utility function that provides a      */
   /* mechanism of populating a Dummy Client Information structure with  */
   /* the information discovered from a GATT Discovery operation.       */
static void QMCTPopulateHandles(QMCT_Client_Info_t *ClientInfo, qapi_BLE_GATT_Service_Discovery_Indication_Data_t *ServiceDiscoveryData)
{
    uint16_t                                              *ClientConfigurationHandle; 
    unsigned int                                           Index1;
    unsigned int                                           Index2;
    qapi_BLE_GATT_Characteristic_Information_t            *CurrentCharacteristic;
    qapi_BLE_GATT_Characteristic_Descriptor_Information_t *CurrentDescriptor;

    QCLI_LOGE(mesh_coex_group, "QMCTPopulateHandles\n");
    /* Verify that the input parameters are semi-valid.                  */
    if((ClientInfo) && (ServiceDiscoveryData) && (ServiceDiscoveryData->ServiceInformation.UUID.UUID_Type == QAPI_BLE_GU_UUID_16_E) && 
        (QAPI_BLE_COMPARE_QMCT_SERVICE_UUID_TO_UUID_16(ServiceDiscoveryData->ServiceInformation.UUID.UUID.UUID_16)))
    {
        QCLI_LOGE(mesh_coex_group, "QMCTPopulateHandles : Num of Charc %d\n", ServiceDiscoveryData->NumberOfCharacteristics);
        /* Loop through all characteristics discovered in the service     */
        /* and populate the correct entry.                                */
        CurrentCharacteristic = ServiceDiscoveryData->CharacteristicInformationList;
        if(CurrentCharacteristic)
        {
            for(Index1 = 0; Index1 < ServiceDiscoveryData->NumberOfCharacteristics; Index1++, CurrentCharacteristic++)
            {
                /* All SPPLE UUIDs are defined to be 128 bit UUIDs.         */
                if(CurrentCharacteristic->Characteristic_UUID.UUID_Type == QAPI_BLE_GU_UUID_16_E)
                {
                   ClientConfigurationHandle = NULL;

                   /* Determine which characteristic this is.               */
                   if(!QAPI_BLE_COMPARE_QMCT_PRIM_SRVC_UUID_TO_UUID_16(CurrentCharacteristic->Characteristic_UUID.UUID.UUID_16))
                   {
                       continue;
                   }
                   else
                   {
                      QCLI_LOGE(mesh_coex_group, "QMCTPopulateHandles : TX Charc Handle %x\n", CurrentCharacteristic->Characteristic_Handle);
                      ClientInfo->Tx_Characteristic = CurrentCharacteristic->Characteristic_Handle;
                      ClientConfigurationHandle = &(ClientInfo->Tx_Client_Configuration_Descriptor);
                   }

                   /* Loop through the Descriptor List.                     */
                   CurrentDescriptor = CurrentCharacteristic->DescriptorList;
                   if((CurrentDescriptor) && (ClientConfigurationHandle))
                   {
                      for(Index2=0;Index2<CurrentCharacteristic->NumberOfDescriptors;Index2++,CurrentDescriptor++)
                      {
                         if(CurrentDescriptor->Characteristic_Descriptor_UUID.UUID_Type == QAPI_BLE_GU_UUID_16_E)
                         {
                            if(QAPI_BLE_GATT_COMPARE_CLIENT_CHARACTERISTIC_CONFIGURATION_ATTRIBUTE_TYPE_TO_BLUETOOTH_UUID_16(CurrentDescriptor->Characteristic_Descriptor_UUID.UUID.UUID_16))
                            {
                                QCLI_LOGE(mesh_coex_group, "QMCTPopulateHandles : TX Charc CCD Handle %x\n", CurrentDescriptor->Characteristic_Descriptor_Handle);
                               *ClientConfigurationHandle = CurrentDescriptor->Characteristic_Descriptor_Handle;
                               break;
                            }
                         }
                      }
                   }
                }
            }
        }
    }
}
	
static int StartSendMeshDataThread(void)
{
    qurt_thread_attr_t     thread_qmesh_attribte;
    qurt_thread_t          thread_qmesh_handle;
    int result = -1;

    qurt_thread_attr_init(&thread_qmesh_attribte);
    qurt_thread_attr_set_name(&thread_qmesh_attribte, "QM Thread");
    qurt_thread_attr_set_priority(&thread_qmesh_attribte, QMCT_THREAD_PRIORITY);
    qurt_thread_attr_set_stack_size(&thread_qmesh_attribte, QMCT_MESH_THREAD_STACK_SIZE);
    result = qurt_thread_create(&thread_qmesh_handle, &thread_qmesh_attribte, SendMeshDataThread, NULL);
    if (result != QURT_EOK)
    {
        QCLI_LOGE(mesh_coex_group, "Qmesh Thread creation failed\n");
    }

    return result;
}


static int StartSendGattDataThread(qapi_BLE_BD_ADDR_t *BD_ADDR)
{
    qurt_thread_attr_t     thread_gatt_attribte;
    qurt_thread_t          thread_gatt_handle;
    int result = -1;

    qurt_thread_attr_init(&thread_gatt_attribte);
    qurt_thread_attr_set_name(&thread_gatt_attribte, "G Thread");
    qurt_thread_attr_set_priority(&thread_gatt_attribte, QMCT_THREAD_PRIORITY);
    qurt_thread_attr_set_stack_size(&thread_gatt_attribte, QMCT_THREAD_STACK_SIZE);
    result = qurt_thread_create(&thread_gatt_handle, &thread_gatt_attribte, SendGattDataThread, BD_ADDR);
    if (result != QURT_EOK)
    {
        QCLI_LOGE(mesh_coex_group, "Gatt Thread creation failed\n");
    }

    return result;
}

static void FillQMCTBuffer(void)
{
    int index = 0;
    for (index = 0; index < QMCTBufferLength; index++)
    {
        QMCTBuffer[index] = (uint8_t) index & 0x00FF;
    }
}

static void SendMeshDataTimeoutHandler(QMESH_TIMER_HANDLE_T tid, void *data)
{
    if ((tid == g_mesh_tx_tmr) && QMCTMeshRunning)
    {
        SendMeshData();
        g_mesh_tx_tmr = QmeshTimerCreate(&coex_timer_ghdl, SendMeshDataTimeoutHandler, NULL, QMCTMeshSendInterval*QMESH_MILLISECOND);
        if (g_mesh_tx_tmr == QMESH_TIMER_INVALID_HANDLE)
        {
            QCLI_LOGE(mesh_coex_group, "Unable to start timer\n");
            if (g_mesh_tx_dur_tmr != QMESH_TIMER_INVALID_HANDLE)
            {
                QmeshTimerDelete(&coex_timer_ghdl, &g_mesh_tx_dur_tmr);
                g_mesh_tx_dur_tmr = QMESH_TIMER_INVALID_HANDLE;
            }
        }
    }
    else if (tid == g_mesh_tx_dur_tmr)
    {
        QCLI_LOGE(mesh_coex_group, "SendMeshDataTimeoutHandler : tid = %x, mesh_tx_dur_tmr = %x\n", tid, g_mesh_tx_dur_tmr);
        QMCTMeshRunning = FALSE;
        if (g_mesh_tx_tmr != QMESH_TIMER_INVALID_HANDLE)
        {
            QmeshTimerDelete(&coex_timer_ghdl, &g_mesh_tx_tmr);
            g_mesh_tx_tmr = QMESH_TIMER_INVALID_HANDLE;
        }
    }
}

static QCLI_Command_Status_t SendMeshPackets(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;

    if (CoExBluetoothInit)
    {
        QCLI_LOGE(mesh_coex_group, "SendMeshPackets : Mesh is not initialized\n");
        QCLI_Display_Prompt();
        return QCLI_STATUS_ERROR_E;
    }

    QCLI_LOGE(mesh_coex_group, "SendMeshPackets\n");
    if((Parameter_Count == 5) && (Parameter_List[0].Integer_Is_Valid) && 
        (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid))
    {
        QMCTMeshDuration = Parameter_List[0].Integer_Value;
        QMCTMeshSendElementAddr = Parameter_List[1].Integer_Value;
        QMCTMeshSrcElementAddr = Parameter_List[2].Integer_Value;
        QMCTMeshSendInterval = Parameter_List[3].Integer_Value;
        QMCTMeshPktTTL = Parameter_List[4].Integer_Value;

        if (QMCTMeshSendInterval < MESH_PKT_SEND_INTERVAL_MIN)
        {
            QCLI_LOGE(mesh_coex_group, "Mesh Packet Interval should be more than %d ms\n",MESH_PKT_SEND_INTERVAL_MIN);
            return QCLI_STATUS_USAGE_E;
        }
        /* Set the flag */
        QMCTMeshRunning = TRUE;

        QMCTMeshSendInterval -= 15; /* QmeshSendMessage() takes around 15 ms */

        if (coex_timer_ghdl == NULL)
        {
            coex_timer_ghdl = QmeshTimerCreateGroup(MAX_NUM_OF_COEX_TIMERS);
        }

        QCLI_LOGE(mesh_coex_group, "SendMeshPackets : duration = %d ms\n",QMCTMeshDuration);
        g_mesh_tx_dur_tmr = QmeshTimerCreate(&coex_timer_ghdl, SendMeshDataTimeoutHandler, NULL, QMCTMeshDuration*QMESH_MILLISECOND);
        if (g_mesh_tx_dur_tmr == QMESH_TIMER_INVALID_HANDLE)
        {
            QCLI_LOGE(mesh_coex_group, "Unable to Create Timer\n");
            QCLI_Display_Prompt();
            return QCLI_STATUS_ERROR_E;
        }
        /* Qmesh Thread */
        StartSendMeshDataThread();

        QCLI_Display_Prompt();
    }
    else
        ret_val = QCLI_STATUS_USAGE_E;

    return(ret_val);
}


static QCLI_Command_Status_t SendMeshPacketsWithoutThread(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;

    if (CoExBluetoothInit)
    {
        QCLI_LOGE(mesh_coex_group, "SendMeshPacketsWithoutThread : Mesh is not initialized\n");
        QCLI_Display_Prompt();
        return QCLI_STATUS_ERROR_E;
    }

    QCLI_LOGE(mesh_coex_group, "SendMeshPacketsWithoutThread\n");
    if((Parameter_Count == 5) && (Parameter_List[0].Integer_Is_Valid) && 
        (Parameter_List[1].Integer_Is_Valid) && (Parameter_List[2].Integer_Is_Valid))
    {
        QMCTMeshDuration = Parameter_List[0].Integer_Value;
        QMCTMeshSendElementAddr = Parameter_List[1].Integer_Value;
        QMCTMeshSrcElementAddr = Parameter_List[2].Integer_Value;
        QMCTMeshSendInterval = Parameter_List[3].Integer_Value;
        QMCTMeshPktTTL = Parameter_List[4].Integer_Value;
        /* Set the flag */
        QMCTMeshRunning = TRUE;
        /* Clear Msg Count */
        QMCTMeshMsgCount = 0;

        if (QMCTMeshSendInterval < MESH_PKT_SEND_INTERVAL_MIN)
        {
            QCLI_LOGE(mesh_coex_group, "Mesh Packet Interval should be more than %d ms\n", MESH_PKT_SEND_INTERVAL_MIN);
            return QCLI_STATUS_USAGE_E;
        }

        if (coex_timer_ghdl == NULL)
        {
            coex_timer_ghdl = QmeshTimerCreateGroup(MAX_NUM_OF_COEX_TIMERS);
        }

        QCLI_LOGE(mesh_coex_group, "SendMeshPacketsWithoutThread : START\n");
        QCLI_LOGE(mesh_coex_group, "SendMeshPacketsWithoutThread : duration = %d ms\n",QMCTMeshDuration);
        g_mesh_tx_dur_tmr = QmeshTimerCreate(&coex_timer_ghdl, SendMeshDataTimeoutHandler, NULL, QMCTMeshDuration*QMESH_MILLISECOND);
        if (g_mesh_tx_dur_tmr == QMESH_TIMER_INVALID_HANDLE)
        {
            QCLI_LOGE(mesh_coex_group, "Unable to start timer\n");
            QCLI_Display_Prompt();
            return QCLI_STATUS_ERROR_E;
        }

        QMCTMeshSendInterval -= 15; /* QmeshSendMessage() takes around 15 ms */

        g_mesh_tx_tmr = QmeshTimerCreate(&coex_timer_ghdl, SendMeshDataTimeoutHandler, NULL, QMCTMeshSendInterval*QMESH_MILLISECOND);
        if (g_mesh_tx_tmr == QMESH_TIMER_INVALID_HANDLE)
        {
            QCLI_LOGE(mesh_coex_group, "Unable to start timer\n");
            QmeshTimerDelete(&coex_timer_ghdl, &g_mesh_tx_dur_tmr);
            g_mesh_tx_dur_tmr = QMESH_TIMER_INVALID_HANDLE;
            QCLI_Display_Prompt();
            return QCLI_STATUS_ERROR_E;
        }
        QCLI_Display_Prompt();
    }
    else
        ret_val = QCLI_STATUS_USAGE_E;

    return(ret_val);
}

static QCLI_Command_Status_t SendGattPackets(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    qapi_BLE_GAP_LE_Address_Type_t addrType = 0;
    uint8_t  num_of_dev = 0;
    int index;
    uint8_t  repeat = 0;
    uint8_t  wait_counter = 0;

   if (!BluetoothStackID)
   {
       BluetoothStackID = GetBluetoothStackId();
       if (!BluetoothStackID)
       {
            QCLI_LOGE(mesh_coex_group, "Bluetooth Not Initialized\n");
            return QCLI_STATUS_ERROR_E;
       }
   }

    QCLI_LOGE(mesh_coex_group, "SendGattPackets\n");
    num_of_dev = Parameter_List[0].Integer_Value;

    if (num_of_dev > MAX_NUM_OF_DEVICES)
        return QCLI_STATUS_USAGE_E;

    if (Parameter_Count < 5)
        return QCLI_STATUS_USAGE_E;

    /* Convert the parameter to a Bluetooth Device Address.        */
    for(index = 0; index < num_of_dev; index++)
    {
        StrToBD_ADDR((char *)(Parameter_List[index+1].String_Value), &QMCT_BD_ADDR[index]);
    }

    QMCTConnInterval = Parameter_List[num_of_dev + 1].Integer_Value;
    QMCTGattDuration = Parameter_List[num_of_dev + 2].Integer_Value;
    QMCTWriteInterval = Parameter_List[num_of_dev + 3].Integer_Value;

    /* Check to see if scanning is in progress, if so cancel it.   */
    if(ScanInProgress)
    {
        /* Attempt to stop scanning.                                */
        if(!StopScan(BluetoothStackID))
           ScanInProgress = FALSE;

        QCLI_LOGE(mesh_coex_group, "Scan stopped before making LE Connection\n");
    }

    if (!CoExBluetoothInit)
    {
        Qmesh_Gatt_Data_T gattData;
        gattData.appGapLeEvtCallback = BLE_COEX_GAP_LE_Event_Callback;
        gattData.appGattEvtCallback = BLE_COEX_GATT_Connection_Event_Callback;
        gattData.provisioner_role_enabled = TRUE;

        if(!InitGattBearer(QMESH_GATT_BEARER_UNKOWN_SVC_E, NULL, &gattData))
        {
            QCLI_LOGE (mesh_coex_group, "FAILED to initialize GATT Bearer\n");
            return QCLI_STATUS_ERROR_E;
        }
    }

    /* Configure the Connection Parameters.  */
    BLEParameters.ConnectionParameters.Connection_Interval_Min    = QMCTConnInterval;
    BLEParameters.ConnectionParameters.Connection_Interval_Max    = QMCTConnInterval;
    BLEParameters.ConnectionParameters.Minimum_Connection_Length  = 0;
    BLEParameters.ConnectionParameters.Maximum_Connection_Length  = 10000;
    BLEParameters.ConnectionParameters.Slave_Latency              = 0;
    BLEParameters.ConnectionParameters.Supervision_Timeout        = 20000;

    /* Flag that the connection parameters are valid so that we do */
    /* not re-configure the defaults un-necessarily.               */
    BLEParameters.Flags |= BLE_PARAMETERS_FLAGS_CONNECTION_PARAMETERS_VALID;

    QMCTConnectionCount = 0;
    QMCTGattRunning = TRUE;

    /* Stop Qmesh Scheduler */
    if (!CoExBluetoothInit)
    {
        QmeshSchedStop();
    }

    for (index = 0; index < num_of_dev; index ++)
    {
        repeat = 5;
        while(repeat > 0)
        {
            DeviceStatus = 0;
            QCLI_LOGE(mesh_coex_group, "SendGattPackets : Create Connection 0x%s, attempt %d\n",
                            Parameter_List[index + 1].String_Value, (6 - repeat));

            /* Attempt to connect to the specified remote address */
            if (!ConnectLEDevice(BluetoothStackID, FALSE, &QMCT_BD_ADDR[index], addrType))
                ret_val = QCLI_STATUS_SUCCESS_E;
            else
                ret_val = QCLI_STATUS_ERROR_E;

            /* Wait for 2 sec */
            qurt_thread_sleep(400);
            repeat--;
            if ((!DeviceStatus) && ((repeat == 0) || (!QMCTGattRunning)))
            {
                /* Cancel LE Create Connection */
                CancelLECreateConnection();
                while (index > 0)
                {
                    DisconnectLEDevice(QMCT_BD_ADDR[index -1]);
                    index --;
                    qurt_thread_sleep(50);
                }
                /* Start QMESH Scheduler */
                if (!CoExBluetoothInit)
                {
                    QmeshSchedStart();
                }
                return QCLI_STATUS_ERROR_E;
            }
            else if (!DeviceStatus)
            {
                /* Cancel LE Create Connection */
                CancelLECreateConnection();
                continue;
            }
            wait_counter = 10;
            /* Wait till Device service discovery get completed , get connected */
            while (QMCTConnectionCount == index)
            {
                wait_counter--;
                QCLI_LOGE(mesh_coex_group, "SendGattPackets : Waiting for Service Discovery Completes !!\n");
                qurt_thread_sleep(100);
                if ((!QMCTGattRunning) || (!wait_counter))
                {
                    while (index >= 0)
                    {
                        DisconnectLEDevice(QMCT_BD_ADDR[index]);
                        index --;
                        qurt_thread_sleep(100);
                    }
                    /* Start QMESH Scheduler */
                    if (!CoExBluetoothInit)
                    {
                        QmeshSchedStart();
                    }
                    QCLI_Display_Prompt();
                    return QCLI_STATUS_ERROR_E;
                }
            }
            repeat = 0;
        }
    }

    /* Start QMESH Scheduler */
    if (!CoExBluetoothInit)
    {
        QmeshSchedStart();
    }

    /* Update data in QMCT Buffer */
    FillQMCTBuffer();

    /* Start Threads */
    for (index = 0; index < num_of_dev; index ++)
    {
        StartSendGattDataThread(&QMCT_BD_ADDR[index]);
    }

    QCLI_Display_Prompt();

    return(ret_val);
}

static QCLI_Command_Status_t SendMeshAndGattPackets(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    qapi_BLE_GAP_LE_Address_Type_t addrType = 0;
    uint8_t  num_of_dev = 0;
    int index;
    uint8_t  repeat = 0;

   if (!BluetoothStackID)
   {
       BluetoothStackID = GetBluetoothStackId();
       if (!BluetoothStackID)
       {
            QCLI_LOGE(mesh_coex_group, "Bluetooth Not Initialized\n");
            return QCLI_STATUS_ERROR_E;
       }
   }

    QCLI_LOGE(mesh_coex_group, "SendMeshAndGattPackets\n");
    num_of_dev = Parameter_List[0].Integer_Value;

    if (num_of_dev > MAX_NUM_OF_DEVICES)
        return QCLI_STATUS_USAGE_E;

    if (Parameter_Count < 7)
        return QCLI_STATUS_USAGE_E;

    /* Convert the parameter to a Bluetooth Device Address.        */
    for(index = 0; index < num_of_dev; index++)
    {
        StrToBD_ADDR((char *)(Parameter_List[index+1].String_Value), &QMCT_BD_ADDR[index]);
    }

    QMCTConnInterval = Parameter_List[num_of_dev + 1].Integer_Value;
    QMCTGattDuration = Parameter_List[num_of_dev + 2].Integer_Value;
    QMCTMeshDuration = Parameter_List[num_of_dev + 2].Integer_Value;
    QMCTWriteInterval = Parameter_List[num_of_dev + 3].Integer_Value;
    QMCTMeshSendElementAddr = Parameter_List[num_of_dev + 4].Integer_Value;
    QMCTMeshSendInterval = Parameter_List[num_of_dev + 5].Integer_Value;

    /* Check to see if scanning is in progress, if so cancel it.   */
    if(ScanInProgress)
    {
        /* Attempt to stop scanning.                                */
        if(!StopScan(BluetoothStackID))
           ScanInProgress = FALSE;

        QCLI_LOGE(mesh_coex_group, "Scan stopped before making LE Connection\n");
    }

    if (!CoExBluetoothInit)
    {
        Qmesh_Gatt_Data_T gattData;
        gattData.appGapLeEvtCallback = BLE_COEX_GAP_LE_Event_Callback;
        gattData.appGattEvtCallback = BLE_COEX_GATT_Connection_Event_Callback;
        gattData.provisioner_role_enabled = TRUE;

        if(!InitGattBearer(QMESH_GATT_BEARER_UNKOWN_SVC_E, NULL, &gattData))
        {
            QCLI_LOGE (mesh_coex_group, "FAILED to initialize GATT Bearer\n");
            return QCLI_STATUS_ERROR_E;
        }
    }

    /* Configure the Connection Parameters.  */
    BLEParameters.ConnectionParameters.Connection_Interval_Min    = QMCTConnInterval;
    BLEParameters.ConnectionParameters.Connection_Interval_Max    = QMCTConnInterval;
    BLEParameters.ConnectionParameters.Minimum_Connection_Length  = 0;
    BLEParameters.ConnectionParameters.Maximum_Connection_Length  = 10000;
    BLEParameters.ConnectionParameters.Slave_Latency              = 0;
    BLEParameters.ConnectionParameters.Supervision_Timeout        = 20000;

    /* Flag that the connection parameters are valid so that we do */
    /* not re-configure the defaults un-necessarily.               */
    BLEParameters.Flags |= BLE_PARAMETERS_FLAGS_CONNECTION_PARAMETERS_VALID;

    QMCTConnectionCount = 0;
    QMCTGattRunning = TRUE;
    QMCTMeshRunning = TRUE;

    /* Stop Qmesh Scheduler */
    if (!CoExBluetoothInit)
    {
        QmeshSchedStop();
    }

    for (index = 0; index < num_of_dev; index ++)
    {
        repeat = 5;
        while(repeat > 0)
        {
            DeviceStatus = 0;
            QCLI_LOGE(mesh_coex_group, "SendMeshAndGattPackets : Create Connection 0x%s, attempt %d\n",
                            Parameter_List[index + 1].String_Value, (11 - repeat));
            /* Attempt to connect to the specified remote address */
            if (!ConnectLEDevice(BluetoothStackID, FALSE, &QMCT_BD_ADDR[index], addrType))
                ret_val = QCLI_STATUS_SUCCESS_E;
            else
                ret_val = QCLI_STATUS_ERROR_E;

            /* Wait for 2 sec */
            qurt_thread_sleep(200);
            repeat--;
            if ((!DeviceStatus) && ((repeat == 0) || (!QMCTGattRunning)))
            {
                /* Cancel LE Create Connection */
                CancelLECreateConnection();
                while (index > 0)
                {
                    DisconnectLEDevice(QMCT_BD_ADDR[index -1]);
                    index --;
                    qurt_thread_sleep(500);
                }
                /* Start QMESH Scheduler */
                if (!CoExBluetoothInit)
                {
                    QmeshSchedStart();
                }
                return QCLI_STATUS_ERROR_E;
            }
            else if (!DeviceStatus)
            {
                /* Cancel LE Create Connection */
                CancelLECreateConnection();
                continue;
            }
            /* Wait till Device service discovery get completed , get connected */
            while (QMCTConnectionCount == index)
            {
                QCLI_LOGE(mesh_coex_group, "SendMeshAndGattPackets : Waiting for Service Discovery Completes !!\n", index + 1);
                qurt_thread_sleep(100);
                if (!QMCTGattRunning)
                {
                    while (index >= 0)
                    {
                        DisconnectLEDevice(QMCT_BD_ADDR[index]);
                        index --;
                        qurt_thread_sleep(500);
                    }
                    /* Start QMESH Scheduler */
                    if (!CoExBluetoothInit)
                    {
                        QmeshSchedStart();
                    }
                    return QCLI_STATUS_ERROR_E;
                }
            }
            repeat = 0;
        }
        QCLI_Display_Prompt();
    }

    /* Start QMESH Scheduler */
    if (!CoExBluetoothInit)
    {
        QmeshSchedStart();
    }

    /* Update data in QMCT Buffer */
    FillQMCTBuffer();

    /* Start Threads */
    StartSendMeshDataThread();
    for (index = 0; index < num_of_dev; index ++)
    {
        StartSendGattDataThread(&QMCT_BD_ADDR[index]);
    }

    return(ret_val);
}



static QCLI_Command_Status_t StopTestRunning(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_LOGE(mesh_coex_group, "StopTest\n");
    QMCTMeshRunning = FALSE ;
    QMCTGattRunning = FALSE ;

    return (QCLI_STATUS_SUCCESS_E);
}

   /* The following function is responsible for starting a scan.        */
static int StartScan(uint32_t BluetoothStackID, qapi_BLE_GAP_LE_Filter_Policy_t FilterPolicy, unsigned int ScanDuration)
{
    int Result;

    /* First, determine if the input parameters appear to be semi-valid. */
    if(BluetoothStackID)
    {
        /* Check to see if we need to configure the default Scan Parameters         */
        /* Configure the default Scan Window and Scan Interval.        */
        BLEParameters.ScanParameters.ScanWindow   = 50;
        BLEParameters.ScanParameters.ScanInterval = 100;

        /* Flag that the scan parameters are valid so that we do not   */
        /* re-configure the defaults un-necessarily.                   */
        BLEParameters.Flags |= BLE_PARAMETERS_FLAGS_SCAN_PARAMETERS_VALID;

        /* See if we should start a timer for this scan.                  */
        if(ScanDuration)
        {
            /* Start a timer for this operation.                           */
            Result = qapi_BLE_BSC_StartTimer(BluetoothStackID, (ScanDuration * 1000), BSC_Timer_Callback, ScanDuration);
            if(Result > 0)
            {
                /* Save the scan timer ID.                                  */
                ScanTimerID = (unsigned int)Result;
                Result = 0;
            }
            else
                QCLI_LOGE(mesh_coex_group, "Unable to start timer: %d\n", Result);
        }
        else
            Result = 0;

        /* Continue if no error occurred.                                 */
        if(!Result)
        {
            /* Not currently scanning, go ahead and attempt to perform the scan */
            if (CoExBluetoothInit)
            {
                Result = qapi_BLE_GAP_LE_Perform_Scan(BluetoothStackID, QAPI_BLE_ST_ACTIVE_E,
                                                      BLEParameters.ScanParameters.ScanInterval, BLEParameters.ScanParameters.ScanWindow,
                                                      QAPI_BLE_LAT_PUBLIC_E, FilterPolicy,
                                                      TRUE, BLE_COEX_GAP_LE_Event_Callback, 0);
            }
            else
            {
                Result = QmeshSchedPerformUserScan(QAPI_BLE_ST_ACTIVE_E, BLEParameters.ScanParameters.ScanInterval, 
                                        BLEParameters.ScanParameters.ScanWindow, QAPI_BLE_LAT_PUBLIC_E,
                                        FilterPolicy, TRUE, BLE_COEX_GAP_LE_Event_Callback, 0);
            }
            if(!Result)
                QCLI_LOGE(mesh_coex_group, "Scan started successfully. Scan Window: %u, Scan Interval: %u.\n", (unsigned int)BLEParameters.ScanParameters.ScanWindow, (unsigned int)BLEParameters.ScanParameters.ScanInterval);
            else
                QCLI_LOGE(mesh_coex_group, "Unable to perform scan: %d\n", Result);
        }
        else
            QCLI_LOGE(mesh_coex_group, "Unable to start scan timer: %d\n", Result);
   }
   else
      Result = -1;

   return(Result);
}

   /* The following function is responsible for stopping on on-going scan   */
static int StopScan(uint32_t BluetoothStackID)
{
    int Result;

    /* First, determine if the input parameters appear to be semi-valid. */
    if(BluetoothStackID)
    {
        QCLI_LOGI (mesh_coex_group, "StopScan.\n");
        /* if scan timer is active stop that.                             */
        if(ScanTimerID)
        {
            /* Stop the timer.                                             */
            qapi_BLE_BSC_StopTimer(BluetoothStackID, ScanTimerID);
            ScanTimerID = 0;
        }
        /* Stop the scan.*/
        if (CoExBluetoothInit)
        {
            Result = qapi_BLE_GAP_LE_Cancel_Scan(BluetoothStackID);
        }
        else
        {
            Result = QmeshSchedCancelUserScan();
        }
        if(!Result)
        {
            QCLI_LOGE(mesh_coex_group, "Scan stopped successfully.\n");
            /* Flag that scanning is not in progess.                       */
            ScanInProgress = FALSE;
        }
        else
            QCLI_LOGE(mesh_coex_group, "Unable to stop scan: %d\n", Result);
    }
    else
        Result = -1;

    return(Result);
}

   /* The following function is responsible for creating an LE          */
   /* connection to the specified Remote Device.                        */
   /* * NOTE * If UseWhiteList is TRUE, then the BD_ADDR may be excluded*/
   /*          (NULL), and the address type will be ignored.  Otherwise,*/
   /*          they MUST be specified.  A remote device address and     */
   /*          address type MUST have been added to the White List in   */
   /*          the controller to use this functionality.                */
   /* * NOTE * If UseWhiteList is FALSE, then this function will perform*/
   /*          no checks on the remote addresss or address type.        */
   /* * NOTE * If Address Resolution has been enabled in the controller */
   /*          and the remote device's identity information has been    */
   /*          added to the Resolving List in the controller, then the  */
   /*          local controller will generate a resolvable private      */
   /*          address (RPA) to send in the connection request.         */
   /* * NOTE * If the user wishes to use the White List and the         */
   /*          Resolving List in the controller, then the remote device */
   /*          address and address type specified for the added White   */
   /*          List entry MUST correspond to the identity address and   */
   /*          identity address type for the remote device.             */
   /* * NOTE * If the user wishes to ONLY use the Resolving List in the */
   /*          controller (Not White List), then the remote device      */
   /*          address and address type specified to this function MUST */
   /*          correspond to the identity address and identity address  */
   /*          type for the remote device.                              */
static int ConnectLEDevice(uint32_t BluetoothStackID, boolean_t UseWhiteList, qapi_BLE_BD_ADDR_t *BD_ADDR, qapi_BLE_GAP_LE_Address_Type_t AddressType)
{
   int Result;

   /* First, determine if the input parameters appear to be semi-valid. */
   if(BluetoothStackID)
   {
      /* Check to see if we need to configure the default Scan Parameters         */
      /* Configure the default Scan Window and Scan Interval.        */
      BLEParameters.ScanParameters.ScanWindow = 4;
      BLEParameters.ScanParameters.ScanInterval = 4;

      /* Flag that the scan parameters are valid so that we do not   */
      /* re-configure the defaults un-necessarily.                   */
      BLEParameters.Flags |= BLE_PARAMETERS_FLAGS_SCAN_PARAMETERS_VALID;

      /* Check to see if we need to configure the default Connection Parameters  */
      /* Initialize the default connection parameters.               */
      BLEParameters.ConnectionParameters.Connection_Interval_Min    = QMCTConnInterval;
      BLEParameters.ConnectionParameters.Connection_Interval_Max    = QMCTConnInterval;
      BLEParameters.ConnectionParameters.Minimum_Connection_Length  = 0;
      BLEParameters.ConnectionParameters.Maximum_Connection_Length  = 10000;
      BLEParameters.ConnectionParameters.Slave_Latency              = 0;
      BLEParameters.ConnectionParameters.Supervision_Timeout        = 20000;

      /* Flag that the connection parameters are valid so that we do */
      /* not re-configure the defaults un-necessarily.               */
      BLEParameters.Flags |= BLE_PARAMETERS_FLAGS_CONNECTION_PARAMETERS_VALID;

      /* Everything appears correct, go ahead and attempt to make the   */
      /* connection.                                                    */
      /* * NOTE * Our local address type will ALWAYS be resolvable      */
      /*          fallback public, in case the Resolving List in the    */
      /*          controller is used for connecting.  It is worth noting*/
      /*          that this demo uses its local/public address as the   */
      /*          public identity address for simplicity.               */

      Result = qapi_BLE_GAP_LE_Create_Connection(BluetoothStackID, BLEParameters.ScanParameters.ScanInterval, BLEParameters.ScanParameters.ScanWindow, (UseWhiteList ? QAPI_BLE_FP_WHITE_LIST_E : QAPI_BLE_FP_NO_FILTER_E), AddressType, BD_ADDR, QAPI_BLE_LAT_RESOLVABLE_FALLBACK_PUBLIC_E, &(BLEParameters.ConnectionParameters), BLE_COEX_GAP_LE_Event_Callback, 0);
      if(!Result)
      {
         QCLI_LOGE(mesh_coex_group, "Connection Request successful.\n");
         QCLI_LOGE(mesh_coex_group, "Scan Parameters:       Window %u, Interval %u.\n", (unsigned int)BLEParameters.ScanParameters.ScanWindow,
                                                                                     (unsigned int)BLEParameters.ScanParameters.ScanInterval);
         QCLI_LOGE(mesh_coex_group, "Connection Parameters: Interval Range %u - %u, Slave Latency %u.\n", (unsigned int)BLEParameters.ConnectionParameters.Connection_Interval_Min,
                                                                                                       (unsigned int)BLEParameters.ConnectionParameters.Connection_Interval_Max,
                                                                                                       (unsigned int)BLEParameters.ConnectionParameters.Slave_Latency);
         QCLI_LOGE(mesh_coex_group, "Using White List:      %s.\n", (UseWhiteList ? "Yes" : "No"));
      }
      else
      {
         /* Unable to create connection.                                */
         QCLI_LOGE(mesh_coex_group, "Unable to create connection: %d.\n", Result);
      }
   }
   else
      Result = -1;

   return(Result);
}

   /* The following function is provided to properly print a UUID.      */
static void DisplayUUID(qapi_BLE_GATT_UUID_t *UUID)
{
   if(UUID)
   {
      if(UUID->UUID_Type == QAPI_BLE_GU_UUID_16_E)
         QCLI_LOGE(mesh_coex_group, "%02X%02X\n", UUID->UUID.UUID_16.UUID_Byte1, UUID->UUID.UUID_16.UUID_Byte0);
      else
      {
         if(UUID->UUID_Type == QAPI_BLE_GU_UUID_128_E)
         {
            QCLI_LOGE(mesh_coex_group, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n", UUID->UUID.UUID_128.UUID_Byte15, UUID->UUID.UUID_128.UUID_Byte14, UUID->UUID.UUID_128.UUID_Byte13,
                                                                                                    UUID->UUID.UUID_128.UUID_Byte12, UUID->UUID.UUID_128.UUID_Byte11, UUID->UUID.UUID_128.UUID_Byte10,
                                                                                                    UUID->UUID.UUID_128.UUID_Byte9,  UUID->UUID.UUID_128.UUID_Byte8,  UUID->UUID.UUID_128.UUID_Byte7,
                                                                                                    UUID->UUID.UUID_128.UUID_Byte6,  UUID->UUID.UUID_128.UUID_Byte5,  UUID->UUID.UUID_128.UUID_Byte4,
                                                                                                    UUID->UUID.UUID_128.UUID_Byte3,  UUID->UUID.UUID_128.UUID_Byte2,  UUID->UUID.UUID_128.UUID_Byte1,
                                                                                                    UUID->UUID.UUID_128.UUID_Byte0);
         }
      }
   }
}

/* GATT Server Implementation : START */
static QCLI_Command_Status_t AdvertiseLE(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    int                               Result;
    QCLI_Command_Status_t             ret_val = QCLI_STATUS_SUCCESS_E;
    unsigned int                      Length;
    unsigned int                      UUIDIndex;
    qapi_BLE_GAP_LE_Connectability_Parameters_t ConnectabilityParameters;
    unsigned int                      AdvertDuration;

    union
    {
      qapi_BLE_Advertising_Data_t              AdvertisingData;
      qapi_BLE_Scan_Response_Data_t            ScanResponseData;
    } Advertisement_Data_Buffer;

    enum
    {
      ADVRT_DISABLE_E,
      ADVRT_ENABLE_E
    };

    if (!BluetoothStackID)
    {
        BluetoothStackID = GetBluetoothStackId();
        if (!BluetoothStackID)
        {
            QCLI_LOGE(mesh_coex_group, "Bluetooth Not Initialized\n");
            return QCLI_STATUS_ERROR_E;
        }
    }

    if (Parameter_Count == 0)
    {
        return QCLI_STATUS_USAGE_E;
    }

    if(Parameter_List[0].Integer_Value == ADVRT_DISABLE_E)
    {
        /* Disable Advertising.                                     */
        if (CoExBluetoothInit)
        {
            Result = qapi_BLE_GAP_LE_Advertising_Disable(BluetoothStackID);
            if(!Result)
            {
               QCLI_LOGE (mesh_coex_group, "GAP_LE_Advertising_Disable success.\n");
               ret_val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               QCLI_LOGE (mesh_coex_group, "GAP_LE_Advertising_Disable returned %d.\n", Result);
               ret_val = QCLI_STATUS_ERROR_E;
            }
        }
        else
        {
            QCLI_LOGE (mesh_coex_group, "Disable Advert\n");
            QmeshSchedUserAdvertDisable();
        }
        if (AdvertTimerID != QMESH_TIMER_INVALID_HANDLE)
        {
            /* Stop Advert timer                           */
            qapi_BLE_BSC_StopTimer(BluetoothStackID, AdvertTimerID);
            AdvertTimerID = QMESH_TIMER_INVALID_HANDLE;
        }
    }
    else
    {
        /* Set the Advertising Data.                                */
        memset(&(Advertisement_Data_Buffer.AdvertisingData), 0, sizeof(qapi_BLE_Advertising_Data_t));

        /* Reset the length to zero.                                */
        Length = 0;

        /* Set the Flags A/D Field (1 byte type and 1 byte Flags.   */
        Advertisement_Data_Buffer.AdvertisingData.Advertising_Data[Length] = 2;
        Advertisement_Data_Buffer.AdvertisingData.Advertising_Data[Length+1] = QAPI_BLE_HCI_LE_ADVERTISING_REPORT_DATA_TYPE_FLAGS;
        Advertisement_Data_Buffer.AdvertisingData.Advertising_Data[Length+2] = 0;

        /* Configure the flags field based on the Discoverability Mode*/
        Advertisement_Data_Buffer.AdvertisingData.Advertising_Data[Length+2] = QAPI_BLE_HCI_LE_ADVERTISING_FLAGS_GENERAL_DISCOVERABLE_MODE_FLAGS_BIT_MASK;
        Advertisement_Data_Buffer.AdvertisingData.Advertising_Data[Length + 2] |= QAPI_BLE_HCI_LE_ADVERTISING_FLAGS_BR_EDR_NOT_SUPPORTED_FLAGS_BIT_MASK;

        /* Update the current length of the advertising data.*/
        Length += (Advertisement_Data_Buffer.AdvertisingData.Advertising_Data[Length] + 1);

        /* Make sure we have room in the buffer.                    */
        if((Length + (unsigned int)QAPI_BLE_UUID_16_SIZE + 2) <= (unsigned int)QAPI_BLE_ADVERTISING_DATA_MAXIMUM_SIZE)
        {
            ((uint8_t *)&Advertisement_Data_Buffer.AdvertisingData)[Length] = 1;
            ((uint8_t *)&Advertisement_Data_Buffer.AdvertisingData)[Length+1] = QAPI_BLE_HCI_LE_ADVERTISING_REPORT_DATA_TYPE_16_BIT_SERVICE_UUID_COMPLETE;

            /* Store the UUID Index location.                        */
            UUIDIndex = (Length + 2);
            /* If CoEx is registered.                                */
            if(CoExInstanceID)
            {
                /* Make sure we do not exceed the bounds of the buffer */
                if((Length + ((uint8_t *)&Advertisement_Data_Buffer.AdvertisingData)[Length] + 1 + QAPI_BLE_UUID_16_SIZE) <= (unsigned int)QAPI_BLE_ADVERTISING_DATA_MAXIMUM_SIZE)
                {
                    /* Assign the CoEx Service UUID.                   */
                    ASSIGN_COEX_SERVICE_UUID_16(&(((uint8_t *)&Advertisement_Data_Buffer.AdvertisingData)[UUIDIndex]));

                    /* Update the UUID Index.                          */
                    UUIDIndex += QAPI_BLE_UUID_16_SIZE;

                    /* Update the advertising report data entry length */
                    /* since we have added another UUID.       */
                    ((uint8_t *)&Advertisement_Data_Buffer.AdvertisingData)[Length] += QAPI_BLE_UUID_16_SIZE;
                }
                else
                {
                     /* Flag that we could not include all the service UUIDs */
                     ((uint8_t *)&Advertisement_Data_Buffer.AdvertisingData)[Length+1] = QAPI_BLE_HCI_LE_ADVERTISING_REPORT_DATA_TYPE_16_BIT_SERVICE_UUID_PARTIAL;
                }
            }

            /* Update the current length of the advertising data.    */
            Length += (((uint8_t *)&Advertisement_Data_Buffer.AdvertisingData)[Length] + 1);

            /* Start a timer for this advert.                  */
            if((Parameter_Count >= 2) && Parameter_List[1].Integer_Value)
            {
                AdvertDuration = Parameter_List[1].Integer_Value;
                /* Start a timer for this operation.                           */
                Result = qapi_BLE_BSC_StartTimer(BluetoothStackID, (AdvertDuration * 1000), Advert_Timer_Callback, AdvertDuration);
                if(Result > 0)
                {
                    /* Save the scan timer ID.                                  */
                    AdvertTimerID = (unsigned int)Result;
                    Result = 0;
                }
                else
                    QCLI_LOGE(mesh_coex_group, "Unable to start timer: %d\n", Result);
            }
            else
                Result = 0;

            /* Write the advertising data to the chip.                  */
            if (CoExBluetoothInit)
            {
                Result = qapi_BLE_GAP_LE_Set_Advertising_Data(BluetoothStackID, Length, &(Advertisement_Data_Buffer.AdvertisingData));
            }
            else
            {
                Result = QmeshSchedSetUserAdvertData(Length, &(Advertisement_Data_Buffer.AdvertisingData));
            }
            if(!Result)
            {
                /* Configure the advertising channel map for the      */
                /* default advertising parameters.                    */
                BLEParameters.AdvertisingParameters.Advertising_Channel_Map = QAPI_BLE_HCI_LE_ADVERTISING_CHANNEL_MAP_DEFAULT;

                /* Set up the default advertising parameters if they  */
                /* have not been configured at the CLI.               */
                if(!(BLEParameters.Flags & BLE_PARAMETERS_FLAGS_ADVERTISING_PARAMETERS_VALID))
                {
                    /* Configure the remaining default advertising parameters  */
                    BLEParameters.AdvertisingParameters.Scan_Request_Filter       = QAPI_BLE_FP_NO_FILTER_E;
                    BLEParameters.AdvertisingParameters.Connect_Request_Filter    = QAPI_BLE_FP_NO_FILTER_E;
                    BLEParameters.AdvertisingParameters.Advertising_Interval_Min  = 100;
                    BLEParameters.AdvertisingParameters.Advertising_Interval_Max  = 200;

                    /* Flag that the parameters are valid so we don't  */
                    /* set them unnecessarily.                         */
                    BLEParameters.Flags |= BLE_PARAMETERS_FLAGS_ADVERTISING_PARAMETERS_VALID;
                }
                /* Initialize the direct address to zero and the type */
                /* to public.                                         */
                ConnectabilityParameters.Direct_Address_Type   = QAPI_BLE_LAT_PUBLIC_E;
                QAPI_BLE_ASSIGN_BD_ADDR(ConnectabilityParameters.Direct_Address, 0, 0, 0, 0, 0, 0);
                /* Configure the Connectability Parameters.           */
                ConnectabilityParameters.Own_Address_Type = QAPI_BLE_LAT_PUBLIC_E;
                ConnectabilityParameters.Connectability_Mode = QAPI_BLE_LCM_CONNECTABLE_E;
                /* Inform the user.                             */
                QCLI_LOGE(mesh_coex_group, "Using connectable un-directed advertising with public address.\n");

                if (CoExBluetoothInit)
                {
                    /* Now enable advertising.                         */
                    Result = qapi_BLE_GAP_LE_Advertising_Enable(BluetoothStackID, TRUE, &(BLEParameters.AdvertisingParameters), &(ConnectabilityParameters), CoExGapLeEventCallback, 0);
                }
                else
                {
                    Result = QmeshSchedUserAdvertEnable(BluetoothStackID, &(BLEParameters.AdvertisingParameters), &(ConnectabilityParameters), CoExGapLeEventCallback, 0);
                }
                if(!Result)
                {
                    QCLI_LOGE(mesh_coex_group, "Advert Enable success, Advertising Interval Range: %u - %u.\n", (unsigned int)BLEParameters.AdvertisingParameters.Advertising_Interval_Min, (unsigned int)BLEParameters.AdvertisingParameters.Advertising_Interval_Max);
                    ret_val = QCLI_STATUS_SUCCESS_E;
                }
                else
                {
                    QCLI_LOGE(mesh_coex_group, "Advert Enable returned %d.\n", Result);
                    ret_val = QCLI_STATUS_ERROR_E;
                }
            }
            else
            {
                QCLI_LOGE(mesh_coex_group, "qapi_BLE_GAP_LE_Set_Advertising_Data returned %d.\n", Result);
                ret_val = QCLI_STATUS_ERROR_E;
            }
        }
    }

    return ret_val;
}

static COEX_GATT_DEVICE_INFO_T* getCoExDeviceInfo(uint32_t connectionId)
{
    COEX_GATT_DEVICE_INFO_T* devInfo = NULL;
    uint8_t idx;

    for (idx = 0; idx < MAX_NUM_OF_COEX_GATT_CONN; idx++)
    {
        if (coexGATTServerData->devInfo[idx].connectionId == connectionId)
        {
            QCLI_LOGE (mesh_coex_group, "getCoExDeviceInfo : At idx %x, connId %x\n",idx, connectionId);
            devInfo = (COEX_GATT_DEVICE_INFO_T*)&coexGATTServerData->devInfo[idx];
            break;
        }
    }

    return devInfo;
}

/* ***************************************************************** */
/*                         Event Callbacks                           */
/* ***************************************************************** */
static void QAPI_BLE_BTPSAPI CoExGapLeEventCallback(uint32_t btStackId, 
        qapi_BLE_GAP_LE_Event_Data_t *GAP_LE_Event_Data, uint32_t CallbackParameter)
{
    if((btStackId == BluetoothStackID) && (GAP_LE_Event_Data))
    {
        QCLI_LOGE (mesh_coex_group, "CoExGapLeEventCallback Event received %x \n", GAP_LE_Event_Data->Event_Data_Type);
        switch(GAP_LE_Event_Data->Event_Data_Type)
        {
            case QAPI_BLE_ET_LE_CONNECTION_COMPLETE_E:
            {
                qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t *data = 
                    GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Complete_Event_Data;

                QCLI_LOGE (mesh_coex_group, "Received QAPI_BLE_ET_LE_CONNECTION_COMPLETE_E status=%x\n",data->Status);
            }
            break;

            case QAPI_BLE_ET_LE_DISCONNECTION_COMPLETE_E:
            {
                qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t *data = 
                    GAP_LE_Event_Data->Event_Data.GAP_LE_Disconnection_Complete_Event_Data;

                QCLI_LOGE (mesh_coex_group, "Received QAPI_BLE_ET_LE_DISCONNECTION_COMPLETE_E reason=%x status=%x\n", 
                    data->Reason, data->Status);
            }
            break;

            case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_REQUEST_E:
            {
                QCLI_LOGE (mesh_coex_group, "Received QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_REQUEST_E\n");
            }
            break;

            case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_RESPONSE_E:
            {
                 qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t *data = 
                    GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Update_Response_Event_Data;

                 QCLI_LOGE (mesh_coex_group, "Received QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_RESPONSE_E, accepted=%d \n", data->Accepted);
            }
            break;

            case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATED_E:
            {
                qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t *data = 
                    GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Updated_Event_Data;

                QCLI_LOGE (mesh_coex_group, "Received QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATED_E,  Status=%d, ConnIntvl=%x, slaceLatency=%x, supervisionTimeout=%x\n", 
                    data->Status, data->Current_Connection_Parameters.Connection_Interval, data->Current_Connection_Parameters.Slave_Latency,
                    data->Current_Connection_Parameters.Supervision_Timeout);
            }
            break;

            default:
            {
                QCLI_LOGE (mesh_coex_group, " %s Event %x not handled\n", __FUNCTION__,GAP_LE_Event_Data->Event_Data_Type);
            }
            break;
        }
    }
}

static void QAPI_BLE_BTPSAPI CoExGattServerEventCallback(uint32_t btStackID,
    qapi_BLE_GATT_Server_Event_Data_t *GattServerEventData, uint32_t CallbackParameter)
{
    COEX_GATT_DEVICE_INFO_T*  devInfo = NULL;
    static int readReqCount = 0;

    if((btStackID == BluetoothStackID) && (GattServerEventData))
    {
        switch(GattServerEventData->Event_Data_Type)
        {
            case QAPI_BLE_ET_GATT_SERVER_DEVICE_CONNECTION_E:
            {
                if (GattServerEventData->Event_Data.GATT_Device_Connection_Data)
                {
                    qapi_BLE_GATT_Device_Connection_Data_t *connData =
                        GattServerEventData->Event_Data.GATT_Device_Connection_Data;

                    QCLI_LOGE (mesh_coex_group, "DEVICE CONNECTED Conn Id %x, MTU %x, Remote BDADDR =%x:%x:%x:%x:%x:%x\n",
                        connData->ConnectionID, connData->MTU, connData->RemoteDevice.BD_ADDR0, connData->RemoteDevice.BD_ADDR1,
                        connData->RemoteDevice.BD_ADDR2, connData->RemoteDevice.BD_ADDR3,connData->RemoteDevice.BD_ADDR4, connData->RemoteDevice.BD_ADDR5);

                    devInfo = getCoExDeviceInfo(0);
                    if (devInfo != NULL)
                    {
                        QCLI_LOGE (mesh_coex_group, "CONN :: Conn Id %x\n", connData->ConnectionID);
                        /* Save Connection ID , Remote Device Address       */
                        devInfo->connectionId = connData->ConnectionID;
                        memcpy(&devInfo->devAddr, &connData->RemoteDevice , sizeof(qapi_BLE_BD_ADDR_t));
                        devInfo->notifyEnabled = FALSE;
                        memset(&devInfo->ccdHndlValue, 0 , CHAR_CCD_WRITE_VALUE_LENGTH);
                    }
                }
            }
            break;

            case QAPI_BLE_ET_GATT_SERVER_DEVICE_DISCONNECTION_E:
            {
                 if (GattServerEventData->Event_Data.GATT_Device_Disconnection_Data)
                {
                    qapi_BLE_GATT_Device_Disconnection_Data_t *discData =
                        GattServerEventData->Event_Data.GATT_Device_Disconnection_Data;

                    QCLI_LOGE (mesh_coex_group, "DEVICE DISCONNECTED Conn Id %x, Remote BDADDR =%x:%x:%x:%x:%x:%x\n",
                        discData->ConnectionID, discData->RemoteDevice.BD_ADDR0, discData->RemoteDevice.BD_ADDR1, discData->RemoteDevice.BD_ADDR2,
                        discData->RemoteDevice.BD_ADDR3,discData->RemoteDevice.BD_ADDR4, discData->RemoteDevice.BD_ADDR5);

                    devInfo = getCoExDeviceInfo(discData->ConnectionID);
                    if (NULL != devInfo)
                    {
                        QCLI_LOGE (mesh_coex_group, "DISC :: Conn Id %x", discData->ConnectionID);
                        /* Reset devInfo data */
                        memset(devInfo, 0, sizeof(COEX_GATT_DEVICE_INFO_T));
                    }
                }
            }
            break;

            case QAPI_BLE_ET_GATT_SERVER_READ_REQUEST_E:
            {
                if(GattServerEventData->Event_Data.GATT_Read_Request_Data)
                {
                    qapi_BLE_GATT_Read_Request_Data_t *rrd =
                        GattServerEventData->Event_Data.GATT_Read_Request_Data;

                    QCLI_LOGE (mesh_coex_group, "READ REQUEST Conn Id %x, TransId %x, SrvcId %u, AttrOffset %x\n",rrd->ConnectionID, rrd->TransactionID, rrd->ServiceID, rrd->AttributeOffset);

                    devInfo = getCoExDeviceInfo(rrd->ConnectionID);
                    if ((NULL != devInfo) && (rrd->ServiceID == coexGATTServerData->srvcId))
                    {
                        /* Read Request for CCD Handle */
                        if (rrd->AttributeOffset == COEX_DATA_OUT_CHAR_CCD_ATT_OFFSET)
                        {
                            QCLI_LOGE (mesh_coex_group, "Sending READ RESPONSE with TransId %x for Srvc %d\n",rrd->TransactionID, coexGATTServerData->srvcId);
                            QCLI_LOGE (mesh_coex_group, "Sending Read Response AttrValue [%x %x]\n", 
                                devInfo->ccdHndlValue[0], devInfo->ccdHndlValue[1]);
                            /* Sending Read Response for Proxy CCD Handle */
                            qapi_BLE_GATT_Read_Response(BluetoothStackID, rrd->TransactionID, CHAR_CCD_WRITE_VALUE_LENGTH, devInfo->ccdHndlValue);
                        }
                        else if (rrd->AttributeOffset == COEX_DATA_IN_CHAR_ATT_OFFSET)
                        {
                            QCLI_LOGE (mesh_coex_group, "Sending READ RESPONSE with TransId %x for Srvc %d\n",rrd->TransactionID, coexGATTServerData->srvcId);
                            QCLI_LOGE (mesh_coex_group, "Sending Read Response AttrValue [%x %x]\n", 
                                devInfo->ccdHndlValue[0], devInfo->ccdHndlValue[1]);
                            readReqCount++;
                            qapi_BLE_GATT_Read_Response(BluetoothStackID, rrd->TransactionID, sizeof(readReqCount), (uint8_t *)&readReqCount);
                        }
                        else
                        {
                             QCLI_LOGE (mesh_coex_group, "Not Handled : Read Request Received for handle with offset %x\n",rrd->AttributeOffset);
                        }
                    }
                }
            }
            break;

            case QAPI_BLE_ET_GATT_SERVER_WRITE_REQUEST_E:
            {
                if(GattServerEventData->Event_Data.GATT_Write_Request_Data)
                {
                    qapi_BLE_GATT_Write_Request_Data_t *wrd =
                        GattServerEventData->Event_Data.GATT_Write_Request_Data;

                    QCLI_LOGE (mesh_coex_group, "WRITE REQUEST Conn Id %x, TransId %x, SrvcId %u, AttrOffset %x, valueLen %x\n",
                               wrd->ConnectionID, wrd->TransactionID, wrd->ServiceID, wrd->AttributeOffset, wrd->AttributeValueLength);

                    devInfo = getCoExDeviceInfo(wrd->ConnectionID);
                    if ((NULL != devInfo) && (wrd->ServiceID == coexGATTServerData->srvcId))
                    {
                        /*CCC value written for enable/diable notification */
                        if (wrd->AttributeOffset == COEX_DATA_OUT_CHAR_CCD_ATT_OFFSET)
                        {
                            uint16_t ntfValue;
                            //bool sendConnUpdateReq = FALSE;

                            QmeshMemCpy((void *) &ntfValue, wrd->AttributeValue, CHAR_CCD_WRITE_VALUE_LENGTH);
                            if(ntfValue > 0)
                            {
                                QCLI_LOGE (mesh_coex_group, "Notification enabled by remote device\n");
                                /* Store Proxy CCD handle value */
                                QmeshMemCpy(devInfo->ccdHndlValue, wrd->AttributeValue, CHAR_CCD_WRITE_VALUE_LENGTH);
                                QCLI_LOGE (mesh_coex_group, "Write Request AttrValue [%x %x]\n", 
                                                        devInfo->ccdHndlValue[0], devInfo->ccdHndlValue[1]);
                                /* Set Notify enable flag */
                                devInfo->notifyEnabled = devInfo->ccdHndlValue[0] == 1 ? TRUE : FALSE;
                                /* Sending Write Response */
                                QCLI_LOGE (mesh_coex_group, "CCD Sending Write Response TransId %x\n", wrd->TransactionID);
                                //sendConnUpdateReq = TRUE;
                            }
                            else
                            {
                                QCLI_LOGE (mesh_coex_group, "Notifications has been disabled\n");
                            }

                            /* Sending Write Response */
                            qapi_BLE_GATT_Write_Response(BluetoothStackID, wrd->TransactionID);

                            /* Send connection parameter update request to remote device */
                            /*if(sendConnUpdateReq)
                            {
                                SendConnectionParamUpdateRequest(devInfo->devAddr);
                            }*/
                        }
                        else
                        {
                            /* Data received via write-request to be passed on to stack */
                            QCLI_LOGE (mesh_coex_group, "AttrValLen %x\n", wrd->AttributeValueLength);
                            qapi_BLE_GATT_Write_Response(BluetoothStackID, wrd->TransactionID);
                        }
                    }
                    else
                    {
                        QCLI_LOGE (mesh_coex_group, "Error -Either the serviceId or connectionID or attributeoffset \
                            is wrong\n");
                    }
                }
            }
            break;
            default:
            {
                QCLI_LOGE (mesh_coex_group, "Ignoring the received message %x\n",
                                 GattServerEventData->Event_Data_Type);
            }
            break;
        }
    }
}


static QCLI_Command_Status_t RegisterCoExService(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;
    int32_t result;

    if (!BluetoothStackID)
    {
        BluetoothStackID = GetBluetoothStackId();
        if (!BluetoothStackID)
        {
            QCLI_LOGE(mesh_coex_group, "Bluetooth Not Initialized\n");
            return QCLI_STATUS_ERROR_E;
        }
    }

    if (coexGATTServerData == NULL)
    {
        coexGATTServerData = (COEX_GATT_SERVER_DATA_T*) malloc(sizeof(COEX_GATT_SERVER_DATA_T));
        memset(coexGATTServerData, 0, sizeof(COEX_GATT_SERVER_DATA_T));
    }

    if (coexGATTServerData->srvcId == 0)
    {
        coexGATTServerData->hndleDetails.Starting_Handle = 0;
        coexGATTServerData->hndleDetails.Ending_Handle = 0;

        result = qapi_BLE_GATT_Register_Service(BluetoothStackID,
                            QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE,
                            COEX_SERVICE_ATTR_COUNT,
                            (qapi_BLE_GATT_Service_Attribute_Entry_t *)CoEx_Service,
                            &coexGATTServerData->hndleDetails,
                            CoExGattServerEventCallback, 0);
        if(result > 0)
        {
            /* Display success message.                                 */
            QCLI_LOGE (mesh_coex_group, "Successfully registered CoEx Service\n");
            QCLI_LOGE (mesh_coex_group, "ServiceID = %u, Start Handle %x End Handle %x\n", result,
                                        coexGATTServerData->hndleDetails.Starting_Handle,
                                        coexGATTServerData->hndleDetails.Ending_Handle);
            coexGATTServerData->srvcId = result;
            CoExInstanceID = (uint32_t)result;
        }
        else
        {
            QCLI_LOGE (mesh_coex_group, "Error - qapi_BLE_GATT_Register_Service() returned %d.\n", result);
            ret_val = QCLI_STATUS_ERROR_E;
        }
    }
    else
    {
        QCLI_LOGE (mesh_coex_group, "Service is already Registered.\n");
    }

    return ret_val;
}

static QCLI_Command_Status_t DeregisterCoExService(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Command_Status_t ret_val = QCLI_STATUS_SUCCESS_E;

    if (!BluetoothStackID)
    {
        BluetoothStackID = GetBluetoothStackId();
        if (!BluetoothStackID)
        {
            QCLI_LOGE(mesh_coex_group, "Bluetooth Not Initialized\n");
            return QCLI_STATUS_ERROR_E;
        }
    }

    if (coexGATTServerData == NULL)
    {
        QCLI_LOGE (mesh_coex_group, "CoEx Service Not Registered\n");
        return QCLI_STATUS_ERROR_E;
    }

    if (coexGATTServerData->srvcId != 0)
    {
        qapi_BLE_GATT_Un_Register_Service(BluetoothStackID, coexGATTServerData->srvcId);
        coexGATTServerData->srvcId = 0;
        CoExInstanceID = 0;
        QmeshMemSet(coexGATTServerData->devInfo, 0, sizeof(COEX_GATT_DEVICE_INFO_T) * MAX_NUM_OF_COEX_GATT_CONN);
        QCLI_LOGE (mesh_coex_group, "Deregister CoEx Service Successfully\n");
    }
    else
    {
        QCLI_LOGE (mesh_coex_group, "Invalid Service Id\n");
        ret_val = QCLI_STATUS_ERROR_E;
    }

    return ret_val;
}


/* GATT Server Implementation End */

   /* Generic Attribute Profile (GATT) QCLI command functions.          */

   /* The following function is responsible for performing a GATT       */
   /* Service Discovery Operation.  This function will return zero on   */
   /* successful execution and a negative value on errors.            */
static QCLI_Command_Status_t DiscoverServices(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   int                                     Result;
   DeviceInfo_t                           *DeviceInfo;
   QCLI_Command_Status_t                   ret_val;

   /* Verify that there is a connection that is established.            */
   if(ConnectionCount)
   {
      /* Lock the Bluetooth stack.                                      */
      if(!qapi_BLE_BSC_LockBluetoothStack(BluetoothStackID))
      {
         /* Get the device info for the connection device.              */
         if((DeviceInfo = SearchDeviceInfoEntryByBD_ADDR(&DeviceInfoList, SelectedRemoteBD_ADDR)) != NULL)
         {
            /* Verify that no service discovery is outstanding for this device */
            if(!(DeviceInfo->Flags & DEVICE_INFO_FLAGS_SERVICE_DISCOVERY_OUTSTANDING))
            {
               Result = qapi_BLE_GATT_Start_Service_Discovery(BluetoothStackID, DeviceInfo->ConnectionID, 0, NULL, BLE_COEX_GATT_Service_Discovery_Event_Callback, 0);
               if(!Result)
               {
                  /* Display success message.                           */
                  QCLI_LOGE(mesh_coex_group, "qapi_BLE_GATT_Service_Discovery_Start() success.\n");

                  /* Flag that a Service Discovery Operation is outstanding   */
                  DeviceInfo->Flags |= DEVICE_INFO_FLAGS_SERVICE_DISCOVERY_OUTSTANDING;
                  ret_val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  /* An error occur so just clean-up.                   */
                  QCLI_LOGE(mesh_coex_group, "Error - GATT_Service_Discovery_Start returned %d.\n", Result);
                  ret_val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_LOGE(mesh_coex_group, "Service Discovery Operation Outstanding for Device.\n");
               ret_val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_LOGE(mesh_coex_group, "No Device Info.\n");
            ret_val = QCLI_STATUS_ERROR_E;
         }

         /* Un-lock the Bluetooth Stack.                                */
         qapi_BLE_BSC_UnLockBluetoothStack(BluetoothStackID);
      }
      else
      {
         QCLI_LOGE(mesh_coex_group, "Unable to acquire Bluetooth Stack Lock.\n");
         ret_val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_LOGE(mesh_coex_group, "No Connection Established\n");
      ret_val = QCLI_STATUS_ERROR_E;
   }

   return(ret_val);
}

   /* Generic Access Profile Service (GAPS) helper functions.           */

   /* The following function is a utility function that provides a      */
   /* mechanism of populating discovered GAP Service Handles.           */
static void GAPSPopulateHandles(GAPS_Client_Info_t *ClientInfo, qapi_BLE_GATT_Service_Discovery_Indication_Data_t *ServiceInfo)
{
   unsigned int                                Index1;
   qapi_BLE_GATT_Characteristic_Information_t *CurrentCharacteristic;

   /* Verify that the input parameters are semi-valid.                  */
   if((ClientInfo) && (ServiceInfo) && (ServiceInfo->ServiceInformation.UUID.UUID_Type == QAPI_BLE_GU_UUID_16_E) && (QAPI_BLE_GAP_COMPARE_GAP_SERVICE_UUID_TO_UUID_16(ServiceInfo->ServiceInformation.UUID.UUID.UUID_16)))
   {
      /* Loop through all characteristics discovered in the service     */
      /* and populate the correct entry.                                */
      CurrentCharacteristic = ServiceInfo->CharacteristicInformationList;
      if(CurrentCharacteristic)
      {
         for(Index1=0;Index1<ServiceInfo->NumberOfCharacteristics;Index1++,CurrentCharacteristic++)
         {
            /* All GAP Service UUIDs are defined to be 16 bit UUIDs.    */
            if(CurrentCharacteristic->Characteristic_UUID.UUID_Type == QAPI_BLE_GU_UUID_16_E)
            {
               /* Determine which characteristic this is.               */
               if(!QAPI_BLE_GAP_COMPARE_GAP_DEVICE_NAME_UUID_TO_UUID_16(CurrentCharacteristic->Characteristic_UUID.UUID.UUID_16))
               {
                  if(!QAPI_BLE_GAP_COMPARE_GAP_DEVICE_APPEARANCE_UUID_TO_UUID_16(CurrentCharacteristic->Characteristic_UUID.UUID.UUID_16))
                     continue;
                  else
                  {
                     ClientInfo->DeviceAppearanceHandle = CurrentCharacteristic->Characteristic_Handle;
                     continue;
                  }
               }
               else
               {
                  ClientInfo->DeviceNameHandle = CurrentCharacteristic->Characteristic_Handle;
                  continue;
               }
            }
         }
      }
   }
}

   /* The following function is used to map a Appearance Value to it's  */
   /* string representation.  This function returns TRUE on success or  */
   /* FALSE otherwise.                                                  */
static boolean_t AppearanceToString(uint16_t Appearance, char **String)
{
   boolean_t    ret_val;
   unsigned int Index;

   /* Verify that the input parameters are semi-valid.                  */
   if(String)
   {
      for(Index=0,ret_val=FALSE;Index<NUMBER_OF_APPEARANCE_MAPPINGS;++Index)
      {
         if(AppearanceMappings[Index].Appearance == Appearance)
         {
            *String = AppearanceMappings[Index].String;
            ret_val = TRUE;
            break;
         }
      }
   }
   else
      ret_val = FALSE;

   return(ret_val);
}

   /* ***************************************************************** */
   /*                         Event Callbacks                           */
   /* ***************************************************************** */

   /* BSC Timer Callback function prototype.                            */
static void BSC_Timer_Callback(uint32_t BluetoothStackID, uint32_t TimerID, uint32_t CallbackParameter)
{
   /* Verify the input parameters.                                      */
   if(BluetoothStackID)
   {
      QCLI_LOGE(mesh_coex_group, "Stopping scan after scanning for %u seconds.\n", CallbackParameter);

      /* Clear the Scan Timer ID.                                       */
      ScanTimerID = 0;

      /* Stop scanning.                                                 */
      StopScan(BluetoothStackID);
   }
}

   /* Advert Timer Callback function prototype.                          */
static void Advert_Timer_Callback(uint32_t BluetoothStackID, uint32_t TimerID, uint32_t CallbackParameter)
{
    int        Result;
    /* Verify the input parameters.                                      */
    if(BluetoothStackID && AdvertTimerID)
    {
        QCLI_LOGE(mesh_coex_group, "Stopping Advert after advertising for %u seconds.\n", CallbackParameter);

        /* Clear the Advert Timer ID.                                    */
        AdvertTimerID = 0;

        /* Disable Advertising.                                          */
        if (CoExBluetoothInit)
        {
            Result = qapi_BLE_GAP_LE_Advertising_Disable(BluetoothStackID);
            if(!Result)
            {
               QCLI_LOGE (mesh_coex_group, "GAP_LE_Advertising_Disable success.\n");
            }
            else
            {
               QCLI_LOGE (mesh_coex_group, "GAP_LE_Advertising_Disable returned %d.\n", Result);
            }
        }
        else
        {
            QmeshSchedUserAdvertDisable();
        }
    }
}

   /* The following function is for the GAP LE Event Receive Data       */
   /* Callback.  This function will be called whenever a Callback has   */
   /* been registered for the specified GAP LE Action that is associated*/
   /* with the Bluetooth Stack.  This function passes to the caller the */
   /* GAP LE Event Data of the specified Event and the GAP LE Event     */
   /* Callback Parameter that was specified when this Callback was      */
   /* installed.  The caller is free to use the contents of the GAP LE  */
   /* Event Data ONLY in the context of this callback.  If the caller   */
   /* requires the Data for a longer period of time, then the callback  */
   /* function MUST copy the data into another Data Buffer.  This       */
   /* function is guaranteed NOT to be invoked more than once           */
   /* simultaneously for the specified installed callback (i.e.  this   */
   /* function DOES NOT have be reentrant).  It Needs to be noted       */
   /* however, that if the same Callback is installed more than once,   */
   /* then the callbacks will be called serially.  Because of this, the */
   /* processing in this function should be as efficient as possible.   */
   /* It should also be noted that this function is called in the Thread*/
   /* Context of a Thread that the User does NOT own.  Therefore,       */
   /* processing in this function should be as efficient as possible    */
   /* (this argument holds anyway because other GAP Events will not be  */
   /* processed while this function call is outstanding).               */
   /* * NOTE * This function MUST NOT Block and wait for Events that can*/
   /*          only be satisfied by Receiving a Bluetooth Event         */
   /*          Callback.  A Deadlock WILL occur because NO Bluetooth    */
   /*          Callbacks will be issued while this function is currently*/
   /*          outstanding.                                             */
static void QAPI_BLE_BTPSAPI BLE_COEX_GAP_LE_Event_Callback(uint32_t BluetoothStackID, qapi_BLE_GAP_LE_Event_Data_t *GAP_LE_Event_Data, uint32_t CallbackParameter)
{
   boolean_t                                              DisplayPrompt;
   BoardStr_t                                             BoardStr;
   unsigned int                                           Index;
   qapi_BLE_GAP_LE_Connection_Parameters_t                ConnectionParams;
   qapi_BLE_GAP_LE_Direct_Advertising_Report_Data_t      *DirectDeviceEntryPtr;
#ifdef V2
   qapi_BLE_GAP_LE_Extended_Advertising_Report_Data_t    *ExtDeviceEntryPtr;
#endif

   /* Verify that all parameters to this callback are Semi-Valid.       */
   if((BluetoothStackID) && (GAP_LE_Event_Data))
   {
      DisplayPrompt = true;

      switch(GAP_LE_Event_Data->Event_Data_Type)
      {
#ifdef V2
         case QAPI_BLE_ET_LE_SCAN_TIMEOUT_E:
            QCLI_LOGE(mesh_coex_group, "etLE_Scan_Timeout with size %d.\n",(int)GAP_LE_Event_Data->Event_Data_Size);
            break;
         case QAPI_BLE_ET_LE_PHY_UPDATE_COMPLETE_E:
            QCLI_LOGE(mesh_coex_group, "etLE_PHY_Update_Complete with size %d.\n",(int)GAP_LE_Event_Data->Event_Data_Size);

            QCLI_LOGE(mesh_coex_group, "  Status:  %d.\n", (int)(GAP_LE_Event_Data->Event_Data.GAP_LE_Phy_Update_Complete_Event_Data->Status));
            QCLI_LOGE(mesh_coex_group, "  Address: 0x%02X%02X%02X%02X%02X%02X.\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Phy_Update_Complete_Event_Data->BD_ADDR.BD_ADDR5,
                                                                               GAP_LE_Event_Data->Event_Data.GAP_LE_Phy_Update_Complete_Event_Data->BD_ADDR.BD_ADDR4,
                                                                               GAP_LE_Event_Data->Event_Data.GAP_LE_Phy_Update_Complete_Event_Data->BD_ADDR.BD_ADDR3,
                                                                               GAP_LE_Event_Data->Event_Data.GAP_LE_Phy_Update_Complete_Event_Data->BD_ADDR.BD_ADDR2,
                                                                               GAP_LE_Event_Data->Event_Data.GAP_LE_Phy_Update_Complete_Event_Data->BD_ADDR.BD_ADDR1,
                                                                               GAP_LE_Event_Data->Event_Data.GAP_LE_Phy_Update_Complete_Event_Data->BD_ADDR.BD_ADDR0);

            break;
         case QAPI_BLE_ET_LE_ADVERTISING_SET_TERMINATED_E:
            QCLI_LOGE(mesh_coex_group, "etLE_Advertising_Set_Terminated with size %d.\n",(int)GAP_LE_Event_Data->Event_Data_Size);

            QCLI_LOGE(mesh_coex_group, "  Status:                                  %d.\n", (int)(GAP_LE_Event_Data->Event_Data.GAP_LE_Advertising_Set_Terminated_Event_Data->Status));
            QCLI_LOGE(mesh_coex_group, "  Advertising Handle:                      %u.\n", (unsigned int)(GAP_LE_Event_Data->Event_Data.GAP_LE_Advertising_Set_Terminated_Event_Data->Advertising_Handle));
            QCLI_LOGE(mesh_coex_group, "  Number of Completed Advertising Events:  %u.\n", (unsigned int)(GAP_LE_Event_Data->Event_Data.GAP_LE_Advertising_Set_Terminated_Event_Data->Num_Completed_Ext_Advertising_Events));

            if(GAP_LE_Event_Data->Event_Data.GAP_LE_Advertising_Set_Terminated_Event_Data->Status == QAPI_BLE_HCI_ERROR_CODE_SUCCESS)
            {
               /* Display the Address Type.                             */
               switch(GAP_LE_Event_Data->Event_Data.GAP_LE_Advertising_Set_Terminated_Event_Data->Connection_Address_Type)
               {
                  case QAPI_BLE_LAT_PUBLIC_E:
                     QCLI_LOGE(mesh_coex_group, "  Connection Address Type:                 %s.\n", "QAPI_BLE_LAT_PUBLIC_E");
                     break;
                  case QAPI_BLE_LAT_RANDOM_E:
                     QCLI_LOGE(mesh_coex_group, "  Connection Address Type:                 %s.\n", "QAPI_BLE_LAT_RANDOM_E");
                     break;
                  case QAPI_BLE_LAT_PUBLIC_IDENTITY_E:
                     QCLI_LOGE(mesh_coex_group, "  Connection Address Type:                 %s.\n", "QAPI_BLE_LAT_PUBLIC_IDENTITY_E");
                     break;
                  case QAPI_BLE_LAT_RANDOM_IDENTITY_E:
                     QCLI_LOGE(mesh_coex_group, "  Connection Address Type:                 %s.\n", "QAPI_BLE_LAT_RANDOM_IDENTITY_E");
                     break;
                  default:
                     QCLI_LOGE(mesh_coex_group, "  Connection Address Type:                 Invalid.\n");
                     break;
               }

               BD_ADDRToStr(GAP_LE_Event_Data->Event_Data.GAP_LE_Advertising_Set_Terminated_Event_Data->Connection_Address, BoardStr);
               QCLI_LOGE(mesh_coex_group, "  Connection Address:                      %s.\n", BoardStr);
            }
            break;
         case QAPI_BLE_ET_LE_SCAN_REQUEST_RECEIVED_E:
            QCLI_LOGE(mesh_coex_group, "etLE_Scan_Request_Received with size %d.\n",(int)GAP_LE_Event_Data->Event_Data_Size);

            QCLI_LOGE(mesh_coex_group, "  Advertising Handle:          %d.\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Scan_Request_Received_Event_Data->Advertising_Handle);

            /* Display the Address Type.                             */
            switch(GAP_LE_Event_Data->Event_Data.GAP_LE_Scan_Request_Received_Event_Data->Scanner_Address_Type)
            {
               case QAPI_BLE_LAT_PUBLIC_E:
                  QCLI_LOGE(mesh_coex_group, "  Scanner Address Type:        %s.\n", "QAPI_BLE_LAT_PUBLIC_E");
                  break;
               case QAPI_BLE_LAT_RANDOM_E:
                  QCLI_LOGE(mesh_coex_group, "  Scanner Address Type:        %s.\n", "QAPI_BLE_LAT_RANDOM_E");
                  break;
               case QAPI_BLE_LAT_PUBLIC_IDENTITY_E:
                  QCLI_LOGE(mesh_coex_group, "  Scanner Address Type:        %s.\n", "QAPI_BLE_LAT_PUBLIC_IDENTITY_E");
                  break;
               case QAPI_BLE_LAT_RANDOM_IDENTITY_E:
                  QCLI_LOGE(mesh_coex_group, "  Scanner Address Type:        %s.\n", "QAPI_BLE_LAT_RANDOM_IDENTITY_E");
                  break;
               default:
                  QCLI_LOGE(mesh_coex_group, "  Scanner Address Type:        Invalid.\n");
                  break;
            }

            BD_ADDRToStr(GAP_LE_Event_Data->Event_Data.GAP_LE_Scan_Request_Received_Event_Data->Scanner_Address, BoardStr);
            QCLI_LOGE(mesh_coex_group, "  Scanner Address:             %s.\n", BoardStr);
            break;
         case QAPI_BLE_ET_LE_CHANNEL_SELECTION_ALGORITHM_UPDATE_E:
            QCLI_LOGE(mesh_coex_group, "etLE_Channel_Selection_Algorithm_Update with size %d.\n", (int)GAP_LE_Event_Data->Event_Data_Size);

            /* Display the new CSA.                                     */
            switch(GAP_LE_Event_Data->Event_Data.GAP_LE_Channel_Selection_Algorithm_Update_Event_Data->Channel_Selection_Algorithm)
            {
               case QAPI_BLE_SA_ALGORITHM_NUM1_E:
                  QCLI_LOGE(mesh_coex_group, "  Channel Selection Algorithm:        %s.\n", "CSA #1");
                  break;
               case QAPI_BLE_SA_ALGORITHM_NUM2_E:
                  QCLI_LOGE(mesh_coex_group, "  Channel Selection Algorithm:        %s.\n", "CSA #2");
                  break;
               default:
                  QCLI_LOGE(mesh_coex_group, "  Channel Selection Algorithm:        %s.\n", "CSA Unkown");
                  break;
            }

            /* Display the Address Type.                             */
            switch(GAP_LE_Event_Data->Event_Data.GAP_LE_Channel_Selection_Algorithm_Update_Event_Data->Connection_Address_Type)
            {
               case QAPI_BLE_LAT_PUBLIC_E:
                  QCLI_LOGE(mesh_coex_group, "  Connection Address Type:            %s.\n", "QAPI_BLE_LAT_PUBLIC_E");
                  break;
               case QAPI_BLE_LAT_RANDOM_E:
                  QCLI_LOGE(mesh_coex_group, "  Connection Address Type:            %s.\n", "QAPI_BLE_LAT_RANDOM_E");
                  break;
               case QAPI_BLE_LAT_PUBLIC_IDENTITY_E:
                  QCLI_LOGE(mesh_coex_group, "  Connection Address Type:            %s.\n", "QAPI_BLE_LAT_PUBLIC_IDENTITY_E");
                  break;
               case QAPI_BLE_LAT_RANDOM_IDENTITY_E:
                  QCLI_LOGE(mesh_coex_group, "  Connection Address Type:            %s.\n", "QAPI_BLE_LAT_RANDOM_IDENTITY_E");
                  break;
               default:
                  QCLI_LOGE(mesh_coex_group, "  Connection Address Type:            Invalid.\n");
                  break;
            }

            BD_ADDRToStr(GAP_LE_Event_Data->Event_Data.GAP_LE_Channel_Selection_Algorithm_Update_Event_Data->Connection_Address, BoardStr);
            QCLI_LOGE(mesh_coex_group, "  Connection Address:                 %s.\n", BoardStr);
            break;
         case QAPI_BLE_ET_LE_EXTENDED_ADVERTISING_REPORT_E:
            QCLI_LOGE(mesh_coex_group, "etLE_Extended_Advertising_Report with size %d.\n",(int)GAP_LE_Event_Data->Event_Data_Size);
            QCLI_LOGE(mesh_coex_group, "  %d Responses.\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Extended_Advertising_Report_Event_Data->Number_Device_Entries);

            for(Index = 0; Index < GAP_LE_Event_Data->Event_Data.GAP_LE_Extended_Advertising_Report_Event_Data->Number_Device_Entries; Index++)
            {
               ExtDeviceEntryPtr = &(GAP_LE_Event_Data->Event_Data.GAP_LE_Extended_Advertising_Report_Event_Data->Extended_Advertising_Data[Index]);

               /* Display the Address Type.                             */
               switch(ExtDeviceEntryPtr->Address_Type)
               {
                  case QAPI_BLE_LAT_PUBLIC_E:
                     QCLI_LOGE(mesh_coex_group, "  Address Type:     %s.\n", "QAPI_BLE_LAT_PUBLIC_E");
                     break;
                  case QAPI_BLE_LAT_RANDOM_E:
                     QCLI_LOGE(mesh_coex_group, "  Address Type:     %s.\n", "QAPI_BLE_LAT_RANDOM_E");
                     break;
                  case QAPI_BLE_LAT_PUBLIC_IDENTITY_E:
                     QCLI_LOGE(mesh_coex_group, "  Address Type:     %s.\n", "QAPI_BLE_LAT_PUBLIC_IDENTITY_E");
                     break;
                  case QAPI_BLE_LAT_RANDOM_IDENTITY_E:
                     QCLI_LOGE(mesh_coex_group, "  Address Type:     %s.\n", "QAPI_BLE_LAT_RANDOM_IDENTITY_E");
                     break;
                  default:
                     QCLI_LOGE(mesh_coex_group, "  Address Type:     Invalid.\n");
                     break;
               }

               QCLI_LOGE(mesh_coex_group, "  Address:          0x%02X%02X%02X%02X%02X%02X.\n", ExtDeviceEntryPtr->BD_ADDR.BD_ADDR5, ExtDeviceEntryPtr->BD_ADDR.BD_ADDR4, ExtDeviceEntryPtr->BD_ADDR.BD_ADDR3, ExtDeviceEntryPtr->BD_ADDR.BD_ADDR2, ExtDeviceEntryPtr->BD_ADDR.BD_ADDR1, ExtDeviceEntryPtr->BD_ADDR.BD_ADDR0);

               if(DisplayAdvertisingEventData)
               {
                  QCLI_LOGE(mesh_coex_group, "  Event Type Flags: 0x%08X.\n", ExtDeviceEntryPtr->Event_Type_Flags);
                  QCLI_LOGE(mesh_coex_group, "  Tx Power:         %d.\n", (int)ExtDeviceEntryPtr->Tx_Power);
                  QCLI_LOGE(mesh_coex_group, "  RSSI:             %d.\n", (int)ExtDeviceEntryPtr->RSSI);
                  QCLI_LOGE(mesh_coex_group, "  Advertising SID:  %d.\n", (int)ExtDeviceEntryPtr->Advertising_SID);

                  switch(ExtDeviceEntryPtr->Data_Status)
                  {
                     case QAPI_BLE_DS_COMPLETE_E:
                        QCLI_LOGE(mesh_coex_group, "  Data Status:      %s.\n", "Complete");
                        break;
                     case QAPI_BLE_DS_INCOMPLETE_DATA_PENDING_E:
                        QCLI_LOGE(mesh_coex_group, "  Data Status:      %s.\n", "Incomplete - More data pending");
                        break;
                     default:
                     case QAPI_BLE_DS_INCOMPLETE_DATA_TRUNCATED_E:
                        QCLI_LOGE(mesh_coex_group, "  Data Status:      %s.\n", "Incomplete - data truncated");
                        break;
                  }

                  QCLI_LOGE(mesh_coex_group, "  Data Length:      %u.\n", (unsigned int)ExtDeviceEntryPtr->Raw_Report_Length);
               }
            }
            break;
#endif

         case QAPI_BLE_ET_LE_DATA_LENGTH_CHANGE_E:
            QCLI_LOGE(mesh_coex_group, "etLE_Data_Length_Change with size %d.\n", (int)GAP_LE_Event_Data->Event_Data_Size);

            BD_ADDRToStr(GAP_LE_Event_Data->Event_Data.GAP_LE_Data_Length_Change_Event_Data->BD_ADDR, BoardStr);
            QCLI_LOGE(mesh_coex_group, "  Connection Address:                 %s.\n", BoardStr);
            QCLI_LOGE(mesh_coex_group, "  Max Tx Octets:                      %u.\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Data_Length_Change_Event_Data->MaxTxOctets);
            QCLI_LOGE(mesh_coex_group, "  Max Tx Time:                        %u.\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Data_Length_Change_Event_Data->MaxTxTime);
            QCLI_LOGE(mesh_coex_group, "  Max Rx Octets:                      %u.\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Data_Length_Change_Event_Data->MaxRxOctets);
            QCLI_LOGE(mesh_coex_group, "  Max Rx Time:                        %u.\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Data_Length_Change_Event_Data->MaxRxTime);
            break;
            
         case QAPI_BLE_ET_LE_ADVERTISING_REPORT_E:
            
            QCLI_LOGE(mesh_coex_group, "etLE_Advertising_Report with size %d, number of entries %d \n",
                (int)GAP_LE_Event_Data->Event_Data_Size, 
                GAP_LE_Event_Data->Event_Data.GAP_LE_Advertising_Report_Event_Data->Number_Device_Entries);
            
            break;
            
         case QAPI_BLE_ET_LE_CONNECTION_COMPLETE_E:
            QCLI_LOGE(mesh_coex_group, "etLE_Connection_Complete with size %d.\n",(int)GAP_LE_Event_Data->Event_Data_Size);

            if(GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Complete_Event_Data)
            {
               BD_ADDRToStr(GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Complete_Event_Data->Peer_Address, BoardStr);

               QCLI_LOGE(mesh_coex_group, "   Status:              0x%02X.\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Complete_Event_Data->Status);
               QCLI_LOGE(mesh_coex_group, "   Role:                %s.\n", (GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Complete_Event_Data->Master)?"Master":"Slave");
               switch(GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Complete_Event_Data->Peer_Address_Type)
               {
                  case QAPI_BLE_LAT_PUBLIC_E:
                     QCLI_LOGE(mesh_coex_group, "   Address Type:        %s.\n", "QAPI_BLE_LAT_PUBLIC_E");
                     break;
                  case QAPI_BLE_LAT_RANDOM_E:
                     QCLI_LOGE(mesh_coex_group, "   Address Type:        %s.\n", "QAPI_BLE_LAT_RANDOM_E");
                     break;
                  case QAPI_BLE_LAT_PUBLIC_IDENTITY_E:
                     QCLI_LOGE(mesh_coex_group, "   Address Type:        %s.\n", "QAPI_BLE_LAT_PUBLIC_IDENTITY_E");
                     break;
                  case QAPI_BLE_LAT_RANDOM_IDENTITY_E:
                     QCLI_LOGE(mesh_coex_group, "   Address Type:        %s.\n", "QAPI_BLE_LAT_RANDOM_IDENTITY_E");
                     break;
                  default:
                     QCLI_LOGE(mesh_coex_group, "   Address Type:        Invalid.\n");
                     break;
               }
               QCLI_LOGE(mesh_coex_group, "   BD_ADDR:             %s.\n", BoardStr);

               if(GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Complete_Event_Data->Status == QAPI_BLE_HCI_ERROR_CODE_NO_ERROR)
               {
                  /* Update Device Status */
                  DeviceStatus++;
                  QCLI_LOGE(mesh_coex_group, "   Connection Interval: %u.\n", (unsigned int)GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Complete_Event_Data->Current_Connection_Parameters.Connection_Interval);
                  QCLI_LOGE(mesh_coex_group, "   Slave Latency:       %u.\n", (unsigned int)GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Complete_Event_Data->Current_Connection_Parameters.Slave_Latency);

                  /* Store the GAP LE Connection information that needs */
                  /* to be stored for the remote device.                */
                  /* * NOTE * These are temporary globals that will hold*/
                  /*          information that needs to be stored for   */
                  /*          the remote device that just connected.    */
                  /* * NOTE * For consistency, this information will NOT*/
                  /*          be stored until the GATT Connection       */
                  /*          Complete event has been received.  This   */
                  /*          event ALWAYS follows the GAP LE Connection*/
                  /*          Complete event.                           */
                  LocalDeviceIsMaster =  GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Complete_Event_Data->Master;
                  RemoteAddressType   = GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Complete_Event_Data->Peer_Address_Type;
               }
            }
            break;
         case QAPI_BLE_ET_LE_DISCONNECTION_COMPLETE_E:
            QCLI_LOGE(mesh_coex_group, "etLE_Disconnection_Complete with size %d.\n", (int)GAP_LE_Event_Data->Event_Data_Size);

            if(GAP_LE_Event_Data->Event_Data.GAP_LE_Disconnection_Complete_Event_Data)
            {
               QCLI_LOGE(mesh_coex_group, "   Status: 0x%02X.\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Disconnection_Complete_Event_Data->Status);
               QCLI_LOGE(mesh_coex_group, "   Reason: 0x%02X.\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Disconnection_Complete_Event_Data->Reason);

               BD_ADDRToStr(GAP_LE_Event_Data->Event_Data.GAP_LE_Disconnection_Complete_Event_Data->Peer_Address, BoardStr);
               QCLI_LOGE(mesh_coex_group, "   BD_ADDR: %s.\n", BoardStr);

            }
            break;
         case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_REQUEST_E:
            QCLI_LOGE(mesh_coex_group, "etLE_Connection_Parameter_Update_Request with size %d.\n",(int)GAP_LE_Event_Data->Event_Data_Size);

            if(GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data)
            {
               BD_ADDRToStr(GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data->BD_ADDR, BoardStr);
               QCLI_LOGE(mesh_coex_group, "   BD_ADDR:                     %s\r\n", BoardStr);
               QCLI_LOGE(mesh_coex_group, "   Connection Interval Minimum: %d\r\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data->Conn_Interval_Min);
               QCLI_LOGE(mesh_coex_group, "   Connection Interval Maximum: %d\r\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data->Conn_Interval_Max);
               QCLI_LOGE(mesh_coex_group, "   Slave Latency:               %d\r\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data->Slave_Latency);
               QCLI_LOGE(mesh_coex_group, "   Supervision Timeout:         %d\r\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data->Conn_Supervision_Timeout);

               ConnectionParams.Connection_Interval_Min    = QMCTConnInterval;
               ConnectionParams.Connection_Interval_Max    = QMCTConnInterval;
               ConnectionParams.Slave_Latency              = GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data->Slave_Latency;
               ConnectionParams.Supervision_Timeout        = 20000;
               ConnectionParams.Minimum_Connection_Length  = 0;
               ConnectionParams.Maximum_Connection_Length  = 10000;

               qapi_BLE_GAP_LE_Connection_Parameter_Update_Response(BluetoothStackID, GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data->BD_ADDR, TRUE, &ConnectionParams);
            }
            break;
         case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATED_E:
            QCLI_LOGE(mesh_coex_group, "etLE_Connection_Parameter_Updated with size %d.\n",(int)GAP_LE_Event_Data->Event_Data_Size);

            if(GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Updated_Event_Data)
            {
               BD_ADDRToStr(GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Updated_Event_Data->BD_ADDR, BoardStr);
               QCLI_LOGE(mesh_coex_group, "   BD_ADDR:             %s\r\n", BoardStr);
               QCLI_LOGE(mesh_coex_group, "   Status:              %d\r\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Updated_Event_Data->Status);
               QCLI_LOGE(mesh_coex_group, "   Connection Interval: %d\r\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Updated_Event_Data->Current_Connection_Parameters.Connection_Interval);
               QCLI_LOGE(mesh_coex_group, "   Slave Latency:       %d\r\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Updated_Event_Data->Current_Connection_Parameters.Slave_Latency);
               QCLI_LOGE(mesh_coex_group, "   Supervision Timeout: %d\r\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Updated_Event_Data->Current_Connection_Parameters.Supervision_Timeout);
            }
            break;
         case QAPI_BLE_ET_LE_ENCRYPTION_CHANGE_E:
            QCLI_LOGE(mesh_coex_group, "etLE_Encryption_Change with size %d.\n",(int)GAP_LE_Event_Data->Event_Data_Size);
            break;
         case QAPI_BLE_ET_LE_ENCRYPTION_REFRESH_COMPLETE_E:
            QCLI_LOGE(mesh_coex_group, "etLE_Encryption_Refresh_Complete with size %d.\n", (int)GAP_LE_Event_Data->Event_Data_Size);
            break;
         case QAPI_BLE_ET_LE_DIRECT_ADVERTISING_REPORT_E:
            QCLI_LOGE(mesh_coex_group, "etLE_Direct_Advertising_Report with size %d.\n", (int)GAP_LE_Event_Data->Event_Data_Size);
            QCLI_LOGE(mesh_coex_group, "  %d Responses.\n", GAP_LE_Event_Data->Event_Data.GAP_LE_Direct_Advertising_Report_Event_Data->Number_Device_Entries);

            for(Index = 0; Index < GAP_LE_Event_Data->Event_Data.GAP_LE_Direct_Advertising_Report_Event_Data->Number_Device_Entries; Index++)
            {
               DirectDeviceEntryPtr = &(GAP_LE_Event_Data->Event_Data.GAP_LE_Direct_Advertising_Report_Event_Data->Direct_Advertising_Data[Index]);

               /* Display the Address Type.                             */
               switch(DirectDeviceEntryPtr->Address_Type)
               {
                  case QAPI_BLE_LAT_PUBLIC_E:
                     QCLI_LOGE(mesh_coex_group, "  Address Type: %s.\n", "QAPI_BLE_LAT_PUBLIC_E");
                     break;
                  case QAPI_BLE_LAT_RANDOM_E:
                     QCLI_LOGE(mesh_coex_group, "  Address Type: %s.\n", "QAPI_BLE_LAT_RANDOM_E");
                     break;
                  case QAPI_BLE_LAT_PUBLIC_IDENTITY_E:
                     QCLI_LOGE(mesh_coex_group, "  Address Type: %s.\n", "QAPI_BLE_LAT_PUBLIC_IDENTITY_E");
                     break;
                  case QAPI_BLE_LAT_RANDOM_IDENTITY_E:
                     QCLI_LOGE(mesh_coex_group, "  Address Type: %s.\n", "QAPI_BLE_LAT_RANDOM_IDENTITY_E");
                     break;
                  default:
                     QCLI_LOGE(mesh_coex_group, "  Address Type: Invalid.\n");
                     break;
               }

               /* Display the Device Address.                           */
               QCLI_LOGE(mesh_coex_group, "  Address:      0x%02X%02X%02X%02X%02X%02X.\n", DirectDeviceEntryPtr->BD_ADDR.BD_ADDR5, DirectDeviceEntryPtr->BD_ADDR.BD_ADDR4, DirectDeviceEntryPtr->BD_ADDR.BD_ADDR3, DirectDeviceEntryPtr->BD_ADDR.BD_ADDR2, DirectDeviceEntryPtr->BD_ADDR.BD_ADDR1, DirectDeviceEntryPtr->BD_ADDR.BD_ADDR0);
               QCLI_LOGE(mesh_coex_group, "  RSSI:         %d.\n", (int)(DirectDeviceEntryPtr->RSSI));
            }
            break;
         default:
            DisplayPrompt = false;
            break;
      }

      if(DisplayPrompt)
         QCLI_Display_Prompt();
   }
}


   /* Generic Attribute Profile (GATT) Event Callback function          */
   /* prototypes.                                                       */

   /* The following function is for an GATT Connection Event Callback.  */
   /* This function is called for GATT Connection Events that occur on  */
   /* the specified Bluetooth Stack.  This function passes to the caller*/
   /* the GATT Connection Event Data that occurred and the GATT         */
   /* Connection Event Callback Parameter that was specified when this  */
   /* Callback was installed.  The caller is free to use the contents of*/
   /* the GATT Client Event Data ONLY in the context of this callback.  */
   /* If the caller requires the Data for a longer period of time, then */
   /* the callback function MUST copy the data into another Data Buffer.*/
   /* This function is guaranteed NOT to be invoked more than once      */
   /* simultaneously for the specified installed callback (i.e.  this   */
   /* function DOES NOT have be reentrant).  It Needs to be noted       */
   /* however, that if the same Callback is installed more than once,   */
   /* then the callbacks will be called serially.  Because of this, the */
   /* processing in this function should be as efficient as possible.   */
   /* It should also be noted that this function is called in the Thread*/
   /* Context of a Thread that the User does NOT own.  Therefore,       */
   /* processing in this function should be as efficient as possible    */
   /* (this argument holds anyway because another GATT Event            */
   /* (Server/Client or Connection) will not be processed while this    */
   /* function call is outstanding).                                    */
   /* * NOTE * This function MUST NOT Block and wait for Events that can*/
   /*          only be satisfied by Receiving a Bluetooth Event         */
   /*          Callback.  A Deadlock WILL occur because NO Bluetooth    */
   /*          Callbacks will be issued while this function is currently*/
   /*          outstanding.                                             */
extern void QAPI_BLE_BTPSAPI BLE_COEX_GATT_Connection_Event_Callback(uint32_t BluetoothStackID, qapi_BLE_GATT_Connection_Event_Data_t *GATT_Connection_Event_Data, uint32_t CallbackParameter)
{
   boolean_t                    DisplayPrompt;
   int                          Result;
   uint16_t                     MTU;
   BoardStr_t                   BoardStr;
   DeviceInfo_t                *DeviceInfo = NULL;
   uint32_t                     ConnectionID;

   /* Verify that all parameters to this callback are Semi-Valid.       */
   if((BluetoothStackID) && (GATT_Connection_Event_Data))
   {
      DisplayPrompt = true;

      /* Determine the Connection Event that occurred.                  */
      switch(GATT_Connection_Event_Data->Event_Data_Type)
      {
         case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_E:
            if(GATT_Connection_Event_Data->Event_Data.GATT_Device_Connection_Data)
            {
               /* Save the Connection ID for later use.                 */
               ConnectionID = GATT_Connection_Event_Data->Event_Data.GATT_Device_Connection_Data->ConnectionID;

               QCLI_LOGE(mesh_coex_group, "etGATT_Connection_Device_Connection with size %u: \n", GATT_Connection_Event_Data->Event_Data_Size);
               BD_ADDRToStr(GATT_Connection_Event_Data->Event_Data.GATT_Device_Connection_Data->RemoteDevice, BoardStr);
               QCLI_LOGE(mesh_coex_group, "   Connection ID:   %u.\n", GATT_Connection_Event_Data->Event_Data.GATT_Device_Connection_Data->ConnectionID);
               QCLI_LOGE(mesh_coex_group, "   Connection Type: %s.\n", ((GATT_Connection_Event_Data->Event_Data.GATT_Device_Connection_Data->ConnectionType == QAPI_BLE_GCT_LE_E)?"LE":"BR/EDR"));
               QCLI_LOGE(mesh_coex_group, "   Remote Device:   %s.\n", BoardStr);
               QCLI_LOGE(mesh_coex_group, "   Connection MTU:  %u.\n", GATT_Connection_Event_Data->Event_Data.GATT_Device_Connection_Data->MTU);

               /* Let's try to find the remote device information.  This*/
               /* will be the case if we previously connected and bonded*/
               /* with the remote device.                               */
               if((DeviceInfo = SearchDeviceInfoEntryByBD_ADDR(&DeviceInfoList, GATT_Connection_Event_Data->Event_Data.GATT_Device_Connection_Data->RemoteDevice)) == NULL)
               {
                  /* This MUST be a new remote device so we will create */
                  /* a remote device information entry.                 */
                  if((DeviceInfo = CreateNewDeviceInfoEntry(&DeviceInfoList, GATT_Connection_Event_Data->Event_Data.GATT_Device_Connection_Data->RemoteDevice)) == NULL)
                     QCLI_LOGE(mesh_coex_group, "Failed to create remote device information.\n");
               }

               /* Make sure we found the remote device information entry*/
               /* or created it.                                        */
               if(DeviceInfo)
               {
                  /* Increment the number of connected remote devices.  */
                  ConnectionCount++;

                  /* The connected remote device will always become the */
                  /* selected remote device.                            */
                  SelectedRemoteBD_ADDR = GATT_Connection_Event_Data->Event_Data.GATT_Device_Connection_Data->RemoteDevice;

                  /* Inform the user of the selected remote device.     */
                  QCLI_LOGE(mesh_coex_group, "\nSelected Remote Device:\n");
                  QCLI_LOGE(mesh_coex_group, "   Address:       %s\n", BoardStr);
                  QCLI_LOGE(mesh_coex_group, "   ID:            %u\n", ConnectionID);

                  /* Store the information for the remote device.       */
                  /* * NOTE * The LocalDeviceIsMaster and               */
                  /*          RemoteAddressType are temporary globals   */
                  /*          that hold information from the GAP LE     */
                  /*          Connection Complete event that MUST be    */
                  /*          stored in the remote device infromation.  */
                  /* * NOTE * The ConnectionID is a unique value for    */
                  /*          every GATT Connection and will be used to */
                  /*          update the selected remote device.        */
                  DeviceInfo->RemoteDeviceIsMaster = (LocalDeviceIsMaster) ? FALSE : TRUE;
                  DeviceInfo->RemoteAddressType    = RemoteAddressType;
                  DeviceInfo->ConnectionID         = ConnectionID;

                  /* Attempt to update the MTU to the maximum supported.*/
                  if(!qapi_BLE_GATT_Query_Maximum_Supported_MTU(BluetoothStackID, &MTU))
                     qapi_BLE_GATT_Exchange_MTU_Request(BluetoothStackID, DeviceInfo->ConnectionID, MTU, GATT_ClientEventCallback_GAPS, 0);

               }
               else
               {
                  /* Simply disconnect the remote device since an error occured */
                  if((Result = qapi_BLE_GAP_LE_Disconnect(BluetoothStackID, GATT_Connection_Event_Data->Event_Data.GATT_Device_Connection_Data->RemoteDevice)) != 0)
                     QCLI_LOGE(mesh_coex_group, "qapi_BLE_GAP_LE_Disconnect returned %d.\n", Result);
               }
            }
            else
               QCLI_LOGE(mesh_coex_group, "Error - Null Connection Data.\n");
            break;
         case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_DISCONNECTION_E:
            if(GATT_Connection_Event_Data->Event_Data.GATT_Device_Disconnection_Data)
            {
               QCLI_LOGE(mesh_coex_group, "etGATT_Connection_Device_Disconnection with size %u: \n", GATT_Connection_Event_Data->Event_Data_Size);
               BD_ADDRToStr(GATT_Connection_Event_Data->Event_Data.GATT_Device_Disconnection_Data->RemoteDevice, BoardStr);
               QCLI_LOGE(mesh_coex_group, "   Connection ID:   %u.\n", GATT_Connection_Event_Data->Event_Data.GATT_Device_Disconnection_Data->ConnectionID);
               QCLI_LOGE(mesh_coex_group, "   Connection Type: %s.\n", ((GATT_Connection_Event_Data->Event_Data.GATT_Device_Disconnection_Data->ConnectionType == QAPI_BLE_GCT_LE_E)?"LE":"BR/EDR"));
               QCLI_LOGE(mesh_coex_group, "   Remote Device:   %s.\n", BoardStr);

               /* Get the remote device information.                    */
               /* * NOTE * If the stack is shutting down, then the      */
               /*          ConnectionCount will be reset to zero.       */
               /*          CloseStack() will send a disconnection       */
               /*          request to each connected remote device.  It */
               /*          will also free the remote device information */
               /*          entries, so we do not need to handle it here.*/
               if((ConnectionCount) && (DeviceInfo = SearchDeviceInfoEntryByBD_ADDR(&DeviceInfoList, GATT_Connection_Event_Data->Event_Data.GATT_Device_Disconnection_Data->RemoteDevice)) != NULL)
               {
                  /* Decrement the number of connected remote devices.  */
                  ConnectionCount--;

                  /* If we have paired with the remote device, then the */
                  /* LTK will be valid and the device information MUST  */
                  /* persist between connections.                       */
                  if(DeviceInfo->Flags & DEVICE_INFO_FLAGS_LTK_VALID)
                  {
                     /* Flag that no service discovery operation is     */
                     /* outstanding for this device.                    */
                     DeviceInfo->Flags &= ~DEVICE_INFO_FLAGS_SERVICE_DISCOVERY_OUTSTANDING;

                     /* Reset the GATT Connection ID to indicate that   */
                     /* the remote device is no longer connected.       */
                     DeviceInfo->ConnectionID = 0;
                  }
                  else
                  {
                     /* Remove the remote device information entry from */
                     /* the list since it is no longer needed.          */
                     if((DeviceInfo = DeleteDeviceInfoEntry(&DeviceInfoList, GATT_Connection_Event_Data->Event_Data.GATT_Device_Disconnection_Data->RemoteDevice)) != NULL)
                     {
                        /* Inform the user the remote device information*/
                        /* is being deleted.                            */
                        QCLI_LOGE(mesh_coex_group, "\nThe remote device information has been deleted.\n", BoardStr);

                        DeviceInfo = NULL;
                     }
                  }
               }

               /* If a remote device is still connected, then the first */
               /* connected remote device will become the selected      */
               /* remote device.                                        */
               if(ConnectionCount)
               {
                  /* Loop through the device information.               */
                  DeviceInfo = DeviceInfoList;
                  while(DeviceInfo)
                  {
                     /* Simply check if the GATT Connection ID is       */
                     /* non-zero.  If this is the case, then we are     */
                     /* connected to the remote device.                 */
                     if(DeviceInfo->ConnectionID)
                     {
                        /* Update the selected remote device.           */
                        SelectedRemoteBD_ADDR = DeviceInfo->RemoteAddress;

                        /* Inform the user of the selected remote device   */
                        QCLI_LOGE(mesh_coex_group, "\nSelected Remote Device:\n");
                        BD_ADDRToStr(DeviceInfo->RemoteAddress, BoardStr);
                        QCLI_LOGE(mesh_coex_group, "   Address:       %s\n", BoardStr);
                        QCLI_LOGE(mesh_coex_group, "   ID:            %u\n", DeviceInfo->ConnectionID);

                        break;
                     }

                     /* Get the next remote device's information.       */
                     DeviceInfo = DeviceInfo->NextDeviceInfoInfoPtr;
                  }
               }
            }
            else
               QCLI_LOGE(mesh_coex_group, "Error - Null Disconnection Data.\n");
            break;
         case QAPI_BLE_ET_GATT_CONNECTION_SERVER_NOTIFICATION_E:
            if(GATT_Connection_Event_Data->Event_Data.GATT_Server_Notification_Data)
            {
              /* Inform the user we received a notification for an  */
              /* unknown handle.                                    */
              QCLI_LOGE(mesh_coex_group, "Error - Unknown attribute handle for notification.\n");
              DisplayPrompt = true;
            }
            else
               QCLI_LOGE(mesh_coex_group, "Error - Null Server Notification Data.\n");
            break;

        case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_BUFFER_EMPTY_E:
            if(GATT_Connection_Event_Data->Event_Data.GATT_Device_Buffer_Empty_Data)
            {
                QCLI_LOGE(mesh_coex_group, "Received QAPI_BLE_ET_GATT_CONNECTION_DEVICE_BUFFER_EMPTY_E event.\n");
                QCLI_LOGE(mesh_coex_group, "Device Connection ID 0x%x\n",GATT_Connection_Event_Data->Event_Data.GATT_Device_Buffer_Empty_Data->ConnectionID);
                DevGattBufferFull = FALSE;
                DisplayPrompt = true;
            }
            else
               QCLI_LOGE(mesh_coex_group, "Error - Null GATT_Device_Buffer_Empty_Data.\n");
            break;

         default:
		    QCLI_LOGE(mesh_coex_group, "BLE_COEX_GATT_Connection_Event_Callback :: Event 0x%x not handled\n",GATT_Connection_Event_Data->Event_Data_Type);
            DisplayPrompt = false;
            break;
      }

      if(DisplayPrompt)
         QCLI_Display_Prompt();
   }
}

   /* The following function is for an GATT Discovery Event Callback.   */
   /* This function will be called whenever a GATT Service is discovered*/
   /* or a previously started service discovery process is completed.   */
   /* This function passes to the caller the GATT Discovery Event Data  */
   /* that occurred and the GATT Client Event Callback Parameter that   */
   /* was specified when this Callback was installed.  The caller is    */
   /* free to use the contents of the GATT Discovery Event Data ONLY in */
   /* the context of this callback.  If the caller requires the Data for*/
   /* a longer period of time, then the callback function MUST copy the */
   /* data into another Data Buffer.  This function is guaranteed NOT to*/
   /* be invoked more than once simultaneously for the specified        */
   /* installed callback (i.e.  this function DOES NOT have be          */
   /* reentrant).  It Needs to be noted however, that if the same       */
   /* Callback is installed more than once, then the callbacks will be  */
   /* called serially.  Because of this, the processing in this function*/
   /* should be as efficient as possible.  It should also be noted that */
   /* this function is called in the Thread Context of a Thread that the*/
   /* User does NOT own.  Therefore, processing in this function should */
   /* be as efficient as possible (this argument holds anyway because   */
   /* another GATT Discovery Event will not be processed while this     */
   /* function call is outstanding).                                    */
   /* * NOTE * This function MUST NOT Block and wait for Events that can*/
   /*          only be satisfied by Receiving a Bluetooth Event         */
   /*          Callback.  A Deadlock WILL occur because NO Bluetooth    */
   /*          Callbacks will be issued while this function is currently*/
   /*          outstanding.                                             */
static void QAPI_BLE_BTPSAPI BLE_COEX_GATT_Service_Discovery_Event_Callback(uint32_t BluetoothStackID, qapi_BLE_GATT_Service_Discovery_Event_Data_t *GATT_Service_Discovery_Event_Data, uint32_t CallbackParameter)
{
   boolean_t     DisplayPrompt;
   DeviceInfo_t *DeviceInfo;

   if((BluetoothStackID) && (GATT_Service_Discovery_Event_Data))
   {
      DisplayPrompt = true;

      if((DeviceInfo = SearchDeviceInfoEntryByBD_ADDR(&DeviceInfoList, SelectedRemoteBD_ADDR)) != NULL)
      {
         switch(GATT_Service_Discovery_Event_Data->Event_Data_Type)
         {
            case QAPI_BLE_ET_GATT_SERVICE_DISCOVERY_INDICATION_E:
               /* Verify the event data.                                */
               if(GATT_Service_Discovery_Event_Data->Event_Data.GATT_Service_Discovery_Indication_Data)
               {
                  QCLI_LOGE(mesh_coex_group, "Service 0x%04X - 0x%04X, UUID: ", GATT_Service_Discovery_Event_Data->Event_Data.GATT_Service_Discovery_Indication_Data->ServiceInformation.Service_Handle, GATT_Service_Discovery_Event_Data->Event_Data.GATT_Service_Discovery_Indication_Data->ServiceInformation.End_Group_Handle);
                  DisplayUUID(&(GATT_Service_Discovery_Event_Data->Event_Data.GATT_Service_Discovery_Indication_Data->ServiceInformation.UUID));
                  QCLI_LOGE(mesh_coex_group, "\n");

                  /* Attempt to populate the handles for the GAP Service     */
                  GAPSPopulateHandles(&(DeviceInfo->GAPSClientInfo), GATT_Service_Discovery_Event_Data->Event_Data.GATT_Service_Discovery_Indication_Data);

                  /* Attempt to populate the handles for the QMCT Service  */
                  QMCTPopulateHandles(&(DeviceInfo->QMCTClientInfo), GATT_Service_Discovery_Event_Data->Event_Data.GATT_Service_Discovery_Indication_Data);
               }
               break;
            case QAPI_BLE_ET_GATT_SERVICE_DISCOVERY_COMPLETE_E:
               /* Verify the event data.                                */
               if(GATT_Service_Discovery_Event_Data->Event_Data.GATT_Service_Discovery_Complete_Data)
               {
                  QCLI_LOGE(mesh_coex_group, "Service Discovery Operation Complete, Status 0x%02X.\n", GATT_Service_Discovery_Event_Data->Event_Data.GATT_Service_Discovery_Complete_Data->Status);

                  /* Flag that no service discovery operation is        */
                  /* outstanding for this device.                       */
                  DeviceInfo->Flags &= ~DEVICE_INFO_FLAGS_SERVICE_DISCOVERY_OUTSTANDING;
                  /* Update Device Status */
                  DeviceStatus++;
                  /* Update Device count on Service Discovery Completes */
                  QMCTConnectionCount++;
               }
               break;
            default:
               DisplayPrompt = false;
               break;
         }
      }

      if(DisplayPrompt)
         QCLI_Display_Prompt();
   }
}

   /* The following function is for an GATT Client Event Callback.  This*/
   /* function will be called whenever a GATT Response is received for a*/
   /* request that was made when this function was registered.  This    */
   /* function passes to the caller the GATT Client Event Data that     */
   /* occurred and the GATT Client Event Callback Parameter that was    */
   /* specified when this Callback was installed.  The caller is free to*/
   /* use the contents of the GATT Client Event Data ONLY in the context*/
   /* of this callback.  If the caller requires the Data for a longer   */
   /* period of time, then the callback function MUST copy the data into*/
   /* another Data Buffer.  This function is guaranteed NOT to be       */
   /* invoked more than once simultaneously for the specified installed */
   /* callback (i.e.  this function DOES NOT have be reentrant).  It    */
   /* Needs to be noted however, that if the same Callback is installed */
   /* more than once, then the callbacks will be called serially.       */
   /* Because of this, the processing in this function should be as     */
   /* efficient as possible.  It should also be noted that this function*/
   /* is called in the Thread Context of a Thread that the User does NOT*/
   /* own.  Therefore, processing in this function should be as         */
   /* efficient as possible (this argument holds anyway because another */
   /* GATT Event (Server/Client or Connection) will not be processed    */
   /* while this function call is outstanding).                         */
   /* * NOTE * This function MUST NOT Block and wait for Events that can*/
   /*          only be satisfied by Receiving a Bluetooth Event         */
   /*          Callback.  A Deadlock WILL occur because NO Bluetooth    */
   /*          Callbacks will be issued while this function is currently*/
   /*          outstanding.                                             */
static void QAPI_BLE_BTPSAPI GATT_ClientEventCallback_GAPS(uint32_t BluetoothStackID, qapi_BLE_GATT_Client_Event_Data_t *GATT_Client_Event_Data, uint32_t CallbackParameter)
{
   boolean_t     DisplayPrompt;
   char         *NameBuffer;
   uint16_t      Appearance;
   BoardStr_t    BoardStr;
   DeviceInfo_t *DeviceInfo;

   /* Verify that all parameters to this callback are Semi-Valid.       */
   if((BluetoothStackID) && (GATT_Client_Event_Data))
   {
      DisplayPrompt = true;

      /* Determine the event that occurred.                             */
      switch(GATT_Client_Event_Data->Event_Data_Type)
      {
         case QAPI_BLE_ET_GATT_CLIENT_ERROR_RESPONSE_E:
            if(GATT_Client_Event_Data->Event_Data.GATT_Request_Error_Data)
            {
               QCLI_LOGE(gaps_group, "Error Response.\n");
               BD_ADDRToStr(GATT_Client_Event_Data->Event_Data.GATT_Request_Error_Data->RemoteDevice, BoardStr);
               QCLI_LOGE(gaps_group, "Connection ID:   %u.\n", GATT_Client_Event_Data->Event_Data.GATT_Request_Error_Data->ConnectionID);
               QCLI_LOGE(gaps_group, "Transaction ID:  %u.\n", GATT_Client_Event_Data->Event_Data.GATT_Request_Error_Data->TransactionID);
               QCLI_LOGE(gaps_group, "Connection Type: %s.\n", (GATT_Client_Event_Data->Event_Data.GATT_Request_Error_Data->ConnectionType == QAPI_BLE_GCT_LE_E)?"LE":"BR/EDR");
               QCLI_LOGE(gaps_group, "BD_ADDR:         %s.\n", BoardStr);
               QCLI_LOGE(gaps_group, "Error Type:      %s.\n", (GATT_Client_Event_Data->Event_Data.GATT_Request_Error_Data->ErrorType == QAPI_BLE_RET_ERROR_RESPONSE_E)?"Response Error":"Response Timeout");

               /* Only print out the rest if it is valid.               */
               if(GATT_Client_Event_Data->Event_Data.GATT_Request_Error_Data->ErrorType == QAPI_BLE_RET_ERROR_RESPONSE_E)
               {
                  QCLI_LOGE(gaps_group, "Request Opcode:  0x%02X.\n", GATT_Client_Event_Data->Event_Data.GATT_Request_Error_Data->RequestOpCode);
                  QCLI_LOGE(gaps_group, "Request Handle:  0x%04X.\n", GATT_Client_Event_Data->Event_Data.GATT_Request_Error_Data->RequestHandle);
                  QCLI_LOGE(gaps_group, "Error Code:      0x%02X.\n", GATT_Client_Event_Data->Event_Data.GATT_Request_Error_Data->ErrorCode);
                  QCLI_LOGE(gaps_group, "Error Mesg:      %s.\n", ErrorCodeStr[GATT_Client_Event_Data->Event_Data.GATT_Request_Error_Data->ErrorCode]);
               }
            }
            else
               QCLI_LOGE(gaps_group, "Error - Null Error Response Data.\n");
            break;
         case QAPI_BLE_ET_GATT_CLIENT_EXCHANGE_MTU_RESPONSE_E:
            if(GATT_Client_Event_Data->Event_Data.GATT_Exchange_MTU_Response_Data)
            {
               QCLI_LOGE(gaps_group, "Exchange MTU Response.\n");
               BD_ADDRToStr(GATT_Client_Event_Data->Event_Data.GATT_Exchange_MTU_Response_Data->RemoteDevice, BoardStr);
               QCLI_LOGE(gaps_group, "Connection ID:   %u.\n", GATT_Client_Event_Data->Event_Data.GATT_Exchange_MTU_Response_Data->ConnectionID);
               QCLI_LOGE(gaps_group, "Transaction ID:  %u.\n", GATT_Client_Event_Data->Event_Data.GATT_Exchange_MTU_Response_Data->TransactionID);
               QCLI_LOGE(gaps_group, "Connection Type: %s.\n", (GATT_Client_Event_Data->Event_Data.GATT_Exchange_MTU_Response_Data->ConnectionType == QAPI_BLE_GCT_LE_E)?"LE":"BR/EDR");
               QCLI_LOGE(gaps_group, "BD_ADDR:         %s.\n", BoardStr);
               QCLI_LOGE(gaps_group, "MTU:             %u.\n", GATT_Client_Event_Data->Event_Data.GATT_Exchange_MTU_Response_Data->ServerMTU);
               /* Discover Services */
               DiscoverLEDeviceServices(GATT_Client_Event_Data->Event_Data.GATT_Exchange_MTU_Response_Data->RemoteDevice);
            }
            else
            {
               QCLI_LOGE(gaps_group, "Error - Null Write Response Data.\n");
            }
            break;
         case QAPI_BLE_ET_GATT_CLIENT_READ_RESPONSE_E:
            if(GATT_Client_Event_Data->Event_Data.GATT_Read_Response_Data)
            {
               DisplayPrompt = false;
               if((DeviceInfo = SearchDeviceInfoEntryByBD_ADDR(&DeviceInfoList, GATT_Client_Event_Data->Event_Data.GATT_Read_Response_Data->RemoteDevice)) != NULL)
               {
                  if((uint16_t)CallbackParameter == DeviceInfo->GAPSClientInfo.DeviceNameHandle)
                  {
                     /* Display the remote device name.                 */
                     if((NameBuffer = (char *)malloc(GATT_Client_Event_Data->Event_Data.GATT_Read_Response_Data->AttributeValueLength+1)) != NULL)
                     {
                        memset(NameBuffer, 0, GATT_Client_Event_Data->Event_Data.GATT_Read_Response_Data->AttributeValueLength+1);
                        memcpy(NameBuffer, GATT_Client_Event_Data->Event_Data.GATT_Read_Response_Data->AttributeValue, GATT_Client_Event_Data->Event_Data.GATT_Read_Response_Data->AttributeValueLength);

                        QCLI_LOGE(gaps_group, "Remote Device Name: %s.\n", NameBuffer);
                        DisplayPrompt = true;

                        free(NameBuffer);
                     }
                  }
                  else
                  {
                     if((uint16_t)CallbackParameter == DeviceInfo->GAPSClientInfo.DeviceAppearanceHandle)
                     {
                        if(GATT_Client_Event_Data->Event_Data.GATT_Read_Response_Data->AttributeValueLength == QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_LENGTH)
                        {
                           Appearance = READ_UNALIGNED_WORD_LITTLE_ENDIAN(GATT_Client_Event_Data->Event_Data.GATT_Read_Response_Data->AttributeValue);
                           if(AppearanceToString(Appearance, &NameBuffer))
                              QCLI_LOGE(gaps_group, "Remote Device Appearance: %s(%u).\n", NameBuffer, Appearance);
                           else
                              QCLI_LOGE(gaps_group, "Remote Device Appearance: Unknown(%u).\n", Appearance);
                        }
                        else
                           QCLI_LOGE(gaps_group, "Invalid Remote Appearance Value Length %u.\n", GATT_Client_Event_Data->Event_Data.GATT_Read_Response_Data->AttributeValueLength);

                        DisplayPrompt = true;
                     }
                  }
               }
            }
            else
            {
               QCLI_LOGE(gaps_group, "Error - Null Read Response Data.\n");
            }
            break;
         default:
		    QCLI_LOGE(mesh_coex_group, "GATT_ClientEventCallback_GAPS :: Event 0x%x not handled\n",GATT_Client_Event_Data->Event_Data_Type);
            break;
      }

      if(DisplayPrompt)
         QCLI_Display_Prompt();
   }
}


/* The following function is used to register the CoEx Command Group with QCLI */
void Initialize_Qmesh_CoEx(void)
{
    /* Attempt to reqister the CoEx Command Group with the QCLI framework.*/
    mesh_coex_group = QCLI_Register_Command_Group(NULL, &qmesh_coex_cmd_group);
    if(mesh_coex_group)
    {
        /* Initialize the BLE Parameters.                              */
        memset(&BLEParameters, 0, sizeof(BLEParameters));
    }
}

   /* The following function is responsible for converting the specified string    */
   /* into data of type BD_ADDR.  The first parameter of this function  */
   /* is the BD_ADDR string to be converted to a BD_ADDR.  The second   */
   /* parameter of this function is a pointer to the BD_ADDR in which   */
   /* the converted BD_ADDR String is to be stored.                     */
static void StrToBD_ADDR(char *BoardStr, qapi_BLE_BD_ADDR_t *Board_Address)
{
   char Buffer[5];

   if((BoardStr) && (strlen(BoardStr) == sizeof(qapi_BLE_BD_ADDR_t)*2) && (Board_Address))
   {
      Buffer[0] = '0';
      Buffer[1] = 'x';
      Buffer[4] = '\0';

      Buffer[2] = BoardStr[0];
      Buffer[3] = BoardStr[1];
      Board_Address->BD_ADDR5 = (uint8_t)StringToUnsignedInteger(Buffer);

      Buffer[2] = BoardStr[2];
      Buffer[3] = BoardStr[3];
      Board_Address->BD_ADDR4 = (uint8_t)StringToUnsignedInteger(Buffer);

      Buffer[2] = BoardStr[4];
      Buffer[3] = BoardStr[5];
      Board_Address->BD_ADDR3 = (uint8_t)StringToUnsignedInteger(Buffer);

      Buffer[2] = BoardStr[6];
      Buffer[3] = BoardStr[7];
      Board_Address->BD_ADDR2 = (uint8_t)StringToUnsignedInteger(Buffer);

      Buffer[2] = BoardStr[8];
      Buffer[3] = BoardStr[9];
      Board_Address->BD_ADDR1 = (uint8_t)StringToUnsignedInteger(Buffer);

      Buffer[2] = BoardStr[10];
      Buffer[3] = BoardStr[11];
      Board_Address->BD_ADDR0 = (uint8_t)StringToUnsignedInteger(Buffer);
   }
   else
   {
      if(Board_Address)
         memset(Board_Address, 0, sizeof(qapi_BLE_BD_ADDR_t));
   }
}

