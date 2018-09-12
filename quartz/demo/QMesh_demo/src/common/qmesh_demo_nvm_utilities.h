/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*
  * This file contains data structures for NVM implementation.
 */
/*****************************************************************************/
#ifndef __QMESH_DEMO_NVM_UTILITIES__ 
#define __QMESH_DEMO_NVM_UTILITIES__ 
#include "qmesh_ps.h"
#include "qmesh_result.h"
#include "qmesh_types.h"
#include "qmesh_config_server.h"

#ifdef CONFIG_QMESH_PROVISIONER_DEMO
#include "qmesh_demo_provisioner.h"
#endif

#ifdef CONFIG_QMESH_CLIENT_DEMO
#include "qmesh_demo_client.h"
#endif

/******************************************************************************
 *NVM Helper Methods
 ******************************************************************************/
#if (QMESH_PS_IFCE_ENABLED == 1)
#define NO_OF_ELEMENTS (3)
#define NO_OF_MODELS (20)
#define MAX_NO_OF_SUBSCRIPTIONS (128)
#define MAX_NO_OF_APPKEYS (20)
#define MAX_NO_OF_LABEL_UUID (10)

/*Element sequence numbers are saved to NVM after every NVM_SEQUENCE_SAVE_TIMEOUT milliseconds */
#define NVM_SEQUENCE_SAVE_TIMEOUT (2000 *60 ) /*2 minutes*/

typedef enum {
    QMESH_APP_PS_KEY_PROV_STATE = QMESH_PS_KEY_APPLICATION_GROUP, /*provisioning state*/
    QMESH_APP_PS_KEY_PRIM_ELEM_ADDR, /*Primary element address*/
    QMESH_APP_PS_KEY_ELEM_SEQ_NUM, /*Sequence numbers for elements*/
    QMESH_APP_PS_KEY_APPKEY_BIND_LIST, /*App Key Bind List*/
    QMESH_APP_PS_KEY_MODEL_PUB_STATUS, /*Model's publish state*/
    QMESH_APP_PS_KEY_MODEL_SUBSCRIPTION, /*Model's subscription info*/
    QMESH_APP_PS_KEY_LABEL_UUID, /*Label UUID stored for virtual address */
    QMESH_APP_PS_KEY_IDENTITY, /*Node Identity stored for every Network Id */
}APP_NVM_KEYS_T;

/*Method to Initialize NVM on bootup*/
QmeshResult NVMInit (QMESHAPP_CONTEXT_T *app_context);
/*Method to Write Provisioning State to NVM*/
void NVMWrite_ProvState(bool prov_state);
/*Method to Write Primary Element Address to NVM*/
void NVMWrite_PrimaryElementAddress(uint16_t elm_addr);
/*Method to write element's sequence number ti NVM*/
void NVMWrite_ElementSequenceNumber (uint8_t * buffer);
/*Method to add/update label UUID in NVM*/
void NVMWrite_LabelUUID (uint8_t elemid, uint8_t modelid, uint8_t * uuid, 
                         QMESH_MODEL_SUB_OPCODE_T op);
/*Update Identity info for subnet*/
void NVMWrite_SubNetIdentityUpdate (QMESHAPP_SUBNET_DATA_T *data, bool delete);
/*Method to write App Key Binding to NVM*/
void NVMWrite_ModelAppKeyAdd(uint8_t elementid, uint8_t modelid, uint16_t app_key_index);
void NVMWrite_ModelAppKeyDelete(uint8_t elementid, uint8_t modelid, uint16_t app_key_index);
/*Methods to add/remove subscriptions info from NVM*/
void NVMWrite_ModelSubscriptionAdd(uint8_t elementid, uint8_t modelid, uint16_t sub_addr);
void NVMWrite_ModelSubscriptionDelete(uint8_t elementid, uint8_t modelid, uint16_t sub_addr);
void NVMWrite_ModelSubscriptionUpdate(uint8_t elementid, uint8_t modelid, uint16_t sub_addr);
void NVMWrite_ModelSubscriptionDeleteAll(uint8_t elementid, uint8_t modelid);
/*Methods to write publication info to NVM*/
void NVMWrite_ModelPublicationStatus(uint8_t elementid, uint8_t modelid, QMESH_MODEL_PUBLISH_STATE_T *pub_state);
/*Create timer for periodically storing the sequence number to NVM*/
void StoreElementSequenceNo (void);
void StopElementSequenceNoTimer (void);
void StartElementSequenceNoTimer (void);
#endif
#endif
