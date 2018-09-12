/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_comp_utils.h
 *
 *  \brief Qmesh utilities for Composition Data access.
 *
 *   This file contains the API for Composition Data Access utilities.
 */
 /*****************************************************************************/

#ifndef _QMESH_COMP_UTILS_H_
#define _QMESH_COMP_UTILS_H_

/*============================================================================*
    Header Files
 *============================================================================*/

/*============================================================================*
    Macro Definitions
 *============================================================================*/

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/

/*============================================================================*
    Data Type Definitions
 *============================================================================*/

typedef struct
{
    uint8_t                                  net_idx;
    const QMESH_DEVICE_COMPOSITION_DATA_T   *comp_data;
    const QMESH_ELEMENT_CONFIG_T            *ele_cfg;
    QMESH_MODEL_DATA_T                      *model_data;
} QMESH_COMP_SEARCH_CNTXT_T;

/*============================================================================*
    API Definitions
 *============================================================================*/

const QMESH_ELEMENT_CONFIG_T *QmeshGetElementInfo(const QMESH_DEVICE_COMPOSITION_DATA_T *comp_data,
                                                  uint16_t addr);

QMESH_MODEL_DATA_T *QmeshGetModelInfo(const QMESH_ELEMENT_CONFIG_T *ele_cfg,
                                      QMESH_MODEL_TYPE_T model_type, uint32_t model_id);

QMESH_MODEL_DATA_T *QmeshGetModel(uint8_t net_idx, uint16_t addr,
                                  QMESH_MODEL_TYPE_T model_type, uint32_t model_id,
                                  QMESH_RESULT_T *result);

void QmeshInitSearchCntxt(QMESH_COMP_SEARCH_CNTXT_T *search_cntxt, uint8_t net_idx, uint16_t addr);

bool QmeshGetNextModel(QMESH_COMP_SEARCH_CNTXT_T *search_cntxt, bool this_element_only);

bool QmeshGetNextElement(QMESH_COMP_SEARCH_CNTXT_T *search_cntxt);

void QmeshDeInitSearchCntxt(QMESH_COMP_SEARCH_CNTXT_T *search_cntxt);

uint16_t QmeshGetElementAddress(const QMESH_COMP_SEARCH_CNTXT_T *search_cntxt);

void QmeshGetModelID(const QMESH_COMP_SEARCH_CNTXT_T *search_cntxt,
                     QMESH_MODEL_TYPE_T *model_type, uint32_t *model_id);

#endif /* _QMESH_COMP_UTILS_H_ */

