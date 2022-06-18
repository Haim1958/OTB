/**
 ******************************************************************************
 * @file    app_sm.h
 * @brief   Application Main State machine public declarations and definitions
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


#ifndef _APP_SM_H_
#define _APP_SM_H_



/* Includes */

#include "App_Inc.h"

/* Definitions */
#define OTBB1_Ethernet HAL_GPIO_ReadPin(P9_1__LOS_GPIO_Port, P9_1__LOS_Pin)
#define OTBB2_Ethernet HAL_GPIO_ReadPin(P9_2__LOS_GPIO_Port, P9_2__LOS_Pin)
#define OTBC_Ethernet HAL_GPIO_ReadPin(P9_0__LOS_GPIO_Port, P9_0__LOS_Pin)


typedef struct {
	FSM_OBJECT_STRUCT_PTR		fsm;

	FSM_OBJECT_STRUCT_PTR		state__INIT;
	FSM_OBJECT_STRUCT_PTR		state__IDLE;
	FSM_OBJECT_STRUCT_PTR		state__IBIT;
	FSM_OBJECT_STRUCT_PTR		state__OPERATE;
	FSM_OBJECT_STRUCT_PTR		state__LASEROFF;
	FSM_OBJECT_STRUCT_PTR		state__ERROR;

	FSM_OBJECT_STRUCT_PTR		trans__INIT_IDLE;
	FSM_OBJECT_STRUCT_PTR		trans__IDLE_IBIT;
	FSM_OBJECT_STRUCT_PTR		trans__IBIT_IDLE;
	FSM_OBJECT_STRUCT_PTR		trans__IDLE_OPERATE;
	FSM_OBJECT_STRUCT_PTR		trans__OPERATE_IDLE;
	FSM_OBJECT_STRUCT_PTR		trans__IDLE_LASEROFF;
	FSM_OBJECT_STRUCT_PTR		trans__LASEROFF_IDLE;
} OTS_APP_SM_BUNDLE, * OTS_APP_SM_BUNDLE_PTR;


typedef enum {
	OTB_STATE__IDLE,
	OTB_STATE__OPERATE,
	OTB_STATE__LASER_OFF,
	OTB_STATE__IBIT,
	OTB_STATE__INIT, // Special state - will never appear in the OTB_STATUS messages, as there's no communication at that state.

	OTB_STATE_COUNT
} OTB_STATE;

//OTB_STATE current_state,previous_state ;
/* Declarations */

int32_t app_sm_init(OTS_APPLICATION_BUNDLE_PTR OTS_APP);
OTB_STATE get_active_state(OTS_APP_SM_BUNDLE_PTR pFSM);


#endif //_APP_SM_H_


