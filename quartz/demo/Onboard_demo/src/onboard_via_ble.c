/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "ble_util.h"
#include "onboard_ble.h"
#include "onboard.h"
#include "wifi_util.h"
#include "log_util.h"

#if defined BOARD_SUPPORTS_WIFI && (ONBOARDED_OPERATION_MODE & OPERATION_MODE_WIFI)
qurt_signal_t wlan_sigevent;


/**
 * @func : Initialize_wifi()
 * @Desc : Initialize wlan interface to onboard
 */
int32_t Initialize_wifi()
{
    if (SUCCESS != qurt_signal_init(&wlan_sigevent))
    {
        LOG_ERROR("Signal init event failed\n");
        return FAILURE;
    }

    if (SUCCESS != wlan_enable(WLAN_NUM_OF_DEVICES, &wlan_sigevent))
    {
        LOG_ERROR("Wlan enable failed\n");
        return FAILURE;
    }

    return SUCCESS;
}
#endif

/**
 * @func : Register_services()
 * @Desc : Register the wifi and zigbee services
 */
static int32_t Register_services()
{
#ifndef OFFLINE
#if defined BOARD_SUPPORTS_WIFI && (ONBOARDED_OPERATION_MODE & OPERATION_MODE_WIFI)
    if (Initialize_wifi() != SUCCESS)
    {
        LOG_ERROR("BLE: error Initializing WiFi interface\n");
        return FAILURE;
    }
    if (Register_wifi_service() != SUCCESS)
    {
        LOG_ERROR("BLE: error register_onboard_service()\n");
        return FAILURE;
    }
#endif
#endif

#if (ONBOARDED_OPERATION_MODE & OPERATION_MODE_ZIGBEE)
    if (Register_zigbee_service() != SUCCESS)
    {
        LOG_ERROR("BLE: error register_onboard_service()\n");
        return FAILURE;
    }
#endif

#if (ONBOARDED_OPERATION_MODE & OPERATION_MODE_THREAD)
	if (Register_thread_service() != SUCCESS)
    {
        LOG_ERROR("BLE: error register_onboard_service()\n");
        return FAILURE;
    }
#endif
#ifdef OFFLINE
    if (Register_offline_service() != SUCCESS)
    {
        LOG_ERROR("BLE: error register_onboard_service()\n");
        return FAILURE;
    }
#endif

    return SUCCESS;
}

int32_t Start_onboard_via_ble()
{
    if (AdvertiseLE(1) != SUCCESS)
    {
        LOG_ERROR("BLE: error AdvertiseLE()\n");
        return FAILURE;
    }

    return SUCCESS;
}


/**
 * @func : InitializeOnboarding
 * @Desc : This function is the Entry Point
 *         Initializes BLE Stack and also registers the Onboard services
 */
int32_t Initialize_onboard_via_ble()
{
    if (InitializeBluetooth() != SUCCESS)
    {
        LOG_ERROR("BLE: Error InitializeBluetooth()\n");
        return FAILURE;
    }

    if (Register_services() != SUCCESS)
    {
        LOG_ERROR("BLE: Error Registering Services()\n");
        return FAILURE;
    }

    return SUCCESS;
}
