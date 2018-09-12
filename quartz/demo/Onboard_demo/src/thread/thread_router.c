/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "thread_util.h"
#include "log_util.h"
#include "qapi_types.h"

/* : Start_Thread_Joiner
 * @Param: Passphrase is the security key to be verified at the time of joining the boards
 * @Desc : Function will initialize the Thread stack and also,
 *         sets the device in Thread Joiner mode.
 */
int32_t Start_Thread_Router(const char *PassPhrase)
{
    uint32_t Result;
    uint32_t Device_Config = JOINER;
    LOG_INFO("%s\n",__func__);

    /** Initializing Joining Router */
    Result = Thread_Initialize(Device_Config);

    if (Result != SUCCESS)
    {
        LOG_ERROR("Failed to Initialize Thread Stack\n");
        return FAILURE;
    }

    /** Blinking R15_4 LED after Thread initialization */
    R15_4_LED_CONFIG(1,50);


    qurt_thread_sleep(500);

    /** Attempts to commission onto an existing Thread network */
    Result =  Thread_MeshCoP_JoinerStart(PassPhrase);
    if (Result != SUCCESS)
    {
        LOG_ERROR("Failed to join thread network\n");
        return FAILURE;
    }

    qurt_thread_sleep(1500);

    /** Start the Thread Interface, connecting or starting a network */
    Result = Thread_Interface_Start();

    if (Result != SUCCESS)
    {
        LOG_ERROR("Failed to Start Thread Interface\n");
        return FAILURE;
    }

    /** Constantly glowing R15_4 LED after Joiner connecting to Border ROuter **/
    R15_4_LED_CONFIG(1,100);

    return SUCCESS;
}
