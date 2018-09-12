/* Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
All rights reserved.

Confidential and Proprietary - Qualcomm Technologies, Inc.

*/

/** @file qapi_net_status.h
 *
 * @addtogroup qapi_networking_status
 * @{
 *
 * @details This section contains QAPI error codes that may be returned as a
 * result of an error in one of the networking services.
 *
 * @}
 */

#ifndef _QAPI_NET_STATUS_H_
#define _QAPI_NET_STATUS_H_

#include "qapi/qapi_status.h"

/** @addtogroup qapi_networking_status
@{ */


/*
 * SSL QAPI return values. See qapi_status.h for common error codes. The
 * ones below are SSL module specific.
 */
/** Error in own certificate. */
#define QAPI_ERR_SSL_CERT __QAPI_ERROR(QAPI_MOD_NETWORKING, 1)

/** Error with SSL connection. */
#define QAPI_ERR_SSL_CONN __QAPI_ERROR(QAPI_MOD_NETWORKING, 2)

/** Handshake must be completed before the operation can be attempted. */
#define QAPI_ERR_SSL_HS_NOT_DONE __QAPI_ERROR(QAPI_MOD_NETWORKING, 3)

/** Received SSL warning alert message. */
#define QAPI_ERR_SSL_ALERT_RECV __QAPI_ERROR(QAPI_MOD_NETWORKING, 4)

/** Received SSL fatal alert message. */
#define QAPI_ERR_SSL_ALERT_FATAL __QAPI_ERROR(QAPI_MOD_NETWORKING, 5)

/** Handshake in progress. */
#define QAPI_SSL_HS_IN_PROGRESS __QAPI_ERROR(QAPI_MOD_NETWORKING, 6)

/** Handshake successful. */
#define QAPI_SSL_OK_HS __QAPI_ERROR(QAPI_MOD_NETWORKING, 7)

/** The peer's SSL certificate is trusted, CN matches the host name, and the time has expired. */
#define QAPI_ERR_SSL_CERT_CN __QAPI_ERROR(QAPI_MOD_NETWORKING, 8)

/** The peer's SSL certificate is trusted, CN does not match the host name, and the time is valid. */
#define QAPI_ERR_SSL_CERT_TIME __QAPI_ERROR(QAPI_MOD_NETWORKING, 9)

/** The peer's SSL certificate is not trusted */
#define QAPI_ERR_SSL_CERT_NONE __QAPI_ERROR(QAPI_MOD_NETWORKING, 10)

/** Connection drop when out of network buffers. This is usually a
 * resource configuration error. */
#define QAPI_ERR_SSL_NETBUF __QAPI_ERROR(QAPI_MOD_NETWORKING, 11)

/** Socket error. Use qapi_errno to check for the reason code. */
#define QAPI_ERR_SSL_SOCK __QAPI_ERROR(QAPI_MOD_NETWORKING, 12)

/** No DTLS server connection matching a peer address was found. */
#define QAPI_ERR_SSL_CONN_NOT_FOUND __QAPI_ERROR(QAPI_MOD_NETWORKING, 13)

/*
 * Generic error codes
 */
/** IP address is invalid. */
#define QAPI_NET_ERR_INVALID_IPADDR         ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  21)))

/** Failed to get the scope ID. */
#define QAPI_NET_ERR_CANNOT_GET_SCOPEID     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  22)))

/** Socket command timed out. */
#define QAPI_NET_ERR_SOCKET_CMD_TIME_OUT    ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  23)))

/** Socket failure. */
#define QAPI_NET_ERR_SOCKET_FAILURE         ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  24)))

/** Operation failed. */
#define QAPI_NET_ERR_OPERATION_FAILED       ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  25)))

/*
 * MQTTC status codes
 */
/** Operation was successful. */
#define QAPI_NET_STATUS_MQTTC_OK                            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  30)))

/** Connection was successful. */
#define QAPI_NET_STATUS_MQTTC_CONNECT_SUCCEEDED             (QAPI_NET_STATUS_MQTTC_OK)

/** Unacceptable protocol version. */
#define QAPI_NET_STATUS_MQTTC_INVALID_PROTOCOL_VERSION      ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  31)))

/** Client ID is rejected by the broker. */
#define QAPI_NET_STATUS_MQTTC_CLIENT_ID_REJECTED            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  32)))

/** MQTT service is not available. */
#define QAPI_NET_STATUS_MQTTC_MQTT_SERVICE_UNAVAILABLE      ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  33)))

/** Malformed user name or password. */
#define QAPI_NET_STATUS_MQTTC_INVALID_USERNAME_PASSWORD     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  34)))

/** Client is not authorized. */
#define QAPI_NET_STATUS_MQTTC_NOT_AUTHORIZED                ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  35)))

/** Socket write error. */
#define QAPI_NET_STATUS_MQTTC_SOCKET_WRITE_ERROR            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  36)))

/** Socket read error. */
#define QAPI_NET_STATUS_MQTTC_SOCKET_READ_ERROR             ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  37)))

/** Malformed packet. */
#define QAPI_NET_STATUS_MQTTC_MALFORMED_PACKET              ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  38)))

/** CONNACK was not received from the broker. */
#define QAPI_NET_STATUS_MQTTC_CONNACK_NOT_RECEIVED          ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  39)))

/** TCP connection failed. */
#define QAPI_NET_STATUS_MQTTC_TCP_CONNECT_FAILED            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  40)))

/** SSL handshake failed. */
#define QAPI_NET_STATUS_MQTTC_SSL_HANDSHAKE_FAILED          ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  41)))

/** MQTT client was not started. */
#define QAPI_NET_STATUS_MQTTC_CLIENT_NOT_STARTED            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  42)))

/** Invalid handle. */
#define QAPI_NET_STATUS_MQTTC_INVALID_HANDLE                ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  43)))

/** Unknown host. */
#define QAPI_NET_STATUS_MQTTC_UNKNOWN_HOST                  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  44)))

/** Socket creation failed. */
#define QAPI_NET_STATUS_MQTTC_SOCKET_CREATION_FAILED        ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  45)))

/** Socket fatal error. */
#define QAPI_NET_STATUS_MQTTC_SOCKET_FATAL_ERROR            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  46)))

/** TCP bind failed. */
#define QAPI_NET_STATUS_MQTTC_TCP_BIND_FAILED               ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  47)))

/** SSL creation failed. */
#define QAPI_NET_STATUS_MQTTC_SSL_CREATION_FAILED           ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  48)))

/** Memory allocation error. */
#define QAPI_NET_STATUS_MQTTC_MEMORY_ALLOCATION_FAILED      ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  49)))

/** Invalid UTF8 string. */
#define QAPI_NET_STATUS_MQTTC_INVALID_UTF8_STRING           ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  50)))

/** Bad parameters. */
#define QAPI_NET_STATUS_MQTTC_BAD_PARAM                     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  51)))

/** Bad state. Must not issue commands at the current state. */
#define QAPI_NET_STATUS_MQTTC_BAD_STATE                     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  52)))

/** Message serialization failed. */
#define QAPI_NET_STATUS_MQTTC_MSG_SERIALIZATION_FAILURE     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  53)))

/** One or more subscriptions failed. */
#define QAPI_NET_STATUS_MQTTC_PARTIAL_SUBSCRIPTION_FAILURE  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  54)))

/** Unknown topic. */
#define QAPI_NET_STATUS_MQTTC_UNKNOWN_TOPIC                 ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  55)))

/** TCP connection closed. */
#define QAPI_NET_STATUS_MQTTC_TCP_CONNECTION_CLOSED         ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  56)))

/** MQTT client existed already. */
#define QAPI_NET_STATUS_MQTTC_CLIENT_EXISTED                ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  57)))

/** MQTT DISCONNECT failed. */
#define QAPI_NET_STATUS_MQTTC_DISCONNECT_FAILED             ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  58)))

/** MQTT client end. */
#define QAPI_NET_STATUS_MQTTC_END                           ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  69)))

/*
 * MDNSD status codes
 */
/** Registration in progrss. */
#define QAPI_NET_STATUS_MDNSD_SVC_REGISTRATION_IN_PROGRESS ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  70)))

/** Registration failure. */
#define QAPI_NET_STATUS_MDNSD_SVC_REGISTRATION_FAILURE ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  71)))

/** Hostname conflict. */
#define QAPI_NET_STATUS_MDNSD_HOSTNAME_CONFLICT ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  72)))

/** Hostname and service conflict. */
#define QAPI_NET_STATUS_MDNSD_HOSTNAME_AND_SVC_CONFLICT ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  73)))

/** Service conflict. */
#define QAPI_NET_STATUS_MDNSD_SVC_CONFLICT ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  76)))

#define QAPI_NET_STATUS_MDNSD_STATE_INVALID ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  77)))

/** @} */

#endif /* _QAPI_NET_STATUS_H_ */
