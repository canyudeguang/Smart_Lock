/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef QOSA_UTIL_H
#define QOSA_UTIL_H

#define LOG_LVL_AT       (1<<0)
#define LOG_LVL_ERR      (1<<1)
#define LOG_LVL_WARN     (1<<2)
#define LOG_LVL_INFO     (1<<3)
#define LOG_LVL_DEBUG    (1<<4)

#define LOG_LVL_DEFAULT LOG_LVL_AT
#define MAX_LOG_LEVEL  (LOG_LVL_AT | LOG_LVL_ERR | LOG_LVL_WARN | LOG_LVL_INFO | LOG_LVL_DEBUG)

void QCLI_Printf(const char *format, ...);
extern uint32_t LogLevel;

#define LOG_AT(fmt, ...)    \
	do { \
		if (LogLevel & LOG_LVL_AT) \
	      	{ \
			QCLI_Printf(fmt, ##__VA_ARGS__); \
	       	} \
	} while(0)

#define LOG_AT_OK()         \
	do { \
		{ QCLI_Printf("\r\nOK\r\n"); } \
	} while(0)

#define LOG_AT_ERROR()      \
	do { \
		{ QCLI_Printf("\r\nERROR\r\n"); } \
	} while(0)

#define LOG_AT_EVT(fmt, ...)    \
    do { \
      	{ QCLI_Printf(fmt, ##__VA_ARGS__); }\
    } while(0)

#ifdef ENABLE_LOG
#define LOG_ERR(fmt, ...)   \
	do { \
	       	if (LogLevel & LOG_LVL_ERR) \
	      	{ \
		       	QCLI_Printf("(E)"fmt, ##__VA_ARGS__); \
	       	} \
       	} while(0)

#define LOG_WARN(fmt, ...)  \
	do { \
	       	if (LogLevel & LOG_LVL_WARN) \
	       	{ \
		       QCLI_Printf("(W)"fmt, ##__VA_ARGS__); \
	       	} \
	} while(0)

#define LOG_INFO(fmt, ...)  \
	do { \
	       	if (LogLevel & LOG_LVL_INFO) \
		{ \
			QCLI_Printf("(I)"fmt, ##__VA_ARGS__); \
		} \
	} while(0)

#define LOG_DEBUG(fmt, ...) \
	do { \
		if (LogLevel & LOG_LVL_DEBUG) \
		{ \
			QCLI_Printf("(D)"fmt, ##__VA_ARGS__); \
		} \
	} while(0)
#else
#define LOG_ERR(fmt, ...)    do { } while(0)
#define LOG_WARN(fmt, ...)   do { } while(0)
#define LOG_INFO(fmt, ...)   do { } while(0)
#define LOG_DEBUG(fmt, ...)  do { } while(0)
#endif

void qc_api_SetLogLevel(int32_t Level);
int32_t qc_api_GetLogLevel(void);

#endif
