/******************************************************************************
 * sdio.h
 *
 * Header file for SDCC Master side transport interface layer
 *
 * Copyright (c) 2017-2018
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
2017-07-11   mmtd      Initial creation


======================================================================*/

htc_status_t sdio_hal_deinit (trans_device_t *tdev);
htc_status_t sdio_hal_init (trans_device_t *tdev);
