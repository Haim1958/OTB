/**
 ******************************************************************************
 * @file    tcpcom_icd.c
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
#include "../inc/tcpcom_icd.h"
#include "../inc/tcpcom_private.h"

// Assumes the packet header opcode (packet type) is already set
static int32_t prepare_header(TCPCOM_DRIVER_BUNDLE_PTR pTCPCOM_bundle, TCPCOM_PACKET_PTR pPacket)
{
	TCPCOM_HEADER_PTR pHeader = &pPacket->HDR;
	pHeader->DESTINATION = TCPCOM_ICD_ID__SPECTRA;
	switch (pHeader->OPCODE)
	{
		case TCPCOM_ICD_OPCODE__OTB_TEST_RESULT:
			pHeader->MSGCOUNTER = pTCPCOM_bundle->last_sent_test_res_msg_counter;
			pHeader->SIZE = sizeof(TCPCOM_PACKET_BODY__OTB_TEST_RESULT);
			break;
		case TCPCOM_ICD_OPCODE__OTB_STATUS:
			pHeader->MSGCOUNTER = pTCPCOM_bundle->last_sent_status_msg_counter;
			pHeader->SIZE = sizeof(TCPCOM_PACKET_BODY__STATUS);
			break;
		case TCPCOM_ICD_OPCODE__OTB_DESCRITE_SIGNAL_REPORT:
			pHeader->MSGCOUNTER = pTCPCOM_bundle->last_sent_test_report_msg_counter;
			pHeader->SIZE = sizeof(TCPCOM_PACKET_BODY__DISCRETE_TEST_REPORT);
			break;
		default:
			printf(":::TCPCOM::: Unknown opcode %d given to prepare_header()\n\r", pHeader->OPCODE);
			return IO_ERROR_TCPCOM__UNDEFINED_TYPE;
	}
	
	pHeader->SIZE += sizeof(TCPCOM_HEADER) - sizeof(pHeader->SIZE);
	pHeader->SOURCE = TCPCOM_ICD_ID__OTBA;
	pHeader->TIMESTAMP = xTaskGetTickCount() * 100; //LB: TickCount is in ms, *100 is to comply with SW requirements doc: LSB should be 10us. this leaves ~12hr of timestamps...
	
	return IO_ERROR_TCPCOM__OK;
}

static void _tcpcom_icd_header_hton(TCPCOM_PACKET_PTR packet)
{
	TCPCOM_HEADER_PTR header = &packet->HDR;
	
	header->SIZE = htonl(header->SIZE);
	header->OPCODE = htons(header->OPCODE);
	header->TIMESTAMP = htonl(header->TIMESTAMP);
	header->MSGCOUNTER = htonl(header->MSGCOUNTER);

	header->RES1 = htons(header->RES1);
	header->RES2 = htons(header->RES2);
	header->RES4 = htons(header->RES4);
	header->RES5 = htons(header->RES5);
}

void _tcpcom_icd_header_ntoh(TCPCOM_PACKET_PTR packet)
{
	TCPCOM_HEADER_PTR header = &packet->HDR;
	
	header->SIZE = ntohl(header->SIZE);
	header->OPCODE = ntohs(header->OPCODE);
	header->TIMESTAMP = ntohl(header->TIMESTAMP);
	header->MSGCOUNTER = ntohl(header->MSGCOUNTER);

	header->RES1 = ntohs(header->RES1);
	header->RES2 = ntohs(header->RES2);
	header->RES4 = ntohs(header->RES4);
	header->RES5 = ntohs(header->RES5);
}

static void _tcpcom_icd_signal_test_ntoh(TCPCOM_PACKET_BODY__DISCRETE_RF_SIGNAL_TEST_PTR signal_test)
{
	signal_test->OTB_DISC_SIG_TEST_PROG = ntohl(signal_test->OTB_DISC_SIG_TEST_PROG);
}

static void _tcpcom_icd_test_result_hton(TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR test_result)
{
	test_result->OTB_CRITICAL_MFL_3 = htons(test_result->OTB_CRITICAL_MFL_3);
	test_result->OTB_CRITICAL_PFL = htons(test_result->OTB_CRITICAL_PFL);
	test_result->OTB_CURRENT_STATE = htonl(test_result->OTB_CURRENT_STATE);
	test_result->OTB_MAINT_SPARE1 = htonl(test_result->OTB_MAINT_SPARE1);
	test_result->OTB_MAINT_SPARE2 = htonl(test_result->OTB_MAINT_SPARE2);
	test_result->OTB_MAINT_SPARE3 = htonl(test_result->OTB_MAINT_SPARE3);
	test_result->OTB_MAINT_SPARE4 = htonl(test_result->OTB_MAINT_SPARE4);
	test_result->OTB_MAINT_SPARE5 = htonl(test_result->OTB_MAINT_SPARE5);
	test_result->OTB_MFL_1 = htons(test_result->OTB_MFL_1);
	test_result->OTB_MFL_2 = htons(test_result->OTB_MFL_2);
	test_result->OTB_NON_CRITICAL_PFL = htons(test_result->OTB_NON_CRITICAL_PFL);
}

static void _tcpcom_icd_signal_discrete_test_report_hton(TCPCOM_PACKET_BODY__DISCRETE_TEST_REPORT_PTR test_report)
{
	test_report->OTB_DISC_SIG_TEST_PROG = htonl(test_report->OTB_DISC_SIG_TEST_PROG);
	test_report->RF_LEVEL = htonl(test_report->RF_LEVEL);
	test_report->RF_STATUS = htonl(test_report->RF_STATUS);
}

static void _tcpcom_icd_status_hton(TCPCOM_PACKET_BODY__STATUS_PTR status)
{
	status->CUR_STATE = htonl(status->CUR_STATE);
}

static void _tcpcom_icd_phasis_control_ntoh(TCPCOM_PACKET_BODY__PHASIS_CONTROL_PTR phasis_control)
{
	phasis_control->OTB_STATE = ntohl(phasis_control->OTB_STATE);
}

static int32_t _tcpcom_icd_body_hton(TCPCOM_PACKET_PTR packet)
{
	packet->HDR.OPCODE = ntohs(packet->HDR.OPCODE);
	switch (packet->HDR.OPCODE)
	{
		case TCPCOM_ICD_OPCODE__OTB_STATUS:
			_tcpcom_icd_status_hton((TCPCOM_PACKET_BODY__STATUS_PTR)packet->BODY);
			break;
		case TCPCOM_ICD_OPCODE__OTB_TEST_RESULT:
			_tcpcom_icd_test_result_hton((TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR)packet->BODY);
			break;
		case TCPCOM_ICD_OPCODE__OTB_DESCRITE_SIGNAL_REPORT:
			_tcpcom_icd_signal_discrete_test_report_hton((TCPCOM_PACKET_BODY__DISCRETE_TEST_REPORT_PTR)packet->BODY);
			break;
		default:
			printf(":::TCPCOM::: hton: Unknown opcode %d given to _tcpcom_icd_send_message()\n\r", packet->HDR.OPCODE);
			return IO_ERROR_TCPCOM__UNDEFINED_TYPE;
	}
	packet->HDR.OPCODE = ntohs(packet->HDR.OPCODE);
	return IO_ERROR_TCPCOM__OK;
}

int32_t _tcpcom_icd_body_ntoh(TCPCOM_PACKET_PTR packet)
{
	switch (packet->HDR.OPCODE)
	{
		case TCPCOM_ICD_OPCODE__OTB_DESCRITE_SIGNAL_TEST:
			_tcpcom_icd_signal_test_ntoh((TCPCOM_PACKET_BODY__DISCRETE_RF_SIGNAL_TEST_PTR)packet->BODY);
			break;
		case TCPCOM_ICD_OPCODE__PHASIS_CONTROL:
			_tcpcom_icd_phasis_control_ntoh((TCPCOM_PACKET_BODY__PHASIS_CONTROL_PTR)packet->BODY);
			break;
		default:
			printf(":::TCPCOM::: ntoh: Unknown opcode %d given to _tcpcom_icd_send_message()\n\r", packet->HDR.OPCODE);
			return IO_ERROR_TCPCOM__UNDEFINED_TYPE;
	}
	
	return IO_ERROR_TCPCOM__OK;
}

static uint32_t _tcpcom_icd_get_packet_length(TCPCOM_PACKET_PTR packet)
{
	switch (packet->HDR.OPCODE)
	{
		case TCPCOM_ICD_OPCODE__OTB_STATUS:
			return sizeof(TCPCOM_HEADER) + sizeof(TCPCOM_PACKET_BODY__STATUS);
		case TCPCOM_ICD_OPCODE__OTB_TEST_RESULT:
			return sizeof(TCPCOM_HEADER) + sizeof(TCPCOM_PACKET_BODY__OTB_TEST_RESULT);
		case TCPCOM_ICD_OPCODE__OTB_DESCRITE_SIGNAL_REPORT:
			return sizeof(TCPCOM_HEADER) + sizeof(TCPCOM_PACKET_BODY__DISCRETE_TEST_REPORT);
		default:
			return 0;
	}
}

void _tcpcom_icd_print_packet(TCPCOM_PACKET_PTR packet, BaseType_t bIncoming)
{
	char *prefix = (bIncoming) ? ":::RX:::" : ":::TX:::";
	/***printf("%s SIZE=%lu; SRC=%d; DST=%d TIME=%lu OPCODE=%d MSGCOUNTER=%lu\n",
			prefix,
			packet->HDR.SIZE,
			packet->HDR.SOURCE,
			packet->HDR.DESTINATION,
			packet->HDR.TIMESTAMP,
			packet->HDR.OPCODE,
			packet->HDR.MSGCOUNTER); ***/
	if (bIncoming)
	{
		switch (packet->HDR.OPCODE)
		{
		case TCPCOM_ICD_OPCODE__PHASIS_CONTROL:
			printf("%s PHASIS_CONTROL, state: %ld\n", prefix, ((TCPCOM_PACKET_BODY__PHASIS_CONTROL_PTR)packet->BODY)->OTB_STATE);
			break;
		case TCPCOM_ICD_OPCODE__OTB_DESCRITE_SIGNAL_TEST:
			printf("%s DESCRITE_SIGNAL_TEST, rf: %d\n", prefix, ((TCPCOM_PACKET_BODY__DISCRETE_RF_SIGNAL_TEST_PTR)packet->BODY)->RF);
			break;
		}
	}
	else
	{
		switch (packet->HDR.OPCODE)
		{
		case TCPCOM_ICD_OPCODE__OTB_STATUS:
			printf("%s OTB_STATUS, state: %ld\n", prefix, ((TCPCOM_PACKET_BODY__STATUS_PTR)packet->BODY)->CUR_STATE);
			break;
		case TCPCOM_ICD_OPCODE__OTB_TEST_RESULT:
		//	printf("%s OTB_TEST_RESULT, state: %ld\n", prefix, ((TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR)packet->BODY)->OTB_CURRENT_STATE);
			break;
		case TCPCOM_ICD_OPCODE__OTB_DESCRITE_SIGNAL_REPORT:
			printf("%s DESCRITE_SIGNAL_REPORT, progress: %ld\n", prefix, ((TCPCOM_PACKET_BODY__DISCRETE_TEST_REPORT_PTR)packet->BODY)->OTB_DISC_SIG_TEST_PROG);
			break;
		}
	}
}

int32_t _tcpcom_icd_send_message(TCPCOM_DRIVER_BUNDLE_PTR tcpcom_bundle, TCPCOM_PACKET_PTR pPacket)
{
	int32_t res;
	uint32_t length = 0;
	uint16_t opcode = pPacket->HDR.OPCODE;
//	uint16_t opcode = ntohs(pPacket->HDR.OPCODE);
	int32_t write_len;
	
	if (!tcpcom_bundle->is_connected__AC)
	{
		return IO_ERROR_TCPCOM__NOT_CONNECTED;
	}

	res = prepare_header(tcpcom_bundle, pPacket);
	if (res != IO_ERROR_TCPCOM__OK)
	{
		return res;
	}
	
	_tcpcom_icd_print_packet(pPacket, pdFALSE);
	
	_tcpcom_icd_header_hton(pPacket);
	res = _tcpcom_icd_body_hton(pPacket);
	if (res != IO_ERROR_TCPCOM__OK)
	{
		return res;
	}
	pPacket->HDR.OPCODE = ntohs(pPacket->HDR.OPCODE);
	length = _tcpcom_icd_get_packet_length(pPacket);
	pPacket->HDR.OPCODE = ntohs(pPacket->HDR.OPCODE);

	//xSemaphoreTake( xSemaphore_TCP_CONN, ( TickType_t ) pdMS_TO_TICKS (0));
	write_len = send(tcpcom_bundle->socket__AC, (uint8_t*)pPacket, length, MSG_DONTWAIT);
	//xSemaphoreGive( xSemaphore_TCP_CONN);

	if (write_len == length)
	{
		res = IO_ERROR_TCPCOM__OK;
	}
	else
	{
		res = IO_ERROR_TCPCOM__FAILED_WRITING_TO_SOCKET;
	}
	
	if (IO_ERROR_TCPCOM__OK == res)
	{
		switch (opcode)
		{
		case TCPCOM_ICD_OPCODE__OTB_STATUS:
			tcpcom_bundle->last_sent_status_msg_counter++;
			break;
		case TCPCOM_ICD_OPCODE__OTB_TEST_RESULT:
			tcpcom_bundle->last_sent_test_res_msg_counter++;
			break;
		case TCPCOM_ICD_OPCODE__OTB_DESCRITE_SIGNAL_REPORT:
			tcpcom_bundle->last_sent_test_report_msg_counter++;
			break;
		}
	}
	
	return res;
}
