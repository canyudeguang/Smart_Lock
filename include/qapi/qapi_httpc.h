/* Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * Export of this technology or software is regulated by the U.S. Government.
 * Diversion contrary to U.S. law prohibited.
*/
/**
 * @file qapi_httpc.h
 *
 * @addtogroup qapi_networking_httpc
 * @{
 *
 * @details The HTTP client service provides a collection of API functions that
 * allow the application to enable and configure HTTP client services.
 * The HTTP client can be configured to support IPv4, IPv6, as well as
 * HTTP mode, HTTPS mode (secure), or both.
 *
 * @}
 */

#ifndef _QAPI_HTTPC_H_
#define _QAPI_HTTPC_H_

#include <stdint.h>
#include "qapi/qapi_status.h"   /* qapi_Status_t */
#include "qapi_ssl.h"           /* qapi_Net_SSL_Obj_Hdl_t */

/** @addtogroup qapi_networking_httpc
@{ */

/**
 * @brief For use with qapi_Net_HTTPc_Request().
 */
typedef enum {
	/*supported http client methods */
	QAPI_NET_HTTP_CLIENT_GET_E = 1,
	QAPI_NET_HTTP_CLIENT_POST_E,
	QAPI_NET_HTTP_CLIENT_PUT_E,
	QAPI_NET_HTTP_CLIENT_PATCH_E
} qapi_Net_HTTPc_Method_e;

/**
 * @brief HTTP client callback state. For use with #qapi_HTTPc_CB_t.
 */
typedef enum
{
	QAPI_NET_HTTPC_RX_ERROR_SERVER_CLOSED = -8, 
	QAPI_NET_HTTPC_RX_ERROR_RX_PROCESS = -7,
	QAPI_NET_HTTPC_RX_ERROR_RX_HTTP_HEADER = -6,
	QAPI_NET_HTTPC_RX_ERROR_INVALID_RESPONSECODE = -5,
	QAPI_NET_HTTPC_RX_ERROR_CLIENT_TIMEOUT = -4,
	QAPI_NET_HTTPC_RX_ERROR_NO_BUFFER = -3,
	QAPI_NET_HTTPC_RX_CONNECTION_CLOSED = -2,
	QAPI_NET_HTTPC_RX_ERROR_CONNECTION_CLOSED = -1,
	QAPI_NET_HTTPC_RX_FINISHED = 0,
	QAPI_NET_HTTPC_RX_MORE_DATA = 1,	
} qapi_Net_HTTPc_CB_State_e;

/**
 * @brief HTTP client response. For use with #qapi_HTTPc_CB_t.
 */
typedef struct {
    uint32_t    length;
    /**< Length of the data. */
    uint32_t    resp_Code;
    /**< Response code. */
    const uint8_t *    data;
    /**< Data associated with the response if not NULL. */
} qapi_Net_HTTPc_Response_t;

/**
 * @brief For use with qapi_Net_HTTPc_New_sess().
 */
typedef void (*qapi_HTTPc_CB_t)(
        void* arg,
        /**< Argument passed in qapi_Net_HTTPc_New_sess(). */

        int32_t state,
        /**< State in qapi_Net_HTTPc_CB_State_e. */

        void* value
        /**< Pointer to qapi_Net_HTTPc_Response_t. */
        );

/**
 * @brief HTTP client session handle.
 */
typedef void* qapi_Net_HTTPc_handle_t;

/**
 * @brief (Re)starts the HTTP client module.
 *
 * @details Normally, this is called to start or restart the server after it was
 *          stopped via a call to qapi_Net_HTTPc_Stop().
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Start(void);

/**
 * @brief Stops the HTTP client module.
 *
 * @return On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Stop(void);

/**
 * @brief Starts a new a HTTP client session. 
 *
 * @param[in] timeout  Timeout (in ms) on the session method; 0 is not recommend.
 *
 * @param[in] ssl_Object_Handle  SSL context for HTTPs connect; 0 for no support for HTTPs in this session.
 *
 * @param[in] callback  Pointer to the callback function; NULL for no support callback.
 *
 * @param[in] arg    Argument for the callback function.
 *
 * @param[in] httpc_Max_Body_Length  Maximum body length in this session.
 *
 * @param[in] httpc_Max_Header_Length   Maximum header length in this session.
 *
 * @return
 * On qapi_Net_HTTPc_handle_t, handle of the current session; NULL for error.
 */
qapi_Net_HTTPc_handle_t qapi_Net_HTTPc_New_sess(
        uint32_t timeout,
        qapi_Net_SSL_Obj_Hdl_t ssl_Object_Handle,
        qapi_HTTPc_CB_t callback,
        void* arg,
        uint16_t httpc_Max_Body_Length,
        uint16_t httpc_Max_Header_Length);

/**
 * @brief Frees an HTTP client session.
 *
 * @details Disconnects from the server and frees the memory.
 *
 * @param[in] handle    HTTP client session handle.
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Free_sess(qapi_Net_HTTPc_handle_t handle);

/**
 * @brief Connects an HTTP server in Blocking mode.
 *
 * @param[in] handle  HTTP client session handle.
 *
 * @param[in] URL   Pointer to the server URL.
 *
 * @param[in] port  Port of the server.
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Connect(qapi_Net_HTTPc_handle_t handle, const char *URL, uint16_t port);

/**
 * @brief Disconnects an HTTP client session from the server.
 *
 * @param[in] handle  HTTP client session handle.
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Disconnect(qapi_Net_HTTPc_handle_t handle);

/**
 * @brief Process request, which is sent to the HTTP server. 
 *
 * @param[in] handle  HTTP client session handle.
 *
 * @param[in] cmd   Method to execute in qapi_Net_HTTPc_Method_e.
 *
 * @param[in] URL   Pointer to the server URL.
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Request(qapi_Net_HTTPc_handle_t handle, qapi_Net_HTTPc_Method_e cmd, const char *URL);

/**
 * @brief Sets the body on an HTTP client session.
 *
 * @param[in] handle  HTTP client session handle.
 *
 * @param[in] body   Pointer to the body.
 *
 * @param[in] body_Length  Length of the body.
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Set_Body(qapi_Net_HTTPc_handle_t handle, const char *body, uint32_t body_Length);

/**
 * @brief Sets a parameter on an HTTP client session.
 *
 * @param[in] handle    HTTP client session handle.
 *
 * @param[in] key   Pointer to the key.
 *
 * @param[in] value    Pointer to the value.
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Set_Param(qapi_Net_HTTPc_handle_t handle, const char *key, const char *value);

/**
 * @brief Sets the header field for an HTTP client session.
 *
 * @param[in] handle   HTTP client session handle.
 *
 * @param[in] type     Pointer to the type.
 *
 * @param[in] value    Pointer to the value.
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Add_Header_Field(qapi_Net_HTTPc_handle_t handle, const char *type, const char *value);

/**
 * @brief Clears the header field for an HTTP client session.
 *
 * @param[in] handle    HTTP client session handle.
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Clear_Header(qapi_Net_HTTPc_handle_t handle);

/**
 * @brief Sets SSL configuration parameters on a secure (i.e., HTTPS) session.
 *
 * @param[in] handle    HTTP client session handle.
 *
 * @param[in] ssl_Cfg   SSL connection parameters.
 *
 * @return On success, 0 is returned. On error, nonzero error code is returned.
 */
qapi_Status_t qapi_Net_HTTPc_Configure_SSL(qapi_Net_HTTPc_handle_t handle, qapi_Net_SSL_Config_t *ssl_Cfg);

/**
 * @brief Enables/disables the addition of an HTTP head in a session callback.
 *
 * @param[in] handle   HTTP client session handle.
 *
 * @param[in] enable   1 -- enable; 0 -- disable.
 *
 * @return On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_CB_Enable_Adding_Header(qapi_Net_HTTPc_handle_t handle, uint16_t enable);

 /** @} */ /* end_addtogroup qapi_networking_httpc */

#endif /* _QAPI_HTTPC_H_ */
