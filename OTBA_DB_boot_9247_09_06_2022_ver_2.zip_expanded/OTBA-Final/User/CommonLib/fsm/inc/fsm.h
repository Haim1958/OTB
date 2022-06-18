/**
 ******************************************************************************
 * @file    fsm.h
 * @brief   Finite State Machine public declarations and definitions
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


#ifndef __fsm_h__
#define __fsm_h__



//--------------------------------------------------------------------------------------------------------------

/*
**	Includes:
*/

//--------------------------------------------------------------------------------------------------------------

/*
**	Macros:
*/

/* FSM Errors */
#define FSM_OK							ERROR_OK
#define FSM_ERROR_EMPTY					FSM_ERROR(1)	// Object or parameter received is empty.
#define FSM_ERROR_WRONG_OBJECT_TYPE		FSM_ERROR(2)	// Object received is not of an expected type.
#define FSM_ERROR_DISABLED				FSM_ERROR(3)	// Current state machine or state is disabled.
#define FSM_ERROR_STATE_RETURN_ERROR	FSM_ERROR(4)	// Current state has returned an error.
#define FSM_ERROR_NO_STATE_ERROR		FSM_ERROR(5)	// Current state has not returned an error, thus cannot modify the error_next_state
#define FSM_ERROR_STATE_NOT_FOUND		FSM_ERROR(6)	// The requested state was not found in the FSM states list.

//--------------------------------------------------------------------------------------------------------------

/*
**	Typedefs:
*/

/* Possible types of the FSM objects */
typedef enum {
	FSM_OBJECT_TYPE_NONE,
	FSM_OBJECT_TYPE_SM,
	FSM_OBJECT_TYPE_STATE,
	FSM_OBJECT_TYPE_TRANSITION
} FSM_OBJECT_TYPE_ENUM_t;

//--------------------------------------------------------------------------------------------------------------

/* FSM generic object type */
typedef struct {
	void*						obj;			// The actual object 
	FSM_OBJECT_TYPE_ENUM_t		type;			// The type of the actual object
} FSM_OBJECT_STRUCT, *FSM_OBJECT_STRUCT_PTR;

//--------------------------------------------------------------------------------------------------------------

/* FSM delegate */
typedef struct {
	int32_t							(*handler)(void*);		// function pointer to the handler
	void*							param;					// function pointer to parameter passed to the handler
} FSM_DELEGATE_STRUCT_t, *FSM_DELEGATE_STRUCT_PTR_t;

//--------------------------------------------------------------------------------------------------------------

/* FSM transition initialization params */
typedef struct {
	FSM_DELEGATE_STRUCT_t		exit_delegate;				// delegate to state exit handler
	FSM_DELEGATE_STRUCT_t		check_delegate;				// delegate to state check if exit handler
	
	FSM_OBJECT_STRUCT_PTR		source_state;				// source state of this transition
	FSM_OBJECT_STRUCT_PTR		target_state;				// destination state of this transition
} FSM_TRANSITION_PARAMS_STRUCT_t, *FSM_TRANSITION_PARAMS_STRUCT_PTR_t;

//--------------------------------------------------------------------------------------------------------------

typedef struct {
	FSM_DELEGATE_STRUCT_t		init_delegate;				// delegate to state initialization handler
	FSM_DELEGATE_STRUCT_t		oper_delegate;				// delegate to state operation handler
} FSM_STATE_PARAMS_STRUCT_t, *FSM_STATE_PARAMS_STRUCT_PTR_t;

//--------------------------------------------------------------------------------------------------------------

typedef struct {
	uint32_t					max_num_states;					// Maximum number of states supported by the state-machine.
	uint8_t						log_enabled;					// State machine logging is enabled
} FSM_PARAMS_STRUCT_t, *FSM_PARAMS_STRUCT_PTR_t;

//--------------------------------------------------------------------------------------------------------------

/*
**	Functions declarations:
*/
	
#ifdef __cplusplus
extern "C" {
#endif

FSM_OBJECT_STRUCT_PTR		fsm_sm_create(FSM_PARAMS_STRUCT_PTR_t);

int32_t						fsm_sm_set_active_state(FSM_OBJECT_STRUCT_PTR,
													FSM_OBJECT_STRUCT_PTR);

int32_t						fsm_sm_set_error_next_state(FSM_OBJECT_STRUCT_PTR,
														FSM_OBJECT_STRUCT_PTR);

int32_t						fsm_sm_run(FSM_OBJECT_STRUCT_PTR fsm_obj);

int32_t						fsm_enable(FSM_OBJECT_STRUCT_PTR);
int32_t						fsm_disable(FSM_OBJECT_STRUCT_PTR);

FSM_OBJECT_STRUCT_PTR		fsm_state_create(FSM_OBJECT_STRUCT_PTR,
											 const char *,
											 FSM_STATE_PARAMS_STRUCT_PTR_t);

FSM_OBJECT_STRUCT_PTR		fsm_transition_create(FSM_OBJECT_STRUCT_PTR,
												  FSM_TRANSITION_PARAMS_STRUCT_PTR_t);

int32_t						fsm_get_error(FSM_OBJECT_STRUCT_PTR,
										  int32_t *);

void* fsm_get_active_state(FSM_OBJECT_STRUCT_PTR obj);
uint32_t fsm_get_if_state_initialized(FSM_OBJECT_STRUCT_PTR obj);


//--------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
#endif	/* __fsm_h__ */

