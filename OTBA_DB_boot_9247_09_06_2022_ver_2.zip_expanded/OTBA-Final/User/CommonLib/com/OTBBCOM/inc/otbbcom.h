/**
 ******************************************************************************
 * @file    otbbcom.h
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



#ifndef __OTBBCOM_H__
#define __OTBBCOM_H__


/* Includes */

#include "cmsis_os.h"
#include "lwip.h"

#include "../inc/otbbcom_icd.h"

/* Definitions */
#define IO_ERROR_OTBBCOM__OK										ERROR_OK
#define IO_ERROR_OTBBCOM__NULL										IO_ERROR_OTBBCOM(0x01)
#define IO_ERROR_OTBBCOM__UNDEFINED_TYPE							IO_ERROR_OTBBCOM(0x02)
#define IO_ERROR_OTBBCOM__OUT_OF_HEAP								IO_ERROR_OTBBCOM(0x03)
#define IO_ERROR_OTBBCOM__UNKNOWN_IOCTL								IO_ERROR_OTBBCOM(0x04)
#define IO_ERROR_OTBBCOM__FAILED_CREATING_SOCKET					IO_ERROR_OTBBCOM(0x05)
#define IO_ERROR_OTBBCOM__FAILED_CONNECTING_SOCKET					IO_ERROR_OTBBCOM(0x06)
#define IO_ERROR_OTBBCOM__NO_AVAILABLE_MESSAGES						IO_ERROR_OTBBCOM(0x07)
#define IO_ERROR_OTBBCOM__NOT_CONNECTED								IO_ERROR_OTBBCOM(0x08)
#define IO_ERROR_OTBBCOM__FAILED_WRITING_TO_SOCKET					IO_ERROR_OTBBCOM(0x09)
#define IO_ERROR_OTBBCOM__BAD_REQUEST								IO_ERROR_OTBBCOM(0x0A)


#define IO_IOCTL_OTBBCOM__CONNECT									IO_IOCTL_OTBBCOM(1)
#define IO_IOCTL_OTBBCOM__IS_CONNECTED								IO_IOCTL_OTBBCOM(2)
#define IO_IOCTL_OTBBCOM_IS_RE_CONNECTED							IO_IOCTL_OTBBCOM(3)
#define IO_IOCTL_OTBBCOM_IS_RE_DELETED                              IO_IOCTL_OTBBCOM(4)

/* Typedefs */

typedef struct {
	uint32_t						temp;
} OTBBCOM_INSTALL_STRUCT, * OTBBCOM_INSTALL_STRUCT_PTR;


typedef enum {
	OTBBCOM_EDGE_TYPE__OTBA,
	OTBBCOM_EDGE_TYPE__OTBB
} OTBBCOM_EDGE_TYPE;


typedef struct {
	OTBBCOM_EDGE_TYPE				type;

	char 							*remote_ip;

	uint32_t						tcp_tx_timeout_ms;
	uint32_t						tcp_rx_timeout_ms;
	uint32_t						tcp_connect_attempts;
} OTBBCOM_INIT_STRUCT, * OTBBCOM_INIT_STRUCT_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_otbbcom(OTBBCOM_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif // __OTBBCOM_H__
