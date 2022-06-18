/**
 ******************************************************************************
 * @file    tcpcom_private.h
 * @brief   TCP Communication private declarations and definitions
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


#ifndef __TCPCOM_PRIVATE_H__
#define __TCPCOM_PRIVATE_H__

/* Includes */

#include "UserLib.h"
#include "lwip.h"
#include "lwip/api.h"
#include "lwip/tcp.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"

/* Definitions */


/* Typedefs */


typedef struct tcpcom_tcp_config {
	ip4_addr_t local_ip;
	ip4_addr_t local_netmask;
	ip4_addr_t local_gw;
	uint16_t local_port;

	char* remote_ip;
	uint16_t remote_port;

	uint32_t tcp_tx_timeout_ms;
	uint32_t tcp_rx_timeout_ms;
	uint32_t tcp_connect_attempts;
} TCPCOM_TCP_CONFIG, * TCPCOM_TCP_CONFIG_PTR;


typedef struct tcpcom_bundle {
	uint8_t							is_connected__AC;					// Flag indicating TCP connection with aircraft is up
	int32_t							socket__AC;							// Socket ID (Client to AC)
	uint32_t						last_handled_phasis_messge;			// Is a new message available in the queue (one that has not been handled yet)?
	uint32_t						last_received_phasis_msg_counter;  	// Counter of the last received message
	uint32_t						last_handled_sig_test_messge;		// Is a new message available in the queue (one that has not been handled yet)?
	uint32_t						last_received_sig_test_msg_counter; // Counter of the last received message
	uint32_t						last_sent_status_msg_counter; 	    // Counter of the last sent message
	uint32_t						last_sent_test_res_msg_counter; 	// Counter of the last sent message
	uint32_t						last_sent_test_report_msg_counter; 	// Counter of the last sent message

	uint32_t                        time_delay_task_conn;
	uint32_t                        time_delay_task_rx;

	TCPCOM_TCP_CONFIG				tcp_config;							// TCP socket configuration

	/* Queues&Mailboxes */
	QueueHandle_t					mailbox__DESCRITE_SIGNAL_TEST;
	QueueHandle_t					mailbox__PHASIS_CONTROL;
	QueueHandle_t					queue__TX;

	/* Tx/Rx Tasks */
	TaskHandle_t 					task__RX;
	TaskHandle_t 					task__CONN;
} TCPCOM_DRIVER_BUNDLE, * TCPCOM_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif


void _tcpcom_task_rx(void *);
void _tcpcom_task_conn(void *);

int32_t _tcpcom_connect(TCPCOM_DRIVER_BUNDLE_PTR driver_bundle);
int32_t _tcpcom_icd_send_message(TCPCOM_DRIVER_BUNDLE_PTR tcpcom_bundle, TCPCOM_PACKET_PTR pPacket);
void _tcpcom_icd_header_ntoh(TCPCOM_PACKET_PTR packet);
int32_t _tcpcom_icd_body_ntoh(TCPCOM_PACKET_PTR packet);
void _tcpcom_icd_print_packet(TCPCOM_PACKET_PTR packet, BaseType_t bIncoming);


#ifdef __cplusplus
}
#endif

#endif //__TCPCOM_PRIVATE_H__
