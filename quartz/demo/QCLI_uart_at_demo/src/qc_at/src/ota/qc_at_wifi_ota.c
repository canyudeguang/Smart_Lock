/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qc_api_wifi_ota.h"

/*-------------------------------------------------------------------------
 * Below are the wrapper functions for the UART AT Wi-Fi OTA commands
 *-----------------------------------------------------------------------*/
QCLI_Command_Status_t qc_at_ota_Fwd(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if (0 == qc_api_ota_Fwd())
    {
        LOG_AT_OK();
        return QCLI_STATUS_SUCCESS_E;
    }

    LOG_AT_ERROR();
    return QCLI_STATUS_ERROR_E;
}

QCLI_Command_Status_t qc_at_ota_DeleteFwd(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if ((Parameter_Count != 1)  || !Parameter_List || (Parameter_List[0].Integer_Is_Valid == 0))
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_USAGE_E;
    }

    if (Parameter_List[0].Integer_Value > 2)
    {
        LOG_AT("invalid fwd number\r\n");
        LOG_AT_ERROR();
        return QCLI_STATUS_ERROR_E;
    }

    if (0 == qc_api_ota_DeleteFwd(Parameter_List[0].Integer_Value))
    {
        LOG_AT_OK();
        return QCLI_STATUS_SUCCESS_E;
    }

    LOG_AT_ERROR();
    return QCLI_STATUS_ERROR_E;
}

QCLI_Command_Status_t qc_at_ota_Trial(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if ((Parameter_Count != 2)  || !Parameter_List || !Parameter_List[0].Integer_Is_Valid || !Parameter_List[1].Integer_Is_Valid)
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_USAGE_E;
    }

    if ((Parameter_List[0].Integer_Value > 1) || (Parameter_List[1].Integer_Value > 1))
    {
        LOG_AT("Invalid parameter\r\n");
        LOG_AT_ERROR();
        return QCLI_STATUS_USAGE_E;
    }

    if (0 == qc_api_ota_Trial(Parameter_List))
    {
        LOG_AT_OK();
        return QCLI_STATUS_SUCCESS_E;
    }

    LOG_AT_ERROR();
    return QCLI_STATUS_ERROR_E;
}

QCLI_Command_Status_t qc_at_ota_ActiveImage(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if (Parameter_Count != 1 || !Parameter_List || !Parameter_List[0].Integer_Is_Valid)
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_USAGE_E;
    }

    if (0 == qc_api_ota_ActiveImage(Parameter_List[0].Integer_Value))
    {
        LOG_AT_OK();
        return QCLI_STATUS_SUCCESS_E;
    }

    LOG_AT_ERROR();
    return QCLI_STATUS_ERROR_E;
}

QCLI_Command_Status_t qc_at_ota_FtpUpgrade(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if ( Parameter_Count != 4 || !Parameter_List || Parameter_List[0].Integer_Is_Valid || Parameter_List[1].Integer_Is_Valid || Parameter_List[2].Integer_Is_Valid || !Parameter_List[3].Integer_Is_Valid)
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_USAGE_E;
    }

    if (0 == qc_api_ota_FtpUpgrade(Parameter_List))
    {
        LOG_AT_OK();
        return QCLI_STATUS_SUCCESS_E;
    }

    LOG_AT_ERROR();
    return QCLI_STATUS_ERROR_E;
}

QCLI_Command_Status_t qc_at_ota_HttpUpgrade(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if (Parameter_Count != 3 || !Parameter_List || Parameter_List[0].Integer_Is_Valid || Parameter_List[1].Integer_Is_Valid || Parameter_List[2].Integer_Is_Valid)
    {
        LOG_AT_ERROR();
        return QCLI_STATUS_USAGE_E;
    }

    if (0 == qc_api_ota_HttpUpgrade(Parameter_List))
    {
        LOG_AT_OK();
        return QCLI_STATUS_SUCCESS_E;
    }

    LOG_AT_ERROR();
    return QCLI_STATUS_ERROR_E;
}

