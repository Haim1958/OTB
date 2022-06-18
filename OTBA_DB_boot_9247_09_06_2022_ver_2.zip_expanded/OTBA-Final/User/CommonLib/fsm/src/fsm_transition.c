/**
 ******************************************************************************
 * @file    fsm.c
 * @brief   Finite State Machine transitions related functions implementations
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



/*
**	Includes:
*/
#include <string.h>
#include <stdlib.h>
#include "UserLib.h"
#include "../inc/fsm.h"
#include "../inc/fsm_prv.h"

//--------------------------------------------------------------------------------------------------------------

/*
**	Global variables:
*/


//--------------------------------------------------------------------------------------------------------------

/*
**	Functions implementations:
*/

/*FUNCTION*****************************************************************
*
* Function Name    : fsm_transition_create
* Returned Value   : FSM_OBJECT_STRUCT_PTR - newly created FSM object.
* Comments         :
*    Creates and returns new FSM transition handler pointer.
*
*END*********************************************************************/
FSM_OBJECT_STRUCT_PTR fsm_transition_create(FSM_OBJECT_STRUCT_PTR fsm_obj,
													FSM_TRANSITION_PARAMS_STRUCT_PTR_t params)
{
	FSM_SM_STRUCT_PTR_t			fsm;
	FSM_TRANSITION_STRUCT_PTR_t	new_transition;
	FSM_OBJECT_STRUCT_PTR		new_transition_obj;
	FSM_STATE_STRUCT_PTR_t		source_state, target_state;
	uint32_t					source_state_ind, target_state_ind;
	
	/* Input validation */
	if (fsm_obj == NULL)
		return NULL;
	
	if (fsm_obj->type != FSM_OBJECT_TYPE_SM)
		return NULL;
	
	fsm = (FSM_SM_STRUCT_PTR_t) fsm_obj->obj;
	if (fsm == NULL)
		return NULL;
	
	if (params == NULL)
		return NULL;
	
	if (params->check_delegate.handler == NULL)
		return NULL;
	
	// params->exit_handler doesn't have to be initialized.
	
	if (params->source_state->type != FSM_OBJECT_TYPE_STATE)
		return NULL;
	
	if (params->target_state->type != FSM_OBJECT_TYPE_STATE)
		return NULL;
	
	/* Create the new transition struct */
	new_transition = (FSM_TRANSITION_STRUCT_PTR_t) pvPortMalloc(sizeof(FSM_TRANSITION_STRUCT_t));
	if (new_transition == NULL)
	{
		goto fsm_transition_create_error_0;
	}
	
	memset(new_transition, 0, sizeof(FSM_TRANSITION_STRUCT_t));
	
	/* set transition params */
	new_transition->g.check_delegate.handler		= params->check_delegate.handler;
	new_transition->g.check_delegate.param			= params->check_delegate.param;
	new_transition->g.exit_delegate.handler			= params->exit_delegate.handler;
	new_transition->g.exit_delegate.param			= params->exit_delegate.param;
	new_transition->g.enable						= 1;	// automatically enabled
	
	/* Create the new fsm object for transition */
	new_transition_obj = (FSM_OBJECT_STRUCT_PTR) pvPortMalloc(sizeof(FSM_OBJECT_STRUCT));
	if (new_transition_obj == NULL)
	{
		goto fsm_transition_create_error_1;
	}
	
	memset(new_transition_obj, 0, sizeof(FSM_OBJECT_STRUCT));
	
	new_transition_obj->obj				= new_transition;
	new_transition_obj->type			= FSM_OBJECT_TYPE_TRANSITION;
	
	/* Update fsm struct */
	source_state						= (FSM_STATE_STRUCT_PTR_t) params->source_state->obj;
	target_state						= (FSM_STATE_STRUCT_PTR_t) params->target_state->obj;
	source_state_ind					= source_state->index;
	target_state_ind					= target_state->index;
	fsm->transitions[source_state_ind][target_state_ind] = new_transition;
	
	return new_transition_obj;
	
	/* error handling */
fsm_transition_create_error_1:
	if (new_transition)
		vPortFree(new_transition);
fsm_transition_create_error_0:
  	return NULL;
}
