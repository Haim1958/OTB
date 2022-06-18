/**
 ******************************************************************************
 * @file    telnet_server.h
 * @brief   Telnet Server declarations and definitions
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


#ifndef __TELNET_SERVER_H__
#define __TELNET_SERVER_H__


/* Includes */
#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "lwip/tcp.h"


/* Definitions */

// Comment out if not required
//#define TELNET_DEBUG

#define TELNET_BUFFER_LENGTH 2056
#define SIZE_TOSTRING_BUFFER 512U
#define TELNET_PORT 23U

#define DEBUG_MESSAGE_HEADER "\n\r--------------------\n\rDebug Message\n\r\tMessage: %s\n\r--------------------\n\r\x1E"
#define ERROR_MESSAGE_HEADER "\n\r--------------------\n\rError Message\n\r\tMessage: %s\n\r--------------------\n\r\x1E"
#define STATUS_MESSAGE_HEADER "\n\r--------------------\n\rStatus Message\n\r\tMessage: %s\n\r--------------------\n\r\x1E"
#define COMMAND_DATA_MESSAGE_HEADER "\n\r--------------------\n\rCommand Data Message\n\r\tMessage: %s\n\r--------------------\n\r\x1E"

#define TELNET_NOT_CONNECTED	(0)
#define TELNET_CONNECTED		(1)

// Telnet commands, as defined by RFC854.
#define TELNET_IAC              ((char) 255)
#define TELNET_WILL             ((char) 251)
#define TELNET_WONT             ((char) 252)
#define TELNET_DO               ((char) 253)
#define TELNET_DONT             ((char) 254)
#define TELNET_SE               ((char) 240)
#define TELNET_NOP              ((char) 241)
#define TELNET_DATA_MARK        ((char) 242)
#define TELNET_BREAK            ((char) 243)
#define TELNET_IP               ((char) 244)
#define TELNET_AO               ((char) 245)
#define TELNET_AYT              ((char) 246)
#define TELNET_EC               ((char) 247)
#define TELNET_EL               ((char) 248)
#define TELNET_GA               ((char) 249)
#define TELNET_SB               ((char) 250)

// Telnet options, as defined by RFC856-RFC861.
#define TELNET_OPT_BINARY       ((char) 0)
#define TELNET_OPT_ECHO         ((char) 1)
#define TELNET_OPT_SUPPRESS_GA  ((char) 3)
#define TELNET_OPT_STATUS       ((char) 5)
#define TELNET_OPT_TIMING_MARK  ((char) 6)
#define TELNET_OPT_EXOPL        ((char) 255)

#define OPT_FLAG_WILL           ((uint8_t) 1)
#define OPT_FLAG_DO             ((uint8_t) 2)


/* Typedefs */


typedef struct {
	char option;			// The option byte.
	char flags;				// The flags for this option. The bits in this byte are defined by OPT_FLAG_WILL and OPT_FLAG_DO.
} TelnetOpts_t;


typedef enum {
	STATE_NORMAL,			// The telnet option parser is in its normal mode.  Characters are passed as is until an IAC byte is received.
	STATE_IAC,				// The previous character received by the telnet option parser was an IAC byte.
	STATE_WILL,				// The previous character sequence received by the telnet option parser was IAC WILL.
	STATE_WONT,				// The previous character sequence received by the telnet option parser was IAC WONT.
	STATE_DO,				// The previous character sequence received by the telnet option parser was was IAC DO.
	STATE_DONT,				// The previous character sequence received by the telnet option parser was was IAC DONT.
} TelnetState_t;

typedef enum {
	TELNET_OK,				// Everything is normal with the telnet server.
	TELNET_ERR_INPUT_PARAM,	// There was an error with function input parameter.
	TELNET_ERR_CONNECTED,	// There was an error in connection with the telnet server.
	/*TELNET_ERR_BADALOC,*/		// There was an error allocating memory for the telnet server.
	TELNET_ERR_BIND, 		// There was an error binding a socket to a port for the telnet server.
	TELNET_ERR_PCBCREATE	// There was an error creating a PCB structure for the telnet server.
} TelnetStatus_t;


typedef struct {
	int						halt;		// Halt signal when the lwIP TCP/IP stack has detected an error
	TelnetState_t			state;		// The current state of the telnet option parser.

	uint32_t				close;		// A value that is non-zero when the telnet connection should be closed down.

//	unsigned char		buffer[TELNET_BUFFER_LENGTH];		// A buffer used to construct a packet of data to be transmitted to the telnet client.
//	volatile unsigned long	length;		// The number of bytes of valid data in the telnet packet buffer.
//	unsigned char		recvBuffer[TELNET_BUFFER_LENGTH];		// A buffer used to receive data from the telnet connection.
//	volatile unsigned long	recvWrite;		// The offset into g_pucTelnetRecvBuffer of the next location to be written in the buffer.
//											// The buffer is full if this value is one less than g_ulTelnetRecvRead (modulo the buffer size).
//	volatile unsigned long	recvRead;		// The offset into g_pucTelnetRecvRead of the next location to be read from the buffer.
//											// The buffer is empty if this value is equal to g_ulTelnetRecvWrite.

	StreamBufferHandle_t	rx_queue;			// OS queue holding data received from lwIP stack.
	StreamBufferHandle_t	tx_queue;			// OS queue holding data to be sent.

	uint8_t					*tx_double_buf;		// Buffer holding data not yet ACKed.
	uint32_t				tx_double_buf_index;
	uint32_t				tx_double_buf_size;

	volatile uint32_t		outstanding;		// A count of the number of bytes that have been transmitted but have not yet been ACKed.
	struct tcp_pcb			*pcb;				// A pointer to the telnet session PCB data structure.
	unsigned char			previous;			// The character most recently received via the telnet interface.  This is used to convert CR/LF sequences into a simple CR sequence.
} TelnetServer_t;


/* Declarations */

#ifdef __cplusplus
extern "C" {
#endif

TelnetStatus_t InitializeTelnetServer(StreamBufferHandle_t rx_queue, StreamBufferHandle_t tx_queue, uint8_t *tx_double_buf);
TelnetStatus_t StartTelnetServer();
void TelnetClose(void);
uint8_t TelnetIsConnected(void);
err_t TelnetPoll(void *arg, struct tcp_pcb *tpcb);
//void TelnetRecvBufferWrite(char character);
//char TelnetRead(void);
//void TelnetWrite(const char character);
//void TelnetWriteString(char* string);
void TelnetProcessWill(uint8_t option);
void TelnetProcessWont(uint8_t option);
void TelnetProcessDo(uint8_t option);
void TelnetProcessDont(uint8_t option);
void TelnetProcessCharacter(uint8_t character);
void TelnetWriteErrorMessage(char* message);
void TelnetWriteStatusMessage(char* message);
void TelnetWriteDebugMessage(char* message);
void TelnetWriteCommandDataMessage(char* message);


#ifdef __cplusplus
}
#endif

#endif /* __TELNET_SERVER_H__ */
