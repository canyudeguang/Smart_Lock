/*=============================================================================
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
/*! \file qmesh_result.h
 *  \brief CSRmesh library result type
 *
 *   This file contains the different result types the application will receive
 *   as a result of invoking the Mesh APIs.
 *
 *   NOTE: This library includes the Mesh Transport Layer, Mesh Control
 *   Layer and Mesh Association Layer functionality.
 */
 /*****************************************************************************/

#ifndef __QMESH_RESULT_H__
#define __QMESH_RESULT_H__

/*! \addtogroup Core
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*!\brief QMesh operation status type.
 *
 * These error codes are mapped to configuration model status codes as well.
 * Currently, they are defined compliant to specification version 1.

 * @note1hang 0x0000 through 0x00FF are reserved for configuration model status
 *            codes and the rest are for internal status codes.
*/
typedef enum
{
    QMESH_RESULT_SUCCESS,                           /*!< Operation status success. */
    QMESH_RESULT_INVALID_ADDRESS,                   /*!< The provided value is not a valid address in this context. */
    QMESH_RESULT_INVALID_MODEL,                     /*!< The provided model index is not valid in this element. */
    QMESH_RESULT_INVALID_APP_KEY,                   /*!< The provided app key index is not valid for this node. */
    QMESH_RESULT_INVALID_NET_KEY,                   /*!< The provided net key index is not valid for this node. */
    QMESH_RESULT_INSUFFICIENT_RESOURCES,            /*!< The node cannot store provided information due to insufficient resources on the node. */
    QMESH_RESULT_KEY_INDEX_ALREADY_STORED,          /*!< The key with the given index is already stored in the node with a different value. */
    QMESH_RESULT_INVALID_PUB_PARAM,                 /*!< Invalid publish parameters. */
    QMESH_RESULT_CONFIG_STATUS_NOT_A_SUB_MODEL,     /*!< The referenced model is not a subscribe model. */
    QMESH_RESULT_CONFIG_STATUS_STORAGE_FAILURE,     /*!< The node was not able to store the new value in persistent storage. */
    QMESH_RESULT_FEATURE_NOT_SUPPORTED,             /*!< The feature is not supported in this node. */
    QMESH_RESULT_CANNOT_UPDATE,                     /*!< The requested update operation cannot be performed due to general constraints. */
    QMESH_RESULT_CONFIG_STATUS_CANNOT_REMOVE,       /*!< The requested delete operation cannot be performed due to general constraints. */
    QMESH_RESULT_CONFIG_STATUS_CANNOT_BIND,         /*!< The requested bind operation cannot be performed due to general constraints. */
    QMESH_RESULT_CONFIG_STATUS_TEMPLY_UNABLE_TO_CHANGE_STATE, /*!< The requested operation temporarily cannot be performed due to internal state of the node. */
    QMESH_RESULT_CONFIG_STATUS_CANNOT_SET,          /*!< The requested set operation cannot be performed due to general constraints. */
    QMESH_RESULT_CONFIG_STATUS_UNSPECIFIED_ERROR,   /*!< An error occurred that does not correspond to any error conditions defined for a given state. */
    QMESH_RESULT_CONFIG_STATUS_INVALID_BINDING,     /*!< The NetKeyIndex and AppKeyIndex combination is not valid. */
    /*!
     * 0x0013 - 0x00FF are RFU for config model status codes.
     */

    /*! All the internal status codes are defined here */
    QMESH_RESULT_INPROGRESS     =   0x0100,         /*!< Operation is in progress. */
    QMESH_RESULT_ROLE_NOT_SUPPORTED,                /*!< Role is not supported. */
    QMESH_RESULT_CANNOT_SET,
    QMESH_RESULT_INVALID_NWK_ID,                    /*!< Mesh operation status is Invalid_Network_id. */
    QMESH_RESULT_APP_KEY_NOT_FOUND,                 /*!< Mesh operation specified application key is not present. */
    QMESH_RESULT_EXCEED_MAX_NO_OF_NWKS,             /*!< Mesh operation status is Exceed_Max_No_of_Network. */
    QMESH_RESULT_NOT_READY,                         /*!< Mesh operation status is Not_ready. */
    QMESH_RESULT_API_NOT_SUPPORTED,                 /*!< Mesh operation status is Api_Not_Supported. */
    QMESH_RESULT_INVALID_COMP_DATA,                 /*!< Invalid Composition data */
    QMESH_RESULT_EXCEED_MAX_NO_OF_APP_KEYS,         /*!< Mesh operation status exceed the maximum number of app keys. */
    QMESH_RESULT_EXCEED_MAX_NO_OF_NET_KEYS,         /*!< Mesh operation status exceed the maximum number of network keys. */
    QMESH_RESULT_APP_ID_SLOT_NOT_EMPTY,             /*!< No slot empty for adding an additional application Id. */
    QMESH_RESULT_INVALID_STATE,                     /*!< Stack is in an invalid state and cannot execute the current function. */
    QMESH_RESULT_MODEL_NOT_REGISTERED,              /*!< Model is not registered. */
    QMESH_RESULT_MODEL_ALREADY_REGISTERD,           /*!< Model is already registered. */
    QMESH_RESULT_TX_INPROGRESS,                     /*!< Tranmission is already in progress. */
    QMESH_RESULT_INVALID_APP_PAYLOAD_SIZE,          /*!< Invalid application payload size. */
    QMESH_RESULT_APP_PAYLOAD_BUF_NOT_INITIALIZED,   /*!< Application payload buffer not initialized. */
    QMESH_RESULT_MESSAGE_NOT_HANDLED,               /*!< Message not handled. */
    QMESH_RESULT_INVALID_TRANS_MIC_SIZE,            /*!< Invalid MIC size. */
    QMESH_RESULT_OUT_OF_MEMORY,                     /*!< Out of memory. */
    QMESH_RESULT_LPN_CACHE_EMPTY,                   /*!< LPN cache is empty. */
    QMESH_RESULT_FRND_CACHE_NOT_REGISTERED,         /*!< Friend cache is not registered. */
    QMESH_RESULT_INVALID_ARGUMENTS,                 /*!< Invalid argument. */
    QMESH_RESULT_INVALID_KEY_TYPE,                  /*!< Invalid key type specified. */
    QMESH_RESULT_ALL_SEGEMENTS_TRANSFERRED,         /*!< All segments succesfully transferred. */
    QMESH_RESULT_CONF_CHECK_FAILED,                 /*!< Qmesh provisioning failed due to a confirmation check fail. */
    QMESH_RESULT_TIMEOUT,                           /*!< Qmesh operation status is Timeout. */
    QMESH_RESULT_NO_BEARER_ENABLED,                 /*!< No bearer enabled. */
    QMESH_RESULT_INVALID_CONFIG_MSG_LEN,            /*!< Invalid configuration message length. */
    QMESH_RESULT_LINK_REASON_SUCCESS,               /*!< Link reason success. */
    QMESH_RESULT_LINK_REASON_TIMEOUT,               /*!< Link reason timeout. */
    QMESH_RESULT_LINK_REASON_FAIL,                  /*!< Link reason fail. */
    QMESH_RESULT_PROV_PROHIBITED,                   /*!< Prohibited. */
    QMESH_RESULT_PROV_INVALID_PDU,                  /*!< Invalid PDU received. */
    QMESH_RESULT_PROV_INVALID_FORMAT,               /*!< Invalid PDU format. */
    QMESH_RESULT_PROV_UNEXPECTED_PDU,               /*!< Unexpected PDU received. */
    QMESH_RESULT_PROV_CONFIRMATION_FAILED,          /*!< Confirmation check failed. */
    QMESH_RESULT_PROV_OUT_OF_RESOURCE,              /*!< Out of resources. */
    QMESH_RESULT_PROV_DECRYPTION_FAILED,            /*!< Decryption failed. */
    QMESH_RESULT_PROV_UNEXPECTED_ERROR,             /*!< Unexpected error occurred. */
    QMESH_RESULT_PROV_CANNOT_ASSIGN_ADDR,           /*!< Can not assign the address. */
    QMESH_RESULT_PROV_TIMEOUT,                      /*!< Provision procedure timeout. */
    QMESH_RESULT_FEATURE_NOT_PRESENT_IN_COMP_DATA,  /*!< Feature is not supported in composition data. */
    QMESH_RESULT_SCHED_NOT_STARTED,                 /*!< Scheduler was not started. */
    QMESH_RESULT_INVALID_SCAN_PARAMS,               /*!< Invalid Scan Parameters. */
    QMESH_RESULT_SCAN_NOT_ENABLED,                  /*!< Scan Not Enabled. */
    QMESH_RESULT_ADVERT_QUEUE_IS_EMPTY,             /*!< Nothing to advertise */
    QMESH_RESULT_PS_NO_ELEMENT,                     /*!< No Element available */
    QMESH_RESULT_PS_NO_KEY,                         /*!< No Key Present in persistent store */
    QMESH_RESULT_PS_READ_FAILED,                    /*!< Key read failed */
    QMESH_RESULT_PS_WRITE_FAILED,                   /*!< Key write failed */
    QMESH_RESULT_BUSY,                              /*!< Module busy */
    QMESH_RESULT_NOT_VALID_ADV,                     /*!< Bearer recieved packet is not valid */

    QMESH_RESULT_FAILURE = 0xFFFF                   /*!< Mesh operation status is Failure. */
} QMESH_RESULT_T;

/*!\brief QmeshResult is deprecated, but kept for backward compatibility. */
typedef QMESH_RESULT_T QmeshResult;

#ifdef __cplusplus
}
#endif

/*!@} */
#endif

