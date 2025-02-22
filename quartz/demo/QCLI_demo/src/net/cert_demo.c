/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "qapi_status.h"
#include "bench.h"
#include "qapi_netservices.h"
#include "qapi_dnsc.h"
#include "qapi_fs.h"
#include "qapi_crypto.h"
#include "qapi_csr_generator.h"

#ifdef CONFIG_NET_SSL_DEMO
/*
 * This file contains the command handlers for certificate management operations
 * on non-volatile memory like store, delete, list, etc.
 *
 */

extern QCLI_Group_Handle_t qcli_net_handle; /* Handle for Net Command Group. */

const char * file_type_certificate = "cert";
const char * file_type_ca_list = "ca_list";

uint8_t *cert_data_buf;
uint16_t cert_data_buf_len;

#define CERT_PRINTF(...) QCLI_Printf(qcli_net_handle, __VA_ARGS__)
#define CERTCS_PRINTF(...) QCLI_Printf(qcli_net_handle, __VA_ARGS__)

QCLI_Command_Status_t hash_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t get_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t generate_csr(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t cert_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if(Parameter_Count < 1)
    {
        return QCLI_STATUS_USAGE_E;
    }

    if (0 == strcmp(Parameter_List[0].String_Value, "store")) {
        return store_cert(Parameter_Count - 1, &Parameter_List[1]);
    }
    else if (0 == strcmp(Parameter_List[0].String_Value, "delete")) {
        return delete_cert(Parameter_Count - 1, &Parameter_List[1]);
    }
    else if (0 == strcmp(Parameter_List[0].String_Value, "list")) {
        return list_cert(Parameter_Count - 1, &Parameter_List[1]);
    }
    else if (0 == strcmp(Parameter_List[0].String_Value, "get")) {
        return get_cert(Parameter_Count - 1, &Parameter_List[1]);
    }
    else if (0 == strcmp(Parameter_List[0].String_Value, "hash")) {
        return hash_cert(Parameter_Count - 1, &Parameter_List[1]);
    }
    else if (0 == strcmp(Parameter_List[0].String_Value, "gencsr")) {
        return generate_csr(Parameter_Count - 1, &Parameter_List[1]);
    }

    return QCLI_STATUS_USAGE_E;
}






int certcs_resolve_hostname(char * hostname, struct ip46addr * p_ip46addr, int32_t * p_scope_id)
{
    int are_parameters_valid = 1 &&
        hostname &&
        p_ip46addr &&
        p_scope_id;

    if ( !are_parameters_valid ) {
        return -1;
    }

    memset(p_ip46addr, 0, sizeof(*p_ip46addr));


    if ( inet_pton(AF_INET, hostname, &p_ip46addr->a) == 0 )
    {
        // valid IPv4 address
        p_ip46addr->type = AF_INET;
        return 0;
    }

    if ( inet_pton(AF_INET6, hostname, &p_ip46addr->a) == 0 )
    {
        // valid IPv6 address
        if ( QAPI_IS_IPV6_LINK_LOCAL(&p_ip46addr->a) )
        {
            // if this is a link local address, then the interface must be specified after %
            char * interface_name_with_percent_char = strchr(hostname, '%');
            char * interface_name = NULL;
            if ( interface_name_with_percent_char ) {
                interface_name = interface_name_with_percent_char + 1;
            }
            else {
                // the IPv6 address is link local, but it does not contain the '%' character to specify the interface
                CERTCS_PRINTF("ERROR: IPv6 link local address is specified, but it is missing %% characters after which the interface name is followed\r\n");
                return -2;
            }

            int status = qapi_Net_IPv6_Get_Scope_ID(interface_name, p_scope_id);
            if ( status != 0 ) {
                // the IPv6 address is link local, but cannot find the scope id for the interface to use it on
                CERTCS_PRINTF("ERROR: IPv6 link local address is specified, but cannot resolve the scope id for the interface (%s)\r\n", interface_name);
                return -3;
            }
        }
        p_ip46addr->type = AF_INET6;
        return 0;
    }

    if ( strlen(hostname) >= QAPI_NET_DNS_MAX_HOSTNAME_LEN )
    {
        // the hostname is too long
        CERTCS_PRINTF("ERROR: Hostname (%s) is too long\r\n", hostname);
        return -4;
    }

    p_ip46addr->type = AF_INET;
    if ( qapi_Net_DNSc_Reshost(hostname, p_ip46addr) == 0 )
    {
        // valid hostname, which IPv4 got successfully resolved
        return 0;
    }

    p_ip46addr->type = AF_INET6;
    if ( qapi_Net_DNSc_Reshost(hostname, p_ip46addr) == 0 )
    {
        // valid hostname, which IPv6 got successfully resolved
        return 0;
    }

    CERTCS_PRINTF("ERROR: Failed to resolve hostname (%s)\r\n", hostname);
    return -5;
}


int certcs_connect(
    char * hostname,
    int port,
    int * p_socket
    )
{
    int are_parameters_valid = 1 &&
        hostname &&
        ( port > 0 ) &&
        p_socket;

    if ( !are_parameters_valid ) {
        return -1;
    }

    struct ip46addr ipaddr;
    int32_t scope_id;

    int status = certcs_resolve_hostname(
        hostname,
        &ipaddr,
        &scope_id
        );
    if ( 0 != status ) {
        return -2;
    }

    *p_socket = qapi_socket(ipaddr.type, SOCK_STREAM, 0);
    if ( *p_socket < 0)
    {
        // failed to create socket
        CERTCS_PRINTF("ERROR: failed to create socket\r\n, error=%d", *p_socket);
        return -3;
    }

    struct sockaddr_in sockaddr;
    struct sockaddr_in6 sockaddr6;

    struct sockaddr *to;
    uint32_t tolen;
    if ( ipaddr.type == AF_INET )
    {
        memset(&sockaddr, 0, sizeof(sockaddr));
        sockaddr.sin_addr.s_addr = ipaddr.a.addr4;
        sockaddr.sin_port = htons(port);
        sockaddr.sin_family = AF_INET;

        to      = (struct sockaddr *)&sockaddr;
        tolen   = sizeof(sockaddr);
    }
    else
    {
        memset(&sockaddr6, 0, sizeof(sockaddr6));
        memcpy(&sockaddr6.sin_addr, &ipaddr.a, sizeof(ip6_addr));
        sockaddr6.sin_port     = htons(port);
        sockaddr6.sin_family   = AF_INET6;
        sockaddr6.sin_scope_id = scope_id;

        to      = (struct sockaddr *)&sockaddr6;
        tolen   = sizeof(sockaddr6);
    }

    status = qapi_connect(*p_socket, to, tolen);
    if ( status < 0)
    {
        // failed to connect
        CERTCS_PRINTF("ERROR: failed on a call to qapi_connect, error=%d\r\n", status);
        qapi_socketclose(*p_socket);
        return -4;
    }

    return 0;
}


int certcs_send_request(
    int socket,
    char * filename
    )
{
    int filename_length = strlen(filename);
    int request_length = sizeof(CERT_HEADER_T) + filename_length;

    uint8_t * p_request = 0;
    p_request = (uint8_t *) malloc(request_length);
    if ( !p_request ) {
        CERTCS_PRINTF("ERROR: failed to allocate request message (%d bytes)\r\n", request_length);
        return -2;
    }
    memset(p_request, 0, request_length);

    CERT_HEADER_T * p_request_header = (CERT_HEADER_T *) p_request;
    p_request_header->id[0] = 'C';
    p_request_header->id[1] = 'R';
    p_request_header->id[2] = 'T';
    p_request_header->id[3] = '0';
    p_request_header->length = htonl(filename_length);

    memcpy(&p_request_header->data[0], filename, filename_length);

    int status = 0;
    uint8_t * p_buffer = p_request;
    do
    {
        int bytes_sent = qapi_send(socket, (char *) p_buffer, request_length, 0);
        if ( bytes_sent < 0 ) {
            CERTCS_PRINTF("ERROR: failed on a call to qapi_send, error=%d\r\n", bytes_sent);
            status = -3;
            break;
        }
        if ( bytes_sent > request_length ) {
            CERTCS_PRINTF("ERROR: sent more bytes than expected, expcted=%d bytes, sent=%d bytes", request_length, bytes_sent);
            status = -4;
            break;
        }
        p_buffer += bytes_sent;
        request_length -= bytes_sent;
    } while ( request_length > 0);

    free(p_request);

    return status;
}


int certcs_receive_data(
    int socket,
    uint8_t * p_buffer,
    size_t buffer_size
    )
{
    int are_parameters_valid = 1 &&
        p_buffer &&
        (buffer_size > 0);

    if ( !are_parameters_valid ) {
        return -1;
    }

    do
    {
        fd_set rset;
        qapi_fd_zero(&rset);
        qapi_fd_set(socket, &rset);

        int select_status = qapi_select(&rset, NULL, NULL, CLIENT_WAIT_TIME);

        if ( select_status > 0 )
        {
            // can receive data
            int bytes_received = qapi_recv(socket, (char *) p_buffer, buffer_size, 0);
            if ( bytes_received < 0 ) {
                CERTCS_PRINTF("ERROR: failed on a call to qapi_recv, status=%d\r\n", bytes_received);
                return -3;
            }
            else {
                if ( bytes_received > buffer_size ) {
                    CERTCS_PRINTF("ERROR: received more bytes than expected. expected=%d bytes, received=%d bytes\r\n", buffer_size, bytes_received);
                }
                p_buffer += bytes_received;
                buffer_size -= bytes_received;
            }
        }
        else
        {
            if ( select_status < 0 ) {
                CERTCS_PRINTF("ERROR: no response\n");
                return -2;
            }
        }

    } while ( buffer_size > 0 );

    return 0;
}

typedef struct certcs_heaser_s {
    uint8_t id[4];
    uint32_t length;
} certcs_heaser_t;





int certcs_receive_response(
    int socket,
    uint8_t ** pp_buffer,
    uint32_t * p_buffer_size
    )
{
    int are_params_valid = 1 &&
        pp_buffer &&
        p_buffer_size;

    if ( !are_params_valid ) {
        return -1;
    }

    certcs_heaser_t certcs_header;
    memset(&certcs_header, 0, sizeof(certcs_header));

    int status = certcs_receive_data(socket, (uint8_t*) &certcs_header, sizeof(certcs_header));
    if ( 0 != status ) {
        CERTCS_PRINTF("ERROR: failed to get certcs_header, status=%d\r\n", status);
        return -2;
    }

    int is_valid_certcs_header = 1 &&
        (certcs_header.id[0] == 'C') &&
        (certcs_header.id[1] == 'R') &&
        (certcs_header.id[2] == 'T');
    if ( !is_valid_certcs_header ) {
        CERTCS_PRINTF("ERROR: Invalid certcs header\r\n");
        return -3;
    }

    uint32_t file_size = htonl(certcs_header.length);

    if(file_size == 0) {
        CERTCS_PRINTF("ERROR: file size is 0\r\n");
        return -6;
    }

    *pp_buffer = (uint8_t *) malloc(file_size);
    if ( !*pp_buffer ) {
        CERTCS_PRINTF("ERROR: failed to allocate buffer\r\n");
        return -4;
    }

    status = certcs_receive_data(socket, *pp_buffer, file_size);
    if ( 0 != status ) {
        CERTCS_PRINTF("ERROR: failed to receive data, status=%d\r\n", status);
        free(*pp_buffer);
        *pp_buffer = 0;
        return -5;
    }

    *p_buffer_size = file_size;
    return 0;
}

int certcs_download_file(
    char * hostname,
    int port,
    char * filename,
    uint8_t ** pp_buffer,
    uint32_t * p_buffer_size
    )
{
    int are_parameters_valid = 1 &&
        hostname &&
        ( port > 0 ) &&
        filename &&
        pp_buffer &&
        p_buffer_size;

    if ( !are_parameters_valid ) {
        return -1;
    }

    int socket;
    int status =  certcs_connect(
        hostname,
        port,
        &socket
        );
    if ( 0 != status ) {
        return -2;
    }

    status = certcs_send_request(socket, filename);
    if ( 0 != status ) {
        status = -3;
        goto certcs_download_file_cleanup;
    }

    status = certcs_receive_response(socket, pp_buffer, p_buffer_size);
    if ( 0 != status ) {
        status = -4;
        goto certcs_download_file_cleanup;
    }

certcs_download_file_cleanup:
    qapi_socketclose(socket);
    return status;
}



QCLI_Command_Status_t get_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    char * hostname = 0;
    char * port_as_string = 0;
    char * certificate_or_ca_list_filename = 0;
    char * key_filename = 0;
    char * type_as_string = 0;
    char * output_filename = 0;
    uint16_t port = 0;
    int type = 0;

    uint8_t * certificate_or_ca_list_buffer = 0;
    uint32_t certificate_or_ca_list_buffer_size = 0;
    uint8_t * key_buffer = 0;
    uint32_t key_buffer_size = 0;

    int parameter_index;
    int status;

    int return_status = QCLI_STATUS_ERROR_E;

    if (Parameter_Count < 2)
    {
        CERTCS_PRINTF("Invalid number of arguments\r\n");
        goto get_cert_on_error;
    }

    certificate_or_ca_list_filename = Parameter_List[0].String_Value;
    hostname = Parameter_List[1].String_Value;


    for ( parameter_index = 2; parameter_index < Parameter_Count; parameter_index++ )
    {
        if ( 0 == strcmp((char*)Parameter_List[parameter_index].String_Value, "-p") )
        {
            parameter_index++;
            if ( parameter_index == Parameter_Count ) {
                CERTCS_PRINTF("ERROR: -p option is present, but no port is specified\r\n");
                goto get_cert_on_error;
            }
            port_as_string = (char*)Parameter_List[parameter_index].String_Value;
        }
        else if ( 0 == strcmp((char*)Parameter_List[parameter_index].String_Value, "-k") )
        {
            parameter_index++;
            if ( parameter_index == Parameter_Count ) {
                CERTCS_PRINTF("ERROR: -k option is present, but no key filename is specified\r\n");
                goto get_cert_on_error;
            }
            key_filename = (char*)Parameter_List[parameter_index].String_Value;
        }
        else if ( 0 == strcmp((char*)Parameter_List[parameter_index].String_Value, "-s") )
        {
            parameter_index++;
            if ( parameter_index == Parameter_Count ) {
                CERTCS_PRINTF("ERROR: -s option is present, but no output filename is specified\r\n");
                goto get_cert_on_error;
            }
            output_filename = (char*)Parameter_List[parameter_index].String_Value;
        }

        else if ( 0 == strcmp((char*)Parameter_List[parameter_index].String_Value, "-t") )
        {
            parameter_index++;
            if ( parameter_index == Parameter_Count ) {
                CERTCS_PRINTF("ERROR: -t option is present, but no type (cert/ca_list) is specified\r\n");
                goto get_cert_on_error;
            }
            type_as_string = (char*)Parameter_List[parameter_index].String_Value;
            parameter_index++;
        }
        else
        {
            CERTCS_PRINTF("ERROR: Unknown option (%s)\r\n", (char*)Parameter_List[parameter_index].String_Value);
            goto get_cert_on_error;
        }
    }

    if ( port_as_string ) {
        port = atoi(port_as_string);
    }
    else {
        // default port
        port = 1443;
    }

    if ( !output_filename ) {
        output_filename = certificate_or_ca_list_filename;
    }


    if ( 0 == strcmp(type_as_string, "pem_cert") )
    {
        if ( !certificate_or_ca_list_filename ) {
            CERTCS_PRINTF("ERROR: must specify certificate filename\r\n");
            goto get_cert_on_error;
        }
        if ( !key_filename ) {
            CERTCS_PRINTF("ERROR: must specify key filename\r\n");
            goto get_cert_on_error;
        }
        type = QAPI_NET_SSL_PEM_CERTIFICATE_WITH_PEM_KEY_E;
    }
    else if ( 0 == strcmp(type_as_string, "pem_ca_list") )
    {
        type = QAPI_NET_SSL_PEM_CA_LIST_E;
    }
    else if ( 0 == strcmp(type_as_string, "cert") )
    {
        type = QAPI_NET_SSL_BIN_CERTIFICATE_E;
    }
    else if ( 0 == strcmp(type_as_string, "ca_list"))
    {
        type = QAPI_NET_SSL_BIN_CA_LIST_E;
    }
    else if ( 0 == strcmp(type_as_string, "pem_cert_tee_key") )
    {
        if ( !certificate_or_ca_list_filename ) {
            CERTCS_PRINTF("ERROR: must specify certificate filename to download\r\n");
            goto get_cert_on_error;
        }
        if ( !key_filename ) {
            CERTCS_PRINTF("ERROR: must specify internally generated keypair filename using -k option\r\n");
            goto get_cert_on_error;
        }
        type = QAPI_NET_SSL_PEM_CERTIFICATE_WITH_TEE_KEYPAIR_E;
    }
    else
    {
        CERTCS_PRINTF("ERROR: must use -t option to specify whether this is cert/ca_list/pem_cert/pem_ca_list\r\n");
        goto get_cert_on_error;
    }




    status =
        certcs_download_file(
            hostname,
            port,
            certificate_or_ca_list_filename,
            &certificate_or_ca_list_buffer,
            &certificate_or_ca_list_buffer_size
            );
    if ( 0 != status ) {
        CERTCS_PRINTF("ERROR: failed to download the cert/ca_list file, status=%d\r\n", status);
        goto get_cert_on_error;
    }
    CERTCS_PRINTF("Successfully downloaded %s\r\n", certificate_or_ca_list_filename);


    if ( QAPI_NET_SSL_BIN_CERTIFICATE_E == type )
    {
        qapi_Net_SSL_Cert_Info_t cert_info;
        memset(&cert_info, 0, sizeof(cert_info));
        cert_info.cert_Type = type;
        cert_info.info.bin_Cert.cert_Buf = certificate_or_ca_list_buffer;
        cert_info.info.bin_Cert.cert_Size = certificate_or_ca_list_buffer_size;

        status = qapi_Net_SSL_Cert_Store(&cert_info, output_filename);
        if ( QAPI_OK != status ) {
            CERTCS_PRINTF("Failed to store certificate\r\n");
            goto get_cert_on_error;
        }
        else
        {
            CERTCS_PRINTF("Successfully stored certificate\r\n");
        }
    }
    else if ( QAPI_NET_SSL_BIN_CA_LIST_E == type )
    {
        qapi_Net_SSL_Cert_Info_t cert_info;
        memset(&cert_info, 0, sizeof(cert_info));
        cert_info.cert_Type = type;
        cert_info.info.bin_CA_List.ca_List_Buf = certificate_or_ca_list_buffer;
        cert_info.info.bin_CA_List.ca_List_Size = certificate_or_ca_list_buffer_size;

        status = qapi_Net_SSL_Cert_Store(&cert_info, output_filename);
        if ( QAPI_OK != status ) {
            CERTCS_PRINTF("Failed to store CA list\r\n");
            goto get_cert_on_error;
        }
        else
        {
            CERTCS_PRINTF("Successfully stored CA list\r\n");
        }
    }
    else if ( QAPI_NET_SSL_PEM_CERTIFICATE_WITH_TEE_KEYPAIR_E == type )
    {
        qapi_Net_SSL_Cert_Info_t cert_info;
        memset(&cert_info, 0, sizeof(cert_info));
        cert_info.cert_Type = type;
        cert_info.info.pem_Cert_TEE_Keypair.cert_Buf = certificate_or_ca_list_buffer;
        cert_info.info.pem_Cert_TEE_Keypair.cert_Size = certificate_or_ca_list_buffer_size;
        cert_info.info.pem_Cert_TEE_Keypair.tee_Keypair_Name = key_filename;
        status = qapi_Net_SSL_Cert_Store(&cert_info, output_filename);
        if ( QAPI_OK != status ) {
            CERTCS_PRINTF("ERROR: failed to convert and store certificate, status=%d\r\n", status);
            goto get_cert_on_error;
        }
        CERTCS_PRINTF("Successfully converted and stored certificate\r\n");
    }
    else if ( QAPI_NET_SSL_PEM_CERTIFICATE_WITH_PEM_KEY_E == type )
    {
        status =
            certcs_download_file(
                hostname,
                port,
                key_filename,
                &key_buffer,
                &key_buffer_size
                );
        if ( 0 != status ) {
            CERTCS_PRINTF("ERROR: failed to download key file, status=%d\r\n", status);
            goto get_cert_on_error;
        }
        CERTCS_PRINTF("Successfully downloaded %s\r\n", key_filename);

        qapi_Net_SSL_Cert_Info_t cert_info;
        memset(&cert_info, 0, sizeof(cert_info));
        cert_info.cert_Type = type;
        cert_info.info.pem_Cert.cert_Buf = certificate_or_ca_list_buffer;
        cert_info.info.pem_Cert.cert_Size = certificate_or_ca_list_buffer_size;
        cert_info.info.pem_Cert.key_Buf = key_buffer;
        cert_info.info.pem_Cert.key_Size = key_buffer_size;
        status = qapi_Net_SSL_Cert_Store(&cert_info, output_filename);
        if ( QAPI_OK != status ) {
            CERTCS_PRINTF("ERROR: failed to convert and store certificate, status=%d\r\n", status);
            goto get_cert_on_error;
        }
        CERTCS_PRINTF("Successfully converted and stored certificate\r\n");
    }
    else if ( QAPI_NET_SSL_PEM_CA_LIST_E == type )
    {
        qapi_Net_SSL_Cert_Info_t cert_info;
        memset(&cert_info, 0, sizeof(cert_info));
        qapi_CA_Info_t ca_info;
        ca_info.ca_Buf = certificate_or_ca_list_buffer;
        ca_info.ca_Size = certificate_or_ca_list_buffer_size;
        cert_info.cert_Type = type;
        cert_info.info.pem_CA_List.ca_Cnt = 1;
        cert_info.info.pem_CA_List.ca_Info[0] = &ca_info;
        status = qapi_Net_SSL_Cert_Store(&cert_info, output_filename);
        if ( QAPI_OK != status ) {
            CERTCS_PRINTF("ERROR: failed to convert and store ca_list, status=%d\r\n", status);
            goto get_cert_on_error;
        }
        CERTCS_PRINTF("Successfully converted and stored CA list\r\n");
    }

    CERTCS_PRINTF("%s is stored in NV memory\n", output_filename);
    return_status =  QCLI_STATUS_SUCCESS_E;

get_cert_on_error:
    if ( QCLI_STATUS_SUCCESS_E != return_status ) {
        if ( !output_filename ) {
            CERT_PRINTF("ERROR: failed to store %s\n", output_filename);
        }

        CERTCS_PRINTF("Usage: cert get <filename> <host> [-p <port>] [-k <key>] [-s <name>] -t <filetype>\r\n");
        CERTCS_PRINTF("\t\tfilename: filename on host representing certificate or ca_list in either binary or PEM format\r\n");
        CERTCS_PRINTF("\t\thost: hostname to connect to and retrieve the certificate or ca_list from\r\n");
        CERTCS_PRINTF("\t\tport: port to connect to and retrieve the certificate or ca_list from\r\n");
        CERTCS_PRINTF("\t\tkey: name of a file containing key in pem format (in case of pem_cert), or the name of generated key (in case of pem_cert_tee_key)\r\n");
        CERTCS_PRINTF("\t\tname: name for the certificate / ca_list to be stored on the device\r\n");
        CERTCS_PRINTF("\t\tfiletype: type of the file to retrieve from host, could be: cert/ca_cert/pem_cert/pem_ca_list/pem_cert_tee_key\r\n");
        CERTCS_PRINTF("\t\t\t\tcert: certificate in binary format\r\n");
        CERTCS_PRINTF("\t\t\t\tca_list: CA list in binary format\r\n");
        CERTCS_PRINTF("\t\t\t\tpem_cert: certificate in PEM format\r\n");
        CERTCS_PRINTF("\t\t\t\tpem_ca_list: CA list in PEM format\r\n");
        CERTCS_PRINTF("\t\t\t\tpem_cert_tee_key: certificate in PEM format, key from internally generated keypair\r\n");
    }
    if ( certificate_or_ca_list_buffer ) {
        free(certificate_or_ca_list_buffer);
        certificate_or_ca_list_buffer = 0;
    }
    if ( key_buffer ) {
        free(key_buffer);
        key_buffer = 0;
    }
    return return_status;
}



QCLI_Command_Status_t store_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    char * name;
    char * option_file_type;
    char * file_type;


    if(Parameter_Count < 3) {
        CERT_PRINTF("ERROR: Invalid number of parameters\r\n");
        goto store_cert_on_error;
    }

    name = Parameter_List[0].String_Value;
    option_file_type = Parameter_List[1].String_Value;
    file_type = Parameter_List[2].String_Value;

    if ( 0 ||
        !option_file_type ||
        !file_type ||
        !( strcmp("-t", option_file_type) == 0 ) ||
        !( (strcmp(file_type_certificate, file_type) == 0) || (strcmp(file_type_ca_list, file_type) == 0) )
    )
    {
        CERT_PRINTF("ERROR: Invalid filetype.  Must be either cert or ca_list\r\n");
        goto store_cert_on_error;
    }

    if (cert_data_buf_len == 0)
    {
        CERT_PRINTF("ERROR: no certificate data.\nHint: Use the \"cert get\" command to read a certificate from a certificate server.\n");
        goto store_cert_on_error;
    }

    int result;
    if ( strcmp(file_type_certificate, file_type) == 0 ) {
        qapi_Net_SSL_Cert_Info_t cert_info;
        memset(&cert_info, 0, sizeof(cert_info));
        cert_info.cert_Type = QAPI_NET_SSL_BIN_CERTIFICATE_E;
        cert_info.info.bin_Cert.cert_Buf = cert_data_buf;
        cert_info.info.bin_Cert.cert_Size = cert_data_buf_len;
        result = qapi_Net_SSL_Cert_Store(&cert_info, name);
    }
    else {
        qapi_Net_SSL_Cert_Info_t cert_info;
        memset(&cert_info, 0, sizeof(cert_info));
        cert_info.cert_Type = QAPI_NET_SSL_BIN_CA_LIST_E;
        cert_info.info.bin_CA_List.ca_List_Buf = cert_data_buf;
        cert_info.info.bin_CA_List.ca_List_Size = cert_data_buf_len;
        result = qapi_Net_SSL_Cert_Store(&cert_info, name);
    }

    if (QAPI_OK == result)
    {
        CERT_PRINTF("%s is stored in NV memory\n", name);
    }
    else
    {
        CERT_PRINTF("ERROR: failed to store %s\n", name);
        goto store_cert_on_error;
    }
    return QCLI_STATUS_SUCCESS_E;

store_cert_on_error:
    CERT_PRINTF("Usage: cert store <name> -t <filetype>\n"
                    "This stores a certificate or CA list in non-volatile memory with <name>. filetype must be either cert of ca_list\n");
    return QCLI_STATUS_ERROR_E;
}

QCLI_Command_Status_t delete_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    char *name;
    char * option_file_type;
    char * file_type;

    if(Parameter_Count < 1)
    {
        if (cert_data_buf) {
            cert_data_buf_len = 0;
            free(cert_data_buf);
            cert_data_buf = NULL;
            CERT_PRINTF("Deleted the certificate data stored in RAM.\n");
            return QCLI_STATUS_SUCCESS_E;
        }
        else {
            goto delete_cert_on_error;
        }
    }

    if (Parameter_Count < 3) {
        goto delete_cert_on_error;
    }

    name = Parameter_List[0].String_Value;
    option_file_type = Parameter_List[1].String_Value;
    file_type = Parameter_List[2].String_Value;

    if ( 0 ||
        !option_file_type ||
        !file_type ||
        !( strcmp("-t", option_file_type) == 0 ) ||
        !( (strcmp(file_type_certificate, file_type) == 0) || (strcmp(file_type_ca_list, file_type) == 0) )
    )
    {
        CERT_PRINTF("ERROR: Invalid filetype.  Must be either cert or ca_list\r\n");
        goto delete_cert_on_error;
    }

    int status;
    if ( strcmp(file_type_certificate, file_type) == 0 ) {
        qapi_Net_SSL_Cert_Info_t cert_info;
        memset(&cert_info, 0, sizeof(cert_info));
        cert_info.cert_Type = QAPI_NET_SSL_BIN_CERTIFICATE_E;
        cert_info.info.bin_Cert.cert_Buf = 0;
        cert_info.info.bin_Cert.cert_Size = 0;
        status = qapi_Net_SSL_Cert_Store(&cert_info, name);
    }
    else {
        qapi_Net_SSL_Cert_Info_t cert_info;
        memset(&cert_info, 0, sizeof(cert_info));
        cert_info.cert_Type = QAPI_NET_SSL_BIN_CA_LIST_E;
        cert_info.info.bin_CA_List.ca_List_Buf = 0;
        cert_info.info.bin_CA_List.ca_List_Size = 0;
        status = qapi_Net_SSL_Cert_Store(&cert_info, name);
    }

    if (QAPI_OK == status)
    {
        CERT_PRINTF("Deleted %s from NV memory\n", name);
    }
    else
    {
        CERT_PRINTF("ERROR: failed to delete %s\n", name);
        goto delete_cert_on_error;
    }

    return QCLI_STATUS_SUCCESS_E;

delete_cert_on_error:
    CERT_PRINTF("Usage: cert delete <name> -t <filetype>\n"
            "This deletes the certificate or CA list with <name> from non-volatile memory. filetype must be either cert or ca_list.\n");
    return QCLI_STATUS_ERROR_E;
}

QCLI_Command_Status_t list_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    qapi_Net_SSL_Cert_List_t list;
    int32_t i, numFiles;

    if ( Parameter_Count < 2 ) {
        goto list_cert_on_error;
    }

    if ( 0 != strcmp("-t", Parameter_List[0].String_Value) ) {
        goto list_cert_on_error;
    }

    if ( 0 == strcmp(file_type_certificate, Parameter_List[1].String_Value) ) {
        numFiles = qapi_Net_SSL_Cert_List(QAPI_NET_SSL_CERTIFICATE_E, &list);
    }
    else if ( 0 == strcmp(file_type_ca_list, Parameter_List[1].String_Value) ) {
        numFiles = qapi_Net_SSL_Cert_List(QAPI_NET_SSL_CA_LIST_E, &list);
    }
    else {
        goto list_cert_on_error;
    }

    if (numFiles < 0)
    {
        CERT_PRINTF("ERROR: failed to list files (%d)\n", numFiles);
        return QCLI_STATUS_ERROR_E;
    }

    CERT_PRINTF("%d %s stored in NV memory\n", numFiles, numFiles == 1 ? "file" : "files");
    for (i=0; i<numFiles; i++)
    {
        CERT_PRINTF("%s\n", list.name[i]);
    }
    return QCLI_STATUS_SUCCESS_E;

list_cert_on_error:
    CERT_PRINTF("Usage: cert list -t <filetype>\r\n"
            "Lists the certificates / CA lists stored on the device. filetype must be either cert or ca_list.\r\n");
    return QCLI_STATUS_ERROR_E;
}



QCLI_Command_Status_t hash_cert(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    qapi_Status_t status;

    if ( Parameter_Count != 3 ) {
        status = QAPI_ERR_INVALID_PARAM;
        goto hash_cert_on_error;
    }

    if ( 0 != strcmp("-t", Parameter_List[0].String_Value) ) {
        status = QAPI_ERR_INVALID_PARAM;
        goto hash_cert_on_error;
    }

    char * name = Parameter_List[2].String_Value;

    uint8_t hash[QAPI_CRYPTO_SHA256_DIGEST_BYTES];

    if ( 0 == strcmp(file_type_certificate, Parameter_List[1].String_Value) ) {
        status = qapi_Net_SSL_Cert_Get_Hash(name , QAPI_NET_SSL_CERTIFICATE_E, (qapi_Net_SSL_Cert_Hash_t) hash);
    }
    else if ( 0 == strcmp(file_type_ca_list, Parameter_List[1].String_Value) ) {
        status = qapi_Net_SSL_Cert_Get_Hash(name , QAPI_NET_SSL_CA_LIST_E, (qapi_Net_SSL_Cert_Hash_t) hash);
    }
    else {
        status = QAPI_ERR_INVALID_PARAM;
        goto hash_cert_on_error;
    }

    if ( QAPI_OK != status )
    {
        CERT_PRINTF("ERROR: failed to retrieve hash\r\n");
        goto hash_cert_on_error;
        return QCLI_STATUS_ERROR_E;
    }

    int i;
    for ( i = 0; i < QAPI_CRYPTO_SHA256_DIGEST_BYTES; i++)
    {
        CERT_PRINTF("%02X", hash[i]);
    }
    CERT_PRINTF("\r\n");

    return QCLI_STATUS_SUCCESS_E;

hash_cert_on_error:
    CERT_PRINTF("Usage: cert hash -t <type> <name>\r\n"
            "Prints SHA256 hash of the certificate / CA list stored on the device. type must be either cert or ca_list.  name is the name of the certificate / CA list.\r\n");
    return QCLI_STATUS_ERROR_E;
}


void hexdump(void * p_buffer, size_t buffer_size, size_t bytes_per_line)
{
    if ( 0 ||
        !p_buffer ||
        ( 0 == buffer_size )
        )
    {
        return;
    }

    if ( bytes_per_line == 0 ) {
        bytes_per_line = buffer_size;
    }

    uint8_t * p_temp = (uint8_t *) p_buffer;
    int bytes_printed;
    for ( bytes_printed = 0; bytes_printed < buffer_size; bytes_printed++ )
    {
        if ( (0 != bytes_printed) && ((bytes_printed % bytes_per_line) == 0) ) {
            CERT_PRINTF("\r\n");
        }
        CERT_PRINTF("%02x", p_temp[bytes_printed]);
    }
}


int demo_generate_and_print_pem_csr(
    const char * keypair_filename,
    qapi_Crypto_Keypair_Params_t * keypair_params,
    char * service_provider_id_string
    )
{
    if ( 0 ||
        !keypair_filename ||
        !keypair_params ||
        !service_provider_id_string
        )
    {
        return -1;
    }

    uint8_t * p_pem_csr = 0;
    size_t pem_csr_size = 0;

    size_t service_provider_id_size = strlen(service_provider_id_string);
    uint8_t * p_CSR = 0;
    size_t CSR_Size = 0;
    uint8_t * p_CSR_Protected = 0;
    size_t CSR_Protected_Size = 0;
    uint8_t nonce[QAPI_PROTECTED_CSR_NONCE_SIZE];
    uint8_t tag[QAPI_PROTECTED_CSR_TAG_SIZE];
    int status =
        qapi_Net_SSL_CSR_Generate(
            keypair_filename,
            keypair_params,
            (uint8_t*) service_provider_id_string,
            service_provider_id_size,
            &p_CSR,
            &CSR_Size,
            &p_CSR_Protected,
            &CSR_Protected_Size,
            nonce,
            tag
            );
    if ( 0 != status ) {
        CERT_PRINTF("Failed on a call to qapi_Net_SSL_CSR_Generate(), status=%d\r\n", status);
        status = -2;
        goto demo_generate_and_print_pem_csr_cleanup;
    }

    CERT_PRINTF("CSR hexdump:\r\n");
    hexdump(p_CSR, CSR_Size, 32);
    CERT_PRINTF("\r\n\r\n");

    CERT_PRINTF("Nonce: ");
    hexdump(nonce, sizeof(nonce), sizeof(nonce));
    CERT_PRINTF("\r\n");

    CERT_PRINTF("Tag: ");
    hexdump(tag, sizeof(tag), sizeof(tag));
    CERT_PRINTF("\r\n");

    CERT_PRINTF("Protected CSR hexdump:\r\n");
    hexdump(p_CSR_Protected, CSR_Protected_Size, 32);
    CERT_PRINTF("\r\n\r\n");


    int bin_csr_to_pem_csr(
        const uint8_t * p_bin_csr,
        const size_t bin_csr_size,
        uint8_t ** pp_pem_csr,
        size_t * p_pem_csr_size
        );

    status = bin_csr_to_pem_csr(
        p_CSR,
        CSR_Size,
        &p_pem_csr,
        &pem_csr_size
        );
    if ( 0 != status ) {
        CERT_PRINTF("Failed on a call to bin_csr_to_pem_csr(), status=%d\r\n", status);
        status = -3;
        goto demo_generate_and_print_pem_csr_cleanup;
    }

    CERT_PRINTF("PEM CSR:\r\n");
    int i;
    for ( i = 0; i < pem_csr_size; i++ ) {
        CERT_PRINTF("%c", p_pem_csr[i]);
    }
    CERT_PRINTF("\r\n");


demo_generate_and_print_pem_csr_cleanup:
    if ( p_CSR ) {
        free(p_CSR);
        p_CSR = 0;
    }
    if ( p_CSR_Protected ) {
        free(p_CSR_Protected);
        p_CSR_Protected = 0;
    }
    if ( p_pem_csr ) {
        free(p_pem_csr);
        p_pem_csr = 0;
    }

    return status;
}

//int csr_generator_test();

QCLI_Command_Status_t generate_csr(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if ( Parameter_Count != 4 ) {
        CERT_PRINTF("Invalid number of arguments\r\n");
        goto generate_csr_on_error;

    }

    char * keypair_filename = Parameter_List[0].String_Value;
    char * key_type = Parameter_List[1].String_Value;
    char * key_params = Parameter_List[2].String_Value;
    char * service_provider_id_string = Parameter_List[3].String_Value;

    qapi_Crypto_Keypair_Params_t keypair_params;

    if ( 0 == strcmp(key_type, "ecc") ) {
        keypair_params.key_Type = QAPI_KEYPAIR_PARAM_KEY_TYPE_ECC_E;
    }
    else if ( 0 == strcmp(key_type, "rsa") ) {
        keypair_params.key_Type = QAPI_KEYPAIR_PARAM_KEY_TYPE_RSA_E;
        CERT_PRINTF("Generation of CSR using RSA key is not supported\r\n");
        goto generate_csr_on_error;
    }
    else {
        CERT_PRINTF("Unknown key type: %s.  Only \"ecc\" key type is supported for now", key_type);
        goto generate_csr_on_error;
    }

    if ( QAPI_KEYPAIR_PARAM_KEY_TYPE_ECC_E == keypair_params.key_Type ) {
        if ( 0 == strcmp(key_params, "secp192r1") ) {
            keypair_params.params.ecc.curve_Type = QAPI_EC_CURVE_ID_SECP192R1;
        }
        else if ( 0 == strcmp(key_params, "secp224r1") ) {
            keypair_params.params.ecc.curve_Type = QAPI_EC_CURVE_ID_SECP224R1;
        }
        else if ( 0 == strcmp(key_params, "secp256r1") ) {
            keypair_params.params.ecc.curve_Type = QAPI_EC_CURVE_ID_SECP256R1;
        }
        else if ( 0 == strcmp(key_params, "secp384r1") ) {
            keypair_params.params.ecc.curve_Type = QAPI_EC_CURVE_ID_SECP384R1;
        }
        else {
            CERT_PRINTF("Unsupported curve type: %s\r\n", key_params);
            goto generate_csr_on_error;
        }
    }


    int status = demo_generate_and_print_pem_csr(keypair_filename, &keypair_params, service_provider_id_string);
    if ( 0 != status ) {
        CERT_PRINTF("Failed on a call to demo_generate_and_print_pem_csr(), status=%d\r\n", status);
        //return QCLI_STATUS_ERROR_E;
    }

    return QCLI_STATUS_SUCCESS_E;

generate_csr_on_error:
    CERT_PRINTF("Usage: gencsr keypair_filename key_type key_params service_provider_id_string\r\n");
    CERT_PRINTF("\t\tkeypair_filename: filename to store keypair into, e.g. \"/spinor/keypair.bin\"\r\n");
    CERT_PRINTF("\t\tkey_type: key type \"rsa\" or \"ecc\".  Only \"ecc\" is supported for now\r\n");
    CERT_PRINTF("\t\tkey_params: key parameters.  This should be the curve type in case of \"ecc\" key. E.g. \"secp192r1\", \"secp224r1\", \"secp256r1\" or \"secp384r1\"\r\n");
    return QCLI_STATUS_ERROR_E;
}

#endif
