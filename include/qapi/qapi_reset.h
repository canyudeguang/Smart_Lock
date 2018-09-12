/*
* Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/**
 * @file qapi_reset.h
 *
 * @addtogroup qapi_reset
 * @{
 *
 * @details QAPIs to support system reset functionality
 *
 * @}
 */

#ifndef _QAPI_RESET_H_
#define _QAPI_RESET_H_

/**
* @brief System reset reasons
*
* @details Provides information about various reset reasons
*
*/

typedef enum
{
    BOOT_REASON_PON_COLD_BOOT,  /** <-- Power On Cold boot / Hardware reset*/
    BOOT_REASON_WATCHDOG_BOOT,  /** <-- Reset from Watchdog */
    BOOT_REASON_SW_COLD_BOOT,   /** <-- Software cold reset  */
    BOOT_REASON_UNKNOWN_BOOT,   /** <-- Unknown SW Cold boot  */
    BOOT_REASON_SIZE_ENUM  = 0x7FFFFFFF   /* <-- Size enum to 32 bits. */
} qapi_System_Reset_Result_t;

/** @addtogroup qapi_reset
@{ */


/**
 * @brief Resets the system. This API will not return.
 *
 * @param[in] 
 *
 * @return 
 */
void qapi_System_Reset(void);

/**
 * @brief Resets the Watchdog counter. If watchdog count exceeds 3, 
 *        bootloader will attempt to load the Golden image if present.
 * @param[in] 
 *
 * @return 
 */
void qapi_System_WDTCount_Reset(void);
/**
 * @brief provides the boot or previous reset reason
 *
 * @param[out] reason returns the previous reset reason/cause of current boot
 *
 * @return status QAPI_OK on successful reset reason retrieval
 *         otherwise appropriate error.
 */
qapi_Status_t qapi_System_Get_BootReason(qapi_System_Reset_Result_t *reason);
/** @} */

#endif /* _QAPI_RESET_H_ */
