 /******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*! \file qmesh_ble_gap.c
 *  \brief qmesh ble gap specific functions
 *
 *   This file contains the function definitions to provide applications
 *   to configure and control LE Bearer.
 */
/*****************************************************************************/

/*============================================================================*
 *  Standard Library Header Files
 *============================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*============================================================================*
 *  Local Header Files
 *============================================================================*/
#include "qmesh_sched.h"
#include "qmesh_ble_gap.h"
#include "qmesh_gw_debug.h"
#include "qmesh_ble_gatt.h"
#include "qmesh_ble_gatt_client.h"
#include "qmesh_bearer_debug.h"


extern QCLI_Group_Handle_t     mesh_group;
/*============================================================================*
 *  Private Definitions
 *============================================================================*/

/*============================================================================*
 *  Private Data
 *============================================================================*/

#define DEVICE_FRIENDLY_NAME       "QUARTZ"         /* Bluetooth Device Friendly Name */

#define GATT_PKT_MAX_QUEUE_SIZE    (5)              /* Max GATT Packets Queue Size    */

static int32_t  BluetoothStackID;                   /* Variable which holds the Handle */
                                                    /* of the opened Bluetooth Protocol*/
                                                    /* Stack.                          */

static qapi_BLE_HCI_DriverInformation_t HCI_DriverInformation;
                                                    /* The HCI Driver structure that   */
                                                    /* holds the HCI Transports        */
                                                    /* settings used to initialize     */
                                                    /* Bluetopia.                      */


/* Generic Access Profile (GAP) Internal Variables.                */
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


/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/
 /*----------------------------------------------------------------------------*
 * _openStack
 */
/*! \brief This function is used to open the Bluetopia Bluetooth Stack
 *
 *  \param HCI_DriverInformation Pointer to HCI Driver Information structure thats
 *                               contains HCI Transport Information.
 *
 *  \returns 0 on Success and a negative value on all errors.
 */
/*---------------------------------------------------------------------------*/
static int32_t _openStack(qapi_BLE_HCI_DriverInformation_t *HCI_DriverInformation);

 /*----------------------------------------------------------------------------*
 * _closeStack
 */
/*! \brief DeInitializes Bluetopia Stack
 *
 *  \param
 *
 *  \returns TRUE if DeIntialize Bluetooth Stack successfuly, else FALSE.
 */
/*---------------------------------------------------------------------------*/
static bool _closeStack(void);

 /*----------------------------------------------------------------------------*
 * _setBluetoothStackId
 */
/*! \brief Set Bluetooth Stack ID variable
 *
 *  \param stackId Bluetooth Stack ID .
 *
 *  \returns
 */
/*---------------------------------------------------------------------------*/
static void _setBluetoothStackId(int32_t stackId);

 /*----------------------------------------------------------------------------*
 * GenerateRandomKeys
 */
/*! \brief This utility function is used to generate random values for the ER and the IR.
 *
 *  \param
 *
 *  \returns
 */
/*---------------------------------------------------------------------------*/
static void GenerateRandomKeys(void);

 /*----------------------------------------------------------------------------*
 * _gatt_bearer_gap_le_event_cb
 */
/*! \brief This function is used as callback function to receive all
 *         GAP LE specific events from Bluetopia stack.
 *
 *  \param stackId Bluetooth Stack ID .
 *  \param GAP_LE_Event_Data Pointer to GAP LE Event data
 *  \param CallbackParameter Parameter provided during registering the callback function
 *
 *  \returns
 */
/*---------------------------------------------------------------------------*/
static void QAPI_BLE_BTPSAPI _gatt_bearer_gap_le_event_cb(uint32_t btStackId,
                                        qapi_BLE_GAP_LE_Event_Data_t *GAP_LE_Event_Data,
                                        uint32_t CallbackParameter);

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/* The following function is a utility function to return the ascii format of the */
/* passed 1 byte (unsigned int) value (0 - 15)                                    */
static char hex_to_ascii(uint8 val)
{
    if(val <= 9)
        return '0' + val;
    else if(val >= 10 && val <= 15)
        return 'A' + val - 10;

    return '0';
}

/* The following function is a utility function to convert byte value into */
/* Hex String.                                                             */
static void convert_u8_str(char *str, uint8 val)
{
    str[0] = hex_to_ascii((val >> 4) & 0xF);
    str[1] = hex_to_ascii(val & 0xF);
}


/* The following function is a utility function for converting data of type */
/* BD_ADDR to a string.  The first parameter of this function is the        */
/* BD_ADDR to be converted to a string.  The second parameter of this       */
/* function is a pointer to the string in which the converted BD_ADDR       */
/* is to be stored.                                                         */
static void BD_ADDRToStr(qapi_BLE_BD_ADDR_t Board_Address, char *BoardStr)
{
    convert_u8_str(BoardStr, Board_Address.BD_ADDR5);
    convert_u8_str(&BoardStr[2], Board_Address.BD_ADDR4);
    convert_u8_str(&BoardStr[4], Board_Address.BD_ADDR3);
    convert_u8_str(&BoardStr[6], Board_Address.BD_ADDR2);
    convert_u8_str(&BoardStr[8], Board_Address.BD_ADDR1);
    convert_u8_str(&BoardStr[12], Board_Address.BD_ADDR0);
}

static void _setBluetoothStackId(int32_t stackId)
{
    BluetoothStackID = stackId;
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
   if((!qapi_BLE_HCI_LE_Rand(GetBluetoothStackId(),  &Status,  &RandomNumber)) && (!Status))
      memcpy(&ER, &RandomNumber, MaxSize);

   if((!qapi_BLE_HCI_LE_Rand(GetBluetoothStackId(),  &Status,  &RandomNumber)) && (!Status))
      memcpy(&(((int8_t *)&ER)[sizeof(qapi_BLE_Encryption_Key_t) / 2]), &RandomNumber, MaxSize);

   if((!qapi_BLE_HCI_LE_Rand(GetBluetoothStackId(),  &Status,  &RandomNumber)) && (!Status))
      memcpy(&IR, &RandomNumber, MaxSize);

   if((!qapi_BLE_HCI_LE_Rand(GetBluetoothStackId(),  &Status,  &RandomNumber)) && (!Status))
      memcpy(&(((int8_t *)&IR)[sizeof(qapi_BLE_Encryption_Key_t) / 2]), &RandomNumber, MaxSize);
}

/* The following function is responsible for opening the SS1         */
/* Bluetooth Protocol Stack.  This function accepts a pre-populated  */
/* HCI Driver Information structure that contains the HCI Driver     */
/* Transport Information.  This function returns zero on successful  */
/* execution and a negative value on all errors.                     */
static int32_t _openStack(qapi_BLE_HCI_DriverInformation_t *HCI_DriverInformation)
{
   int32_t                 Result;
   int32_t                 ret_val = 0;
   char                    BluetoothAddress[16];
   uint32_t                serviceID;

   /* First check to see if the Stack has already been opened.          */
   if(!GetBluetoothStackId())
   {
      /* Next, makes sure that the Driver Information passed is valid */
      if(HCI_DriverInformation)
      {
         DEBUG_BEARER_INFO(mesh_group, "OpenStack()\n");
         /* Set the HCI driver information.                             */
         QAPI_BLE_HCI_DRIVER_SET_COMM_INFORMATION(HCI_DriverInformation, 1, 115200,
                                                    QAPI_BLE_COMM_PROTOCOL_UART_E);
         /* Initialize the Bluetooth Stack                              */
         Result = qapi_BLE_BSC_Initialize(HCI_DriverInformation, 0);

         /* Check the return value of the initialization to see if it was successful.*/
         if(Result > 0)
         {
            /* Set the return value of the initialization function   */
            /* to the Bluetooth Stack ID.                               */
            _setBluetoothStackId(Result);
            DEBUG_BEARER_INFO(mesh_group, "Bluetooth Stack ID: %d.\n", GetBluetoothStackId());

            /* Retrieve Bluetooth Device Address                        */
            if(!qapi_BLE_GAP_Query_Local_BD_ADDR(GetBluetoothStackId(), &LocalBD_ADDR))
            {
               BD_ADDRToStr(LocalBD_ADDR, BluetoothAddress);
               DEBUG_BEARER_INFO(mesh_group, "Local BD_ADDR: %s\n", BluetoothAddress);
            }

            /* Generate some random values for IR and ER (normally would be in flash).   */
            GenerateRandomKeys();
            DEBUG_BEARER_INFO(mesh_group, "Random keys successfully generated\n");
            /* Regenerate IRK and DHK from the constant Identity Root Key */
            qapi_BLE_GAP_LE_Diversify_Function(GetBluetoothStackId(), (qapi_BLE_Encryption_Key_t *)(&IR), 1,0, &IRK);
            qapi_BLE_GAP_LE_Diversify_Function(GetBluetoothStackId(), (qapi_BLE_Encryption_Key_t *)(&IR), 3, 0, &DHK);

            /* Initialize the GATT Service.                                */
            if((Result = qapi_BLE_GATT_Initialize(GetBluetoothStackId(),
                          (QAPI_BLE_GATT_INITIALIZATION_FLAGS_SUPPORT_LE | QAPI_BLE_GATT_INITIALIZATION_FLAGS_DISABLE_SERVICE_CHANGED_CHARACTERISTIC),
                          GATT_Connection_Event_Callback, 0)) == 0)
            {
                uint32_t maxNumDataPkt = GATT_PKT_MAX_QUEUE_SIZE;
                /* Set Queuing Parameters */
                Result = qapi_BLE_GATT_Set_Queuing_Parameters(BluetoothStackID, maxNumDataPkt,
                                                            maxNumDataPkt-1, TRUE);
                if (Result != 0)
                {
                    DEBUG_BEARER_ERROR(mesh_group, "GATT Set Queuing Parameters failed, error %d\n", Result);
                }
                /* Initialize the GAPS Service.                             */
                Result = qapi_BLE_GAPS_Initialize_Service(GetBluetoothStackId(), &serviceID);
                if(Result > 0)
                {
                    DEBUG_BEARER_INFO(mesh_group, "GAP Service Id %x, Result %x\n", serviceID, Result);
                    /* Set the GAP Device Name.        */
                   if (qapi_BLE_GAPS_Set_Device_Name(GetBluetoothStackId(), serviceID, DEVICE_FRIENDLY_NAME) > 0)
                   {
                        DEBUG_BEARER_INFO(mesh_group, "GAP Service Name %s\n", DEVICE_FRIENDLY_NAME);
                   }
                   /* Set the GAP Device Appearance.        */
                   if (qapi_BLE_GAPS_Set_Device_Appearance(GetBluetoothStackId(),
                                               serviceID,
                                               QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_COMPUTER) > 0)
                   {
                        DEBUG_BEARER_INFO(mesh_group, "GAP Device Appearence Set %x\n",
                            QAPI_BLE_GAP_DEVICE_APPEARENCE_VALUE_GENERIC_COMPUTER);
                   }
                }
            }

            /* Enable address resolution in the controller.       */
            Result = qapi_BLE_GAP_LE_Set_Address_Resolution_Enable(GetBluetoothStackId(), TRUE);
            if(!Result)
            {
               /* Set Resolvable Private Address (RPA) Timeout to 60 seconds.*/
               Result = qapi_BLE_GAP_LE_Set_Resolvable_Private_Address_Timeout(GetBluetoothStackId(), 60);
               if(Result)
               {
                    DEBUG_BEARER_ERROR(mesh_group, "Error - set resolvable private address returned %d.\n", Result);
               }
            }
            else
            {
                DEBUG_BEARER_ERROR(mesh_group, "Error - set addr resolution enable returned %d.\n", Result);
            }
            /* Set default data length to the maximum.        */
            Result = qapi_BLE_GAP_LE_Set_Default_Data_Length(GetBluetoothStackId(), 251, 2120);
            if(Result)
            {
                DEBUG_BEARER_INFO(mesh_group, "qapi_BLE_GAP_LE_Set_Default_Data_Length %d \n", Result);
            }
         }
         else
         {
            /* Stack is NOT initialized successfully, inform the   */
            /* user and set the return value of the initialization      */
            /* function to an error.                                    */
            DEBUG_BEARER_ERROR(mesh_group, "qapi_BLE_BSC_Initialize() %d", Result);
            _setBluetoothStackId(0);

            ret_val = -1;
         }
      }
      else
      {
         /* HCI Driver Information is not invalid.        */
         DEBUG_BEARER_ERROR(mesh_group, "HCI Driver Information is not valid\n");
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

static bool _closeStack(void)
{
    bool    ret_val = FALSE;

    /* First check to see if the Stack has been opened.                  */
    if(GetBluetoothStackId())
    {
        /* Cleanup GATT Module.                                           */
        qapi_BLE_GATT_Cleanup(GetBluetoothStackId());

        /* Simply close the Stack                                         */
        qapi_BLE_BSC_Shutdown(GetBluetoothStackId());

        /* Inform the user that the stack has been shut down.             */
        DEBUG_BEARER_INFO(mesh_group, "Stack Shutdown.\n");

        /* Flag that the Stack is no longer initialized.                  */
        _setBluetoothStackId(0);

        /* Flag success to the caller.                                    */
        ret_val          = TRUE;
    }
    else
    {
        DEBUG_BEARER_ERROR(mesh_group, " Bluetooth Stack ID is not valid\n");
    }

    return(ret_val);
}


/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/

bool InitializeBluetooth(void)
{
    int32_t   Result;
    bool      ret_val = FALSE;

    DEBUG_BEARER_INFO(mesh_group, "InitializeBluetooth()\n");
    /* First, check that the stack is not currently initialized.         */
    if (GetBluetoothStackId() == 0)
    {
        /* Attempt to open the stack.                                     */
        Result = _openStack(&HCI_DriverInformation);
        if(!Result)
        {
            ret_val = TRUE;
        }
        else
        {
            _closeStack();
        }
    }
    else
    {
        /* No valid Bluetooth Stack ID exists.                            */
        DEBUG_BEARER_INFO(mesh_group, "Bluetooth stack is already initialized\n");
    }

    return(ret_val);
}


int32_t GetBluetoothStackId(void)
{
    return BluetoothStackID;
}

bool InitiateGattConnection(qapi_BLE_GAP_LE_Address_Type_t remDevAddrType,
                             qapi_BLE_BD_ADDR_t *remDevAddr,
                             QMESH_GATT_BEARER_SERVICE_TYPE_T srvc)
{
    int8_t retVal=-1;
    qapi_BLE_GAP_LE_Connection_Parameters_t connParams;

    DEBUG_BEARER_INFO (mesh_group, "CreateConnection Address 0x%2x%2x%2x%2x%2x%2x\n",
            remDevAddr->BD_ADDR5,remDevAddr->BD_ADDR4,remDevAddr->BD_ADDR3,remDevAddr->BD_ADDR2,
            remDevAddr->BD_ADDR1,remDevAddr->BD_ADDR0);

    connParams.Connection_Interval_Max = MAX_CONNECTION_INTERVAL;
    connParams.Connection_Interval_Min = MIN_CONNECTION_INTERVAL;
    connParams.Slave_Latency = SLAVE_LATENCY;
    connParams.Supervision_Timeout = SUPERVISION_TIMEOUT;
    connParams.Maximum_Connection_Length = MAX_CONNECTION_LENGTH;
    connParams.Minimum_Connection_Length = MIN_CONNECTION_LENGTH;

    SetGATTClientServiceType(srvc);
    /* Stopping the scheduler while establishing the connection
     * as sometimes connection fails to establish if schedular is running */
    QmeshSchedStop();

    retVal = qapi_BLE_GAP_LE_Create_Connection(GetBluetoothStackId(),
                                                SCAN_INTERVAL,
                                                SCAN_WINDOW,
                                                QAPI_BLE_FP_NO_FILTER_E,
                                                remDevAddrType,
                                                remDevAddr,
                                                QAPI_BLE_LAT_PUBLIC_E,
                                                &connParams,
                                                _gatt_bearer_gap_le_event_cb,
                                                0);

    if(retVal == 0)
        return TRUE;
    else
    {
        DEBUG_BEARER_ERROR(mesh_group, "Failed to intiate connection, Error = %d\n", retVal);
        return FALSE;
    }
}


bool SendConnectionParamUpdateRequest(qapi_BLE_BD_ADDR_t remDevAddr)
{
    int result;
    bool retVal = FALSE;

    DEBUG_BEARER_INFO (mesh_group, "BEARER : SendConnectionParamUpdateRequest device %x:%x:%x:%x:%x:%x\n",
                        remDevAddr.BD_ADDR0, remDevAddr.BD_ADDR1, remDevAddr.BD_ADDR2,
                        remDevAddr.BD_ADDR3, remDevAddr.BD_ADDR4, remDevAddr.BD_ADDR5);

    result = qapi_BLE_GAP_LE_Connection_Parameter_Update_Request(GetBluetoothStackId(), remDevAddr,
                                         MIN_CONNECTION_INTERVAL,
                                         MAX_CONNECTION_INTERVAL,
                                         SLAVE_LATENCY,SUPERVISION_TIMEOUT);

    if(result == 0)
    {
        retVal = TRUE;
    }
    else
    {
        DEBUG_BEARER_ERROR(mesh_group, "Failed to send to connection param update request, Error=%d\n", result);
    }

    return retVal;
}


bool SendConnectionParamUpdateResponse(qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t *data)
{
    /* Check if passed data is valid */
    if(!data)
        return FALSE;

    qapi_BLE_GAP_LE_Connection_Parameters_t connParams, *ptr=NULL;
    bool acceptParams = FALSE;
    bool retVal = FALSE;
    int result;

    memset(&connParams, 0, sizeof(qapi_BLE_GAP_LE_Connection_Parameters_t));

    DEBUG_BEARER_INFO(mesh_group, "Received QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_REQUEST_E connIntvlMin=%x, connIntvlMax=%x, connSupervisionTimeout=%x, latency=%x\n",
        data->Conn_Interval_Min, data->Conn_Interval_Max, data->Conn_Supervision_Timeout, data->Slave_Latency);

    /*Making sure atleast the interval_min of remote device is less than the interval_max of our device to have a lesser connInterval*/
    if(data->Conn_Interval_Max <= MAX_PERMISSABLE_CONNECTION_INTERVAL)
    {
        DEBUG_BEARER_INFO(mesh_group, "Accepting connection parameters\n");
        acceptParams = TRUE;
        /*If Parameters are accepted only the stack checks the qapi_BLE_GAP_LE_Connection_Parameters_t*/
        connParams.Connection_Interval_Max = data->Conn_Interval_Max;
        connParams.Connection_Interval_Min = data->Conn_Interval_Min;
        connParams.Slave_Latency = data->Slave_Latency;
        connParams.Supervision_Timeout = data->Conn_Supervision_Timeout;
        connParams.Maximum_Connection_Length = MAX_CONNECTION_LENGTH;
        connParams.Minimum_Connection_Length = MIN_CONNECTION_LENGTH;
        ptr = &connParams;
    }

    result = qapi_BLE_GAP_LE_Connection_Parameter_Update_Response(GetBluetoothStackId(), data->BD_ADDR, acceptParams, ptr);

    if(result == 0)
    {
        retVal = TRUE;
    }
    else
    {
        DEBUG_BEARER_ERROR(mesh_group, "connection parameter update response send failed, Error=%d \n", result);
    }

    return retVal;
}


void InitiateGattDisconnection(qapi_BLE_BD_ADDR_t remoteAddr)
{
   int32_t status;

   DEBUG_BEARER_INFO (mesh_group,"InitiateGattDisconection\n");

   status = qapi_BLE_GAP_LE_Disconnect(GetBluetoothStackId(), remoteAddr);

   if(status)
   {
       DEBUG_BEARER_ERROR (mesh_group, "Disconect request failed due to %d\n", status);
   }
}


/* ***************************************************************** */
/*                         Event Callbacks                           */
/* ***************************************************************** */
static void QAPI_BLE_BTPSAPI _gatt_bearer_gap_le_event_cb(uint32_t btStackId, qapi_BLE_GAP_LE_Event_Data_t *GAP_LE_Event_Data, uint32_t CallbackParameter)
{

    if(btStackId == GetBluetoothStackId())
    {
        DEBUG_BEARER_INFO(mesh_group, "adv_bearer_gap_le_event_cb Event received %x \n", GAP_LE_Event_Data->Event_Data_Type);
        switch(GAP_LE_Event_Data->Event_Data_Type)
        {
            case QAPI_BLE_ET_LE_CONNECTION_COMPLETE_E:
            {
                #ifdef DEBUG_GAP_GATT_BEARER
                qapi_BLE_GAP_LE_Connection_Complete_Event_Data_t *data =
                    GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Complete_Event_Data;

                DEBUG_BEARER_INFO(mesh_group, "Received QAPI_BLE_ET_LE_CONNECTION_COMPLETE_E status=%x\n",data->Status);
                #endif
            }
            break;

            case QAPI_BLE_ET_LE_DISCONNECTION_COMPLETE_E:
            {
                #ifdef DEBUG_GAP_GATT_BEARER
                qapi_BLE_GAP_LE_Disconnection_Complete_Event_Data_t *data =
                    GAP_LE_Event_Data->Event_Data.GAP_LE_Disconnection_Complete_Event_Data;

                DEBUG_BEARER_INFO(mesh_group, "Received QAPI_BLE_ET_LE_DISCONNECTION_COMPLETE_E reason=%x status=%x\n",
                    data->Reason, data->Status);
                #endif
            }
            break;

            case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_REQUEST_E:
            {
                qapi_BLE_GAP_LE_Connection_Parameter_Update_Request_Event_Data_t *data =
                    GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Update_Request_Event_Data;

                if(!SendConnectionParamUpdateResponse(data))
                {
                    /*Not happy with connection parameters hence removing the connection*/
                    DEBUG_BEARER_INFO(mesh_group, "Removing the connection as not happy with connection parameters of remote device\n");

                    InitiateGattDisconnection(data->BD_ADDR);
                }
            }
            break;

            case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_RESPONSE_E:
            {
                #ifdef DEBUG_GAP_GATT_BEARER
                qapi_BLE_GAP_LE_Connection_Parameter_Update_Response_Event_Data_t *data =
                    GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Update_Response_Event_Data;

                DEBUG_BEARER_INFO(mesh_group, "Received QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATE_RESPONSE_E, accepted=%d \n", data->Accepted);
                #endif
            }
            break;

            case QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATED_E:
            {
                #ifdef DEBUG_GAP_GATT_BEARER
                qapi_BLE_GAP_LE_Connection_Parameter_Updated_Event_Data_t *data =
                    GAP_LE_Event_Data->Event_Data.GAP_LE_Connection_Parameter_Updated_Event_Data;

                DEBUG_BEARER_INFO(mesh_group, "Received QAPI_BLE_ET_LE_CONNECTION_PARAMETER_UPDATED_E,  Status=%d, ConnIntvl=%x, slaceLatency=%x, supervisionTimeout=%x\n",
                    data->Status, data->Current_Connection_Parameters.Connection_Interval, data->Current_Connection_Parameters.Slave_Latency,
                    data->Current_Connection_Parameters.Supervision_Timeout);
                #endif
            }
            break;

            default:
            {
                DEBUG_BEARER_INFO(mesh_group, " %s Event %x not handled\n", __FUNCTION__,GAP_LE_Event_Data->Event_Data_Type);
            }
            break;
        }
    }
}

