/**
 ******************************************************************************
 * @file    telnet.h
 * @brief   Telnet public declarations and definitions
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



#ifndef __TELNET_H__
#define __TELNET_H__


/* Includes */

#include "cmsis_os.h"

/* Definitions */
#define IO_ERROR_TELNET__OK											ERROR_OK
#define IO_ERROR_TELNET__NULL										IO_ERROR_TELNET(0x01)
#define IO_ERROR_TELNET__UNDEFINED_TYPE								IO_ERROR_TELNET(0x02)
#define IO_ERROR_TELNET__OUT_OF_HEAP								IO_ERROR_TELNET(0x03)
#define IO_ERROR_TELNET__UNKNOWN_IOCTL								IO_ERROR_TELNET(0x04)
#define IO_ERROR_TELNET__INITIALIZATION_ERROR						IO_ERROR_TELNET(0x05)
#define IO_ERROR_TELNET__NOT_CONNECTED								IO_ERROR_TELNET(0x06)
#define IO_ERROR_TELNET__FAILED_TO_READ_REQUESTED_AMOUNT			IO_ERROR_TELNET(0x07)


#define IO_IOCTL_TELNET__START_SERVER								IO_IOCTL_TELNET(1)
#define IO_IOCTL_TELNET__IS_CONNECTED								IO_IOCTL_TELNET(2)

/* Typedefs */

typedef struct {
	uint32_t						temp;
} TELNET_INSTALL_STRUCT, * TELNET_INSTALL_STRUCT_PTR;


typedef struct {
	uint16_t	local_port;

	uint32_t	rx_queue_size;
	uint32_t	tx_queue_size;
} TELNET_INIT_STRUCT, * TELNET_INIT_STRUCT_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_telnet(TELNET_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif // __TELNET_H__
