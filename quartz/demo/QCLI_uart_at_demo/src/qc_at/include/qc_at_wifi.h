/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QC_AT_WIFI_H
#define __QC_AT_WIFI_H

#include <qcli_api.h>
#include <qcli_util.h>

void Initialize_ATW_Demo(void);
int32_t arp_offload(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
int32_t ns_offload(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
int32_t p2p_passphrase(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

extern int32_t Util_String_To_Hex(void *dest_addr, void *src_addr, uint32_t len);

#endif
