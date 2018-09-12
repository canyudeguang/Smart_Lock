/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_light_utilities.c
 *  \brief defines and functions for light utilities
 *
 *   This file contains the implementation of utilities for lighting.
 */
/******************************************************************************/
#include "qmesh_light_utilities.h"

#define ABSLT(a)          (((a) < 0) ? -(a) : (a))

/*----------------------------------------------------------------------------*
*  NAME
*      QmeshConvertHSLtoRGB
*
*  DESCRIPTION
*      This function converts HSL to RGB.
*
*  RETURNS/MODIFIES
*      RGB values.
*
*----------------------------------------------------------------------------*/

extern QMESH_RGB_COLOR QmeshConvertHSLtoRGB(int32_t hue, int32_t sat,int32_t lum)
{
    int32_t R1, G1, B1;
    QMESH_RGB_COLOR rgb_val;

    /* Find the Chroma */
    int32_t chroma = ((255 - ABSLT(2 * lum - 255)) * sat) >> 8;

    /* Convert Hue to Degrees */
    int32_t hue_deg = 360 * (hue) / 255;

    /* Check under which one of the six sectors in the hue color cylinder
       the hue value(in degrees) falls. */
    int32_t hue_sector = 0;

    if (hue_deg <= 60)
        hue_sector = 1; /* Sector 1 */
    else if ((hue_deg >= 60) && (hue_deg <= 120))
        hue_sector = 2; /* Sector 2 */
    else if ((hue_deg >= 120) && (hue_deg <= 180))
        hue_sector = 3; /* Sector 3 */
    else if ((hue_deg >= 180) && (hue_deg <= 240))
        hue_sector = 4; /* Sector 4 */
    else if ((hue_deg >= 240) && (hue_deg <= 300))
        hue_sector = 5; /* Sector 5 */
    else if ((hue_deg >= 300) && (hue_deg <= 360))
        hue_sector = 6; /* Sector 6 */

    /* Compute the intermediate value */
    int32_t x = (chroma * (255 - ABSLT(hue_sector % (2 - 1)))) >>8;

    /* Find {R1,G1,B1} along the bottom three faces of the RGB cube
     * with the same hue and chroma as our color(using intermediate value
     * x for nthe second largest component of this color.
     */
    if (hue_sector <= 1)
    {
        R1 = chroma;
        G1 = x;
        B1 = 0;
    }
    else if (hue_sector == 2)
    {
        R1 = x;
        G1 = chroma;
        B1 = 0;
    }
    else if (hue_sector == 3)
    {
        R1 = 0;
        G1 = chroma;
        B1 = x;
    }
    else if (hue_sector == 4)
    {
        R1 = 0;
        G1 = x;
        B1 = chroma;
    }
    else if (hue_sector == 5)
    {
        R1 = x;
        G1 = 0;
        B1 = chroma;
    }
    else if (hue_sector == 6)
    {
        R1 = chroma;
        G1 = 0;
        B1 = x;
    }
    else
    {
        R1 = 0;
        G1 = 0;
        B1 = 0;
    }

    /* Find an amount to calibrate the final R,G,B by adding the same amount
     * to each component
     */
    int32_t m = lum - (chroma / 2);

    R1 = (R1 + m);
    G1 = (G1 + m);
    B1 = (B1 + m);

    rgb_val.red = R1;
    rgb_val.green = G1;
    rgb_val.blue = B1;
    
    return rgb_val;
}