// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "azure_c_shared_utility/platform.h"
#include "tlsio_qca402x.h"

int platform_init(void)
{
	return 0;
}

const IO_INTERFACE_DESCRIPTION* platform_get_default_tlsio(void)
{
	
	return (tlsio_qca402x_get_interface_description());
}

void platform_deinit(void)
{
	
}

STRING_HANDLE platform_get_platform_info(void)
{
    return STRING_construct("(qca402x)");
}