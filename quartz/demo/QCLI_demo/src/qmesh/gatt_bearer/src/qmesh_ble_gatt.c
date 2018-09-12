/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "qapi_ble_gatt.h"
#include "qmesh.h"
#include "qmesh_sched.h"
#include "qmesh_types.h"
#include "qmesh_ble_gap.h"
#include "qmesh_hal_ifce.h"
#include "qmesh_gw_debug.h"
#include "qmesh_ble_gatt.h"
#include "qmesh_ble_gatt_server.h"
#include "qmesh_ble_gatt_client.h"
#include "qmesh_bearer_debug.h"

Qmesh_Gatt_Data_T *g_gattData;
extern QCLI_Group_Handle_t     mesh_group;
static bool gattBearerInitialized = FALSE;
QMESH_GATT_BEARER_CNTXT_INFO_T *cntxtPtr;

bool InitGattBearer(QMESH_GATT_BEARER_SERVICE_TYPE_T serviceReq,
                            APP_GATT_Bearer_Callback funcPtr,
                            Qmesh_Gatt_Data_T * gattData)
{
    bool retVal = TRUE;

    DEBUG_BEARER_INFO (mesh_group,"BEARER : InitializeGattBearer\n");

    if (gattData == NULL)
        return FALSE;

    /* Extra condition added to allow client init bearer second time in case 
       when one of the call back (either for Provisioning or Proxy service) is 
       stil left to be initialized 
    */
    if ((FALSE == gattBearerInitialized) ||
        ((TRUE == gattData->provisioner_role_enabled) && (TRUE == IsClientBearerInitAllowed())))
    {
        if (NULL == g_gattData)
        {
            /* Allocate memory and initialize g_gattData */
            g_gattData = (Qmesh_Gatt_Data_T *) malloc (sizeof(Qmesh_Gatt_Data_T));

            if(NULL == g_gattData )
            {
               DEBUG_BEARER_ERROR (mesh_group, "BEARER : Failed to allocate memory\n");
               return FALSE;
            }

            QmeshMemSet(g_gattData, 0, sizeof(Qmesh_Gatt_Data_T));

            if (gattData->appGapLeEvtCallback!= NULL)
                g_gattData->appGapLeEvtCallback = gattData->appGapLeEvtCallback;

            if (gattData->appGattEvtCallback!= NULL)
                g_gattData->appGattEvtCallback = gattData->appGattEvtCallback;

            g_gattData->provisioner_role_enabled = gattData->provisioner_role_enabled;

            gattBearerInitialized = TRUE;
        }

        if (FALSE == g_gattData->provisioner_role_enabled)
        {
            InitializeGattServerBearer();
        }
        else
        {
            InitializeGattClientBearer(funcPtr, serviceReq);
        }
    }
    return retVal;
}


bool DeInitGattBearer(void)
{
    bool retVal=TRUE;

    if(g_gattData)
    {
        if (IsProvisionerRoleEnabled())
        {
            DeInitializeGattClientBearer();
        }
        else
        {
            DeInitializeGattServerBearer();
        }

        QmeshFree(g_gattData);
        g_gattData = NULL;
    }
    gattBearerInitialized = FALSE;
    return retVal;
}


extern void QAPI_BLE_BTPSAPI GATT_Connection_Event_Callback(uint32_t btStackId,
                            qapi_BLE_GATT_Connection_Event_Data_t *GATT_Connection_Event_Data,
                            uint32_t CallbackParameter)
{
    if (btStackId == GetBluetoothStackId())
    {
        if (NULL == GATT_Connection_Event_Data)
        {
            DEBUG_BEARER_INFO(mesh_group, "GATT_Connection_Event_Data is NULL in %s\n", __FUNCTION__);
            return;
        }

        switch(GATT_Connection_Event_Data->Event_Data_Type)
        {
            case QAPI_BLE_ET_GATT_CONNECTION_SERVICE_DATABASE_UPDATE_E:
            {
                DEBUG_BEARER_INFO(mesh_group, "BEARER : %s : SERVICE DATABASE UPDATE Event need to handle\n", __FUNCTION__);
            }
            break;

            case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_E:
            {
                qapi_BLE_GATT_Device_Connection_Data_t *connData = GATT_Connection_Event_Data->Event_Data.GATT_Device_Connection_Data;

                if(connData)
                {
                    DEBUG_BEARER_INFO(mesh_group, "BEARER : ConnId=%x, connType=%d, MTU=0x%x, Addr=0x%2x%2x%2x%2x%2x%2x\n",
                            connData->ConnectionID, connData->ConnectionType, connData->MTU,
                            connData->RemoteDevice.BD_ADDR5, connData->RemoteDevice.BD_ADDR4, connData->RemoteDevice.BD_ADDR3,
                            connData->RemoteDevice.BD_ADDR2, connData->RemoteDevice.BD_ADDR1, connData->RemoteDevice.BD_ADDR0);

                    if ((g_gattData != NULL) && 
                        (g_gattData->appGattEvtCallback != NULL))
                    {
                        qapi_BLE_GATT_Connection_Event_Data_t   GattConnectionEventData;
                        GattConnectionEventData.Event_Data_Type = QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_E;
                        GattConnectionEventData.Event_Data_Size = GATT_Connection_Event_Data->Event_Data_Size;
                        GattConnectionEventData.Event_Data.GATT_Device_Connection_Data = GATT_Connection_Event_Data->Event_Data.GATT_Device_Connection_Data;
                        g_gattData->appGattEvtCallback(GetBluetoothStackId(), &GattConnectionEventData, GetGATTClientServiceType(connData->ConnectionID));

                        /*Holding the new-gatt connection data, this might be required in case the MTU-exchange throws error*/
                        memset(&g_gattData->newGatConnData, 0, QAPI_BLE_GATT_DEVICE_CONNECTION_DATA_SIZE);
                        memcpy(&g_gattData->newGatConnData, GATT_Connection_Event_Data->Event_Data.GATT_Device_Connection_Data, QAPI_BLE_GATT_DEVICE_CONNECTION_DATA_SIZE);

                    }
                    
                    if(IsProvisionerRoleEnabled())
                    {
                    	bool checkSrvcs = FALSE;
                        QmeshSchedStart();
                    
                        DEBUG_BEARER_INFO(mesh_group, "BEARER : %s : Proceeding with ATT_MTU exchange \n");
                        
                        if(!ExchangeMtu(connData))
                             checkSrvcs = TRUE;
                        
                        if(checkSrvcs)
                        {
                            DEBUG_BEARER_INFO(mesh_group, "BEARER : %s : Initiating service discovery\n", __FUNCTION__);
                            InitiateServiceDiscovery(connData);
                        }
                    }
                }
                else
                {
                    DEBUG_BEARER_ERROR(mesh_group," Connection data is not valid\n");
                }
            }
            break;

            case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_DISCONNECTION_E:
            {
                QMESH_GATT_BEARER_SERVICE_TYPE_T srvcRegd = QMESH_GATT_BEARER_UNKOWN_SVC_E;

                DEBUG_BEARER_INFO(mesh_group, "BEARER : Received GATT-Disconnection, srvcRegd=%x\n", srvcRegd);

                if((g_gattData != NULL) &&
                   (g_gattData->appGattEvtCallback != NULL))
                {
                    qapi_BLE_GATT_Connection_Event_Data_t   GattConnectionEventData;
                    qapi_BLE_GATT_Device_Disconnection_Data_t *discData;

                    GattConnectionEventData.Event_Data_Type = QAPI_BLE_ET_GATT_CONNECTION_DEVICE_DISCONNECTION_E;
                    GattConnectionEventData.Event_Data_Size = GATT_Connection_Event_Data->Event_Data_Size;
                    GattConnectionEventData.Event_Data.GATT_Device_Disconnection_Data = GATT_Connection_Event_Data->Event_Data.GATT_Device_Disconnection_Data;
                    DEBUG_BEARER_INFO(mesh_group, "BEARER : %s : Invoking GATT-Connection-CB function of app\n", __FUNCTION__);

                    discData = GattConnectionEventData.Event_Data.GATT_Device_Disconnection_Data;

                    if(FALSE == IsProvisionerRoleEnabled())
                    {
                        srvcRegd = GetGATTBearerServiceType();
                    }
                    else
                    {
                        srvcRegd = GetGATTClientServiceType(discData->ConnectionID);
                        DestroyGattClientData(discData->ConnectionID);
                    }

                    if (QMESH_GATT_BEARER_UNKOWN_SVC_E != srvcRegd)
                    {
                        g_gattData->appGattEvtCallback(GetBluetoothStackId(), &GattConnectionEventData, srvcRegd);
                    }
                }
            }
            break;

            /* Handled by Provisioning/Proxy client to receive the data from server via
             * notification
             */
            case QAPI_BLE_ET_GATT_CONNECTION_SERVER_NOTIFICATION_E:
            {
                if(GATT_Connection_Event_Data->Event_Data.GATT_Server_Notification_Data)
                {
                    HandleServerNtf(GATT_Connection_Event_Data->Event_Data.GATT_Server_Notification_Data);
                }
                else
                {
                    DEBUG_BEARER_ERROR (mesh_group,  "Error - Notification data is not valid\n");
                }
            }
            break;
            
            case QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_MTU_UPDATE_E:
            {
                #ifdef DEBUG_GAP_GATT_BEARER
                qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *connMtuUpd = 
                    GATT_Connection_Event_Data->Event_Data.GATT_Device_Connection_MTU_Update_Data;

                DEBUG_BEARER_INFO(mesh_group, "BEARER : QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_MTU_UPDATE_E : ConnectionId=%d, MTU=0x%x\n", 
                    connMtuUpd->ConnectionID, connMtuUpd->MTU);
                #endif
                
            }
            break;
            
            default:
            {
                DEBUG_BEARER_INFO(mesh_group, "BEARER : %s : Event %x not handled\n", __FUNCTION__, GATT_Connection_Event_Data->Event_Data_Type);
            }
            break;
        }
    }

}

QMESH_GATT_BEARER_CNTXT_INFO_T *SetGATTBearerHandler(QMESH_GATT_BEARER_SERVICE_TYPE_T srvcType,
                                                                uint32_t connId)
{
    cntxtPtr = (QMESH_GATT_BEARER_CNTXT_INFO_T *) malloc(sizeof(QMESH_GATT_BEARER_CNTXT_INFO_T));

    DEBUG_BEARER_INFO(mesh_group,  "Setting the GATT bearer send handler\n");
    if(!cntxtPtr)
    {
        DEBUG_BEARER_ERROR(mesh_group,  "Error - Unable to allocate memory\n");
        return NULL;
    }
    cntxtPtr->connectionId = connId;
    if(IsProvisionerRoleEnabled())
    {
        /*Provisioning/Proxy Client*/
        if((srvcType == QMESH_GATT_BEARER_PROV_SVC_E) ||
            (srvcType == QMESH_GATT_BEARER_PROXY_SVC_E))
        {
            cntxtPtr->funcPtr = SendMeshGattClientData;
        }
        else
        {
            DEBUG_BEARER_ERROR(mesh_group,  "Error - Invalid service\n");
        }
    }
    else
    {
        /*Provisioning/Proxy Server*/
        if(srvcType == QMESH_GATT_BEARER_PROV_SVC_E)
        {
            cntxtPtr->funcPtr = SendMeshProvDataOutNtf;
        }
        else
        {
            cntxtPtr->funcPtr = SendMeshProxyDataOutNtf;
        }
    }

    return cntxtPtr;
}

bool SendMtuUpdateEvent(uint32_t connId, 
    					qapi_BLE_GATT_Connection_Type_t connType, 
    					const qapi_BLE_BD_ADDR_t *addr, 
    					uint16_t mtu)
{
    bool retVal=FALSE;
    
	if((g_gattData != NULL) &&
       (g_gattData->appGattEvtCallback != NULL))
    {
        qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *mtuUpdateData;
        qapi_BLE_GATT_Connection_Event_Data_t   GattConnectionEventData;

        mtuUpdateData = (qapi_BLE_GATT_Device_Connection_MTU_Update_Data_t *) malloc(QAPI_BLE_GATT_DEVICE_CONNECTION_MTU_UPDATE_DATA_SIZE);

        if(mtuUpdateData)
        {
            memset(mtuUpdateData, 0, QAPI_BLE_GATT_DEVICE_CONNECTION_MTU_UPDATE_DATA_SIZE);

            /*Copy the data required for sending the MTU event to above application*/
	        mtuUpdateData->ConnectionID = connId;
	        mtuUpdateData->ConnectionType = connType;
	        memcpy(&mtuUpdateData->RemoteDevice, addr, sizeof(qapi_BLE_BD_ADDR_t));
	        mtuUpdateData->MTU = mtu;
	        
	        GattConnectionEventData.Event_Data_Type = QAPI_BLE_ET_GATT_CONNECTION_DEVICE_CONNECTION_MTU_UPDATE_E;
	        GattConnectionEventData.Event_Data_Size = QAPI_BLE_GATT_DEVICE_CONNECTION_MTU_UPDATE_DATA_SIZE;
	        GattConnectionEventData.Event_Data.GATT_Device_Connection_MTU_Update_Data = mtuUpdateData;
	        
	        DEBUG_BEARER_INFO(mesh_group, "BEARER : %s : Invoking GATT-Connection-CB function of app\n", __FUNCTION__);
	        g_gattData->appGattEvtCallback(GetBluetoothStackId(), &GattConnectionEventData, GetGATTClientServiceType(connId));

			/*Initiate service discovery for client role since the MTU exchange has compelted */
	        if(IsProvisionerRoleEnabled())
	        {
	            DEBUG_BEARER_INFO(mesh_group, "BEARER : %s : Initiating service discovery\n", __FUNCTION__);
	            InitiateServiceDiscovery((qapi_BLE_GATT_Device_Connection_Data_t *)mtuUpdateData);
	        }
            retVal=TRUE;

            free(mtuUpdateData);
        }
        else
        {
            DEBUG_BEARER_ERROR(mesh_group, "BEARER : %s : Failed to allocate memory\n", __FUNCTION__);
        }
    }
    else
    {
    	DEBUG_BEARER_ERROR(mesh_group, "BEARER : %s : Invalid callback pointer\n", __FUNCTION__);
    }

    return retVal;
}

qapi_BLE_GATT_Device_Connection_Data_t *GetNewGattConnData(void)
{
    if(g_gattData)
    {
       return &g_gattData->newGatConnData;
    }

    return NULL;
}

void RemoveGATTBearerHandler(QMESH_GATT_BEARER_SERVICE_TYPE_T srvcType)
{
    DEBUG_BEARER_INFO(mesh_group, "Remove the GATT bearer send handler\n");
    if(cntxtPtr)
    {
        QmeshFree(cntxtPtr);
    }
}

bool IsProvisionerRoleEnabled(void)
{
    return g_gattData->provisioner_role_enabled;
}
