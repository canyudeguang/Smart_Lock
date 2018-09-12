/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __ONBOARD_BLE_H__
#define __ONBOARD_BLE_H__


#include "qurt_types.h"
#include "qurt_signal.h"

/**
 *  Definitions Required for WiFi Services
 */
#define WIFI_SERVICE_UUID_CONSTANT                                        {0x18, 0x11}
#define WIFI_STATUS_CHARACTERISTIC_UUID_CONSTANT                          {0x19, 0x11}
#define WIFI_SSID_CHARACTERISTIC_UUID_CONSTANT                            {0x20, 0x11}
#define WIFI_SSID_CHARACTERISTIC_STR_CONSTANT                             "1120"
#define WIFI_PASSWD_CHARACTERISTIC_UUID_CONSTANT                          {0x21, 0x11}
#define WIFI_PASSWD_CHARACTERISTIC_STR_CONSTANT                           "1121"
#define WIFI_NOTIFY_CHARACTERISTIC_UUID_CONSTANT                          {0x22, 0x11}

#define WIFI_STR_NOTIFY_LENGTH                                            4
#define STR(a)                                                            #a
#define CONVERT_TO_STRING(a)                                              STR(a)

#define WIFI_STATUS_CREDITS_CHARACTERISTIC_ATTRIBUTE_OFFSET               2
#define WIFI_SSID_CREDITS_CHARACTERISTIC_ATTRIBUTE_OFFSET                 4
#define WIFI_PASSWD_CREDITS_CHARACTERISTIC_ATTRIBUTE_OFFSET               6
#define WIFI_NOTIFICATION_CREDITS_CHARACTERISTIC_ATTRIBUTE_OFFSET         8

#define QAPI_BLE_GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH          (QAPI_BLE_NON_ALIGNED_WORD_SIZE)
#define BLE_BUFFR_SIZE                                                    32


/**
 *    Definitions Required for Zigbee Services
 */

#define OP_MODE_SIZE                                                      3
#define LINK_KEY_SIZE                                                     16

#define ZIGBEE_SERVICE_UUID_CONSTANT                                      {0x23, 0x11}
#define ZIGBEE_STATUS_UUID_CONSTANT                                       {0x24, 0x11}
#define ZIGBEE_SUPPORT_MODE_UUID_CONSTANT                                 {0x25, 0x11}
#define ZIGBEE_OPERATE_MODE_UUID_CONSTANT                                 {0x26, 0x11}
#define ZIGBEE_LINK_KEY_UUID_CONSTANT                                     {0x27, 0x11}
#define ZIGBEE_NOTIFY_UUID_CONSTANT                                       {0x28, 0x11}

#define ZIGBEE_STR_NOTIFY_LENGTH                                          4



#define ZIGBEE_CREDITS_STATUS_ATTRIBUTE_OFFSET                            2
#define ZIGBEE_CREDITS_SUPPORT_MODE_ATTRIBUTE_OFFSET                      4
#define ZIGBEE_CREDITS_OPERATE_MODE_ATTRIBUTE_OFFSET                      6
#define ZIGBEE_CREDITS_LINK_KEY_ATTRIBUTE_OFFSET                          8
#define ZIGBEE_NOTIFICATION_CREDITS_ATTRIBUTE_OFFSET                      10

/**                                                                             
 *    Definitions Required for Thread Services                                  
 */                                                                             
                                                                                
#define THREAD_SERVICE_UUID_CONSTANT                                      {0x30, 0x11}
#define THREAD_STATUS_UUID_CONSTANT                                       {0x31, 0x11}
#define THREAD_SUPPORT_MODE_UUID_CONSTANT                                 {0x32, 0x11}
#define THREAD_OPERATE_MODE_UUID_CONSTANT                                 {0x33, 0x11}
#define THREAD_PASSPHRASE_UUID_CONSTANT                                   {0x34, 0x11}
#define THREAD_NOTIFY_UUID_CONSTANT                                       {0x35, 0x11}
                                                                                
#define THREAD_STR_NOTIFY_LENGTH                                          4     
#define PASSPHRASE_KEY_SIZE                                               16
                                                                               
                                                                  
                                                                                
#define THREAD_CREDITS_STATUS_ATTRIBUTE_OFFSET                            2     
#define THREAD_CREDITS_SUPPORT_MODE_ATTRIBUTE_OFFSET                      4     
#define THREAD_CREDITS_OPERATE_MODE_ATTRIBUTE_OFFSET                      6     
#define THREAD_CREDITS_PASSPHRASE_ATTRIBUTE_OFFSET                        8     
#define THREAD_NOTIFICATION_CREDITS_ATTRIBUTE_OFFSET                      10   

/**                                                                             
   *    Definitions Required for Offline Services                                  
    */
#define OFFLINE_SERVICE_UUID_CONSTANT                                     {0x36, 0x11}
#define OFFLINE_DATA_UUID_CONSTANT                                        {0x37, 0x11}
#define OFFLINE_NOTIFY_UUID_CONSTANT                                      {0x38, 0x11}

#define OFFLINE_CREDITS_DATA_ATTRIBUTE_OFFSET                             2     
#define OFFLINE_NOTIFICATION_CREDITS_ATTRIBUTE_OFFSET                     4

#define QAPI_BLE_GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_LENGTH          (QAPI_BLE_NON_ALIGNED_WORD_SIZE)

int Register_thread_service(void);
int Register_zigbee_service(void);
int Register_wifi_service(void);
int get_wifi_onboard_status(void);
boolean is_wifi_onboarded(void);

#endif
