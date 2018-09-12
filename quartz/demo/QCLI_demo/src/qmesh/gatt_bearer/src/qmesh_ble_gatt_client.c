/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "qapi_ble_gatt.h"
#include "qmesh_types.h"
#include "qmesh_ble_gap.h"
#include "qmesh_hal_ifce.h"
#include "qmesh_gw_debug.h"
#include "qmesh_ble_gatt.h"
#include "qmesh_ble_gatt_client.h"
#include "qmesh_qca402x_sched.h"
#include "qmesh_bearer_debug.h"

#define HANDLE_VALUE_ONE    1
#define HANDLE_VALUE_TWO    2
#define NUMBER_OF_CHARACTERISTICS_SUPPORTED 2
#define NUMBER_OF_CHAR_DESC_SUPPORTED   1
#define ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(_x, _y)        \
{                                                                       \
  ((uint8_t *)(_x))[0] = ((uint8_t)(((uint16_t)(_y)) & 0xFF));          \
  ((uint8_t *)(_x))[1] = ((uint8_t)((((uint16_t)(_y)) >> 8) & 0xFF));   \
}

#define INVALID_CLIENT_DATA_HANDLE ((QMESH_GATT_CLIENT_DATA_T *) NULL)
#define EXCHANGE_MTU_OPCODE        (uint8_t)(0x02)
#define ERROR_CODE_REQUEST_NOT_SUPPORTED (uint8_t)(0x06)

extern Qmesh_Gatt_Data_T    *g_gattData;
extern QCLI_Group_Handle_t  mesh_group;

/* Mesh Provisioning service UUID */
uint8_t Mesh_Prov_Service_UUID[2] = MESH_PROVISIONING_SERVICE_UUID;

/* Mesh Provisioning Service characteristic UUIDS */
uint8_t Mesh_Prov_Char_Data_In_UUID[2] = MESH_PROVISIONING_DATA_IN_UUID;
uint8_t Mesh_Prov_Char_Data_Out_UUID[2] = MESH_PROVISIONING_DATA_OUT_UUID;

/* Mesh Proxy service UUID */
uint8_t Mesh_Proxy_Srvc_UUID[2] = MESH_PROXY_SERVICE_UUID;

/* Mesh Proxy Service characteristic UUIDS */
uint8_t Mesh_Proxy_Char_Data_In_UUID[2] = MESH_PROXY_DATA_IN_UUID;
uint8_t Mesh_Proxy_Char_Data_Out_UUID[2] = MESH_PROXY_DATA_OUT_UUID;

/*0x2902 UUID for CCCD*/
static const uint8_t Char_Desc_Ccc_UUID[2] = MESH_GATT_CHAR_DESC_UUID;

static QMESH_GATT_CLIENT_DATA_T *g_gattClientData[MAX_NUM_OF_GATT_CONN];
static QMESH_GATT_CALLBACK_INFO_T g_gattCallbackInfo;

static int _discoverServiceByUuid(QMESH_GATT_CLIENT_DATA_T *gattClientData,
                                  qapi_BLE_GATT_UUID_t *srvcUuid);
static int _discoverCharacteristics(QMESH_GATT_CLIENT_DATA_T *gattClientData,
                                    uint16_t startHndle,
                                    uint16_t endHndle);
static int _discoverCharacteristicDesc(QMESH_GATT_CLIENT_DATA_T *gattClientData,
                                       uint16_t startHndle,
                                       uint16_t endHndle);
static int _enableNtf(QMESH_GATT_CLIENT_DATA_T *gattClientData, uint16_t handle);
static bool _parseServicesOnUuid(qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t *data,
                                 QMESH_GATT_CLIENT_DATA_T *gattClientData);
static bool _parseCharacteristic(qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t *data,
                                 QMESH_GATT_CLIENT_DATA_T *gattClientData);
static bool _parseCharDesc(qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t *data,
                           QMESH_GATT_CLIENT_DATA_T *gattClientData);
static void _handleProvServerNtf(QMESH_GATT_CLIENT_DATA_T *gattClientData, uint16_t dataLength, uint8_t *data);
static void _handleProxyServerNtf(QMESH_GATT_CLIENT_DATA_T *gattClientData, uint16_t dataLength, uint8_t *data);
static QMESH_GATT_CLIENT_DATA_T* getGattClientDataForConnection(uint32_t connectionId);

void GattClientEventCallback(uint32_t btStackID,
            qapi_BLE_GATT_Client_Event_Data_t *GATT_Client_Event_Data,
            uint32_t callbackParameter);


static QMESH_GATT_CLIENT_DATA_T* getGattClientDataForConnection(uint32_t connectionId)
{
    uint8_t idx;

    for (idx = 0; idx < MAX_NUM_OF_GATT_CONN; idx++)
    {
        if ((INVALID_CLIENT_DATA_HANDLE != g_gattClientData[idx]) &&
            (g_gattClientData[idx]->connectionId == connectionId))
        {
            return g_gattClientData[idx];
        }
    }

    return INVALID_CLIENT_DATA_HANDLE;
}

void DestroyGattClientData(uint32_t connectionId)
{
    uint8_t idx;

    for (idx = 0; idx < MAX_NUM_OF_GATT_CONN; idx++)
    {
        if ((NULL != g_gattClientData[idx]) &&
            (g_gattClientData[idx]->connectionId == connectionId))
        {
            /* This connection needs to be destroyed */
            QmeshFree(g_gattClientData[idx]);
            g_gattClientData[idx] = NULL;
            break;
        }
    }
}

bool IsClientBearerInitAllowed()
{
   return ((NULL == g_gattCallbackInfo.cbFuncPtr[0]) || (NULL == g_gattCallbackInfo.cbFuncPtr[1]));
}

bool InitializeGattClientBearer(APP_GATT_Bearer_Callback funcPtr, QMESH_GATT_BEARER_SERVICE_TYPE_T serviceReq)
{
    bool retVal = TRUE;

    if (QMESH_GATT_BEARER_UNKOWN_SVC_E != serviceReq)
    {
        /* Storing the callback based on the service required */
        g_gattCallbackInfo.cbFuncPtr[serviceReq - 1] = funcPtr;
    }

    return retVal;
}

void DeInitializeGattClientBearer(void)
{
    uint8_t idx;

    for (idx=0; idx < MAX_NUM_OF_GATT_CONN; idx++)
    {
        if (NULL != g_gattClientData[idx])
        {
            /* Free allocated pointers */
            QmeshFree(g_gattClientData[idx]);
            g_gattClientData[idx] = NULL;
        }
    }
}

bool ExchangeMtu(qapi_BLE_GATT_Device_Connection_Data_t *data)
{
 	int resp;
    uint16_t tempMtu = (g_gattCallbackInfo.currentServiceReq == QMESH_GATT_BEARER_PROV_SVC_E) ? PROV_MAX_ATT_MTU_SIZE : PROX_MAX_ATT_MTU_SIZE;
    uint16_t maxMtuSupptd = 0;
    
    DEBUG_BEARER_INFO (mesh_group,"BEARER : InitializeGattClientBearer, tempMtu=0x%x\n", tempMtu);

    /* Attempt to update the MTU to the maximum supported.*/
    resp = qapi_BLE_GATT_Query_Maximum_Supported_MTU(GetBluetoothStackId(), &maxMtuSupptd);
    if(resp == 0)
    {
        DEBUG_BEARER_INFO(mesh_group, "Maixmum MTU supported is 0x%x\n", maxMtuSupptd);
        
        if(maxMtuSupptd > tempMtu)
        {
            maxMtuSupptd = tempMtu;
            DEBUG_BEARER_INFO(mesh_group, "Setting maixmum MTU supported to default value 0x%x\n", maxMtuSupptd);
        }
    }
    else
    {
        maxMtuSupptd = tempMtu;
        DEBUG_BEARER_ERROR(mesh_group,"Failed to get Maximum MTU supported info, Error %d, setting MTU to default value 0x%x\n", resp, maxMtuSupptd);
    }
    
    resp = qapi_BLE_GATT_Exchange_MTU_Request(GetBluetoothStackId(), 
                                               data->ConnectionID, maxMtuSupptd, 
                                               GattClientEventCallback, 0);
    if(resp <= 0)
    {
        DEBUG_BEARER_ERROR(mesh_group, "Failed to send exchange MTU request, Error= %d\n",resp);
        
        return FALSE;
    }

    return TRUE;
}

bool InitiateServiceDiscovery(qapi_BLE_GATT_Device_Connection_Data_t *data)
{
    int32_t retVal;
    qapi_BLE_GATT_UUID_t servcUuid;
    uint8_t idx;

    if(NULL != data)
    {
        for (idx = 0; idx < MAX_NUM_OF_GATT_CONN; idx++)
        {
            if (NULL == g_gattClientData[idx])
            {
                g_gattClientData[idx] = (QMESH_GATT_CLIENT_DATA_T *) malloc(sizeof(QMESH_GATT_CLIENT_DATA_T));

                if (NULL == g_gattClientData[idx])
                {
                    DEBUG_BEARER_ERROR(mesh_group,"InitiateServiceDiscovery: Failed allocating memory for the client data\n");
                    return FALSE;
                }

                /* Slot available for this connection */
                g_gattClientData[idx]->connectionId = data->ConnectionID;
                QmeshMemCpy(&g_gattClientData[idx]->devAddr, &data->RemoteDevice,sizeof(qapi_BLE_BD_ADDR_t));
                g_gattClientData[idx]->mtu = data->MTU;

                g_gattClientData[idx]->srvcReqd = g_gattCallbackInfo.currentServiceReq;

                QmeshMemSet(&servcUuid, 0, sizeof(qapi_BLE_GATT_UUID_t));
                servcUuid.UUID_Type = QAPI_BLE_GU_UUID_16_E;
                
                if(g_gattCallbackInfo.currentServiceReq == QMESH_GATT_BEARER_PROV_SVC_E)
                {
                    servcUuid.UUID.UUID_16.UUID_Byte0 = Mesh_Prov_Service_UUID[0];
                    servcUuid.UUID.UUID_16.UUID_Byte1 = Mesh_Prov_Service_UUID[1];
                }
                else if(g_gattCallbackInfo.currentServiceReq == QMESH_GATT_BEARER_PROXY_SVC_E)
                {
                    servcUuid.UUID.UUID_16.UUID_Byte0 = Mesh_Proxy_Srvc_UUID[0];
                    servcUuid.UUID.UUID_16.UUID_Byte1 = Mesh_Proxy_Srvc_UUID[1];
                }
                else
                {
                    DEBUG_BEARER_ERROR(mesh_group,"InitiateServiceDiscovery: Invalid service=%d\n", g_gattCallbackInfo.currentServiceReq);
                    return FALSE;
                }
                
                
                /*Check if the device supports the provisioning/proxy service*/
                retVal = _discoverServiceByUuid(g_gattClientData[idx], &servcUuid);

                if(retVal <= 0)
                {
                    DEBUG_BEARER_ERROR(mesh_group,"InitiateServiceDiscovery: Failed to discover services retVal=%d\n", retVal);
                    InitiateGattDisconnection(g_gattClientData[idx]->devAddr);
                    return FALSE;
                }

                g_gattClientData[idx]->curTransactionId = retVal;

                return TRUE;
            }
        }

        DEBUG_BEARER_ERROR(mesh_group,"InitiateServiceDiscovery: Max no of GATT connections allocated\n");
    }
    else
    {
        DEBUG_BEARER_ERROR(mesh_group,"InitiateServiceDiscovery: Invalid GATT connection data\n");
    }

    return FALSE;
}


bool HandleServerNtf(qapi_BLE_GATT_Server_Notification_Data_t *ntfData)
{
    QMESH_GATT_CLIENT_DATA_T *gattClientData = getGattClientDataForConnection(ntfData->ConnectionID);

    if((INVALID_CLIENT_DATA_HANDLE != gattClientData) &&
        (gattClientData->handles.dataOutCharValHandle == ntfData->AttributeHandle) &&
        (QmeshMemCmp(&gattClientData->devAddr, &ntfData->RemoteDevice, sizeof(qapi_BLE_BD_ADDR_t)) == 0))
    {
        if(gattClientData->srvcReqd == QMESH_GATT_BEARER_PROV_SVC_E)
        {
            DEBUG_BEARER_INFO(mesh_group, "BEARER : Notification received from server\n");
            _handleProvServerNtf(gattClientData, ntfData->AttributeValueLength, ntfData->AttributeValue);
        }
        else if(gattClientData->srvcReqd == QMESH_GATT_BEARER_PROXY_SVC_E)
        {
            DEBUG_BEARER_INFO(mesh_group, "BEARER : Proxy Notification received from server\n");
            _handleProxyServerNtf(gattClientData, ntfData->AttributeValueLength, ntfData->AttributeValue);
        }
        else
        {
            DEBUG_BEARER_ERROR(mesh_group,"HandleServerNtf Invalid service=%d\n", gattClientData->srvcReqd);
            return FALSE;
        }
    }
    else
    {
        DEBUG_BEARER_ERROR (mesh_group,  "Error - Received notification from an unknown device\n");
        return FALSE;
    }
    return TRUE;
}


void GattClientEventCallback(uint32_t btStackID, qapi_BLE_GATT_Client_Event_Data_t *GATT_Client_Event_Data, uint32_t callbackParameter)
{
    QMESH_GATT_CLIENT_DATA_T *gattClientData;

    if((GATT_Client_Event_Data) && (btStackID == GetBluetoothStackId()))
    {
        switch(GATT_Client_Event_Data->Event_Data_Type)
        {
            case QAPI_BLE_ET_GATT_CLIENT_SERVICE_DISCOVERY_BY_UUID_RESPONSE_E:
            {
                qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t *data =
                    GATT_Client_Event_Data->Event_Data.GATT_Service_Discovery_By_UUID_Response_Data;


                if ((NULL != data) && 
                    (NULL != (gattClientData = getGattClientDataForConnection(data->ConnectionID))))
                {
                    DEBUG_BEARER_INFO(mesh_group, "SERVICE_DISCOVERY_BY_UUID_RESPONSE_E srvcReqd=%x\n",
                        gattClientData->srvcReqd);

                    if (data->TransactionID == gattClientData->curTransactionId)
                    {
                        if(!_parseServicesOnUuid(data, gattClientData))
                        {
                            InitiateGattDisconnection(gattClientData->devAddr);
                        }
                    }
                    else
                    {
                        DEBUG_BEARER_ERROR(mesh_group, "Event data is NULL\n");
                        InitiateGattDisconnection(gattClientData->devAddr);
                    }
                }
                else
                {
                    DEBUG_BEARER_ERROR(mesh_group, "EventData and/or Client data NULL\n");
                }
            }
            break;

            case QAPI_BLE_ET_GATT_CLIENT_CHARACTERISTIC_DISCOVERY_RESPONSE_E:
            {
                qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t *data =
                    GATT_Client_Event_Data->Event_Data.GATT_Characteristic_Discovery_Response_Data;

                if ((NULL != data) && 
                    (NULL != (gattClientData = getGattClientDataForConnection(data->ConnectionID))))
                {
                    DEBUG_BEARER_INFO(mesh_group, "CHARACTERISTIC_DISCOVERY_RESPONSE_E srvcReqd=%x\n",
                        gattClientData->srvcReqd);

                    if (data->TransactionID == gattClientData->curTransactionId)
                    {
                        if(!_parseCharacteristic(data, gattClientData))
                        {
                            InitiateGattDisconnection(gattClientData->devAddr);
                        }
                    }
                    else
                    {
                        DEBUG_BEARER_ERROR(mesh_group, "Event data is NULL\n");
                        InitiateGattDisconnection(gattClientData->devAddr);
                    }
                }
                else
                {
                    DEBUG_BEARER_ERROR(mesh_group, "EventData and/or Client data NULL\n");
                }
            }
            break;

            case QAPI_BLE_ET_GATT_CLIENT_CHARACTERISTIC_DESCRIPTOR_DISCOVERY_RESPONSE_E:
            {
                qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t *data =
                    GATT_Client_Event_Data->Event_Data.GATT_Characteristic_Descriptor_Discovery_Response_Data;

                if ((NULL != data) && 
                    (NULL != (gattClientData = getGattClientDataForConnection(data->ConnectionID))))
                {
                    DEBUG_BEARER_INFO(mesh_group, "CHARACTERISTIC_DESCRIPTOR_DISCOVERY_RESPONSE_E srvcReqd=%x\n",
                        gattClientData->srvcReqd);

                    if (data->TransactionID == gattClientData->curTransactionId)
                    {
                        if(!_parseCharDesc(data, gattClientData))
                        {
                            InitiateGattDisconnection(gattClientData->devAddr);
                        }
                    }
                    else
                    {
                        DEBUG_BEARER_ERROR(mesh_group, "Event data is NULL\n");
                        InitiateGattDisconnection(gattClientData->devAddr);
                    }
                }
                else
                {
                    DEBUG_BEARER_ERROR(mesh_group, "EventData and/or Client data NULL\n");
                }
            }
            break;

            case QAPI_BLE_ET_GATT_CLIENT_WRITE_RESPONSE_E:
            {
                /*This event shall be recvd only for enabling the notification, hence we can trigger */
                qapi_BLE_GATT_Write_Response_Data_t *data =
                    GATT_Client_Event_Data->Event_Data.GATT_Write_Response_Data;

                if((NULL != data) &&
                   (NULL != (gattClientData = getGattClientDataForConnection(data->ConnectionID))))
                {
                    DEBUG_BEARER_INFO(mesh_group, "WRITE_RESPONSE_E srvcReqd=%x, bytesWritten=%d\n",
                        gattClientData->srvcReqd, data->BytesWritten);

                    if((data->BytesWritten > 0) &&
                       (QMESH_GATT_BEARER_UNKOWN_SVC_E != gattClientData->srvcReqd) &&
                       (g_gattCallbackInfo.cbFuncPtr[gattClientData->srvcReqd - 1]))
                    {
                        g_gattCallbackInfo.cbFuncPtr[gattClientData->srvcReqd - 1](gattClientData);
                    }
                    else
                    {
                        DEBUG_BEARER_ERROR(mesh_group,"Either function pointer is null or no data written\n");
                    }
                }
            }
            break;
            
			case QAPI_BLE_ET_GATT_CLIENT_EXCHANGE_MTU_RESPONSE_E:
            {
                qapi_BLE_GATT_Exchange_MTU_Response_Data_t *mtuUpd = 
                    GATT_Client_Event_Data->Event_Data.GATT_Exchange_MTU_Response_Data;
                
                DEBUG_BEARER_INFO (mesh_group, "BEARER : Received  QAPI_BLE_ET_GATT_CLIENT_EXCHANGE_MTU_RESPONSE_E Conn Id %x, MTU %x\n",
                               mtuUpd->ConnectionID, mtuUpd->ServerMTU);

				/*Send the MTU update event to application*/
                SendMtuUpdateEvent(mtuUpd->ConnectionID, mtuUpd->ConnectionType, &mtuUpd->RemoteDevice, mtuUpd->ServerMTU);
            }
            break;
            
            case QAPI_BLE_ET_GATT_CLIENT_ERROR_RESPONSE_E:
            {
                qapi_BLE_GATT_Request_Error_Data_t *errResp = 
                    GATT_Client_Event_Data->Event_Data.GATT_Request_Error_Data;

                DEBUG_BEARER_ERROR(mesh_group,"Received error response for opcode %x, error-type %x , error-code %x\n", 
                    errResp->RequestOpCode, errResp->ErrorType, errResp->ErrorCode);

                /*MTU exchange has resulted an error, using the default MTU received in connection event. Initiating service discovery*/
                if((errResp->RequestOpCode == EXCHANGE_MTU_OPCODE) && 
                    (errResp->ErrorCode == ERROR_CODE_REQUEST_NOT_SUPPORTED))
                {
                    DEBUG_BEARER_INFO (mesh_group, "BEARER : Since Exchange-MTU is not supported by remote device, initiating srvc discy\n");
                    
                    InitiateServiceDiscovery(GetNewGattConnData());
                }
            }
            break;
            
            default:
                DEBUG_BEARER_INFO (mesh_group,"GattClientEventCallback: Event %x not handled\n", GATT_Client_Event_Data->Event_Data_Type);
            break;
        }
    }
    else
    {
        DEBUG_BEARER_ERROR(mesh_group, "Invalid stackId or client-event-data not valid\n");
    }
}


static bool _parseServicesOnUuid(qapi_BLE_GATT_Service_Discovery_By_UUID_Response_Data_t *data, QMESH_GATT_CLIENT_DATA_T *gattClientData)
{
	DEBUG_BEARER_INFO (mesh_group," Number of services present is %d\n", data->NumberOfServices);
    if((gattClientData->connectionId == data->ConnectionID) &&
       (data->ConnectionType == QAPI_BLE_GCT_LE_E) &&
       (data->NumberOfServices == 1) &&
       (data->ServiceInformationList))
    {
        uint16_t indx=0;
        int32_t retVal;

        qapi_BLE_GATT_Service_Information_By_UUID_t *srvcInfo =
            &data->ServiceInformationList[indx];

        if((gattClientData->srvcReqd == QMESH_GATT_BEARER_PROV_SVC_E) ||
            (gattClientData->srvcReqd == QMESH_GATT_BEARER_PROXY_SVC_E))
        {
            DEBUG_BEARER_INFO (mesh_group,"Service found Start 0x%x, End 0x%x\n",
                                         srvcInfo->Service_Handle, srvcInfo->End_Group_Handle);
            gattClientData->handles.srvcHndleDetails.Starting_Handle =
                                                srvcInfo->Service_Handle;
            gattClientData->handles.srvcHndleDetails.Ending_Handle =
                                                srvcInfo->End_Group_Handle;
        }
        DEBUG_BEARER_INFO (mesh_group,"Discovering characteristics\n");

        retVal = _discoverCharacteristics(gattClientData,
                                          srvcInfo->Service_Handle,
                                          srvcInfo->End_Group_Handle);
        if(retVal > 0)
        {
            gattClientData->curTransactionId = retVal;
            DEBUG_BEARER_INFO (mesh_group,"Request to discover characteristics successfully initiated\n");
            return TRUE;
        }
        else
        {
            DEBUG_BEARER_ERROR(mesh_group, " discover characteristics failed with error %d\n", retVal);
            return FALSE;
        }
    }
    else
    {
        DEBUG_BEARER_ERROR(mesh_group, " Event data is not Valid\n");
        return FALSE;
    }
}


static bool _parseCharacteristic(qapi_BLE_GATT_Characteristic_Discovery_Response_Data_t *data, QMESH_GATT_CLIENT_DATA_T *gattClientData)
{
    bool dataInFnd = FALSE;
    bool dataOutCharPropExist = FALSE;

    DEBUG_BEARER_INFO (mesh_group,"Number of services present is %d\n", data->NumberOfCharacteristics);

    if((gattClientData->connectionId == data->ConnectionID) &&
       (data->ConnectionType == QAPI_BLE_GCT_LE_E) &&
       (data->NumberOfCharacteristics == NUMBER_OF_CHARACTERISTICS_SUPPORTED) &&
       (data->CharacteristicEntryList))
    {
        uint16_t indx=0;

        while(indx < data->NumberOfCharacteristics)
        {
            qapi_BLE_GATT_Characteristic_Entry_t *chrInfo =
                &data->CharacteristicEntryList[indx];

            qapi_BLE_GATT_UUID_t *charUuid =
                &chrInfo->CharacteristicValue.CharacteristicUUID;
            if (charUuid->UUID_Type == QAPI_BLE_GU_UUID_16_E)
            {
                if(gattClientData->srvcReqd == QMESH_GATT_BEARER_PROV_SVC_E)
                {
                    if(QmeshMemCmp(&charUuid->UUID.UUID_16, Mesh_Prov_Char_Data_In_UUID, sizeof(qapi_BLE_UUID_16_t)) == 0)
                    {
                        gattClientData->handles.dataInCharValHandle =
                            chrInfo->CharacteristicValue.CharacteristicValueHandle;
                        DEBUG_BEARER_INFO (mesh_group,"Provisioning Data In characteristic found, Handle 0x%x \n",
                                         gattClientData->handles.dataInCharValHandle);
                        dataInFnd = TRUE;
                    }
                    else if(QmeshMemCmp(&charUuid->UUID.UUID_16, Mesh_Prov_Char_Data_Out_UUID, sizeof(qapi_BLE_UUID_16_t)) == 0)
                    {
                        gattClientData->handles.dataOutCharValHandle =
                            chrInfo->CharacteristicValue.CharacteristicValueHandle;

                        DEBUG_BEARER_INFO (mesh_group,"Provisioning Data Out characteristic found, handle 0x%x\n",
                                         gattClientData->handles.dataOutCharValHandle);

                        if(chrInfo->CharacteristicValue.CharacteristicProperties)
                        {
                            dataOutCharPropExist = TRUE;
                        }
                        else
                        {
                            DEBUG_BEARER_ERROR(mesh_group,"No characteristic properties present\n");
                        }
                    }
                    else
                    {
                        DEBUG_BEARER_ERROR(mesh_group,"Unknown Provisioning charcteristic found\n");
                    }
                }
                else
                {
                    DEBUG_BEARER_INFO (mesh_group," Proxy characteristics\n");

                    /*Proxy code here*/
                    if(QmeshMemCmp(&charUuid->UUID.UUID_16, Mesh_Proxy_Char_Data_In_UUID, sizeof(qapi_BLE_UUID_16_t)) == 0)
                    {
                        DEBUG_BEARER_INFO (mesh_group,"Proxy Data In characteristic handle 0x%x found\n", chrInfo->CharacteristicValue.CharacteristicValueHandle);
                        gattClientData->handles.dataInCharValHandle =
                            chrInfo->CharacteristicValue.CharacteristicValueHandle;
                        dataInFnd = TRUE;
                    }
                    else if(QmeshMemCmp(&charUuid->UUID.UUID_16, Mesh_Proxy_Char_Data_Out_UUID, sizeof(qapi_BLE_UUID_16_t)) == 0)
                    {
                        DEBUG_BEARER_INFO (mesh_group,"Proxy Data Out characteristic handle 0x%x found\n",chrInfo->CharacteristicValue.CharacteristicValueHandle);
                        gattClientData->handles.dataOutCharValHandle =
                            chrInfo->CharacteristicValue.CharacteristicValueHandle;
                        if(chrInfo->CharacteristicValue.CharacteristicProperties)
                        {
                            dataOutCharPropExist = TRUE;
                        }
                        else
                        {
                            DEBUG_BEARER_ERROR(mesh_group,"No characteristic properties present\n");
                        }
                    }
                    else
                    {
                        DEBUG_BEARER_ERROR(mesh_group,"Unknown Proxy charcteristic found\n");
                    }
                }
            }
            else if (charUuid->UUID_Type == QAPI_BLE_GU_UUID_32_E)
            {
                DEBUG_BEARER_INFO (mesh_group,"Need to verify characteristics UUID in 32 bit format\n");
                return FALSE;
            }
            else if (charUuid->UUID_Type == QAPI_BLE_GU_UUID_128_E)
            {
                DEBUG_BEARER_INFO (mesh_group,"Need to verify characteristics UUID in 128 bit format\n");
                return FALSE;
            }
            else
            {
                DEBUG_BEARER_ERROR(mesh_group, "Invalid UUID-type %d\n", charUuid->UUID_Type);
                return FALSE;
            }

            if(dataInFnd && dataOutCharPropExist)
                break;

            indx++;
        }

        /*If service found then discover its characteristics*/
        if(dataInFnd && dataOutCharPropExist)
        {
            int32_t retVal = 0;

            DEBUG_BEARER_INFO (mesh_group,"Discovering characteristic descriptors\n");

            /*The characteristics might be placed in any order hence checking for handle values before
                        choosing the handle range for querying the characteristics descriptors*/
            if(gattClientData->handles.dataOutCharValHandle > gattClientData->handles.dataInCharValHandle)
            {
                retVal = _discoverCharacteristicDesc(gattClientData,
                                                     gattClientData->handles.dataOutCharValHandle,
                                                     gattClientData->handles.srvcHndleDetails.Ending_Handle);
            }
            else
            {
                retVal = _discoverCharacteristicDesc(gattClientData,
                                                     (gattClientData->handles.dataOutCharValHandle + HANDLE_VALUE_ONE),
                                                     (gattClientData->handles.dataInCharValHandle - HANDLE_VALUE_TWO));
            }

            if(retVal > 0)
            {
                gattClientData->curTransactionId = retVal;
                DEBUG_BEARER_INFO (mesh_group,"Request to discover char desc successfully sent\n");
                return TRUE;
            }
            else
            {
                DEBUG_BEARER_ERROR(mesh_group, " discover characteristics descriptor failed with error %d\n", retVal);
                return FALSE;
            }
        }
        else
        {
            DEBUG_BEARER_ERROR(mesh_group, "Provisioning/Proxy service not found\n");
            return FALSE;
        }
    }
    else
    {
        DEBUG_BEARER_ERROR(mesh_group, "Event data is not Valid\n");
        return FALSE;
    }
}


static bool _parseCharDesc(qapi_BLE_GATT_Characteristic_Descriptor_Discovery_Response_Data_t *data, QMESH_GATT_CLIENT_DATA_T *gattClientData)
{
    bool retVal = FALSE;
    uint8_t indx = 0;
    if((data->ConnectionID == gattClientData->connectionId) &&
       (data->ConnectionType == QAPI_BLE_GCT_LE_E))
    {
        qapi_BLE_GATT_Characteristic_Descriptor_Entry_t *desc =
                data->CharacteristicDescriptorEntryList;

        DEBUG_BEARER_INFO (mesh_group,"Number of Characteristics Desc %d\n",
                         data->NumberOfCharacteristicDescriptors);
        for (indx = 0; indx < data->NumberOfCharacteristicDescriptors; indx++)
        {
            if(desc->UUID.UUID_Type == QAPI_BLE_GU_UUID_16_E)
            {
                if(QmeshMemCmp(Char_Desc_Ccc_UUID, &desc->UUID.UUID.UUID_16, sizeof(Char_Desc_Ccc_UUID)) == 0)
                {
                    int32_t result;

                    if((gattClientData->srvcReqd == QMESH_GATT_BEARER_PROV_SVC_E) ||
                        (gattClientData->srvcReqd == QMESH_GATT_BEARER_PROXY_SVC_E))
                    {
                        gattClientData->handles.dataOutNtfHandle = desc->Handle;

                        DEBUG_BEARER_INFO (mesh_group,"Enabling notification, Handle 0x%x\n",
                                         gattClientData->handles.dataOutNtfHandle);

                        result = _enableNtf(gattClientData, gattClientData->handles.dataOutNtfHandle);

                        if(result > 0)
                        {
                            DEBUG_BEARER_INFO (mesh_group,"Enabling notification request successfully sent\n");
                            retVal = TRUE;
                            break;
                        }
                        else
                        {
                            DEBUG_BEARER_ERROR(mesh_group, " discover characteristics descriptor failed with error %d\n", retVal);
                        }
                    }
                    else
                    {
                        DEBUG_BEARER_ERROR(mesh_group,"Invalid Service type\n");
                    }
                }
                else
                {
                    DEBUG_BEARER_ERROR(mesh_group, "Characteristic-descriptor UUID doesn't match\n");
                }
            }
            else if (desc->UUID.UUID_Type == QAPI_BLE_GU_UUID_32_E)
            {
                DEBUG_BEARER_ERROR(mesh_group,"Need to verify characteristics descriptors UUID in 32 bit format\n");
            }
            else if (desc->UUID.UUID_Type == QAPI_BLE_GU_UUID_128_E)
            {
                DEBUG_BEARER_ERROR(mesh_group,"Need to verify characteristics descriptors UUID in 128 bit format\n");
            }
            else
            {
                DEBUG_BEARER_ERROR(mesh_group, "Invalid UUID-type %d\n", desc->UUID.UUID_Type);
            }
            /* Next Dexcriptor */
            desc++;
        }
    }
    else
    {
        DEBUG_BEARER_ERROR(mesh_group, "Event data is not Valid\n");
    }
    return retVal;
}


static int _discoverServiceByUuid(QMESH_GATT_CLIENT_DATA_T *gattClientData, qapi_BLE_GATT_UUID_t *srvcUuid)
{
    return qapi_BLE_GATT_Discover_Services_By_UUID(GetBluetoothStackId(),
                                            gattClientData->connectionId,
                                            QAPI_BLE_ATT_PROTOCOL_HANDLE_MINIMUM_VALUE,
                                            QAPI_BLE_ATT_PROTOCOL_HANDLE_MAXIMUM_VALUE,
                                            srvcUuid,
                                            GattClientEventCallback,
                                            0);
}


static int _discoverCharacteristics(QMESH_GATT_CLIENT_DATA_T *gattClientData, uint16_t startHndle, uint16_t endHndle)
{
    return qapi_BLE_GATT_Discover_Characteristics(GetBluetoothStackId(),
                                                  gattClientData->connectionId,
                                                  gattClientData->handles.srvcHndleDetails.Starting_Handle,
                                                  gattClientData->handles.srvcHndleDetails.Ending_Handle,
                                                  GattClientEventCallback,
                                                  0);
}


static int _discoverCharacteristicDesc(QMESH_GATT_CLIENT_DATA_T *gattClientData, uint16_t startHndle, uint16_t endHndle)
{

    return qapi_BLE_GATT_Discover_Characteristic_Descriptors(GetBluetoothStackId(),
                                                             gattClientData->connectionId,
                                                             startHndle,
                                                             endHndle,
                                                             GattClientEventCallback,
                                                             0);
}


static int _enableNtf(QMESH_GATT_CLIENT_DATA_T *gattClientData, uint16_t handle)
{
    qapi_BLE_NonAlignedWord_t Buffer;

    /* Verify the input parameters.                                      */
    ASSIGN_HOST_WORD_TO_LITTLE_ENDIAN_UNALIGNED_WORD(&Buffer, QAPI_BLE_GATT_CLIENT_CONFIGURATION_CHARACTERISTIC_NOTIFY_ENABLE);

    return qapi_BLE_GATT_Write_Request(GetBluetoothStackId(),
                                        gattClientData->connectionId,
                                        handle,
                                        sizeof(Buffer),
                                        &Buffer,
                                        GattClientEventCallback,
                                        0);
}

bool SendMeshGattClientData(void *meshDataPtr)
{
    if(!meshDataPtr)
        return FALSE;

    bool retVal = FALSE;
    QMESH_MESH_MSG_DATA_T *dataPtr = (QMESH_MESH_MSG_DATA_T *) meshDataPtr;

    if((dataPtr->length > 0) && (dataPtr->msg))
    {
        int result = 0;
        QMESH_GATT_CLIENT_DATA_T *gattClientData = getGattClientDataForConnection(dataPtr->connectionId);

        DEBUG_BEARER_INFO(mesh_group, " dataLength=%d\n", dataPtr->length);

        if (INVALID_CLIENT_DATA_HANDLE != gattClientData)
        {
            result = qapi_BLE_GATT_Write_Without_Response_Request(GetBluetoothStackId(),
                                                                  dataPtr->connectionId,
                                                                  gattClientData->handles.dataInCharValHandle,
                                                                  dataPtr->length,
                                                                  dataPtr->msg);
        }

        if(result > 0)
        {
            retVal = TRUE;
        }
        else
        {
            DEBUG_BEARER_ERROR(mesh_group, "Failed to do write command retVal=%x\n", retVal);
        }
    }
    else
    {
        DEBUG_BEARER_ERROR(mesh_group, "SendMeshGattClientData: Invalid Parameters\n");
    }
    return retVal;
}


static void _handleProvServerNtf(QMESH_GATT_CLIENT_DATA_T *gattClientData, uint16_t dataLength, uint8_t *data)
{
    QMESH_GATT_BEARER_DATA_T *gbd;

    if((data)  && (dataLength > 0) && (gattClientData->bearer_handle != QMESH_BEARER_HANDLE_INVALID))
    {
        DEBUG_BEARER_INFO(mesh_group, "_handleProvServerNtf\n");

        gbd = (QMESH_GATT_BEARER_DATA_T *) malloc (sizeof(QMESH_GATT_BEARER_DATA_T) + dataLength);

        if(gbd == NULL)
        {
            DEBUG_BEARER_ERROR(mesh_group, "BEARER : Error -Failed to allocate memory\n");
            return;
        }

        QmeshMemSet(gbd, 0, sizeof(QMESH_GATT_BEARER_DATA_T) + dataLength);

        gbd->srvcType = QMESH_GATT_BEARER_PROV_SVC_E;
        gbd->handle   = gattClientData->bearer_handle;
        gbd->length = dataLength;
        gbd->data = (uint8_t *)gbd + sizeof(QMESH_GATT_BEARER_DATA_T);
        QmeshMemCpy(gbd->data, data, dataLength);

        QmeshQueueGattRxData(gbd);
    }
    else
    {
        DEBUG_BEARER_ERROR(mesh_group, "BEARER : Error -Parameters not valid\n");
    }
}


static void _handleProxyServerNtf(QMESH_GATT_CLIENT_DATA_T *gattClientData, uint16_t dataLength, uint8_t *data)
{
    QMESH_GATT_BEARER_DATA_T *gbd;

    if((data) && (dataLength > 0) && (gattClientData->bearer_handle != QMESH_BEARER_HANDLE_INVALID))
    {
        DEBUG_BEARER_INFO(mesh_group, "_handleProxyServerNtf\n");

        gbd = (QMESH_GATT_BEARER_DATA_T *) malloc (sizeof(QMESH_GATT_BEARER_DATA_T) + dataLength);

        if(gbd == NULL)
        {
            DEBUG_BEARER_ERROR(mesh_group, "BEARER : Error -Failed to allocate memory\n");
            return;
        }

        QmeshMemSet(gbd, 0, sizeof(QMESH_GATT_BEARER_DATA_T) + dataLength);

        gbd->srvcType = QMESH_GATT_BEARER_PROXY_SVC_E;
        gbd->handle   = gattClientData->bearer_handle;
        gbd->length = dataLength;
        gbd->data = (uint8_t *)gbd + sizeof(QMESH_GATT_BEARER_DATA_T);
        QmeshMemCpy(gbd->data, data, dataLength);

        QmeshQueueGattRxData(gbd);
    }
    else
    {
        DEBUG_BEARER_ERROR(mesh_group, "BEARER : Error -Parameters not valid\n");
    }
}


void SetGATTClientServiceType(QMESH_GATT_BEARER_SERVICE_TYPE_T srvcReqd)
{
    DEBUG_BEARER_INFO(mesh_group, "Service requested to %x\n", srvcReqd);
    g_gattCallbackInfo.currentServiceReq = srvcReqd;
}

QMESH_GATT_BEARER_SERVICE_TYPE_T GetGATTClientServiceType(uint32_t connectionId)
{
    QMESH_GATT_CLIENT_DATA_T *gattClientData = getGattClientDataForConnection(connectionId);

    if (INVALID_CLIENT_DATA_HANDLE != gattClientData)
    {
        return gattClientData->srvcReqd;
    }

    return QMESH_GATT_BEARER_UNKOWN_SVC_E;
}

