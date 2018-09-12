/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "qc_drv_main.h"

qc_drv_context *qc_drv_register(qc_drv_cb_t *qc_drv_ops)
{
	qc_drv_context *drv_ctx = malloc(sizeof(qc_drv_context));
	if (!drv_ctx) {
		QCLI_Printf("qc_drv_wifi: qc_drv_context creation failed\n\r");
		return NULL;
	}

	drv_ctx->drv_ops = qc_drv_ops;

	return drv_ctx;
}

void qc_drv_deregister(qc_drv_context *qc_drv_ctx)
{
	if (qc_drv_ctx->drv_ops) {
		free(qc_drv_ctx->drv_ops);
	}

	if (qc_drv_ctx) {
		free(qc_drv_ctx);
	}
}
