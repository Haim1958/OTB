/**
 ******************************************************************************
 * @file    telnet_server.c
 * @brief   Telnet Server functions implementations
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

/*--------------------------------------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------------------------------------*/

#include "../inc/telnet_server.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/*--------------------------------------------------------------------------------------------------------*/
/* PRIVATE DEFINES */
/*--------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------*/
/* PRIVATE VARIABLES */
/*--------------------------------------------------------------------------------------------------------*/

/**
 * @internal
 * @brief Pointer to the TCP port being used for the Telnet server.
 */
static struct tcp_pcb *telnet_pcb;

/**
 * @internal
 * @brief Pointer to the current Telnet server.
 */
static TelnetServer_t telnet_server;

/**
 * @internal
 * @brief Indicates the connection status of the telnet server.
 */
static uint8_t IsConnected = 0u;

/**
 * @internal
 * @brief Buffer for printing the TOSTRING_BUFFER with additional formatting.
 */
static char MESSAGE_BUFFER[SIZE_TOSTRING_BUFFER];

/**
 * @internal
 * @brief The error message provided when an attempt is made to play the game when
 *         it is already being played over a different interface.
 */
static const uint8_t ErrorMessage[] =
		"Already in use...try again later!\r\n";

/**
 * @internal
 * @brief The initialization sequence sent to a remote telnet client when it first connects to the telnet server.
 */
static const uint8_t TelnetInit[] = { TELNET_IAC, TELNET_DO,
TELNET_OPT_SUPPRESS_GA, TELNET_IAC, TELNET_WILL, TELNET_OPT_ECHO };

/**
 * @internal
 * @brief This telnet server will always suppress go ahead generation, regardless of this setting.
 */
static TelnetOpts_t TelnetOptions[] = { { .option = TELNET_OPT_SUPPRESS_GA,
		.flags = (0x01 << OPT_FLAG_WILL) }, { .option = TELNET_OPT_ECHO,
		.flags = (1 << OPT_FLAG_DO) } };


static StreamBufferHandle_t _telnet_rx_queue;
static StreamBufferHandle_t _telnet_tx_queue;
static uint8_t *_telnet_tx_double_buf;

/*--------------------------------------------------------------------------------------------------------*/
/* PRIVATE FUNCTION PROTOTYPES */
/*--------------------------------------------------------------------------------------------------------*/

static err_t TelnetAccept(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t TelnetReceive(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
static err_t TelnetSent(void *arg, struct tcp_pcb *pcb, u16_t len);
static void TelnetError(void *arg, err_t err);
static void ClearToMessageBuffer(void);
static TelnetServer_t* CreateTelnetServer(void);

/*--------------------------------------------------------------------------------------------------------*/
/* PRIVATE METHODS */
/*--------------------------------------------------------------------------------------------------------*/

/*
 * @internal
 * This function is called when the lwIP TCP/IP stack has an incoming
 * connection request on the telnet port.
 *
 * @param arg void* Argument pointer passed to the handler by the lwIP stack.
 * @param pcb tcp_pcb* struct The PCB structure this callback is for.
 * @param err lwIP err_t with the current error status.
 * @retval err lwIP err_t with the result of the this function.
 */
static err_t TelnetAccept(void *arg, struct tcp_pcb *pcb, err_t err) {
#ifdef TELNET_DEBUG
	printf("[Telnet Server] Incoming connection requested on Telnet port.\n\r");
#endif
	err_t ret_err;

	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);

	/* Check if already connected. */
	if (TelnetIsConnected() == 1u) {
		/* There is already a connected client, so refuse this connection with
		 a message indicating this fact. */
#ifdef TELNET_DEBUG
		printf("[Telnet Server] A connection was attempted while an active connection is open.\n\r");
#endif
		tcp_accepted(pcb);
		tcp_arg(pcb, NULL);
		tcp_sent(pcb, TelnetSent);
		tcp_write(pcb, ErrorMessage, sizeof(ErrorMessage), 1);
		tcp_output(pcb);
		/* Temporarily accept this connection until the message is transmitted. */
		return (ERR_OK);
	}

	/* Setup the TCP connection priority. */
	tcp_setprio(pcb, TCP_PRIO_MIN);

	CreateTelnetServer();
#ifdef TELNET_DEBUG
	printf("[Telnet Server] Initializing telnet server.\n\r");
#endif
	tcp_nagle_enable(pcb);
	telnet_server.pcb = pcb;
	telnet_server.pcb->so_options |= SOF_KEEPALIVE;
	telnet_server.pcb->keep_idle = 300000UL; // 5 Minutes
	telnet_server.pcb->keep_intvl = 1000UL; // 1 Second
	telnet_server.pcb->keep_cnt = 9; // 9 Consecutive failures terminate

	/* Mark that a client has connected. */
	IsConnected = 1u;
	/* Accept this connection. */
	tcp_accepted(pcb);
#ifdef TELNET_DEBUG
	printf("[Telnet Server] An incoming connection was accepted.\n\r");
#endif

	/* Setup the TCP callback argument. */
	tcp_arg(pcb, &telnet_server);

	/* Initialize lwIP tcp_recv callback function for pcb  */
	tcp_recv(pcb, TelnetReceive);

	/* Initialize lwIP tcp_err callback function for pcb  */
	tcp_err(pcb, TelnetError);

	/* Initialize lwIP tcp_poll callback function for pcb */
	tcp_poll(pcb, TelnetPoll, 1);

	/* Setup the TCP sent callback function. */
	tcp_sent(pcb, TelnetSent);
	/* Initialize the count of outstanding bytes.  The initial byte acked as
	 part of the SYN -> SYN/ACK sequence is included so that the byte count
	 works out correctly at the end. */
	telnet_server.outstanding = sizeof(TelnetInit) + 1;
	/* Do not close the telnet connection until requested. */
	telnet_server.close = 0;
#ifdef TELNET_DEBUG
	printf("[Telnet Server] Writing the init messages\n\r");
#endif
	/* Send the telnet initialization string. */
	tcp_write(pcb, TelnetInit, sizeof(TelnetInit), 1);
	tcp_output(pcb);

	TelnetWriteDebugMessage("[TELNET] Telnet Server Connected. Welcome.");

	/* Return a success code. */
	ret_err = ERR_OK;
	return ret_err;
}

/*
 * @internal
 * This function is called when the lwIP TCP/IP stack has an incoming packet to
 * be processed.
 *
 * @param arg void* Argument pointer passed to the handler by the lwIP stack.
 * @param pcb tcp_pcb* struct The PCB structure this callback is for.
 * @param p pbuf* struct The data buffer from the lwIP stack.
 * @param err lwIP err_t with the current error status.
 * @retval err lwIP err_t with the result of the this function.
 */
static err_t TelnetReceive(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	struct pbuf *q;
	uint32_t ulIdx;
	uint8_t *pucData;
	TelnetServer_t* server = (TelnetServer_t*) arg;

	if (server == NULL)
	{
		#ifdef TELNET_DEBUG
		printf("[Telnet Server] Could not cast receive args to telnet server struct because they were null.\n\r");
		#endif

		return ERR_VAL;
	}

	/* Process the incoming packet. */
	if (err == ERR_OK && p != NULL)
	{
		#ifdef TELNET_DEBUG
		printf("[Telnet Server] Processing received packet.\n\r");
		#endif

		/* Accept the packet from TCP. */
		tcp_recved(pcb, p->tot_len);

		for (q = p, pucData = (uint8_t*) q->payload; q != NULL; q = q->next)
		{
			/* Loop through the bytes in this pbuf. */
			for (ulIdx = 0; ulIdx < q->len; ulIdx++)
			{
				TelnetProcessCharacter(pucData[ulIdx]);
			}
		}

		pbuf_free(p);
	}
	else if (err == ERR_OK && p == NULL)
	{
		/* If a null packet is passed in, close the connection. */
//		server->length = 0;
		xStreamBufferReset(server->tx_queue);
		TelnetClose();
	}

	return (ERR_OK);
}

/**
 * @internal
 * This function is called when the lwIP TCP/IP stack has received an
 * acknowledge for data that has been transmitted.
 *
 * @param arg void* Argument pointer passed to the handler by the lwIP stack.
 * @param pcb tcp_pcb* struct The PCB structure this callback is for.
 * @param len u16_t The number of bytes which were sent.
 * @retval err lwIP err_t with the result of the this function.
 */
static err_t TelnetSent(void *arg, struct tcp_pcb *pcb, u16_t len) {
	/* See if this is for the game connection or for a secondary connection. */
	if (arg)
	{
		TelnetServer_t* server = (TelnetServer_t*) arg;

		#ifdef TELNET_DEBUG
		printf("[Telnet Server] Sent packet was acknowledged.\n\r");
		#endif

		/* Decrement the count of outstanding bytes. */
		server->outstanding -= len;
	}
	else
	{
		/* See if this is the ACK for the error message. */
		if (len == sizeof(ErrorMessage))
		{
			/* Close this telnet connection now that the error message has been transmitted. */
			tcp_sent(pcb, 0);
			tcp_close(pcb);
		}
	}

	return (ERR_OK);
}

/**
 * @internal
 * This function implements the tcp_err callback function (called when a fatal
 * tcp_connection error occurs. As soon as this function is called, the Telnet
 * server is invalid and should not be used anymore.
 *
 * @param  arg Pointer to argument parameter
 * @param  err Not used
 * @retval None
 */
static void TelnetError(void *arg, err_t err)
{
	printf("[Telnet Server] Telnet error received: %i\n\r", err);

	TelnetServer_t* server = (TelnetServer_t*) arg;
	if (server != NULL)
	{
		mem_free(server);
	}

	IsConnected = 0;
}

/**
 * @internal
 * Creates and initializes a Telnet server.
 *
 * @param none
 * @retval TelnetServer_t* The structure used to represent the current server.
 */
static TelnetServer_t* CreateTelnetServer(void)
{
	#ifdef TELNET_DEBUG
	printf("[Telnet Server] Allocating memory for server.\n\r");
	#endif

	/* Allocate structure server to maintain Telnet connection information */
	IsConnected = 0;
	telnet_server.halt = 0;

	telnet_server.rx_queue			= _telnet_rx_queue;
	telnet_server.tx_queue			= _telnet_tx_queue;
	telnet_server.tx_double_buf		= _telnet_tx_double_buf;

//	telnet_server.recvWrite = 0;
//	telnet_server.recvRead = 0;

	telnet_server.previous = 0;

	xStreamBufferReset(_telnet_tx_queue);

//	telnet_server.length = 0;
//	for (int i = 0; i < TELNET_BUFFER_LENGTH; ++i) {
//		telnet_server.recvBuffer[i] = 0;
//	}

	return &telnet_server;
}

/**
 * @internal
 * Clears the internal message string buffer.
 *
 * @param none
 * @retval none
 */
static void ClearToMessageBuffer(void)
{
	for (uint_fast8_t i = 0; i < SIZE_TOSTRING_BUFFER; ++i) {
		MESSAGE_BUFFER[i] = '\0';
	}
}


static void TelnetWriteBuffer(uint8_t *buffer, uint32_t length)
{
//	portENTER_CRITICAL();
//	xStreamBufferSend(telnet_server.tx_queue, buffer, length, 0);
//	portEXIT_CRITICAL();
	if (length > telnet_server.tx_double_buf_size)
	{
		return;
	}

	if (length > telnet_server.tx_double_buf_size - telnet_server.tx_double_buf_index)
	{
		// Send maximum possible, the rest data will not be sent
		length = telnet_server.tx_double_buf_size - telnet_server.tx_double_buf_index;
	}

	memcpy(&telnet_server.tx_double_buf[telnet_server.tx_double_buf_index], buffer, length);
	telnet_server.tx_double_buf_index += length;
}

/**
 * Initializes the provided TelnetServer_t struct with default values and creates a TCP port for it.
 *
 * @param none
 * @retval none
 */
TelnetStatus_t InitializeTelnetServer(StreamBufferHandle_t rx_queue, StreamBufferHandle_t tx_queue, uint8_t *tx_double_buf)
{
	uint32_t buf_length;

	if (rx_queue == NULL || tx_queue == NULL || tx_double_buf == NULL)
	{
		return TELNET_ERR_INPUT_PARAM;
	}

	_telnet_rx_queue = rx_queue;
	_telnet_tx_queue = tx_queue;
	_telnet_tx_double_buf = tx_double_buf;

	xStreamBufferReset(tx_queue);
	xStreamBufferReset(rx_queue);

	buf_length = xStreamBufferSpacesAvailable(tx_queue);
	memset(tx_double_buf, 0, buf_length);

	return TELNET_OK;
}


/**
 * Initializes the provided TelnetServer_t struct with default values and creates a TCP port for it.
 *
 * @param none
 * @retval none
 */
TelnetStatus_t StartTelnetServer()
{
	uint32_t buf_length;
	err_t err;

	telnet_server.rx_queue = _telnet_rx_queue;
	telnet_server.tx_queue = _telnet_tx_queue;
	telnet_server.tx_double_buf = _telnet_tx_double_buf;

	xStreamBufferReset(telnet_server.rx_queue);
	xStreamBufferReset(telnet_server.tx_queue);

	buf_length = xStreamBufferSpacesAvailable(telnet_server.tx_queue);
	memset(telnet_server.tx_double_buf, 0, buf_length);
	telnet_server.tx_double_buf_size = buf_length;
	telnet_server.tx_double_buf_index = 0;

//	telnet_server.length = 0;
	telnet_server.outstanding = 0;
//	for (uint_fast16_t i = 0; i < sizeof(telnet_server.buffer); ++i) {
//		telnet_server.buffer[i] = 0;
//	}

	/* Create a new tcp pcb */
	#ifdef TELNET_DEBUG
	printf("[Telnet Server] Creating TCP port for Telnet server.\n\r");
	#endif

	telnet_pcb = tcp_new();

	if (telnet_pcb == NULL)
	{
		#ifdef TELNET_DEBUG
		printf("[Telnet Server] Can not create new TCP port.\n\r");
		#endif

		return TELNET_ERR_PCBCREATE;
	}

	#ifdef TELNET_DEBUG
	printf("[Telnet Server] Successfully created Telnet TCP port.\n\r");
	#endif

	/* Bind telnet to port TELNET_PORT */
	err = tcp_bind(telnet_pcb, IP_ADDR_ANY, TELNET_PORT);
	if (err != ERR_OK)
	{
		/* Deallocate the pcb */
		memp_free(MEMP_TCP_PCB, telnet_pcb);
		#ifdef TELNET_DEBUG
		printf("[Telnet Server] Can not bind pcb\n\r");
		#endif

		return TELNET_ERR_BIND;
	}

	/* Start tcp listening for Telnet PCB */
	telnet_pcb = tcp_listen(telnet_pcb);

	#ifdef TELNET_DEBUG
	printf("[Telnet Server] Now listening for incoming connections on port %i\n\r", TELNET_PORT);
	#endif

	/* Initialize LwIP tcp_accept callback function */
	tcp_accept(telnet_pcb, TelnetAccept);
	return TELNET_OK;
}

/**
 * This function is called when the the TCP connection should be closed.
 *
 * @param none
 * @retval none
 */
void TelnetClose(void) {
	printf("Closing telnet connection.\n\r");
	struct tcp_pcb *pcb = telnet_server.pcb;

	/* Remove all callbacks */
	tcp_arg(pcb, NULL);
	tcp_sent(pcb, NULL);
	tcp_recv(pcb, NULL);
	tcp_err(pcb, NULL);
	tcp_poll(pcb, NULL, 0);

	/* Clear the telnet data structure pointer, to indicate that there is no longer a connection. */
	telnet_server.pcb = 0;

	/* Close tcp connection */
	tcp_close(pcb);
	IsConnected = 0;

	/* Re-initialize the Telnet Server */
	StartTelnetServer();
}

/**
 * Returns the connection status of the Telnet server. Since this implementation supports only
 * one client at a time, this method can be used to determine if an incoming request can be honored
 * as well as allowing other parts of the program to adjust their behavior depending on if a client
 * is connected.
 *
 * @param none
 * @retval bool TRUE if the telnet server has an active connection.
 */
uint8_t TelnetIsConnected(void) {
	return (IsConnected);
}

/**
 * Called by the lwIP stack when there is data to send/receive to/from the Telnet server.
 *
 * @param arg void* to argument passed to callback by lwIP stack.
 * @param tpcb tcp_pcb* To the tcp_pcb struct for the current tcp connection.
 * @retval err_t The error/status code.
 */
err_t TelnetPoll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	TelnetServer_t* server = (TelnetServer_t*) arg;

	if (server != NULL)
	{
		uint32_t length = xStreamBufferBytesAvailable(server->tx_queue);
		if ((server->pcb != NULL) && (server->tx_double_buf_index != 0 || length != 0))
		{
			if (length > server->tx_double_buf_size - server->tx_double_buf_index)
			{
				length = server->tx_double_buf_size - server->tx_double_buf_index;
			}

			/* Read data to send from asynchronous queue to a buffer which already contains data to send immediately */
			xStreamBufferReceive(server->tx_queue, &server->tx_double_buf[server->tx_double_buf_index], length, 0);
			server->tx_double_buf_index += length;

			/* Write the data from the transmit buffer. */
			tcp_write(server->pcb, server->tx_double_buf, server->tx_double_buf_index, 1);

			/* Increment the count of outstanding bytes. */
			server->outstanding += server->tx_double_buf_index;

			/* Output the telnet data. */
			tcp_output(server->pcb);

			/* Reset the size of the data in the transmit buffer. */
			server->tx_double_buf_index = 0;
		}

		/* See if the telnet connection should be closed; this will only occur once
		 all transmitted data has been ACKed by the client (so that some or all
		 of the final message is not lost). */
		if (server->pcb && (server->outstanding == 0) && (server->close != 0))
		{
			#ifdef TELNET_DEBUG
			printf("[Telnet Server] Telnet server should be closed.\n\r");
			#endif

			TelnetClose();
		}

		ret_err = ERR_OK;
	}
	else
	{
		#ifdef TELNET_DEBUG
		printf("[Telnet Server] Cannot process poll request due to null server structure.\n\r");
		#endif
		/* Nothing to be done */
		tcp_abort(tpcb);
		ret_err = ERR_ABRT;
	}

	return ret_err;
}

/**
 * Writes a character into the telnet receive buffer.
 *
 * @param character char The character to write.
 * @retval none
 */
void TelnetRecvBufferWrite(uint8_t character)
{
//	unsigned long ulWrite;

	/* Ignore this character if it is the NULL character. */
	if (character == 0) {
		return;
	}

	/* Ignore this character if it is the second part of a CR/LF or LF/CR sequence. */
	if (((character == '\r') && (telnet_server.previous == '\n')) ||
		((character == '\n') && (telnet_server.previous == '\r')))
	{
		return;
	}

	/* Store this character into the receive buffer if there is space for it. */
	xStreamBufferSend(telnet_server.rx_queue, &character, 1, 0);

//	ulWrite = telnet_server.recvWrite;
//	if (((ulWrite + 1) % sizeof(telnet_server.recvBuffer))
//			!= telnet_server.recvRead) {
//		telnet_server.recvBuffer[ulWrite] = character;
//		telnet_server.recvWrite = (ulWrite + 1)
//				% sizeof(telnet_server.recvBuffer);
//#ifdef TELNET_CHAR_DEBUG
//		for (int i = 0; i <= telnet_server.recvWrite; ++i) {
//			printf("%c", telnet_server.recvBuffer[i]);
//		}
//		printf("\n\r");
//#endif
//	} else {
//#ifdef TELNET_DEBUG
//		printf("[Telnet Server] Could not store new character because buffer was full.\n\r");
//#endif
//	}

	/* Save this character as the previously received telnet character. */
	telnet_server.previous = character;
}

///**
// * Reads a character from the telnet interface.
// *
// * @param none
// * @retval char The character read from the telnet interface.
// */
//char TelnetRead(void) {
//	uint32_t read;
//	char ret;
//	/* Return a NULL if there is no data in the receive buffer. */
//	read = telnet_server.recvRead;
//	if (read == telnet_server.recvWrite) {
//		return (0);
//	}
//	/* Read the next byte from the receive buffer. */
//	ret = telnet_server.recvBuffer[read];
//	telnet_server.recvRead = (read + 1) % sizeof(telnet_server.recvBuffer);
//	/* Return the byte that was read. */
//	return (ret);
//}

/**
 * Writes a character to the telnet interface.
 *
 * @param character const char The character to write to the interface.
 * @retval none
 */
//static void TelnetWrite(const char character) {
//	/* Delay until there is some space in the output buffer.  The buffer is not
//	 completly filled here to leave some room for the processing of received
//	 telnet commands. */
//	while (telnet_server.length > (sizeof(telnet_server.buffer) - 32)) {
//#ifdef TELNET_DEBUG
//		printf("[Telnet Server] Telnet buffer is full!\n\r");
//#endif
//		/* Handle periodic timers for LwIP */
////		LwIP_Periodic_Handle(GetLocalTime());
//	}
//
//	/* Write this character into the output buffer. */
//	telnet_server.buffer[telnet_server.length++] = character;
//}

///**
// * Writes a string to the specified Telnet server. Disables Ethernet
// * interrupts during this process in order to prevent an intervening
// * interrupt from corrupting the output buffer.
// *
// * @param string char* Pointer to a C-String to write to the interface.
// * @retval none
// */
//void TelnetWriteString(char* string) {
//	if (TelnetIsConnected()) {
//#if 0
//		Eth_EXTI_Disable();
//#endif
//		while (*string) {
//			TelnetWrite(*string);
//			++string;
//		}
//#if 0
//		Eth_EXTI_Enable();
//#endif
//	}
//}

/**
 * This function will handle a WILL request for a telnet option.  If it is an
 * option that is known by the telnet server, a DO response will be generated
 * if the option is not already enabled.  For unknown options, a DONT response
 * will always be generated.
 *
 * The response (if any) is written into the telnet transmit buffer.
 *
 * @param option char Option for the WILL command.
 * @retval none
 */
void TelnetProcessWill(uint8_t option)
{
	uint32_t index;
	uint8_t response_do[3] = { TELNET_IAC, TELNET_DO, 0 };
	uint8_t response_dont[3] = { TELNET_IAC, TELNET_DONT, 0 };

	response_do[2] = option;
	response_dont[2] = option;

	#ifdef TELNET_CHAR_DEBUG
	printf("[Telnet Server] Processing WILL command with option: %c/0x%02X\n\r", option, option);
	#endif

	for (index = 0; index < (sizeof(TelnetOptions) / sizeof(TelnetOptions[0])); index++)
	{
		/* See if this option matches the option in question. */
		if (TelnetOptions[index].option == option)
		{
			/* See if the WILL flag for this option has already been set. */
			if (((TelnetOptions[index].flags >> OPT_FLAG_WILL) & 0x01) == 0)
			{
				/* Set the WILL flag for this option. */
				TelnetOptions[index].flags = (TelnetOptions[index].flags & 0xFD) | (0x01 << OPT_FLAG_WILL);

				/* Send a DO response to this option. */
				TelnetWriteBuffer(response_do, sizeof(response_do));
//				telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
//				telnet_server.buffer[telnet_server.length++] = TELNET_DO;
//				telnet_server.buffer[telnet_server.length++] = option;
			}
			/* Return without any further processing. */
			return;
		}
	}

	/* This option is not recognized, so send a DONT response. */
	TelnetWriteBuffer(response_dont, sizeof(response_dont));
//	telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
//	telnet_server.buffer[telnet_server.length++] = TELNET_DONT;
//	telnet_server.buffer[telnet_server.length++] = option;
}

/**
 * This function will handle a WONT request for a telnet option.  If it is an
 * option that is known by the telnet server, a DONT response will be generated
 * if the option is not already disabled.  For unknown options, a DONT response
 * will always be generated.
 *
 * The response (if any) is written into the telnet transmit buffer.
 *
 * @param server Pointer to the server to process.
 * @param option char Option for the WONT command.
 * @retval none
 */
void TelnetProcessWont(uint8_t option)
{
	uint32_t index;
	uint8_t response_dont[3] = { TELNET_IAC, TELNET_DONT, 0 };

	response_dont[2] = option;

	#ifdef TELNET_CHAR_DEBUG
	printf("[Telnet Server] Processing WONT command with option: %c/0x%02X\n\r", option, option);
	#endif
	/* Loop through the known options. */
	for (index = 0; index < (sizeof(TelnetOptions) / sizeof(TelnetOptions[0])); index++)
	{
		/* See if this option matches the option in question. */
		if (TelnetOptions[index].option == option)
		{
			/* See if the WILL flag for this option is currently set. */
			if (((TelnetOptions[index].flags >> OPT_FLAG_WILL) & 0x01) == 1)
			{
				/* Clear the WILL flag for this option. */
				TelnetOptions[index].flags = (TelnetOptions[index].flags & 0xFD) | 0x00;

				/* Send a DONT response to this option. */
				TelnetWriteBuffer(response_dont, sizeof(response_dont));
//				telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
//				telnet_server.buffer[telnet_server.length++] = TELNET_DONT;
//				telnet_server.buffer[telnet_server.length++] = option;
			}
			/* Return without any further processing. */
			return;
		}
	}

	/* This option is not recognized, so send a DONT response. */
	TelnetWriteBuffer(response_dont, sizeof(response_dont));
//	telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
//	telnet_server.buffer[telnet_server.length++] = TELNET_DONT;
//	telnet_server.buffer[telnet_server.length++] = option;
}

/**
 * This function will handle a DO request for a telnet option.  If it is an
 * option that is known by the telnet server, a WILL response will be generated
 * if the option is not already enabled.  For unknown options, a WONT response
 * will always be generated.
 *
 * The response (if any) is written into the telnet transmit buffer.
 *
 * @param option char Option for the DO command.
 * @return none
 */
void TelnetProcessDo(uint8_t option)
{
	uint32_t index;
	uint8_t response_will[3] = { TELNET_IAC, TELNET_WILL, 0 };
	uint8_t response_wont[3] = { TELNET_IAC, TELNET_WONT, 0 };

	response_will[2] = option;
	response_wont[2] = option;

	#ifdef TELNET_CHAR_DEBUG
	printf("[Telnet Server] Processing DO command with option: %c/0x%02X\n\r", option, option);
	#endif

	for (index = 0; index < (sizeof(TelnetOptions) / sizeof(TelnetOptions[0])); index++)
	{
		/* See if this option matches the option in question. */
		if (TelnetOptions[index].option == option)
		{
			/* See if the DO flag for this option has already been set. */
			if (((TelnetOptions[index].flags >> OPT_FLAG_DO) & 0x01) == 0)
			{
				/* Set the DO flag for this option. */
				TelnetOptions[index].flags = (TelnetOptions[index].flags & 0xFB) | (0x01 << OPT_FLAG_DO);

				/* Send a WILL response to this option. */
				TelnetWriteBuffer(response_will, sizeof(response_will));
//				telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
//				telnet_server.buffer[telnet_server.length++] = TELNET_WILL;
//				telnet_server.buffer[telnet_server.length++] = option;
			}
			/* Return without any further processing. */
			return;
		}
	}

	// This option is not recognized, so send a WONT response.
	TelnetWriteBuffer(response_wont, sizeof(response_wont));
//	telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
//	telnet_server.buffer[telnet_server.length++] = TELNET_WONT;
//	telnet_server.buffer[telnet_server.length++] = option;
}

/**
 * This funciton will handle a DONT request for a telnet option.  If it is an
 * option that is known by the telnet server, a WONT response will be generated
 * if the option is not already disabled.  For unknown options, a WONT resopnse
 * will always be generated.
 *
 * The response (if any) is written into the telnet transmit buffer.
 *
 * @param option char Option for the DONT command.
 * @return none
 */
void TelnetProcessDont(uint8_t option) {
	uint32_t index;
	uint8_t response_wont[3] = { TELNET_IAC, TELNET_WONT, 0 };

	response_wont[2] = option;

	#ifdef TELNET_CHAR_DEBUG
	printf("[Telnet Server] Processing DONT command with option: %c/0x%02X\n\r", option, option);
	#endif

	for (index = 0; index < (sizeof(TelnetOptions) / sizeof(TelnetOptions[0])); index++)
	{
		/* See if this option matches the option in question. */
		if (TelnetOptions[index].option == option)
		{
			/* See if the DO flag for this option is currently set. */
			if (((TelnetOptions[index].flags >> OPT_FLAG_DO) & 0x01) == 1)
			{
				/* Clear the DO flag for this option. */
				TelnetOptions[index].flags = (TelnetOptions[index].flags & 0xFB) | 0x00;

				/* Send a WONT response to this option. */
				TelnetWriteBuffer(response_wont, sizeof(response_wont));
//				telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
//				telnet_server.buffer[telnet_server.length++] = TELNET_WONT;
//				telnet_server.buffer[telnet_server.length++] = option;
			}
			/* Return without any further processing. */
			return;
		}
	}

	/* This option is not recognized, so send a WONT response. */
	TelnetWriteBuffer(response_wont, sizeof(response_wont));
//	telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
//	telnet_server.buffer[telnet_server.length++] = TELNET_WONT;
//	telnet_server.buffer[telnet_server.length++] = option;
}

/*
 * This function processes a character received from the telnet port, handling
 * the interpretation of telnet commands (as indicated by the telnet interpret
 * as command (IAC) byte).
 *
 * @param character char The character to process.
 * @retval none
 */
void TelnetProcessCharacter(uint8_t character)
{
	switch (telnet_server.state)
	{
		/* The normal state of the parser, were each character is either sent
		 to the UART or is a telnet IAC character. */
		case STATE_NORMAL:
			if (character == TELNET_IAC)
			{
				telnet_server.state = STATE_IAC;
			}
			else
			{
				TelnetRecvBufferWrite(character);
				TelnetWriteBuffer(&character, 1); //Echo the character back
			}
			break;

		case STATE_IAC:
			// Determine how to interpret this character.
			switch (character)
			{
				case TELNET_IAC:
					// Write 0xff to the receive buffer.
					TelnetRecvBufferWrite(0xff);
					telnet_server.state = STATE_NORMAL;
					break;

				case TELNET_WILL:
					// Switch to the WILL mode; the next character will have the option in question.
					telnet_server.state = STATE_WILL;
					break;

				case TELNET_WONT:
					// Switch to the WONT mode; the next character will have the option in question.
					telnet_server.state = STATE_WONT;
					break;

				case TELNET_DO:
					// Switch to the DO mode; the next character will have the option in question.
					telnet_server.state = STATE_DO;
					break;

				case TELNET_DONT:
					// Switch to the DONT mode; the next character will have the option in question.
					telnet_server.state = STATE_DONT;
					break;

				case TELNET_AYT:
					// Send a short string back to the client so that it knows that the server is still alive.
					TelnetWriteBuffer((uint8_t*)"\r\n[Yes]\r\n", strlen("\r\n[Yes]\r\n"));
//					telnet_server.buffer[telnet_server.length++] = '\r';
//					telnet_server.buffer[telnet_server.length++] = '\n';
//					telnet_server.buffer[telnet_server.length++] = '[';
//					telnet_server.buffer[telnet_server.length++] = 'Y';
//					telnet_server.buffer[telnet_server.length++] = 'e';
//					telnet_server.buffer[telnet_server.length++] = 's';
//					telnet_server.buffer[telnet_server.length++] = ']';
//					telnet_server.buffer[telnet_server.length++] = '\r';
//					telnet_server.buffer[telnet_server.length++] = '\n';

					telnet_server.state = STATE_NORMAL;
					break;

				// Explicitly ignore the GA and NOP request, plus provide a catch-all ignore for unrecognized requests.
				case TELNET_GA:
				case TELNET_NOP:
				default:
					telnet_server.state = STATE_NORMAL;
					break;
			}
			break;

		case STATE_WILL:
			TelnetProcessWill(character);
			telnet_server.state = STATE_NORMAL;
			break;

		case STATE_WONT:
			TelnetProcessWont(character);
			telnet_server.state = STATE_NORMAL;
			break;

		case STATE_DO:
			TelnetProcessDo(character);
			telnet_server.state = STATE_NORMAL;
			break;

		case STATE_DONT:
			TelnetProcessDont(character);
			telnet_server.state = STATE_NORMAL;
			break;

		// A catch-all for unknown states.  This should never be reached, but is provided just in case it is ever needed.
		default:
			telnet_server.state = STATE_NORMAL;
			break;
	}
}

/**
 * Print a message to the telnet connection formatted as an error.
 *
 * @param message char* Pointer to the string to send
 * @retval none
 */
void TelnetWriteErrorMessage(char* message)
{
	if (TelnetIsConnected())
	{
		ClearToMessageBuffer();

		char* character = message;

		while (*character)
		{
			character++;
		}

		uint32_t n = snprintf(MESSAGE_BUFFER, sizeof(MESSAGE_BUFFER), ERROR_MESSAGE_HEADER, message);
		TelnetWriteBuffer((uint8_t*)MESSAGE_BUFFER, n);
	}
}

/**
 * Print a message to the telnet connection formatted as a status.
 *
 * @param message char* Pointer to the string to send
 * @retval none
 */
void TelnetWriteStatusMessage(char* message)
{
	if (TelnetIsConnected())
	{
		ClearToMessageBuffer();

		uint8_t count = 0;
		char* character = message;

		while (*character)
		{
			++character;
			++count;
		}

		uint32_t n = snprintf(MESSAGE_BUFFER, sizeof(MESSAGE_BUFFER), STATUS_MESSAGE_HEADER, message);
		TelnetWriteBuffer((uint8_t*)MESSAGE_BUFFER, n);
	}
}

/**
 * Print a message to the telnet connection formatted as a debug.
 *
 * @param message char* Pointer to the string to send
 * @retval none
 */
void TelnetWriteDebugMessage(char* message)
{
	if (TelnetIsConnected())
	{
		ClearToMessageBuffer();

		uint8_t count = 0;
		char* character = message;

		while (*character)
		{
			++character;
			++count;
		}

		uint32_t n = snprintf(MESSAGE_BUFFER, sizeof(MESSAGE_BUFFER), DEBUG_MESSAGE_HEADER, message);
		TelnetWriteBuffer((uint8_t*)MESSAGE_BUFFER, n);
	}
}

/**
 * Print a message to the telnet connection formatted as a command data.
 *
 * @param message char* Pointer to the string to send
 * @retval none
 */
void TelnetWriteCommandDataMessage(char* message)
{
	if (TelnetIsConnected())
	{
		ClearToMessageBuffer();

		uint8_t count = 0;
		char* character = message;

		while (*character)
		{
			++character;
			++count;
		}

		uint32_t n = snprintf(MESSAGE_BUFFER, sizeof(MESSAGE_BUFFER), COMMAND_DATA_MESSAGE_HEADER, message);
		TelnetWriteBuffer((uint8_t*)MESSAGE_BUFFER, n);
	}
}
