/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
 
#ifndef TLSIO_QCA402X_H
#define TLSIO_QCA402X_H

/* This is a template file used for porting */

/* Make sure that you replace tlsio_template everywhere in this file with your own TLS library name (like tlsio_mytls) */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/umock_c_prod.h"

MOCKABLE_FUNCTION(, const IO_INTERFACE_DESCRIPTION*, tlsio_qca402x_get_interface_description);
const IO_INTERFACE_DESCRIPTION* tlsio_qca402x_get_interface_description(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TLSIO_QCA402X_H */
