/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
   @brief This function registers the WiFi Commands with QCLI.
*/
void Initialize_WIFI_Demo(void);
int32_t arp_offload(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
int32_t ns_offload(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
int32_t p2p_passphrase(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern int32_t Util_String_To_Hex(void *dest_addr, void *src_addr, uint32_t len);
