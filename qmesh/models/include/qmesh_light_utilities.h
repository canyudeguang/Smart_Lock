/*=============================================================================
 Copyright (c) 2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/******************************************************************************/
/*! \file qmesh_light_utilities.h
 *  \brief defines and functions for light utilities
 *
 *   This file contains utilities for lighting
 */
/******************************************************************************/

#ifndef _QMESH_LIGHT_UTILITIES_H_
#define _QMESH_LIGHT_UTILITIES_H_

#include "qmesh_types.h"

/*!\brief  RGB Value */
typedef struct
{
    int32_t red;   /*!< Red */
    int32_t green; /*!< Green */
    int32_t blue;  /*!< Blue */        
}QMESH_RGB_COLOR;     

/*----------------------------------------------------------------------------*
 * QmeshConvertHSLtoRGB
 */
/*! \brief Convert HSL to RGB
 *
 *  \param [in] hue          hue value \ref int32_t
 *  \param [in] sat          saturation value \ref int32_t
 *  \param [in] lum          luminance value \ref int32_t 
 *
 *  \return \ref QMESH_RGB_COLOR
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RGB_COLOR QmeshConvertHSLtoRGB(int32_t hue, int32_t sat,int32_t lum);

#endif /* _QMESH_LIGHT_UTILITIES_H_ */

