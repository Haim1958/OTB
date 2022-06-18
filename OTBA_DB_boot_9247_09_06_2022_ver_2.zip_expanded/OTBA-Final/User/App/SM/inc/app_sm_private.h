/**
 ******************************************************************************
 * @file    app_sm_private.h
 * @brief   Application Main State machine private declarations and definitions
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


#ifndef _APP_SM_PRIVATE_H_
#define _APP_SM_PRIVATE_H_

/* Includes */



/* Definitions */

#define OTS_APP_SM_NUMBER_OF_STATES		(5u)			// not including Error state
#define LASER_OFF 1
#define LASER_ON 0
enum logic_state{
		logic_state__INIT,
		logic_state__IDLE,
		logic_state__IBIT,
		logic_state__OPERATE,
		logic_state__LASEROFF,
		logic_state__INIT_DONE,
		logic_state__IDLE_DONE,
		logic_state__IBIT_DONE,
		logic_state__OPERATE_DONE,
		logic_state__LASEROFF_DONE,
		logic_state__ERROR=5,

		logic_state__INIT_IDLE,
		logic_state__IDLE_IBIT,
		logic_state__IBIT_IDLE,
		logic_state__IDLE_OPERATE,
		logic_state__OPERATE_IDLE,
		logic_state__IDLE_LASEROFF,
		logic_state__LASEROFF_IDLE,
		NO_Change};

/* Declarations */

// States
int32_t _app_sm__state_INIT__init(void* param);
int32_t _app_sm__state_INIT__operation(void* param);
int32_t _app_sm__state_IDLE__init(void* param);
int32_t _app_sm__state_IDLE__operation(void* param);
int32_t _app_sm__state_IBIT__init(void* param);
int32_t _app_sm__state_IBIT__operation(void* param);
int32_t _app_sm__state_OPERATE__init(void* param);
int32_t _app_sm__state_OPERATE__operation(void* param);
int32_t _app_sm__state_LASEROFF__init(void* param);
int32_t _app_sm__state_LASEROFF__operation(void* param);
int32_t _app_sm__state_ERROR__init(void* param);
int32_t _app_sm__state_ERROR__operation(void* param);

// Transitions
int32_t _app_sm__trans_INIT_IDLE__check(void* param);
int32_t _app_sm__trans_INIT_IDLE__exit(void* param);
int32_t _app_sm__trans_IDLE_IBIT__check(void* param);
int32_t _app_sm__trans_IDLE_IBIT__exit(void* param);
int32_t _app_sm__trans_IBIT_IDLE__check(void* param);
int32_t _app_sm__trans_IBIT_IDLE__exit(void* param);
int32_t _app_sm__trans_IDLE_OPERATE__check(void* param);
int32_t _app_sm__trans_IDLE_OPERATE__exit(void* param);
int32_t _app_sm__trans_OPERATE_IDLE__check(void* param);
int32_t _app_sm__trans_OPERATE_IDLE__exit(void* param);
int32_t _app_sm__trans_IDLE_LASEROFF__check(void* param);
int32_t _app_sm__trans_IDLE_LASEROFF__exit(void* param);
int32_t _app_sm__trans_LASEROFF_IDLE__check(void* param);
int32_t _app_sm__trans_LASEROFF_IDLE__exit(void* param);

void SM_logic_app(OTS_APPLICATION_BUNDLE_PTR OTS_APP);
#endif //_APP_SM_PRIVATE_H_


