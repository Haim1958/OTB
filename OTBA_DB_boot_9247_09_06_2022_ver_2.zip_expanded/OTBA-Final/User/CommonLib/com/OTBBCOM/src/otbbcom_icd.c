/**
 ******************************************************************************
 * @file    otbbcom_icd.c
 * @brief   TCP Communication ICD utility functions.
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

#include <string.h>
#include "cmsis_os.h"
#include "UserLib.h"
#include "app.h"
#include "../inc/otbbcom_icd.h"
#include "../inc/otbbcom_private.h"

// Assumes the packet header opcode (packet type) is already set
static int32_t prepare_header(OTBBCOM_DRIVER_BUNDLE_PTR otbbcom_bundle, OTBBCOM_PACKET_PTR pPacket)
{
	OTBBCOM_HEADER_PTR pHeader = &pPacket->HDR;
	pHeader->MSGCOUNTER = otbbcom_bundle->last_sent_msg_counter;
	pHeader->SIZE = pHeader->LEN + sizeof(OTBBCOM_HEADER) - sizeof(pHeader->SIZE);
	pHeader->SOURCE = otbbcom_bundle->tcp_config.type == OTBBCOM_EDGE_TYPE__OTBA ? OTBBCOM_ICD_ID__OTBA : OTBBCOM_ICD_ID__OTBB;
	pHeader->TIMESTAMP = xTaskGetTickCount() * 100; //LB: TickCount is in ms, *100 is to comply with SW requirements doc: LSB should be 10us. this leaves ~12hr of timestamps...
	
	return IO_ERROR_OTBBCOM__OK;
}

static void _otbbcom_icd_header_hton(OTBBCOM_PACKET_PTR packet)
{
	OTBBCOM_HEADER_PTR header = &packet->HDR;
	
	header->SIZE = htonl(header->SIZE);
	header->OPCODE = htons(header->OPCODE);
	header->TIMESTAMP = htonl(header->TIMESTAMP);
	header->MSGCOUNTER = htonl(header->MSGCOUNTER);

	header->RES1 = htons(header->RES1);
	header->RES2 = htons(header->RES2);
	header->RES4 = htons(header->RES4);

	header->LEN = htons(header->LEN);
}

void _otbbcom_icd_header_ntoh(OTBBCOM_PACKET_PTR packet)
{
	OTBBCOM_HEADER_PTR header = &packet->HDR;
	
	header->SIZE = ntohl(header->SIZE);
	header->OPCODE = ntohs(header->OPCODE);
	header->TIMESTAMP = ntohl(header->TIMESTAMP);
	header->MSGCOUNTER = ntohl(header->MSGCOUNTER);

	header->RES1 = ntohs(header->RES1);
	header->RES2 = ntohs(header->RES2);
	header->RES4 = ntohs(header->RES4);

	header->LEN = ntohs(header->LEN);
}

static int32_t _otbbcom_icd_body_hton(OTBBCOM_PACKET_PTR packet)
{
	packet->HDR.OPCODE = ntohs(packet->HDR.OPCODE);

	// TODO
//	switch (packet->HDR.OPCODE)
//	{
//		case OTBBCOM_ICD_OPCODE__OTB_STATUS:
//			_otbbcom_icd_status_hton((OTBBCOM_PACKET_BODY__STATUS_PTR)packet->BODY);
//			break;
//		case OTBBCOM_ICD_OPCODE__OTB_TEST_RESULT:
//			_otbbcom_icd_test_result_hton((OTBBCOM_PACKET_BODY__OTB_TEST_RESULT_PTR)packet->BODY);
//			break;
//		case OTBBCOM_ICD_OPCODE__OTB_DESCRITE_SIGNAL_REPORT:
//			_otbbcom_icd_signal_discrete_test_report_hton((OTBBCOM_PACKET_BODY__DISCRETE_TEST_REPORT_PTR)packet->BODY);
//			break;
//		default:
//			printf(":::OTBBCOM::: hton: Unknown opcode %d given to _otbbcom_icd_send_message()\n\r", packet->HDR.OPCODE);
//			return IO_ERROR_OTBBCOM__UNDEFINED_TYPE;
//	}

	packet->HDR.OPCODE = ntohs(packet->HDR.OPCODE);
	return IO_ERROR_OTBBCOM__OK;
}

int32_t _otbbcom_icd_body_ntoh(OTBBCOM_PACKET_PTR packet)
{
	return IO_ERROR_OTBBCOM__OK;
}

void _otbbcom_icd_print_packet(OTBBCOM_PACKET_PTR packet, BaseType_t bIncoming)
{
	/***char *prefix = (bIncoming) ? ":::RX:::" : ":::TX:::";
	printf("%s SIZE=%lu; SRC=%d; DST=%d TIME=%lu OPCODE=%d MSGCOUNTER=%lu\r\n",
			prefix,
			packet->HDR.SIZE,
			packet->HDR.SOURCE,
			packet->HDR.DESTINATION,
			packet->HDR.TIMESTAMP,
			packet->HDR.OPCODE,
			packet->HDR.MSGCOUNTER);
	 ***/
	if (bIncoming)
	{
		// TODO
//		switch (packet->HDR.OPCODE)
//		{
//		case OTBBCOM_ICD_OPCODE__PHASIS_CONTROL:
//			printf("%s PHASIS_CONTROL, state: %ld\n", prefix, ((OTBBCOM_PACKET_BODY__PHASIS_CONTROL_PTR)packet->BODY)->OTB_STATE);
//			break;
//		case OTBBCOM_ICD_OPCODE__OTB_DESCRITE_SIGNAL_TEST:
//			printf("%s DESCRITE_SIGNAL_TEST, rf: %d\n", prefix, ((OTBBCOM_PACKET_BODY__DISCRETE_RF_SIGNAL_TEST_PTR)packet->BODY)->RF);
//			break;
//		}
	}
	else
	{
		// TODO
//		switch (packet->HDR.OPCODE)
//		{
//		case OTBBCOM_ICD_OPCODE__OTB_STATUS:
//			printf("%s OTB_STATUS, state: %ld\n", prefix, ((OTBBCOM_PACKET_BODY__STATUS_PTR)packet->BODY)->CUR_STATE);
//			break;
//		case OTBBCOM_ICD_OPCODE__OTB_TEST_RESULT:
//			printf("%s OTB_TEST_RESULT, state: %ld\n", prefix, ((OTBBCOM_PACKET_BODY__OTB_TEST_RESULT_PTR)packet->BODY)->OTB_CURRENT_STATE);
//			break;
//		case OTBBCOM_ICD_OPCODE__OTB_DESCRITE_SIGNAL_REPORT:
//			printf("%s DESCRITE_SIGNAL_REPORT, progress: %ld\n", prefix, ((OTBBCOM_PACKET_BODY__DISCRETE_TEST_REPORT_PTR)packet->BODY)->OTB_DISC_SIG_TEST_PROG);
//			break;
//		}
	}
}

int32_t _otbbcom_icd_send_message(OTBBCOM_DRIVER_BUNDLE_PTR otbbcom_bundle, OTBBCOM_PACKET_PTR pPacket)
{
	int32_t res;
	uint32_t length = 0;
	int32_t write_len;
	
	if (otbbcom_bundle->socket < 0)
	{
		return IO_ERROR_OTBBCOM__NOT_CONNECTED;
	}

	res = prepare_header(otbbcom_bundle, pPacket);
	if (res != IO_ERROR_OTBBCOM__OK)
	{
		return res;
	}
	
	_otbbcom_icd_print_packet(pPacket, pdFALSE);
	length = sizeof(OTBBCOM_HEADER) + pPacket->HDR.LEN;
	
	_otbbcom_icd_header_hton(pPacket);
	res = _otbbcom_icd_body_hton(pPacket);
	if (res != IO_ERROR_OTBBCOM__OK)
	{
		return res;
	}



	write_len = send(otbbcom_bundle->socket, (uint8_t*)pPacket, length, MSG_DONTWAIT);

	if (write_len == length)
	{
		res = IO_ERROR_OTBBCOM__OK;
	}
	else
	{
		res = IO_ERROR_OTBBCOM__FAILED_WRITING_TO_SOCKET;
	}
	
	if (IO_ERROR_OTBBCOM__OK == res)
	{
		otbbcom_bundle->last_sent_msg_counter++;
	}
	
	return res;
}
