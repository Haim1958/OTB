/**
 ******************************************************************************
 * @file    otbbcom_icd.h
 * @brief   OTS TCP ICD public definitions and declarations,
			related to the icd: packet structure, message IDs, etc...
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


#ifndef __OTBBCOM_ICD_H__
#define __OTBBCOM_ICD_H__

/* Includes */

/* Definitions */

#define OTBBCOM_ICD_MAX_PACKET_BODY_SIZE					(100u)			// Maximum possible packet body size

#define UINT32_MAX_MSG_COUNTER								(4294967295)

// ID Numbers
#define OTBBCOM_ICD_ID__OTBA						    	(100u)			// OTS main block
#define OTBBCOM_ICD_ID__OTBB					    		(200u)			// TRDS

// Ports
#define OTBBCOM_SERVER_PORT_OTBA							(20750u)
#define OTBBCOM_CLIENT_PORT_OTBB							(20752u)
#define OTBBCOM_SERVER_PORT_OTBA_n							(5540u)												

// Opcodes
#define OTBBCOM_OPCODE__SERDES_SFP_STATUS_ERROR				(0x0001u)
#define OTBBCOM_OPCODE__FPGA_REG_23_VALUE_REQ				(0x0002u)
#define OTBBCOM_OPCODE__FPGA_REG_23_VALUE					(0x0003u)


/* Typedefs */

// Packet Header definition
// All fields are in host endianess (ARM32 - LITTLE)

typedef struct __attribute__((__packed__)) otbbcom_header {
	uint32_t				SIZE;				// = sizeof(OTBBCOM_HEADER) - sizeof(OTBBCOM_HEADER.SIZE) + sizeof(BODY)
	uint8_t					SOURCE;				// ID of sender
	uint8_t					DESTINATION;		// ID of destination. May be "All Computers"
	uint16_t				OPCODE;				// Opcodes 0-255 are reserved
	uint32_t				TIMESTAMP;			// System time (LSB=10us) just before sending the message
	uint32_t				MSGCOUNTER;			// Communication counter for the packet loss detection algorithm.
												// Incremented when message is sent.
												// One message counter for each TCP connection and each UDP port.
												// Incremented independently of the message opcode.
												// Reset to value 0 upon power-up or reset or overflow.
	uint32_t				RES1;				// Reserved for future use, value 0
	uint32_t				RES2;				// Reserved for future use, value 0
	uint32_t				RES4;				// Reserved for future use, value 0
	uint32_t				LEN;				// Payload length
	uint8_t                 OTBB_IS_CONNECTED;									   
} OTBBCOM_HEADER, * OTBBCOM_HEADER_PTR;



typedef struct __attribute__((__packed__)) otbbcom_packet {
	OTBBCOM_HEADER			HDR;
	uint8_t					BODY[OTBBCOM_ICD_MAX_PACKET_BODY_SIZE];
} OTBBCOM_PACKET, * OTBBCOM_PACKET_PTR;


/* Declarations */

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif

#endif //__OTBBCOM_ICD_H__
