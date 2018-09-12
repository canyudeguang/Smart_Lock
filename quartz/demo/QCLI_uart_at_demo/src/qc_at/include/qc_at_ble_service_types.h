/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __BLE_OTA_SERVICE_TYPES__
#define __BLE_OTA_SERVICE_TYPES__

#if !defined(__PACKED_STRUCT_BEGIN__) && !defined(__PACKED_STRUCT_END__)
      /* Packed structure macros. */
   #if (defined(__CC_ARM) || defined(__GNUC__))
      /* Packed macros for ARM and GCC. */
      #define __PACKED_STRUCT_BEGIN__
      #define __PACKED_STRUCT_END__       __attribute__ ((packed))
   #elif (defined(__ICCARM__))
      /* Packed macros for IAR. */
      #define __PACKED_STRUCT_BEGIN__     __packed
      #define __PACKED_STRUCT_END__
   #else
      #define __PACKED_STRUCT_BEGIN__
      #define __PACKED_STRUCT_END__
   #endif
#endif

#define BLE_OTA_COMMAND_QUERY_IMAGE_REQUEST           0x01
#define BLE_OTA_COMMAND_QUERY_IMAGE_RESPONSE          0x02
#define BLE_OTA_COMMAND_READ_IMAGE_DATA_REQUEST       0x03
#define BLE_OTA_COMMAND_READ_IMAGE_DATA_RESPONSE      0x04

typedef __PACKED_STRUCT_BEGIN__ struct BLE_OTA_Command_Query_Image_Request_s
{
   uint8_t  Command;
   uint32_t Version;
   uint8_t  FileNameLength;
   uint8_t  FileName[];
} __PACKED_STRUCT_END__ BLE_OTA_Command_Query_Image_Request_t;

#define BLE_OTA_COMMAND_QUERY_IMAGE_REQUEST_SIZE(FileNameLength)           (sizeof(BLE_OTA_Command_Query_Image_Request_t) + FileNameLength)

typedef __PACKED_STRUCT_BEGIN__ struct BLE_OTA_Command_Query_Image_Response_s
{
   uint8_t  Command;
   uint8_t  Status;
   uint32_t ImageID;
   uint32_t ImageLength;
   uint32_t Version;
} __PACKED_STRUCT_END__ BLE_OTA_Command_Query_Image_Response_t;

#define BLE_OTA_COMMAND_QUERY_IMAGE_RESPONSE_SIZE                          (sizeof(BLE_OTA_Command_Query_Image_Response_t))

typedef __PACKED_STRUCT_BEGIN__ struct BLE_OTA_Command_Read_Image_Data_Request_s
{
   uint8_t  Command;
   uint32_t ImageID;
   uint32_t FileOffset;
   uint16_t DataLength;
} __PACKED_STRUCT_END__ BLE_OTA_Command_Read_Image_Data_Request_t;

#define BLE_OTA_COMMAND_READ_IMAGE_DATA_REQUEST_SIZE                       (sizeof(BLE_OTA_Command_Read_Image_Data_Request_t))

typedef __PACKED_STRUCT_BEGIN__ struct BLE_OTA_Command_Read_Image_Data_s
{
   uint8_t  Command;
   uint8_t  Status;
   uint32_t ImageID;
   uint32_t FileOffset;
   uint16_t DataLength;
   uint8_t  Data[];
} __PACKED_STRUCT_END__ BLE_OTA_Command_Read_Image_Data_Response_t;

#define BLE_OTA_COMMAND_READ_IMAGE_DATA_RESPONSE_SIZE(DataLength)          (sizeof(BLE_OTA_Command_Read_Image_Data_Response_t) + DataLength)

#endif // __BLE_OTA_SERVICE_TYPES__
