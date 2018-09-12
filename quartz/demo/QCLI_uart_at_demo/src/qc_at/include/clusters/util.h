/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdint.h>

#define BE2CPU16(x)   ((uint16_t)(((x)<<8)&0xff00) | (uint16_t)(((x)>>8)&0x00ff))
#define BE2CPU32(x)   ((((x)<<24)&0xff000000) | (((x)&0x0000ff00)<<8) | (((x)&0x00ff0000)>>8) | (((x)>>24)&0x000000ff))
#define LE2CPU16(x)   (x)
#define LE2CPU32(x)   (x)
#define CPU2BE16(x)   ((uint16_t)(((x)<<8)&0xff00) | (uint16_t)(((x)>>8)&0x00ff))
#define CPU2BE32(x)   ((((x)<<24)&0xff000000) | (((x)&0x0000ff00)<<8) | (((x)&0x00ff0000)>>8) | (((x)>>24)&0x000000ff))
#define CPU2LE32(x)   (x)
#define CPU2LE16(x)   (x)



#define htonl(l) (((((l) >> 24) & 0x000000ff)) | \
                 ((((l) >>  8) & 0x0000ff00)) | \
                 (((l) & 0x0000ff00) <<  8) | \
                 (((l) & 0x000000ff) << 24))
#define ntohl(l) htonl(l)
#define htons(s) ((((s) >> 8) & 0xff) | \
                 (((s) << 8) & 0xff00))
#define ntohs(s) htons(s)


#define AF_INET              2     /* internetwork: UDP, TCP, etc. */
#define AF_INET6             3     /* IPv6 */

int32_t check_empty_ip_addr_string(const char *src);
uint8_t ascii_to_hex(char val);
char hex_to_ascii(uint8_t val);
uint32_t mystrtoul(const char* arg, const char* endptr, int base);
int32_t parse_ipv4_ad(unsigned long *ip_addr, unsigned *sbits, char *stringin);
uint32_t ether_aton(const char *orig, uint8_t *eth);

#ifndef __ICCARM__           /* IAR */
int ishexdigit(char digit);
#endif
