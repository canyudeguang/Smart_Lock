/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _OFF_LINE_H_
#define _OFF_LINE_H_

#define MAX_OFFLINE_BUF_SIZE      512
#define OFFLINE_THREAD_PRIORITY   10
#define OFFLINE_THREAD_STACK_SIZE 2048

#define OFFLINE_RECV          (1<<0)
#define OFFLINE_BREACH          (1<<1)

#define DLIST           "dList"
#define GETLIST         "getList"
#define SENSORS         "sensors"
#define DESIRED        "desired"
int32_t Start_offline_thread(void);
void process_request_data(char *, uint32_t data);
int32_t fill_remote_device_info(char *, uint32_t);
int32_t process_offline_data(char *);
int32_t update_temp_data(char *);
int32_t Notify_breach_update_offline(char *);
void signal_set_breach_update(char *); 
int32_t Pir_offline_breach_message(void);
#endif
