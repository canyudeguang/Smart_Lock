/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __LED_UTILS_H__
#define __LED_UTILS_H__

#include <qapi_pwm.h>

#ifdef BOARD_SUPPORTS_WIFI
#define LED_WLAN           0
#define LED_BLE            5 
#define LED_R15_4          7
#define LED_RED            6
#define LED_BLUE           1
#define LED_GREEN          2

#define WLAN_LED_CONFIG(f, d)       led_config(LED_WLAN, f, d)
#define BLE_LED_CONFIG(f, d)        led_config(LED_BLE, f, d)
#define R15_4_LED_CONFIG(f, d)      led_config(LED_R15_4, f, d)
#define RED_LED_CONFIG(f, d)        led_config(LED_RED, f, d)
#define BLUE_LED_CONFIG(f, d)       led_config(LED_BLUE, f, d)
#define GREEN_LED_CONFIG(f, d)      led_config(LED_GREEN, f, d)

#else

#define LED_BLE            5
#define LED_R15_4          0
#define LED_RED            6
#define LED_BLUE           1
#define LED_GREEN          2

#define WLAN_LED_CONFIG(f, d)       
#define BLE_LED_CONFIG(f, d)        led_config(LED_BLE, f, d)
#define R15_4_LED_CONFIG(f, d)      led_config(LED_R15_4, f, d)
#define RED_LED_CONFIG(f, d)        led_config(LED_RED, f, d)
#define BLUE_LED_CONFIG(f, d)       led_config(LED_BLUE, f, d)
#define GREEN_LED_CONFIG(f, d)      led_config(LED_GREEN, f, d)
#endif


int32_t led_initialize(void);
int32_t led_config(int32_t led_chan, int32_t freq_hz, int32_t duty_percent);
void led_deinitialize(void);

#endif
