/* Copyright (c) 2017 Qualcomm Technologies, Inc.
All rights reserved.

Confidential and Proprietary - Qualcomm Technologies, Inc.

*/


/**
 * @file qapi_ssl_cert.h
 *
 * @addtogroup qapi_networking_ssl_cert
 * @{
 *
 * @brief   This section describes the QAPIs used for TLS certificate, CA list,
 *          and  Pre-Shared Secret Key (PSK) storage and loading.
 *
 *
 * @}
 */


#ifndef __QAPI_SSL_CERT_H__
#define __QAPI_SSL_CERT_H__

#include <stdint.h>


/** @addtogroup qapi_networking_ssl_cert
@{ */


/**
 * Handle to an SSL object.
 * This is obtained from a call to qapi_Net_SSL_Obj_New(). The handle is
 * freed with a call to qapi_Net_SSL_Obj_Free().
 */
typedef uint32_t qapi_Net_SSL_Obj_Hdl_t;

/** Maximum number of characters in a certificate or CA list name. */
#define QAPI_NET_SSL_MAX_CERT_NAME_LEN (64)

/** Maximum number of file names returned in the qapi_Net_SSL_Cert_List() API. */
#define QAPI_NET_SSL_MAX_NUM_CERTS (10)

/**
 * @brief Structure to hold a preshared key (PSK) used to populate
 * the PSK table. The PSK table is used by TLS PSK cipher suites.
 */
typedef struct __qapi_Net_SSL_PSK_s
{
    uint8_t *identity;
    /**< Identity used for PSK. */
    uint16_t identity_Size;
    /**< Length of the identity field in bytes. */
    uint8_t *psk;
    /**< Preshared key. */
    uint16_t psk_Size;
    /**< Preshared key length in bytes. */
} qapi_Net_SSL_PSK_t;

/**
 * @brief Structure to get a list of certificates stored in nonvolatile memory.
 */
typedef struct __qapi_Net_SSL_Cert_List_s {
    char name[QAPI_NET_SSL_MAX_NUM_CERTS][QAPI_NET_SSL_MAX_CERT_NAME_LEN];
    /**< Certificate name. */
} qapi_Net_SSL_Cert_List_t;

/**
 * @brief SSL certificate type.
 */
typedef enum
{
    QAPI_NET_SSL_INVALID_TYPE_E = 0,
    QAPI_NET_SSL_PEM_CERTIFICATE_WITH_PEM_KEY_E = 1,
    QAPI_NET_SSL_PEM_CERTIFICATE_WITH_TEE_KEYPAIR_E = 2,
    QAPI_NET_SSL_BIN_CERTIFICATE_E = 3,
    QAPI_NET_SSL_PEM_CA_LIST_E = 4,
    QAPI_NET_SSL_BIN_CA_LIST_E = 5,

    QAPI_NET_SSL_CERTIFICATE_E = QAPI_NET_SSL_BIN_CERTIFICATE_E,
    QAPI_NET_SSL_CA_LIST_E = QAPI_NET_SSL_BIN_CA_LIST_E
} qapi_Net_SSL_Cert_Type_t;

/** Maximum certificate authority list entries allowed for conversion to binary format. */
#define QAPI_NET_SSL_MAX_CA_LIST 10

/**
 * SSL certificate information for conversion and storage.
 */
typedef struct qapi_Net_SSL_PEM_Cert_s
{
    uint8_t *cert_Buf;
    /**< Certificate buffer. */
    uint32_t cert_Size;
    /**< Certificate buffer size. */
    uint8_t *key_Buf;
    /**< Private key buffer. */
    uint32_t key_Size;
    /**< Private key buffer size. */
    uint8_t *pass_Key;
    /**< Password phrase. */
} qapi_Net_SSL_PEM_Cert_t;

/**
 * SSL certificate authority list information.
 */
typedef struct qapi_CA_Info_s
{
    uint8_t *ca_Buf;
    /**< Certificate authority list buffer. */
    uint32_t ca_Size;
    /**< Certificate authority list buffer size. */
} qapi_CA_Info_t;

/**
  * SSL certificate authority information for conversion and storage.
  */
typedef struct qapi_Net_SSL_PEM_CA_List_s
{
    uint32_t ca_Cnt;
    /**< Certificate authority list count. */
    qapi_CA_Info_t *ca_Info[QAPI_NET_SSL_MAX_CA_LIST];
    /**< Certificate authority list information. */
} qapi_Net_SSL_PEM_CA_List_t;

/**
  * SSL PSK table information for conversion and storage.
  */
typedef struct qapi_Net_SSL_PSK_Table_s
{
    uint32_t psk_Size;
    /**< PSK table buffer size. */
    uint8_t *psk_Buf;
    /**< PSK table buffer. */
} qapi_Net_SSL_PSK_Table_t;




/**
 * SSL client certificate information in binary format for storage.
 */
typedef struct qapi_Net_SSL_Bin_Cert_s
{
    uint8_t *cert_Buf;
    /**< Certificate buffer. */
    uint32_t cert_Size;
    /**< Certificate buffer size. */
} qapi_Net_SSL_Bin_Cert_t;


/**
 * SSL CA List information in binary format for storage.
 */
typedef struct qapi_Net_SSL_Bin_CA_List_s
{
    uint8_t * ca_List_Buf;
    /**< CA list buffer. */
    uint32_t ca_List_Size;
    /**< CA list buffer size. */
} qapi_Net_SSL_Bin_CA_List_t;


/**
 * SSL PEM certificate with a key coming from the TEE generated keypair.
 */
typedef struct qapi_Net_SSL_PEM_Cert_TEE_Keypair_s
{
    uint8_t * cert_Buf;
    /**< Certificate buffer. */
    uint32_t cert_Size;
    /**< Certificate buffer size. */
    char * tee_Keypair_Name;
    /**< Name of TEE generated key-pair associated with the certificate. */
} qapi_Net_SSL_PEM_Cert_TEE_Keypair_t;

/**
  * SSL general certification information for conversion and
  * storage for certificates, CA lists, and PSK tables.
  */
typedef struct qapi_Net_SSL_Cert_Info_s
{
    qapi_Net_SSL_Cert_Type_t cert_Type;   /**< Certificate type. */
    union
    {
        qapi_Net_SSL_PEM_Cert_t             pem_Cert;           /**< Certificate. */
        qapi_Net_SSL_PEM_CA_List_t          pem_CA_List;        /**< CA list. */
        qapi_Net_SSL_Bin_Cert_t             bin_Cert;
        qapi_Net_SSL_Bin_CA_List_t          bin_CA_List;
        qapi_Net_SSL_PEM_Cert_TEE_Keypair_t pem_Cert_TEE_Keypair;
        qapi_Net_SSL_PSK_Table_t psk_Tbl;           /**< PSK table. */
    } info;                               /**< Certificate information. */
} qapi_Net_SSL_Cert_Info_t;

/**
 * Internal certificate format. See user guide on how to convert
 * certificates to the internal format.
 */
typedef const void * qapi_Net_SSL_Cert_t;

/**
 * Type for the hash of certificate/CA list. This is used with the
 * qapi_Net_SSL_Cert_Hash() function to retrieve the hash of the
 * certificate/CA list. The hash used is SHA256. The size
 * of the hash must be at least QAPI_CRYPTO_SHA256_DIGEST_BYTES.
 */
typedef void * qapi_Net_SSL_Cert_Hash_t;

/**
 * @brief Stores a certificate/CA list data in nonvolatile memory.
 *
 * @param[in] cert_Info    Pointer to #qapi_Net_SSL_Cert_Info_t, which describes the certificate/CA list.
 *
 * @param[in] cert_Name    Name of the certificate/CA list. The maximum length
 *                         allowed is QAPI_NET_SSL_MAX_CERT_NAME_LEN (including
 *                         the NULL character).
 *
 * @return
 * QAPI_OK on success. \n
 * QAPI error value on error.
 */
qapi_Status_t qapi_Net_SSL_Cert_Store(qapi_Net_SSL_Cert_Info_t *cert_Info, const char *cert_Name);

/**
 * @brief Reads a certificate/CA list from nonvolatile memory and adds it to the SSL object.
 *
 * @details Certificates or CA lists must be added before the qapi_Net_SSL_Connect()
 * or qapi_Net_SSL_Accept() API is called.
 *
 * @param[in] hdl    SSL object handle.
 * @param[in] type   Certificate or CA list.
 * @param[in] name   Name of the certificate/CA list to load.
 *
 * @return
 * 0 on success; a QAPI error value on error.
 */
qapi_Status_t qapi_Net_SSL_Cert_Load(qapi_Net_SSL_Obj_Hdl_t hdl, qapi_Net_SSL_Cert_Type_t type, const char * name);

/**
 * @brief Gets a list of certificate/CA list names stored in nonvolatile memory.
 *
 * @details The structure #qapi_Net_SSL_Cert_List_t must be allocated by the
 * caller.
 *
 * @param[in] type      Type of data (certificate/CA list) to list. This could
 *                      be either QAPI_NET_SSL_CERTIFICATE_E or QAPI_NET_SSL_CA_LIST_E.
 * @param[in,out] list  List of file names.
 *
 * @return
 * The number of files, or a negative value on error.
 */
int32_t qapi_Net_SSL_Cert_List(qapi_Net_SSL_Cert_Type_t type, qapi_Net_SSL_Cert_List_t *list);

/**
 * @brief Gets the hash of the certificate/CA list.
 *
 * @details The structure #qapi_Net_SSL_Cert_Hash_t must be allocated by the
 * caller and must be at least CRYPTO_SHA256_DIGEST_BYTES bytes in length.
 * This function returns the SHA256 value of the certificate/CA list specified
 * by name.
 *
 * @param[in] name     Name of the certificate/CA list for which the hash
 *                     is intended.
 * @param[in] type     Type of data (certificate/CA list) to list. This could
 *                     be either QAPI_NET_SSL_CERTIFICATE_E or QAPI_NET_SSL_CA_LIST_E.
 * @param[inout] hash  Caller allocated buffer into which this function will
 *                     put the hash value.
 * @return
 * QAPI_OK on success, or a QAPI error value on error.
 */
qapi_Status_t qapi_Net_SSL_Cert_Get_Hash(const char *name, qapi_Net_SSL_Cert_Type_t type, qapi_Net_SSL_Cert_Hash_t hash);

/**
 * @brief Populates the PSK table used by TLS PSK cipher suites.
 *
 * @param[in] hdl              SSL object handle.
 * @param[in] psk_Entries      Array of preshared keys and their
 *                             corresponding identities.
 * @param[in] num_PSK_Entries Length of psk_entries.
 *
 * @return QAPI_OK on success, or a QAPI error value on error.
 */
qapi_Status_t qapi_Net_SSL_PSK_Table_Set(qapi_Net_SSL_Obj_Hdl_t hdl, qapi_Net_SSL_PSK_t *psk_Entries, uint16_t num_PSK_Entries);


/**
 * @brief Frees and clears the PSK table used by PSK ciphersuites.
 * If the PSK table is set to a new value, this function should
 * be called to clear the old PSK table.
 *
 * @param[in] hdl    SSL object handle.
 *
 * @return
 * None.
 */
void qapi_Net_SSL_PSK_Table_Clear(qapi_Net_SSL_Obj_Hdl_t hdl);

/** @} */

#endif /* __QAPI_SSL_CERT_H__ */
