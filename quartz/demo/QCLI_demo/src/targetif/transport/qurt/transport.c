/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * 2015-2016 Qualcomm Atheros, Inc.
 * All Rights Reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "com_dtypes.h"
#include "qapi_types.h"
#include "qapi_status.h"
#include "osal_types.h"
#include "htc_defs.h"
#include "transport.h"
#include "malloc.h"
#include "sdio_hal_api.h"
#include "spi_hal_api.h"
#include "string.h"

#define SPI_TRANSPORT
//#define SDIO_TRANSPORT

htc_status_t qapi_to_htcstatus(	qapi_Status_t	qapistatus)
{
	htc_status_t  status = HTC_ERROR;
	switch(qapistatus)
	{
	case  QAPI_OK:
	    status = HTC_OK;
		break;
	case  QAPI_ERROR:
		status = HTC_ERROR;
		break;
	case  QAPI_ERR_NO_MEMORY:
	    status = HTC_NO_MEMORY;
		break;
	case  QAPI_ERR_BUSY:
	    status = HTC_EBUSY;
		break;
	case  QAPI_ERR_NO_RESOURCE:
		status = HTC_NO_RESOURCE;
		break;
	case  QAPI_ERR_INVALID_PARAM:
	    status = HTC_EINVAL;
		break;
	case  QAPI_ERR_TIMEOUT:
	    status = HTC_ERROR;
		break;
	case  QAPI_ERR_NO_ENTRY:
	    status = HTC_ENOENT;
		break;
	case  QAPI_ERR_NOT_SUPPORTED:
	    status = HTC_ENOTSUP;
		break;
	case  QAPI_ERR_BOUNDS:
	    status = HTC_EINVAL;
		break;
	case  QAPI_ERR_BAD_PAYLOAD:
	    status = HTC_EINVAL;
		break;	
	case  QAPI_ERR_EXISTS:
	    status = HTC_EEXIST;
		break;	
	}
	
	return status;
}


/* Called once during host initialization */
htc_status_t trans_init(hif_callbacks_t *callbacks) {

	htc_status_t ret = HTC_OK;
	trans_device_t *tdev = NULL;

    /* We dont have hotplug mechanism in place, as Quartz does not have one for
       SDIO/SPI, we are assuming two targets are statically wired during boot */
    /* SDIO Instance */
#ifdef SDIO_TRANSPORT    
	tdev = (trans_device_t*) malloc(sizeof(trans_device_t));

	if (tdev == NULL) {
		return HTC_ERROR;
	}
	
	memset (tdev, 0, sizeof(trans_device_t));
	
	/* Populate transfer context from devcfg */	
	tdev->id = 0; /* SDCC 0 instance */
    tdev->transport = HTC_TRANSPORT_SDIO;

	memcpy(&tdev->hif_callbacks, callbacks, sizeof(hif_callbacks_t));
	
	ret = sdio_hal_init(tdev);

    if (ret != HTC_OK) {
        free(tdev);
    }
#endif

#ifdef SPI_TRANSPORT

    /* SPI Instance */
    /* PS: This is not duplicates, one for SDIO and one for SPI */

	tdev = (trans_device_t*) malloc(sizeof(trans_device_t));

	if (tdev == NULL) {
		return HTC_ERROR;
	}
	
	memset (tdev, 0, sizeof(trans_device_t));
	
	/* Populate transfer context from devcfg */	
	tdev->id = 0; /* SPI 0  instance */
    tdev->transport = HTC_TRANSPORT_SPI;

	memcpy(&tdev->hif_callbacks, callbacks, sizeof(hif_callbacks_t));
	
	ret = spi_hal_init(tdev);

    if (ret != HTC_OK) {
        free(tdev);
    }
#endif

	return ret;
}

/* Called once during host shutdown */
htc_status_t trans_deinit(trans_device_t *tdev) {
	htc_status_t ret = HTC_OK;

    if (tdev->transport == HTC_TRANSPORT_SDIO)
    {
    	ret = sdio_hal_deinit(tdev);
    }
    else
    {
        ret = spi_hal_deinit(tdev);
    }

	free(tdev);

	return ret;
}

void trans_set_hifcontext(trans_device_t *tdev, void *hifdevice) {
	tdev->context = hifdevice;
}

void * trans_get_hifcontext(trans_device_t *tdev) {
	return tdev->context;
}

int trans_reset_target(trans_device_t *tdev) {
	return 0;
}

htc_status_t trans_read_data(trans_device_t *tdev, io_mode_t iomode, unsigned int addr, void *src, int count) {
    return tdev->ops->read_data(tdev, iomode, addr, src, count);
}
htc_status_t trans_write_data(trans_device_t *tdev, io_mode_t iomode, unsigned int addr, void *src, int count) {
    return tdev->ops->write_data(tdev, iomode, addr, src, count);
}
htc_status_t trans_register_interrupt(trans_device_t *tdev, hif_isr_handler_t pfunc) {
    return tdev->ops->register_interrupt(tdev, pfunc);
}
htc_status_t trans_deregister_interrupt(trans_device_t *tdev) {
    return tdev->ops->deregister_interrupt(tdev);
}

htc_status_t trans_enable_trans (trans_device_t *tdev) {
    return tdev->ops->enable_trans(tdev);
}
htc_status_t trans_disable_trans (trans_device_t *tdev) {
    return tdev->ops->disable_trans(tdev);
}
