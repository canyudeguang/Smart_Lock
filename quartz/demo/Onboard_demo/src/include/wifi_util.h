/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __WIFI_UTIL_H__
#define __WIFI_UTIL_H__

/**------------------------------------------------------------------------
  - Include Files
  ------------------------------------------------------------------------*/

/* TODO: Assumption is 2 VDEV are supported.
 * VDEV 0 is used as SAP for onboarding
 * VDEV 1 is used to connect to HomeAP
 */


/*-------------------------------------------------------------------------
  - Preprocessor Definitions and Constants
  ------------------------------------------------------------------------*/
//TODO: Need to set this based on the device and firmware capability
#ifndef WLAN_NUM_OF_DEVICES
#define WLAN_NUM_OF_DEVICES 2
#endif

#define DEVID_SAP                         0                            /* Device virtual mode 0 */
#define DEVID_STA                         1                            /* Device virtual mode 1 */
#define INTF_AP                           "wlan0"                      /* Interface mode 0 */
#define INTF_STA                          "wlan1"                      /* Interface mode 1 */

#define MAX_SSID_LEN                       (32)                        /* Maximum Length of ssid */
#define MAX_PASSPHRASE_LEN                 (64)                        /* Maximum length of password */
#define MIN_PASSPHRASE_LEN                 8                           /* Minimum length of password */

#define MAC_ADDR_LEN                       6

// Signal events
// AP signal 0:7
#define AP_SIGNAL_CONNECT_EVENT            (1 << 0)
#define AP_SIGNAL_DISCONNECT_EVENT         (1 << 1)

//STA signal 8:15
#define STA_SIGNAL_CONNECT_EVENT           (1 << 8)
#define STA_SIGNAL_DISCONNECT_EVENT        (1 << 9)


int32_t wlan_enable(uint32_t num_dev, qurt_signal_t *wlan_sig);
int32_t wlan_disable(void);
int32_t wlan_set_operate_mode(uint32_t deviceId);
/* Supports only OPEN and WPA2 security mode */
int32_t wlan_connect_to_network(uint32_t deviceId, const char *ssid, const char *passphrase);
int32_t wlan_disconnect_from_network(uint32_t deviceId);
int32_t wlan_get_device_mac_address(uint8_t devId, char *mac);
int32_t wlan_is_device_connected(uint32_t devId);

#endif //__WIFI_UTIL_H


