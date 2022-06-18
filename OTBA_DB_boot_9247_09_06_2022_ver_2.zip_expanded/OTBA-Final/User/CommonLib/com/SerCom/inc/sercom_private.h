/**
 ******************************************************************************
 * @file    sercom_private.h
 * @brief   Serial Communication Protocol private definitions and declarations
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

 
#ifndef __SERCOM_PRIVATE_H__
#define __SERCOM_PRIVATE_H__

/* Includes */

#include "./sercom_protocol.h"

/* Definitions */


/* Typedefs */

typedef enum {
	SERCOM_PARSER_STATE__PREAMBLE,								// Waiting to completely receive PREAMBLE field
	SERCOM_PARSER_STATE__OPCODE,								// Waiting to receive OPCODE field
	SERCOM_PARSER_STATE__SIZE,									// Waiting to receive SIZE field
	SERCOM_PARSER_STATE__PAYLOAD,								// Waiting to completely receive PAYLOAD field
	SERCOM_PARSER_STATE__FOOTER,								// Waiting to completely receive FOOTER field
		
	SERCOM_PARSER_STATE_COUNT
} SERCOM_PARSER_STATE;


typedef struct sercom_instance {
	uint32_t						index;						// Instance index

	IO_HANDLER_PTR					driver;         			// Low layer driver
	
	osThreadId						tx_task;					// TX task - transmitts packets to low level driver
	osThreadId						rx_task;					// RX task - receives byte stream from low level driver, parses and forwards to app

	QueueHandle_t					tx_queue;					// Packets TX queue
	QueueHandle_t					rx_queue;					// Packets TX queue

	uint32_t						tx_timeout_ms;				// Maximum TX timeout in milliseconds
	uint32_t						rx_timeout_ms;				// Maximum RX timeout in milliseconds

	SERCOM_PARSER_STATE				rx_state;					// Packet parsing state
	SERCOM_PACKET					rx_packet;					// Parsed packet
	uint8_t							rx_packet_field_index;		// Number of bytes received for current field
} SERCOM_INSTANCE, * SERCOM_INSTANCE_PTR;


typedef struct sercom_bundle {
	SERCOM_INSTANCE_PTR				*instance;
	uint32_t						num_instances;
} SERCOM_DRIVER_BUNDLE, * SERCOM_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif
	 

int32_t _sercom_protocol_handle_input_byte(SERCOM_INSTANCE_PTR, uint8_t);
int32_t _sercom_send_packet(SERCOM_INSTANCE_PTR, SERCOM_PACKET_PTR);

void _sercom_task_tx(void * argument);
void _sercom_task_rx(void * argument);


#ifdef __cplusplus
}
#endif

#endif //__SERCOM_PRIVATE_H__
