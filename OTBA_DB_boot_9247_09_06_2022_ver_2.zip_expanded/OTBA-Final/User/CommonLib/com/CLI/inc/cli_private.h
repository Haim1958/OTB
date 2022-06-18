/**
 ******************************************************************************
 * @file    cli_private.h
 * @brief   CLI private declarations and definitions
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


#ifndef __CLI_PRIVATE_H__
#define __CLI_PRIVATE_H__

/* Includes */

#include "UserLib.h"
#include "stream_buffer.h"

/* Definitions */


/* Typedefs */


typedef struct cli_instance_generic {
	uint8_t					write_buffer[CLI_LINE_MAX_LENGTH + 1];	// incl. null termination
	uint32_t				write_index;

	char					read_line[CLI_LINE_MAX_LENGTH + 1];		// incl. null termination
	char*					strtok_internal;

	char*					command;								// current command being processed
	char*					token;									// last token read
} CLI_INSTANCE_GENERIC, * CLI_INSTANCE_GENERIC_PTR;

typedef struct cli_instance {
	uint32_t				instance_index;
	CLI_INSTANCE_GENERIC	g;
} CLI_INSTANCE, * CLI_INSTANCE_PTR;


typedef struct cli_bundle {
	uint32_t				total_number_instances;
	uint32_t				next_instance_index;
	CLI_INSTANCE_PTR		*INSTANCES;
} CLI_DRIVER_BUNDLE, * CLI_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif


int32_t cli_write_data(CLI_INSTANCE_PTR cli_instance, uint8_t* data, uint32_t count);
int32_t cli_purge_command(CLI_INSTANCE_PTR cli_instance);
int32_t cli_read_command(CLI_INSTANCE_PTR cli_instance);
int32_t cli_read_token(CLI_INSTANCE_PTR cli_instance);


#ifdef __cplusplus
}
#endif

#endif //__CLI_PRIVATE_H__
