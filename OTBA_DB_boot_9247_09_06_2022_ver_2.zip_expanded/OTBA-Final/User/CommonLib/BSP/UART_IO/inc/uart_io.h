/**
 ******************************************************************************
 * @file    uart_io.h
 * @brief   UART IO public declarations and definitions
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


#ifndef __UART_IO_H__
#define __UART_IO_H__

/* Includes */


/* Definitions */

// Error definitions
#define IO_ERROR_UART_IO__OK						IO_ERROR__OK
#define IO_ERROR_UART_IO__OUT_OF_HEAP				IO_ERROR_UART_IO(0x01)
#define IO_ERROR_UART_IO__UNKNOWN_HAL_DRIVER		IO_ERROR_UART_IO(0x02)
#define IO_ERROR_UART_IO__UNKNOWN_IOCTL				IO_ERROR_UART_IO(0x03)
#define IO_ERROR_UART_IO__DATA_TOO_LONG				IO_ERROR_UART_IO(0x04)
#define IO_ERROR_UART_IO__TX_IN_PROCESS				IO_ERROR_UART_IO(0x05)
#define IO_ERROR_UART_IO__RX_QUEUE_EMPTY			IO_ERROR_UART_IO(0x06)
#define IO_ERROR_UART_IO__TX_QUEUE_FULL				IO_ERROR_UART_IO(0x07)


// IOCTL definitions
//#define IO_IOCTL_UART_IO__BAUD_115200_FLOW_NONE		IO_IOCTL_UART_IO(1)



/* Typedefs */

typedef struct {
	uint32_t temp;
} UART_IO_INSTALL_STRUCT, * UART_IO_INSTALL_STRUCT_PTR;


typedef struct {
	UART_HandleTypeDef*				HAL_UART;		/* ST HAL handler of UART */
	
	uint32_t						rx_buf_size;	// Read buffer max size
	uint32_t						tx_buf_size;	// Read buffer max size
} UART_IO_INIT_STRUCT, * UART_IO_INIT_STRUCT_PTR;

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_uart_io(UART_IO_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif


#endif	//__UART_IO_H__
