/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*-------------------------------------------------------------------------
 *  Include Files
 *-----------------------------------------------------------------------*/
#ifndef __QC_API_MQTT_H
#define __QC_API_MQTT_H

#include <stdlib.h>
#include <string.h>

#include "qcli_api.h"
#include "qurt_signal.h"
#include "net_bench.h"
#include "net_utils.h"
#include "qapi_mqttc.h"


/*-------------------------------------------------------------------------
 *  Function prototypes
 *-----------------------------------------------------------------------*/
QCLI_Command_Status_t qc_api_mqttc_init(uint32_t Parameter_Count, char *ca_file);
QCLI_Command_Status_t qc_api_mqttc_shut(void);
QCLI_Command_Status_t qc_api_mqttc_new(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_mqttc_destroy(uint32_t Parameter_Count, int32_t id);
QCLI_Command_Status_t qc_api_mqttc_config(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_mqttc_connect(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_mqttc_sub(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_mqttc_pub(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_mqttc_unsub(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qc_api_mqttc_disconnect(uint32_t Parameter_Count, int32_t id);
QCLI_Command_Status_t qc_api_mqttc_sslconfig(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

#endif // _MQTT_API_H_
