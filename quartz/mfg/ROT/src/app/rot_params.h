/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _ROT_PARAM_H
#define _ROT_PARAM_H

/*This tool allows a user to activate or revoke a root certificate in QCA402X.
  The tool uses KDF APIs to modify Multiple-Root-of-Trust fuses. A PC based
  KDF password generator tool is used to generate the password that unlocks
  the KDF module in the HW. Please refer to QCA402X Security Guide for more 
  details.
*/  

#define ROT_STATUS_SUCCESS               0
#define ROT_STATUS_ERR_PASSWORD_MISMATCH 1
#define ROT_STATUS_ERR_WRITE_FAIL        2
#define ROT_STATUS_ERR_READ_FAIL         3
#define ROT_STATUS_KDF_FAIL              4 


/*This API will read and return the Root of trust vector fuse value in OTP*/
uint32_t get_ROT(uint8_t* rot_vector);

/*This API will update the ROT bits in OTP using KDF operations*/
uint32_t update_ROT(uint8_t* activate_input, uint8_t* revoc_input, uint64_t activate_password, uint64_t revoc_password);


/********************************************************************/
/* CAUTION!!!  The parameters below must be populated based on the out 
   of the KDF password generator tool. An incorrect parameter will lead
   to KDF operation failure.
*/

/****************** ACTIVATION PARAMETERS ****************************/

/*ROT activation password generated by KDF password tool*/   
#define ROT_ACTIVATION_PASSWORD (0x79702BF91CD0E937)


/* Activation Input: 16 bytes. This value should match the 
   sw input parameter provided to kdf password generator for
   opcode 7.
   The lower nibble of the first byte encodes the ROT 
   activation vector value. Each bit corresponds to one Root Certificate. 
   Setting a bit activate the corresponding Root Certificate    */
static uint8_t activate_input[]   = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00,  0x00, 0x00, 0x00};

/****************** REVOCATION PARAMETERS ****************************/
/*ROT revocation password generated by KDF password tool*/
#define ROT_REVOCATION_PASSWORD (0x4D3FC6BE49D3AC87)

/* Revocation Input: 16 bytes. This value should match the 
   sw input parameter provided to kdf password generator for
   opcode 8.
   The lower nibble of the first byte encodes the ROT 
   revocation vector value. Each bit corresponds to one 
   Root Certificate. Setting a bit revoke the corresponding Root Certificate */
static uint8_t revoc_input[]      = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*********************************************************************/
#endif

