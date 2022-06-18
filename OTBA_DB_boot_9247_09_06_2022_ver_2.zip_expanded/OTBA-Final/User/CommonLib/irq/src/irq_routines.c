/**
 ******************************************************************************
 * @file    irq.h
 * @brief   Interrupt Handler private routines
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

//extern IRQ_DESCRIPTOR IRQ_DESC_EXTI[IRQ_NUMBER_OF_DESCRIPTORS__EXTI];
//extern IRQ_DESCRIPTOR IRQ_DESC_SPI[IRQ_NUMBER_OF_DESCRIPTORS__SPI];
extern IRQ_DESCRIPTOR IRQ_DESC_UART_TX[IRQ_NUMBER_OF_DESCRIPTORS__UART];
extern IRQ_DESCRIPTOR IRQ_DESC_UART_RX[IRQ_NUMBER_OF_DESCRIPTORS__UART];
//extern IRQ_DESCRIPTOR IRQ_DESC_I2C_MEM_TX[IRQ_NUMBER_OF_DESCRIPTORS__I2C];
//extern IRQ_DESCRIPTOR IRQ_DESC_I2C_MEM_RX[IRQ_NUMBER_OF_DESCRIPTORS__I2C];
//extern IRQ_DESCRIPTOR IRQ_DESC_PVD[IRQ_NUMBER_OF_DESCRIPTORS__PVD];
//extern IRQ_DESCRIPTOR IRQ_DESC_ADC[IRQ_NUMBER_OF_DESCRIPTORS__ADC];

/* Functions definitions */

/*******************************************************************************
* Function Name: HAL_GPIO_EXTI_Callback
********************************************************************************
Description: Overloaded implementation of EXTI callback function
Inputs:
uint16_t		- The EXTI index
Outputs:
None.
*******************************************************************************/
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	uint8_t index = 0xFF;
//
//	switch (GPIO_Pin)
//	{
//	  case GPIO_PIN_0:
//		index = IRQ_INDEX__EXTI__0;
//		break;
//	  case GPIO_PIN_1:
//		index = IRQ_INDEX__EXTI__1;
//		break;
//	  case GPIO_PIN_2:
//		index = IRQ_INDEX__EXTI__2;
//		break;
//	  case GPIO_PIN_3:
//		index = IRQ_INDEX__EXTI__3;
//		break;
//	  case GPIO_PIN_4:
//		index = IRQ_INDEX__EXTI__4;
//		break;
//	  case GPIO_PIN_5:
//		index = IRQ_INDEX__EXTI__5;
//		break;
//	  case GPIO_PIN_6:
//		index = IRQ_INDEX__EXTI__6;
//		break;
//	  case GPIO_PIN_7:
//		index = IRQ_INDEX__EXTI__7;
//		break;
//	  case GPIO_PIN_8:
//		index = IRQ_INDEX__EXTI__8;
//		break;
//	  case GPIO_PIN_9:
//		index = IRQ_INDEX__EXTI__9;
//		break;
//	  case GPIO_PIN_10:
//		index = IRQ_INDEX__EXTI__10;
//		break;
//	  case GPIO_PIN_11:
//		index = IRQ_INDEX__EXTI__11;
//		break;
//	  case GPIO_PIN_12:
//		index = IRQ_INDEX__EXTI__12;
//		break;
//	  case GPIO_PIN_13:
//		index = IRQ_INDEX__EXTI__13;
//		break;
//	  case GPIO_PIN_14:
//		index = IRQ_INDEX__EXTI__14;
//		break;
//	  case GPIO_PIN_15:
//		index = IRQ_INDEX__EXTI__15;
//		break;
//	  default:
//		return;
//	}
//
//	/* Execute the callback */
//	if (IRQ_DESC_EXTI[index].callback)
//	{
//		IRQ_DESC_EXTI[index].callback(IRQ_DESC_EXTI[index].param);
//	}
//}



/*******************************************************************************
* Function Name: HAL_SPI_TxRxCpltCallback
********************************************************************************
Description: Overloaded implementation of SPI interrupt callback function
Inputs:
SPI_HandleTypeDef		- The SPI handler
Outputs:
None.
*******************************************************************************/
//void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
//{
//	if (hspi->Instance == SPI1 && IRQ_DESC_SPI[IRQ_INDEX__SPI__SPI1].callback != NULL)
//	{
//		IRQ_DESC_SPI[IRQ_INDEX__SPI__SPI1].callback(IRQ_DESC_SPI[IRQ_INDEX__SPI__SPI1].param);
//	}
//	else if (hspi->Instance == SPI2 && IRQ_DESC_SPI[IRQ_INDEX__SPI__SPI2].callback != NULL)
//	{
//		IRQ_DESC_SPI[IRQ_INDEX__SPI__SPI2].callback(IRQ_DESC_SPI[IRQ_INDEX__SPI__SPI2].param);
//	}
//	else if (hspi->Instance == SPI3 && IRQ_DESC_SPI[IRQ_INDEX__SPI__SPI3].callback != NULL)
//	{
//		IRQ_DESC_SPI[IRQ_INDEX__SPI__SPI3].callback(IRQ_DESC_SPI[IRQ_INDEX__SPI__SPI3].param);
//	}
//}



/*******************************************************************************
* Function Name: HAL_UART_RxRxCpltCallback
********************************************************************************
Description: Overloaded implementation of UART interrupt callback function
Inputs:
SPI_HandleTypeDef		- The UART handler
Outputs:
None.
*******************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1 && IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART1].callback != NULL)
	{
		IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART1].callback(IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART1].param);
	}
	else if (huart->Instance == USART2 && IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART2].callback != NULL)
	{
		IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART2].callback(IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART2].param);
	}
	else if (huart->Instance == USART3 && IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART3].callback != NULL)
	{
		IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART3].callback(IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART3].param);
	}
	else if (huart->Instance == UART4 && IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART4].callback != NULL)
	{
		IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART4].callback(IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART4].param);
	}
	else if (huart->Instance == UART5 && IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART5].callback != NULL)
	{
		IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART5].callback(IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART5].param);
	}
	else if (huart->Instance == USART6 && IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART6].callback != NULL)
	{
		IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART6].callback(IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART6].param);
	}
	else if (huart->Instance == UART7 && IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART7].callback != NULL)
	{
		IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART7].callback(IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART7].param);
	}
	else if (huart->Instance == UART8 && IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART8].callback != NULL)
	{
		IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART8].callback(IRQ_DESC_UART_RX[IRQ_INDEX__UART__UART8].param);
	}
}

/*******************************************************************************
* Function Name: HAL_UART_TxCpltCallback
********************************************************************************
Description: Overloaded implementation of UART interrupt callback function
Inputs:
UART_HandleTypeDef		- The UART handler
Outputs:
None.
*******************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1 && IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART1].callback != NULL)
	{
		IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART1].callback(IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART1].param);
	}
	else if (huart->Instance == USART2 && IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART2].callback != NULL)
	{
		IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART2].callback(IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART2].param);
	}
	else if (huart->Instance == USART3 && IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART3].callback != NULL)
	{
		IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART3].callback(IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART3].param);
	}
	else if (huart->Instance == UART4 && IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART4].callback != NULL)
	{
		IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART4].callback(IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART4].param);
	}
	else if (huart->Instance == UART5 && IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART5].callback != NULL)
	{
		IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART5].callback(IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART5].param);
	}
	else if (huart->Instance == USART6 && IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART6].callback != NULL)
	{
		IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART6].callback(IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART6].param);
	}
	else if (huart->Instance == UART7 && IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART7].callback != NULL)
	{
		IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART7].callback(IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART7].param);
	}
	else if (huart->Instance == UART8 && IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART8].callback != NULL)
	{
		IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART8].callback(IRQ_DESC_UART_TX[IRQ_INDEX__UART__UART8].param);
	}
}


/*******************************************************************************
* Function Name: HAL_I2C_MemTxCpltCallback
********************************************************************************
Description: Overloaded implementation of I2C interrupt callback function
Inputs:
I2C_HandleTypeDef		- The I2C handler
Outputs:
None.
*******************************************************************************/
//void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//	if (hi2c->Instance == I2C1 && IRQ_DESC_I2C_MEM_TX[IRQ_INDEX__I2C__I2C1].callback != NULL)
//	{
//		IRQ_DESC_I2C_MEM_TX[IRQ_INDEX__I2C__I2C1].callback(IRQ_DESC_I2C_MEM_TX[IRQ_INDEX__I2C__I2C1].param);
//	}
//	else if (hi2c->Instance == I2C2 && IRQ_DESC_I2C_MEM_TX[IRQ_INDEX__I2C__I2C2].callback != NULL)
//	{
//		IRQ_DESC_I2C_MEM_TX[IRQ_INDEX__I2C__I2C2].callback(IRQ_DESC_I2C_MEM_TX[IRQ_INDEX__I2C__I2C2].param);
//	}
//	else if (hi2c->Instance == I2C3 && IRQ_DESC_I2C_MEM_TX[IRQ_INDEX__I2C__I2C3].callback != NULL)
//	{
//		IRQ_DESC_I2C_MEM_TX[IRQ_INDEX__I2C__I2C3].callback(IRQ_DESC_I2C_MEM_TX[IRQ_INDEX__I2C__I2C3].param);
//	}
//}


/*******************************************************************************
* Function Name: HAL_I2C_MemRxCpltCallback
********************************************************************************
Description: Overloaded implementation of I2C interrupt callback function
Inputs:
I2C_HandleTypeDef		- The I2C handler
Outputs:
None.
*******************************************************************************/
//void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//	if (hi2c->Instance == I2C1 && IRQ_DESC_I2C_MEM_RX[IRQ_INDEX__I2C__I2C1].callback != NULL)
//	{
//		IRQ_DESC_I2C_MEM_RX[IRQ_INDEX__I2C__I2C1].callback(IRQ_DESC_I2C_MEM_RX[IRQ_INDEX__I2C__I2C1].param);
//	}
//	else if (hi2c->Instance == I2C2 && IRQ_DESC_I2C_MEM_RX[IRQ_INDEX__I2C__I2C2].callback != NULL)
//	{
//		IRQ_DESC_I2C_MEM_RX[IRQ_INDEX__I2C__I2C2].callback(IRQ_DESC_I2C_MEM_RX[IRQ_INDEX__I2C__I2C2].param);
//	}
//	else if (hi2c->Instance == I2C3 && IRQ_DESC_I2C_MEM_RX[IRQ_INDEX__I2C__I2C3].callback != NULL)
//	{
//		IRQ_DESC_I2C_MEM_RX[IRQ_INDEX__I2C__I2C3].callback(IRQ_DESC_I2C_MEM_RX[IRQ_INDEX__I2C__I2C3].param);
//	}
//}



/*******************************************************************************
* Function Name: HAL_PWR_PVDCallback
********************************************************************************
Description: Overloaded implementation of I2C interrupt callback function
Inputs:
I2C_HandleTypeDef		- The I2C handler
Outputs:
None.
*******************************************************************************/
//void HAL_PWR_PVDCallback(void)
//{
//	if (IRQ_DESC_PVD[0].callback != NULL)
//	{
//		IRQ_DESC_PVD[0].callback(IRQ_DESC_PVD[0].param);
//	}
//}


/*******************************************************************************
* Function Name: HAL_PWR_PVDCallback
********************************************************************************
Description: Overloaded implementation of I2C interrupt callback function
Inputs:
I2C_HandleTypeDef		- The I2C handler
Outputs:
None.
*******************************************************************************/
//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
//{
//	if (hadc->Instance == ADC1 && IRQ_DESC_ADC[IRQ_INDEX__ADC__ADC1].callback != NULL)
//	{
//		IRQ_DESC_ADC[IRQ_INDEX__ADC__ADC1].callback(IRQ_DESC_ADC[IRQ_INDEX__ADC__ADC1].param);
//	}
//	else if (hadc->Instance == ADC2 && IRQ_DESC_ADC[IRQ_INDEX__ADC__ADC2].callback != NULL)
//	{
//		IRQ_DESC_ADC[IRQ_INDEX__ADC__ADC2].callback(IRQ_DESC_ADC[IRQ_INDEX__ADC__ADC2].param);
//	}
//	else if (hadc->Instance == ADC3 && IRQ_DESC_ADC[IRQ_INDEX__ADC__ADC3].callback != NULL)
//	{
//		IRQ_DESC_ADC[IRQ_INDEX__ADC__ADC3].callback(IRQ_DESC_ADC[IRQ_INDEX__ADC__ADC3].param);
//	}
//}
