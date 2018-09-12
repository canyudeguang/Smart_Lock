/******************************************************************************
Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/
/*
  * This file contains NVM routines to read/write Qmesh data on to persistent storage.
 */
/*****************************************************************************/

#ifdef CONFIG_QMESH_PROVISIONER_DEMO
#include "qmesh_demo_config.h"
#endif

#ifdef CONFIG_QMESH_CLIENT_DEMO
#include "qmesh_demo_client_config.h"
#endif
#include "qmesh_demo_nvm_utilities.h"
#include "qmesh_demo_utilities.h"
#include "qmesh_demo_debug.h"
#include "qmesh_ps.h"
#include "qmesh_model_nvm.h"

#include "qmesh_model_common.h"
//#include "qmesh_demo_composition.h"
#include <stdlib.h>

#define PS_PUB_STATUS_LEN        (26) /*size of QMESH_MODEL_PUBLISH_STATE_T - conf_handler*/
#define PS_LABEL_UUID_LEN        (18) /*elemid(1) + modelid(1) + QMESH_UUID_SIZE_OCTETS*/

typedef struct __attribute__ ((__packed__))
{
    QMESH_KEY_IDX_INFO_T        netkey_idx;
} APP_NVM_IDENTITY_DATA_T;

typedef void (*QMESH_PS_READ_HANDLER_T)(uint8_t *buf, uint8_t buf_len);
typedef struct {
    QMESH_PS_KEY_INFO_T key;
    QMESH_PS_READ_HANDLER_T handler;
} QMESH_APP_PS_KEY_INFO_T;

extern QMESH_TIMER_GROUP_HANDLE_T app_timer_ghdl;
extern QCLI_Group_Handle_t mesh_group;
extern QMESH_DEVICE_COMPOSITION_DATA_T* dev_comp;

#if (QMESH_PS_IFCE_ENABLED == 1)

static QMESHAPP_CONTEXT_T *g_app_context = NULL;
void NVMHandle_ProvState (uint8_t * buffer, uint8_t buf_len);
void NVMHandle_PrimAddress (uint8_t * buffer, uint8_t buf_len);
void NVMHandle_SubNetIdentity (uint8_t * buffer, uint8_t buf_len);
void NVMHandle_LabelUUID (uint8_t * buffer, uint8_t buf_len);
void NVMHandle_ElementSequenceNumber (uint8_t * buffer, uint8_t buf_len);
void NVMHandle_ModelPublicationStatus (uint8_t * buffer, uint8_t buf_len);
void NVMHandle_ModelSubscription (uint8_t * buffer, uint8_t buf_len);
void NVMHandle_ModelAppKey (uint8_t * buffer, uint8_t buf_len);

/*Storage for Application NVM key's definitions and handlers*/
static const QMESH_APP_PS_KEY_INFO_T application_keys [] = {
    {   /*key: QMESH_APP_PS_KEY_PROV_STATE*/
        {
            QMESH_PS_DATA_SINGLE_FIXED,
            QMESH_APP_PS_KEY_PROV_STATE,
            sizeof (bool),
            1
        },
        NVMHandle_ProvState
    },    
    {   /*key: QMESH_APP_PS_KEY_PRIM_ELEM_ADDR*/
        {
            QMESH_PS_DATA_SINGLE_FIXED,
            QMESH_APP_PS_KEY_PRIM_ELEM_ADDR,
            sizeof (uint16_t),
            1
        },
        NVMHandle_PrimAddress
    },
    {   /*key: QMESH_APP_PS_KEY_ELEM_SEQ_NUM*/
        {
            QMESH_PS_DATA_SINGLE_FIXED,
            QMESH_APP_PS_KEY_ELEM_SEQ_NUM,
            sizeof (uint32_t) * NO_OF_ELEMENTS,
            1
        },
        NVMHandle_ElementSequenceNumber
    },
    {   /*key: QMESH_APP_PS_KEY_MODEL_PUB_STATUS*/
        {
            QMESH_PS_DATA_MULTIPLE_FIXED,
            QMESH_APP_PS_KEY_MODEL_PUB_STATUS,
            PS_PUB_STATUS_LEN, //elementid(1) | modelid(1) | pub address(2)
            NO_OF_MODELS
        },
        NVMHandle_ModelPublicationStatus
    },
    {   /*key: QMESH_APP_PS_KEY_MODEL_SUBSCRIPTION*/
        {
            QMESH_PS_DATA_MULTIPLE_FIXED,
            QMESH_APP_PS_KEY_MODEL_SUBSCRIPTION,
            4, //elementid(1) | modelid(1) | subscription address(2)
            MAX_NO_OF_SUBSCRIPTIONS
        },
        NVMHandle_ModelSubscription
    },
    {   /*key: QMESH_APP_PS_KEY_APPKEY_BIND_LIST*/
        {
            QMESH_PS_DATA_MULTIPLE_FIXED,
            QMESH_APP_PS_KEY_APPKEY_BIND_LIST,
            4, //elementid(1) | modelid(1) | appkey index(2)
            MAX_NO_OF_APPKEYS
        },
        NVMHandle_ModelAppKey
    },
    {   /*key: QMESH_APP_PS_KEY_LABEL_UUID*/
        {
            QMESH_PS_DATA_MULTIPLE_FIXED,
            QMESH_APP_PS_KEY_LABEL_UUID,
            PS_LABEL_UUID_LEN,
            MAX_NO_OF_LABEL_UUID
        },
        NVMHandle_LabelUUID
    },
    {   /*key: QMESH_APP_PS_KEY_IDENTITY*/
        {
            QMESH_PS_DATA_MULTIPLE_FIXED,
            QMESH_APP_PS_KEY_IDENTITY,
            sizeof (APP_NVM_IDENTITY_DATA_T),
            MAX_NUM_OF_SUB_NETWORK
        },
        NVMHandle_SubNetIdentity
    },
};

#define QMESH_APP_PS_KEY_COUNT (sizeof (application_keys)/sizeof (application_keys [0]))
/******************************************************************************
 *NVM Helper Methods
 ******************************************************************************/
/*Method to Add NVM Key on first run*/
static QMESH_RESULT_T NVMAddKeys (void);
/*Method to Read NVM Key Data on bootup*/
static QMESH_RESULT_T NVMReadAppData (QMESHAPP_CONTEXT_T *app_context);
static void NVMSerialize_ModelPublicationStatus(uint8_t *buffer, uint8_t elementid, uint8_t modelid, 
                                                QMESH_MODEL_PUBLISH_STATE_T *pub_state, size_t *length);
void NVMDeserialize_ModelPublicationStatus(uint8_t *buffer);
QMESH_TIMER_HANDLE_T sequenceTimerHandle; 
/*Buffer to store current element sequence numbers*/
static uint8_t current_seq_numbers [12] = {0};

/*Function to compare subscription and App Key data. This is passed to NVM 
 *interface so that the key data can be searched in the NVM
 */
bool KeyDataCompare (const void *user_data, const void *p_value)
{
    if (memcmp ((uint8_t*)user_data, (uint8_t*)p_value, 4))
        return false;
    return true;
}

bool ElemModelCompare (const void *user_data, const void *p_value)
{
    if (memcmp ((uint8_t*)user_data, (uint8_t*)p_value, 2))
        return false;
    return true;
}

bool IdentityCompare (const void *user_data, const void *p_value)
{
    APP_NVM_IDENTITY_DATA_T * a = (APP_NVM_IDENTITY_DATA_T *)user_data;
    APP_NVM_IDENTITY_DATA_T * b = (APP_NVM_IDENTITY_DATA_T *)p_value;
    if (a->netkey_idx.prov_net_idx == b->netkey_idx.prov_net_idx)
        return true;
    return false;
}

/*Check if sequence numbers have changed*/
static bool ElementSequenceNoChanged (uint8_t * buffer)
{
    if(memcmp (buffer, current_seq_numbers, 12))
    {
        memcpy (current_seq_numbers, buffer, 12);
        return true;
    }

    return false;
}

static void SequenceNoTimeoutHandler (QMESH_TIMER_HANDLE_T tid, void *data)
{
    if(tid == sequenceTimerHandle)
    {
        StoreElementSequenceNo (); 
        StartElementSequenceNoTimer ();
    }
}

void StartElementSequenceNoTimer ()
{
    /*Create timer for periodically storing the sequence number to NVM*/
    sequenceTimerHandle = QmeshTimerCreate(&app_timer_ghdl,
                                           SequenceNoTimeoutHandler,
                                           NULL,
                                           NVM_SEQUENCE_SAVE_TIMEOUT);
}

void StopElementSequenceNoTimer ()
{
     QmeshTimerDelete (&app_timer_ghdl, &sequenceTimerHandle);
}

void StoreElementSequenceNo ()
{
    const QMESH_ELEMENT_CONFIG_T *p_ele, *p_end_ele;
    uint8_t index = 0;
    uint8_t *buffer = malloc (sizeof(uint32_t) * dev_comp->num_elements);
    if (!buffer)
        return;

	memset (buffer, 0, sizeof(uint32_t) * dev_comp->num_elements);
    for(p_ele = &dev_comp->elements[0], p_end_ele = p_ele + dev_comp->num_elements;
        p_ele < p_end_ele;
        p_ele++)
    {
        QMESH_ELEMENT_DATA_T *ele_data = p_ele->element_data;
        memcpy (buffer + index, &ele_data->seq_num, sizeof (ele_data->seq_num));
        index +=  sizeof (ele_data->seq_num);
    }
    
    /*Do not write to NVM if buffer has not changed*/
    if (ElementSequenceNoChanged (buffer))
        NVMWrite_ElementSequenceNumber (buffer);

    free (buffer);

}

/*Write the Provisioning State to NVM*/
void  NVMWrite_ProvState(bool prov_state)
{
    size_t len = 1;
    QmeshPsAddKeyData (QMESH_APP_PS_KEY_PROV_STATE, (uint16_t*)&prov_state, &len);
}

/*Write the Sequence numbers for all elements to NVM*/
void  NVMWrite_ElementSequenceNumber (uint8_t * buffer)
{
    size_t len = 12;
    QmeshPsAddKeyData (QMESH_APP_PS_KEY_ELEM_SEQ_NUM, (uint16_t*)buffer, &len);
}

/*Write the Primary Element Address to NVM*/
void  NVMWrite_PrimaryElementAddress(uint16_t elm_addr)
{
    size_t len = 2;
    QmeshPsAddKeyData (QMESH_APP_PS_KEY_PRIM_ELEM_ADDR, &elm_addr, &len);
}

/*Write the Model Publication Status to NVM*/
void NVMWrite_ModelPublicationStatus(uint8_t elementid, uint8_t modelid, 
                                     QMESH_MODEL_PUBLISH_STATE_T *pub_state)
{
    uint8_t buffer[PS_PUB_STATUS_LEN]={0};
    uint8_t read_buf[PS_PUB_STATUS_LEN];
    size_t len = 0;
    if (pub_state->publish_uuid != NULL)
    {
        NVMWrite_LabelUUID (elementid, modelid, (uint8_t*)pub_state->publish_uuid, 
                            QMESH_MODEL_SUB_EVENT_ADD);
    }

    NVMSerialize_ModelPublicationStatus (buffer, elementid, modelid, pub_state, &len);
    if (QmeshPsUpdateKeyData (QMESH_APP_PS_KEY_MODEL_PUB_STATUS, ElemModelCompare, (uint16_t*)buffer,
                          (uint16_t*)read_buf, (uint16_t*)buffer, &len) == QMESH_RESULT_PS_NO_ELEMENT)
    {
        QmeshPsAddKeyData (QMESH_APP_PS_KEY_MODEL_PUB_STATUS, (uint16_t *)buffer, &len);
    }
}

/*Delete the Model Subscription list from NVM*/
void NVMWrite_ModelSubscriptionDelete (uint8_t elementid, uint8_t modelid, uint16_t sub_addr)
{
    uint8_t buffer [4];
    uint8_t out_buffer [4];
    size_t len = 0;
    buffer [len++] = elementid;
    buffer [len++] = modelid;
    memcpy (buffer+len, &sub_addr, sizeof (uint16_t));
    len += sizeof (uint16_t);
    QmeshPsDeleteKeyData(QMESH_APP_PS_KEY_MODEL_SUBSCRIPTION, KeyDataCompare, (uint16_t*)buffer, 
                         (uint16_t*)out_buffer, len);
}

/*Update the Model Subscription list in NVM*/
void NVMWrite_ModelSubscriptionUpdate (uint8_t elementid, uint8_t modelid, uint16_t sub_addr)
{
    uint8_t buffer[4];
    uint8_t read_buf[4];
    size_t len = 0;
    buffer [len++] = elementid;
    buffer [len++] = modelid;
    memcpy (buffer+len, &sub_addr, sizeof (uint16_t));
    len += sizeof (uint16_t);
    QmeshPsUpdateKeyData (QMESH_APP_PS_KEY_MODEL_SUBSCRIPTION, KeyDataCompare, (uint16_t*)buffer,
                          (uint16_t*)read_buf, (uint16_t*)buffer, &len);
}

/*Delete all Model Subscription list in NVM*/
void NVMWrite_ModelSubscriptionDeleteAll (uint8_t elementid, uint8_t modelid)
{
    uint8_t buffer[4];
    uint8_t out_buffer[4];
    size_t len = 0;
    buffer [len++] = elementid;
    buffer [len++] = modelid;
    QmeshPsDeleteKeyData (QMESH_APP_PS_KEY_APPKEY_BIND_LIST, ElemModelCompare, (uint16_t*)buffer, 
                         (uint16_t*)out_buffer, len);
}

/*Write the Model Subscription list to NVM*/
void NVMWrite_ModelSubscriptionAdd (uint8_t elementid, uint8_t modelid, uint16_t sub_addr)
{
    uint8_t buffer[4];
    uint8_t read_buf[4];
    size_t len = 0;
    buffer [len++] = elementid;
    buffer [len++] = modelid;
    memcpy (buffer+len, &sub_addr, sizeof (uint16_t));
    len += sizeof (uint16_t);
    if (QmeshPsUpdateKeyData (QMESH_APP_PS_KEY_MODEL_SUBSCRIPTION, KeyDataCompare, (uint16_t*)buffer,
                              (uint16_t*)read_buf, (uint16_t*)buffer, &len) == QMESH_RESULT_PS_NO_ELEMENT)
    {                              
        QmeshPsAddKeyData (QMESH_APP_PS_KEY_MODEL_SUBSCRIPTION, (uint16_t*)buffer, &len);
    }
}

/*Delete the Application Bind List in NVM*/
void NVMWrite_ModelAppKeyDelete(uint8_t elementid, uint8_t modelid, uint16_t app_key_index)
{
    uint8_t buffer[4];
    uint8_t out_buffer [4];
    size_t len = 0;
    buffer [len++] = elementid;
    buffer [len++] = modelid;
    memcpy (buffer+len, &app_key_index, sizeof (uint16_t));
    len += sizeof (uint16_t);
    QmeshPsDeleteKeyData(QMESH_APP_PS_KEY_APPKEY_BIND_LIST, KeyDataCompare, (uint16_t*)buffer, 
                         (uint16_t*)out_buffer, len);
}

/*Write the Application Bind List to to NVM*/
void NVMWrite_ModelAppKeyAdd (uint8_t elemid, uint8_t modelid, uint16_t app_key_index)
{
    uint8_t buffer[4];
    uint8_t read_buf[4];
    size_t len = 0;
    buffer [len++] = elemid;
    buffer [len++] = modelid;
    memcpy (buffer+len, &app_key_index, sizeof (uint16_t));
    len += sizeof (uint16_t);
    if (QmeshPsUpdateKeyData (QMESH_APP_PS_KEY_APPKEY_BIND_LIST, KeyDataCompare, (uint16_t*)buffer,
                              &read_buf, (uint16_t*)buffer, &len) == QMESH_RESULT_PS_NO_ELEMENT)
    {                              
        QmeshPsAddKeyData (QMESH_APP_PS_KEY_APPKEY_BIND_LIST, (uint16_t*)&buffer, &len);
    }
}

void NVMWrite_SubNetIdentityUpdate (QMESHAPP_SUBNET_DATA_T *data, bool delete)
{
    size_t len = sizeof (APP_NVM_IDENTITY_DATA_T);
    if (delete == true)
    {
        uint8_t *readbuf = malloc (sizeof (APP_NVM_IDENTITY_DATA_T));
        if (!readbuf)
            return;

        QmeshPsDeleteKeyData (QMESH_APP_PS_KEY_IDENTITY, IdentityCompare, 
                             (uint16_t*)&data->netkey_idx, (uint16_t*)readbuf, len);
        free (readbuf);
    }
    else
    {
        QmeshPsAddKeyData (QMESH_APP_PS_KEY_IDENTITY, (uint16_t*)&data->netkey_idx, &len);
    }
}

void NVMWrite_LabelUUID (uint8_t elemid, uint8_t modelid, uint8_t * uuid, 
                         QMESH_MODEL_SUB_OPCODE_T op)
{
    uint8_t buffer [PS_LABEL_UUID_LEN] = {0};
    uint8_t read_buf [PS_LABEL_UUID_LEN] = {0};
    size_t len = 0;
    buffer [len++] = elemid;
    buffer [len++] = modelid;
    memcpy (buffer+len, uuid, QMESH_UUID_SIZE_OCTETS);
    len += QMESH_UUID_SIZE_OCTETS;
    switch (op)
    {
    case QMESH_MODEL_SUB_EVENT_DELETE:
    case QMESH_MODEL_SUB_EVENT_DELETE_ALL:
        QmeshPsDeleteKeyData (QMESH_APP_PS_KEY_LABEL_UUID, ElemModelCompare, 
                             (uint16_t*)buffer, (uint16_t*)read_buf, len);
        break;                         
    case QMESH_MODEL_SUB_EVENT_ADD:
    case QMESH_MODEL_SUB_EVENT_OVERWRITE:
        if (QmeshPsUpdateKeyData (QMESH_APP_PS_KEY_LABEL_UUID, ElemModelCompare, 
                                  (uint16_t*)buffer, &read_buf, (uint16_t*)buffer, 
                                  &len) == QMESH_RESULT_PS_NO_ELEMENT)
        {                              
            QmeshPsAddKeyData (QMESH_APP_PS_KEY_LABEL_UUID, (uint16_t*)&buffer, &len);
        }
        break;
    }
}

void NVMHandle_ProvState (uint8_t * buffer, uint8_t buf_len)
{
//    g_app_context->device_provisioned = ((uint8_t*)buffer)[0];
}

void NVMHandle_PrimAddress (uint8_t * buffer, uint8_t buf_len)
{
    memcpy ((uint8_t*)&dev_comp->elements[0].element_data->unicast_addr, (uint8_t*)buffer, 2);
}

void NVMHandle_SubNetIdentity (uint8_t * buffer, uint8_t buf_len)
{
    APP_NVM_IDENTITY_DATA_T data;
    uint8_t i;
    uint8_t index = 0;

    memset(g_app_context->subnet_data, 0x00, sizeof(g_app_context->subnet_data));

    for (i = 0; i < buf_len; i++)
    {
        memcpy ((uint8_t*)&data, buffer + index, sizeof (APP_NVM_IDENTITY_DATA_T));
        index += sizeof (APP_NVM_IDENTITY_DATA_T);
        g_app_context->subnet_data[i].netkey_idx.prov_net_idx = data.netkey_idx.prov_net_idx; 
        g_app_context->subnet_data[i].netkey_idx.key_idx = data.netkey_idx.key_idx; 
        g_app_context->subnet_data[i].node_identity = ((dev_comp->feature_mask & PROXY_FEATURE_MASK) == PROXY_FEATURE_MASK)?
                                                     (QMESH_NODE_IDENTITY_STOPPED):
                                                     (QMESH_NODE_IDENTITY_NOT_SUPPORTED);
        g_app_context->subnet_data[i].valid = true;
    }
}

void NVMHandle_LabelUUID (uint8_t * buffer, uint8_t buf_len)
{
    uint8_t elemid;
    uint8_t modelid;
    QMESH_LABEL_UUID_T label_uuid;
    uint16_t subaddr;
    uint8_t i,j;
    uint8_t index = 0;
    for (i = 0; i < buf_len; i++)
    {
        elemid = buffer [index++]; 
        modelid = buffer [index++]; 
        memcpy ((uint8_t*)&label_uuid, buffer + index, sizeof (subaddr));
        index += sizeof (QMESH_LABEL_UUID_T);
        /*Send UUID to mesh lib and store the returned index as subaddr*/
        if (QmeshSetVirtualAddress (0, label_uuid, TRUE, &subaddr) == QMESH_RESULT_SUCCESS)
        {
            uint16_t * subs_list;
            subs_list = (uint16_t*)dev_comp->elements[elemid].element_data->model_data[modelid].subs_list;
            /*Find vacant spot in subscription list and add this address*/
            for (j = 0; j<QMESH_MAX_SUB_ADDRS_LIST; j++)
            {
                if (subs_list [j] == QMESH_UNASSIGNED_ADDRESS);
                {
                    subs_list [j] = subaddr;
                    break;
                }
            }
        }
    }
}
    
/*Handle the data read from NVM for key QMESH_APP_PS_KEY_ELEM_SEQ_NUM*/
void NVMHandle_ElementSequenceNumber (uint8_t * buffer, uint8_t buf_len)
{
    uint32_t sequence;
    uint8_t i = 0;
    uint8_t index = 0;
    for (i = 0; i < dev_comp->num_elements; i++)
    {
        memcpy ((uint8_t*)&sequence, buffer + index, sizeof (sequence));
        index += sizeof (sequence);
        /*save in composition data*/
        dev_comp->elements[i].element_data->seq_num = sequence;
    }
}

void NVMSerialize_ModelPublicationStatus(uint8_t *buffer, uint8_t elementid, uint8_t modelid, 
                                         QMESH_MODEL_PUBLISH_STATE_T *pub_state, size_t *length)
{
    uint8_t len = 0;
    buffer [len++] = elementid;
    buffer [len++] = modelid;
    memcpy (buffer+len, &pub_state->app_key_idx, sizeof (pub_state->app_key_idx));
    len += sizeof (pub_state->app_key_idx);
    memcpy (buffer+len, &pub_state->publish_addr, sizeof (pub_state->publish_addr));
    len += sizeof (pub_state->publish_addr);
    if (pub_state->publish_uuid != NULL)
        memcpy (buffer+len, pub_state->publish_uuid, QMESH_UUID_SIZE_OCTETS);
    else
        memset (buffer+len, 0, QMESH_UUID_SIZE_OCTETS);
    
    len += QMESH_UUID_SIZE_OCTETS;
    memcpy (buffer+len, &pub_state->use_frnd_key, sizeof (pub_state->use_frnd_key));
    len += sizeof (pub_state->use_frnd_key);
    memcpy (buffer+len, &pub_state->publish_ttl, sizeof (pub_state->publish_ttl));
    len += sizeof (pub_state->publish_ttl);
    memcpy (buffer+len, &pub_state->publish_period, sizeof (QMESH_PUBLISH_PERIOD_T));
    len += sizeof (QMESH_PUBLISH_PERIOD_T);
    memcpy (buffer+len, &pub_state->retransmit_param, sizeof (QMESH_PUBLISH_RETRANSMIT_STATE_T));
    len += sizeof (QMESH_PUBLISH_RETRANSMIT_STATE_T);
    *length = len;
}

void NVMDeserialize_ModelPublicationStatus(uint8_t *buffer)
{
    uint16_t len = 0;
    uint8_t elementid, modelid;
    QMESH_MODEL_PUBLISH_STATE_T *pub_state;
    elementid = buffer [len++];
    modelid = buffer [len++];
    pub_state = dev_comp->elements[elementid].element_data->model_data[modelid].publish_state;
    memcpy (&pub_state->app_key_idx, buffer+len, sizeof (pub_state->app_key_idx));
    len += sizeof (pub_state->app_key_idx);
    memcpy (&pub_state->publish_addr, buffer+len, sizeof (pub_state->publish_addr));
    len += sizeof (pub_state->publish_addr);
    pub_state->publish_uuid = (const QMESH_LABEL_UUID_T*)malloc (sizeof(QMESH_LABEL_UUID_T));
    if (!pub_state->publish_uuid)
        return;
    memcpy (pub_state->publish_uuid, buffer+len, QMESH_UUID_SIZE_OCTETS);
    len += QMESH_UUID_SIZE_OCTETS;
    memcpy (&pub_state->use_frnd_key, buffer+len, sizeof (pub_state->use_frnd_key));
    len += sizeof (pub_state->use_frnd_key);
    memcpy (&pub_state->publish_ttl, buffer+len, sizeof (pub_state->publish_ttl));
    len += sizeof (pub_state->publish_ttl);
    memcpy (&pub_state->publish_period,buffer+len, sizeof (QMESH_PUBLISH_PERIOD_T));
    len += sizeof (QMESH_PUBLISH_PERIOD_T);
    memcpy (&pub_state->retransmit_param, buffer+len, sizeof (QMESH_PUBLISH_RETRANSMIT_STATE_T));
}

/*Handle the data read from NVM for key QMESH_APP_PS_KEY_MODEL_PUB_STATUS*/
void NVMHandle_ModelPublicationStatus (uint8_t * buffer, uint8_t buf_len)
{
    int i;
    for (i=0; i<buf_len; i++)
    {
        NVMDeserialize_ModelPublicationStatus (buffer + (i*PS_PUB_STATUS_LEN));
    }
}

/*Handle the data read from NVM for key QMESH_APP_PS_KEY_MODEL_SUBSCRIPTION*/
void NVMHandle_ModelSubscription (uint8_t * buffer, uint8_t buf_len)
{
    uint8_t elemid;
    uint8_t modelid;
    uint16_t subaddr;
    uint8_t i;
    uint8_t index = 0;
    for (i = 0; i < buf_len; i++)
    {
        uint16_t * subs_list;
        elemid = buffer [index++]; 
        modelid = buffer [index++]; 
        memcpy ((uint8_t*)&subaddr, buffer + index, sizeof (subaddr));
        index += sizeof (subaddr);
        /*save in composition data*/
        subs_list = (uint16_t*)dev_comp->elements[elemid].element_data->model_data[modelid].subs_list;
        /*Find vacant spot in subscription list and add this address*/
        QmeshAddToList(subs_list, subaddr);
    }
}

/*Handle the data read from NVM for key QMESH_APP_PS_KEY_APPKEY_BIND_LIST*/
void NVMHandle_ModelAppKey (uint8_t * buffer, uint8_t buf_len)
{
    uint8_t elemid;
    uint8_t modelid;
    uint16_t app_key;
    uint8_t i;
    uint8_t index = 0;
    for (i = 0; i < buf_len; i++)
    {
        uint16_t * app_key_list;
        elemid = buffer [index++]; 
        modelid = buffer [index++]; 
        memcpy ((uint8_t*)&app_key, buffer + index, sizeof (app_key));
        index += sizeof (app_key);
        /*save in composition data*/
        app_key_list = (uint16_t*)dev_comp->elements[elemid].element_data->model_data[modelid].app_key_list;
         /*Find vacant spot in subscription list and add this address*/
        QmeshAddToList(app_key_list, app_key);
    }
}

/*Initialize the NVM. If this is the first run new keys are created else read the key data*/
QMESH_RESULT_T NVMInit (QMESHAPP_CONTEXT_T *app_context)
{
    bool initialized  = false;
    QMESH_RESULT_T status = QMESH_RESULT_FAILURE;

    if (QmeshPsInit () == QMESH_RESULT_SUCCESS)
    {
        initialized = true;
        if (QmeshPsReadyForAccess() == QMESH_RESULT_SUCCESS)
        {
            if (QmeshPsIsKeyPresent (QMESH_APP_PS_KEY_PROV_STATE) == QMESH_RESULT_SUCCESS)
            {
                /*Read the keys data from NVM*/
                status = NVMReadAppData (app_context);
            }
            else
            {
                /*First run. Add the keys to NVM*/
                status = NVMAddKeys ();
            }
        }
    }

    if (status != QMESH_RESULT_SUCCESS && initialized == true)
        QmeshPsDeInit ();

    return status;
}

/*Create Application keys in NVM*/
QMESH_RESULT_T NVMAddKeys ()
{
    QCLI_LOGI (mesh_group, "NVM Initialization on first run\n");
    int i;
    for (i=0; i<QMESH_APP_PS_KEY_COUNT; i++)
    {
        if (QmeshPsAddKey (&application_keys [i].key) == QMESH_RESULT_FAILURE)
            return QMESH_RESULT_FAILURE;
    }

    /* Add keys for models */
    return NVMAddModelKeys ();
}

/*Print NVM Data*/
static void printNVMBuffer (uint8_t key, uint8_t count, uint16_t size, uint8_t * buffer)
{
    uint16_t entry, data;
    uint16_t pos = 0;
    uint8_t strpos;
    char * str = malloc (4 * size);
    QCLI_LOGI (mesh_group, "NVM: Key:0x%x, Num of Entries:%d\n", key, count);
    for (entry = 0; entry < count; entry++)
    {
        for (data=0, strpos=0; data < size; data++)
        {
            strpos+= snprintf (&str[strpos], 4, "%02x ", buffer [pos++]);
        }
        
        /*snprintf (&str[strpos], 1, "\n");*/
        QCLI_LOGI (mesh_group, "\t\tEntry: %d, Value: %s\n", entry, str);                        
    }

    free (str);
}        

/*
 *Read a key from NVM. Returns keyheader and an allocated buffer with complete data.
 *Caller should free the buffer after use.
 */
QMESH_RESULT_T NVMReadKey (uint8_t key, uint8_t **out_buf, QMESH_PS_KEY_HEADER_INFO_T * keyhdr)
{
    QMESH_RESULT_T status;
    size_t len;
    if ((status = QmeshPsKeyGetHeaderInfo (key, keyhdr)) == QMESH_RESULT_SUCCESS)
    {
        if (keyhdr->num_entries != 0)
        {
            len = keyhdr->entry_size * keyhdr->num_entries;
            *out_buf = malloc (len);
            if (!*out_buf) 
            {
                QCLI_LOGI (mesh_group, "%s: MALLOC failure (size: %d)!\n", 
                           __FUNCTION__, len);
                return QMESH_RESULT_FAILURE;
            }

            if ((status = QmeshPsReadKeyDataMultiple (key, 0, 
                                                      keyhdr->num_entries,
                                                      (uint16_t*)*out_buf, 
                                                      &len)) != QMESH_RESULT_SUCCESS)
            {
                QCLI_LOGI (mesh_group, "NVM Read failed for Key:%d. Status:%d\n",
                           __FUNCTION__, key, status);
                free (*out_buf);
                return QMESH_RESULT_FAILURE;
            }
        }
    }
    else
        QCLI_LOGI (mesh_group, "NVM header read failed for Key:%d. Status:%d\n",
                   key, status);

    return status;
}

/*Print all Application's keys*/
void NVMPrintAllKeys ()
{
    uint8_t * out_buf = NULL;
    QMESH_PS_KEY_HEADER_INFO_T keyhdr;
    int i;
    /*App Keys.*/
    for (i=0; i<QMESH_APP_PS_KEY_COUNT; i++)
    {
        const QMESH_PS_KEY_INFO_T * key = &application_keys [i].key;
        if (NVMReadKey (key->key, &out_buf, &keyhdr) == QMESH_RESULT_SUCCESS)
        {
            printNVMBuffer (key->key, keyhdr.num_entries, keyhdr.entry_size, out_buf);
            free (out_buf);
            out_buf = NULL;
        }
    }
    /*TODO. Add for Model and Stack Keys*/
}

QmeshResult NVMReadModelData (void)
{
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_MODEL_COMMON,
                  "NVMReadModelData: Read Model Data From NVM\n");

    NvmReadOnOffModel(&g_onoff_model_context);
    NvmReadGenericModelLevel();
    NvmReadGenericPowerOnOffState(&g_pwronoff_context);
    NvmReadGenericDefaultTimeTransitionServerState(&g_gen_dtt_context);
    NvmReadLightnessServerState(&g_lightness_server_context);
    NvmReadReadHSLServerState(&g_hsl_server_context);
    NvmReadModelLightHueServerState(&g_hsl_hue_server_context);
    NvmReadSaturationServerState(&g_hsl_sat_server_context);

#ifdef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE
    NvmReadGenericPowerLevelServerState(&g_pwr_lvl_context);
#endif
    return QMESH_RESULT_SUCCESS;
}

/*Read Application Keys from NVM*/
QMESH_RESULT_T NVMReadAppData (QMESHAPP_CONTEXT_T *app_context)
{
    QMESH_RESULT_T status;
    uint8_t * out_buf = NULL;
    QMESH_PS_KEY_HEADER_INFO_T keyhdr;
    int i;

    g_app_context = app_context;
    for (i=0; i<QMESH_APP_PS_KEY_COUNT; i++)
    {
        const QMESH_PS_KEY_INFO_T * key = &application_keys [i].key;
        if ((status = NVMReadKey (key->key, &out_buf, &keyhdr)) == QMESH_RESULT_SUCCESS)
        {
            /*printNVMBuffer (key->key, keyhdr.num_entries, keyhdr.entry_size, out_buf);*/
            application_keys [i].handler (out_buf, keyhdr.num_entries);
            free (out_buf);
            out_buf = NULL;
        }
        else
            return status;
    }

    /* Read model data */
    NVMReadModelData();
#if 0
    if ((app_context->device_provisioned) && (app_context->role == NODE_ROLE))
    {
        StoreElementSequenceNo();
    }
#endif
    return QMESH_RESULT_SUCCESS;
}
#endif
