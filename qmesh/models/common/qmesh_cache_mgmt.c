/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_cache_mgmt.c
 *  \brief defines and functions for Mode message cache management
 *
 *   This file contains the implementation on 6 seconds window cache management. As per Model
 *   specification, any duplicate messages received within last 6 seconds should be dropped.
 */
/******************************************************************************/
#include "qmesh_cache_mgmt.h"
#include "qmesh_model_config.h"
#include "qmesh_model_common.h"

/* This can be moved to other places later if required */
int16_t g_current_index = 0; /* index where last message added */
/* Model message cache */
static QMESH_MODEL_CACHE_NODE_T g_message_cache[QMESH_MODEL_MSG_CACHE_SIZE] = {};

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshModelInitMsgCache
 *
 *  DESCRIPTION
 *      Initialises the model message cache
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
void QmeshModelInitMsgCache (void)
{
#ifdef QMESH_POOL_BASED_MEM_MGR
    /* Allocate mm pools */
    static const QMESH_MM_POOL_CONFIG_T models_pool_config[] = 
    {
         QMESH_MM_POOLS_ENTRY(MODEL_CACHE_ENTRY_SIZE, NUM_DELAY_CACHE_ENTRIES)
    };

    QMESH_RESULT_T result = QmeshMemPoolsConfigure(QMESH_MM_SECTION_MODEL_LAYER,  models_pool_config,
                                                   sizeof( models_pool_config) / sizeof( models_pool_config[0]));
    if(result!= QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE, "%s %d QmeshMemPoolsConfigure Failed\n",
                      __FUNCTION__, __LINE__);
        return;
    }
#endif /* QMESH_POOL_BASED_MEM_MGR */

    g_current_index = 0; /* Set current index to 0 */        
    model_timer_ghdl = QmeshTimerCreateGroup(QMESH_NUM_MODEL_SOFT_TIMERS);
    if(model_timer_ghdl == QMESH_TIMER_INVALID_GROUP_HANDLE)
    {
        DEBUG_MODEL_INFO(DBUG_MODEL_MASK_DELAY_CACHE,"\r\n Soft timer Model group creation failed ");
        return;
    }
    QmeshMemSet (g_message_cache, 0, sizeof (g_message_cache));
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE, "%s %d ------------\n",
                      __FUNCTION__, __LINE__);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshModelCacheAddMsg
 *
 *  DESCRIPTION
 *      The function adds the 'msg' into the cache if the 'msg' is not found in the 6 seconds timestamp
 * window. If the 'msg' is not found, the 'msg' will be added/overwriten in the next slot. The search is
 * limited to messages with timestamp not greater than 6 seconds old. This reduces the search time.
 *
 * If 'transactional' is TRUE, the message will be added into the cache in the next slot even if a message 
 * exists with same TID in the last 6 seconds window. This is required for 'Transactional' messages like
 * 'Generic Delta Set' which will have same TID but 'level' param will have delta change. Such message
 * need to be processed always.
 *
 *  RETURNS/MODIFIES
 *      returns TRUE if message is adde into the cache
 *                  FALSE if message is already present or othe failure
 *
 *----------------------------------------------------------------------------*/
bool QmeshModelCacheAddMsg (QMESH_MODEL_MSG_COMMON_T *msg,
                                     bool transactional)
{
    int16_t i;
    uint16_t curTime[3] = {0};
    uint16_t temp[3] = {0};
    uint32_t diffSec = 0;
    
    /* Get the current time in milliseconds */
    QmeshGetCurrentTimeInMs((uint16_t *)&curTime);

    /* Check if the slot pointed by 'cur' is empty */
    if (0 == QmeshMemCmp (temp, g_message_cache[g_current_index].ts, sizeof (temp)))
    {
        /* Add the message into the cache */
        QmeshMemCpy (&g_message_cache[g_current_index].msg, msg,
                   sizeof (QMESH_MODEL_MSG_COMMON_T));
        QmeshMemCpy (g_message_cache[g_current_index].ts, curTime, sizeof (curTime));
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "%s %d Msg added into the cache at index=%d\n", __FUNCTION__, __LINE__,
                          g_current_index);
        return TRUE;
    }

    /* If the message is transactional, add into the cache */
    if (transactional)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "%s %d transactional or empty - jumping to 'add'  \n", __FUNCTION__, __LINE__);
        goto add;
    }

    /* Search if the 'msg' exists in the cache in the last 6 seconds - that is search messages 
         with  '(current_time-6)>=timestamp' */
    /* Check if message pointed to by 'cur' index matches */
    {
        /* Calculate time difference between curret time and the timestamp of the message */
        QmeshTimeSub (temp, curTime, g_message_cache[g_current_index].ts);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "%s %d current time =0x%04x:0x%04x:0x%04x\n", __FUNCTION__, __LINE__,
                          curTime[2], curTime[1], curTime[0]);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "%s %d old time =0x%04x:0x%04x:0x%04x\n", __FUNCTION__, __LINE__,
                          g_message_cache[g_current_index].ts[2],
                          g_message_cache[g_current_index].ts[1],
                          g_message_cache[g_current_index].ts[0]);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "%s %d Diff time =0x%04x:0x%04x:0x%04x\n", __FUNCTION__, __LINE__, temp[2],
                          temp[1], temp[0]);
        /* Conver the difference from milli seconds into 'seconds' */
        diffSec = (((uint32_t)temp[1] << 16) | (uint32_t)temp[0]) /1000;
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "%s %d Time stamp difference=%d \n", __FUNCTION__, __LINE__, diffSec);

        /* timestamp is older than 6 seconds - stop searching since any message beyond this will be 
             older than this message */
        if (diffSec >= 6)
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                              "%s %d Message with timestamp older than 6 seconds reached - Add the message \n",
                              __FUNCTION__, __LINE__);
            goto add;
        }

        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "%s %d Cur.msg, elm=0x%x, opcode=0x%08x, src=0x%x, dst=0x%x, tid=0x%x \n",
                          __FUNCTION__, __LINE__,
                          g_message_cache[g_current_index].msg.elem_id,
                          g_message_cache[g_current_index].msg.opcode,
                          g_message_cache[g_current_index].msg.src,
                          g_message_cache[g_current_index].msg.dst,
                          g_message_cache[g_current_index].msg.tid);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "%s %d Cur.msg, elm=0x%x, opcode=0x%08x, src=0x%x, dst=0x%x, tid=0x%x \n",
                          __FUNCTION__, __LINE__,
                          msg->elem_id, msg->opcode, msg->src, msg->dst, msg->tid);

        /* Check if the message is same */
        if (0 == QmeshMemCmp (&g_message_cache[g_current_index].msg, msg,
                            sizeof (QMESH_MODEL_MSG_COMMON_T)))
        {
            /* Message exists in the cache - do not add into the cache */
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
              "%s %d Message exists at index=%d and msg not older than 6 seconds - Drop the message\n",
              __FUNCTION__, __LINE__, g_current_index);
            return FALSE;
        }
    }
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                      "%s %d ----- start searching backwards ----------\n", __FUNCTION__, __LINE__);
    /* Start searching from 'cur' -1 position and search backwards until 'cur' positoin is reached */
    i = g_current_index - 1;

    /* Wrap the index */
    if (i < 0)
    {
        i = QMESH_MODEL_MSG_CACHE_SIZE - 1;
    }

    /* Search if the 'msg' exists in the cache in the last 6 seconds */
    while (i != g_current_index)
    {
        QmeshTimeSub (temp, curTime, g_message_cache[i].ts);
        diffSec = (((uint32_t)temp[1] << 16) | (uint32_t)temp[0]) /1000;
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "%s %d Time stamp difference=%d \n", __FUNCTION__, __LINE__, diffSec);

        /* Timestamp of the message is less than 6 seconds */
        if (diffSec < 6)
        {
            /* Compare the incoming message */
            if (0 == QmeshMemCmp (&g_message_cache[i].msg, msg,
                                sizeof (QMESH_MODEL_MSG_COMMON_T)))
            {
                /* Message exists */
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                                  "%s %d Msg exists in 6 sec window at i=%d\n", __FUNCTION__, __LINE__, i);
                return FALSE;
            }
        }
        else
        {
            /* Messages with timestamp older than 6 secs - stop searching and add the message */
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                              "%s %d Msg not found in 6 sec window - Add the message \n", __FUNCTION__,
                              __LINE__);
            break;
        }

        i --;

        /* Wrap the index */
        if (i < 0)
        {
            i = QMESH_MODEL_MSG_CACHE_SIZE - 1;
        }
    }

add:
    /* Message does not exist - add message into the cache in the next slot */
    g_current_index = (g_current_index + 1) % QMESH_MODEL_MSG_CACHE_SIZE;
    QmeshMemCpy (& (g_message_cache[g_current_index].msg), msg,
               sizeof (QMESH_MODEL_MSG_COMMON_T));
    QmeshMemCpy (g_message_cache[g_current_index].ts, curTime, sizeof (curTime));
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                      "%s %d Message added into the cache at index=%d \n", __FUNCTION__, __LINE__,
                      g_current_index);
    return TRUE;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      QmeshModelCacheDump
 *
 *  DESCRIPTION
 *      Prints all the model messages in the cache  from latest to oldest message.
 *
 *  RETURNS/MODIFIES
 *      None
 *
 *----------------------------------------------------------------------------*/
void QmeshModelCacheDump (void)
{
    int16_t i = 0;
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE, "QmeshModelCacheDump \n");
    i = g_current_index;

    /* Condition added for Klocwork */
    if (i >= 0)
    {
        /* Message pointing by 'g_current_index' */
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE, "Timestamp: %ld:%ld:%ld\n",
                          g_message_cache[i].ts[2], g_message_cache[i].ts[1], g_message_cache[i].ts[0]);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "Element Addr=%d, Opcode=0x%x, Src=0x%x, Dst=ox%x, TID=0x%x\n",
                          g_message_cache[i].msg.elem_id,
                          g_message_cache[i].msg.opcode, g_message_cache[i].msg.src,
                          g_message_cache[i].msg.dst, g_message_cache[i].msg.tid);
    }

    /* Wrap the index */
    if ((g_current_index - 1) < 0)
    {
        i = QMESH_MODEL_MSG_CACHE_SIZE - 1;
    }
    else
    {
        i = g_current_index - 1;
    }

    while (i != g_current_index)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE, "Timestamp: %ld:%ld:%ld\n",
                          g_message_cache[i].ts[2], g_message_cache[i].ts[1], g_message_cache[i].ts[0]);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "Element Addr=%d, Opcode=0x%x, Src=0x%x, Dst=ox%x, TID=0x%x\n",
                          g_message_cache[i].msg.elem_id,
                          g_message_cache[i].msg.opcode, g_message_cache[i].msg.src,
                          g_message_cache[i].msg.dst, g_message_cache[i].msg.tid);
        i --;

        /* Wrap the index */
        if (i < 0)
        {
            i = QMESH_MODEL_MSG_CACHE_SIZE - 1;
        }
    }
}
