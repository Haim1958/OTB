/**
 ******************************************************************************
 * @file    sercom_protocol.c
 * @brief   Serial Communication Protocol private (protocol related) functions implementations
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

/* Includes */
#include <string.h>
#include <stdlib.h>
#include "UserLib.h"
#include "../inc/sercom.h"
#include "../inc/sercom_private.h"

/* Global variables */


/* Functions definitions */


void _sercom_protocol_reset_parser(SERCOM_INSTANCE_PTR protocol_instance)
{	
	protocol_instance->rx_state = SERCOM_PARSER_STATE__PREAMBLE;
	protocol_instance->rx_packet_field_index = 0;
}

/*******************************************************************************
* Function Name: _sercom_protocol_handle_input_byte
********************************************************************************
Description: Handles input byte with SERCOM protocol
Inputs:
SERCOM_DRIVER_BUNDLE_PTR	- SERCOM driver bundle
char					- input byte
Outputs:
None.
*******************************************************************************/
int32_t _sercom_protocol_handle_input_byte(SERCOM_INSTANCE_PTR protocol_instance, uint8_t input)
{
	switch (protocol_instance->rx_state)
	{
	  case SERCOM_PARSER_STATE__PREAMBLE:
		if (protocol_instance->rx_packet_field_index == 0 &&
			input == SERCOM_PROTOCOL_PREAMBLE_BYTE0)
		{
			protocol_instance->rx_packet.PREAMBLE = ((uint16_t)SERCOM_PROTOCOL_PREAMBLE_BYTE0) << 8;
			protocol_instance->rx_packet_field_index++;
		}
		else if (protocol_instance->rx_packet_field_index == 1 &&
				 input == SERCOM_PROTOCOL_PREAMBLE_BYTE1)
		{
			protocol_instance->rx_packet.PREAMBLE += SERCOM_PROTOCOL_PREAMBLE_BYTE1;
			protocol_instance->rx_packet_field_index = 0;
			protocol_instance->rx_state = SERCOM_PARSER_STATE__OPCODE;
		}
		else
		{
			_sercom_protocol_reset_parser(protocol_instance);
		}
		break;
		
	  case SERCOM_PARSER_STATE__OPCODE:
	  		protocol_instance->rx_packet.OPCODE = input;
	  		protocol_instance->rx_state = SERCOM_PARSER_STATE__SIZE;
	  		break;

	  case SERCOM_PARSER_STATE__SIZE:
		if (input <= SERCOM_PROTOCOL_PACKET_PAYLOAD_MAX_LENGTH)
		{
			protocol_instance->rx_packet.SIZE = input;
			protocol_instance->rx_state = SERCOM_PARSER_STATE__PAYLOAD;
		}
		else
		{
			_sercom_protocol_reset_parser(protocol_instance);
		}
		break;
		
	  case SERCOM_PARSER_STATE__PAYLOAD:
		protocol_instance->rx_packet.PAYLOAD[protocol_instance->rx_packet_field_index] = input;
		
		protocol_instance->rx_packet_field_index++;
		if (protocol_instance->rx_packet_field_index >= protocol_instance->rx_packet.SIZE ||
			protocol_instance->rx_packet_field_index >= SERCOM_PROTOCOL_PACKET_PAYLOAD_MAX_LENGTH)
		{
			protocol_instance->rx_packet_field_index = 0;
			protocol_instance->rx_state = SERCOM_PARSER_STATE__FOOTER;
		}
		break;
		
	  case SERCOM_PARSER_STATE__FOOTER:
		if (protocol_instance->rx_packet_field_index == 0)
		{
			protocol_instance->rx_packet.FOOTER = ((uint16_t)input) << 8;
			protocol_instance->rx_packet_field_index++;
		}
		else if (protocol_instance->rx_packet_field_index == 1)
		{
			if (protocol_instance->rx_packet_field_index == 0 &&
				input == SERCOM_PROTOCOL_FOOTER_BYTE0)
			{
				protocol_instance->rx_packet.FOOTER = ((uint16_t)SERCOM_PROTOCOL_FOOTER_BYTE0) << 8;
				protocol_instance->rx_packet_field_index++;
			}
			else if (protocol_instance->rx_packet_field_index == 1 &&
					 input == SERCOM_PROTOCOL_FOOTER_BYTE1)
			{
				protocol_instance->rx_packet.FOOTER += SERCOM_PROTOCOL_FOOTER_BYTE1;
				protocol_instance->rx_packet_field_index = 0;

				// If no more room in packet queue - drop packets (no delay)
				xQueueSend(protocol_instance->rx_queue, &protocol_instance->rx_packet, 0);

				_sercom_protocol_reset_parser(protocol_instance);
			}
			else
			{
				_sercom_protocol_reset_parser(protocol_instance);
			}
		}
		else
		{
			_sercom_protocol_reset_parser(protocol_instance);
		}
		break;

	  default:
		break;
	}
	
	return IO_ERROR_SERCOM__OK;
}


/*******************************************************************************
* Function Name: _sercom_send_packet
********************************************************************************
Description: Sends the TX packet to the lower driver
Inputs:
SERCOM_DRIVER_BUNDLE_PTR	- driver bundle object
Outputs:
IO_ERROR_SERCOM__OK	- The command has been sent
*******************************************************************************/
int32_t _sercom_send_packet(SERCOM_INSTANCE_PTR protocol_instance, SERCOM_PACKET_PTR tx_packet)
{
	uint32_t index;
	uint8_t temp_arr[sizeof(SERCOM_PACKET) + SERCOM_PROTOCOL_PACKET_PAYLOAD_MAX_LENGTH];
	int32_t err;
	
	/* Prepare the packet */
	
	tx_packet->PREAMBLE = (((uint16_t)SERCOM_PROTOCOL_PREAMBLE_BYTE1) << 8u) + SERCOM_PROTOCOL_PREAMBLE_BYTE0;
	tx_packet->FOOTER = (((uint16_t)SERCOM_PROTOCOL_FOOTER_BYTE1) << 8u) + SERCOM_PROTOCOL_FOOTER_BYTE0;
	
	/* Send the packet */
	index = 0;
	memcpy(&temp_arr[index], (uint8_t*)&tx_packet->PREAMBLE, sizeof(uint16_t));
	index += sizeof(uint16_t);
	
	temp_arr[index] = tx_packet->OPCODE;
	index++;
	
	temp_arr[index] = tx_packet->SIZE;
		index++;

	memcpy(&temp_arr[index], (uint8_t*)&tx_packet->PAYLOAD[0], tx_packet->SIZE);
	index += tx_packet->SIZE;
	
	memcpy(&temp_arr[index], (uint8_t*)&tx_packet->FOOTER, sizeof(uint16_t));
	index += sizeof(uint16_t);
	
	
	err = io_write(protocol_instance->driver, temp_arr, index);
	if (err != IO_ERROR_SERCOM__OK)
	{
		return err;
	}
	
	return IO_ERROR_SERCOM__OK;
}
