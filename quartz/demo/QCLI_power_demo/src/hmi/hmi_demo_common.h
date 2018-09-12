/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __HMI_DEMO_COMMON_H__
#define __HMI_DEMO_COMMON_H__

/* This structure represents an entry in the remove device list. It contains
   the extended and short address of the remote device and a flag indicating
   if the device is sleepy. */
typedef struct Device_List_Entry_s
{
   uint64_t ExtAddr;
   uint16_t ShortAddr;
   uint8_t  Flags;
   uint8_t  DeviceIndex;
} Device_List_Entry_t;

#define DEVICE_LIST_ENTRY_FLAG_ENTRY_IN_USE        (0x0001)
#define DEVICE_LIST_ENTRY_FLAG_IS_SLEEPY           (0x0002)

/* This structure stores the information for mode transitions. */
typedef struct HMI_Demo_Transition_Data_s
{
   uint32_t            Interface_ID;
   uint16_t            PAN_ID;
   uint16_t            Config;
   uint8_t             KeyIndex;
   uint8_t             Device_Count;
   struct
   {
      uint64_t         Expire_Time;
      uint32_t         Period;
      uint32_t         Device_Index;
      uint32_t         Time_Remaining;
      uint8_t          Flags;
      uint8_t          MSDULength;
      uint8_t          TxOptions;
   } Send_Info;
   Device_List_Entry_t Device_List[];
} HMI_Demo_Transition_Data_t;


#endif

