/*                                                                              
 * Copyright (c) 2018 Qualcomm Technologies, Inc.                               
 * All Rights Reserved.                                                         
 * Confidential and Proprietary - Qualcomm Technologies, Inc.                   
 */                                                                             
                                                                                
     
#ifndef __QC_API_WIFI_OTA_H                                                         
#define __QC_API_WIFI_OTA_H 

/*-------------------------------------------------------------------------     
 *  Include Files                                                               
 *-----------------------------------------------------------------------*/
#include "qcli_api.h"
#include "qcli_api.h"
#include "qapi_status.h"
#include "qc_api_main.h"
#include "qc_at_ota_ftp.h"
#include "qc_at_ota_http.h"
#include "qapi_firmware_upgrade.h"
#include "qosa_util.h"

/*-------------------------------------------------------------------------
 * Function prototypes
 *-----------------------------------------------------------------------*/
int32_t qc_api_ota_Fwd(void);
int32_t qc_api_ota_DeleteFwd(int32_t fwd_num);
int32_t qc_api_ota_Trial(QCLI_Parameter_t *Parameter_List);
int32_t qc_api_ota_ActiveImage(int32_t img_id);
int32_t qc_api_ota_FtpUpgrade(QCLI_Parameter_t *Parameter_List);
int32_t qc_api_ota_HttpUpgrade(QCLI_Parameter_t *Parameter_List);

#endif  //QC_API_WIFI_OTA_H 
