/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#ifndef __QC_AT_WIFI_OTA_H
#define __QC_AT_WIFI_OTA_H

#include <qcli_api.h>

QCLI_Command_Status_t qc_at_ota_Fwd(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_at_ota_DeleteFwd(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_at_ota_Trial(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_at_ota_ActiveImage(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_at_ota_FtpUpgrade(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_at_ota_HttpUpgrade(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

#endif
