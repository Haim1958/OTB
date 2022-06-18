/**
 ******************************************************************************
 * @file    UserLib.h
 * @brief   User library main include file
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



#ifndef __LIB_H__
#define __LIB_H__

#include <stdint.h>
#include "stm32f7xx.h"
#include "cmsis_os.h"
#include "./errors.h"
#include "../io/inc/io.h"
#include "../irq/inc/irq.h"
#include "../BSP/inc/BSP.h"
#include "../fsm/inc/fsm.h"
#include "../com/inc/com.h"
#include "../Middlewares/inc/Middlewares.h"
#include "../algo/inc/algo.h"
#include "../printf/inc/printf.h"


#define uflgLASER_OFF_ON	0
#define uflgLASER_OFF_OFF	1


TaskHandle_t 	Handle_OTBB_task_RX;
TaskHandle_t 	Handle_TCP_task_CONN;
TaskHandle_t 	Handle_TCP_task_RX;
SemaphoreHandle_t xSemaphore_IP_Alloct;
SemaphoreHandle_t xSemaphore_IP_Alloct_Accept;											  
SemaphoreHandle_t xSemaphore_TCP_CONN;									   
SemaphoreHandle_t xSemaphore_OTBB_CONNECT_1;											
SemaphoreHandle_t xSemaphore_IP_Queue;									  
StaticMessageBuffer_t xOTBBMessageBuffer_1;

//MessageBufferHandle_t xOTBBMessageBuffer;

QueueHandle_t	PACKET_RECEIVE_FROM_OTBB;										   


SemaphoreHandle_t xSemaphore_OTBB_Queue;


StaticSemaphore_t xSemaphore_OTBB_Accept_RX_Buffer;
StaticSemaphore_t xSemaphore_OTBB_Receive_RX_Buffer;
SemaphoreHandle_t xSemaphore_OTBB_Accept_RX;
SemaphoreHandle_t xSemaphore_OTBB_Receive_RX;
SemaphoreHandle_t xSemaphore_OTBB_Queue_Laser_Off;

SemaphoreHandle_t xSemaphore_RF_READ_DAC_TEMP;


TaskHandle_t Gl_handle_rx;

int32_t OTBB1_Ethernet_glb;
int32_t OTBB2_Ethernet_glb;
int32_t OTBC_Ethernet_glb;

uint8_t ulaser_off_flg;

char 				*gl_tcp_remote_ip;
uint32_t			 gl_tcp_connect_attempts;
uint32_t			 gl_tcp_rx_timeout_ms;
uint32_t			 gl_tcp_tx_timeout_ms;

uint8_t gl_release_sem_receive;
uint8_t gl_task_give_notify;


OPTMOD_STRUCT_PTR OPTMOD_GLB;

RF_STRUCT_PTR     RF_STRCT_GLB;


OPTMOD_OPER_PARAMS_PTR oper_params;
OPTMOD_CALIB_PARAMS_PTR calib_params;

OPTMOD_CALIB_SAMPLE_PTR calib_sample;

OPTMOD_OPER_PARAMS_OLD_PTR oper_params_old;
OPTMOD_CALIB_SAMPLE_PTR DOT_MIDDLE;

OPTMOD_STRUCT_TEMP_CAILB_PTR 	TEMP_DATA;


StaticMessageBuffer_t xOTBBMessageBuffer_1;

//MessageBufferHandle_t xOTBBMessageBuffer;




void OTBB_COM_examine_task(void  * argument);

void *Gl_driver_bundle;


#endif // __LIB_H__
