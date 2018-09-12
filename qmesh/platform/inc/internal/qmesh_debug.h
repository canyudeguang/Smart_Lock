/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_debug.h
 *  \brief Functions to print core stack debug messages
 *
 */
 /******************************************************************************/

#ifndef __QMESH_DEBUG_H__
#define __QMESH_DEBUG_H__

#include "qmesh_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup Platform_Debug
 * @{
 */

/*----------------------------------------------------------------------------*
 * QmeshDebugWriteString
 */
/*! \brief Prints the given string
 *
 *  \param str const string
 *
 *  \returns Nothing
 */
/*----------------------------------------------------------------------------*/
void QmeshDebugWriteString(const char *str);

/*----------------------------------------------------------------------------*
 * QmeshDebugWriteUint8
 */
/*! \brief Prints the given 8 bit hexadecimal value
 *
 *  \param v_u8 8 bit Unsigned Integer
 *
 *  \returns Nothing
 */
/*----------------------------------------------------------------------------*/
void QmeshDebugWriteUint8(uint8_t v_u8);

/*----------------------------------------------------------------------------*
 * QmeshDebugWriteUint16
 */
/*! \brief Prints the given 16 bit hexadecimal value
 *
 *  \param v_u16  16 bit Unsigned Integer
 *
 *  \returns Nothing
 */
/*----------------------------------------------------------------------------*/
void QmeshDebugWriteUint16(uint16_t v_u16);

/*----------------------------------------------------------------------------*
 * QmeshDebugWriteUint32
 */
/*! \brief Prints the given 32 bit hexadecimal value
 *
 *  \param v_u32 32 bit Unsigned Integer to print
 *
 *  \returns Nothing
 */
/*----------------------------------------------------------------------------*/
void QmeshDebugWriteUint32(uint32_t v_u32);

/*----------------------------------------------------------------------------*
 * QmeshDebugWriteUint8Array
 */
/*! \brief Prints the given array of 8bit hexadecimal value
 *
 *  \param v_u8_arr Array of 8 bit Unsigned Integer to print
 *  \param offset Offset from where print
 *  \param n Length of the array
 *
 *  \returns Nothing
 */
/*----------------------------------------------------------------------------*/
void QmeshDebugWriteUint8Arr(const uint8_t *v_u8_arr, uint16_t offset, uint16_t n);

/*----------------------------------------------------------------------------*
 * QmeshDebugWriteUint16Array
 */
/*! \brief Prints the given array of 16bit hexadecimal value
 *
 *  \param v_u16_arr Array of 16 bit Unsigned Integer to print
 *  \param offset Offset from where print
 *  \param n Length of the array
 *
 *  \returns Nothing
 */
/*----------------------------------------------------------------------------*/
void QmeshDebugWriteUint16Arr(const uint16_t *v_u16_arr, uint16_t offset, uint16_t n);

#ifdef __cplusplus
}
#endif
/*!@} */

#endif /* __QMESH_DEBUG_H__ */

