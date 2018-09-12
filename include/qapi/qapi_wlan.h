/* Copyright (c) 2016  Qualcomm Atheros, Inc.
   All rights reserved.
   Qualcomm Atheros Confidential and Proprietary
*/

#ifndef __QAPI_WLAN_H__
#define __QAPI_WLAN_H__

/**
@file qapi_wlan_base.h
*/

#if ENABLE_P2P_MODE
#include "qapi_wlan_p2p.h" /* This file includes qapi_wlan_base.h internally */
#else
#include "qapi_wlan_base.h"
#endif

#include "qapi_wlan_prof.h"
#include "qapi_wlan_errors.h"

#endif // __QAPI_WLAN_H__
