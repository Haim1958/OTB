/**
 ******************************************************************************
 * @file    app_sm_init.c
 * @brief   Application Main State machine initialization method
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


#include "UserLib.h"
#include "app.h"
#include "../inc/app_sm.h"
#include "../inc/app_sm_private.h"


/*! \brief Constructs packet of type FIRMWARE_VERSION.
 *
 ###  Detailed description:
 *
 *  @param OTS_APP - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t app_sm_init(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	FSM_PARAMS_STRUCT_t fsm_params;
	FSM_STATE_PARAMS_STRUCT_t state_params;
	FSM_TRANSITION_PARAMS_STRUCT_t transition_params;
	
	/* 
	**	Initialize the State Machine
	*/
	fsm_params.max_num_states = OTS_APP_SM_NUMBER_OF_STATES;
	fsm_params.log_enabled = 1u;
	OTS_APP->FSM.fsm = fsm_sm_create(&fsm_params);
	if (NULL == OTS_APP->FSM.fsm)
	{
		return FSM_ERROR_EMPTY;
	}
	
	/*
	**	Initialize States
	*/
	
	/* Initialize state INIT */
	state_params.init_delegate.handler = _app_sm__state_INIT__init;
	state_params.init_delegate.param = OTS_APP;
	state_params.oper_delegate.handler = _app_sm__state_INIT__operation;
	state_params.oper_delegate.param = OTS_APP;
	OTS_APP->FSM.state__INIT = fsm_state_create(OTS_APP->FSM.fsm, "STATE_INIT", &state_params);
	if (NULL == OTS_APP->FSM.state__INIT)
	{
		return FSM_ERROR_EMPTY;
	}

	/* Initialize state IDLE */
	state_params.init_delegate.handler = _app_sm__state_IDLE__init;
	state_params.init_delegate.param = OTS_APP;
	state_params.oper_delegate.handler = _app_sm__state_IDLE__operation;
	state_params.oper_delegate.param = OTS_APP;
	OTS_APP->FSM.state__IDLE = fsm_state_create(OTS_APP->FSM.fsm, "STATE_IDLE", &state_params);
	if (NULL == OTS_APP->FSM.state__IDLE)
	{
		return FSM_ERROR_EMPTY;
	}

	/* Initialize state IBIT */
	state_params.init_delegate.handler = _app_sm__state_IBIT__init;
	state_params.init_delegate.param = OTS_APP;
	state_params.oper_delegate.handler = _app_sm__state_IBIT__operation;
	state_params.oper_delegate.param = OTS_APP;
	OTS_APP->FSM.state__IBIT = fsm_state_create(OTS_APP->FSM.fsm, "STATE_IBIT", &state_params);
	if (NULL == OTS_APP->FSM.state__IBIT)
	{
		return FSM_ERROR_EMPTY;
	}

	/* Initialize state OPERATE */
	state_params.init_delegate.handler = _app_sm__state_OPERATE__init;
	state_params.init_delegate.param = OTS_APP;
	state_params.oper_delegate.handler = _app_sm__state_OPERATE__operation;
	state_params.oper_delegate.param = OTS_APP;
	OTS_APP->FSM.state__OPERATE = fsm_state_create(OTS_APP->FSM.fsm, "STATE_OPERATE", &state_params);
	if (NULL == OTS_APP->FSM.state__OPERATE)
	{
		return FSM_ERROR_EMPTY;
	}

	/* Initialize state LASEROFF */
	state_params.init_delegate.handler = _app_sm__state_LASEROFF__init;
	state_params.init_delegate.param = OTS_APP;
	state_params.oper_delegate.handler = _app_sm__state_LASEROFF__operation;
	state_params.oper_delegate.param = OTS_APP;
	OTS_APP->FSM.state__LASEROFF = fsm_state_create(OTS_APP->FSM.fsm, "STATE_LASEROFF", &state_params);
	if (NULL == OTS_APP->FSM.state__LASEROFF)
	{
		return FSM_ERROR_EMPTY;
	}

	/* Initialize state ERROR - special FSM state */
	state_params.init_delegate.handler = _app_sm__state_ERROR__init;
	state_params.init_delegate.param = OTS_APP;
	state_params.oper_delegate.handler = _app_sm__state_ERROR__operation;
	state_params.oper_delegate.param = OTS_APP;
	OTS_APP->FSM.state__ERROR = fsm_state_create(OTS_APP->FSM.fsm, "error", &state_params);
	if (NULL == OTS_APP->FSM.state__ERROR)
	{
		return FSM_ERROR_EMPTY;
	}
	
	
	/*
	**	Initialize Transitions
	*/
	
	/* Initialize transition INIT --> IDLE */
	transition_params.source_state = OTS_APP->FSM.state__INIT;
	transition_params.target_state = OTS_APP->FSM.state__IDLE;
	transition_params.check_delegate.handler = _app_sm__trans_INIT_IDLE__check;
	transition_params.check_delegate.param = OTS_APP;
	transition_params.exit_delegate.handler = _app_sm__trans_INIT_IDLE__exit;
	transition_params.exit_delegate.param = OTS_APP;
	OTS_APP->FSM.trans__INIT_IDLE = fsm_transition_create(OTS_APP->FSM.fsm, &transition_params);
	if (NULL == OTS_APP->FSM.trans__INIT_IDLE)
	{
		return FSM_ERROR_EMPTY;
	}

	/* Initialize transition IDLE --> IBIT */
	transition_params.source_state = OTS_APP->FSM.state__IDLE;
	transition_params.target_state = OTS_APP->FSM.state__IBIT;
	transition_params.check_delegate.handler = _app_sm__trans_IDLE_IBIT__check;
	transition_params.check_delegate.param = OTS_APP;
	transition_params.exit_delegate.handler = _app_sm__trans_IDLE_IBIT__exit;
	transition_params.exit_delegate.param = OTS_APP;
	OTS_APP->FSM.trans__IDLE_IBIT = fsm_transition_create(OTS_APP->FSM.fsm, &transition_params);
	if (NULL == OTS_APP->FSM.trans__IDLE_IBIT)
	{
		return FSM_ERROR_EMPTY;
	}

	/* Initialize transition IBIT --> IDLE */
	transition_params.source_state = OTS_APP->FSM.state__IBIT;
	transition_params.target_state = OTS_APP->FSM.state__IDLE;
	transition_params.check_delegate.handler = _app_sm__trans_IBIT_IDLE__check;
	transition_params.check_delegate.param = OTS_APP;
	transition_params.exit_delegate.handler = _app_sm__trans_IBIT_IDLE__exit;
	transition_params.exit_delegate.param = OTS_APP;
	OTS_APP->FSM.trans__IBIT_IDLE = fsm_transition_create(OTS_APP->FSM.fsm, &transition_params);
	if (NULL == OTS_APP->FSM.trans__IBIT_IDLE)
	{
		return FSM_ERROR_EMPTY;
	}

	/* Initialize transition IDLE --> OPERATE */
	transition_params.source_state = OTS_APP->FSM.state__IDLE;
	transition_params.target_state = OTS_APP->FSM.state__OPERATE;
	transition_params.check_delegate.handler = _app_sm__trans_IDLE_OPERATE__check;
	transition_params.check_delegate.param = OTS_APP;
	transition_params.exit_delegate.handler = _app_sm__trans_IDLE_OPERATE__exit;
	transition_params.exit_delegate.param = OTS_APP;
	OTS_APP->FSM.trans__IDLE_OPERATE = fsm_transition_create(OTS_APP->FSM.fsm, &transition_params);
	if (NULL == OTS_APP->FSM.trans__IDLE_OPERATE)
	{
		return FSM_ERROR_EMPTY;
	}

	/* Initialize transition OPERATE --> IDLE */
	transition_params.source_state = OTS_APP->FSM.state__OPERATE;
	transition_params.target_state = OTS_APP->FSM.state__IDLE;
	transition_params.check_delegate.handler = _app_sm__trans_OPERATE_IDLE__check;
	transition_params.check_delegate.param = OTS_APP;
	transition_params.exit_delegate.handler = _app_sm__trans_OPERATE_IDLE__exit;
	transition_params.exit_delegate.param = OTS_APP;
	OTS_APP->FSM.trans__OPERATE_IDLE = fsm_transition_create(OTS_APP->FSM.fsm, &transition_params);
	if (NULL == OTS_APP->FSM.trans__OPERATE_IDLE)
	{
		return FSM_ERROR_EMPTY;
	}

	/* Initialize transition IDLE --> LASEROFF */
	transition_params.source_state = OTS_APP->FSM.state__IDLE;
	transition_params.target_state = OTS_APP->FSM.state__LASEROFF;
	transition_params.check_delegate.handler = _app_sm__trans_IDLE_LASEROFF__check;
	transition_params.check_delegate.param = OTS_APP;
	transition_params.exit_delegate.handler = _app_sm__trans_IDLE_LASEROFF__exit;
	transition_params.exit_delegate.param = OTS_APP;
	OTS_APP->FSM.trans__IDLE_LASEROFF = fsm_transition_create(OTS_APP->FSM.fsm, &transition_params);
	if (NULL == OTS_APP->FSM.trans__IDLE_LASEROFF)
	{
		return FSM_ERROR_EMPTY;
	}

	/* Initialize transition LASEROFF --> IDLE */
	transition_params.source_state = OTS_APP->FSM.state__LASEROFF;
	transition_params.target_state = OTS_APP->FSM.state__IDLE;
	transition_params.check_delegate.handler = _app_sm__trans_LASEROFF_IDLE__check;
	transition_params.check_delegate.param = OTS_APP;
	transition_params.exit_delegate.handler = _app_sm__trans_LASEROFF_IDLE__exit;
	transition_params.exit_delegate.param = OTS_APP;
	OTS_APP->FSM.trans__LASEROFF_IDLE = fsm_transition_create(OTS_APP->FSM.fsm, &transition_params);
	if (NULL == OTS_APP->FSM.trans__LASEROFF_IDLE)
	{
		return FSM_ERROR_EMPTY;
	}

	/* Set INIT as the first state */
	return fsm_sm_set_active_state(OTS_APP->FSM.fsm, OTS_APP->FSM.state__INIT);
}


OTB_STATE get_active_state(OTS_APP_SM_BUNDLE_PTR pFSM)
{



	void *active_state;

	 //xSemaphoreTake( xSemaphore_OTBB_A_COM_CONNECT, ( TickType_t ) pdMS_TO_TICKS (0));

		//xSemaphoreGive(xSemaphore_OTBB_A_COM_CONNECT);
	//xSemaphoreTake( xSemaphore_OTBB_A_STATE_EXM, ( TickType_t ) pdMS_TO_TICKS (0));
	active_state = fsm_get_active_state(pFSM->fsm);
	//xSemaphoreGive(xSemaphore_OTBB_A_STATE_EXM);

	if (active_state == pFSM->state__INIT->obj)
		return OTB_STATE__INIT;
	else if (active_state == pFSM->state__IDLE->obj)
		return OTB_STATE__IDLE;
	else if (active_state == pFSM->state__IBIT->obj)
		return OTB_STATE__IBIT;
	else if (active_state == pFSM->state__OPERATE->obj)
		return OTB_STATE__OPERATE;
	else if (active_state == pFSM->state__LASEROFF->obj)
		return OTB_STATE__LASER_OFF;
	else
		return OTB_STATE_COUNT;
}
