/**
 ******************************************************************************
 * @file    irq.h
 * @brief   Interrupt Handler private declarations and definitions
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
	 
	 
#ifndef __IRQ_PRIVATE_H__
#define __IRQ_PRIVATE_H__

/* Includes */

#include "./irq.h"

/* Definitions */

//#define IRQ_NUMBER_OF_DESCRIPTORS__EXTI		(16u)
//#define IRQ_NUMBER_OF_DESCRIPTORS__SPI		(3u)
#define IRQ_NUMBER_OF_DESCRIPTORS__UART		(8u)
//#define IRQ_NUMBER_OF_DESCRIPTORS__I2C		(3u)
//#define IRQ_NUMBER_OF_DESCRIPTORS__PVD		(1u)
//#define IRQ_NUMBER_OF_DESCRIPTORS__ADC		(3u)

/* Typedefs */

typedef struct {
	IRQ_CALLBACK	callback;				// The callback of the interrupt
	void*			param;					// The parameter to be sent to the callback
} IRQ_DESCRIPTOR, * IRQ_DESCRIPTOR_PTR;

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

	 
#ifdef __cplusplus
}
#endif


#endif //__IRQ_PRIVATE_H__
