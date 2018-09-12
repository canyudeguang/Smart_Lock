/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/*****< qapi_ble_bttypes.h >***************************************************/
/*      Copyright 2015 - 2016 Qualcomm Technologies, Inc.                     */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  qapi_ble_bttypes - Common Bluetooth Defined Types.                        */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   08/28/15  T. Cook        Initial creation.                               */
/******************************************************************************/
#ifndef __QAPI_BLE_BTTYPES_H__
#define __QAPI_BLE_BTTYPES_H__

   /* Miscellaneous defined type declarations.                          */

   /* Definitions for compilers that required structure to be explicitly*/
   /* declared as packed.                                               */

   /* Check for ARM Real View.                                          */
#ifdef __ARMCC_VERSION
   #define __QAPI_BLE_PACKED_STRUCT_BEGIN__   __packed
#else
   #define __QAPI_BLE_PACKED_STRUCT_BEGIN__
#endif

   /* Check for GCC.                                                    */
#ifdef __GNUC__
   #define __QAPI_BLE_PACKED_STRUCT_END__     __attribute__ ((packed))
#else
   #define __QAPI_BLE_PACKED_STRUCT_END__
#endif

#define __QAPI_BLE_BTTYPESH_INC__

   /* Force ALL Structure Declarations to be Byte Packed (noting the    */
   /* current Structure Packing).                                       */

#include "qapi_ble_basetypes.h"
#include "qapi_ble_btbtypes.h"

   /* Restore Structure Packing.                                        */

#undef __QAPI_BLE_BTTYPESH_INC__


#endif
