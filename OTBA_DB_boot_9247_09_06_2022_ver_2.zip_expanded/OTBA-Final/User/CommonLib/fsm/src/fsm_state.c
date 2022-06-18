/**
 ******************************************************************************
 * @file    fsm_state.c
 * @brief   Finite State Machine states related functions implementations
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
#include "cmsis_os.h"
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
* Function Name    : fsm_state_create
* Returned Value   : FSM_OBJECT_STRUCT_PTR - newly created FSM object.
* Comments         :
*    Creates and returns new FSM state handler pointer.
*
*END*********************************************************************/
FSM_OBJECT_STRUCT_PTR fsm_state_create
				(	FSM_OBJECT_STRUCT_PTR fsm_obj,
					const char* name,
					FSM_STATE_PARAMS_STRUCT_PTR_t params	)
{
	FSM_SM_STRUCT_PTR_t		fsm;
	FSM_STATE_STRUCT_PTR_t	new_state;
	FSM_OBJECT_STRUCT_PTR	new_state_obj;
	uint32_t					name_len;
	uint32_t					if_error_state = 0;		// flag which signs if error state is supposed to be openned.
	
	/* Input validation */
	if (fsm_obj == NULL)
		return NULL;
	
	if (fsm_obj->type != FSM_OBJECT_TYPE_SM)
		return NULL;
	
	if (name == NULL)
		return NULL;
	
	fsm = (FSM_SM_STRUCT_PTR_t) fsm_obj->obj;
	if (fsm == NULL)
		return NULL;
	
	/* Check if the name equalls to "error". If yes, open error_state. */
	if (strcmp(name, "error") == 0 && fsm->error_state == NULL)
	{
		if_error_state = 1;
	}
	else
	{
		if (fsm->count_states >= fsm->max_num_states)
			return NULL;
	}
	
	
	/* Create the new state */
	new_state = (FSM_STATE_STRUCT_PTR_t) pvPortMalloc(sizeof(FSM_STATE_STRUCT_t));
	if (new_state == NULL)
	{
		goto fsm_state_create_error_0;
	}
	
	memset(new_state, 0, sizeof(FSM_STATE_STRUCT_t));
	
	/* Copy state name */
	name_len = strlen(name);
	new_state->name = (char*) pvPortMalloc(name_len);
	if (new_state->name == NULL)
	{
		goto fsm_state_create_error_1;
	}
	
	memcpy(new_state->name, name, name_len);
	new_state->name[name_len] = '\0';		// string END char
	
	/* Initialize new_state */
	if (params != NULL)
	{
		new_state->g.init_delegate.handler		= params->init_delegate.handler;
		new_state->g.init_delegate.param		= params->init_delegate.param;
		new_state->g.oper_delegate.handler		= params->oper_delegate.handler;
		new_state->g.oper_delegate.param		= params->oper_delegate.param;
	}
	new_state->g.initialized 		= 0;
	new_state->g.enable		 		= 1;
	new_state->index				= fsm->count_states;
	
	/* Check if the name equalls to "error". If yes, open error_state. */
	if (if_error_state)
	{
		if (fsm->error_state != NULL)
		{
			goto fsm_state_create_error_2;
		}
		
		fsm->error_state = new_state;
	}
	else
	{
		/* Update fsm struct */
		fsm->states[fsm->count_states] = new_state;
		fsm->count_states++;
	}

	/* Create the new state object */
	new_state_obj = (FSM_OBJECT_STRUCT_PTR) pvPortMalloc(sizeof(FSM_OBJECT_STRUCT));
	if (new_state_obj == NULL)
	{
		goto fsm_state_create_error_2;
	}
	
	memset(new_state_obj, 0, sizeof(FSM_OBJECT_STRUCT));
	
	new_state_obj->obj	= new_state;
	new_state_obj->type	= FSM_OBJECT_TYPE_STATE;
	
	/* return the new state */
	return new_state_obj;
	
	/* error handling */
fsm_state_create_error_2:
	if (new_state->name != NULL)
		vPortFree(new_state->name);
fsm_state_create_error_1:
	if (new_state)
		vPortFree(new_state);
fsm_state_create_error_0:
  	return NULL;
}



/*FUNCTION*****************************************************************
*
* Function Name    : fsm_get_if_state_initialized
* Returned Value   : uint32_t
* Comments         :
*    Returns pointer to error state.
*
*END*********************************************************************/
uint32_t fsm_get_if_state_initialized(FSM_OBJECT_STRUCT_PTR obj)
{
	FSM_STATE_STRUCT_PTR_t	state;
	uint8_t initialized = 0;
	
	if (obj == NULL)
		return 0u;
	
	if (obj->type != FSM_OBJECT_TYPE_STATE)
		return 0u;
	
	FSM_lock();
	
	state = (FSM_STATE_STRUCT_PTR_t) obj->obj;
	initialized = state->g.initialized;
	
	FSM_unlock();
	
	return initialized;	
}
