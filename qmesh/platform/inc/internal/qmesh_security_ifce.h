/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_security_ifce.h
 *  \brief security interface related function and type defines.
 *
 *   This file contains the security interface related function and type defines.
 */
 /******************************************************************************/

#ifndef __QMESH_SECURITY_IFCE_H__
#define __QMESH_SECURITY_IFCE_H__

#include "qmesh_data_types.h"
#include "qmesh_result.h"

/*! \addtogroup Platform_Security
 * @{
 */

/*! \name Security Block sizes
 *
 * \{
 */
#define AES_BLOCK_SIZE_OCTETS      (16U)
#define AES_BLOCK_SIZE_HWORDS      (8U)
#define NONCE_SIZE_OCTETS          (13U)

#define EC256_KEY_BLOCK_SIZE       (17U * sizeof(uint16_t))
/*! \} */

/*! Get Number of AES blocks from length */
#define GET_NUM_AES_BLOCKS(x)      (((x) + (AES_BLOCK_SIZE_OCTETS - 1)) / AES_BLOCK_SIZE_OCTETS)

/* Spec currently uses 128 bit aad derived from 128 bit UUID.
 * When aad data is present it spans across two blocks (B1 and B2)
 */
/*! Additional data in blocks length */
#define AAD_DATA_BLOCKS_LENGTH      (2)

/*! \brief Elliptic Curve Affine representation of a point */
typedef struct
{
    uint16_t a_x[EC256_KEY_BLOCK_SIZE]; /*!< \brief x co-ordinate of affine point */
    uint16_t a_y[EC256_KEY_BLOCK_SIZE]; /*!< \brief y co-ordinate of affine point */
} EC_256_AFFINE_POINT_T;


/*! \brief ECC Callback Event Type */
typedef enum
{
    QMESH_ECC_KEY_PAIR_CFM              = 0,  /*!< Mesh Ecc Key Pair Confirm */
    QMESH_ECC_SHARED_SECRET_CFM         = 1,  /*!< Mesh ECC Shared Secret confirm */
} QMESH_ECC_CB_EVENT_T;

/*----------------------------------------------------------------------------*
 * AES_ECB
 */
/*! \brief This function encrypts/decrypts data using AES128 ECB
 *
 *  \param input Data to be encrypted or decrypted
 *  \param key   Pointer to the buffer containing the encryption key
 *  \param output Pointer to the buffer where output is to be stored
 *  \param encrypt Boolean. TRUE to encrypt. FALSE to decrypt
 *
 *  \returns Nothing
 */
/*---------------------------------------------------------------------------*/
extern void AES_ECB(const uint8_t *input, const uint16_t *key, uint8_t *output, bool encrypt);



/*----------------------------------------------------------------------------*
 * AES_CMAC
 */
/*! \brief Computes the AES CMAC for the given data
 *
 *  \param key pointer to the buffer containing the 128 bit key
 *  \param input Pointer to the buffer containing the input data
 *  \param length Length of the input data
 *  \param mac Output poiter to the buffer where the calculated mac will be stored
 *
 *  \returns Nothing
 */
/*----------------------------------------------------------------------------*/
extern void AES_CMAC(const uint16_t *key, const uint8_t *input, uint8_t length, uint8_t *mac);
extern void AES_CMAC_HWORD(const uint16_t *key, const uint8_t *input, uint8_t length, uint16_t *mac);


/*----------------------------------------------------------------------------*
 * AES_CCMEncrypt
 */
/*! \brief Encrypts the data using AES CCM method
 *
 *  \param key     Encryption Key
 *  \param aad     Additional authentication data to be used
 *  \param aad_len Length of the additional authentication data (in hwords)
 *  \param nonce   Nonce [Length 13 octets]
 *  \param input   Pointer to the plain text data to be encrypted. Cypher text will
 *                 be copied over the input. The MIC calculated will be appended
 *                 to the input.
 *  \param len     Length of the input data
 *  \param mic_len Size(octets) of the MIC either 8 or 4 octets
 *
 *  \returns Nothing
 */
/*----------------------------------------------------------------------------*/
extern void AES_CCMEncrypt(const uint16_t *key, const uint16_t *aad, uint16_t aad_len,
                           const uint8_t *nonce, uint8_t *input,
                           uint16_t len, uint8_t mic_len);


/*----------------------------------------------------------------------------*
 * AES_CCMDecrypt
 */
/*! \brief Decrypts the data using AES CCM method
 *
 *  \param key     Encryption Key
 *  \param aad     Additional authentication data to be used
 *  \param aad_len Length of the additional authentication data (in hwords)
 *  \param nonce   Nonce [Length 13 octets]
 *  \param input   Pointer to the cypher text with the MIC. Decrypted data will be
 *                 copied over the input.
 *  \param len     Length of the input data
 *  \param mic_len Size(octets) of the MIC either 8 or 4 octets
 *
 *  \returns TRUE if the MIC is validated. FALSE otherwise
 */
/*----------------------------------------------------------------------------*/
extern bool AES_CCMDecrypt(const uint16_t *key, const uint16_t *aad, uint16_t aad_len,
                           const uint8_t *nonce, uint8_t *input,
                           uint16_t len, uint8_t mic_len);

/*----------------------------------------------------------------------------*
 * EC256_MakeKeypair
 */
/*! \brief Generate a public-private key pair
 *
 *  \param private_key Private Key
 *  \param public_key Generated public key
 *
 *  \returns Nothing
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T EC256_MakeKeypair(uint16_t *private_key,
                                        EC_256_AFFINE_POINT_T *public_key);

/*----------------------------------------------------------------------------*
 * EC256_GetSharedSecret
 */
/*! \brief Generate the shared secret from the public and private key
 *
 *  \param private_key Private Key
 *  \param public_key Generated public key
 *  \param shared_secret Pointer to buffer where the calculated shared secret is stored.
 *
 *  \returns Nothing
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T EC256_GetSharedSecret(uint16_t *private_key,
                                            EC_256_AFFINE_POINT_T *public_key,
                                            EC_256_AFFINE_POINT_T *shared_secret);

/*----------------------------------------------------------------------------*
 * QmeshEccCryptCbHandler
 */
/*! \brief Call back function to inform the stack when ECC keys are generated.
 *         This function is only applicable for aynchronous ECC crypto functions.
 *
 *  \param[in] event_type       Ecc event type.
 *  \param[in] status           Status of operation.
 */
/*---------------------------------------------------------------------------*/
extern void QmeshEccCryptCbHandler(QMESH_ECC_CB_EVENT_T event_type, bool status);

/*!@} */

#endif /* __QMESH_SECURITY_IFCE_H__ */

