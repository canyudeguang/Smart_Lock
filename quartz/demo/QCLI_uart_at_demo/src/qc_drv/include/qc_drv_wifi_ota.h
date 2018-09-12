/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QC_DRV_WIFI_OTA_H
#define __QC_DRV_WIFI_OTA_H

#include "qc_drv_main.h"

qapi_Status_t qc_drv_Fw_Upgrade_Get_Image_ID(qc_drv_context *qc_drv_ctx,
        qapi_Part_Hdl_t hdl, uint32_t *result);
qapi_Status_t qc_drv_Fw_Upgrade_Get_Image_Version(qc_drv_context *qc_drv_ctx,
        qapi_Part_Hdl_t hdl, uint32_t *result);
qapi_Status_t qc_drv_Fw_Upgrade_Get_Partition_Start(qc_drv_context *qc_drv_ctx,
        qapi_Part_Hdl_t hdl, uint32_t *result);
qapi_Status_t qc_drv_Fw_Upgrade_Get_Partition_Size(qc_drv_context *qc_drv_ctx,
        qapi_Part_Hdl_t hdl, uint32_t *size);
qapi_Status_t qc_drv_Fw_Upgrade_Read_Partition(qc_drv_context *qc_drv_ctx,
        qapi_Part_Hdl_t hdl, int32_t val, char *buf, int32_t buf_size,
        uint32_t *size);
qapi_Status_t qc_drv_Fw_Upgrade_init(qc_drv_context *qc_drv_ctx);
qapi_Status_t qc_drv_Fw_Upgrade_Get_Active_FWD(qc_drv_context *qc_drv_ctx,
        uint32_t *boot_type, uint32_t *fwd_present);
qapi_Status_t qc_drv_Fw_Upgrade_Get_FWD_Magic(qc_drv_context *qc_drv_ctx,
        int32_t Index, uint32_t *magic);
qapi_Status_t qc_drv_Fw_Upgrade_Get_FWD_Rank(qc_drv_context *qc_drv_ctx,
        int32_t Index, uint32_t *Result_u32);
qapi_Status_t qc_drv_Fw_Upgrade_Get_FWD_Version(qc_drv_context *qc_drv_ctx,
        int32_t Index, uint32_t *Result_u32);
qapi_Status_t qc_drv_Fw_Upgrade_Get_FWD_Status(qc_drv_context *qc_drv_ctx,
        int32_t Index, uint8_t *Result_u8);
qapi_Status_t qc_drv_Fw_Upgrade_Get_FWD_Total_Images(qc_drv_context *qc_drv_ctx,
        int32_t Index, uint8_t *Result_u8);
qapi_Status_t qc_drv_Fw_Upgrade_First_Partition(qc_drv_context *qc_drv_ctx,
        int32_t Index, qapi_Part_Hdl_t *hdl);
qapi_Status_t qc_drv_Fw_Upgrade_Next_Partition(qc_drv_context *qc_drv_ctx,
        qapi_Part_Hdl_t hdl, qapi_Part_Hdl_t *hdl_next);
qapi_Status_t qc_drv_Fw_Upgrade_Close_Partition(qc_drv_context *qc_drv_ctx,
        qapi_Part_Hdl_t hdl);
qapi_Status_t qc_drv_Fw_Upgrade_Erase_FWD(qc_drv_context *qc_drv_ctx, 
		int32_t fwd_num);
qapi_Status_t qc_drv_Fw_Upgrade_Done(qc_drv_context *qc_drv_ctx,
        int32_t accept, int32_t flags);
qapi_Status_t qc_drv_Fw_Upgrade_Find_Partition(qc_drv_context *qc_drv_ctx,
        uint8_t Index, int32_t img_id, qapi_Part_Hdl_t *hdl);
qapi_Status_t qc_drv_Fw_Upgrade(qc_drv_context *qc_drv_ctx, char *iface_name,
        qapi_Fw_Upgrade_Plugin_t *plugin, char *url, char *cfg_file,
        int32_t flags, qapi_Fw_Upgrade_CB_t cb, void *init_param);

#endif
