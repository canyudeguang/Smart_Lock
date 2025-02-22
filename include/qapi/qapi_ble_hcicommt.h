/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/*****< qapi_ble_hcicommt.h >**************************************************/
/*      Copyright 2015 - 2016 Qualcomm Technologies, Inc.                     */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  HCICOMMT - Serial HCI Driver Layer Types.                                 */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   08/28/15  T. Cook        Initial creation.                               */
/******************************************************************************/
#ifndef __QAPI_BLE_HCICOMMT_H__
#define __QAPI_BLE_HCICOMMT_H__

#include "qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.            */
#include "qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.      */

   /* The following constants represent the Minimum, Maximum, and Values*/
   /* that are used with the Initialization Delay member of the         */
   /* qapi_BLE_HCI_COMMDriverInformation_t structure.  These Delays are */
   /* specified in Milliseconds and represent the delay that is to be   */
   /* added between Port Initialization (Open) and the writing of any   */
   /* data to the Port.  This functionality was added because it was    */
   /* found that some PCMCIA/Compact Flash Cards required a delay       */
   /* between the time the Port was opened and the time when the Card   */
   /* was ready to accept data.  The default is NO Delay (0             */
   /* Milliseconds).                                                    */
#define QAPI_BLE_HCI_COMM_INFORMATION_INITIALIZATION_DELAY_MINIMUM     0
#define QAPI_BLE_HCI_COMM_INFORMATION_INITIALIZATION_DELAY_MAXIMUM  5000
#define QAPI_BLE_HCI_COMM_INFORMATION_INITIALIZATION_DELAY_DEFAULT     0

   /* The following type declaration defines the HCI Serial Protocol    */
   /* that will be used as the physical HCI Transport Protocol on the   */
   /* actual COM Port that is opened.  This type declaration is used in */
   /* the qapi_BLE_HCI_COMMDriverInformation_t structure that is        */
   /* required when an HCI COMM Port is opened.                         */
typedef enum
{
   QAPI_BLE_COMM_PROTOCOL_UART_E,
   QAPI_BLE_COMM_PROTOCOL_UART_RTS_CTS_E,
   QAPI_BLE_COMM_PROTOCOL_BCSP_E,
   QAPI_BLE_COMM_PROTOCOL_BCSP_MUZZLED_E,
   QAPI_BLE_COMM_PROTOCOL_H4DS_E,
   QAPI_BLE_COMM_PROTOCOL_H4DS_RTS_CTS_E,
   QAPI_BLE_COMM_PROTOCOL_HCILL_E,
   QAPI_BLE_COMM_PROTOCOL_HCILL_RTS_CTS_E,
   QAPI_BLE_COMM_PROTOCOL_3WIRE_E,
   QAPI_BLE_COMM_PROTOCOL_3WIRE_RTS_CTS_E,
   QAPI_BLE_COMM_PROTOCOL_SIBS_E,
   QAPI_BLE_COMM_PROTOCOL_SIBS_RTS_CTS_E
} qapi_BLE_HCI_COMM_Protocol_t;

typedef enum
{
   QAPI_BLE_COMM_DRIVER_NO_PARITY_E,
   QAPI_BLE_COMM_DRIVER_EVEN_PARITY_E
} qapi_BLE_HCI_COMM_Parity_t;

   /* The following prototype is for a callback function to handle      */
   /* changes in the sleep state of the COMM layer.  The first parameter*/
   /* indicates if sleep is allowed.  If this parameter is true, the    */
   /* application can safely power-down the associated hardware.  If    */
   /* this parameter is false, the application should ensure that the   */
   /* hardware is in a state that allows for normal communication.      */
   /* * NOTE * If the COM port is put to sleep, the application is      */
   /*          responsible for reactivating the port when data is       */
   /*          received or when needs to be sent.                       */
   /* * NOTE * The callback function should not make any direct calls   */
   /*          into the Bluetooth stack.                                */
   /* * NOTE * This function can be called in the context of any thread */
   /*          that calls a stack API or the HCITRANS COM Data Callback */
   /*          function.  No locks should be aquired within this        */
   /*          function that could also be held by another thread       */
   /*          accessing the stack or a thread lock may occur.          */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_HCI_Sleep_Callback_t)(boolean_t SleepAllowed, uint32_t CallbackParameter);

   /* The following type declaration represents the structure of all    */
   /* Data that is needed to open an HCI COMM Port.                     */
typedef struct qapi_BLE_HCI_COMMDriverInformation_s
{
   uint32_t                      DriverInformationSize;
                                                /* Physical Size of this      */
                                                /* structure.                 */
   uint32_t                      COMPortNumber;
                                                /* Physical COM Port Number   */
                                                /* of the Physical COM Port to*/
                                                /* Open.                      */
   uint32_t                      BaudRate;      
                                                /* Baud Rate to Open COM Port.*/
   qapi_BLE_HCI_COMM_Protocol_t  Protocol;
                                                /* HCI Protocol that will be  */
                                                /* used for communication over*/
                                                /* Opened COM Port.           */
   uint32_t                      InitializationDelay;
                                                /* Time (In Milliseconds) to  */
                                                /* Delay after the Port is    */
                                                /* opened before any data is  */
                                                /* sent over the Port.  This  */
                                                /* member is present because  */
                                                /* some PCMCIA/Compact Flash  */
                                                /* Cards have been seen that  */
                                                /* require a delay because the*/
                                                /* card does not function for */
                                                /* some specified period of   */
                                                /* time.                      */
  char                          *COMDeviceName;
                                                /* Physical Device Name to use*/
                                                /* to override the device to  */
                                                /* open.  If COMPortNumber is */
                                                /* specified to be the        */
                                                /* equivalent of negative 1   */
                                                /* (-1), then this value is   */
                                                /* taken as an absolute name  */
                                                /* and the COM Port Number is */
                                                /* NOT appended to this value.*/
                                                /* If this value is NULL then */
                                                /* the default (compiled) COM */
                                                /* Device Name is used (and   */
                                                /* the COM Port Number is     */
                                                /* appended to the default).  */
  uint32_t                       Flags;
                                                /* Flags for additional port  */
                                                /* information.  See          */
                                                /* definitions below for      */
                                                /* contents.                  */
} qapi_BLE_HCI_COMMDriverInformation_t;

   /* The following define the bit mask values of the flags member of   */
   /* the qapi_BLE_HCI_COMMDriverInformation_t structure.               */
#define QAPI_BLE_HCI_COMM_DRIVER_INFORMATION_FLAGS_USE_EVEN_PARITY_FLAG    0x0001

   /* The following type declaration represents the settings that can be*/
   /* changed for an already open HCI COM port.                         */
typedef struct qapi_BLE_HCI_COMMReconfigureInformation_s
{
   uint32_t                     ReconfigureFlags;
                                                /* Flags to indicate what     */
                                                /* aspects of the COMM port   */
                                                /* are to be reconfigured.    */
   qapi_BLE_HCI_COMM_Protocol_t Protocol;
                                                /* Specifies the protocol to  */
                                                /* be used for the COMM port. */
                                                /* This value is ignored if   */
                                                /* the Change Protocol flag is*/
                                                /* not set.                   */
   uint32_t                     BaudRate;
                                                /* Specifies the baudrate to  */
                                                /* be used for the COMM port. */
                                                /* This value is ignored if   */
                                                /* the Change BaudRate flag is*/
                                                /* not set.                   */
   boolean_t                    UseXonXoff;
                                                /* Specifies if Xon/Xoff flow */
                                                /* control should be used for */
                                                /* the COMM port. This value  */
                                                /* is ignored if the Change   */
                                                /* Xon/Xoff flag is not set.  */
   unsigned char                XonValue;
                                                /* Specifies The Xon and Xoff */
                                                /* values to be used for the  */
                                                /* COMM port. This value is   */
                                                /* ignored if the Change      */
                                                /* Xon/Xoff flag is not set or*/
                                                /* the Use Xon/Xoff flag is   */
                                                /* FALSE.                     */
   unsigned char                XoffValue;
                                                /* Specifies The Xon and Xoff */
                                                /* values to be used for the  */
                                                /* COMM port. This value is   */
                                                /* ignored if the Change      */
                                                /* Xon/Xoff flag is not set or*/
                                                /* the Use Xon/Xoff flag is   */
                                                /* FALSE.                     */
   qapi_BLE_HCI_COMM_Parity_t   ParityValue;
                                                /* Specifies the parity that  */
                                                /* should be used if the      */
                                                /* protocol in use allows     */
                                                /* different options.  Zero   */
                                                /* specifies no parity, or one*/
                                                /* for even parity.  This     */
                                                /* valus is ignored if the    */
                                                /* Change parity flag is not  */
                                                /* set or the setting is not  */
                                                /* supported by the protocol  */
                                                /* in use.                    */
} qapi_BLE_HCI_COMMReconfigureInformation_t;

   /* The following constants represent the bit mask of values for the  */
   /* ReconfigureFlags member of the                                    */
   /* qapi_BLE_HCI_COMM_ReconfigureInformation_t structure to indicate  */
   /* what parameters should be updated.  If a bit is set, its          */
   /* parameters will be updated.                                       */
#define QAPI_BLE_HCI_COMM_RECONFIGURE_INFORMATION_RECONFIGURE_FLAGS_CHANGE_PROTOCOL    0x0001
#define QAPI_BLE_HCI_COMM_RECONFIGURE_INFORMATION_RECONFIGURE_FLAGS_CHANGE_BAUDRATE    0x0002
#define QAPI_BLE_HCI_COMM_RECONFIGURE_INFORMATION_RECONFIGURE_FLAGS_CHANGE_XON_XOFF    0x0004
#define QAPI_BLE_HCI_COMM_RECONFIGURE_INFORMATION_RECONFIGURE_FLAGS_CHANGE_PARITY      0x0008

   /* The following structure defines the application configurable      */
   /* settings for an H4DS instance.                                    */
   /* * NOTE * For all parameters except the SleepCallbackFunction and  */
   /*          SleepCallbackParameter, a value of 0 will keep the       */
   /*          current setting.                                         */
typedef struct qapi_BLE_HCI_H4DSConfiguration_s
{
   uint32_t                      SyncTimeMS;
                                                /* The time between successive*/
                                                /* sync messages being sent.  */
   uint32_t                      ConfTimeMS;
                                                /* The time between successive*/
                                                /* conf messages being sent.  */
   uint32_t                      WakeUpTimeMS;
                                                /* The time between successive*/
                                                /* wake-up messages being     */
                                                /* sent.                      */
   uint32_t                      IdleTimeMS;
                                                /* The time of inactivity     */
                                                /* before the link is         */
                                                /* considered idle.           */
   uint32_t                      MaxTxFlushTimeMS;
                                                /* The time to wait for the   */
                                                /* transmit buffer to be      */
                                                /* flushed before the         */
                                                /* connection can be put to   */
                                                /* sleep.                     */
   uint32_t                      WakeUpCount;
                                                /* The number of wake-up      */
                                                /* messages that will be sent */
                                                /* before the peer is assumed */
                                                /* idle.                      */
   uint32_t                      TicksPerWakueUp;
                                                /* The number of ticks that   */
                                                /* will be sent before every  */
                                                /* wake-up message.           */
   qapi_BLE_HCI_Sleep_Callback_t SleepCallbackFunction;
                                                /* Provides the function to be*/
                                                /* used for sleep callback    */
                                                /* indications. A NULL value  */
                                                /* will disable callbacks.    */
   uint32_t                      SleepCallbackParameter;
                                                /* The Parameter to be passed */
                                                /* with the callback function.*/
                                                /* This value is ignored if   */
                                                /* the SleepCallbackFunction  */
                                                /* specified is NULL.         */
} qapi_BLE_HCI_H4DSConfiguration_t;

   /* The following structure defines the application configurable      */
   /* settings for an HCILL instance.                                   */
typedef struct qapi_BLE_HCI_HCILLConfiguration_s
{
   qapi_BLE_HCI_Sleep_Callback_t SleepCallbackFunction;
                                                /* Provides the function to be*/
                                                /* used for sleep callback    */
                                                /* indications. A NULL value  */
                                                /* will disable callbacks.    */
   uint32_t                      SleepCallbackParameter; 
                                                /* The Parameter to be passed */
                                                /* with the callback function.*/
                                                /* This value is ignored if   */
                                                /* the SleepCallbackFunction  */
                                                /* specified is NULL.         */
} qapi_BLE_HCI_HCILLConfiguration_t;

   /* The following struct contains the configuration for the link of   */
   /* the 3-Wire interface.  These parameters will only be applied      */
   /* during the link negotiation with the baseband that takes place    */
   /* when the first is sent or when the 3-wire state machines are reset*/
   /* via the qapi_BLE_HCI_Reconfigure_Driver() function.               */
   /* * NOTE * That resetting the state machines will result is any     */
   /*          currently queued packets being lost so should only be    */
   /*          done when the interface is idle, usually during initial  */
   /*          setup.                                                   */
typedef struct qapi_BLE_HCI_3WireLinkConfiguration_s
{
   uint32_t             SlidingWindowSize;      /* The maximum size of the    */
                                                /* 3-wire sliding window in   */
                                                /* packets.                   */
   boolean_t            SupportCRC;             /* Denotes if a CRC Data      */
                                                /* Integrity Check is         */
                                                /* supported.                 */
   boolean_t            SupportOOFFlowControl;  /* Denotes if a OOF Flow      */
                                                /* Control (Xon/Xoff) is      */
                                                /* supported.                 */
} qapi_BLE_HCI_3WireLinkConfiguration_t;

   /* The following structure defines the application configuration     */
   /* settings for the Three Wire UART interface.                       */
   /* * NOTE * For all parameters except the SleepCallbackFunction,     */
   /*          SleepCallbackParameter, LinkConfiguration, and Flags, a  */
   /*          value of zero will keep the current setting.  The        */
   /*          LinkConfiguration will keep its current settings if the  */
   /*          SlidingWindowSize is set to zero.                        */
typedef struct qapi_BLE_HCI_3WireConfiguration_s
{
   qapi_BLE_HCI_3WireLinkConfiguration_t  LinkConfiguration;
                                                /* The link configuration     */
                                                /* to use. If the Sliding     */
                                                /* Window size is 0, then the */
                                                /* settings in this structure */
                                                /* are ignored. Additionally, */
                                                /* these settings will not    */
                                                /* have an affect unless the  */
                                                /* 3-Wire state machines are  */
                                                /* restarted.                 */
   uint32_t                               IdleTimeMS;
                                                /* The time of inactivity     */
                                                /* before the link is         */
                                                /* considered idle and put to */
                                                /* sleep.                     */
   uint32_t                               RetransmitTimeMS;
                                                /* The time between packet    */
                                                /* retransmissions.           */
   uint32_t                               AcknowledgeDelayMS;
                                                /* The maximum time to        */
                                                /* delay before acknowledging */
                                                /* a received packet.         */
   qapi_BLE_HCI_Sleep_Callback_t          SleepCallbackFunction; 
                                                /* Provides the               */
                                                /* function to be used for    */
                                                /* sleep callback indications.*/
                                                /* A NULL value will disable  */
                                                /* callbacks.                 */
   uint32_t                               SleepCallbackParameter;
                                                /* The Parameter to  */
                                                /* be passed with the callback*/
                                                /* function.  This value is   */
                                                /* ignored if the             */
                                                /* SleepCallbackFunction      */
                                                /* specified is NULL.         */
   uint32_t                               Flags;
                                                /* Miscellaneous configuration*/
                                                /* flags for 3-wire Interface.*/
} qapi_BLE_HCI_3WireConfiguration_t;

   /* The following constants represent the bit mask of values for the  */
   /* Flags member of the qapi_BLE_HCI_3WireConfiguration_t.            */
#define QAPI_BLE_HCI_3WIRE_CONFIGURATION_FLAGS_SCO_IS_RELIABLE                      0x0001

   /* The following structure defines the application configurable      */
   /* settings for an SIBS instance.                                    */
typedef struct qapi_BLE_HCI_SIBSConfiguration_s
{
   uint32_t                      TxIdleTimeMs;
                                                /* The time of inactivity     */
                                                /* before the transmit link is*/
                                                /* considered idle and put to */
                                                /* sleep.                     */
   uint32_t                      RetransmitTimeMs;
                                                /* The time between message   */
                                                /* retransmissions.           */
   boolean_t                     AggressiveSleep;
                                                /* This flag indicates        */
                                                /* that the SIBS instance     */
                                                /* should use Aggressive Sleep*/
                                                /* mode.  In this mode, the   */
                                                /* transmit link is put to    */
                                                /* sleep after the packet     */
                                                /* queue is emptied instead of*/
                                                /* waiting for the idle timer */
                                                /* to expire.                 */
   qapi_BLE_HCI_Sleep_Callback_t SleepCallbackFunction;
                                                /* Provides the function to be*/
                                                /* used for sleep callback    */
                                                /* indications. A NULL value  */
                                                /* will disable callbacks.    */
   uint32_t                      SleepCallbackParameter;
                                                /* The Parameter to be passed */
                                                /* with the callback function.*/
                                                /* This value is ignored if   */
                                                /* the SleepCallbackFunction  */
                                                /* specified is NULL.         */
} qapi_BLE_HCI_SIBSConfiguration_t;

   /* This command has been deprecated, Use                             */
   /* QAPI_BLE_HCI_COMM_DRIVER_RECONFIGURE_DATA_COMMAND_CHANGE_COMM_PARA*/
   /* to change the baud rate of the comm port.                         */
#define QAPI_BLE_HCI_COMM_DRIVER_RECONFIGURE_DATA_COMMAND_CHANGE_PARAMETERS         (QAPI_BLE_HCI_DRIVER_RECONFIGURE_DATA_RECONFIGURE_COMMAND_TRANSPORT_START)

   /* The following constant is used with the                           */
   /* qapi_BLE_HCI_Driver_Reconfigure_Data_t structure                  */
   /* (ReconfigureCommand member) to specify that the H4DS parameters   */
   /* are required to change.  When specified, the ReconfigureData      */
   /* member will point to a valid qapi_BLE_HCI_H4DSConfigureation_t    */
   /* structure which holds the new parameters.                         */
   /* * NOTE * HCI_ReconfigureDriver will return 1 if the connection is */
   /*          currently asleep, 0 if it is not asleep or a negative    */
   /*          value if there is an error fore this command.            */
#define QAPI_BLE_HCI_COMM_DRIVER_RECONFIGURE_DATA_COMMAND_CHANGE_H4DS_PARAMETERS    (QAPI_BLE_HCI_DRIVER_RECONFIGURE_DATA_RECONFIGURE_COMMAND_TRANSPORT_START + 1)

   /* The following constant is used with the                           */
   /* qapi_BLE_HCI_Driver_Reconfigure_Data_t structure                  */
   /* (ReconfigureCommand member) to specify that the HCILL parameters  */
   /* are required to change.  When specified, the ReconfigureData      */
   /* member will point to a valid qapi_BLE_HCI_HCILLConfigureation_t   */
   /* structure which holds the new parameters.                         */
   /* * NOTE * HCI_ReconfigureDriver will return 1 if the connection is */
   /*          currently asleep, 0 if it is not asleep or a negative    */
   /*          value if there is an error fore this command.            */
#define QAPI_BLE_HCI_COMM_DRIVER_RECONFIGURE_DATA_COMMAND_CHANGE_HCILL_PARAMETERS   (QAPI_BLE_HCI_DRIVER_RECONFIGURE_DATA_RECONFIGURE_COMMAND_TRANSPORT_START + 2)

   /* The following constant is used with the                           */
   /* qapi_BLE_HCI_Driver_Reconfigure_Data_t structure                  */
   /* (ReconfigureCommand member) to specify that the Three Wire UART   */
   /* parameters are required to change.  When specified, the           */
   /* ReconfigureData member will point to a valid                      */
   /* qapi_BLE_HCI_3WireConfigureation_t structure which holds the      */
   /* protocol parameters.                                              */
   /* * NOTE * HCI_ReconfigureDriver will return 1 if the connection is */
   /*          currently asleep, 0 if it is not asleep or a negative    */
   /*          value if there is an error fore this command.            */
#define QAPI_BLE_HCI_COMM_DRIVER_RECONFIGURE_DATA_COMMAND_CHANGE_3WIRE_PARAMETERS   (QAPI_BLE_HCI_DRIVER_RECONFIGURE_DATA_RECONFIGURE_COMMAND_TRANSPORT_START + 3)

   /* The following constant is used with the                           */
   /* qapi_BLE_HCI_Driver_Reconfigure_Data_t structure                  */
   /* (ReconfigureCommand member) to specify reconfiguring of the port  */
   /* parameters.  When specified, the ReconfigureData member will point*/
   /* to a valid qapi_BLE_HCI_COMMReconfigureInformation_s structure    */
   /* which holds the information for the parameters to be changed.     */
#define QAPI_BLE_HCI_COMM_DRIVER_RECONFIGURE_DATA_COMMAND_CHANGE_COMM_PARAMETERS    (QAPI_BLE_HCI_DRIVER_RECONFIGURE_DATA_RECONFIGURE_COMMAND_TRANSPORT_START + 4)

   /* The following constant is used with the                           */
   /* qapi_BLE_HCI_Driver_Reconfigure_Data_t structure                  */
   /* (ReconfigureCommand member) to specify that the SIBS parameters   */
   /* are required to change.  When specified, the ReconfigureData      */
   /* member will point to a valid qapi_BLE_HCI_SIBSConfigureation_t    */
   /* structure which holds the new parameters.                         */
   /* * NOTE * HCI_ReconfigureDriver will return 1 if the connection is */
   /*          currently asleep, 0 if it is not asleep or a negative    */
   /*          value if there is an error fore this command.            */
#define QAPI_BLE_HCI_COMM_DRIVER_RECONFIGURE_DATA_COMMAND_CHANGE_SIBS_PARAMETERS     (QAPI_BLE_HCI_DRIVER_RECONFIGURE_DATA_RECONFIGURE_COMMAND_TRANSPORT_START + 5)

   /* The following constants represent the Minimum and Maximum time in */
   /* milliseconds before the H4DS layer considers the port to be idle. */
#define QAPI_BLE_HCI_H4DS_MINIMUM_IDLE_TIME                       50
#define QAPI_BLE_HCI_H4DS_MAXIMUM_IDLE_TIME                       30000

   /* The following constants represent the Minimum and Maximum time in */
   /* milliseconds between wake-up messages being sent by the H4DS      */
   /* layer.                                                            */
#define QAPI_BLE_HCI_H4DS_MINIMUM_WAKE_UP_MESSAGE_TIME            50
#define QAPI_BLE_HCI_H4DS_MAXIMUM_WAKE_UP_MESSAGE_TIME            1000

   /* The following constants represent the Minimum and Maximum number  */
   /* of ticks sent before each wake-up message by the H4DS layer.      */
#define QAPI_BLE_HCI_H4DS_MINIMUM_TICKS_PER_WAKE_UP               1
#define QAPI_BLE_HCI_H4DS_MAXIMUM_TICKS_PER_WAKE_UP               50

   /* The following constants represent the Minimum and Maximum number  */
   /* of Wake-up messages sent by the H4DS layer before the peer is     */
   /* considered idle.                                                  */
#define QAPI_BLE_HCI_H4DS_MINIMUM_WAKE_UP_MESSAGE_COUNT           1
#define QAPI_BLE_HCI_H4DS_MAXIMUM_WAKE_UP_MESSAGE_COUNT           100

   /* The following constants represent the Minimum and Maximum time in */
   /* milliseconds that the H4DS layer waits for the transmit buffers to*/
   /* be flushed before transitioning to a state where sleep is allowed.*/
#define QAPI_BLE_HCI_H4DS_MINIMUM_TRANSMIT_FLUSH_TIME             50
#define QAPI_BLE_HCI_H4DS_MAXIMUM_TRANSMIT_FLUSH_TIME             30000

   /* The following constants represent the Minimum and Maximum time in */
   /* milliseconds between SYNC messages being sent by the H4DS layer.  */
#define QAPI_BLE_HCI_H4DS_MINIMUM_SYNC_MESSAGE_TIME               50
#define QAPI_BLE_HCI_H4DS_MAXIMUM_SYNC_MESSAGE_TIME               1000

   /* The following constants represent the Minimum and Maximum time in */
   /* milliseconds between CONF messages being sent by the H4DS layer.  */
#define QAPI_BLE_HCI_H4DS_MINIMUM_CONF_MESSAGE_TIME               50
#define QAPI_BLE_HCI_H4DS_MAXIMUM_CONF_MESSAGE_TIME               1000

   /* The following constants represent the Minimum and Maximum values  */
   /* in packets that can be used for the 3-Wire sliding window size.   */
#define QAPI_BLE_HCI_3WIRE_MINIMUM_SLIDING_WINDOW_SIZE            1
#define QAPI_BLE_HCI_3WIRE_MAXIMUM_SLIDING_WINDOW_SIZE            7

   /* The following constants represent the Minimum and Maximum time in */
   /* milliseconds before the Three Wire UART layer considers the port  */
   /* to be idle.                                                       */
#define QAPI_BLE_HCI_3WIRE_MINIMUM_IDLE_TIME                      50
#define QAPI_BLE_HCI_3WIRE_MAXIMUM_IDLE_TIME                      30000

   /* The following constants represent the Minimum and Maximum time in */
   /* milliseconds before the Three Wire UART layer will retransmit a   */
   /* packet.                                                           */
#define QAPI_BLE_HCI_3WIRE_MINIMUM_RETRANSMIT_TIME                50
#define QAPI_BLE_HCI_3WIRE_MAXIMUM_RETRANSMIT_TIME                1000

   /* The following constants represent the Minimum and Maximum time in */
   /* milliseconds before the Three Wire UART layer to wait before      */
   /* acknowledging a received packet.                                  */
#define QAPI_BLE_HCI_3WIRE_MINIMUM_ACKNOWLEDGE_DELAY              50
#define QAPI_BLE_HCI_3WIRE_MAXIMUM_ACKNOWLEDGE_DELAY              500

   /* The following constants represent the Minimum and Maximum time in */
   /* milliseconds before the QCA In-Band Sleep protocol considers the  */
   /* port to be idle.                                                  */
#define QAPI_BLE_HCI_SIBS_MINIMUM_TX_IDLE_TIME                    10
#define QAPI_BLE_HCI_SIBS_MAXIMUM_TX_IDLE_TIME                    30000

   /* The following constants represent the Minimum and Maximum time in */
   /* milliseconds between message retransmission in the QCA In-Band    */
   /* Sleep protocol.                                                   */
#define QAPI_BLE_HCI_SIBS_MINIMUM_RETRANSMIT_TIME                 10
#define QAPI_BLE_HCI_SIBS_MAXIMUM_RETRANSMIT_TIME                 1000

#endif
