/* Copyright (c) 2011-2016.  Qualcomm Atheros, Inc.
 * All rights reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "qapi_types.h"
#include "qapi_status.h"
#include "qapi_ssl.h"
#include "qapi_socket.h"
#include "qapi_netservices.h"
#include "qapi_ns_gen_v4.h"
#include "qapi_ns_gen_v6.h"
#include "qapi_ns_utils.h"
#include "qapi_netbuf.h"
#include "qapi_ota.h"
#include "malloc.h"
#include "ota_https.h"


/*****************************************************************************************************************************/
/*                                                                                                                           */
/*       OTA_HTTPS                                                                                                             */
/*                                                                                                                           */
/*****************************************************************************************************************************/
extern uint32_t generate_rand32(void);


/*----------------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------------*/ 

#define htonl(l) (((((l) >> 24) & 0x000000ff)) | \
                 ((((l) >>  8) & 0x0000ff00)) | \
                 (((l) & 0x0000ff00) <<  8) | \
                 (((l) & 0x000000ff) << 24))
#define ntohl(l) htonl(l)
#define htons(s)    ((((s) >> 8) & 0xff) | (((s) << 8) & 0xff00))
#define ntohs(s) htons(s)
 
#define OTA_HTTPS_MAX_FILE_LEN          (256)
#define OTA_HTTPS_MAX_SERVER_LEN        (64)
#define OTA_HTTPS_REQ_MAX	            (600)           /* 192(field of header) + length of file name + length of server name */
#define OTA_HTTPS_TIMEOUT               (3000)
#define OTA_HTTPS_PORT                  (443)
#define OTA_HTTPS_ALT_PORT              (46030)
#define OTA_HTTPS_LOCAL_PORT            (1068)

//#define DEBUG_OTA_HTTPS
#undef  DEBUG_OTA_HTTPS
#if defined(DEBUG_OTA_HTTPS)
#define ota_hprintf(args...) A_PRINTF(args)
#else
#define ota_hprintf(args...)
#endif

#define A_PRINTF(args...)

/*----------------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------------*/

typedef struct _OTA_HTTPS_REQ {
    char     *file;	
    uint16_t port;	
    int32_t  ip_addr;
    uint8_t  ip_v6addr[16];
    uint8_t  v6_enable_flag;
} OTA_HTTPS_REQ_t;

typedef struct _OTA_HTTPS_RES {
    char    *buf;
    int32_t buf_length;
    int32_t buf_size;
    int32_t response_code;
    int32_t content_length;
} OTA_HTTPS_RES_t;

typedef struct _OTA_HTTPS {
    int32_t                 sock_fd;
    int16_t                 port;
    int32_t                 scope_id;
    uint32_t                timeout;        
    OTA_HTTPS_REQ_t         *req;
    OTA_HTTPS_RES_t         *res;
    qapi_Net_SSL_Obj_Hdl_t  ssl_ctx;
    qapi_Net_SSL_Con_Hdl_t  hssl;
} OTA_HTTPS_t;

/*----------------------------------------------------------------------------------
 *  Global  
 * -------------------------------------------------------------------------------*/

OTA_HTTPS_t *ota_https_sess = NULL;
 
/*----------------------------------------------------------------------------------
 * 
 * -------------------------------------------------------------------------------*/
static uint16_t ota_https_get_port(void);
static int32_t ota_https_open(char *interface_name, char *url);
static void ota_https_close(void);
static int32_t ota_https_read_packet(OTA_HTTPS_t *ota_https, uint32_t timeout);
static int32_t ota_https_check_header(OTA_HTTPS_t *ota_https);
static int32_t ota_https_connect(OTA_HTTPS_t *ota_https, uint32_t timeout);
static int32_t ota_https_query(OTA_HTTPS_t *ota_https);

/*----------------------------------------------------------------------------------
 * 
 * -------------------------------------------------------------------------------*/

/*
 *
 */
static uint16_t ota_https_get_port(void)
{
    uint16_t sock_port = generate_rand32() % 0x7fff;
    if( sock_port < OTA_HTTPS_LOCAL_PORT ) 
        sock_port += OTA_HTTPS_LOCAL_PORT;
    return sock_port;
}

/*
 * url format: <host>:<port>/<url-path>
 *         or  |ipv6|:<port>/<url-path>
 */
static int32_t ota_https_open(char *interface_name, char *url)
{
    struct sockaddr_in local_addr;
    struct sockaddr_in6 local_addr6;
    struct sockaddr *addr;
    OTA_HTTPS_t *ota_https;
    char  *ptr, *ptr_next;
    uint32_t len, addrlen, file_len;
    int family;
    int32_t rtn;
    
    if ( ota_https_sess != NULL ) {
        return OTA_ERR_HTTPS_SESSION_ALREADY_START_E;
    }
    
    /*allocate mem for ota_https_sess */
    ota_https_sess = (OTA_HTTPS_t *)malloc(sizeof(OTA_HTTPS_t));
    if (!ota_https_sess) {
        return OTA_ERR_HTTPS_NO_MEMORY_E;
    }
    
    ota_https = ota_https_sess;
    memset(ota_https, 0, sizeof(OTA_HTTPS_t));

    ota_https->req = (OTA_HTTPS_REQ_t *)malloc(sizeof(OTA_HTTPS_REQ_t));
    if (!ota_https->req) {
        rtn = OTA_ERR_HTTPS_NO_MEMORY_E;
        goto https_open_end;
    }
    memset(ota_https->req, 0, sizeof(OTA_HTTPS_REQ_t));
    ota_https->ssl_ctx = QAPI_NET_SSL_INVALID_HANDLE;
    ota_https->hssl = QAPI_NET_SSL_INVALID_HANDLE;

    /* parse url */    
    ptr = ptr_next = url;
    
    /* get ip address */
    if ( *ptr_next == '|') {  //IPV6 
        ota_https->req->v6_enable_flag = 1;
        family = AF_INET6;        

        ptr_next++;
        //find next '|'
        ptr = strchr(ptr_next, '|');
        if( ptr == NULL )
        {
            rtn = OTA_ERR_HTTPS_URL_FORMAT_E;
            goto https_open_end;
        } 
        *ptr = '\0';
        
        /*Get IPv6 address of Peer*/
        if (inet_pton(AF_INET6, ptr_next, ota_https->req->ip_v6addr) != 0 ) {
            rtn = OTA_ERR_HTTPS_URL_FORMAT_E;
            goto https_open_end; 
        }
        
        if ( QAPI_IS_IPV6_LINK_LOCAL(ota_https->req->ip_v6addr) ) {   
            if( qapi_Net_IPv6_Get_Scope_ID(interface_name, &(ota_https_sess->scope_id)) != 0 ) {
                rtn = OTA_ERR_HTTPS_GET_LOCAL_ADDRESS_E;
                goto https_open_end; 
            }
        }

        //move to next segment
        *ptr = '|';
        ptr = strtok(ptr, ":");
        if ( ptr == NULL ) {
            rtn = OTA_ERR_HTTPS_URL_FORMAT_E;
            goto https_open_end;
        }
    } else {    /* IPV4 */
        ota_https_sess->req->v6_enable_flag = 0;
        family = AF_INET; 
        
        //get host ip address
        ptr = strtok(ptr, ":");
        if( ptr == NULL ) {
            rtn = OTA_ERR_HTTPS_URL_FORMAT_E;
            goto https_open_end;
        }
    
        if( inet_pton(AF_INET, ptr, (uint32_t *) &(ota_https_sess->req->ip_addr)) != 0 ) {
            rtn = OTA_ERR_HTTPS_URL_FORMAT_E;
            goto https_open_end;     
        }
    }

    //get https port number
    ptr = strtok(NULL, "/");
    if ( ptr == NULL ) {
        rtn = OTA_ERR_HTTPS_URL_FORMAT_E;
        goto https_open_end;     
    }
    ota_https->req->port = strtol(ptr,NULL,10);
    
    //get file name
    ptr += strlen(ptr) + 1;
    if ((file_len = strlen(ptr) + 1) > OTA_HTTPS_MAX_FILE_LEN) {
        rtn = OTA_ERR_HTTPS_FILE_NAME_TOO_LONG_E;
        goto https_open_end;
    }

    ota_https->req->file = malloc(file_len);
    if (!ota_https->req->file) {
        rtn = OTA_ERR_HTTPS_NO_MEMORY_E;
        goto https_open_end;
    }

    //save file name
    strncpy(ota_https->req->file, ptr, file_len);

    //create socket
    ota_https->sock_fd = qapi_socket(family, SOCK_STREAM, 0);
    if (ota_https->sock_fd  == -1) {
        rtn = OTA_ERR_HTTPS_CREATE_SOCKET_E;
        goto https_open_end;   
    }

    /* bind to local port.*/
    /* set local port */
    ota_https->port	= ota_https_get_port();
    /* set https timeout */
    ota_https->timeout = OTA_HTTPS_TIMEOUT;

    if (ota_https_sess->req->v6_enable_flag) {
        memset(&local_addr6, 0, sizeof(local_addr6));
        local_addr6.sin_port = htons(ota_https->port);
        local_addr6.sin_family = AF_INET6;

        addr = (struct sockaddr *)&local_addr6;
        addrlen = sizeof(struct sockaddr_in6);
    } else {
        memset(&local_addr, 0, sizeof(local_addr));
        local_addr.sin_port = htons(ota_https->port);
        local_addr.sin_family = AF_INET;

        addr = (struct sockaddr *)&local_addr;
        addrlen = sizeof(struct sockaddr_in);
    }

    /* bind to local port.*/
    if(qapi_bind( ota_https->sock_fd, addr, addrlen) == -1) {
        rtn = OTA_ERR_HTTPS_BIND_FAIL_E;
        goto https_open_end;   
    }

    ota_https->res = (OTA_HTTPS_RES_t *)malloc(sizeof(OTA_HTTPS_RES_t));
    if (!ota_https->res) {
        rtn = OTA_ERR_HTTPS_NO_MEMORY_E;
        goto https_open_end;
    }
    memset(ota_https->res, 0, sizeof(OTA_HTTPS_RES_t));

    if (ota_https->req->port == OTA_HTTPS_PORT || ota_https->req->port == OTA_HTTPS_ALT_PORT) {
        ota_https->ssl_ctx = qapi_Net_SSL_Obj_New(QAPI_NET_SSL_CLIENT_E);
        if(ota_https->ssl_ctx == QAPI_NET_SSL_INVALID_HANDLE ){
            rtn = OTA_ERR_HTTPS_SSL_CREATE_CTX_ERR_E;
            goto https_open_end;
        }
        ota_https->hssl = qapi_Net_SSL_Con_New(ota_https->ssl_ctx, QAPI_NET_SSL_TLS_E);
        if( ota_https->hssl == QAPI_NET_SSL_INVALID_HANDLE ) {
            rtn = OTA_ERR_HTTPS_SSL_CREATE_CTX_ERR_E;
            goto https_open_end;
        }
        
        if( qapi_Net_SSL_Fd_Set(ota_https->hssl, ota_https->sock_fd) < 0 ) {
            rtn = OTA_ERR_HTTPS_SSL_ADD_SOCKET_FAIL_E;
            goto https_open_end;
        }
    } 

    rtn = OTA_OK_E;

https_open_end:
    if( rtn != OTA_OK_E ) {
        ota_https_close();
    }
    
    return rtn;
}

static void ota_https_close(void)
{
    OTA_HTTPS_REQ_t *req;
    OTA_HTTPS_RES_t *res;

    if (!ota_https_sess) return;

    if ( ota_https_sess->hssl != QAPI_NET_SSL_INVALID_HANDLE ){
        qapi_Net_SSL_Shutdown(ota_https_sess->hssl);
        ota_https_sess->hssl = QAPI_NET_SSL_INVALID_HANDLE; 
    }

    if (ota_https_sess->ssl_ctx != QAPI_NET_SSL_INVALID_HANDLE ) {
        qapi_Net_SSL_Obj_Free(ota_https_sess->ssl_ctx);
        ota_https_sess->ssl_ctx = QAPI_NET_SSL_INVALID_HANDLE;
    }

    if (ota_https_sess->sock_fd > 0) {
        qapi_socketclose(ota_https_sess->sock_fd);
        ota_https_sess->sock_fd = 0;
    }  
    
    req = ota_https_sess->req;
    res = ota_https_sess->res;

    if (req) {
        if (req->file) free(req->file);
        req->file = 0;

        free(req);
        ota_https_sess->req = 0;
    }

    if (res) {
        free(res);
        ota_https_sess->res = 0;
    }

    free(ota_https_sess);
    ota_https_sess = NULL;
    
    return;
}

static int32_t ota_https_query(OTA_HTTPS_t *ota_https)
{
    OTA_HTTPS_REQ_t *req = ota_https->req;
    int32_t ret = OTA_OK_E;
    int32_t len, size;
    char *cp, *message = NULL; 
    char ip_str[48];

    size = OTA_HTTPS_REQ_MAX;    
    message = (char *)malloc(size);
    if(!message) {
        ret = OTA_ERR_HTTPS_NO_MEMORY_E;
        A_PRINTF("Memory allocate failed\n");
        goto https_query_end;
    }
    memset(message, 0, size);
 
    cp = message;
    len = snprintf(cp, size, "GET /%s HTTP/1.1\r\n", req->file);
    cp+=len;
    size-=len;
    if( req->v6_enable_flag == 1 ) {
        len = snprintf(cp, size, "Host: %s\r\n", inet_ntop(AF_INET6, (void *) req->ip_v6addr, ip_str, sizeof(ip_str)));
    } else {
        len = snprintf(cp, size, "Host: %s\r\n", inet_ntop(AF_INET, (void *) &(req->ip_addr), ip_str, sizeof(ip_str)));
    }
    cp+=len;
    size-=len;
    len = snprintf(cp, size, "Accept: text/html, */*\r\n");
    cp+=len;
    size-=len;
    len = snprintf(cp, size, "User-Agent: IOE Client\r\n");
    cp+=len;
    size-=len;
    len = snprintf(cp, size, "Connection: keep-alive\r\n");
    cp+=len;
    size-=len;
    len = snprintf(cp, size, "Cache-control: no-cache\r\n");
    cp+=len;
    size-=len;
    len = snprintf(cp, size, "\r\n");

    size = strlen(message);
    if (req->port == OTA_HTTPS_PORT || req->port == OTA_HTTPS_ALT_PORT) {
        if( qapi_Net_SSL_Write(ota_https->hssl, message, size) < 0) {
            A_PRINTF("ssl write error(%d)\n", ret);
            ret = OTA_ERR_HTTPS_SEND_E;
        }
    } else {
        if( qapi_send(ota_https->sock_fd, message, size, 0) < 0) {
            A_PRINTF("qcom_send error(%d)\n", ret);
            ret = OTA_ERR_HTTPS_SEND_E;
        }
    }

https_query_end:
    if( message != NULL )
        free(message);
    
    return ret;
}

/* Input Format: "HTTP/%d.%d %d "*/
static int32_t ota_https_parse_header_response_code(char *buf)
{
    int32_t response_code = 404;
    int32_t ver1, ver2, len;
    char *ptr, *buf_ptr = NULL;
        
    len = strlen(buf)+1;
    buf_ptr = malloc(len);
    if( buf_ptr == 0) 
        goto parse_header_end;
    memset(buf_ptr, 0, len);
    memcpy(buf_ptr, buf, len);
        
    ptr = buf_ptr;
    ptr = strstr(ptr, "HTTP/");
    if(ptr == NULL)
        goto parse_header_end;
    ptr += 5;
    ptr = strtok(ptr, ".");
    if(ptr == NULL)
        goto parse_header_end;
    
    ver1 = strtol(ptr, NULL, 10);
        
    ptr = strtok(NULL, " ");
    if(ptr == NULL)
        goto parse_header_end;
    ver2 = strtol(ptr, NULL, 10);

    ptr = strtok(NULL, " ");
    if(ptr == NULL)
        goto parse_header_end;
    response_code = strtol(ptr, NULL, 10);

parse_header_end:
    if( buf_ptr != NULL)
        free(buf_ptr);
        
    return response_code;
}

static int32_t ota_https_parse_header_content_length(char *buf)
{
#define _MAX_CONTENT_DIGIT      (20)
    char *str;
    char temp[_MAX_CONTENT_DIGIT];
    int32_t i = 0;

    str = strstr(buf, "Content-Length: ");
    if (str) {
        str += 16;

        while (*str) 
        {
            if (i >= _MAX_CONTENT_DIGIT)
                return (-1);

            if (*str == '\r' && *(str+1) == '\n') {
                break;
            } else {
                temp[i] = *str;
            }

            i++; str++;
        }

        temp[i] = 0;
        return atoi(temp);
    }

    return (-1);
}

static int32_t ota_https_check_header(OTA_HTTPS_t *ota_https)
{
    OTA_HTTPS_RES_t *res = ota_https->res;
    char *hend;
    int32_t len;

    if (res->response_code > 0) return res->response_code;
    if (!res->buf_length) return 0;

    hend = strstr(res->buf, "\r\n\r\n");
    if (!hend) return 0;

    *hend = 0;

    res->response_code = ota_https_parse_header_response_code(res->buf);
    res->content_length = ota_https_parse_header_content_length(res->buf);

    if( (res->response_code >= 300) || (res->response_code < 200) ) return 0;

    len = res->buf_length - (int32_t)((hend + 4) - res->buf);

    memmove(res->buf, hend + 4, len);
    res->buf_length = len;

    return 1;
}

static int32_t ota_https_read_packet(OTA_HTTPS_t *ota_https, uint32_t timeout)
{
    OTA_HTTPS_RES_t *res = ota_https->res;
    fd_set rset;
    int32_t len, ret;    
    
    //set default receive size
    res->buf_length = 0;
    
    qapi_fd_zero(&rset);
    qapi_fd_set(ota_https->sock_fd, &rset);
    if( qapi_select(&rset, NULL, NULL, timeout) == 0) {
        return (-OTA_ERR_HTTPS_SERVER_RSP_TIMEOUT_E);
    }

    if(ota_https->req->port == OTA_HTTPS_PORT || ota_https->req->port == OTA_HTTPS_ALT_PORT) {
        len = qapi_Net_SSL_Read(ota_https->hssl, &res->buf[0], res->buf_size);
        if (len >= 0) {
            res->buf_length = len;
        } else {
            A_PRINTF("SSL read error(%d)\n", len);
            return (-1);
        }
    } else {
        len = qapi_recv(ota_https->sock_fd, &res->buf[0], res->buf_size, 0);
        if (len >= 0) {
            res->buf_length = len;
        } else {
            A_PRINTF("qcom_recv error(%d)\n", len);
            return (-1);
        }
    }

    return len;
}

static int32_t ota_https_connect(OTA_HTTPS_t *ota_https, uint32_t timeout)
{
    struct sockaddr_in foreign_addr;
    struct sockaddr_in6 foreign_addr6;
    struct sockaddr *to;
    uint32_t tolen;  
    int32_t ret;

    if(ota_https_sess->req->v6_enable_flag) {
        memset(&foreign_addr6, 0, sizeof(foreign_addr6));
        memcpy(&foreign_addr6.sin_addr, ota_https->req->ip_v6addr, sizeof(foreign_addr6.sin_addr));
        foreign_addr6.sin_port     = htons(ota_https->req->port);
        foreign_addr6.sin_family   = AF_INET6;
        foreign_addr6.sin_scope_id = ota_https->scope_id;

        to = (struct sockaddr *)&foreign_addr6;
        tolen = sizeof(foreign_addr6);
    } else {
        memset(&foreign_addr, 0, sizeof(foreign_addr));
        foreign_addr.sin_addr.s_addr = ota_https->req->ip_addr;
        foreign_addr.sin_port = htons(ota_https->req->port);
        foreign_addr.sin_family  = AF_INET;
        
        to = (struct sockaddr *)&foreign_addr;
        tolen = sizeof(foreign_addr);
    } 
    
    if ( qapi_connect(ota_https->sock_fd, to, tolen) == -1) {
        ret = OTA_ERR_HTTPS_CONNECT_FAIL_E;
        return ret;
    }

    if (ota_https->req->port == OTA_HTTPS_PORT || ota_https->req->port == OTA_HTTPS_ALT_PORT) {
        if ( qapi_Net_SSL_Connect(ota_https->hssl) < 0) {
            ret= OTA_ERR_HTTPS_SSL_CONNECT_FAIL_E;
            return ret;
        }
    } 

    return OTA_OK_E;
}

ota_Status_Code_t ota_Plugin_Https_Init(const char* interface_name, const char *url)
{
    uint32_t len;
    int32_t  rtn;
    char    *local_url = NULL;
    
    if( ota_https_sess != NULL )
        return OTA_ERR_HTTPS_SESSION_ALREADY_START_E;
    
    //allocate buf to parse url
    len = strlen(url)+1;
    local_url = malloc(len);
    if (local_url == NULL) {
        rtn = OTA_ERR_HTTPS_NO_MEMORY_E;
        goto plugin_https_init_end;
    } 
    memset(local_url, 0, len);
    
    //copy url at local
    strncpy(local_url, url, len);
 
    //open https session
    if( (rtn = ota_https_open((char *)interface_name, local_url)) != OTA_OK_E ) {
        goto plugin_https_init_end;
    } 

    //connect https server
    /* set to non-blocking mode */
    qapi_setsockopt(ota_https_sess->sock_fd, SOL_SOCKET, SO_NBIO, NULL, 0);
    if( (rtn = ota_https_connect(ota_https_sess, ota_https_sess->timeout)) != OTA_OK_E ) {
        goto plugin_https_init_end;
    }
    
    if ((rtn = ota_https_query(ota_https_sess)) != OTA_OK_E) {
        goto plugin_https_init_end;
    }    
    
    rtn = OTA_OK_E;

plugin_https_init_end:
    if( rtn != OTA_OK_E ) {
        ota_https_close();
    }
    
    if(local_url != NULL )
        free(local_url);
    
    return rtn;
}

ota_Status_Code_t ota_Plugin_Https_Fini(void)
{
    ota_https_close();    
    return OTA_OK_E;
}

ota_Status_Code_t ota_Plugin_Https_Recv_Data(uint8_t *buffer, uint32_t buf_len, uint32_t *ret_size)
{
    OTA_HTTPS_RES_t *res;
    int32_t	ret = OTA_OK_E;
    uint32_t offset = 0;

    res = ota_https_sess->res;
    *ret_size = 0;
    res->buf = buffer;
    res->buf_size = buf_len;
    do {
        ret = ota_https_read_packet(ota_https_sess, ota_https_sess->timeout);
        if (ret < 0) {
            ret = OTA_ERR_HTTPS_SERVER_RSP_TIMEOUT_E;
            return ret;
        }

        if (!res->response_code) {
            ret = ota_https_check_header(ota_https_sess);
            if (ret == 0) {
                ret = OTA_ERR_HTTPS_HEADER_ERROR_E;
                return ret; /* error */             
            } 
        }

        if (!res->buf_length) {
            res->buf = buffer;
            res->buf_size = buf_len;            
            continue; // Wait for next packet
        } else {
            //get data
            break;
        }        
    } while (1);

    *ret_size = res->buf_length;
    return OTA_OK_E;
}

ota_Status_Code_t ota_Plugin_Https_Abort(void)
{
    return OTA_OK_E;    
}

#if 0
//example
/**
   @brief This function processes the "HTTPS OTA Upgrade" command from the CLI.

*/
static QCLI_Command_Status_t Command_OTA_HTTPS_Upgrade(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    ota_Status_Code_t resp_code;
    ota_Plugin_t ota_Plugin = { ota_Plugin_Https_Init, 
                                ota_Plugin_Https_Recv_Data,
                                ota_Plugin_Https_Abort, 
                                ota_Plugin_Https_Fini};
    
    if( Parameter_Count != 2 )
    {
        QCLI_Printf(OTA_PRINTF_HANDLE, "usage: https [if_name] [ip]:[port]/[file name]\r\n");
        return QCLI_STATUS_SUCCESS_E;
    }

    resp_code = qapi_OTA_Upgrade(Parameter_List[0].String_Value, &ota_Plugin, Parameter_List[1].String_Value, 0, ota_upgrade_callback );

    if(OTA_OK_E != resp_code)
    {
        QCLI_Printf(OTA_PRINTF_HANDLE, "OTA Image Download Failed ERR:%d\r\n",resp_code);
        if( resp_code == OTA_ERR_TRIAL_IS_RUNNING_E )
        {
            QCLI_Printf(OTA_PRINTF_HANDLE, "Trial Partition is running, need reboot to do OTA.\r\n");
        }    
    } else {
        QCLI_Printf(OTA_PRINTF_HANDLE, "OTA Image Download Completed successfully\r\n");
    }
    
   return QCLI_STATUS_SUCCESS_E;
}
#endif