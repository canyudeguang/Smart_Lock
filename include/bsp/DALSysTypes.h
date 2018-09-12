#ifndef DALSYSTYPES_H
#define DALSYSTYPES_H
/*==============================================================================
 Copyright (c) 2015 Qualcomm Technologies Incorporated.
           All Rights Reserved.
  Qualcomm Confidential and Proprietary

$Header: //components/rel/core.ioe/1.0/v2/rom/release/api/dal/DALSysTypes.h#1 $
==============================================================================*/

/*------------------------------------------------------------------------------
* Include Files
*-----------------------------------------------------------------------------*/
#include "DALStdDef.h"
#include "DALStdErr.h"

/*=============================================================================
                        TYPEDEFS
=============================================================================*/
typedef uint32 DALSYSPropertyHandle[2];

typedef struct  DALSYSConfig DALSYSConfig;
struct DALSYSConfig
{
    void *pCfgShared;
    void *pCfgMode;
    void *reserved;
};

typedef struct  DALSYSPropertyVar DALSYSPropertyVar;
struct DALSYSPropertyVar
{
    uint32 dwType;
    uint32 dwLen;
    union
    {
        byte *pbVal;
        char *pszVal;
        uint32 dwVal;
        uint32 *pdwVal;
        const void *pStruct;
    }Val;
};

typedef struct DALProps DALProps;
struct DALProps
{
   const byte *pDALPROP_PropBin;
   const void **pDALPROP_StructPtrs;
   uint32 dwDeviceSize; //Size of Devices array
   const void *pDevices; //String Device array 
};

typedef struct DALPropsDir DALPropsDir;
struct DALPropsDir
{
   uint32 dwLen;
   uint32 dwPropsNameSectionOffset;
   uint32 dwPropsStringSectionOffset;
   uint32 dwPropsByteSectionOffset;
   uint32 dwPropsUint32SectionOffset;
   uint32 dwNumDevices;
   uint32 dwPropsDeviceOffset[1][2]; // structure like this will follow...
};


#endif /* DALSYSTYPES_H */
