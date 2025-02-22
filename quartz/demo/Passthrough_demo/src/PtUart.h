/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __PTUART_H__
#define __PTUART_H__

#define PT_UART_ERROR_SUCCESS                      (0)
#define PT_UART_ERROR_UNABLE_TO_OPEN_TRANSPORT     (-1)
#define PT_UART_ERROR_WRITING_TO_PORT              (-2)

typedef struct PtUart_Initialization_Data_s
{
   unsigned int Flags;
   unsigned int BaudRate;
} PtUart_Initialization_Data_t;

#define PT_UART_INITIALIZATION_DATA_FLAG_USE_HS_UART     (0x1)
#define PT_UART_INITIALIZATION_DATA_FLAG_USE_HW_FLOW     (0x2)

typedef void (*PtUart_Callback_t)(unsigned int DataLength, unsigned char *DataBuffer, unsigned long CallbackParameter);

int PtUart_Initialize(PtUart_Initialization_Data_t *InitializationData, PtUart_Callback_t PtUartCallback, unsigned long CallbackParameter);
void PtUart_UnInitialize(void);
int PtUart_Write(unsigned int Length, unsigned char *Buffer);
void PtUart_Process_Packet(unsigned int Length, const uint8_t *Data);
int PtUart_Wake(void);


   /* Needed types not in QAPIs.                                        */

typedef uint32_t  uint32;

typedef enum
{
   UART_FIRST_PORT = 0,
   UART_DEBUG_PORT = UART_FIRST_PORT,

   UART_SECOND_PORT,
   UART_HS_PORT = UART_SECOND_PORT,

   UART_MAX_PORTS,
}uart_port_id;

typedef enum
{
   UART_SUCCESS = 0,
   UART_ERROR,
}uart_result;

typedef enum
{
  UART_5_BITS_PER_CHAR  = 0,
  UART_6_BITS_PER_CHAR  = 1,
  UART_7_BITS_PER_CHAR  = 2,
  UART_8_BITS_PER_CHAR  = 3,
} uart_bits_per_char;

typedef enum
{
  UART_0_5_STOP_BITS    = 0,
  UART_1_0_STOP_BITS    = 1,
  UART_1_5_STOP_BITS    = 2,
  UART_2_0_STOP_BITS    = 3,
} uart_num_stop_bits;

typedef enum
{
  UART_NO_PARITY        = 0,
  UART_ODD_PARITY       = 1,
  UART_EVEN_PARITY      = 2,
  UART_SPACE_PARITY     = 3,
} uart_parity_mode;

typedef void* uart_handle;

typedef void(*UART_CALLBACK)(uint32 num_bytes, void *cb_data);

typedef struct
{
   uint32                baud_rate;
   uart_parity_mode      parity_mode;
   uart_num_stop_bits    num_stop_bits;
   uart_bits_per_char    bits_per_char;
   uint32                enable_loopback;
   uint32                enable_flow_ctrl;
   UART_CALLBACK         tx_cb_isr;
   UART_CALLBACK         rx_cb_isr;
}uart_open_config;

extern uart_result uart_transmit(uart_handle h, char* buf, uint32 bytes_to_tx, void* cb_data);
extern uart_result uart_receive(uart_handle h, char* buf, uint32 buf_size, void* cb_data);
extern uart_result uart_open(uart_handle* h, uart_port_id id, uart_open_config* config);
extern uart_result uart_close(uart_handle h);

#endif

