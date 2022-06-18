/**
 ******************************************************************************
 * @file    irq.h
 * @brief   Interrupt Handler public definitions and declarations
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
	 
	 
#ifndef __IRQ_H__
#define __IRQ_H__

/* Includes */

#include "./irq_errors.h"

/* Definitions */

typedef enum {
//	IRQ_TYPE__EXTI,
//	IRQ_TYPE__SPI,
	IRQ_TYPE__UART_TX,
	IRQ_TYPE__UART_RX,
//	IRQ_TYPE__I2C_MEM_TX,
//	IRQ_TYPE__I2C_MEM_RX,
//	IRQ_TYPE__PVD,
//	IRQ_TYPE__ADC,
	
	IRQ_TYPE_COUNT
} IRQ_TYPE;
		
//typedef enum {
//	IRQ_INDEX__EXTI__0,
//	IRQ_INDEX__EXTI__1,
//	IRQ_INDEX__EXTI__2,
//	IRQ_INDEX__EXTI__3,
//	IRQ_INDEX__EXTI__4,
//	IRQ_INDEX__EXTI__5,
//	IRQ_INDEX__EXTI__6,
//	IRQ_INDEX__EXTI__7,
//	IRQ_INDEX__EXTI__8,
//	IRQ_INDEX__EXTI__9,
//	IRQ_INDEX__EXTI__10,
//	IRQ_INDEX__EXTI__11,
//	IRQ_INDEX__EXTI__12,
//	IRQ_INDEX__EXTI__13,
//	IRQ_INDEX__EXTI__14,
//	IRQ_INDEX__EXTI__15
//} IRQ_INDEX__EXTI;

//typedef enum {
//	IRQ_INDEX__SPI__SPI1,
//	IRQ_INDEX__SPI__SPI2,
//	IRQ_INDEX__SPI__SPI3
//} IRQ_INDEX__SPI;

typedef enum {
	IRQ_INDEX__UART__UART1,
	IRQ_INDEX__UART__UART2,
	IRQ_INDEX__UART__UART3,
	IRQ_INDEX__UART__UART4,
	IRQ_INDEX__UART__UART5,
	IRQ_INDEX__UART__UART6,
	IRQ_INDEX__UART__UART7,
	IRQ_INDEX__UART__UART8,
	
	IRQ_INDEX__UART_COUNT
} IRQ_INDEX__UART;

//typedef enum {
//	IRQ_INDEX__I2C__I2C1,
//	IRQ_INDEX__I2C__I2C2,
//	IRQ_INDEX__I2C__I2C3
//} IRQ_INDEX__I2C;

//typedef enum {
//	IRQ_INDEX__ADC__ADC1,
//	IRQ_INDEX__ADC__ADC2,
//	IRQ_INDEX__ADC__ADC3
//} IRQ_INDEX__ADC;


typedef void (*IRQ_CALLBACK)(void*);

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t irq_register(IRQ_TYPE type, uint8_t index, IRQ_CALLBACK, void*);
	 
#ifdef __cplusplus
}
#endif


#endif //__IRQ_H__
