/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _OTP_PARAM_H
#define _OTP_PARAM_H

#define OTP_STATUS_SUCCESS               0
#define OTP_STATUS_ERR_OTP_OP_FAIL       1
#define OTP_STATUS_ERR_ENC_KEY_LEN       2
#define OTP_STATUS_ERR_PK_HASH_LEN       3
#define OTP_STATUS_ERR_INVLID_PROFILE    4
#define OTP_STATUS_ERR_LAST              5

/*********************************************************************/

/* This API will write KDF related setting to OTP */
uint32_t OTP_set_kdf(uint32_t kdf_enable, uint8_t *enc_key, uint32_t enc_key_len);

/* This API will write secure boot related setting to OTP */
uint32_t OTP_set_secure_boot(uint32_t secure_boot_enable, uint8_t *pk_hash, uint32_t  pk_hask_len);

/* This API will write firmware region write setting to OTP */
uint32_t OTP_set_firmware_region_write_disable(uint32_t disable);

/* This API will write model id to OTP */
uint32_t OTP_set_model_id(uint32_t model_id);

/* This API will write otp profile related setting to OTP */
uint32_t OTP_set_profile(uint32_t otp_profile);

/*********************************************************************/
#endif

