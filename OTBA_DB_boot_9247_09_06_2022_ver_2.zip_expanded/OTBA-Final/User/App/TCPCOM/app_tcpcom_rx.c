/**
 ******************************************************************************
 * @file    app_tcpcom.c
 * @brief   OTS Application TCP communication public functions implementations
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
#include "UserLib.h"
#include "app.h"

/* Global variables */

/* Functions definitions */
extern OTBA_ST_LSR ST_LSR_CURR , ST_LSR_PREV;
static int32_t _app_tcpcom_handle_comm__phasis_control(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	TCPCOM_PACKET packet;
//	TCPCOM_PACKET packet_to_send;
//	TickType_t start_time;
//	TickType_t now_time;
	int32_t err;

//	packet_to_send.HDR.OPCODE = TCPCOM_ICD_OPCODE__OTB_STATUS;

	err = io_ioctl(OTS_APP->DRIVERS.TCPCOM, IO_IOCTL_TCPCOM__PEEK_INPUT_MESSAGE_PHASIS_CONTROL, &packet);

	if (err == IO_ERROR_TCPCOM__NO_AVAILABLE_MESSAGES)
	{
		// Do nothing
	}
	else if (err != IO_ERROR_TCPCOM__OK)
	{
		return err;
	}
	else
	{
		// New packet received, handle new packet
		TCPCOM_PACKET_BODY__PHASIS_CONTROL_PTR phasis_control = (TCPCOM_PACKET_BODY__PHASIS_CONTROL_PTR)packet.BODY;

		if (phasis_control->OTB_STATE < OTB_STATE_COUNT)
		{
			OTS_APP->TCP.phasis_control_state = phasis_control->OTB_STATE;
			OTS_APP->TCP.if_send_response_status = 1;
		}

//		if (fsm_get_active_state(OTS_APP->FSM.fsm) == OTS_APP->FSM.state__IDLE->obj || phasis_control->OTB_STATE == OTB_STATE__IDLE)
//			((TCPCOM_PACKET_BODY__STATUS_PTR)packet_to_send.BODY)->CUR_STATE = phasis_control->OTB_STATE;
//		else if(fsm_get_active_state(OTS_APP->FSM.fsm) == OTS_APP->FSM.state__OPERATE->obj)
//					((TCPCOM_PACKET_BODY__STATUS_PTR)packet_to_send.BODY)->CUR_STATE = OTB_STATE__OPERATE;
//		else if(fsm_get_active_state(OTS_APP->FSM.fsm) == OTS_APP->FSM.state__LASEROFF->obj)
//							((TCPCOM_PACKET_BODY__STATUS_PTR)packet_to_send.BODY)->CUR_STATE = OTB_STATE__LASER_OFF;
//		else if(fsm_get_active_state(OTS_APP->FSM.fsm) == OTS_APP->FSM.state__IBIT->obj)
//			((TCPCOM_PACKET_BODY__STATUS_PTR)packet_to_send.BODY)->CUR_STATE = OTB_STATE__IBIT;

//		// for DEBUG: delay
//		start_time = xTaskGetTickCount();
//		now_time = xTaskGetTickCount();
//		while((now_time - start_time) < 100)
//		{
//			ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
//			now_time = xTaskGetTickCount();
//		}

//		io_write(OTS_APP->DRIVERS.TCPCOM, (uint8_t*)&packet_to_send, 1);
	}

	return APP_ERROR_OK;
}


static int32_t _app_tcpcom_handle_comm__descrite_signal_test(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	TCPCOM_PACKET packet;
	int32_t err;

	err = io_ioctl(OTS_APP->DRIVERS.TCPCOM, IO_IOCTL_TCPCOM__PEEK_INPUT_MESSAGE_DESCRITE_SIGNAL_TEST, &packet);

	if (err == IO_ERROR_TCPCOM__NO_AVAILABLE_MESSAGES)
	{
		// Do nothing
	}
	else if (err != IO_ERROR_TCPCOM__OK)
	{
		return err;
	}
	else
	{
		// New packet received, handle new packet
		TCPCOM_PACKET_BODY__DISCRETE_RF_SIGNAL_TEST_PTR signal_test = (TCPCOM_PACKET_BODY__DISCRETE_RF_SIGNAL_TEST_PTR)packet.BODY;
		memcpy(&(OTS_APP->IBIT.test_request), signal_test, sizeof(TCPCOM_PACKET_BODY__DISCRETE_RF_SIGNAL_TEST));

		err = perform_discrete_signal_test_IBIT(OTS_APP);

		if (err != IO_ERROR_TCPCOM__OK)
			return err;
	}

	return APP_ERROR_OK;
}


/*******************************************************************************
* Function Name: app_tcpcom_handle_comm
********************************************************************************
Description: Handles TCPCOM RX
Inputs:
OTS_APPLICATION_BUNDLE_PTR - OTS Application bundle pointer
Outputs:
IO_ERROR_TCPCOM__OK - No errors
*******************************************************************************/
int32_t app_tcpcom_handle_comm(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	void * current_state = fsm_get_active_state(OTS_APP->FSM.fsm);
	int32_t err = APP_ERROR_OK;

	err = io_ioctl(OTS_APP->DRIVERS.TCPCOM, IO_IOCTL_TCPCOM__IS_CONNECTED_AC, &OTS_APP->TCP.is_connected);
	if (err != IO_ERROR_TCPCOM__OK)
	{
		return err;
	}

	if (!OTS_APP->TCP.is_connected)
	{
		return APP_ERROR_OK;
	}

	if (current_state == OTS_APP->FSM.state__IDLE->obj ||
		current_state == OTS_APP->FSM.state__LASEROFF->obj ||
		current_state == OTS_APP->FSM.state__OPERATE->obj)
	{
		// Handle state expected messages
		err = _app_tcpcom_handle_comm__phasis_control(OTS_APP);
		if (err != APP_ERROR_OK)
		{
			return err;
		}

		if(current_state == OTS_APP->FSM.state__IDLE->obj)
		{
			if(OTS_APP->TCP.phasis_control_state == OTB_STATE__LASER_OFF )
			{
				//ST_LSR_CURR = ST_LASER_OFF_RUN;
				//ST_LSR_PREV = ST_IDLE_RUN;
			}

		 }

		if(current_state == OTS_APP->FSM.state__LASEROFF->obj)
		{


			if(OTS_APP->TCP.phasis_control_state == OTB_STATE__IDLE )
			{
				//ST_LSR_CURR = ST_IDLE_RUN;
				//ST_LSR_PREV = ST_LASER_OFF_RUN;
			}
		}



	}
	else if (current_state == OTS_APP->FSM.state__IBIT->obj)
	{
		// Handle state IBIT expected messages
		err = _app_tcpcom_handle_comm__phasis_control(OTS_APP);
		if (err != APP_ERROR_OK)
		{
			return err;
		}

		err = _app_tcpcom_handle_comm__descrite_signal_test(OTS_APP);
		if (err != APP_ERROR_OK)
		{
			return err;
		}
	}
	else if (current_state == OTS_APP->FSM.state__INIT->obj)
	{
		// No messages are expected in INIT state
	}

	return APP_ERROR_OK;
}
