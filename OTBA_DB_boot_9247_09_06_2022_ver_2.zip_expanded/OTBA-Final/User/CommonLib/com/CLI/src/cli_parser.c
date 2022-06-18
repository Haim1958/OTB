/**
 ******************************************************************************
 * @file    cli_parser.c
 * @brief   OTS CLI Parser public functions implementations
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

/* Includes */
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "UserLib.h"
#include "../inc/cli_private.h"

/* Global variables */

static const char* _CLI_DELIM = " ,";
static const char* _CLI_EMPTY_LINE = " ";

/* Functions declarations */


/* Functions definitions */



static int32_t cli_write_char(CLI_INSTANCE_PTR cli_instance, char c)
{
	if (cli_instance->g.write_index >= (CLI_LINE_MAX_LENGTH))
	{
		cli_instance->g.write_index = 0;
	}
	else if (c == '\r' || c == '\n')
	{
		cli_instance->g.command = NULL;

		if (cli_instance->g.write_index)
		{
			// Non-empty line is ready to be handled
			cli_instance->g.write_buffer[cli_instance->g.write_index] = '\0';

			strncpy(cli_instance->g.read_line, (char*)cli_instance->g.write_buffer, CLI_LINE_MAX_LENGTH);

			cli_instance->g.write_index = 0;
		}
		else
		{
			// special case - empty command
			strncpy(cli_instance->g.read_line, _CLI_EMPTY_LINE, CLI_LINE_MAX_LENGTH);
		}
	}
	else if (isalnum(c) || isspace(c) || ispunct(c))
	{
		// valid character, buffer it
		cli_instance->g.write_buffer[cli_instance->g.write_index++] = c;
	}
	else
	{
		// Invalid character, restart line buffering and parsing
		cli_instance->g.write_index = 0;
	}

	return IO_ERROR_CLI__OK;
}


int32_t cli_write_data(CLI_INSTANCE_PTR cli_instance, uint8_t* data, uint32_t count)
{
	uint32_t i;

	if (data == NULL)
	{
		return IO_ERROR_CLI__NULL;
	}

	for (i = 0; i < count; i++)
	{
		int32_t err = cli_write_char(cli_instance, (char)data[i]);
		if (err != IO_ERROR_CLI__OK)
		{
			return err;
		}
	}

	return IO_ERROR_CLI__OK;
}


int32_t cli_purge_command(CLI_INSTANCE_PTR cli_instance)
{
	cli_instance->g.command = NULL;
	cli_instance->g.token = NULL;
	cli_instance->g.read_line[0] = '\0';		// empty the line

	return IO_ERROR_CLI__OK;
}


int32_t cli_read_command(CLI_INSTANCE_PTR cli_instance)
{
	if (cli_instance->g.command != NULL)
	{
		return IO_ERROR_CLI__COMMAND_ALREADY_READ;
	}

	if (strlen(cli_instance->g.read_line) == 0)
	{
		return IO_ERROR_CLI__COMMAND_NOT_READY;
	}

	cli_instance->g.token = NULL;

	if (strcmp(cli_instance->g.read_line, _CLI_EMPTY_LINE) == 0)
	{
		cli_instance->g.command = "";
		return IO_ERROR_CLI__OK;
	}

	cli_instance->g.command = strtok_r(
			cli_instance->g.read_line,
			_CLI_DELIM,
			&cli_instance->g.strtok_internal); // first token must be a command

	if (cli_instance->g.command == NULL)
	{
		return IO_ERROR_CLI__COMMAND_NOT_READY;
	}

	return IO_ERROR_CLI__OK;
}


int32_t cli_read_token(CLI_INSTANCE_PTR cli_instance)
{
	cli_instance->g.token = NULL;

	if (cli_instance->g.command == NULL)
	{
		return IO_ERROR_CLI__COMMAND_NOT_READY;
	}

	cli_instance->g.token = strtok_r(
			NULL,
			_CLI_DELIM,
			&cli_instance->g.strtok_internal);

	if (cli_instance->g.token == NULL)
	{
		return IO_ERROR_CLI__NO_MORE_TOKENS_FOUND;
	}

	return IO_ERROR_CLI__OK;
}

