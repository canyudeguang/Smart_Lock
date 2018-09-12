/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

#ifndef __MODEL_CLIENT_COMMON_H__
#define __MODEL_CLIENT_COMMON_H__

#include "qmesh_types.h"
#include "qmesh.h"
#include "qmesh_hal_ifce.h"

#define WRITE_uint8_t(p, v, len)  p[*len] = (v) & 0x00FF; *len = (*len)+1;

#ifdef   ARCH_BIG_ENDIAN
    #define WRITE_uint16_t(p, v, len) p[*len] = ((v) & 0xFF00)>>8; p[(*len) + 1] = ((v) & 0x00FF); *len = ((*len) + 2);

    #define WRITE_uint32_t(p, v, len) p[*len] = ((v) & 0xFF000000)>>24; p[(*len)+1] = ((v) & 0x00FF0000)>>16; \
                                                p[(*len) +2 ] = ((v) & 0x0000FF00)>>8; p[(*len) +3 ] = ((v) & 0x000000FF); *len = *len + 4;
#else
    #define WRITE_uint16_t(p, v, len) p[*len] = ((v) & 0x00FF); p[(*len) + 1] = (((v) & 0xFF00)>>8); *len = ((*len) + 2);

    #define WRITE_uint32_t(p, v, len) p[*len] = ((v) & 0x000000FF); p[(*len)+1] = ((v) & 0x0000FF00)>>8; \
                                                p[(*len) +2 ] = ((v) & 0x00FF0000)>>16; p[(*len) +3 ] = ((v) & 0xFF000000)>>24; *len = *len + 4;
#endif

#define WRITE_int16_t WRITE_uint16_t
#define WRITE_int32_t WRITE_uint32_t
#define WRITE_Opcode8(p, v)     p[0] = (v) & 0x00FF;
#define WRITE_Opcode16(p, v)   p[0] = ((v) & 0xFF00)>>8; p[1] = ((v) & 0x00FF);


#define READ_uint8_t(p)  (uint8_t)(*(p));

#ifdef   ARCH_BIG_ENDIAN
#define READ_uint16_t(p)   ( ((uint16_t)*(p))<<8 | ((uint16_t)*(p+1)) );
#define READ_uint24_t(p)   ( ((uint24_t)*(p))<<16  | ((uint24_t)*(p+1))<<8 |  ((uint24_t)*(p+2)) );
#define READ_uint32_t(p)   ( ((uint32_t)*(p))<<24  | ((uint32_t)*(p+1))<<16  | ((uint32_t)*(p+2))<<8 |  ((uint32_t)*(p+3)) );

#else
#define READ_uint16_t(p)   ( ((uint16_t)*(p+1))<<8 |  ((uint16_t)*(p)) );
#define READ_uint24_t(p)   ( ((uint24_t)*(p+2))<<16  | ((uint24_t)*(p+1))<<8 |  ((uint24_t)*(p)) );
#define READ_uint32_t(p)   ( ((uint32_t)*(p+3))<<24  | ((uint32_t)*(p+2))<<16  | ((uint32_t)*(p+1))<<8 |  ((uint32_t)*(p)) );

#endif

#define READ_int8_t READ_uint8_t
#define READ_int16_t READ_uint16_t
#define READ_int24_t READ_uint24_t
#define READ_int32_t READ_uint32_t

/* For backward compatibility */
#define READ_uint8 READ_uint8_t
#define READ_uint16 READ_uint16_t
#define READ_uint24 READ_uint24_t
#define READ_uint32 READ_uint32_t

#define READ_int8 READ_uint8_t
#define READ_int16 READ_uint16_t
#define READ_int24 READ_uint24_t
#define READ_int32 READ_uint32_t


/* 1 and 2 byte opcode based on the len field provided */
#define WRITE_Opcode(p, v, len) \
{ \
    if(len==2)\
       {\
        WRITE_Opcode16(p, v) \
    }\
    else \
    {\
        WRITE_Opcode8(p, v) \
    }\
}

/*! \brief Message header. */
typedef struct
{
    QMESH_APP_KEY_IDX_INFO_T    keyInfo;        /* Application key index */
    bool                        use_frnd_keys;  /* Use of Friend Keys for sending message */
    uint16_t                    srcAddr;        /* Element address */
    uint16_t                    dstAddr;        /* Destination element address */
    uint8_t                     ttl;            /* Time to Live */
    bool                        reliable;       /* Reliable or Unreliable */
} QMESH_MSG_HEADER_T;


/*! \brief Client Model message opcodes. */
typedef enum
{
    /* Generic Models */
    /* Generic OnOff Client Model */
    QMESH_GENERIC_ONOFF_CLIENT_GET                                 =   0X8201,
    QMESH_GENERIC_ONOFF_CLIENT_SET                                 =   0X8202,
    QMESH_GENERIC_ONOFF_CLIENT_SET_UNRELIABLE                      =   0X8203,
    QMESH_GENERIC_ONOFF_CLIENT_STATUS                              =   0X8204,
    /* Generic Level Client Model */
    QMESH_GENERIC_LEVEL_CLIENT_GET                                 =   0X8205,
    QMESH_GENERIC_LEVEL_CLIENT_SET                                 =   0X8206,
    QMESH_GENERIC_LEVEL_CLIENT_SET_UNRELIABLE                      =   0X8207,
    QMESH_GENERIC_LEVEL_CLIENT_STATUS                              =   0X8208,
    QMESH_GENERIC_LEVEL_CLIENT_DELTA_SET                           =   0X8209,
    QMESH_GENERIC_LEVEL_CLIENT_DELTA_SET_UNRELIABLE                =   0X820A,
    QMESH_GENERIC_LEVEL_CLIENT_MOVE_SET                            =   0X820B,
    QMESH_GENERIC_LEVEL_CLIENT_MOVE_SET_UNRELIABLE                 =   0X820C,
    /* Generic Default Transition Time Client Model */
    QMESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_GET               =   0X820D,
    QMESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_SET               =   0X820E,
    QMESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_SET_UNRELIABLE    =   0X820F,
    QMESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_STATUS            =   0X8210,
    /* Generic Power OnOff Client Model */
    QMESH_GENERIC_POWER_ONOFF_CLIENT_ONPOWERUP_GET                 =   0x8211,
    QMESH_GENERIC_POWER_ONOFF_CLIENT_ONPOWERUP_SET                 =   0x8213,
    QMESH_GENERIC_POWER_ONOFF_CLIENT_ONPOWERUP_SET_UNRELIABLE      =   0x8214,
    QMESH_GENERIC_POWER_ONOFF_CLIENT_STATUS                        =   0x8212,
    /* Generic Power Level Client Model */
    QMESH_GENERIC_POWER_LEVEL_CLIENT_GET                           =   0x8215,
    QMESH_GENERIC_POWER_LEVEL_CLIENT_SET                           =   0x8216,
    QMESH_GENERIC_POWER_LEVEL_CLIENT_SET_UNRELIABLE                =   0x8217,
    QMESH_GENERIC_POWER_LEVEL_CLIENT_STATUS                        =   0x8218,
    QMESH_GENERIC_POWER_LEVEL_CLIENT_LAST_GET                      =   0x8219,
    QMESH_GENERIC_POWER_LEVEL_CLIENT_LAST_STATUS                   =   0x821A,
    QMESH_GENERIC_POWER_LEVEL_CLIENT_DEFAULT_GET                   =   0x821B,
    QMESH_GENERIC_POWER_LEVEL_CLIENT_DEFAULT_STATUS                =   0x821C,
    QMESH_GENERIC_POWER_LEVEL_CLIENT_RANGE_GET                     =   0x821D,
    QMESH_GENERIC_POWER_LEVEL_CLIENT_RANGE_STATUS                  =   0x821E,
    QMESH_GENERIC_POWER_LEVEL_CLIENT_DEFAULT_SET                   =   0x821F,
    QMESH_GENERIC_POWER_LEVEL_CLIENT_DEFAULT_SET_UNRELIABLE        =   0x8220,
    QMESH_GENERIC_POWER_LEVEL_CLIENT_RANGE_SET                     =   0x8221,
    QMESH_GENERIC_POWER_LEVEL_CLIENT_RANGE_SET_UNRELIABLE          =   0x8222,

    /* Light Model */
    /* Light Lightnes Client Model */
    QMESH_LIGHT_LIGHTNESS_CLIENT_GET                               =   0x824B,
    QMESH_LIGHT_LIGHTNESS_CLIENT_SET                               =   0x824C,
    QMESH_LIGHT_LIGHTNESS_CLIENT_SET_UNRELIABLE                    =   0x824D,
    QMESH_LIGHT_LIGHTNESS_CLIENT_STATUS                            =   0x824E,
    QMESH_LIGHT_LIGHTNESS_CLIENT_LINEAR_GET                        =   0x824F,
    QMESH_LIGHT_LIGHTNESS_CLIENT_LINEAR_SET                        =   0x8250,
    QMESH_LIGHT_LIGHTNESS_CLIENT_LINEAR_SET_UNRELIABLE             =   0x8251,
    QMESH_LIGHT_LIGHTNESS_CLIENT_LINEAR_STATUS                     =   0x8252,
    QMESH_LIGHT_LIGHTNESS_CLIENT_LAST_GET                          =   0x8253,
    QMESH_LIGHT_LIGHTNESS_CLIENT_LAST_STATUS                       =   0x8254,
    QMESH_LIGHT_LIGHTNESS_CLIENT_DEFAULT_GET                       =   0x8255,
    QMESH_LIGHT_LIGHTNESS_CLIENT_DEFAULT_STATUS                    =   0x8256,
    QMESH_LIGHT_LIGHTNESS_CLIENT_RANGE_GET                         =   0x8257,
    QMESH_LIGHT_LIGHTNESS_CLIENT_RANGE_STATUS                      =   0x8258,
    QMESH_LIGHT_LIGHTNESS_CLIENT_DEFAULT_SET                       =   0x8259,
    QMESH_LIGHT_LIGHTNESS_CLIENT_DEFAULT_SET_UNRELIABLE            =   0x825A,
    QMESH_LIGHT_LIGHTNESS_CLIENT_RANGE_SET                         =   0x825B,
    QMESH_LIGHT_LIGHTNESS_CLIENT_RANGE_SET_UNRELIABLE              =   0x825C,
    /* Light HSL Client Model */
    QMESH_LIGHT_HSL_CLIENT_GET                                     =   0x826D,
    QMESH_LIGHT_HSL_CLIENT_HUE_GET                                 =   0x826E,
    QMESH_LIGHT_HSL_CLIENT_HUE_SET                                 =   0x826F,
    QMESH_LIGHT_HSL_CLIENT_HUE_SET_UNRELIABLE                      =   0x8270,
    QMESH_LIGHT_HSL_CLIENT_HUE_STATUS                              =   0x8271,
    QMESH_LIGHT_HSL_CLIENT_SATURATION_GET                          =   0x8272,
    QMESH_LIGHT_HSL_CLIENT_SATURATION_SET                          =   0x8273,
    QMESH_LIGHT_HSL_CLIENT_SATURATION_SET_UNRELIABLE               =   0x8274,
    QMESH_LIGHT_HSL_CLIENT_SATURATION_STATUS                       =   0x8275,
    QMESH_LIGHT_HSL_CLIENT_SET                                     =   0x8276,
    QMESH_LIGHT_HSL_CLIENT_SET_UNRELIABLE                          =   0x8277,
    QMESH_LIGHT_HSL_CLIENT_STATUS                                  =   0x8278,
    QMESH_LIGHT_HSL_CLIENT_TARGET_GET                              =   0x8279,
    QMESH_LIGHT_HSL_CLIENT_TARGET_STATUS                           =   0x827A,
    QMESH_LIGHT_HSL_CLIENT_DEFAULT_GET                             =   0x827B,
    QMESH_LIGHT_HSL_CLIENT_DEFAULT_STATUS                          =   0x827C,
    QMESH_LIGHT_HSL_CLIENT_RANGE_GET                               =   0x827D,
    QMESH_LIGHT_HSL_CLIENT_RANGE_STATUS                            =   0x827E,
    QMESH_LIGHT_HSL_CLIENT_DEFAULT_SET                             =   0x827F,
    QMESH_LIGHT_HSL_CLIENT_DEFAULT_SET_UNRELIABLE                  =   0x8280,
    QMESH_LIGHT_HSL_CLIENT_RANGE_SET                               =   0x8281,
    QMESH_LIGHT_HSL_CLIENT_RANGE_SET_UNRELIABLE                    =   0x8282
}QMESH_CLIENT_MODEL_EVENT_T;


#endif /* __MODEL_CLIENT_COMMON_H__ */

