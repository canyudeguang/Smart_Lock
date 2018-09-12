/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QC_API_MAIN_H
#define __QC_API_MAIN_H

#include "qc_drv_main.h"

typedef struct qc_api_ctx {
	qc_drv_context *drv_ctx;
}qc_api_ctx;

extern void qc_api_set_drv_ctx(qc_drv_context *drv_ctx);
extern qc_api_ctx * qc_api_get_qc_api_ctx();
extern qc_drv_context *qc_api_get_qc_drv_context();
extern void qc_api_free_qc_api_ctx();

#endif
