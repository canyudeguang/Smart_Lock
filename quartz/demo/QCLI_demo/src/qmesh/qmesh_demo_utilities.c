/******************************************************************************
Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.
******************************************************************************/

#include "qmesh_demo_utilities.h"
#include "qmesh_demo_debug.h"
#include "qmesh_demo_menu.h"

/******************************************************************************
 *Common utilities
 *****************************************************************************/
/* Convert ASCII to HEX string */
static unsigned char ascii_to_hex_char(char val)
{
    if('0' <= val && '9' >= val){
        return (unsigned char)(val - '0');
    }else if('a' <= val && 'f' >= val){
        return (unsigned char)(val - 'a') + 0x0a;
    }else if('A' <= val && 'F' >= val){
        return (unsigned char)(val - 'A') + 0x0a;
    }

    return 0xff;/* error */
}

char AsciiToHex(char *buf, int bufLen, uint8_t *outBuf)
{
    int j,r;
	int l = 0;
    int val;
    int k = 0;

    if(buf==NULL || outBuf==NULL)
    {
        return 0xff;
    }

    for(j=0; j < bufLen; j++)
    {
        val = ascii_to_hex_char(buf[j]);

        if(val == 0xff)
        {
            return 0xff;
        }
        else
        {
            if((j%2) == 0)
            {
                val <<= 4;
                l = val;
            }
            else
            {
                r = val;
                outBuf [k] = l | r;
                k++;
            }
        }
    }

    return k;
}

char AsciiToHexU16_BE(char *buf, int bufLen, uint16_t *outBuf)
{
    char len = AsciiToHex(buf, bufLen, (uint8_t *) outBuf);

    if(len != 0xFF)
    {
        uint16_t *endOutBuf;
        uint8_t msb, lsb;

        len = (len + 1) / 2;
        for(endOutBuf = outBuf + len;
            outBuf < endOutBuf;
            outBuf++)
        {
            msb = *outBuf & 0xFF;
            lsb = (*outBuf >> 8) & 0xFF;
            *outBuf = (msb << 8) | lsb;
        }
    }

    return len; 
}


/* Prints QCLI Parameters */
void PrintQCLIParameters(uint32_t Parameter_Count, 
                                QCLI_Parameter_t *Parameter_List)
{
    int i;
    for(i=0; i<Parameter_Count; i++)
    {
        QCLI_LOGI (mesh_group, "Param[%d] : [%d]  [%d]  [%s]\n", 
                   i, 
                   Parameter_List[i].Integer_Value, 
                   Parameter_List[i].Integer_Is_Valid,
                   Parameter_List[i].String_Value);
    }
}

/* Displays a function error message. */
void DisplayFunctionError(char *Function, int Status)
{
    QCLI_LOGE (mesh_group, "%s Failed: %d.\n", Function, Status);
}

void DisplayU16Array(const uint16_t *arr, uint16_t len)
{
    const uint16_t *end_arr;

    for(end_arr = arr + len; arr < end_arr; arr++)
    {
        QCLI_LOGE (mesh_group, " %04x", *arr);
    }
}

void DisplayU32Array(const uint32_t *arr, uint16_t len)
{
    const uint32_t *end_arr;

    for(end_arr = arr + len; arr < end_arr; arr++)
    {
        QCLI_LOGE (mesh_group, " %08x", *arr);
    }
}