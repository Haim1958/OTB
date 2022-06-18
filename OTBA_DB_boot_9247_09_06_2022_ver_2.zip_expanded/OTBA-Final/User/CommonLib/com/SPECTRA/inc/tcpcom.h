/**
 ******************************************************************************
 * @file    tcpcom.h
 * @brief   TCP Communication public declarations and definitions
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



#ifndef __TCPCOM_H__
#define __TCPCOM_H__


/* Includes */

#include "cmsis_os.h"
#include "lwip.h"

#include "../../SPECTRA/inc/tcpcom_icd.h"

/* Definitions */
#define IO_ERROR_TCPCOM__OK											ERROR_OK
#define IO_ERROR_TCPCOM__NULL										IO_ERROR_TCPCOM(0x01)
#define IO_ERROR_TCPCOM__UNDEFINED_TYPE								IO_ERROR_TCPCOM(0x02)
#define IO_ERROR_TCPCOM__OUT_OF_HEAP								IO_ERROR_TCPCOM(0x03)
#define IO_ERROR_TCPCOM__UNKNOWN_IOCTL								IO_ERROR_TCPCOM(0x04)
#define IO_ERROR_TCPCOM__FAILED_CREATING_SOCKET						IO_ERROR_TCPCOM(0x05)
#define IO_ERROR_TCPCOM__FAILED_CONNECTING_SOCKET					IO_ERROR_TCPCOM(0x06)
#define IO_ERROR_TCPCOM__NO_AVAILABLE_MESSAGES						IO_ERROR_TCPCOM(0x07)
#define IO_ERROR_TCPCOM__NOT_CONNECTED								IO_ERROR_TCPCOM(0x08)
#define IO_ERROR_TCPCOM__FAILED_WRITING_TO_SOCKET					IO_ERROR_TCPCOM(0x09)
#define IO_ERROR_TCPCOM__BAD_REQUEST								IO_ERROR_TCPCOM(0x0A)


#define IO_IOCTL_TCPCOM__CONNECT_AC									IO_IOCTL_TCPCOM(1)
#define IO_IOCTL_TCPCOM__IS_CONNECTED_AC							IO_IOCTL_TCPCOM(2)
#define IO_IOCTL_TCPCOM__PEEK_INPUT_MESSAGE_DESCRITE_SIGNAL_TEST	IO_IOCTL_TCPCOM(3)
#define IO_IOCTL_TCPCOM__PEEK_INPUT_MESSAGE_PHASIS_CONTROL			IO_IOCTL_TCPCOM(4)


/* Typedefs */

typedef struct {
	uint32_t						temp;
} TCPCOM_INSTALL_STRUCT, * TCPCOM_INSTALL_STRUCT_PTR;


typedef struct {
	ip4_addr_t local_ip;
	ip4_addr_t local_netmask;
	ip4_addr_t local_gw;
	uint16_t local_port;

	char *remote_ip;
	uint16_t remote_port;

	uint32_t tcp_tx_timeout_ms;
	uint32_t tcp_rx_timeout_ms;
	uint32_t tcp_connect_attempts;
} TCPCOM_INIT_STRUCT, * TCPCOM_INIT_STRUCT_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_tcpcom(TCPCOM_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif // __TCPCOM_H__
