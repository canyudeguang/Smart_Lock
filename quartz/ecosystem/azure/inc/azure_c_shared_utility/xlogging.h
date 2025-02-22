// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef XLOGGING_H
#define XLOGGING_H

#ifdef __cplusplus
#include <cstdio>
#else
#include <stdio.h>
#endif /* __cplusplus */

#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/optimize_size.h"

#ifdef TIZENRT
#undef LOG_INFO
#endif

typedef enum LOG_CATEGORY_TAG
{
    AZ_LOG_ERROR,
    AZ_LOG_INFO,
    AZ_LOG_TRACE
} LOG_CATEGORY;

#if defined _MSC_VER
#define FUNC_NAME __FUNCDNAME__
#else
#define FUNC_NAME __func__
#endif

typedef void(*LOGGER_LOG)(LOG_CATEGORY log_category, const char* file, const char* func, const int line, unsigned int options, const char* format, ...);

#define LOG_NONE 0x00
#define LOG_LINE 0x01

//#define USE_QCLI_DBG
/*no logging is useful when time and fprintf are mocked*/
#ifdef NO_LOGGING
#ifdef USE_QCLI_DBG

#include "qcli_api.h"
extern QCLI_Group_Handle_t qcli_ecosystem_handle;

#define LOG(log_category, log_options, ...)  QCLI_Printf(qcli_ecosystem_handle, (const char*)__VA_ARGS__);
#define LogInfo(...)  QCLI_Printf(qcli_ecosystem_handle, (const char*)__VA_ARGS__);
#define LogError(...) QCLI_Printf(qcli_ecosystem_handle, (const char*)__VA_ARGS__);

#else

	
#define LOG(...)  	  
#define LogInfo(...)  
#define LogError(...) 

#endif
	

#define xlogging_get_log_function() NULL
#define xlogging_set_log_function(...)
#define LogErrorWinHTTPWithGetLastErrorAsString(...)
#define UNUSED(x) (void)(x)
#elif (defined MINIMAL_LOGERROR)
#define LOG(...)
#define LogInfo(...)
#define LogError(...) printf("error %s: line %d\n",__FILE__,__LINE__);
#define xlogging_get_log_function() NULL
#define xlogging_set_log_function(...)
#define LogErrorWinHTTPWithGetLastErrorAsString(...)
#define UNUSED(x) (void)(x)
#elif defined(ARDUINO_ARCH_ESP8266)
/*
The ESP8266 compiler doesn't do a good job compiling this code; it doesn't understand that the 'format' is
a 'const char*' and moves it to RAM as a global variable, increasing the .bss size. So we create a
specific LogInfo that explicitly pins the 'format' on the PROGMEM (flash) using a _localFORMAT variable
with the macro PSTR.

#define ICACHE_FLASH_ATTR   __attribute__((section(".irom0.text")))
#define PROGMEM     ICACHE_RODATA_ATTR
#define PSTR(s) (__extension__({static const char __c[] PROGMEM = (s); &__c[0];}))
const char* __localFORMAT = PSTR(FORMAT);

On the other hand, vsprintf does not support the pinned 'format' and os_printf does not work with va_list,
so we compacted the log in the macro LogInfo.
*/
#include "esp8266/azcpgmspace.h"
#define LOG(log_category, log_options, FORMAT, ...) { \
        const char* __localFORMAT = PSTR(FORMAT); \
        os_printf(__localFORMAT, ##__VA_ARGS__); \
        os_printf("\r\n"); \
}

#define LogInfo(FORMAT, ...) { \
        const char* __localFORMAT = PSTR(FORMAT); \
        os_printf(__localFORMAT, ##__VA_ARGS__); \
        os_printf("\r\n"); \
}
#define LogError LogInfo

#else /* !ARDUINO_ARCH_ESP8266 */

#if defined _MSC_VER
#define LOG(log_category, log_options, format, ...) { LOGGER_LOG l = xlogging_get_log_function(); if (l != NULL) l(log_category, __FILE__, FUNC_NAME, __LINE__, log_options, format, __VA_ARGS__); }
#else
#define LOG(log_category, log_options, format, ...) { LOGGER_LOG l = xlogging_get_log_function(); if (l != NULL) l(log_category, __FILE__, FUNC_NAME, __LINE__, log_options, format, ##__VA_ARGS__); } 
#endif

#if defined _MSC_VER
#define LogInfo(FORMAT, ...) do{LOG(AZ_LOG_INFO, LOG_LINE, FORMAT, __VA_ARGS__); }while(0)
#else
#define LogInfo(FORMAT, ...) do{LOG(AZ_LOG_INFO, LOG_LINE, FORMAT, ##__VA_ARGS__); }while(0)
#endif

#if defined _MSC_VER
#define LogError(FORMAT, ...) do{ LOG(AZ_LOG_ERROR, LOG_LINE, FORMAT, __VA_ARGS__); }while(0)
#define TEMP_BUFFER_SIZE 1024
#define MESSAGE_BUFFER_SIZE 260
#define LogErrorWinHTTPWithGetLastErrorAsString(FORMAT, ...) do { \
                DWORD errorMessageID = GetLastError(); \
                LogError(FORMAT, __VA_ARGS__); \
                CHAR messageBuffer[MESSAGE_BUFFER_SIZE]; \
                if (errorMessageID == 0) \
                {\
                    LogError("GetLastError() returned 0. Make sure you are calling this right after the code that failed. "); \
                } \
                else\
                {\
                    int size = FormatMessage(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS, \
                        GetModuleHandle("WinHttp"), errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), messageBuffer, MESSAGE_BUFFER_SIZE, NULL); \
                    if (size == 0)\
                    {\
                        size = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), messageBuffer, MESSAGE_BUFFER_SIZE, NULL); \
                        if (size == 0)\
                        {\
                            LogError("GetLastError Code: %d. ", errorMessageID); \
                        }\
                        else\
                        {\
                            LogError("GetLastError: %s.", messageBuffer); \
                        }\
                    }\
                    else\
                    {\
                        LogError("GetLastError: %s.", messageBuffer); \
                    }\
                }\
            } while(0)
#else
#define LogError(FORMAT, ...) do{ LOG(AZ_LOG_ERROR, LOG_LINE, FORMAT, ##__VA_ARGS__); }while(0)
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void xlogging_set_log_function(LOGGER_LOG log_function);
extern LOGGER_LOG xlogging_get_log_function(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ARDUINO_ARCH_ESP8266 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    /**
     * @brief   Print the memory content byte pre byte in hexadecimal and as a char it the byte correspond to any printable ASCII chars.
     *
     *    This function prints the 'size' bytes in the 'buf' to the log. It will print in portions of 16 bytes, 
     *         and will print the byte as a hexadecimal value, and, it is a printable, this function will print 
     *         the correspondent ASCII character.
     *    Non printable characters will shows as a single '.'. 
     *    For this function, printable characters are all characters between ' ' (0x20) and '~' (0x7E).
     *
     * @param   buf  Pointer to the memory address with the buffer to print.
     * @param   size Number of bytes to print.
     */
    extern void xlogging_dump_buffer(const void* buf, size_t size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XLOGGING_H */
