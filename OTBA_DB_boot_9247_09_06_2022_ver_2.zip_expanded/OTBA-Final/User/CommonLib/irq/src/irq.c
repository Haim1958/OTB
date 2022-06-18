/**
 ******************************************************************************
 * @file    irq.c
 * @brief   Interrupt Handler public functions implementations
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



/* Includes */
#include "UserLib.h"
#include "../inc/irq_private.h"

/* Global variables */

//IRQ_DESCRIPTOR IRQ_DESC_EXTI[IRQ_NUMBER_OF_DESCRIPTORS__EXTI];
//IRQ_DESCRIPTOR IRQ_DESC_SPI[IRQ_NUMBER_OF_DESCRIPTORS__SPI];
IRQ_DESCRIPTOR IRQ_DESC_UART_TX[IRQ_NUMBER_OF_DESCRIPTORS__UART];
IRQ_DESCRIPTOR IRQ_DESC_UART_RX[IRQ_NUMBER_OF_DESCRIPTORS__UART];
//IRQ_DESCRIPTOR IRQ_DESC_I2C_MEM_TX[IRQ_NUMBER_OF_DESCRIPTORS__I2C];
//IRQ_DESCRIPTOR IRQ_DESC_I2C_MEM_RX[IRQ_NUMBER_OF_DESCRIPTORS__I2C];
//IRQ_DESCRIPTOR IRQ_DESC_PVD[IRQ_NUMBER_OF_DESCRIPTORS__PVD];
//IRQ_DESCRIPTOR IRQ_DESC_ADC[IRQ_NUMBER_OF_DESCRIPTORS__ADC];

IRQ_DESCRIPTOR* IRQ_DESC_LUT[IRQ_TYPE_COUNT] = {
//	IRQ_DESC_EXTI,
//	IRQ_DESC_SPI,
	IRQ_DESC_UART_TX,
	IRQ_DESC_UART_RX,
//	IRQ_DESC_I2C_MEM_TX,
//	IRQ_DESC_I2C_MEM_RX,
//	IRQ_DESC_PVD,
//	IRQ_DESC_ADC,
};

uint8_t IRQ_DESC_SIZE_LUT[IRQ_TYPE_COUNT] = {
//	IRQ_NUMBER_OF_DESCRIPTORS__EXTI,
//	IRQ_NUMBER_OF_DESCRIPTORS__SPI,
	IRQ_NUMBER_OF_DESCRIPTORS__UART,
	IRQ_NUMBER_OF_DESCRIPTORS__UART,
//	IRQ_NUMBER_OF_DESCRIPTORS__I2C,
//	IRQ_NUMBER_OF_DESCRIPTORS__I2C,
//	IRQ_NUMBER_OF_DESCRIPTORS__PVD,
//	IRQ_NUMBER_OF_DESCRIPTORS__ADC,
};

/* Functions definitions */

/*******************************************************************************
* Function Name: irq_register
********************************************************************************
Description: Registers to the required interrupt callback
Inputs:
IRQ_TYPE		- The type of the interrupt
uint8_t			- The index of the interrupt
IRQ_CALLBACK	- The callback
void*			- Parameter to the callback
Outputs:
IRQ_ERROR__OK - installed handler
IRQ_ERROR__UNKNOWN_TYPE - the requested callback is unknown
*******************************************************************************/
int32_t irq_register(IRQ_TYPE type,
					 uint8_t index,
					 IRQ_CALLBACK callback,
					 void* param)
{
	if (type >= IRQ_TYPE_COUNT)
	{
		return IRQ_ERROR__UNKNOWN_TYPE;
	}
	
	if (index >= IRQ_DESC_SIZE_LUT[type])
	{
		return IRQ_ERROR__INDEX_OUT_OF_RANGE;
	}
	
	if ((IRQ_DESC_LUT[type])[index].callback != NULL)
	{
		return IRQ_ERROR__ALREADY_REGISTERED;
	}
	
	if (NULL == callback)
	{
		return IRQ_ERROR__INVALID_CALLBACK;
	}
	
	(IRQ_DESC_LUT[type])[index].callback = callback;
	(IRQ_DESC_LUT[type])[index].param	 = param;
	
	return IRQ_ERROR__OK;
}
