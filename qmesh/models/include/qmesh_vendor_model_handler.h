/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_vendor_model_handler.h
 *  \brief defines and functions for Vendor model Handler
 *
 *   This file contains data types and APIs exposed by the 
 *   Vendor model to the Applications
 */
/******************************************************************************/
#ifndef __QMESH_VENDOR_MODEL_HANDLER_H__
#define __QMESH_VENDOR_MODEL_HANDLER_H__

/*! \addtogroup Vendor Model Application Handler
 * @{
 */

#include "qmesh_model_common.h"

#define QMESH_VENDOR_MODEL_OPCODE_SET_STATE_1             (0xC00001)
#define QMESH_VENDOR_MODEL_OPCODE_GET_STATE_1             (0xC00002)
#define QMESH_VENDOR_MODEL_OPCODE_STATUS_1                   (0xC00003)
#define QMESH_VENDOR_MODEL_OPCODE_SET_UNRELIABLE_STATE_1             (0xC00004)

#define QMESH_VENDOR_MODEL_MAX_DATA_LEN        (380)

/*!\brief  Vendor Model Data */
typedef struct
{
    uint16_t    elm_addr;  /*!<  Element address  */
    uint8_t     data[QMESH_VENDOR_MODEL_MAX_DATA_LEN];    /*!< Model State Value  */
    uint32_t    last_msg_seq_no;  /*!<  Last processed Seq Number */
    uint16_t    last_src_addr;  /*!<  Last Src address processed */
    QMESH_ACCESS_PAYLOAD_KEY_INFO_T key_info;  /*!<  Transport key information  */
} QMESH_VENDOR_MODEL_DATA_T;

/*----------------------------------------------------------------------------*
* QmeshVendorModelStatusSend
*/
/*! \brief Call this API to send vendor model status message.
 *
 *  \param [in] model_context           Pointer to Model Data \ref QMESH_VENDOR_MODEL_DATA_T
 *  \param [in] msg_cmn                 Pointer to Model message details \ref QMESH_MODEL_MSG_COMMON_T
 *  \param [in] len                     Message Data Length
 *
 *  \returns nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshVendorModelStatusSend (
    QMESH_VENDOR_MODEL_DATA_T *model_context,
    QMESH_MODEL_MSG_COMMON_T *msg_cmn, uint16_t len);


/*----------------------------------------------------------------------------*
 * QmeshVendorModelServerHandler
 */
/*! \brief Vendor Model Server Event Handler
 *
 *  \param [in] nw_hdr                  Network header \ref QMESH_NW_HEADER_INFO_T
 *  \param [in] key_info                Pointer to Access layer key \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T
 *  \param [in] elem_data               Pointer to element data \ref QMESH_ELEMENT_CONFIG_T
 *  \param [in] model_data              Pointer to Model context \ref QMESH_MODEL_DATA_T
 *  \param [in] msg                     Pointer to Message Data
 *  \param [in] msg_len                 Message Data Length
 *
 *  \returns \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshVendorModelServerHandler (
    const QMESH_NW_HEADER_INFO_T *nw_hdr,
    const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
    const QMESH_ELEMENT_CONFIG_T *elem_data,
    QMESH_MODEL_DATA_T *model_data,
    const uint8_t *msg,
    uint16_t msg_len);

/*----------------------------------------------------------------------------*
 * QmeshVendorModelSet
 */
/*! \brief Calls this API to set the associated state value.
 *
 *  \param [in] context           Pointer to Model Data \ref QMESH_VENDOR_MODEL_DATA_T
 *  \param [in] data              Pointer to Message Data
 *
 *  \returns nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshVendorModelSet (QMESH_VENDOR_MODEL_DATA_T *context, uint8_t *data);

/*----------------------------------------------------------------------------*
 * QmeshVendorModelInit
 */
/*! \brief Call this API to initialize vendor server model.
 *
 *  \param [in] model_data           Pointer to Model Data \ref QMESH_VENDOR_MODEL_DATA_T
 *
 *  \returns nothing
 */
/*----------------------------------------------------------------------------*/
extern void QmeshVendorModelInit (QMESH_VENDOR_MODEL_DATA_T *model_data);

/*!@} */
#endif /* __QMESH_VENDOR_MODEL_HANDLER_H__ */
