/******************************************************************************
 * spi_hal_api.h
 *
 * Header file for SPI Master side transport interface layer
 *
 * Copyright (c) 2017
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 ******************************************************************************/
/*=============================================================================


                              EDIT HISTORY 

  $Header: 

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2018-01-01   mmtd      Initial creation


======================================================================*/
#ifndef _SPI_HAL_API_H
#define _SPI_HAL_API_H

htc_status_t spi_hal_deinit (trans_device_t *tdev);
htc_status_t spi_hal_init (trans_device_t *tdev);
#endif
