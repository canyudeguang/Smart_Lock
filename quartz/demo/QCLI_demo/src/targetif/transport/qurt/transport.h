/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * 2015-2016 Qualcomm Atheros, Inc.
 * All Rights Reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include "qapi_status.h"
#include "qapi_spi_master.h"
#include "spi_hal.h"

typedef enum {
    HTC_TRANSPORT_SDIO = 0,
    HTC_TRANSPORT_SPI
}HTC_TARGET_TRANSPORT;

typedef enum {
	FIFO_MODE,
	MEMORY_MODE
} io_mode_t;


typedef void (*hif_isr_handler_t) (void *pcontext);
typedef htc_status_t (*read_data_t) (void *tdev, io_mode_t iomode, unsigned int addr, void *src, int count);
typedef htc_status_t (*write_data_t) (void *tdev, io_mode_t iomode, unsigned int addr, void *src, int count);
typedef htc_status_t (*register_interrupt_t)  (void *tdev, hif_isr_handler_t pfunc);
typedef htc_status_t (*deregister_interrupt_t)  (void *tdev);
typedef htc_status_t (*enable_func_t) (void *tdev);
typedef htc_status_t (*disable_func_t) (void *tdev);


typedef struct trans_dev_ops {
   read_data_t  read_data;
   write_data_t write_data;
   register_interrupt_t  register_interrupt;
   deregister_interrupt_t  deregister_interrupt;
   enable_func_t enable_trans;
   disable_func_t disable_trans;
} trans_dev_ops_t;

typedef htc_status_t (*hif_trans_device_func_t)(void *dev, OSAL_BOOL hotplug);

typedef struct hif_callbacks {
    hif_trans_device_func_t hif_trans_device_inserted;
    hif_trans_device_func_t hif_trans_device_removed;
} hif_callbacks_t;

typedef struct trans_device {
    uint32 id; 
    trans_dev_ops_t *ops;
    void *context;
	void *dev;
	hif_callbacks_t hif_callbacks;
    HTC_TARGET_TRANSPORT transport;
} trans_device_t;

/* prototypes */
int trans_reset_target(trans_device_t *tdev);
void trans_set_hifcontext(trans_device_t *tdev, void *hifdevice);
void * trans_get_hifcontext(trans_device_t *tdev);


/* Inlines */
htc_status_t trans_read_data(trans_device_t *tdev, io_mode_t iomode, unsigned int addr, void *src, int count);
htc_status_t trans_write_data(trans_device_t *tdev, io_mode_t iomode, unsigned int addr, void *src, int count);
htc_status_t trans_register_interrupt(trans_device_t *tdev, hif_isr_handler_t pfunc);
htc_status_t trans_deregister_interrupt(trans_device_t *tdev);
htc_status_t trans_enable_trans (trans_device_t *tdev);
htc_status_t trans_disable_trans (trans_device_t *tdev);

/* Called once during host initialization */
htc_status_t trans_init(hif_callbacks_t *callbacks);

/* Called once during host shutdown */
htc_status_t trans_deinit(trans_device_t *tdev);

htc_status_t qapi_to_htcstatus(	qapi_Status_t	qapistatus);

#endif
