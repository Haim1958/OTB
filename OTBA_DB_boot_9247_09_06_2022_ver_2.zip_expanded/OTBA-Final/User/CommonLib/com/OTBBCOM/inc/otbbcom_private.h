/**
 ******************************************************************************
 * @file    otbbcom_private.h
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


#ifndef __OTBBCOM_PRIVATE_H__
#define __OTBBCOM_PRIVATE_H__

/* Includes */

#include "UserLib.h"
#include "lwip.h"
#include "lwip/api.h"
#include "lwip/tcp.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"

/* Definitions */


/* Typedefs */


typedef struct otbbcom_tcp_config {
	OTBBCOM_EDGE_TYPE				type;

	char							*remote_ip;

	uint32_t						tcp_tx_timeout_ms;
	uint32_t						tcp_rx_timeout_ms;
	uint32_t						tcp_connect_attempts;
} OTBBCOM_TCP_CONFIG, * OTBBCOM_TCP_CONFIG_PTR;


typedef struct otbbcom_bundle {
//	uint8_t							is_connected;						// Flag indicating TCP connection with aircraft is up
	int32_t							socket;								// Data socket ID
	int32_t							server_conn_socket;					// Server pending connection Socket ID
	int32_t							server_accept;
	int32_t							socket_accept;

	uint32_t						last_sent_msg_counter;
	uint32_t						last_received_msg_counter;
	uint32_t						last_handled_msg_counter;

	uint32_t						OTBB_is_connected;


	uint8_t						    release_sem_receive;
	uint8_t						    task_give_notify;


	OTBBCOM_TCP_CONFIG				tcp_config;							// TCP socket configuration

	/* Queues&Mailboxes */
	QueueHandle_t					mailbox__RX;
	QueueHandle_t					queue__TX;

	/* Tx/Rx Tasks */
	TaskHandle_t 					task__RX;
	TaskHandle_t 					task__CONN;
	TaskHandle_t 					task__OTBB_COMM;

	TimerHandle_t					OTBBcom_task_rx_timer;

} OTBBCOM_DRIVER_BUNDLE, * OTBBCOM_DRIVER_BUNDLE_PTR;

QueueHandle_t queue_TX_GLB;
QueueHandle_t queue_mailbox_RX;

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif


void _otbbcom_task_rx(void *);
void _otbbcom_task_conn(void *);
void _OTBB_COM_examine_task(void *);
//void OTBB_COM_examine_task_rx(void *);


int32_t _otbbcom_create_configure_socket_server(OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle);
int32_t _otbbcom_accept_connection(OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle);
int32_t _otbbcom_create_configure_socket_client(OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle);
int32_t _otbbcom_connect(OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle);
int32_t   _otbbcom_reopen (void);//OTBBCOM_EXAM_BUNDLE_PTR driver_bundle);
int32_t _otbbcom_redelete (OTBBCOM_DRIVER_BUNDLE_PTR driver_bundle);

int32_t _otbbcom_icd_send_message(OTBBCOM_DRIVER_BUNDLE_PTR otbbcom_bundle, OTBBCOM_PACKET_PTR pPacket);
void _otbbcom_icd_header_ntoh(OTBBCOM_PACKET_PTR packet);
int32_t _otbbcom_icd_body_ntoh(OTBBCOM_PACKET_PTR packet);
void _otbbcom_icd_print_packet(OTBBCOM_PACKET_PTR packet, BaseType_t bIncoming);


OTBBCOM_DRIVER_BUNDLE_PTR gl_otbb_bundle;
TimerHandle_t gl_otbb_task_rx;
#ifdef __cplusplus
}
#endif

#endif //__OTBBCOM_PRIVATE_H__
