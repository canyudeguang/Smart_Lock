/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QPKTZR_H__
#define __QPKTZR_H__

#include <stdint.h>

   /* The following constants define the packet types that can be parsed*/
   /* by this Parser.                                                   */
   /* * NOTE * The HCI types map directly to the HCI packet types       */
   /*          defined for the H4/UART protocol by the Bluetooth        */
   /*          specification and should not be changed.                 */
#define QPKTZR_PACKET_TYPE_HCI_COMMAND                                1
#define QPKTZR_PACKET_TYPE_HCI_ACL_DATA                               2
#define QPKTZR_PACKET_TYPE_HCI_SCO_DATA                               3
#define QPKTZR_PACKET_TYPE_HCI_EVENT                                  4

#define QPKTZR_PACKET_TYPE_15_4                                      10
#define QPKTZR_PACKET_TYPE_UART                                      20
#define QPKTZR_PACKET_TYPE_COEX                                      30

   /* The following structure is a container structure that is used to  */
   /* define a parsed packetizer packet.                                */
typedef struct _tagQPKTZR_Packet_t
{
   unsigned int   PacketType;
   unsigned int   PacketLength;
   unsigned char *PacketData;
} QPKTZR_Packet_t;

   /* The following function is used to initialize the Packetizer       */
   /* module.  This function returns zero if successful or a negative   */
   /* return error code if there was an error.                          */
int QPKTZR_Initialize(void);

   /* The following function is used to de-initialize the Packetizer    */
   /* module.  This function returns zero if successful or a negative   */
   /* return error code if there was an error.                          */
int QPKTZR_Cleanup(void);

   /* The following function is called to process received stream data. */
   /* If the stream data contains a valid packet that was packetized, it*/
   /* is returned as the parsed packet.  The return value from this     */
   /* function represents the number of bytes that are still left in the*/
   /* stream (zero means all bytes were processed).  This function      */
   /* returns a negative return code if there was an error parsing the  */
   /* stream.                                                           */
   /* * NOTE * If this function returns a parsed packet then the        */
   /*          caller *MUST* free this memory by calling the            */
   /*          QPKTZR_Free_Packet_Memory() function.                    */
   /* * NOTE * If this function returns a positive value, then the      */
   /*          caller should call this function again with the remaining*/
   /*          stream buffer bytes (and size).                          */
int QPKTZR_Packetize_Stream(unsigned int StreamBufferSize, unsigned char *StreamBuffer, QPKTZR_Packet_t **ParsedPacket);

   /* The folling function accepts an already packetized packet and     */
   /* calculates the correct stream header that is to be sent (in front */
   /* of the packet data).  This function requires the caller to specify*/
   /* the stream buffer and stream buffer size.  This function can be   */
   /* called with NULL as the stream buffer AND zero as the stream      */
   /* length and it will calculate the required stream header length and*/
   /* return it as the return value from this function.  This function  */
   /* returns a positive value that represents the size of the required */
   /* stream header if successful, or a negative return value if there  */
   /* was an error.                                                     */
int QPKTZR_Build_Stream_Header(QPKTZR_Packet_t *Packet, unsigned int StreamBufferSize, unsigned char *StreamBuffer);

   /* The following function is used to free a packet that was          */
   /* successfully parsed from a stream (and returned via a successful  */
   /* call to the QPKTZR_Packetize_Stream() function.                   */
void QPKTZR_Free_Parsed_Packet(QPKTZR_Packet_t *ParsedPacket);

#endif
