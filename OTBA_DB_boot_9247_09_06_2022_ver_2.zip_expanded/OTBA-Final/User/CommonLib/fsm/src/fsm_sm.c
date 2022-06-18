/**
 ******************************************************************************
 * @file    fsm_sm.c
 * @brief   Finite State Machine state-machine related functions implementations
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
* Function Name    : fsm_sm_create
* Returned Value   : FSM_OBJECT_STRUCT_PTR - newly created FSM object.
* Comments         :
*    Creates and returns new FSM handler pointer.
*
*END*********************************************************************/
FSM_OBJECT_STRUCT_PTR fsm_sm_create(FSM_PARAMS_STRUCT_PTR_t params)
{
	FSM_OBJECT_STRUCT_PTR	fsm_obj;
	FSM_SM_STRUCT_PTR_t 		fsm;
	uint32_t					i;	
	
	/* Allocate the state machine struct */
	fsm = (FSM_SM_STRUCT_PTR_t) pvPortMalloc(sizeof(FSM_SM_STRUCT_t));
	if (fsm == NULL)
	{
		goto fsm_sm_create_error_0;
	}
	
	memset(fsm, 0, sizeof(FSM_SM_STRUCT_t));
	
	fsm->log_enabled		= params->log_enabled;

	fsm->max_num_states		= params->max_num_states;
	fsm->count_states		= 0;
	
	fsm->g.if_error			= 0;
	fsm->g.enable			= 1;
	
	/* Allocate the states vector */
	fsm->states = (FSM_STATE_STRUCT_PTR_t *) pvPortMalloc(sizeof(FSM_STATE_STRUCT_PTR_t) * fsm->max_num_states);
	if (fsm->states == NULL)
	{
		goto fsm_sm_create_error_1;
	}
	
	memset(fsm->states, 0, sizeof(FSM_STATE_STRUCT_PTR_t) * fsm->max_num_states);
	
	
	/* Allocate the transitions matrix */
	fsm->transitions = (FSM_TRANSITION_STRUCT_PTR_t **) pvPortMalloc(sizeof(FSM_TRANSITION_STRUCT_PTR_t *) * fsm->max_num_states);
	if (fsm->transitions == NULL)
	{
		goto fsm_sm_create_error_2;
	}
	
	memset(fsm->transitions, 0, sizeof(FSM_TRANSITION_STRUCT_PTR_t *) * fsm->max_num_states);
	
	/* Allocate each collumn of the transitions matrix */
	for (i = 0; i < fsm->max_num_states; i++)
	{
		fsm->transitions[i] = (FSM_TRANSITION_STRUCT_PTR_t *) pvPortMalloc(sizeof(FSM_TRANSITION_STRUCT_PTR_t) * fsm->max_num_states);
		if (fsm->transitions[i] == NULL)
		{
			goto fsm_sm_create_error_3;
		}
		
		memset(fsm->transitions[i], 0, sizeof(FSM_TRANSITION_STRUCT_PTR_t) * fsm->max_num_states);		
	}
	
	/* Allocate fsm_object for fsm */
	fsm_obj = (FSM_OBJECT_STRUCT_PTR) pvPortMalloc(sizeof(FSM_OBJECT_STRUCT));
	if (fsm_obj == NULL)
	{
		goto fsm_sm_create_error_3;
	}
	
	memset(fsm_obj, 0, sizeof(FSM_OBJECT_STRUCT));	
		
	fsm_obj->obj	= fsm;
	fsm_obj->type	= FSM_OBJECT_TYPE_SM;
	
	return fsm_obj;
	
	/* 
	**	Error handling
	*/
	
fsm_sm_create_error_3:	
	for (i = 0; i < fsm->max_num_states; i++)
	{
		if (fsm->transitions[i] != NULL)
			vPortFree(fsm->transitions[i]);
	}
	if (fsm->transitions != NULL)
		vPortFree(fsm->transitions);
fsm_sm_create_error_2:	
	if (fsm->states != NULL)
		vPortFree(fsm->states);
fsm_sm_create_error_1:	
	if (fsm != NULL)
		vPortFree(fsm);
fsm_sm_create_error_0:	
	return NULL;
}

//--------------------------------------------------------------------------------------------------------------

/*FUNCTION*****************************************************************
*
* Function Name    : fsm_sm_set_active_state
* Returned Value   : int32_t - FSM_ERROR_EMPTY,
								FSM_ERROR_WRONG_OBJECT_TYPE,
								FSM_ERROR_STATE_RETURN_ERROR,
								FSM_ERROR_STATE_NOT_FOUND,
								FSM_OK
* Comments         :
*    Set the requested state as the next state the state machine will run.
*	 Doesn't work if in error state. Use fsm_sm_set_error_next_state instead.
*
*END*********************************************************************/
int32_t fsm_sm_set_active_state
			(	FSM_OBJECT_STRUCT_PTR fsm_obj,
				FSM_OBJECT_STRUCT_PTR state_obj)
{
	FSM_SM_STRUCT_PTR_t		fsm;
	FSM_STATE_STRUCT_PTR_t	state;
	uint32_t 					i;
	
	/* Input validation */
	if (fsm_obj == NULL)
		return FSM_ERROR_EMPTY;
	
	if (state_obj == NULL)
		return FSM_ERROR_EMPTY;
	
	if (fsm_obj->obj == NULL)
		return FSM_ERROR_EMPTY;
	
	if (state_obj->obj == NULL)
		return FSM_ERROR_EMPTY;
	
	if (fsm_obj->type != FSM_OBJECT_TYPE_SM)
		return FSM_ERROR_WRONG_OBJECT_TYPE;
	
	if (state_obj->type != FSM_OBJECT_TYPE_STATE)
		return FSM_ERROR_WRONG_OBJECT_TYPE;
			
	/* Setting active state */
	
	fsm		= (FSM_SM_STRUCT_PTR_t) fsm_obj->obj;
	state	= (FSM_STATE_STRUCT_PTR_t) state_obj->obj;
	
	/* Check if fsm is right now in error state */
	if (fsm->g.if_error)
		return FSM_ERROR_STATE_RETURN_ERROR;
			
	/* Try to find the state in the FSM state list */
	//for (i = 0; i < fsm->max_num_states, fsm->states[i] != state; i++);
	for (i = 0; i < fsm->max_num_states && fsm->states[i] != state; i++);

	if (i == fsm->max_num_states)
	{	// we haven't found the requested state in the FSM state list
		return FSM_ERROR_STATE_NOT_FOUND;
	}	
	
	FSM_lock();
	
	fsm->g.next_state = state;			// next state will be activated the next time state-machine is run
	
	FSM_unlock();	
	
	return FSM_OK;
}

//--------------------------------------------------------------------------------------------------------------

/*FUNCTION*****************************************************************
*
* Function Name    : fsm_sm_set_error_next_state
* Returned Value   : int32_t - FSM_ERROR_EMPTY,
								FSM_ERROR_WRONG_OBJECT_TYPE,
								FSM_ERROR_NO_STATE_ERROR,
								FSM_ERROR_STATE_NOT_FOUND,
								FSM_OK
* Comments         :
*    Set the requested state as the next state the state machine will run after the error state.
*	 Doesn't work if not in error state. Use fsm_sm_set_active_state instead.
*
*END*********************************************************************/
int32_t fsm_sm_set_error_next_state
			(	FSM_OBJECT_STRUCT_PTR fsm_obj,
				FSM_OBJECT_STRUCT_PTR state_obj)
{
	FSM_SM_STRUCT_PTR_t		fsm;
	FSM_STATE_STRUCT_PTR_t	state;
	uint32_t 					i;
	
	/* Input validation */
	if (fsm_obj == NULL)
		return FSM_ERROR_EMPTY;
	
	if (state_obj == NULL)
		return FSM_ERROR_EMPTY;
	
	if (fsm_obj->obj == NULL)
		return FSM_ERROR_EMPTY;
	
	if (state_obj->obj == NULL)
		return FSM_ERROR_EMPTY;
	
	if (fsm_obj->type != FSM_OBJECT_TYPE_SM)
		return FSM_ERROR_WRONG_OBJECT_TYPE;
	
	if (state_obj->type != FSM_OBJECT_TYPE_STATE)
		return FSM_ERROR_WRONG_OBJECT_TYPE;
	
	fsm		= (FSM_SM_STRUCT_PTR_t) fsm_obj->obj;
	state	= (FSM_STATE_STRUCT_PTR_t) state_obj->obj;
	
	/* Check if fsm is right now in error state */
	if (fsm->g.if_error == 0)
		return FSM_ERROR_NO_STATE_ERROR;
			
	/* Try to find the state in the FSM state list */
	for (i = 0; i < fsm->max_num_states && fsm->states[i] != state; i++);

	if (i == fsm->max_num_states)
	{	// we haven't found the requested state in the FSM state list
		return FSM_ERROR_STATE_NOT_FOUND;
	}	
	
	FSM_lock();
	
	/* Setting active state */
	fsm->g.error_next_state = state;
	
	FSM_unlock();
	
	return FSM_OK;
}

//--------------------------------------------------------------------------------------------------------------

/*FUNCTION*****************************************************************
*
* Function Name    : fsm_sm_run
* Returned Value   : int32_t - FSM_ERROR_EMPTY,
								FSM_ERROR_WRONG_OBJECT_TYPE,
								FSM_ERROR_DISABLED,
								FSM_OK
* Comments         :
*    Runs the state machine.
*
*END*********************************************************************/
int32_t fsm_sm_run(FSM_OBJECT_STRUCT_PTR fsm_obj)
{
	FSM_SM_STRUCT_PTR_t			fsm;
	FSM_STATE_STRUCT_PTR_t		active_state;
	uint32_t						trans_ret;
	uint32_t						status = 0;					// flag for indication moving to another state
	
	
	if (fsm_obj == NULL)
		return FSM_ERROR_EMPTY;
	
	if (fsm_obj->type != FSM_OBJECT_TYPE_SM)
		return FSM_ERROR_WRONG_OBJECT_TYPE;
	
	fsm	= (FSM_SM_STRUCT_PTR_t) fsm_obj->obj;
	
	if (fsm->g.enable == 0)
		return FSM_ERROR_DISABLED;
	
	/* Check if we previously set fsm->next_state. If yes, change active_state to next_state. */
	if (fsm->g.next_state != NULL)
	{
		fsm->g.active_state		= fsm->g.next_state;
		fsm->g.next_state		= NULL;
	}
	
	active_state	= fsm->g.active_state;
	
	/* Check enable flag */
	if (!active_state->g.enable)
		return FSM_ERROR_DISABLED;
	
	/* Initialize the state */
	if (!active_state->g.initialized)
	{
		if(active_state->g.init_delegate.handler != NULL)
		{
			status = active_state->g.init_delegate.handler(active_state->g.init_delegate.param);		// run the initialization handler
			if (status == FSM_OK)
			{
				active_state->g.initialized = 1;			// set the flag for the next time the state is run
			}
			else 
			{
				_fsm_move_to_error_state(fsm, status);	// handle error state
			}
		}
	}
	
	/*
	**	Run the state handler
	*/
	status = active_state->g.oper_delegate.handler(active_state->g.oper_delegate.param);
	if (status != FSM_OK)
	{
		_fsm_move_to_error_state(fsm, status);	// handle error state
	}
	else	/* else if active state handler returned MQX_OK value */
	{
		FSM_STATE_STRUCT_PTR_t next_state = NULL;
		FSM_TRANSITION_STRUCT_PTR_t	*active_state_transitions;
		uint32_t max_number_states = fsm->max_num_states;
		uint32_t i = 0;

		/*
		**	Check if we need to move to the another state
		*/
		active_state_transitions	= fsm->transitions[active_state->index];
		
		/* check transitions for the current state */
		while (i < max_number_states && next_state == NULL)
		{
			if (active_state_transitions[i] != NULL && active_state_transitions[i]->g.enable)
			{
				/* check if we need to exit to another state */
				trans_ret = active_state_transitions[i]->g.check_delegate.handler(active_state_transitions[i]->g.check_delegate.param);
				
				if (trans_ret)
				{
					/* save the next state and run the exit function */
					next_state = fsm->states[i];
					
					if (fsm->log_enabled)
					{
						printf(":::FSM::: %s --> %s\r\n", active_state->name, next_state->name);
					}

					if (active_state_transitions[i]->g.exit_delegate.handler != NULL)
					{
						status = active_state_transitions[i]->g.exit_delegate.handler(active_state_transitions[i]->g.exit_delegate.param);
						if (status != ERROR_OK)
						{
							_fsm_move_to_error_state(fsm, status);	// handle error state
                            return FSM_OK;
						}
					}
				}
			}
			
			i++;
		}
	
		if (next_state != NULL)
		{
			fsm->g.next_state = next_state;
			active_state->g.initialized = 0;		// clear initialization flag for the next time
		}
	}
		
	
	return FSM_OK;
}

//--------------------------------------------------------------------------------------------------------------

/*FUNCTION*****************************************************************
*
* Function Name    : fsm_enable
* Returned Value   : int32_t - FSM_ERROR_EMPTY,
								FSM_ERROR_WRONG_OBJECT_TYPE,
								FSM_OK
* Comments         :
*    Detects which type is the object of (fsm, state, transition) and enables it.
*
*END*********************************************************************/
int32_t fsm_enable(FSM_OBJECT_STRUCT_PTR obj)
{
	if (obj == NULL)
	{
		return FSM_ERROR_EMPTY;
	}
	
	FSM_lock();

	switch (obj->type)
	{
	  case FSM_OBJECT_TYPE_STATE:
		{
			FSM_STATE_STRUCT_PTR_t state			= (FSM_STATE_STRUCT_PTR_t) obj->obj;
			state->g.enable							= 1;		// enable
		}
		break;
		
	  case FSM_OBJECT_TYPE_TRANSITION:
		
		{
			FSM_TRANSITION_STRUCT_PTR_t transition	= (FSM_TRANSITION_STRUCT_PTR_t) obj->obj;
			transition->g.enable					= 1;		// enable
		}
		break;
	  
	  case FSM_OBJECT_TYPE_SM:		/* see default case */
		{
			FSM_SM_STRUCT_PTR_t fsm					= (FSM_SM_STRUCT_PTR_t) obj->obj;
			fsm->g.enable							= 1;		// enable
		}
		break;
	  
	  default:
		FSM_unlock();
		return FSM_ERROR_WRONG_OBJECT_TYPE;
	}
	
	FSM_unlock();
	
	return FSM_OK;	
}
//--------------------------------------------------------------------------------------------------------------

/*FUNCTION*****************************************************************
*
* Function Name    : fsm_disable
* Returned Value   : int32_t - FSM_ERROR_EMPTY,
								FSM_ERROR_WRONG_OBJECT_TYPE,
								FSM_OK
* Comments         :
*    Detects which type is the object of (fsm, state, transition) and disables it.
*
*END*********************************************************************/
int32_t fsm_disable(FSM_OBJECT_STRUCT_PTR obj)
{
	if (obj == NULL)
	{
		return FSM_ERROR_EMPTY;
	}
	
	FSM_lock();
	
	switch (obj->type)
	{
	  case FSM_OBJECT_TYPE_STATE:
		{
			FSM_STATE_STRUCT_PTR_t state			= (FSM_STATE_STRUCT_PTR_t) obj->obj;
			state->g.enable							= 0;		// disable
		}
		break;
		
	  case FSM_OBJECT_TYPE_TRANSITION:
		
		{
			FSM_TRANSITION_STRUCT_PTR_t transition	= (FSM_TRANSITION_STRUCT_PTR_t) obj->obj;
			transition->g.enable					= 0;		// disable
		}
		break;
	  
	  case FSM_OBJECT_TYPE_SM:		/* see default case */
		{
			FSM_SM_STRUCT_PTR_t fsm					= (FSM_SM_STRUCT_PTR_t) obj->obj;
			fsm->g.enable							= 0;		// disable
		}
		break;
	  
	  default:
		FSM_unlock();
		return FSM_ERROR_WRONG_OBJECT_TYPE;
	}
	
	FSM_unlock();
	
	return FSM_OK;	
}

/*FUNCTION*****************************************************************
*
* Function Name    : fsm_get_error
* Returned Value   : int32_t - FSM_ERROR_EMPTY,
								FSM_ERROR_WRONG_OBJECT_TYPE,
								FSM_OK
* Comments         :
*    Returns the last error if ever occured during state operation handling.
*
*END*********************************************************************/
int32_t fsm_get_error(FSM_OBJECT_STRUCT_PTR obj, int32_t *error)
{
	FSM_SM_STRUCT_PTR_t			fsm;
	
	if (obj == NULL)
		return FSM_ERROR_EMPTY;
	
	if (obj->type != FSM_OBJECT_TYPE_SM)
		return FSM_ERROR_WRONG_OBJECT_TYPE;
	
	FSM_lock();
	
	fsm = (FSM_SM_STRUCT_PTR_t) obj->obj;
	*error = fsm->g.error;
	
	FSM_unlock();
	
	return FSM_OK;	
}


/*FUNCTION*****************************************************************
*
* Function Name    : fsm_get_error
* Returned Value   : int32_t - FSM_ERROR_EMPTY,
								FSM_ERROR_WRONG_OBJECT_TYPE,
								FSM_OK
* Comments         :
*    Moves to error state.
*
*END*********************************************************************/
void _fsm_move_to_error_state(FSM_SM_STRUCT_PTR_t fsm, int32_t error)
{
	/* If there was an error when running the state handler, activate the error state */
	if (fsm->error_state != NULL && fsm->error_state->g.enable)
	{
		fsm->g.error_next_state = NULL;
		fsm->g.if_error			= 1;							// Set the error bit to make it possible to set error_next_state
																// using the API
		fsm->g.error			= error;
		
		/* Initialize the error_state */
		if (!fsm->error_state->g.initialized)
		{
			if (fsm->error_state->g.init_delegate.handler != NULL)
			{
				fsm->error_state->g.init_delegate.handler(fsm->error_state->g.init_delegate.param); 	// run the initialization handler
			}
			fsm->error_state->g.initialized = 1;				// set the flag for the next time
		}
		
		printf(":::FSM::: Error was raised: 0x%X\r\n", (unsigned int)error);

		/*
		**	Run the error_state handler, ONCE!
		*/
		if (fsm->error_state->g.oper_delegate.handler != NULL)
		{
			fsm->error_state->g.oper_delegate.handler(fsm->error_state->g.oper_delegate.param);			// run the state handler
		}
		fsm->error_state->g.initialized = 0;
		
		/*
		**	Here, during error_state, fsm->error_state should have been updated.
		*/
		
		if (fsm->g.error_next_state != NULL)
		{
			fsm->g.next_state = fsm->g.error_next_state;		// Set error_next_state as next_state
			fsm->g.active_state->g.initialized = 0;					// clear initialization flag for the next time
		}	
		
		fsm->g.if_error			= 0;							// Clear the error bit to make it impossible to set error_next_state
		fsm->g.error			= FSM_OK;
	}
}



/*FUNCTION*****************************************************************
*
* Function Name    : fsm_get_active_state
* Returned Value   : FSM_STATE_STRUCT_PTR_t
* Comments         :
*    Returns pointer to error state.
*
*END*********************************************************************/
void* fsm_get_active_state(FSM_OBJECT_STRUCT_PTR obj)
{
	FSM_SM_STRUCT_PTR_t	fsm;
	FSM_STATE_STRUCT_PTR_t active_state;
	
	if (obj == NULL)
		return NULL;
	
	if (obj->type != FSM_OBJECT_TYPE_SM)
		return NULL;
	
	//FSM_lock();
	
	fsm = (FSM_SM_STRUCT_PTR_t) obj->obj;
	active_state = fsm->g.active_state;
	
	//FSM_unlock();
	
	return active_state;	
}
