/**
 ******************************************************************************
 * @file    sercom_protocol.h
 * @brief   Serial Communication Protocol public definitions and declarations,
related to the protocol: packet structure, message IDs, etc...
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; LEGAL INFORMATION</center></h2>
 *
*  RAFAEL ADVANCED DEFENCE SYSTEMS LTD. All rights reserved. Proprietary. CONFIDENTIAL.
*  All Rights Reserved.
* 
*  NOTICE:  All information contained herein is, and remains
*  the property of RAFAEL ADVANCED DEFENCE SYSTEMS LTD. and its suppliers,
*  if any.  The intellectual and technical concepts contained
*  herein are proprietary to RAFAEL ADVANCED DEFENCE SYSTEMS LTD.
*  and its suppliers and may be covered by U.S. and Foreign Patents,
*  patents in process, and are protected by trade secret or copyright law.
*  Dissemination of this information or reproduction of this material
*  is strictly forbidden unless prior written permission is obtained
*  from RAFAEL ADVANCED DEFENCE SYSTEMS LTD.
 *
 ******************************************************************************
 */

 
#ifndef __SERCOM_PROTOCOL_H__
#define __SERCOM_PROTOCOL_H__

/* Includes */


/* Definitions */

#define SERCOM_PROTOCOL_PREAMBLE_BYTE0								(0xDA)
#define SERCOM_PROTOCOL_PREAMBLE_BYTE1								(0xDE)
#define SERCOM_PROTOCOL_FOOTER_BYTE0								(0x18)
#define SERCOM_PROTOCOL_FOOTER_BYTE1								(0x5A)

#define SERCOM_PROTOCOL_PACKET_PAYLOAD_MAX_LENGTH					(50u)

/* Typedefs */

typedef struct sercom_packet {
	uint16_t				PREAMBLE;
	uint8_t					OPCODE;
	uint8_t					SIZE;
	uint8_t					PAYLOAD[SERCOM_PROTOCOL_PACKET_PAYLOAD_MAX_LENGTH];
	uint16_t				FOOTER;
} SERCOM_PACKET, * SERCOM_PACKET_PTR;



/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif
	 


#ifdef __cplusplus
}
#endif

#endif //__SERCOM_PROTOCOL_H__
