/**
 ******************************************************************************
 * @file    telnet_private.h
 * @brief   Telnet private declarations and definitions
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


#ifndef __TELNET_PRIVATE_H__
#define __TELNET_PRIVATE_H__

/* Includes */

#include "UserLib.h"
#include "stream_buffer.h"

/* Definitions */


/* Typedefs */





typedef struct telnet_bundle {
	StreamBufferHandle_t		rx_queue;			// Received bytes stream buffer
	StreamBufferHandle_t		tx_queue;			// Transmit bytes stream buffer
	uint8_t						*tx_double_buf;		// Transmit double buffer
	SemaphoreHandle_t			tx_mutex;			// TX queue (StreamBuffer) protection mutex
} TELNET_DRIVER_BUNDLE, * TELNET_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif //__TELNET_PRIVATE_H__
