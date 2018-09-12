/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "thread_util.h"

/* : Start_Thread_Boarder_Router
 * @Param: PassPhrase is the security key to be verified at the time of joining the boards
 * @Desc : Function will initialize the Zigbee stack and also,
 *         sets the device in Zigbee Co-ordinator Mode
 */
int32_t Start_Thread_Boarder_Router(char *PassPhrase)
{
    uint32_t Result;
    uint32_t Device_Config = JOINER_ROUTER;
    uint32_t TimeOut = JOINER_TIMEOUT;
    uint64_t ExtAddr = JOINER_EXTADDR;

    /** Initializing Joining Router */
    Result = Thread_Initialize(uint32_t Device_Config);

    if (Result != SUCCESS)
    {
        LOG_ERROR("Failed to Initialize Thread Stack\n");
        return FAILURE;
    }

    /** Function to set up the default Network Data for this demo application */
    Result = Thread_UseDefaultInfo();

    if (Result != SUCCESS)
    {
        LOG_ERROR("Failed to use default configuration\n");
        return FAILURE;
    }

    /** Start the Thread Interface, connecting or starting a network */
    Result = Thread_Interface_Start();

    if (Result != SUCCESS)
    {
        LOG_ERROR("Failed to Start Thread Interface\n");
        return FAILURE;
    }

    /** Start the Commissioner role on this device */
    Result = Thread_MeshCoP_CommissionerStart();

    if (Result != SUCCESS)
    {
        LOG_ERROR("Failed to Start Commissioner role\n");
        return FAILURE;
    }

    /** Add a joining device information to the Steering Information */
    Result = Thread_MeshCoP_CommissionerAddJoiner(PassPhrase, ExtAddr, TimeOut);

    if (Result != SUCCESS)
    {
        LOG_ERROR("Failed to add joining device information\n");
        return FAILURE;
    }

    return SUCCESS;
}
