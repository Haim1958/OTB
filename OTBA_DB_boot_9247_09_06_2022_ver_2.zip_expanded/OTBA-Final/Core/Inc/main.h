/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc3;

extern I2C_HandleTypeDef hi2c1;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
uint64_t HIGH_RES_TIMER();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define P9_0_TXDIS_Pin GPIO_PIN_3
#define P9_0_TXDIS_GPIO_Port GPIOE
#define LASER_SHUT_DOWN_Pin GPIO_PIN_0
#define LASER_SHUT_DOWN_GPIO_Port GPIOE
#define SPI1_NSS_Pin GPIO_PIN_15
#define SPI1_NSS_GPIO_Port GPIOA
#define P9_0__TXFLT_Pin GPIO_PIN_4
#define P9_0__TXFLT_GPIO_Port GPIOE
#define P9_0__LOS_Pin GPIO_PIN_5
#define P9_0__LOS_GPIO_Port GPIOE
#define P9_1_TXDIS_Pin GPIO_PIN_6
#define P9_1_TXDIS_GPIO_Port GPIOE
#define DAC_LDACn_Pin GPIO_PIN_1
#define DAC_LDACn_GPIO_Port GPIOD
#define LED_RED2_Pin GPIO_PIN_9
#define LED_RED2_GPIO_Port GPIOI
#define LED_GREEN1_Pin GPIO_PIN_4
#define LED_GREEN1_GPIO_Port GPIOI
#define PG_1V1_Pin GPIO_PIN_15
#define PG_1V1_GPIO_Port GPIOH
#define LED_RED1_Pin GPIO_PIN_1
#define LED_RED1_GPIO_Port GPIOI
#define WDI_Pin GPIO_PIN_0
#define WDI_GPIO_Port GPIOF
#define LED_GREEN2_Pin GPIO_PIN_11
#define LED_GREEN2_GPIO_Port GPIOI
#define PG_1V8_Pin GPIO_PIN_13
#define PG_1V8_GPIO_Port GPIOH
#define PG_1V5_Pin GPIO_PIN_14
#define PG_1V5_GPIO_Port GPIOH
#define DAC_CSn_Pin GPIO_PIN_0
#define DAC_CSn_GPIO_Port GPIOI
#define EN_3_3_Pin GPIO_PIN_3
#define EN_3_3_GPIO_Port GPIOF
#define EN_5_Pin GPIO_PIN_4
#define EN_5_GPIO_Port GPIOF
#define PG_2V5_Pin GPIO_PIN_12
#define PG_2V5_GPIO_Port GPIOH
#define M3V3_PG_Pin GPIO_PIN_11
#define M3V3_PG_GPIO_Port GPIOH
#define P3V3_PG_Pin GPIO_PIN_10
#define P3V3_PG_GPIO_Port GPIOH
#define OPT_ADC_RST_Pin GPIO_PIN_2
#define OPT_ADC_RST_GPIO_Port GPIOB
#define P5V0_PG_Pin GPIO_PIN_8
#define P5V0_PG_GPIO_Port GPIOH
#define M5V0_PG_Pin GPIO_PIN_9
#define M5V0_PG_GPIO_Port GPIOH
#define OPT_DAC_SYNC_Pin GPIO_PIN_4
#define OPT_DAC_SYNC_GPIO_Port GPIOA
#define DAC_RSTn_Pin GPIO_PIN_11
#define DAC_RSTn_GPIO_Port GPIOD
#define P9_1__LOS_Pin GPIO_PIN_8
#define P9_1__LOS_GPIO_Port GPIOE
#define P9_2__TXFLT_Pin GPIO_PIN_11
#define P9_2__TXFLT_GPIO_Port GPIOE
#define LAser_RESET_Pin GPIO_PIN_12
#define LAser_RESET_GPIO_Port GPIOB
#define P9_1__TXFLT_Pin GPIO_PIN_7
#define P9_1__TXFLT_GPIO_Port GPIOE
#define P9_2_TXDIS_Pin GPIO_PIN_10
#define P9_2_TXDIS_GPIO_Port GPIOE
#define P9_2__LOS_Pin GPIO_PIN_12
#define P9_2__LOS_GPIO_Port GPIOE
#define SW_INTn_Pin GPIO_PIN_10
#define SW_INTn_GPIO_Port GPIOB
#define SPI3__NSS_Pin GPIO_PIN_11
#define SPI3__NSS_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
