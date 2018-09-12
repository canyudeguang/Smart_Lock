/*
* Copyright (c) 2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef __UTIL_H__
#define __UTIL_H__

#define FW_VER_BUF    32
void change_to_upper(char *s);
void get_dev_fw_version(char *buf, int32_t buf_size);
int32_t get_localdevice_name(char *buf,int32_t buf_size);
int32_t get_ble_localdevice_name(char *buf,int32_t buf_size);
int32_t get_remotedevice_name(char *buf, int32_t buf_size);
void restart_device(void);
void print_banner(void);
#endif
