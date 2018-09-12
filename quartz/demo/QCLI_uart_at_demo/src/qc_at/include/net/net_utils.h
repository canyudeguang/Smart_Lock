/*
* Copyright (c) 2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef _netutils_h_
#define _netutils_h_

#include <stdint.h>

#undef htons
#undef ntohs
#undef htonl
#undef ntohl
#define htons(s)    ((((s) >> 8) & 0xff) | (((s) << 8) & 0xff00))
#define ntohs(s)    htons(s)
#define htonl(l)    (((((l) >> 24) & 0x000000ff)) | \
                    ((((l) >>  8) & 0x0000ff00)) | \
                    (((l) & 0x0000ff00) <<  8) | \
                    (((l) & 0x000000ff) << 24))
#define ntohl(l)    htonl(l)

#ifndef min
#define  min(a,b)    (((a) <= (b)) ? (a) : (b))
#endif

#define ET_ADDRLEN  6

#define HEX_BYTES_PER_LINE      16

#define INTR_DISABLE()
#define INTR_ENABLE()

/* queue element: cast to right type */
struct q_elt
{
    struct q_elt *qe_next;  /* pointer to next elt */
};
typedef struct q_elt  *qp;  /* q pointer */

/* queue header */
typedef struct queue
{
    struct q_elt *q_head;   /* first element in queue */
    struct q_elt *q_tail;   /* last element in queue */
    int  q_len;             /* number of elements in queue */
} QUEUE_T;

typedef struct
{
    uint32_t    seconds;        /* number of seconds */
    uint32_t    milliseconds;   /* number of milliseconds */
    uint32_t    ticks;          /* number of systicks */
} time_struct_t;

void enqueue(QUEUE_T *q, void *item);
void * dequeue(QUEUE_T *q);

/* Return milliseconds */
uint32_t app_get_time(time_struct_t *time);
void app_msec_delay(uint32_t ms);
uint32_t app_get_time_difference(time_struct_t *time1, time_struct_t *time2);

void app_hexdump(void *inbuf, unsigned inlen, int ascii, int addr);
int hwaddr_pton(const char *txt, uint8_t *hwaddr, size_t buflen);
int hexstr2bin(const char *hex, uint8_t *buf, size_t len);

#endif /* _netutils_h_ */
