/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <qapi/com_dtypes.h>

#include "zigbee_util.h"
#include "onboard.h"
#include "led_utils.h"

/**
 * @func : Start_ZB_Router
 * @Param: Master_key is the security key to be verified at the time of joining the boards
 * @Desc : function will initialize the zigbee stack
 *         Device mode will be set to Router to join the formed network.(i.e Co-ordinator)
 */
int32_t Start_ZB_Router(char *Master_key)
{
    qapi_Status_t      Result;

    /*Initializing zigbee stack */
    Result = Zigbee_Initialize();
    if (Result != SUCCESS)
    {
        LOG_ERROR( "In Start_ZB_Router Code: Failed to Initialize Zigbee Stack\n");
        return FAILURE;
    }
    else
    {
		/** Zigbee LED Blink */
        R15_4_LED_CONFIG(1,50);
    }

    /** Setting Extended Address. Passing BLE mac addr @Param*/	
    Result = Zigbee_SetExtAddress(GetExtenAddr());
    if (Result != SUCCESS)
    {
        LOG_ERROR("Failed to Set Extended Address to Router\n");
        return FAILURE;
    }

    /** Getting Extended Address */
    Zigbee_GetAddresses();

    /** Joining Network, @Param enable/disable router, security, Rejoin/join mode, channel mask and Link Key*/
    Result = Zigbee_Join(ENABLE_ROUTER, ZIGBEE_SECURITY, ZIGBEE_REJOIN, ZIGBEE_CHANNEL_MASK, Master_key);
    if (Result != SUCCESS)
    {
        LOG_ERROR("Failed to Join the network\n");
        return FAILURE;
    }

    LOG_INFO("Router Successfully Joined Zigbee network\n");
    return SUCCESS;
}
