/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "qcli_api.h"
#include "qapi_wlan.h"
#include "qapi_status.h"
#include "qapi_fs.h"
#include "qapi_crypto.h"
#include "qapi_ssl.h"
#include "qurt_signal.h"  
#include "qurt_timer.h"
#include "qurt_error.h"
#include "qapi_ns_utils.h"
#include "qapi_netbuf.h"
#include "qapi_socket.h"
#include "qapi_netservices.h"
#include "qapi_ns_gen_v4.h"
#include "qapi_ns_gen_v6.h"
#include "qapi_delay.h"
#include "qapi_fatal_err.h"
#include "util.h"
#include "kpi_demo.h"


/*
 * This file contains the command handlers for kpi demo
 *
 */
QCLI_Command_Status_t dummy_cmd_2(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t m4_boot_time(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t wlan_boot_time(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t wlan_test_setup_command(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t wlan_storerecall_test(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t wlan_cummulative_throughput_test(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t kpi_demo_cleanup(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);


QCLI_Group_Handle_t qcli_kpi_handle; /* Handle for kpi demo Command Group. */



const QCLI_Command_t kpi_cmd_list[] =
{
    {m4_boot_time, true, "m4_boot_time", "Give this command to display M4 boot time breakdown \n", "Display M4 boot time"},
    {wlan_boot_time, true, "wlan_boot_time", "Give this command to display wlan boot time breakdown \n", "Display WLAN boot time"},
    {wlan_test_setup_command, true, "wlan_test_setup", "Give this command to setup wlan params \n", "Setup wlan parameters"},
    {wlan_storerecall_test, true, "wlan_storerecall_test", "Give this command to display storerecall time breakdown", "Display storerecall boot time"},
    {wlan_cummulative_throughput_test, true, "wlan_cummulative_throughput_test", "Give this command to run cumulative throughput test\n", "Measure power with throughput test"},
    {kpi_demo_cleanup, true, "kpi_demo_cleanup", "Command to clean up assigned memory", "Cleanup assigned memory"},
	{dummy_cmd_2,false,"dummy",NULL,NULL},
};

const QCLI_Command_Group_t kpi_cmd_group =
{
    "KPI Demo",              /* Group_String: will display cmd prompt as "KPI Demo> " */
    sizeof(kpi_cmd_list)/sizeof(kpi_cmd_list[0]),   /* Command_Count */
    kpi_cmd_list        /* Command_List */
};

kpi_demo_ctx *kpi_ctx;
uint8_t kpi_quit = 0;
uint32_t deviceId = 1;


/*****************************************************************************
 * This function is used to register the Fs Command Group with QCLI.
 *****************************************************************************/
void Initialize_KPI_Demo(void)
{
    /* Attempt to reqister the Command Groups with the qcli framework.*/
    qcli_kpi_handle = QCLI_Register_Command_Group(NULL, &kpi_cmd_group);
    if (qcli_kpi_handle)
    {
        QCLI_Printf(qcli_kpi_handle, "KPI Demo Registered\n");
    }

    return;
}

QCLI_Command_Status_t dummy_cmd_2(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    return QCLI_STATUS_SUCCESS_E;  
}

void kpi_wlan_callback_handler(uint8_t  deviceId, 
                               uint32_t cbId, 
                               void *pApplicationContext,
                               void     *payload,
                               uint32_t payload_Length)
{

    switch(cbId)
    {
     case QAPI_WLAN_CONNECT_CB_E:  
       {
         qapi_WLAN_Connect_Cb_Info_t *cxnInfo  = (qapi_WLAN_Connect_Cb_Info_t *)(payload);

         if(cxnInfo->value == FOUR_WAY_HANDSHAKE_COMPLETION) {
                 QCLI_Printf(qcli_kpi_handle,"4-way Handshake successful \n");
                 qurt_signal_set(&kpi_ctx->wlan_kpi_event, KPI_CONNECT_EVENT_MASK);
         }
       }
       break;
     case QAPI_WLAN_ERROR_HANDLER_CB_E:
     {
         /* currently no information is provided on fatal error, hence left blank */
         QCLI_Printf(qcli_kpi_handle,"Fatal error occured. Error= ");

         /* take necessary action based on error */
         QAPI_FATAL_ERR(0,0,0);
     }
     case QAPI_WLAN_RESUME_HANDLER_CB_E:
     {
         QCLI_Printf(qcli_kpi_handle, "\r\n WLAN FIRMWARE RESUME Completed");
         qurt_signal_set(&kpi_ctx->wlan_kpi_event, KPI_RESUME_DONE_EVENT_MASK);
     }

     default:
         break;
    } 

    return;

}

uint32_t kpi_get_time(time_struct_t *time)
{
    uint32_t ticks, ms;

    ticks = (uint32_t)qurt_timer_get_ticks();
    ms = (uint32_t)qurt_timer_convert_ticks_to_time(ticks, QURT_TIME_MSEC);

    if (time)
    {
        time->seconds = ms / 1000;
        time->milliseconds = ms % 1000;
        time->ticks = ticks;
    }

    return ms;
}


static int32_t kpi_ipconfig_dhcpc_success_cb(uint32_t addr, uint32_t mask, uint32_t gw)
{
    char ip_str[20];
    char mask_str[20];
    char gw_str[20];

    QCLI_Printf(qcli_kpi_handle, "DHCPv4c: IP=%s  Subnet Mask=%s  Gateway=%s\n",
            inet_ntop(AF_INET, &addr, ip_str, sizeof(ip_str)),
            inet_ntop(AF_INET, &mask, mask_str, sizeof(mask_str)),
            inet_ntop(AF_INET, &gw, gw_str, sizeof(gw_str)));

    qurt_signal_set(&kpi_ctx->wlan_kpi_event, KPI_DHCP_EVENT_MASK);

    kpi_ctx->params.tx_params.source_ipv4_addr = addr;
    memcpy(kpi_ctx->ip, ip_str, 20);
 
    return 0;
}

QCLI_Command_Status_t m4_boot_time(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    /* Display M4 Boot time */

    /* break down as follows, PBL boot time, SBL Boot time, RTOS Boot time, App_start */

    return QCLI_STATUS_SUCCESS_E;

}

extern uint32_t *g_boot_time_measure;
extern uint32_t *g_wlan_strrcl_time_measure;

QCLI_Command_Status_t wlan_boot_time(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

   uint32_t temp1, temp2, duration;
   int32_t multiplier = RTC_CLOCK_MULTIPLIER;
   int32_t divider = RTC_CLOCK_DIVIDER;
   int32_t time_elapsed;

   /* enable wlan to measure the wlan boot time */
   QCLI_Printf(qcli_kpi_handle,"Enabling wlan \n");

   
    if (0 == qapi_WLAN_Enable(QAPI_WLAN_ENABLE_E))
    {
           QCLI_Printf(qcli_kpi_handle, "wlan enabled \n");
    }
    else {
           QCLI_Printf(qcli_kpi_handle, "wlan enabled failed \n");
          return QCLI_STATUS_ERROR_E;

    }
    /* print boot time results */
    QCLI_Printf(qcli_kpi_handle,"Boot Time Results for WLAN");

    temp1 = g_boot_time_measure[WLAN_BOOT_KF_INIT_INDEX];
    temp2 = g_boot_time_measure[WLAN_BOOT_KF_POWER_ON_INDEX];

    if (temp2 < temp1)
    {
        /* Assume the systick wraps around once */
        duration = ~temp1 + 1 + temp2;
    }
    else
    {
        duration = temp2 - temp1;
    }

    time_elapsed = (duration * multiplier)/divider;

    QCLI_Printf(qcli_kpi_handle,"Boot Time: time to power up wlan chip %d ms \n", time_elapsed);

    temp1 = g_boot_time_measure[WLAN_BOOT_KF_POWER_ON_INDEX];
    temp2 = g_boot_time_measure[WLAN_BOOT_KF_FW_DOWNLOAD_INDEX];

    if (temp2 < temp1)
    {
        /* Assume the systick wraps around once */
        duration = ~temp1 + 1 + temp2;
    }
    else
    {
        duration = temp2 - temp1;
    }

    time_elapsed = (duration * multiplier)/divider;

    QCLI_Printf(qcli_kpi_handle,"Boot Time: time to download wlan firmware %d ms \n", time_elapsed);

    temp1 = g_boot_time_measure[WLAN_BOOT_KF_FW_DOWNLOAD_INDEX];
    temp2 = g_boot_time_measure[WLAN_BOOT_KF_WMI_READY_TIME];

    if (temp2 < temp1)
    {
        /* Assume the systick wraps around once */
        duration = ~temp1 + 1 + temp2;
    }
    else
    {
        duration = temp2 - temp1;
    }

    time_elapsed = (duration * multiplier)/divider;

    QCLI_Printf(qcli_kpi_handle,"Boot Time: time between firmware download & wmi_ready, KF Boot Time %d ms \n", time_elapsed);

    temp1 = g_boot_time_measure[WLAN_BOOT_KF_INIT_INDEX];
    temp2 = g_boot_time_measure[WLAN_BOOT_KF_WMI_READY_TIME];

    if (temp2 < temp1)
    {
        /* Assume the systick wraps around once */
        duration = ~temp1 + 1 + temp2;
    }
    else
    {
        duration = temp2 - temp1;
    }

    time_elapsed = (duration * multiplier)/divider;

    QCLI_Printf(qcli_kpi_handle,"Boot Time: Total Boot time %d ms \n", time_elapsed);

    if(0 == qapi_WLAN_Enable(QAPI_WLAN_DISABLE_E))
    {
        QCLI_Printf(qcli_kpi_handle,"Wlan successfully disabled \n");
    }

    return QCLI_STATUS_SUCCESS_E;

}

QCLI_Command_Status_t kpi_demo_cleanup(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    if(kpi_ctx->ssid != NULL)
        free(kpi_ctx->ssid);

    if(kpi_ctx->wpa2_psk != NULL)
        free(kpi_ctx->wpa2_psk);

    if(kpi_ctx != NULL)
        free(kpi_ctx);

    kpi_ctx = NULL;

    return QCLI_STATUS_SUCCESS_E;


}

QCLI_Command_Status_t wlan_test_setup_command(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

   /*store parameters, 
     SSID,
     WPA2 key,
     WEP key, 
   */
    kpi_ctx = malloc(sizeof(kpi_demo_ctx));

    memset(kpi_ctx, 0, sizeof(kpi_demo_ctx));

    if(Parameter_Count < 2) {
        QCLI_Printf(qcli_kpi_handle,"All parameters are required \n");
        QCLI_Printf(qcli_kpi_handle,"Usage: wlan_test_setup ssid wpa2_key operating_channel \n");
        return QCLI_STATUS_SUCCESS_E;
    }

    kpi_ctx->ssid = malloc(WLAN_MAX_SSID_LEN);
    kpi_ctx->wpa2_psk = malloc(WLAN_WPA_KEY_MAX_LEN);

    /* parse command line params */
    memcpy(kpi_ctx->ssid,
        (char *)Parameter_List[0].String_Value,strlen(Parameter_List[0].String_Value)+1);
    memcpy(kpi_ctx->wpa2_psk,
        (char *)Parameter_List[1].String_Value,strlen(Parameter_List[1].String_Value)+1);

    kpi_ctx->operating_channel = Parameter_List[2].Integer_Value;

    return QCLI_STATUS_SUCCESS_E;

}

QCLI_Command_Status_t wlan_storerecall_test(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

   /* steps required,
      1. Make sure wlan_test_setup has been called before
      2. enable wlan
      3. connect 1. open security 2. wpa2_security.
      4. Sub steps - after successful connection
         - suspend first for 30 seconds  --> print results
         - suspend second time for 120 seconds. --> print results
   */

    int32_t error = 0;
    int ssidLength = 0;
    uint32_t temp_mode = 0, dataLen = 0; 
    char *interface_name = "wlan1";
    qbool_t if_is_up = FALSE;
    char *bssid = NULL;
    qapi_WLAN_Crypt_Type_e cipher;
    qapi_WLAN_Auth_Mode_e wpa_ver;
    uint32_t temp1, temp2, duration, time_elapsed;
    int32_t multiplier = RTC_CLOCK_MULTIPLIER;
    int32_t divider = RTC_CLOCK_DIVIDER;
    uint32_t signal_set;

    if(Parameter_Count < 1) {
        QCLI_Printf(qcli_kpi_handle,"All parameters are required \n");
        QCLI_Printf(qcli_kpi_handle,"Usage: wlan_storerecall_test security_type \n");
        QCLI_Printf(qcli_kpi_handle,"Note: Only open security and WPA2 CCMP is supported in this demo \n");
        QCLI_Printf(qcli_kpi_handle,"0 = open security, 1 = WPA2 Securtiy \n");
        return QCLI_STATUS_SUCCESS_E;
    }

    kpi_ctx->security_mode = Parameter_List[0].Integer_Value;

    if((kpi_ctx->security_mode != KPI_SEC_MODE_OPEN) && (kpi_ctx->security_mode != KPI_SEC_MODE_WPA)) {
        QCLI_Printf(qcli_kpi_handle, "Invalid security type, 0 = open security, 1 = WPA2 Securtiy supported \n");
    }

    if(kpi_ctx == NULL){
        QCLI_Printf(qcli_kpi_handle,"Please run wlan_test_setup command \n");
        return QCLI_STATUS_SUCCESS_E;
    }

    qurt_signal_init(&kpi_ctx->wlan_kpi_event);


    if (0 == qapi_WLAN_Enable(QAPI_WLAN_ENABLE_E))
    {
        QCLI_Printf(qcli_kpi_handle, "Wlan enabled \n");
        qapi_WLAN_Add_Device(deviceId);
        qapi_WLAN_Set_Callback(deviceId, (qapi_WLAN_Callback_t)kpi_wlan_callback_handler, NULL);

    }
    else
    {
        QCLI_Printf(qcli_kpi_handle, "Wlan Enabled failed \n");
        return -1;
    }

    ssidLength = strlen(kpi_ctx->ssid);
    
    qapi_WLAN_Get_Param (deviceId, 
                         __QAPI_WLAN_PARAM_GROUP_WIRELESS,
                         __QAPI_WLAN_PARAM_GROUP_WIRELESS_OPERATION_MODE,
                         &temp_mode,
                         &dataLen);

    if (ssidLength > __QAPI_WLAN_MAX_SSID_LENGTH || ssidLength < 0) 
    {
       QCLI_Printf(qcli_kpi_handle, "SSID length exceeds Maximum value\r\n");
       return QCLI_STATUS_ERROR_E;
    }
    error = qapi_WLAN_Set_Param (deviceId,
                                 __QAPI_WLAN_PARAM_GROUP_WIRELESS,
                                 __QAPI_WLAN_PARAM_GROUP_WIRELESS_SSID,
                                 (void *) kpi_ctx->ssid,
                                 ssidLength,
                                 FALSE);
    if (error != 0)
    {
        QCLI_Printf(qcli_kpi_handle,"Error during setting of ssid %s error=%08x!\r\n", kpi_ctx->ssid, error);
        return error;
    }

    if (temp_mode == QAPI_WLAN_DEV_MODE_STATION_E)
    {
        uint8_t bssidToConnect[__QAPI_WLAN_MAC_LEN] = {0};
        if (bssid && (ether_aton(bssid, bssidToConnect) != 0))
        {
            QCLI_Printf(qcli_kpi_handle, "Invalid BSSID to connect\r\n");
            return QCLI_STATUS_ERROR_E;
        }
        error = qapi_WLAN_Set_Param (deviceId,
                             __QAPI_WLAN_PARAM_GROUP_WIRELESS,
                             __QAPI_WLAN_PARAM_GROUP_WIRELESS_BSSID,
                             bssidToConnect,
                             __QAPI_WLAN_MAC_LEN,
                             FALSE);
        if (error != 0)
        {
            QCLI_Printf(qcli_kpi_handle,"Error during setting of bssid  error=%08x!\r\n", error);
            return error;
        }

        if(bssid)
           QCLI_Printf(qcli_kpi_handle,"\r\nSetting BSSID to %s \r\n",bssid);

    }
   
    if(KPI_SEC_MODE_WPA == kpi_ctx->security_mode)
    {
        uint32_t passphraseLen = 0;
        cipher = QAPI_WLAN_CRYPT_AES_CRYPT_E; 
        wpa_ver = QAPI_WLAN_AUTH_WPA2_PSK_E;

        QCLI_Printf(qcli_kpi_handle,"Setting security mode to wpa \n");

        if(0 != qapi_WLAN_Set_Param(deviceId,
                                       __QAPI_WLAN_PARAM_GROUP_WIRELESS_SECURITY,
                                       __QAPI_WLAN_PARAM_GROUP_SECURITY_ENCRYPTION_TYPE,
                                       (void *) &cipher, //cipher is set in set_wpa
                                       sizeof(qapi_WLAN_Crypt_Type_e), 
                                       FALSE))
        {
            QCLI_Printf(qcli_kpi_handle,"Setting cipher failed \n");
            return -1;
        }

        if( 0 != qapi_WLAN_Set_Param (deviceId,
                                        __QAPI_WLAN_PARAM_GROUP_WIRELESS_SECURITY,
                                        __QAPI_WLAN_PARAM_GROUP_SECURITY_AUTH_MODE,
                                        (void *) &wpa_ver,
                                        sizeof(qapi_WLAN_Auth_Mode_e),
                                        FALSE))
        {
            QCLI_Printf(qcli_kpi_handle,"Setting wpa version failed \n");
            return QCLI_STATUS_ERROR_E;
        }
        passphraseLen  = strlen(kpi_ctx->wpa2_psk);
        if((passphraseLen >= 8) && (passphraseLen <= 63))
        {
            if (0 != qapi_WLAN_Set_Param(deviceId,
                                            __QAPI_WLAN_PARAM_GROUP_WIRELESS_SECURITY,
                                            __QAPI_WLAN_PARAM_GROUP_SECURITY_PASSPHRASE,
                                            (void *)kpi_ctx->wpa2_psk,
                                             passphraseLen,
                                             FALSE))
            {
                QCLI_Printf(qcli_kpi_handle, "Unable to set passphrase\r\n");
                return QCLI_STATUS_ERROR_E;
            }
        }
        else if(passphraseLen == MAX_PASSPHRASE_LENGTH)
        {
            if (0 != qapi_WLAN_Set_Param(deviceId,
                                            __QAPI_WLAN_PARAM_GROUP_WIRELESS_SECURITY,
                                            __QAPI_WLAN_PARAM_GROUP_SECURITY_PMK,
                                            (void *) kpi_ctx->wpa2_psk,
                                            passphraseLen,
                                            FALSE))
            {
                QCLI_Printf(qcli_kpi_handle, "Unable to set pmk\r\n");
                return QCLI_STATUS_ERROR_E;
            }
       }
       else
       {
           QCLI_Printf(qcli_kpi_handle, "invalid password\r\n");
           return QCLI_STATUS_ERROR_E;
       }
        kpi_ctx->security_mode = KPI_SEC_MODE_OPEN;
   }

    error = qapi_WLAN_Commit(deviceId);
    if(error != 0)
    { 
        QCLI_Printf(qcli_kpi_handle, "Commit failed \r\n");
        return QCLI_STATUS_ERROR_E;
    }

    if(qurt_signal_wait_timed(&kpi_ctx->wlan_kpi_event, KPI_CONNECT_EVENT_MASK, QURT_SIGNAL_ATTR_CLEAR_MASK, &signal_set, QURT_TIME_WAIT_FOREVER) != QURT_EOK) {
        QCLI_Printf(qcli_kpi_handle, "Timed out waiting for the signal \r\n");
        return QCLI_STATUS_ERROR_E;
    }

    QCLI_Printf(qcli_kpi_handle,"getting IP address \n");

    /* get dhcp IP address */
    if (qapi_Net_Interface_Exist(interface_name, &if_is_up) == FALSE ||
        if_is_up == FALSE)
    {
        QCLI_Printf(qcli_kpi_handle, "ERROR: %s does not exist or is DOWN.\n", interface_name);
        return QCLI_STATUS_ERROR_E;
    }

    if (qapi_Net_DHCPv4c_Register_Success_Callback(interface_name, kpi_ipconfig_dhcpc_success_cb) != 0 ||
            qapi_Net_IPv4_Config(interface_name, QAPI_NET_IPV4CFG_DHCP_IP_E, NULL, NULL, NULL) != 0)
    {
            QCLI_Printf(qcli_kpi_handle, "ERROR: DHCPv4 new failed\n");
            return QCLI_STATUS_ERROR_E;
    }

    /* wait for dhcp callback */
    if(qurt_signal_wait_timed(&kpi_ctx->wlan_kpi_event, KPI_DHCP_EVENT_MASK, QURT_SIGNAL_ATTR_CLEAR_MASK, &signal_set, QURT_TIME_WAIT_FOREVER) != QURT_EOK) {
        return -1;
    }

    /* wait one second before starting the storerecall test */
    qapi_Task_Delay(1000000);


    /* initiate storerecall for 30 seconds */
    qapi_WLAN_Suspend_Start(30000);


    /* wait for resume done callback */
    if(qurt_signal_wait_timed(&kpi_ctx->wlan_kpi_event, KPI_RESUME_DONE_EVENT_MASK, QURT_SIGNAL_ATTR_CLEAR_MASK, &signal_set, QURT_TIME_WAIT_FOREVER) != QURT_EOK) {
        /* todo, check for correct signal */
        return -1;
    }

    /* print the storerecall events */
    temp1 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_INIT_INDEX];
    temp2 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_POWER_ON_INDEX];

    if (temp2 < temp1)
    {
        /* Assume the systick wraps around once */
        duration = ~temp1 + 1 + temp2;
    }
    else
    {
        duration = temp2 - temp1;
    }

    time_elapsed = (duration * multiplier)/divider;

    QCLI_Printf(qcli_kpi_handle,"storerecall Time: time to power up wlan chip %d ms \n", time_elapsed);

    temp1 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_POWER_ON_INDEX];
    temp2 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_FW_DOWNLOAD_INDEX];


    if (temp2 < temp1)
    {
        /* Assume the systick wraps around once */
        duration = ~temp1 + 1 + temp2;
    }
    else
    {
        duration = temp2 - temp1;
    }

    time_elapsed = (duration * multiplier)/divider;

    QCLI_Printf(qcli_kpi_handle,"storerecall Time: time to download wlan firmware %d ms \n", time_elapsed);

    temp1 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_FW_DOWNLOAD_INDEX];
    temp2 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_DONE_TIME];


    if (temp2 < temp1)
    {
        /* Assume the systick wraps around once */
        duration = ~temp1 + 1 + temp2;
    }
    else
    {
        duration = temp2 - temp1;
    }

    time_elapsed = (duration * multiplier)/divider;

    QCLI_Printf(qcli_kpi_handle,"storerecall Time: time between firmware download & storerecall completion %d ms \n", time_elapsed);

    temp1 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_INIT_INDEX];
    temp2 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_DONE_TIME];

    if (temp2 < temp1)
    {
        /* Assume the systick wraps around once */
        duration = ~temp1 + 1 + temp2;
    }
    else
    {
        duration = temp2 - temp1;
    }

    time_elapsed = (duration * multiplier)/divider;

    QCLI_Printf(qcli_kpi_handle,"storerecall Time: Total Boot time %d ms \n", time_elapsed);


    /* wait one second before starting the storerecall test */
    qapi_Task_Delay(1000);

    /* initiate storerecall for 150 seconds */
    qapi_WLAN_Suspend_Start(150000);

    /* wait for resume done callback */
    if(qurt_signal_wait_timed(&kpi_ctx->wlan_kpi_event, KPI_RESUME_DONE_EVENT_MASK, QURT_SIGNAL_ATTR_CLEAR_MASK, &signal_set, QURT_TIME_WAIT_FOREVER) != QURT_EOK) {
        return -1;
    }

    /* print the storerecall events */
    temp1 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_INIT_INDEX];
    temp2 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_POWER_ON_INDEX];

    if (temp2 < temp1)
    {
        /* Assume the systick wraps around once */
        duration = ~temp1 + 1 + temp2;
    }
    else
    {
        duration = temp2 - temp1;
    }

    time_elapsed = (duration * multiplier)/divider;

    QCLI_Printf(qcli_kpi_handle,"storerecall Time: time to power up wlan chip %d ms \n", time_elapsed);

    temp1 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_POWER_ON_INDEX];
    temp2 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_FW_DOWNLOAD_INDEX];

    if (temp2 < temp1)
    {
        /* Assume the systick wraps around once */
        duration = ~temp1 + 1 + temp2;
    }
    else
    {
        duration = temp2 - temp1;
    }

    time_elapsed = (duration * multiplier)/divider;

    QCLI_Printf(qcli_kpi_handle,"storerecall Time: time to download wlan firmware %d ms \n", time_elapsed);

    temp1 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_FW_DOWNLOAD_INDEX];
    temp2 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_DONE_TIME];

    if (temp2 < temp1)
    {
        /* Assume the systick wraps around once */
        duration = ~temp1 + 1 + temp2;
    }
    else
    {
        duration = temp2 - temp1;
    }

    time_elapsed = (duration * multiplier)/divider;

    QCLI_Printf(qcli_kpi_handle,"storerecall Time: time between firmware download & storerecall completion %d ms \n", time_elapsed);

    temp1 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_INIT_INDEX];
    temp2 = g_wlan_strrcl_time_measure[WLAN_STORERECALL_KF_DONE_TIME];

    if (temp2 < temp1)
    {
        /* Assume the systick wraps around once */
        duration = ~temp1 + 1 + temp2;
    }
    else
    {
        duration = temp2 - temp1;
    }

    time_elapsed = (duration * multiplier)/divider;

    QCLI_Printf(qcli_kpi_handle,"storerecall Time: Total Boot time %d ms \n", time_elapsed);

    qurt_signal_delete(&kpi_ctx->wlan_kpi_event);

    if(0 == qapi_WLAN_Enable(QAPI_WLAN_DISABLE_E))
    {
        QCLI_Printf(qcli_kpi_handle,"Wlan successfully disabled \n");
    }

    return 0;

}

QCLI_Command_Status_t wlan_cummulative_throughput_test(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    int32_t error = 0;
    int ssidLength = 0;
    uint32_t temp_mode = 0, dataLen = 0; 
    char *interface_name = "wlan1";
    qbool_t if_is_up = FALSE;
    char *bssid = NULL;
    int rate = 1;
    qapi_WLAN_Crypt_Type_e cipher;
    qapi_WLAN_Auth_Mode_e wpa_ver;
    uint32_t signal_set;


    /* take port, ip address and rate as input */

    if(Parameter_Count < 4) {
        QCLI_Printf(qcli_kpi_handle,"All parameters are required \n");
        QCLI_Printf(qcli_kpi_handle,"Usage: wlan_cummulative_throughput_test ip_address port rate security_type \n");
        return QCLI_STATUS_SUCCESS_E;
    }

    /* parse command line params */
    inet_pton(AF_INET,Parameter_List[0].String_Value, &kpi_ctx->params.tx_params.ip_address);
    kpi_ctx->params.tx_params.port = Parameter_List[1].Integer_Value;;
    rate = Parameter_List[2].Integer_Value;
    kpi_ctx->security_mode = Parameter_List[3].Integer_Value;

    qurt_signal_init(&kpi_ctx->wlan_kpi_event);


    if(kpi_ctx == NULL){
        QCLI_Printf(qcli_kpi_handle,"Please run wlan_test_setup command \n");
        return QCLI_STATUS_SUCCESS_E;
    }

    qurt_signal_init(&kpi_ctx->wlan_kpi_event);


    if (0 == qapi_WLAN_Enable(QAPI_WLAN_ENABLE_E))
    {
        QCLI_Printf(qcli_kpi_handle, "Wlan enabled \n");
        qapi_WLAN_Add_Device(deviceId);
        qapi_WLAN_Set_Callback(deviceId, (qapi_WLAN_Callback_t)kpi_wlan_callback_handler, NULL);

    }
    else
    {
        QCLI_Printf(qcli_kpi_handle, "Wlan Enabled failed \n");
        return -1;
    }

    ssidLength = strlen(kpi_ctx->ssid);
    
    qapi_WLAN_Get_Param (deviceId, 
                         __QAPI_WLAN_PARAM_GROUP_WIRELESS,
                         __QAPI_WLAN_PARAM_GROUP_WIRELESS_OPERATION_MODE,
                         &temp_mode,
                         &dataLen);

    if (ssidLength > __QAPI_WLAN_MAX_SSID_LENGTH || ssidLength < 0) 
    {
       QCLI_Printf(qcli_kpi_handle, "SSID length exceeds Maximum value\r\n");
       return QCLI_STATUS_ERROR_E;
    }
    error = qapi_WLAN_Set_Param (deviceId,
                                 __QAPI_WLAN_PARAM_GROUP_WIRELESS,
                                 __QAPI_WLAN_PARAM_GROUP_WIRELESS_SSID,
                                 (void *) kpi_ctx->ssid,
                                 ssidLength,
                                 FALSE);
    if (error != 0)
    {
        QCLI_Printf(qcli_kpi_handle,"Error during setting of ssid %s error=%08x!\r\n", kpi_ctx->ssid, error);
        return error;
    }

    if (temp_mode == QAPI_WLAN_DEV_MODE_STATION_E)
    {
        uint8_t bssidToConnect[__QAPI_WLAN_MAC_LEN] = {0};
        if (bssid && (ether_aton(bssid, bssidToConnect) != 0))
        {
            QCLI_Printf(qcli_kpi_handle, "Invalid BSSID to connect\r\n");
            return QCLI_STATUS_ERROR_E;
        }
        error = qapi_WLAN_Set_Param (deviceId,
                             __QAPI_WLAN_PARAM_GROUP_WIRELESS,
                             __QAPI_WLAN_PARAM_GROUP_WIRELESS_BSSID,
                             bssidToConnect,
                             __QAPI_WLAN_MAC_LEN,
                             FALSE);
        if (error != 0)
        {
            QCLI_Printf(qcli_kpi_handle,"Error during setting of bssid  error=%08x!\r\n", error);
            return error;
        }

        if(bssid)
           QCLI_Printf(qcli_kpi_handle,"\r\nSetting BSSID to %s \r\n",bssid);

    }
   
    if(KPI_SEC_MODE_WPA == kpi_ctx->security_mode)
    {
        uint32_t passphraseLen = 0;
        cipher = QAPI_WLAN_CRYPT_AES_CRYPT_E; 
        wpa_ver = QAPI_WLAN_AUTH_WPA2_PSK_E;

        QCLI_Printf(qcli_kpi_handle,"Setting security mode to wpa \n");

        if(0 != qapi_WLAN_Set_Param(deviceId,
                                       __QAPI_WLAN_PARAM_GROUP_WIRELESS_SECURITY,
                                       __QAPI_WLAN_PARAM_GROUP_SECURITY_ENCRYPTION_TYPE,
                                       (void *) &cipher, //cipher is set in set_wpa
                                       sizeof(qapi_WLAN_Crypt_Type_e), 
                                       FALSE))
        {
            QCLI_Printf(qcli_kpi_handle,"Setting cipher failed \n");
            return -1;
        }

        if( 0 != qapi_WLAN_Set_Param (deviceId,
                                        __QAPI_WLAN_PARAM_GROUP_WIRELESS_SECURITY,
                                        __QAPI_WLAN_PARAM_GROUP_SECURITY_AUTH_MODE,
                                        (void *) &wpa_ver,
                                        sizeof(qapi_WLAN_Auth_Mode_e),
                                        FALSE))
        {
            QCLI_Printf(qcli_kpi_handle,"Setting wpa version failed \n");
            return QCLI_STATUS_ERROR_E;
        }
        passphraseLen  = strlen(kpi_ctx->wpa2_psk);
        if((passphraseLen >= 8) && (passphraseLen <= 63))
        {
            if (0 != qapi_WLAN_Set_Param(deviceId,
                                            __QAPI_WLAN_PARAM_GROUP_WIRELESS_SECURITY,
                                            __QAPI_WLAN_PARAM_GROUP_SECURITY_PASSPHRASE,
                                            (void *)kpi_ctx->wpa2_psk,
                                             passphraseLen,
                                             FALSE))
            {
                QCLI_Printf(qcli_kpi_handle, "Unable to set passphrase\r\n");
                return QCLI_STATUS_ERROR_E;
            }
        }
        else if(passphraseLen == MAX_PASSPHRASE_LENGTH)
        {
            if (0 != qapi_WLAN_Set_Param(deviceId,
                                            __QAPI_WLAN_PARAM_GROUP_WIRELESS_SECURITY,
                                            __QAPI_WLAN_PARAM_GROUP_SECURITY_PMK,
                                            (void *) kpi_ctx->wpa2_psk,
                                            passphraseLen,
                                            FALSE))
            {
                QCLI_Printf(qcli_kpi_handle, "Unable to set pmk\r\n");
                return QCLI_STATUS_ERROR_E;
            }
       }
       else
       {
           QCLI_Printf(qcli_kpi_handle, "invalid password\r\n");
           return QCLI_STATUS_ERROR_E;
       }
        kpi_ctx->security_mode = KPI_SEC_MODE_OPEN;
   }

    error = qapi_WLAN_Commit(deviceId);
    if(error != 0)
    { 
        QCLI_Printf(qcli_kpi_handle, "Commit failed \r\n");
        return QCLI_STATUS_ERROR_E;
    }

    if(qurt_signal_wait_timed(&kpi_ctx->wlan_kpi_event, KPI_CONNECT_EVENT_MASK, QURT_SIGNAL_ATTR_CLEAR_MASK, &signal_set, QURT_TIME_WAIT_FOREVER) != QURT_EOK) {
        QCLI_Printf(qcli_kpi_handle, "Timed out waiting for the signal \r\n");
        return QCLI_STATUS_ERROR_E;
    }

    QCLI_Printf(qcli_kpi_handle,"getting IP address \n");

    /* get dhcp IP address */
    if (qapi_Net_Interface_Exist(interface_name, &if_is_up) == FALSE ||
        if_is_up == FALSE)
    {
        QCLI_Printf(qcli_kpi_handle, "ERROR: %s does not exist or is DOWN.\n", interface_name);
        return QCLI_STATUS_ERROR_E;
    }

    if (qapi_Net_DHCPv4c_Register_Success_Callback(interface_name, kpi_ipconfig_dhcpc_success_cb) != 0 ||
            qapi_Net_IPv4_Config(interface_name, QAPI_NET_IPV4CFG_DHCP_IP_E, NULL, NULL, NULL) != 0)
    {
            QCLI_Printf(qcli_kpi_handle, "ERROR: DHCPv4 new failed\n");
            return QCLI_STATUS_ERROR_E;
    }

    /* wait for dhcp callback */
    if(qurt_signal_wait_timed(&kpi_ctx->wlan_kpi_event, KPI_DHCP_EVENT_MASK, QURT_SIGNAL_ATTR_CLEAR_MASK, &signal_set, QURT_TIME_WAIT_FOREVER) != QURT_EOK) {
        return -1;
    }
    /* wait one second before starting the throughput test */
    qapi_Task_Delay(1000);

    /* initiate throughput test */
    kpi_ctx->params.tx_params.zerocopy_send = 1; /* zerocopy */
    kpi_ctx->params.tx_params.ip_tos = 0;
    kpi_ctx->params.tx_params.test_mode = PACKET_TEST;
    kpi_ctx->params.tx_params.interval_us = 0; /* interval is 0 */
    kpi_ctx->params.tx_params.packet_number = (rate*ONE_MEGABIT_RATE_PACKET_COUNT);
    /* run the test for 1 minute */
    kpi_ctx->total_packet_count = (rate*ONE_MEGABIT_RATE_PACKET_COUNT*60);

    kpi_udp_cumulative_tx();

    if(0 == qapi_WLAN_Enable(QAPI_WLAN_DISABLE_E))
    {
        QCLI_Printf(qcli_kpi_handle,"Wlan successfully disabled \n");
    }

    return QCLI_STATUS_SUCCESS_E;


}

uint32_t kpi_suspend_time()
{
    uint32_t duration;          /* in ticks */
    uint64_t total_tx_time;     /* in msec */
    uint32_t last_tick = kpi_ctx->pktStats.last_time.ticks;
    uint32_t first_tick = kpi_ctx->pktStats.first_time.ticks;
    uint32_t time_interval = 1000000; /* 1000 ms total duration */
    uint32_t time_to_suspend = 0;

    if (last_tick < first_tick)
    {
        /* Assume the systick wraps around once */
        duration = ~first_tick + 1 + last_tick;
    }
    else
    {
        duration = last_tick - first_tick;
    }

    total_tx_time = duration * qurt_timer_convert_ticks_to_time(1, QURT_TIME_MSEC);

    /* already took us 1 sec to transmit the required rate */
    if(total_tx_time > time_interval)
        return 1;

    time_to_suspend = time_interval - total_tx_time;

    /* start the suspend process */
    /* Needs to be replaced by proper M4 Deep sleep routine, waiting on Coreteam reply */
    qapi_Task_Delay(time_to_suspend);

    return 1;

}

void kpi_udp_cumulative_tx()
{
    struct sockaddr_in foreign_addr;
    struct sockaddr *to;
    uint32_t tolen;
    char ip_str[20];
    int32_t send_bytes;
    uint32_t packet_size = 1400;
    uint32_t cur_packet_number,n_send_ok;
    uint32_t netbuf_id;
    int send_flag;
    int family;
    int tos_opt;
    uint32_t zerocopy_send;
    struct sockaddr_in src_sin;
    KPI_EOT_PACKET eot_packet, *endmark;
    uint32_t total_packet_count = 0;

    endmark = &eot_packet;

    family = AF_INET;
    memcpy(ip_str, kpi_ctx->ip, 20);

    memset(&foreign_addr, 0, sizeof(foreign_addr));
    foreign_addr.sin_addr.s_addr    = kpi_ctx->params.tx_params.ip_address;
    foreign_addr.sin_port           = htons(kpi_ctx->params.tx_params.port);
    foreign_addr.sin_family         = family;

    src_sin.sin_family              = family;
    src_sin.sin_addr.s_addr         = kpi_ctx->params.tx_params.source_ipv4_addr;
    src_sin.sin_port                = htons(0);

    to = (struct sockaddr *)&foreign_addr;
    tolen = sizeof(foreign_addr);
    tos_opt = IP_TOS;

    zerocopy_send = kpi_ctx->params.tx_params.zerocopy_send;

    /* Create UDP socket */
    if ((kpi_ctx->sock_peer = qapi_socket(family, SOCK_DGRAM, 0)) == -1)
    {
        QCLI_Printf(qcli_kpi_handle, "Socket creation failed\n");
        goto ERROR_1;
    }

    if (kpi_ctx->params.tx_params.source_ipv4_addr != 0) {
        if (qapi_bind(kpi_ctx->sock_peer, (struct sockaddr*)&src_sin, sizeof(src_sin)) == -1) {
            QCLI_Printf(qcli_kpi_handle, "Socket bind failed\n");
            goto ERROR_2;
        }
    }

    if (kpi_ctx->params.tx_params.ip_tos > 0)
    {
        qapi_setsockopt(kpi_ctx->sock_peer, IP_OPTIONS, tos_opt, &kpi_ctx->params.tx_params.ip_tos, sizeof(uint8_t));
    }

    /* Connect to the server.*/
    QCLI_Printf(qcli_kpi_handle, "Connecting\n");
    if (qapi_connect( kpi_ctx->sock_peer, to, tolen) == -1)
    {
        QCLI_Printf(qcli_kpi_handle, "Connection failed\n");
        goto ERROR_2;
    }

    /* Sending.*/
    QCLI_Printf(qcli_kpi_handle, "Sending\n");

    netbuf_id = QAPI_NETBUF_SYS;
    send_flag = MSG_ZEROCOPYSEND;

    /*Reset all counters*/
    cur_packet_number = 0;
    n_send_ok = 0;

    kpi_get_time(&kpi_ctx->pktStats.first_time);

    uint32_t is_test_done = 0;
    while ( !is_test_done )
    {
        if (kpi_quit)
        {
            kpi_get_time(&kpi_ctx->pktStats.last_time);
            break;
        }

        /* allocate the buffer, if needed */
        if ( kpi_ctx->buffer == NULL )
        {
            while ((kpi_ctx->buffer = qapi_Net_Buf_Alloc(packet_size, netbuf_id)) == NULL)
            {
                /*Wait till we get a buffer*/
                if (kpi_quit)
                {
                    kpi_get_time(&kpi_ctx->pktStats.last_time);
                    goto ERROR_2;
                }
            }

            /* Update net buffer:
             *
             * [START]<4-byte Packet Index><4-byte Packet Size>000102..FF000102..FF0001..[END]
             * Byte counts: 8 + 4 + 4 + (packet_size-22) + 6
             */
             char *pkt_start = "[START]";
             char *pkt_end = "[END]";
             uint32_t val;


             /* Add "[START]" */
             qapi_Net_Buf_Update(kpi_ctx->buffer, 0, pkt_start, 8, netbuf_id);

             /* Packet index */
             val = htonl(cur_packet_number);
             qapi_Net_Buf_Update(kpi_ctx->buffer, 8, &val, 4, netbuf_id);

             /* Packet size */
             val = htonl(packet_size);
             qapi_Net_Buf_Update(kpi_ctx->buffer, 12, &val, 4, netbuf_id);
                        

             /* Add pattern
              * The pattern is repeated 00 01 02 03 .. FE FF
              */
             uint8_t data[] = {0xaa, 0xbb, 0xcc, 0xdd};
             qapi_Net_Buf_Update(kpi_ctx->buffer, 16, data, packet_size - 16 - 6, netbuf_id);

             /* Add "[END]" */
             qapi_Net_Buf_Update(kpi_ctx->buffer, packet_size-6, pkt_end, 6, netbuf_id);
        }
        else 
        {
            uint32_t val;
            uint32_t idx;

            idx = 8;

            /* Packet index */
            val = htonl(cur_packet_number);
            qapi_Net_Buf_Update(kpi_ctx->buffer, idx, &val, 4, netbuf_id);
        }
        do
        {
            
            {
                send_bytes = qapi_send(kpi_ctx->sock_peer, kpi_ctx->buffer, packet_size, send_flag);
            }

            if ( send_bytes != packet_size )
            {
                int errno = qapi_errno(kpi_ctx->sock_peer);
                if ( (EPIPE == errno) ||
                     (ENOTSOCK == errno) ||
                     (EBADF == errno) ||
                     (EIEIO == errno))
                {
                    QCLI_Printf(qcli_kpi_handle, "\nError: send_bytes=%d, errno=%d\n", send_bytes, errno);
                    kpi_ctx->buffer = NULL;
                    is_test_done = 1;
                    kpi_get_time(&kpi_ctx->pktStats.last_time);
                    break;
                }
                else
                {
                    if ( ENOBUFS != errno )
                    {
                        QCLI_Printf(qcli_kpi_handle, "\nFailed to qapi_send, send_bytes=%d, errno=%d\n", send_bytes, errno);
                    }

                    /* severe push back, let the other processes run (won't happen on blocking sockets) */
                    qapi_Task_Delay(1000);
                }
            }
            else
            {
                cur_packet_number ++;
                total_packet_count++;
            }

            /* Useful notes related to UDP zerocopy send:
             *
             * In UDP zerocopy send,  we can have the following qapi_send() return codes:
             * IPv4:
             *  return_code < 0: the buffer was not sent, qapi_send() freed the buffer
             *  return_code == packet_size: the buffer was sent successfully, qapi_send() freed the buffer
             *  return_code < packet_size: the buffer sent partially, qapi_send() freed the buffer
             * IPv6:
             *  return_code < 0: the buffer was not sent, qapi_send() freed the buffer
             *  return_code == packet_size: the buffer was sent successfully, qapi_send() freed the buffer
             */
            if (zerocopy_send)
            {
                kpi_ctx->buffer = NULL;
            }

            if (send_bytes > 0)
            {
                kpi_ctx->pktStats.bytes += send_bytes;
                ++n_send_ok;
            }

            /*Test mode can be "number of packets" or "fixed time duration"*/
            if (kpi_ctx->params.tx_params.test_mode == PACKET_TEST)
            {
                if ((cur_packet_number >= kpi_ctx->params.tx_params.packet_number))
                {
                    kpi_get_time(&kpi_ctx->pktStats.last_time);

                    /* optimization, KF can be put in sleep here, before going to suspend */

                    kpi_suspend_time(); /* now suspend for rest of 1 second interval */
                    cur_packet_number = 0;
                }
            }

            if (kpi_ctx->params.tx_params.interval_us)
                qapi_Task_Delay(kpi_ctx->params.tx_params.interval_us);

            /* end the test */
            if(total_packet_count > kpi_ctx->total_packet_count) {
                is_test_done = TRUE;
                break;
            }

        } while ( !((is_test_done) || (send_bytes == packet_size) || (NULL == kpi_ctx->buffer)) );   /* send loop */

    } /* while ( !is_test_done ) */

    if ( kpi_ctx->buffer )
    {
        qapi_Net_Buf_Free(kpi_ctx->buffer, netbuf_id);
    }

ERROR_2:

    /* send endmark packet to print details stats on ath_console side */
    ((KPI_EOT_PACKET*)endmark)->code    = END_OF_TEST_CODE;
    ((KPI_EOT_PACKET*)endmark)->packet_count = htonl(total_packet_count);

    qapi_send(kpi_ctx->sock_peer, (char *)endmark, sizeof(KPI_EOT_PACKET), 0);

    qapi_socketclose(kpi_ctx->sock_peer);

ERROR_1:
    QCLI_Printf(qcli_kpi_handle, "Test Completed \n");

    return;
}




