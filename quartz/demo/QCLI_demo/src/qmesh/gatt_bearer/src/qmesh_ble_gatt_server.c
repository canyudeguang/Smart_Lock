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
#include "qmesh_ble_gatt.h"
#include "qmesh_ble_gatt_server.h"
#include "qmesh_hal_ifce.h"
#include "qmesh_gw_debug.h"
#include "qmesh_qca402x_sched.h"
#include "qmesh_bearer_debug.h"

typedef struct
{
    qapi_BLE_GATT_Attribute_Handle_Group_t  hndleDetails;   /*Holds registered service handle details*/
    QMESH_GATT_BEARER_SERVICE_TYPE_T        srvcRegd;       /*Holds registered service information*/
    int32_t                                 srvcId;         /*Holds Proxy/Provisioning service ID*/
    QMESH_GATT_DEVICE_INFO_T                devInfo[MAX_NUM_OF_GATT_CONN];     /*Hold remote device information*/
} QMESH_GATT_SERVER_DATA_T;

/* Mesh Provisioning Service Declaration                                */
static const qapi_BLE_GATT_Primary_Service_16_Entry_t Mesh_Provisioning_Service_UUID =
{
   MESH_PROVISIONING_SERVICE_UUID
} ;

/* Mesh Provisioning Data In Characteristic Declaration.                                */
static const qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t Mesh_Prov_Data_In =
{
    QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_WRITE_WITHOUT_RESPONSE,
    MESH_PROVISIONING_DATA_IN_UUID,
} ;


/* Mesh Provisioning Data In Characteristic Value.                                */
static const qapi_BLE_GATT_Characteristic_Value_16_Entry_t  Mesh_Prov_Data_In_Value =
{
   MESH_PROVISIONING_DATA_IN_UUID,
   0,
   NULL
} ;

/* Mesh Provisioning Data Out Characteristic Declaration.                        */
static const qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t Mesh_Prov_Data_Out =
{
   QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_NOTIFY,
   MESH_PROVISIONING_DATA_OUT_UUID
} ;

/* Mesh Provisioning Data Out Characteristic Value.                              */
static const qapi_BLE_GATT_Characteristic_Value_16_Entry_t Mesh_Prov_Data_Out_Value =
{
   MESH_PROVISIONING_DATA_OUT_UUID,
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

/*          Mesh Provisioning Server Database.                                   */
const qapi_BLE_GATT_Service_Attribute_Entry_t Mesh_Gatt_Prov_Service[] =
{
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE,          QAPI_BLE_AET_PRIMARY_SERVICE_16_E,            (uint8_t *)&Mesh_Provisioning_Service_UUID                  },
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE,          QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_16_E, (uint8_t *)&Mesh_Prov_Data_In                },
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, QAPI_BLE_AET_CHARACTERISTIC_VALUE_16_E,       (uint8_t *)&Mesh_Prov_Data_In_Value                      },
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE,          QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_16_E, (uint8_t *)&Mesh_Prov_Data_Out                },
   { 0,                                               QAPI_BLE_AET_CHARACTERISTIC_VALUE_16_E,       (uint8_t *)&Mesh_Prov_Data_Out_Value                      },
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, QAPI_BLE_AET_CHARACTERISTIC_DESCRIPTOR_16_E,   (uint8_t *)&Client_Characteristic_Configuration }
};

#define MESH_GATT_PROV_SERVICE_ATTR_COUNT               (sizeof(Mesh_Gatt_Prov_Service)/sizeof(qapi_BLE_GATT_Service_Attribute_Entry_t))

#define DATA_IN_CHARACTERISTIC_ATTRIBUTE_OFFSET          2
#define DATA_OUT_CHARACTERISTIC_ATTRIBUTE_OFFSET         (DATA_IN_CHARACTERISTIC_ATTRIBUTE_OFFSET + 2)
#define DATA_OUT_CHARACTERISTIC_CCD_ATTRIBUTE_OFFSET     (DATA_OUT_CHARACTERISTIC_ATTRIBUTE_OFFSET + 1)

#define DATA_OUT_CHARACTERISTIC_CCD_WRITE_VALUE_LENGTH   2

   /* Mesh Proxy Service Declaration                                */
static const qapi_BLE_GATT_Primary_Service_16_Entry_t Mesh_Proxy_Service_UUID =
{
   MESH_PROXY_SERVICE_UUID
} ;

   /* Mesh Proxy Data In Characteristic Declaration.                                */
static const qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t Mesh_Proxy_Data_In =
{
    (QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_READ|QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_WRITE_WITHOUT_RESPONSE),
    MESH_PROXY_DATA_IN_UUID,
} ;


   /* Mesh Proxy Data In Characteristic Value.                                */
static const qapi_BLE_GATT_Characteristic_Value_16_Entry_t  Mesh_Proxy_Data_In_Value =
{
   MESH_PROXY_DATA_IN_UUID,
   0,
   NULL
} ;

   /* Mesh Proxy Data Out Characteristic Declaration.                        */
static const qapi_BLE_GATT_Characteristic_Declaration_16_Entry_t Mesh_Proxy_Data_Out =
{
   (QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_READ|QAPI_BLE_GATT_CHARACTERISTIC_PROPERTIES_NOTIFY),
   MESH_PROXY_DATA_OUT_UUID
} ;

   /* Mesh Proxy Data Out Characteristic Value.                              */
static const qapi_BLE_GATT_Characteristic_Value_16_Entry_t Mesh_Proxy_Data_Out_Value =
{
   MESH_PROXY_DATA_OUT_UUID,
   0,
   NULL
} ;

   /* The following defines the Mesh Proxy Service that is registered with   */
   /* the GATT_Register_Service function call.                          */
   /* * NOTE * This array will be registered with GATT in the call to   */
   /*          GATT_Register_Service.                                   */
const qapi_BLE_GATT_Service_Attribute_Entry_t Mesh_Gatt_Proxy_Service[] =
{
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE,          QAPI_BLE_AET_PRIMARY_SERVICE_16_E,            (uint8_t *)&Mesh_Proxy_Service_UUID             },
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE,          QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_16_E, (uint8_t *)&Mesh_Proxy_Data_In                  },
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, QAPI_BLE_AET_CHARACTERISTIC_VALUE_16_E,       (uint8_t *)&Mesh_Proxy_Data_In_Value            },
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE,          QAPI_BLE_AET_CHARACTERISTIC_DECLARATION_16_E, (uint8_t *)&Mesh_Proxy_Data_Out                 },
   { 0,                                               QAPI_BLE_AET_CHARACTERISTIC_VALUE_16_E,       (uint8_t *)&Mesh_Proxy_Data_Out_Value           },
   { QAPI_BLE_GATT_ATTRIBUTE_FLAGS_READABLE_WRITABLE, QAPI_BLE_AET_CHARACTERISTIC_DESCRIPTOR_16_E,  (uint8_t *)&Client_Characteristic_Configuration }
} ;

#define MESH_GATT_PROXY_SERVICE_ATTR_COUNT               (sizeof(Mesh_Gatt_Proxy_Service)/sizeof(qapi_BLE_GATT_Service_Attribute_Entry_t))

#define MESH_PROXY_DATA_IN_CHARACTERISTIC_ATTRIBUTE_OFFSET            2
#define MESH_PROXY_DATA_OUT_CHARACTERISTIC_ATTRIBUTE_OFFSET           4
#define MESH_PROXY_DATA_OUT_CHARACTERISTIC_CCD_ATTRIBUTE_OFFSET       5

extern Qmesh_Gatt_Data_T *g_gattData;
extern QCLI_Group_Handle_t mesh_group;
QMESH_GATT_SERVER_DATA_T *g_gattServerData;


static bool _registerProvisioningService(void);
static bool _registerProxyService(void);
static void _sendDataToMeshStack(QMESH_GATT_BEARER_SERVICE_TYPE_T srvcType,
                                    QMESH_GATT_DEVICE_INFO_T *devInfo,
                                            uint16_t length,uint8_t *msg);
static void QAPI_BLE_BTPSAPI GattServerEventCallback(uint32_t btStackID,
                          qapi_BLE_GATT_Server_Event_Data_t *GattServerEventData,
                          uint32_t CallbackParameter);


static QMESH_GATT_DEVICE_INFO_T* getDeviceInfo(uint32_t connectionId)
{
    QMESH_GATT_DEVICE_INFO_T* devInfo = NULL;
    uint8_t idx;

    for (idx = 0; idx < MAX_NUM_OF_GATT_CONN; idx++)
    {
        if (g_gattServerData->devInfo[idx].connectionId == connectionId)
        {
            DEBUG_BEARER_INFO(mesh_group, "getDeviceInfo : At idx %x, connId %x\n",idx, connectionId);
            devInfo = (QMESH_GATT_DEVICE_INFO_T*)&g_gattServerData->devInfo[idx];
            break;
        }
    }

    return devInfo;
}


static bool _registerProvisioningService(void)
{
    bool retVal = FALSE;
    int32_t result;

    if (NULL == g_gattServerData)
    {
        DEBUG_BEARER_ERROR(mesh_group, "ERROR: %s :: GATT BEARER NOT INITIALIZED\n", __FUNCTION__);
        return retVal;
    }

    if ((g_gattServerData->srvcRegd == QMESH_GATT_BEARER_PROV_SVC_E) &&
        (g_gattServerData->srvcId != 0))
    {
        DEBUG_BEARER_INFO (mesh_group, "Provisioning service already registered\n");
        retVal = TRUE;
    }
    else
    {
        g_gattServerData->hndleDetails.Starting_Handle = 0;
        g_gattServerData->hndleDetails.Ending_Handle = 0;

        result = qapi_BLE_GATT_Register_Service(GetBluetoothStackId(),
                            QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE,
                            MESH_GATT_PROV_SERVICE_ATTR_COUNT,
                            (qapi_BLE_GATT_Service_Attribute_Entry_t *)Mesh_Gatt_Prov_Service,
                            &g_gattServerData->hndleDetails,
                            GattServerEventCallback, 0);
        if(result > 0)
        {
            /* Display success message.                                 */
            DEBUG_BEARER_INFO (mesh_group, "BEARER : Successfully registered Provisioning Service, ServiceID = %d, Start Handle %x  End Handle %x\n", result,
                                                                        g_gattServerData->hndleDetails.Starting_Handle, g_gattServerData->hndleDetails.Ending_Handle);
            g_gattServerData->srvcId = result;
            /* Save the ServiceID of the registered service.            */
            g_gattServerData->srvcRegd = QMESH_GATT_BEARER_PROV_SVC_E;

            /* Return success to the caller.                            */
            retVal = TRUE;
        }
        else
        {
            DEBUG_BEARER_ERROR (mesh_group, "Error - qapi_BLE_GATT_Register_Service() returned %d.\n", result);
        }
    }

    return retVal;
}


static bool _registerProxyService(void)
{
    bool retVal = FALSE;
    int32_t result;

    if ((g_gattServerData->srvcRegd == QMESH_GATT_BEARER_PROXY_SVC_E) &&
       (g_gattServerData->srvcId != 0))
    {
        retVal = TRUE;
    }
    else
    {
        g_gattServerData->hndleDetails.Starting_Handle = 0;
        g_gattServerData->hndleDetails.Ending_Handle = 0;

        result = qapi_BLE_GATT_Register_Service(GetBluetoothStackId(),
                            QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE,
                            MESH_GATT_PROXY_SERVICE_ATTR_COUNT,
                            (qapi_BLE_GATT_Service_Attribute_Entry_t *)Mesh_Gatt_Proxy_Service,
                            &g_gattServerData->hndleDetails,
                            GattServerEventCallback, 0);
        if(result > 0)
        {
            /* Display success message.                                 */
            DEBUG_BEARER_INFO (mesh_group, "BEARER : Successfully registered Proxy Service, ServiceID = %u, Start Handle %x  End Handle %x\n", result,
                                                g_gattServerData->hndleDetails.Starting_Handle, g_gattServerData->hndleDetails.Ending_Handle);
            g_gattServerData->srvcId = (uint32_t)result;
            /* Save the ServiceID of the registered service.            */
            g_gattServerData->srvcRegd = QMESH_GATT_BEARER_PROXY_SVC_E;

            /* Return success to the caller.                            */
            retVal = TRUE;
        }
        else
        {
            DEBUG_BEARER_ERROR (mesh_group, "BEARER : Error - qapi_BLE_GATT_Register_Service() returned %d.\n", result);
        }
    }

    return retVal;
}


static void _sendDataToMeshStack(QMESH_GATT_BEARER_SERVICE_TYPE_T srvcType, 
                                    QMESH_GATT_DEVICE_INFO_T *devInfo,
                                    uint16_t length,uint8_t *msg)
{
    QMESH_GATT_BEARER_DATA_T *gbd;

    if((msg) && (length > 0))
    {
        uint8_t indx=0;

        if (devInfo->bearerHandle == QMESH_BEARER_HANDLE_INVALID)
        {
            DEBUG_BEARER_ERROR (mesh_group, "BEARER : Error -Bearer Handle is NULL\n");
            return;
        }

        gbd = (QMESH_GATT_BEARER_DATA_T *) QmeshMemCalloc (sizeof(QMESH_GATT_BEARER_DATA_T) + length);
        if(gbd == NULL)
        {
            DEBUG_BEARER_ERROR (mesh_group, "BEARER : Error -Failed to allocate memory\n");
            return;
        }
        QmeshMemSet(gbd, 0, sizeof(QMESH_GATT_BEARER_DATA_T) + length);

        gbd->srvcType = srvcType;
        gbd->handle = devInfo->bearerHandle;
        gbd->length = length;
        gbd->data = (uint8_t *)gbd + sizeof(QMESH_GATT_BEARER_DATA_T);
        QmeshMemCpy(gbd->data, msg, length);

        DEBUG_BEARER_INFO (mesh_group, "GATT data recvd is \n");
        for(; indx < length; indx++)
                DEBUG_BEARER_INFO (mesh_group, " 0x%x", msg[indx]);
        DEBUG_BEARER_INFO (mesh_group, "\n");

        QmeshQueueGattRxData(gbd);
    }
    else
    {
        DEBUG_BEARER_ERROR (mesh_group, "BEARER : Error -Parameters not valid\n");
    }
}


bool InitializeGattServerBearer(void)
{
    DEBUG_BEARER_INFO (mesh_group,"BEARER : InitializeGattServerBearer\n");
    g_gattServerData = (QMESH_GATT_SERVER_DATA_T *) QmeshMemCalloc (sizeof(QMESH_GATT_SERVER_DATA_T));
    if(!g_gattServerData )
    {
        DEBUG_BEARER_ERROR(mesh_group, "BEARER : Failed to allocate memory for GATT-Server\n");
        return FALSE;
    }
    QmeshMemSet(g_gattServerData, 0, sizeof(QMESH_GATT_SERVER_DATA_T));
    return TRUE;
}


void DeInitializeGattServerBearer(void)
{
    if(g_gattServerData)
    {
        QmeshFree(g_gattServerData);
        g_gattServerData = NULL;
    }
}

bool RegisterGattBearerService(QMESH_GATT_BEARER_SERVICE_TYPE_T serviceType)
{
    bool result=FALSE;

    DEBUG_BEARER_INFO (mesh_group,"BEARER : RegisterService %d\n", serviceType);
    if((serviceType != QMESH_GATT_BEARER_PROXY_SVC_E) &&
        (serviceType != QMESH_GATT_BEARER_PROV_SVC_E))
    {
        DEBUG_BEARER_ERROR (mesh_group, "BEARER : RegisterService - Invalid service type value\n");
    }
    else
    {
        if(serviceType == QMESH_GATT_BEARER_PROV_SVC_E)
        {
            result=_registerProvisioningService();
        }
        else
        {
            result= _registerProxyService();
        }
    }

    return result;
}

bool UnregisterGattBearerService(QMESH_GATT_BEARER_SERVICE_TYPE_T serviceType)
{
    bool retVal = FALSE;
    if(g_gattServerData->srvcRegd != serviceType)
    {
        DEBUG_BEARER_INFO (mesh_group,"Error -Trying to unregister a non-registered service %d.\n", serviceType);
    }
    else
    {
        if((g_gattServerData->srvcRegd == QMESH_GATT_BEARER_PROV_SVC_E) || (g_gattServerData->srvcRegd == QMESH_GATT_BEARER_PROXY_SVC_E))
        {
            qapi_BLE_GATT_Un_Register_Service(GetBluetoothStackId(), g_gattServerData->srvcId);
            g_gattServerData->srvcRegd = QMESH_GATT_BEARER_UNKOWN_SVC_E;
            g_gattServerData->srvcId = 0;
            QmeshMemSet(g_gattServerData->devInfo, 0, sizeof(QMESH_GATT_DEVICE_INFO_T) * MAX_NUM_OF_GATT_CONN);
            retVal = TRUE;
        }
        else
        {
            DEBUG_BEARER_ERROR (mesh_group, "GATT-beare holds invalid service-registered data\n");
        }
    }
    return retVal;
}

bool SendMeshProvDataOutNtf(void *meshDataPtr)
{
    if(!meshDataPtr)
        return FALSE;

    bool retVal = FALSE;
    QMESH_MESH_MSG_DATA_T *dataPtr = (QMESH_MESH_MSG_DATA_T *) meshDataPtr;

    DEBUG_BEARER_INFO(mesh_group, "SendMeshProvDataOutNtf\n");

    if((dataPtr->length > 0) && (dataPtr->msg))
    {
        if((g_gattServerData->srvcRegd == QMESH_GATT_BEARER_PROV_SVC_E) && (g_gattServerData->srvcId > 0))
        {
            QMESH_GATT_DEVICE_INFO_T *devInfo = getDeviceInfo(dataPtr->connectionId);
            if (devInfo == NULL)
            {
                DEBUG_BEARER_INFO (mesh_group, "SendMeshProvDataOutNtf : Dev Info NULL\n");
                return FALSE;
            }
            /* Check for Notification Enabled */
            if (devInfo->notifyEnabled)
            {
                int32_t result= -1;
                DEBUG_BEARER_INFO(mesh_group, "BEARER : SrvcId %x, ConnId %x, PayloadLen %x, AttrOffset %x\n", g_gattServerData->srvcId,
                                                        dataPtr->connectionId, dataPtr->length, DATA_OUT_CHARACTERISTIC_ATTRIBUTE_OFFSET);

                result = qapi_BLE_GATT_Handle_Value_Notification(GetBluetoothStackId(),
                                                g_gattServerData->srvcId,
                                                dataPtr->connectionId,
                                                DATA_OUT_CHARACTERISTIC_ATTRIBUTE_OFFSET,
                                                dataPtr->length, dataPtr->msg);
                if(result <= 0)
                {
                    DEBUG_BEARER_ERROR (mesh_group, "Error -Failed to send the notification data %d\n",result);
                }
                else
                {
                    retVal = TRUE;
                }
            }
        }
        else
        {
            DEBUG_BEARER_ERROR (mesh_group, "Error -Provisioning service is not registered or service-ID is not valid\n");
        }
    }
    else
    {
        DEBUG_BEARER_ERROR (mesh_group, "Error -Parameters not valid\n");
    }

    return retVal;
}

bool SendMeshProxyDataOutNtf(void *meshDataPtr)
{
    if(!meshDataPtr)
        return FALSE;

    bool retVal = FALSE;
    QMESH_MESH_MSG_DATA_T *dataPtr = (QMESH_MESH_MSG_DATA_T *) meshDataPtr;

    DEBUG_BEARER_INFO (mesh_group, "SendMeshProxyDataOutNtf\n");
    if((dataPtr->length > 0) && (dataPtr->msg))
    {
        if((g_gattServerData->srvcRegd == QMESH_GATT_BEARER_PROXY_SVC_E) &&
            (g_gattServerData->srvcId > 0) &&
            (dataPtr->connectionId > 0))
        {
            QMESH_GATT_DEVICE_INFO_T *devInfo = getDeviceInfo(dataPtr->connectionId);
            if (devInfo == NULL)
            {
                DEBUG_BEARER_INFO (mesh_group, "SendMeshProxyDataOutNtf : Dev Info NULL\n");
                return FALSE;
            }

            /* Check for Notification Enabled */
            if (devInfo->notifyEnabled)
            {
                int32_t result= -1;
                DEBUG_BEARER_INFO (mesh_group, "BEARER : SrvcId %x, ConnId %x, PayloadLen %x, AttrOffset %x\n",
                     g_gattServerData->srvcId, dataPtr->connectionId, dataPtr->length, DATA_OUT_CHARACTERISTIC_ATTRIBUTE_OFFSET);

                result = qapi_BLE_GATT_Handle_Value_Notification(GetBluetoothStackId(),
                                            g_gattServerData->srvcId,
                                            dataPtr->connectionId,
                                            DATA_OUT_CHARACTERISTIC_ATTRIBUTE_OFFSET,
                                            dataPtr->length, dataPtr->msg);
                if(result <= 0)
                {
                    DEBUG_BEARER_ERROR (mesh_group, "Error -Failed to send the notification data %d\n", result);
                }
                else
                {
                    retVal = TRUE;
                }
            }
            else
            {
                DEBUG_BEARER_ERROR (mesh_group, "Notification Not Enabled\n");
            }
        }
        else
        {
            DEBUG_BEARER_ERROR (mesh_group, "Error -Proxy Service is not registered or Connection lost\n");
        }
    }
    else
    {
        DEBUG_BEARER_ERROR (mesh_group, "Error -Parameters not valid\n");
    }

    return retVal;
}


QMESH_GATT_BEARER_SERVICE_TYPE_T GetGATTBearerServiceType(void)
{
    if(g_gattServerData)
        return g_gattServerData->srvcRegd;
    else
        return QMESH_GATT_BEARER_UNKOWN_SVC_E;
}


bool IsGATTProxyConnected(void)
{
    if((g_gattServerData) && (g_gattServerData->srvcRegd == QMESH_GATT_BEARER_PROXY_SVC_E))
    {
        uint8_t idx;
        /* Check for valid connection ID*/
        for(idx = 0; idx < MAX_NUM_OF_GATT_CONN; idx++)
        {
            if (g_gattServerData->devInfo[idx].connectionId > 0)
                break;
        }

        return (idx < MAX_NUM_OF_GATT_CONN) ? TRUE : FALSE;
    }
    else
        return FALSE;
}

bool setGATTConnBearerHandle(uint32_t connId, QMESH_BEARER_HANDLE_T bearer_handle)
{
    bool retVal = FALSE;
    QMESH_GATT_DEVICE_INFO_T*  devInfo = NULL;

    devInfo = getDeviceInfo(connId);
    if (devInfo != NULL)
    {
        DEBUG_BEARER_INFO (mesh_group, "setGATTConnBearerHandle : Success\n");
        devInfo->bearerHandle = bearer_handle;
        retVal = TRUE;
    }

    return retVal;
}


static void QAPI_BLE_BTPSAPI GattServerEventCallback(uint32_t btStackID,
    qapi_BLE_GATT_Server_Event_Data_t *GattServerEventData, uint32_t CallbackParameter)
{
    QMESH_GATT_DEVICE_INFO_T*  devInfo = NULL;

    if((btStackID == GetBluetoothStackId()) && (GattServerEventData))
    {
        switch(GattServerEventData->Event_Data_Type)
        {
            case QAPI_BLE_ET_GATT_SERVER_DEVICE_CONNECTION_E:
            {
                if (GattServerEventData->Event_Data.GATT_Device_Connection_Data)
                {
                    qapi_BLE_GATT_Device_Connection_Data_t *connData =
                        GattServerEventData->Event_Data.GATT_Device_Connection_Data;

                    DEBUG_BEARER_INFO (mesh_group, "BEARER :DEVICE CONNECTED Conn Id %x, MTU %x, Remote BDADDR =%x:%x:%x:%x:%x:%x\n",
                        connData->ConnectionID, connData->MTU, connData->RemoteDevice.BD_ADDR0, connData->RemoteDevice.BD_ADDR1,
                        connData->RemoteDevice.BD_ADDR2, connData->RemoteDevice.BD_ADDR3,connData->RemoteDevice.BD_ADDR4, connData->RemoteDevice.BD_ADDR5);

                    devInfo = getDeviceInfo(0);
                    if (devInfo != NULL)
                    {
                        DEBUG_BEARER_INFO (mesh_group, "BEARER : CONN :: Conn Id %x\n", connData->ConnectionID);
                        /* Save Connection ID , Remote Device Address       */
                        devInfo->connectionId = connData->ConnectionID;
                        memcpy(&devInfo->devAddr, &connData->RemoteDevice , sizeof(qapi_BLE_BD_ADDR_t));
                        devInfo->notifyEnabled = FALSE;
                        memset(&devInfo->ccdHndlValue, 0 , DATA_OUT_CHARACTERISTIC_CCD_WRITE_VALUE_LENGTH);
                        devInfo->bearerHandle = QMESH_BEARER_HANDLE_INVALID;
                    }
                    /* TODO:- Need to handle if all index are used */
                }
            }
            break;

            case QAPI_BLE_ET_GATT_SERVER_DEVICE_DISCONNECTION_E:
            {
                 if (GattServerEventData->Event_Data.GATT_Device_Disconnection_Data)
                {
                    qapi_BLE_GATT_Device_Disconnection_Data_t *discData =
                        GattServerEventData->Event_Data.GATT_Device_Disconnection_Data;

                    DEBUG_BEARER_INFO (mesh_group, "BEARER : DEVICE DISCONNECTED Conn Id %x, Remote BDADDR =%x:%x:%x:%x:%x:%x\n",
                        discData->ConnectionID, discData->RemoteDevice.BD_ADDR0, discData->RemoteDevice.BD_ADDR1, discData->RemoteDevice.BD_ADDR2,
                        discData->RemoteDevice.BD_ADDR3,discData->RemoteDevice.BD_ADDR4, discData->RemoteDevice.BD_ADDR5);

                    devInfo = getDeviceInfo(discData->ConnectionID);
                    if (NULL != devInfo)
                    {
                        DEBUG_BEARER_INFO (mesh_group, "DISC :: Conn Id %x", discData->ConnectionID);
                        /* Reset devInfo data */
                        memset(devInfo, 0, sizeof(QMESH_GATT_DEVICE_INFO_T));
                        devInfo->bearerHandle = QMESH_BEARER_HANDLE_INVALID;
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

                    DEBUG_BEARER_INFO (mesh_group, "BEARER : READ REQUEST Conn Id %x, TransId %x, SrvcId %u, AttrOffset %x\n",rrd->ConnectionID, rrd->TransactionID, rrd->ServiceID, rrd->AttributeOffset);
                    devInfo = getDeviceInfo(rrd->ConnectionID);

                    if ((NULL != devInfo) && (rrd->ServiceID == g_gattServerData->srvcId))
                    {
                        /* Read Request for CCD Handle */
                        if (rrd->AttributeOffset == DATA_OUT_CHARACTERISTIC_CCD_ATTRIBUTE_OFFSET)
                        {
                            DEBUG_BEARER_INFO (mesh_group, "BEARER : Sending READ RESPONSE with TransId %x for Srvc %d\n",rrd->TransactionID, g_gattServerData->srvcId);

                            if(g_gattServerData->srvcRegd == QMESH_GATT_BEARER_PROV_SVC_E)
                            {
                                DEBUG_BEARER_INFO (mesh_group, "BEARER : PROV Sending Read Response AttrValue [%x %x]\n", 
                                    devInfo->ccdHndlValue[0], devInfo->ccdHndlValue[1]);
                                /* Sending Read Response for Provisioinng CCD Handle */
                                qapi_BLE_GATT_Read_Response(GetBluetoothStackId(), rrd->TransactionID, DATA_OUT_CHARACTERISTIC_CCD_WRITE_VALUE_LENGTH, devInfo->ccdHndlValue);
                            }
                            else if(g_gattServerData->srvcRegd == QMESH_GATT_BEARER_PROXY_SVC_E)
                            {
                                DEBUG_BEARER_INFO (mesh_group, "BEARER : PROXY Sending Read Response AttrValue [%x %x]\n", 
                                    devInfo->ccdHndlValue[0], devInfo->ccdHndlValue[1]);
                                /* Sending Read Response for Proxy CCD Handle */
                                qapi_BLE_GATT_Read_Response(GetBluetoothStackId(), rrd->TransactionID, DATA_OUT_CHARACTERISTIC_CCD_WRITE_VALUE_LENGTH, devInfo->ccdHndlValue);
                            }
                            else
                            {
                                DEBUG_BEARER_ERROR (mesh_group, "BEARER : Error -Provisioning/Proxy service is not registered or serviceID is not valid\n");
                            }
                        }
                        else
                        {
                             DEBUG_BEARER_ERROR (mesh_group, "BEARER : Not Handled : Read Request Received for handle with offset %x\n",rrd->AttributeOffset);
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

                    DEBUG_BEARER_INFO (mesh_group, "BEARER : WRITE REQUEST Conn Id %x, TransId %x, SrvcId %u, AttrOffset %x, valueLen %x\n",
                               wrd->ConnectionID, wrd->TransactionID, wrd->ServiceID, wrd->AttributeOffset, wrd->AttributeValueLength);

                    devInfo = getDeviceInfo(wrd->ConnectionID);
                    if ((NULL != devInfo) && (wrd->ServiceID == g_gattServerData->srvcId))
                    {
                        /*CCC value written for enable/diable notification */
                        if (wrd->AttributeOffset == DATA_OUT_CHARACTERISTIC_CCD_ATTRIBUTE_OFFSET)
                        {
                            uint16_t ntfValue;
                            bool sendConnUpdateReq = FALSE;

                            QmeshMemCpy((void *) &ntfValue, wrd->AttributeValue, DATA_OUT_CHARACTERISTIC_CCD_WRITE_VALUE_LENGTH);

                            if(ntfValue > 0)
                            {
                                DEBUG_BEARER_INFO (mesh_group, "BEARER : Notification enabled by remote device\n");
                                if(g_gattServerData->srvcRegd == QMESH_GATT_BEARER_PROV_SVC_E)
                                {
                                    /* Store Provisioning CCD handle value */
                                    QmeshMemCpy(devInfo->ccdHndlValue, wrd->AttributeValue, DATA_OUT_CHARACTERISTIC_CCD_WRITE_VALUE_LENGTH);
                                    DEBUG_BEARER_INFO (mesh_group, "BEARER : PROV Write Request AttrValue [%x %x]\n", 
                                                            devInfo->ccdHndlValue[0], devInfo->ccdHndlValue[1]);
                                    /* Set Notify enable flag */
                                    devInfo->notifyEnabled = devInfo->ccdHndlValue[0] == 1 ? TRUE : FALSE;
                                    DEBUG_BEARER_INFO (mesh_group, "BEARER : PROV CCD Sending Write Response TransId %x\n", wrd->TransactionID);
                                    sendConnUpdateReq = TRUE;
                                }
                                else if(g_gattServerData->srvcRegd == QMESH_GATT_BEARER_PROXY_SVC_E)
                                {
                                    /* Store Proxy CCD handle value */
                                    QmeshMemCpy(devInfo->ccdHndlValue, wrd->AttributeValue, DATA_OUT_CHARACTERISTIC_CCD_WRITE_VALUE_LENGTH);
                                    DEBUG_BEARER_INFO (mesh_group, "BEARER : PROXY Write Request AttrValue [%x %x]\n", 
                                                            devInfo->ccdHndlValue[0], devInfo->ccdHndlValue[1]);
                                    /* Set Notify enable flag */
                                    devInfo->notifyEnabled = devInfo->ccdHndlValue[0] == 1 ? TRUE : FALSE;
                                    /* Sending Write Response */
                                    DEBUG_BEARER_INFO (mesh_group, "BEARER : PROXY CCD Sending Write Response TransId %x\n", wrd->TransactionID);
                                    sendConnUpdateReq = TRUE;
                                }
                                else
                                {
                                    DEBUG_BEARER_ERROR (mesh_group, "BEARER : Error -Provisioning/Proxy service is not registered or serviceID is not valid\n");
                                }

                                if (sendConnUpdateReq)
                                {
                                    uint16_t attValZero = (wrd->AttributeValue[0] & 0xFF) << 8;

                                    /* Updating APP for CCD Write Request */
                                    qapi_BLE_GATT_Connection_Event_Data_t   GattConnectionEventData;
                                    qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t   cccdUpdateData;
                                    cccdUpdateData.ConnectionID = wrd->ConnectionID;
                                    cccdUpdateData.ClientConfigurationValue = (attValZero) || (wrd->AttributeValue[1] & 0xFF);
                                    QmeshMemCpy(&cccdUpdateData.RemoteDevice, &wrd->RemoteDevice, sizeof(qapi_BLE_BD_ADDR_t));

                                    GattConnectionEventData.Event_Data_Type = QAPI_BLE_ET_GATT_CONNECTION_SERVICE_CHANGED_CCCD_UPDATE_E;
                                    GattConnectionEventData.Event_Data_Size = sizeof(qapi_BLE_GATT_Connection_Service_Changed_CCCD_Update_Data_t);
                                    GattConnectionEventData.Event_Data.GATT_Connection_Service_Changed_CCCD_Update_Data = &cccdUpdateData;
                                    g_gattData->appGattEvtCallback(GetBluetoothStackId(), &GattConnectionEventData, g_gattServerData->srvcRegd);
                                }
                            }
                            else
                            {
                                DEBUG_BEARER_ERROR (mesh_group, "BEARER : Notifications has been disabled\n");
                            }

                            /* Sending Write Response */
                            qapi_BLE_GATT_Write_Response(GetBluetoothStackId(), wrd->TransactionID);

                            /* Send connection parameter update request to remote device */
                            if(sendConnUpdateReq)
                            {
                                SendConnectionParamUpdateRequest(devInfo->devAddr);
                            }
                        }
                        else
                        {
                            /* Data received via write-request to be passed on to stack */
                            if(g_gattServerData->srvcRegd == QMESH_GATT_BEARER_PROV_SVC_E)
                            {
                                DEBUG_BEARER_INFO (mesh_group, "BEARER : PROV AttrValLen %x\n", wrd->AttributeValueLength);
                                /* Passing Data to Mesh Stack */
                                _sendDataToMeshStack(QMESH_GATT_BEARER_PROV_SVC_E, devInfo, wrd->AttributeValueLength, wrd->AttributeValue);
                            }
                            else if(g_gattServerData->srvcRegd == QMESH_GATT_BEARER_PROXY_SVC_E)
                            {
                                DEBUG_BEARER_INFO (mesh_group, "BEARER : PROXY AttrValLen %x\n", wrd->AttributeValueLength);
                                /* Passing Data to Mesh Stack */
                                _sendDataToMeshStack(QMESH_GATT_BEARER_PROXY_SVC_E, devInfo, wrd->AttributeValueLength, wrd->AttributeValue);
                            }
                            else
                            {
                                DEBUG_BEARER_ERROR (mesh_group, "BEARER : Error -Provisioning/Proxy service is not registered or serviceID is not valid\n");
                            }
                            qapi_BLE_GATT_Write_Response(GetBluetoothStackId(), wrd->TransactionID);
                        }
                    }
                    else
                    {
                        DEBUG_BEARER_ERROR (mesh_group,  "Error -Either the serviceId or connectionID or attributeoffset \
                            is wrong, hence not sending data for processing\n");
                    }
                }
            }
            break;

            case QAPI_BLE_ET_GATT_SERVER_DEVICE_CONNECTION_MTU_UPDATE_E:
            {
                
                qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *mtuUpd = 
                    GattServerEventData->Event_Data.GATT_Device_Connection_MTU_Update_Data;
                
                DEBUG_BEARER_INFO (mesh_group, "BEARER : Received  QAPI_BLE_ET_GATT_SERVER_DEVICE_CONNECTION_MTU_UPDATE_E Conn Id %x, MTU 0x%x\n",
                               mtuUpd->ConnectionID, mtuUpd->MTU);

                /*Send the MTU update event to application*/
                SendMtuUpdateEvent(mtuUpd->ConnectionID, mtuUpd->ConnectionType, &mtuUpd->RemoteDevice, mtuUpd->MTU);
            }  
            break;

            default:
            {
                DEBUG_BEARER_INFO (mesh_group, "Ignoring the received message %x\n",
                                 GattServerEventData->Event_Data_Type);
            }
            break;
        }
    }
}

