/**
 ******************************************************************************
 * @file    fsm_private.h
 * @brief   Finite State Machine private declarations and definitions
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


#ifndef __fsm_prv_h__
#define __fsm_prv_h__

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------------------------------------------------

/*
**	Includes:
*/

#include "./fsm.h"

//--------------------------------------------------------------------------------------------------------------

/*
**	Macros:
*/

#define FSM_lock()						portENTER_CRITICAL()
#define FSM_unlock()					portEXIT_CRITICAL()

//--------------------------------------------------------------------------------------------------------------

/*
**	Typedefs:
*/

typedef struct {
	FSM_DELEGATE_STRUCT_t				init_delegate;					// delegate to state initialization handler
	FSM_DELEGATE_STRUCT_t				oper_delegate;					// delegate to state operation handler
	
	uint32_t							initialized;					// identifier if the state is initialized
	uint32_t							enable;							// identifier if the state is enabled
} FSM_STATE_GENERIC_STRUCT_t, *FSM_STATE_GENERIC_STRUCT_PTR_t;

//--------------------------------------------------------------------------------------------------------------

typedef struct {
	FSM_DELEGATE_STRUCT_t				exit_delegate;					// delegate to state exit handler
	FSM_DELEGATE_STRUCT_t				check_delegate;					// delegate to state check if exit handler
	
	uint32_t							enable;							// identifier if the transition is enabled
} FSM_TRANSITION_GENERIC_STRUCT_t, *FSM_TRANSITION_GENERIC_STRUCT_PTR_t;

//--------------------------------------------------------------------------------------------------------------

typedef struct {
	FSM_TRANSITION_GENERIC_STRUCT_t		g;								// generic params
} FSM_TRANSITION_STRUCT_t, *FSM_TRANSITION_STRUCT_PTR_t;

//--------------------------------------------------------------------------------------------------------------

typedef struct {
	FSM_STATE_GENERIC_STRUCT_t			g;					// generic params
	                                                            
	char*								name;				// state name
	uint32_t							index;				// state index in the state-machine
} FSM_STATE_STRUCT_t, *FSM_STATE_STRUCT_PTR_t;

//--------------------------------------------------------------------------------------------------------------

typedef struct {
	FSM_STATE_STRUCT_PTR_t				active_state;		// active state object
	FSM_STATE_STRUCT_PTR_t				next_state;			// next state object (will be activated on the next run)
	FSM_STATE_STRUCT_PTR_t				error_next_state;	// next state after error state is exited
	
	uint32_t							if_error;			// private identifier if there was error in order to be able to set error_next_state
	int32_t								error;				// private identifier for the error number
	uint32_t							enable;				// identifier if the state-machine is enabled
	
	uint32_t							log_index;			// MQX lwlog index
	uint32_t							log_entries;		// MQX lwlog number of entries
	uint32_t							*log_ptr;			// MQX lwlog pointer to the log pool
} FSM_GENERIC_STRUCT_t, *FSM_GENERIC_STRUCT_PTR_t;

//--------------------------------------------------------------------------------------------------------------

typedef struct {
	FSM_GENERIC_STRUCT_t				g;					// generic params
	
	FSM_STATE_STRUCT_PTR_t				*states;			// vector of all the states in the state-machine
	FSM_STATE_STRUCT_PTR_t				error_state;		// error state object
	FSM_TRANSITION_STRUCT_PTR_t			**transitions;		// 2d array of transitions ordered in a [source, dest] order.
	
	uint32_t							max_num_states;		// Maximum number of states to be created
	uint32_t							count_states;		// Created states counter
	
	uint8_t								log_enabled;		// State machine log enabled

} FSM_SM_STRUCT_t, *FSM_SM_STRUCT_PTR_t;

//--------------------------------------------------------------------------------------------------------------

/*
**	Functions declarations:
*/

void _fsm_move_to_error_state(FSM_SM_STRUCT_PTR_t, int32_t);


//--------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif	/* __fsm_prv_h__ */

