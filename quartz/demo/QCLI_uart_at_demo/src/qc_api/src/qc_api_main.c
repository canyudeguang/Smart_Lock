/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "qc_api_main.h"

qc_api_ctx * qc_api_cx = NULL;

void qc_api_set_drv_ctx(qc_drv_context *drv_ctx)
{
	qc_api_cx = malloc(sizeof(qc_api_ctx));
	if (!qc_api_cx) {
		QCLI_Printf("qc_api_wifi: qc_api_context creation failed\n\r");
	}

	qc_api_cx->drv_ctx = drv_ctx;
}

qc_api_ctx * qc_api_get_qc_api_ctx()
{
	return qc_api_cx;
}

qc_drv_context *qc_api_get_qc_drv_context()
{
	if (qc_api_cx) {
		return qc_api_cx->drv_ctx;
	}

	return NULL;
}

void qc_api_free_qc_api_ctx()
{
	if (qc_api_cx) {
		free(qc_api_cx);
		qc_api_cx = NULL;
	}
}
