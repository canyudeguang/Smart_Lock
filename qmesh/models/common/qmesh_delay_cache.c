/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 *****************************************************************************/
/*! \file delay_cache.c
 *  \brief Implements Delay cache
 *
 *   This file implements Delay cache.Delay cache is used for storing the
 *   messages which have non-zero delay.This cache stores all the message
 *   parameters and when delay expires, callback function associated with the
 *   message is called.
 */
/*****************************************************************************/

/*============================================================================*
 *  Local Header Files
 *============================================================================*/

#include "qmesh_cache_mgmt.h"

/*============================================================================*
 *  Private Data Types
 *============================================================================*/

/* Delay Cache Internal Data Type */
typedef struct
{
    uint8_t delay_cache_idx;
    uint8_t free_cache_head_idx;
    uint8_t free_cache_tail_idx;
    QMESH_MUTEX_T mutex;
} QMESH_DELAY_CACHE_INTERNAL_DATA_T;

/*============================================================================*
 *  Private Data
 *============================================================================*/

/* Delay Cache Internal Data  */
QMESH_DELAY_CACHE_INTERNAL_DATA_T g_dcache;

/* Delay Cache */
QMESH_DELAY_CACHE_INFO_T g_delay_cache[NUM_DELAY_CACHE_ENTRIES];

/* Free Index Cache */
uint8_t g_free_index[NUM_DELAY_CACHE_ENTRIES];

/*============================================================================*
 *  Private Function Prototypes
 *============================================================================*/

/* This function checks if free index head pointer is valid */
static bool isFreeIndexHeadValid (void);

/* This function checks if free index tail pointer is valid  */
static bool isFreeIndexTailValid (void);

/* This function initialises cache indexes */
static void initialiseCacheIndex (void);

/* This function reinitialises cache entry at a particular index */
static void reinitialiseCacheEntry (uint8_t index);

/* This function returns the oldest message index */
static uint8_t getOldestMessage (void);

/* This function returns the free index from free index cache */
static uint8_t getfromFreeIndexCache (void);

/* This function adds the free index to free index cache */
static void addtoFreeIndexCache (uint8_t index);

/* This function handles the delay cache timer expiry */
static void handleDelayTimerExpiry (QMESH_TIMER_HANDLE_T timerHandle, void *data);

/*============================================================================*
 *  Private Function Implementations
 *============================================================================*/

/*-----------------------------------------------------------------------------*
*  NAME
*      isFreeIndexHeadValid
*
*  DESCRIPTION
*      This function checks if free index head pointer is valid
*
*  RETURNS/MODIFIES
*      Boolean -TRUE/FALSE
*
*----------------------------------------------------------------------------*/
static bool isFreeIndexHeadValid (void)
{
    if ((g_dcache.free_cache_head_idx >= NUM_DELAY_CACHE_ENTRIES) ||
        (g_free_index[g_dcache.free_cache_head_idx] == INVALID_INDEX))
        return FALSE;
    else
        return TRUE;
}

/*-----------------------------------------------------------------------------*
*  NAME
*      isFreeIndexTailValid
*
*  DESCRIPTION
*      This function checks if free index tail pointer is valid
*
*  RETURNS/MODIFIES
*      Boolean -TRUE/FALSE
*
*----------------------------------------------------------------------------*/
static bool isFreeIndexTailValid (void)
{
    if (g_dcache.free_cache_tail_idx >= NUM_DELAY_CACHE_ENTRIES)
        return FALSE;
    else
        return TRUE;
}

/*-----------------------------------------------------------------------------*
*  NAME
*      initialiseCacheIndex
*
*  DESCRIPTION
*      This function initialises cache indexes
*
*  RETURNS/MODIFIES
*      Nothing
*
*----------------------------------------------------------------------------*/
static void initialiseCacheIndex (void)
{
    /* Initailse the free index cache to INVALID_INDEX */
    QmeshMemSet (g_free_index, INVALID_INDEX, NUM_DELAY_CACHE_ENTRIES);
    /* Set delay_cache_idx to 0 */
    g_dcache.delay_cache_idx = 0;
    /* Initailse the  free_cache_head_idx and free_cache_tail_idx to
     * INVALID_INDEX
     */
    g_dcache.free_cache_head_idx = INVALID_INDEX;
    g_dcache.free_cache_tail_idx = INVALID_INDEX;
}

/*-----------------------------------------------------------------------------*
*  NAME
*      reinitialiseCacheEntry
*
*  DESCRIPTION
*      This function reinitialises cache entry at a particular index
*
*  RETURNS/MODIFIES
*      Nothing
*
*----------------------------------------------------------------------------*/
static void reinitialiseCacheEntry (uint8_t index)
{
    /* Check if index is vaid */
    if (index <  NUM_DELAY_CACHE_ENTRIES)
    {
        /* Initialise the timer_id to QMESH_TIMER_INVALID_HANDLE */
        g_delay_cache[index].timer_id = QMESH_TIMER_INVALID_HANDLE;
        /* Set model conetxt to NULL */
        g_delay_cache[index].model_context = NULL;
        /* Set Model Callback function to NULL */
        g_delay_cache[index].fn_callback = NULL;
        /* Free the memory */
        QmeshFree (g_delay_cache[index].data.msg);
    }
}

/*-----------------------------------------------------------------------------*
*  NAME
*      getOldestMessage
*
*  DESCRIPTION
*      This function returns the oldest message index
*
*  RETURNS/MODIFIES
*      Index - uint8_t
*
*----------------------------------------------------------------------------*/
static uint8_t getOldestMessage (void)
{
    uint8_t index = 0, i;

    /* Search delay cache entries to retrieve the message with the oldest
     * timestamp
     */
    for (i = 0 ; i < NUM_DELAY_CACHE_ENTRIES ; i++)
    {
        /* Search for the oldest timestamp.If first argument is less than
         * second argument returns TRUE
         */
        if (QmeshTimeCmp (g_delay_cache[i].data.timestamp,
                          g_delay_cache[index].data.timestamp))
        {
            index = i;
        }
    }

    return index;
}

/*-----------------------------------------------------------------------------*
*  NAME
*      getfromFreeIndexCache
*
*  DESCRIPTION
*      This function returns the free index from free index cache
*
*  RETURNS/MODIFIES
*      Index - uint8_t
*
*----------------------------------------------------------------------------*/
static uint8_t getfromFreeIndexCache (void)
{
    uint8_t index = 0;
    /* Lock the mutex */
    QmeshMutexLock (&g_dcache.mutex);

    /* If free_cache_head_idx is invalid , this means delay cache is full.
     * Delete the oldest entry in delay cache
     */
    if (!isFreeIndexHeadValid())
    {
        /* Get the oldest index */
        index = getOldestMessage();
        /* Delete the timer associated with oldest timestamp */
        QmeshTimerDelete (&model_timer_ghdl,&g_delay_cache[index].timer_id);
        /* Reinitialise that cache index entry */
        reinitialiseCacheEntry (index);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "Oldest Index to replace:%d\n", index);
    }
    else
    {
        /* Take the free index stored at free_cache_head_idx */
        index = g_free_index[g_dcache.free_cache_head_idx];
        /* Make that index as busy */
        g_free_index[g_dcache.free_cache_head_idx] = INVALID_INDEX;
        /* Increment the free_cache_head_idx */
        g_dcache.free_cache_head_idx++;

        /* If free_cache_head_idx is greater than the maximum number of entries,
        * initialise the free_cache_head_idx to 0.
        */
        if (g_dcache.free_cache_head_idx >= NUM_DELAY_CACHE_ENTRIES)
            g_dcache.free_cache_head_idx = 0;

        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "Index to Replace:%d\n", index);
    }

    /* Unlock the mutex */
    QmeshMutexUnlock (&g_dcache.mutex);
    return index;
}

/*-----------------------------------------------------------------------------*
*  NAME
*      addtoFreeIndexCache
*
*  DESCRIPTION
*      This function adds the free index to free index cache
*
*  RETURNS/MODIFIES
*      Nothing
*
*----------------------------------------------------------------------------*/
static void addtoFreeIndexCache (uint8_t index)
{
    /* Check if index is vaid */
    if (index <  NUM_DELAY_CACHE_ENTRIES)
    {
        /* Lock the mutex */
        QmeshMutexLock (&g_dcache.mutex);

        /* Initialise free_cache_tail_idx and free_cache_head_idx to 0 */
        if (!isFreeIndexTailValid())
        {
            g_dcache.free_cache_tail_idx = 0;
            g_dcache.free_cache_head_idx = 0;
        }

        /* Store the free index at free_cache_tail_idx in free_index array */
        g_free_index[g_dcache.free_cache_tail_idx] = index;
        /* Increment the free_cache_tail_idx */
        g_dcache.free_cache_tail_idx++;

        /* If free_cache_tail_idx is greater than the maximum number of entries,
             * initialise the free_cache_tail_idx to 0.
         */
        if (g_dcache.free_cache_tail_idx >= NUM_DELAY_CACHE_ENTRIES)
        {
            g_dcache.free_cache_tail_idx = 0;
        }

        /* Store the free index at free_cache_tail_idx in free_index array */
        g_free_index[g_dcache.free_cache_tail_idx] = index;
        /* Increment the free_cache_tail_idx */
        g_dcache.free_cache_tail_idx++;
        /* Unlock the mutex */
        QmeshMutexUnlock (&g_dcache.mutex);
    }
}

/*-----------------------------------------------------------------------------*
*  NAME
*      handleDelayTimerExpiry
*
*  DESCRIPTION
*      This function handles the delay cache timer expiry
*
*  RETURNS/MODIFIES
*      Nothing
*
*----------------------------------------------------------------------------*/
static void handleDelayTimerExpiry (QMESH_TIMER_HANDLE_T timerHandle, void *data)
{
    /* Retrieve the index in delay cache for which timer expired */
    uint8_t index = * ((uint8_t *) (data));

    /* Check if index is vaid */
    if (index <  NUM_DELAY_CACHE_ENTRIES)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "Delay Cache Timer Expired\n");
        /* Delete the timer */
        QmeshTimerDelete (&model_timer_ghdl,&g_delay_cache[index].timer_id);
        
        /* Call the callback associated with that cache entry */
        g_delay_cache[index].fn_callback (&g_delay_cache[index].data,
                                          g_delay_cache[index].model_context);
        /* Reinitialise that cache index entry */
        reinitialiseCacheEntry (index);
        /* Add the index to free_index array */
        addtoFreeIndexCache (index);
    }
}

/*============================================================================*
 *  Public Function Implementations
 *============================================================================*/
/*-----------------------------------------------------------------------------*
*  NAME
*      QmeshAddMsgToDelayCache
*
*  DESCRIPTION
*      This function adds a new message to delay cache
*
*  RETURNS/MODIFIES
*      Nothing
*
*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshAddMsgToDelayCache (QMESH_MODEL_MSG_T *model_msg,
        modelCallbackFunction callbackFunction,
        void *context,
        const uint8_t *msg,
        uint16_t msg_len)
{
    uint8_t index;
    uint16_t current_time[3], delay_expiry[3], delay_48[3];
    uint32_t delay_duration;
    QMESH_MODEL_MSG_T *ptr_data;
    QMESH_RESULT_T result = QMESH_RESULT_SUCCESS;

    /* If delay cache is full, check free_index cache to retreive the free index
     * else take the index as delay_cache_idx for storing data
     */
    if (g_dcache.delay_cache_idx == INVALID_INDEX)
    {
        index = getfromFreeIndexCache();
    }
    else  /*  Take the index as delay_cache_idx for storing data */
    {
        /*  Take the index as delay_cache_idx for storing data */
        index = g_dcache.delay_cache_idx;
        /*  Increment the delay_cache_idx  */
        g_dcache.delay_cache_idx++;

        /*  Make the delay_cache_idx as INVALID_INDEX if cache is full  */
        if (g_dcache.delay_cache_idx == NUM_DELAY_CACHE_ENTRIES)
            g_dcache.delay_cache_idx = INVALID_INDEX;
    }

    if (index < NUM_DELAY_CACHE_ENTRIES)
    {
        /* Copy the model_msg received in delay cache */
        QmeshMemCpy (&g_delay_cache[index].data, model_msg, sizeof (QMESH_MODEL_MSG_T));
        /* Allocate the memory for variable model message */
        ptr_data = &g_delay_cache[index].data;
        ptr_data->msg =  (uint8_t *)QmeshMalloc (QMESH_MM_SECTION_MODEL_LAYER, msg_len);

        if (ptr_data->msg == NULL)
        {
            /* Add the index to free_index array */
            addtoFreeIndexCache (index);
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                              "Delay cache: Memory Allocation Failed");
            return QMESH_RESULT_FAILURE;
        }

        /* Copy the variable message received */
        QmeshMemCpy (ptr_data->msg, msg, msg_len);
        
        /* Copy the callback function associated with the message */
        g_delay_cache[index].fn_callback = callbackFunction;
        /* Copy the model context associated with the message */
        g_delay_cache[index].model_context = context;
        /* Copy the index */
        g_delay_cache[index].index = index;
        /* Calculate the delay */
        delay_duration = ((uint32_t)
                          (g_delay_cache[index].data.delay) * QMESH_MODEL_DELAY_RESOLUTION);
        delay_48[0] = (uint16_t) delay_duration;
        delay_48[1] = (uint16_t) (delay_duration >> 16);
        delay_48[2] = 0;
        /* Get the current time in ms */
        QmeshGetCurrentTimeInMs (current_time);
        /* Calculate the expected expiry time */
        QmeshTimeAdd (delay_expiry, delay_48, g_delay_cache[index].data.timestamp);
        /* Compare the current time with expected delay expiry time */
        bool res = QmeshTimeCmp (current_time, delay_expiry);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "Delay cache: Current Time %x %x %x\n",
                          current_time[2], current_time[1], current_time[0]);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "Delay cache: Res %d\n", res);
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "Delay cache: Delay Duration %4X and Stored Time %x %x %x\n",
                          delay_duration, g_delay_cache[index].data.timestamp[2],
                          g_delay_cache[index].data.timestamp[1],
                          g_delay_cache[index].data.timestamp[0]);

        /* If the current time is less than the expected delay expiry time, then
         * create the delay timer
         */
        if (res)
        {
            /* Create delay timer */
            QmeshTimeSub (delay_expiry, delay_expiry, current_time);
            delay_duration =
             ((uint32_t)delay_expiry[0]) | (((uint32_t)delay_expiry[1]) << 16);
             g_delay_cache[index].timer_id = QmeshTimerCreate(&model_timer_ghdl,
                                                       handleDelayTimerExpiry,
                                                       (void *)&g_delay_cache[index].index,
                                                       delay_duration);
            /* If timer creation failed, then reinitailse and free that
             * cache entry
             */
            if (g_delay_cache[index].timer_id == QMESH_TIMER_INVALID_HANDLE)
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                                  "Timer Creation Failed in delay cache\n");
                /* Reinitialise the cache entry */
                reinitialiseCacheEntry (index);
                /* Add the index to free_index array */
                addtoFreeIndexCache (index);
            }
            else
            {
                DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                                  "Delay cache: Timer Created for duration %04X\n",
                                  delay_duration);
            }
        }
        else /* If the current time is greater than equal to the expected delay
              * expiry time, then call the callback function , reinitailse and
              * free that cache entry */
        {
            DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                              "Call callback function\n");
            /* Call the associated callback function */
            callbackFunction (&g_delay_cache[index].data,
                              g_delay_cache[index].model_context);
            /* Reinitialise the cache entry */
            reinitialiseCacheEntry (index);
            /* Add the index to free_index array */
            addtoFreeIndexCache (index);
        }
    }
    else
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "Delay cache: Incorrect Index\n");
        return QMESH_RESULT_FAILURE;
    }

    return result;
}

/*-----------------------------------------------------------------------------*
*  NAME
*      QmeshInitDelayCache
*
*  DESCRIPTION
*      This function initialises the delay cache and free_index array indexes
*
*  RETURNS/MODIFIES
*      Nothing
*
*----------------------------------------------------------------------------*/
extern void QmeshInitDelayCache (void)
{
    /* Initialise cache indexes */
    initialiseCacheIndex();
    /* Create a mutex for cache update */
    DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                      "QmeshInitDelayCache:Creating mutex\n");

    if (QmeshMutexCreate (&g_dcache.mutex) != QMESH_RESULT_SUCCESS)
    {
        DEBUG_MODEL_INFO (DBUG_MODEL_MASK_DELAY_CACHE,
                          "QmeshInitDelayCache:Creating mutex Failed\n");
    }
}

/*-----------------------------------------------------------------------------*
*  NAME
*      QmeshDeInitDelayCache
*
*  DESCRIPTION
*      This function de-initialises the delay cache
*
*  RETURNS/MODIFIES
*      Nothing
*
*----------------------------------------------------------------------------*/
extern void QmeshDeInitDelayCache (void)
{
    /* Initialise cache indexes */
    initialiseCacheIndex();
    /* Destroy the mutex for cache update */
    QmeshMutexDestroy (&g_dcache.mutex);
}

