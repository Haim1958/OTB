/**
 ******************************************************************************
 * @file    sercom.h
 * @brief   Serial Communication Protocol public definitions and declarations,
			related to the IO component itself.
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


#ifndef __SERCOM_H__
#define __SERCOM_H__

/* Includes */

#include "./sercom_protocol.h"


/* Definitions */

// Error definitions
#define IO_ERROR_SERCOM__OK							IO_ERROR__OK
#define IO_ERROR_SERCOM__OUT_OF_HEAP				IO_ERROR_SERCOM(0x01)
#define IO_ERROR_SERCOM__UNKNOWN_IOCTL				IO_ERROR_SERCOM(0x02)
#define IO_ERROR_SERCOM__PACKET_NOT_READY			IO_ERROR_SERCOM(0x03)
#define IO_ERROR_SERCOM__INDEX_OUT_RANGE			IO_ERROR_SERCOM(0x04)
#define IO_ERROR_SERCOM__INSTANCE_NOT_ALLOCATED		IO_ERROR_SERCOM(0x05)
#define IO_ERROR_SERCOM__TIMEOUT					IO_ERROR_SERCOM(0x06)

// IOCTL definitions
//#define IO_IOCTL_SERCOM__RUN_SM_RX					IO_IOCTL_SERCOM(1)




/* Typedefs */


typedef struct {
	uint32_t			max_num_instances;				// Maximum number of parsers allowed
} SERCOM_INSTALL_STRUCT, * SERCOM_INSTALL_STRUCT_PTR;


typedef struct {
	uint8_t				if_new;							// Flag indicating a new instance is requested to open
	uint32_t			index;							// Instance index in case if_new = 0
	IO_HANDLER_PTR		driver;         			    // Low layer driver

	uint32_t			tx_queue_size;         			// Maximum number of packets in TX queue
	uint32_t			rx_queue_size;         			// Maximum number of packets in RX queue

	uint32_t			tx_timeout_ms;					// Maximum TX timeout in milliseconds
	uint32_t			rx_timeout_ms;					// Maximum RX timeout in milliseconds
} SERCOM_INIT_STRUCT, * SERCOM_INIT_STRUCT_PTR;

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_sercom(SERCOM_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif


#endif	//__SERCOM_H__
