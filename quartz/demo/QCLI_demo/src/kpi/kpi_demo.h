/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#define M4_BOOT_PBL_TIME 1
#define M4_BOOT_SBL_TIME 2
#define M4_BOOT_RTOS_TIME 3
#define M4_BOOT_TOTAL_TIME 4

#define WLAN_BOOT_KF_INIT_INDEX 0
#define WLAN_BOOT_KF_POWER_ON_INDEX 10
#define WLAN_BOOT_KF_FW_DOWNLOAD_INDEX 11
#define WLAN_BOOT_KF_WMI_READY_TIME 15

#define WLAN_STORERECALL_KF_INIT_INDEX 3 //due storerecall store bug, index 0 is called delay for suspend time is triggered
#define WLAN_STORERECALL_KF_POWER_ON_INDEX 12
#define WLAN_STORERECALL_KF_FW_DOWNLOAD_INDEX 14
#define WLAN_STORERECALL_KF_DONE_TIME 16

#define FOUR_WAY_HANDSHAKE_COMPLETION 0x10
#define MAX_PASSPHRASE_LENGTH 64

#define WLAN_MAX_SSID_LEN 32
#define WLAN_WPA_KEY_MAX_LEN 63

#define DECISION_TIME_TO_SUSPEND 7
#define TIME_NEEDED_FOR_WAKE_UP 6
/*count based on 1400 byte packet size 
 * ((1500000/8)/1400) accounting for packet loss
 */
#define ONE_MEGABIT_RATE_PACKET_COUNT 134

#define KPI_SEC_MODE_OPEN    0
#define KPI_SEC_MODE_WPA     1

#define RTC_CLOCK_MULTIPLIER 3125
#define RTC_CLOCK_DIVIDER 100000

#define END_OF_TEST_CODE (0xAABBCCDD)

enum Test_Mode
{
    TIME_TEST,
    PACKET_TEST
};

#define KPI_RESUME_DONE_EVENT_MASK   0x1
#define KPI_DHCP_EVENT_MASK          0x2
#define KPI_CONNECT_EVENT_MASK       0x4


typedef struct transmit_params
{
    uint32_t ip_address;    /* peer's IPv4 address */
    uint8_t v6addr[16];     /* peer's IPv6 address */
    int32_t scope_id;
    uint32_t source_ipv4_addr;  /* To fill the 'source address' field in IPv4 header (in net order)
                                 * This is for IP_RAW_TX_HDR.
                                 */
    int packet_size;
    int tx_time;
    int packet_number;
    uint32_t interval_us;
    uint16_t port;          /* port for UDP/TCP, protocol for IP_RAW */
    uint8_t zerocopy_send;  /* 1 = this is zero-copy TX */
    uint8_t test_mode;      /* TIME_TEST or PACKET_TEST */
    uint8_t v6;             /* 1 = this is to TX IPv6 packets */
    uint8_t ip_tos;         /* TOS value in IPv4 header */
} TX_PARAMS;

typedef struct receive_params
{
    uint16_t port;
    uint16_t local_if;
    uint32_t local_address;
    uint8_t local_v6addr[16];
    uint32_t mcIpaddr;
    uint32_t mcRcvIf;
    uint8_t mcIpv6addr[16];
    int32_t scope_id;
    uint8_t mcEnabled;
    uint8_t v6;
} RX_PARAMS;

typedef struct
{
    uint32_t    seconds;        /* number of seconds */
    uint32_t    milliseconds;   /* number of milliseconds */
    uint32_t    ticks;          /* number of systicks */
} time_struct_t;


typedef struct stats {
    time_struct_t first_time;       /* Test start time */
    time_struct_t last_time;
    unsigned long long bytes;       /* Number of bytes received in current test */
    unsigned long long kbytes;      /* Number of kilo bytes received in current test */
    unsigned long long last_bytes;  /* Number of bytes received in the previous test */
    unsigned long long last_kbytes;
    unsigned long long sent_bytes;
    uint32_t    pkts_recvd;
    //uint32_t    pkts_expctd;
    uint32_t    last_interval;
    uint32_t    last_throughput;
    /* iperf stats */
    uint32_t    iperf_display_interval;
    uint32_t    iperf_time_sec;
    uint32_t    iperf_stream_id;
    uint32_t    iperf_udp_rate;
} STATS;


typedef struct kpi_demo_ctx
{
    char *ssid;
    char *bssid;
    char *wpa2_psk;
    uint32 operating_channel;
    uint32 total_packet_count;
    char ip[20];
    qurt_signal_t wlan_kpi_event;
    uint32_t security_mode;
    uint32_t protocol;              /* 1:TCP 2:UDP 4:SSL*/
// not needed    uint32_t zc;                    /* zero-copy */
    uint16_t port;
    int32_t sock_local;             /* Listening socket.*/
    int32_t sock_peer;              /* Foreign socket.*/
    int32_t rxcode; 				/* event code from rx_upcall */
    char* buffer;
    STATS pktStats;
    union params_u
    {
        TX_PARAMS tx_params;
        RX_PARAMS rx_params;
    } params;

}kpi_demo_ctx;

typedef struct kpi_end_of_test {
    int code;
    int packet_count;
} KPI_EOT_PACKET;


void kpi_udp_cumulative_tx();

void Initialize_KPI_Demo();  

