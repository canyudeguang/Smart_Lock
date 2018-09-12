/*=============================================================================
 Copyright (c) 2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_ps.h
 *  \brief Qmesh Persistent Storage API.
 *
 *   This file contains the API for Qmesh Persistent Storage API.
 *
 */
 /*****************************************************************************/

#ifndef _QMESH_PS_H_
#define _QMESH_PS_H_

#include "qmesh_data_types.h"
#include "qmesh_result.h"

/*! \addtogroup Platform_PSInterface
* @{
*/

/*============================================================================*
    Macro Definitions
 *============================================================================*/

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/
/*! \brief Persistent storage data type is single or multiple entity. */
typedef enum
{
     QMESH_PS_DATA_SINGLE_FIXED         = 0,     /*!< Single fixed. */
     QMESH_PS_DATA_SINGLE_VARIABLE      = 1,     /*!< Single variable. */
     QMESH_PS_DATA_MULTIPLE_FIXED       = 2      /*!< Multiple fixed. */
}QMESH_PS_DATA_ENTRY_TYPE_T;

/*! \brief Persistent storage keys. */
typedef enum
{
    QMESH_PS_KEY_STACK_GROUP            = 0x00,
    QMESH_PS_KEY_RESET_STATE            = QMESH_PS_KEY_STACK_GROUP,    /*!< Device reset state. */
    QMESH_PS_KEY_DEVICE_KEY_LIST,                                      /*!< Device key information. */
    QMESH_PS_KEY_NETWORK_KEY_LIST,                                     /*!< Network key information. */
    QMESH_PS_KEY_IVI_LIST,                                             /*!< Application key IV List information. */
    QMESH_PS_KEY_TTL,                                                  /*!< TTL */
    QMESH_PS_KEY_NETWORK_RETRANSMIT,                                   /*!< Network Retransmit information */
    QMESH_PS_KEY_RELAY_INFO,                                           /*!< Relay Retransmit and state information */
    QMESH_PS_KEY_APPLICATION_KEY_LIST,                                 /*!< Application key information. */
    QMESH_PS_KEY_BEACON_STATE,                                         /*!< Beacon State */
    QMESH_PS_KEY_FRIEND_STATE,                                         /*!< Friend State */
    QMESH_PS_KEY_HB_PUB_STATE,                                         /*!< Heartbeat Publication State */

    QMESH_PS_KEY_APPLICATION_GROUP      = 0x40,                        /*!< Application information. */

    QMESH_PS_KEY_MODEL_GROUP            = 0x80,                        /*!< Model information. */

    QMESH_PS_KEY_MAXIMUM_VALUE          = 0xFFF                        /*!< Maximum value allowed as a PS key. */
} QMESH_PS_KEY_T;

/*============================================================================*
    Data Type Definitions
 *============================================================================*/

/*! \brief Persistent storage key information used to create a new key. */
typedef struct
{
    QMESH_PS_DATA_ENTRY_TYPE_T  entry_type;     /*!< Type of entry. */
    QMESH_PS_KEY_T              key;            /*!< 12-bit key to distinguish the data on the persistent storage. */
    uint16_t                    entry_size;     /*!< Size of each entry in the block. */
    uint16_t                    max_entries;    /*!< Maximum number of entries in the block. */
} QMESH_PS_KEY_INFO_T;

/*! \brief Persistent storage key header information. */
typedef struct
{
    QMESH_PS_DATA_ENTRY_TYPE_T  entry_type;
    uint16_t                    entry_size;     /*!< Size of each entry in the block. */
    uint16_t                    num_entries;    /*!< Number of entries in the PS key. */
} QMESH_PS_KEY_HEADER_INFO_T;

/*! \brief Function to compare user data against PS keys. */
typedef bool (*QMESH_PS_COMPARE_FN_T)(const void *user_data, const void *p_value);

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
    API Definitions
 *============================================================================*/
 
/*----------------------------------------------------------------------------*
 * QmeshPsInit
 */
/*! \brief Called by the stack to initialize the PS interface,
 *         if any.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshPsInit(void);

/*----------------------------------------------------------------------------*
 * QmeshPsReadyForAccess
 */
/*! \brief The Qmesh stack calls this function to check whether the persistent storage
 *         contains valid values.
 *
 *  The platform must validate the contents of the persistent storage. 
 *  If the persistent storage is corrupt or it is brought up for the
 *  first time, this function must return FAILURE. SUCCESS otherwise.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshPsReadyForAccess(void);

/*----------------------------------------------------------------------------*
 * QmeshPsIsKeyPresent
 */
/*! \brief The Qmesh stack calls this API to check whether the key it needs is present
 *         on the persistent storage.
 *
 *  \param[in] key PS key to be associated with the data type. See \ref QMESH_PS_KEY_T.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshPsIsKeyPresent(QMESH_PS_KEY_T key);

/*----------------------------------------------------------------------------*
 * QmeshPsKeyGetHeaderInfo
 */
/*! \brief The Qmesh stack calls this API to get the header information, such as the current number of
 *         entries or the size of each entry for the key.
 *
 *  \param[in] key PS key to be associated with the data type. See \ref QMESH_PS_KEY_T.
 *  \param[out] key_hdr_info Pointer to the key header information is returned.
 *                           See \ref QMESH_PS_KEY_HEADER_INFO_T.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T
QmeshPsKeyGetHeaderInfo(QMESH_PS_KEY_T key, QMESH_PS_KEY_HEADER_INFO_T *key_hdr_info);

/*----------------------------------------------------------------------------*
 * QmeshPsAddKey
 */
/*! \brief The Qmesh stack calls this API to add a new PS key entry.
 *
 *  \param[in] key_info Pointer to the key information needed to create a new PS key.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshPsAddKey(const QMESH_PS_KEY_INFO_T *key_info);

/*----------------------------------------------------------------------------*
 * QmeshPsDeleteKey
 */
/*! \brief The Qmesh stack calls this API to delete an existing PS key entry.
 *
 *  \param[in] key PS key to be associated with the data type. See \ref QMESH_PS_KEY_T. 
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshPsDeleteKey(QMESH_PS_KEY_T key);

/*----------------------------------------------------------------------------*
 * QmeshPsReadKeyData
 */
/*! \brief Reads a value associated with the key from the persistent storage.
 *
 *  \param[in] key          Key for the persistent storage data to be read. See \ref QMESH_PS_KEY_T.
 *  \param[out] p_value     Pointer to a buffer to which to copy the value.
 *  \param[in, out] buf_len Pointer to the length of the p_value buffer. This parameter is used to
 *                          return the actual length that was read.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T
QmeshPsReadKeyData(QMESH_PS_KEY_T key, void *p_value, size_t *buf_len);

/*----------------------------------------------------------------------------*
 * QmeshPsReadKeyDataMultiple
 */
/*! \brief Reads multiple values associated with the key
 *         from the persistent storage.
 *
 *  \param[in] key          Key for the persistent storage data to be read. See \ref QMESH_PS_KEY_T.
 *  \param[in] start_entry  Starting data number.
 *  \param[in] num_entry    Number of entries to be read when the associated key has multiple entries.
 *                          This value should be either 1 or maximum number of entries returned in
 *                          \ref QmeshPsKeyGetHeaderInfo.
 *  \param[out] p_value     Pointer to a buffer to which to copy the value.
 *  \param[in, out] buf_len Pointer to the length of the p_value buffer. This parameter is used to
 *                          return the actual length that was read. 
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshPsReadKeyDataMultiple(QMESH_PS_KEY_T key, uint16_t start_entry,
                                                 uint16_t num_entry, void *p_value,
                                                 size_t *buf_len);

/*----------------------------------------------------------------------------*
 * QmeshPsAddKeyData
 */
/*! \brief Writes a value associated with the key
 *         from the persistent storage.
 *
 *  \param[in] key              Identifier of the persistent storage parameter to be written.
 *  \param[in] p_value          Pointer to a buffer containing the value to be written.
 *  \param[in, out] buf_len     Pointer to the length of the p_value buffer. This parameter is used to
 *                              return the actual length that was written. 
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T
QmeshPsAddKeyData(QMESH_PS_KEY_T key, const void *p_value, size_t *buf_len);

/*----------------------------------------------------------------------------*
 * QmeshPsAddKeyDataMultiple
 */
/*! \brief Writes multiple values associated with the key
 *         from the persistent storage.
 *
 *  \param[in] key          Identifier of the persistent storage parameter to be written.
 *  \param[in] num_entry    Number of entries to be read when the associated key has multiple entries.
 *  \param[in] p_value      Pointer to a buffer containing the value to be written.
 *  \param[in] buf_len      Pointer to the length of the p_value buffer. This parameter is used to
 *                          return the actual length that was written.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshPsAddKeyDataMultiple(QMESH_PS_KEY_T key, uint16_t num_entry,
                                                const void *p_value, size_t *buf_len);

/*----------------------------------------------------------------------------*
 * QmeshPsUpdateKeyData
 */
/*! \brief Updates a value associated with the key
 *         from the persistent storage.
 *
 *  \param[in] key              Identifier of the persistent storage parameter to be written.
 *  \param[in] fn               Function to compare the data values.
 *  \param[in] user_data        User data against which the key data is to be compared. 
 *  \param[in] current_value    Buffer to be used for copying and comparing the key data from NVM.
 *  \param[in] new_value        Pointer to a buffer containing the value to be written.
 *  \param[in, out] buf_len     Pointer to the length of the new_value buffer. This parameter is used to
 *                              return the actual length that was written.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T
QmeshPsUpdateKeyData(QMESH_PS_KEY_T key, QMESH_PS_COMPARE_FN_T fn, const void *user_data,
                     void *current_value, const void *new_value,
                     size_t *buf_len);

/*----------------------------------------------------------------------------*
 * QmeshPsDeleteKeyData
 */
/*! \brief Deletes a value associated with the key
 *         from the persistent storage.
 *
 *  \param[in] key        Identifier of the persistent storage parameter to be written.
 *  \param[in] fn         Function to compare the data entry.
 *  \param[in] user_data  User data against which the key data is to be compared.
 *  \param[in] p_value    Buffer to be used for copying and comparing the key data from NVM.
 *  \param[in] buf_len    Length of the p_value.
 
 *  \returns \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T
QmeshPsDeleteKeyData(QMESH_PS_KEY_T key, QMESH_PS_COMPARE_FN_T fn, const void *user_data,
                     void *p_value, size_t buf_len);

/*----------------------------------------------------------------------------*
 * QmeshPsDeleteKeyDataAll
 */
/*! \brief Deletes all values associated with the key
 *         from the persistent storage.
 *
 *  \param[in] key  Identifier of the persistent storage parameter to be deleted.
 
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshPsDeleteKeyDataAll(QMESH_PS_KEY_T key);

/*----------------------------------------------------------------------------*
 * QmeshPsDeInit
 */
/*! \brief Called when the Qmesh stack is reset.
 *         This function resets any variables initialized for the Qmesh 
 *         persistent storage API and frees any allocated buffers.
 *
 *  \return None.
 */
/*----------------------------------------------------------------------------*/
extern void QmeshPsDeInit(void);

#ifdef __cplusplus
}
#endif
/*!@} */
#endif /* _QMESH_PS_H_ */

