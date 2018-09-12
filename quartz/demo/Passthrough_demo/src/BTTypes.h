/*
 * Copyright (c) 2000-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __BTTYPESH__
#define __BTTYPESH__

   /* Miscellaneous defined type declarations.                          */

   /* Definitions for compilers that required structure to be explicitly*/
   /* declared as packed.                                               */
#define __PACKED_STRUCT_BEGIN__  __QAPI_BLE_PACKED_STRUCT_BEGIN__

#define __PACKED_STRUCT_END__    __QAPI_BLE_PACKED_STRUCT_END__

#define __BTTYPESH_INC__

   /* Include the QAPI header first.                                    */
#include "qapi_ble_bttypes.h"

   /* Force ALL Structure Declarations to be Byte Packed (noting the    */
   /* current Structure Packing).                                       */
#include "BaseTypes.h"
#include "BTBTypes.h"

   /* Restore Structure Packing.                                        */

#undef __BTTYPESH_INC__

#endif
