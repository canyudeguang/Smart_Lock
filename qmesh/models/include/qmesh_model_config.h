/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
============================================================================*/

/******************************************************************************/
/*! \file qmesh_model_config.h
 *  \brief
 *  Defines the configuration of the models
 *
 */
/******************************************************************************/
#ifndef __QMESH_MODEL_CONFIG_H__
#define __QMESH_MODEL_CONFIG_H__

#define QMESH_NUMBER_OF_ELEMENTS                (3)

/*!\brief Maximum model instances.This should be updated as per models defined in
 * composition data
 */
#define QMESH_MAX_GENERIC_LEVEL_INSTANCES       (3)
#define QMESH_MAX_GENERIC_DTT_INSTANCES         (1)
#define QMESH_MAX_GENERIC_ONOFF_INSTANCES       (1)
#define QMESH_MAX_GENERIC_POWER_LEVEL_INSTANCES (1)
#define QMESH_MAX_GENERIC_POWER_ONOFF_INSTANCES (1)
#define QMESH_MAX_LIGHTNESS_INSTANCES           (1)

/* If Power Level Model is supported as part of composition data, following flag should be enabled */
#undef QMESH_POWERLEVEL_SERVER_MODEL_ENABLE

#endif
