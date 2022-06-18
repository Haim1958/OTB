/**
 ******************************************************************************
 * @file    app_otbbcom.c
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
#define test_telnt 0
extern SemaphoreHandle_t xSemaphore_OTBA_RD_WR_FPGA;
extern SemaphoreHandle_t xSemaphore_OTBB_A_COM_CONNECT;

static int32_t cnt_get_reg_1 = 0;

char buf[25];
static int32_t _app_otbbcom_handle_packet(OTS_APPLICATION_BUNDLE_PTR OTS_APP, OTBBCOM_PACKET_PTR rx_packet)
{
	uint8_t telnet_connected;

	// TODO
	switch (rx_packet->HDR.OPCODE)
	{
		case OTBBCOM_OPCODE__SERDES_SFP_STATUS_ERROR:
		//{
			//int32_t err;
			//FPGA_OTBA_RW_REG request;

			OTS_APP->OTBBCOM.otbb_reg23_value = rx_packet->BODY[0];
						OTS_APP->OTBBCOM.otbb_reg23_value_received = 1;
            /***
			#if test_telnt

						io_ioctl(OTS_APP->DRIVERS.TELNET, IO_IOCTL_TELNET__IS_CONNECTED, &telnet_connected);
						if(cnt_get_reg_1++% 700 == 0)
				        {
							cnt_get_reg_1 = 0;
						   io_ioctl(OTS_APP->DRIVERS.TELNET, IO_IOCTL_TELNET__IS_CONNECTED, &telnet_connected);
						   if (telnet_connected)
						   {
							 snprintf(buf, 25-1, "GET_DATA_FROM_B= 0x%x \r\n",OTS_APP->OTBBCOM.otbb_reg23_value);
							 io_write(OTS_APP->DRIVERS.TELNET, (uint8_t*)buf, strlen(buf));
						   }
						}
			#endif
            ***/
						break;

            /*
			xSemaphoreTake( xSemaphore_OTBA_RD_WR_FPGA, ( TickType_t ) pdMS_TO_TICKS (0));
			request.value = 0xA5;
			request.address = FPGA_OTBA_REG__SERDES2_OTBB_ERROR_STATUS;

			OTS_APP->OTBBCOM.otbb_disc_rx_err = 1;

			err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__WRITE_REG, &request);
			xSemaphoreGive( xSemaphore_OTBA_RD_WR_FPGA);

			if (IO_ERROR_FPGA_OTBA__OK != err)
			{
				return err;
			}
			break;
		}


		*/
		case OTBBCOM_OPCODE__FPGA_REG_23_VALUE:

			OTS_APP->OTBBCOM.otbb_reg23_value = rx_packet->BODY[0];
			OTS_APP->OTBBCOM.otbb_reg23_value_received = 1;

#if test_telnt

			io_ioctl(OTS_APP->DRIVERS.TELNET, IO_IOCTL_TELNET__IS_CONNECTED, &telnet_connected);
			if(cnt_get_reg_1++% 700 == 0)
	        {
				cnt_get_reg_1 = 0;
			   io_ioctl(OTS_APP->DRIVERS.TELNET, IO_IOCTL_TELNET__IS_CONNECTED, &telnet_connected);
			   if (telnet_connected)
			   {
				 snprintf(buf, 25-1, "GET_DATA_FROM_B= 0x%x \r\n",OTS_APP->OTBBCOM.otbb_reg23_value);
				 io_write(OTS_APP->DRIVERS.TELNET, (uint8_t*)buf, strlen(buf));
			   }
			}
#endif

			break;

		default:
			break;
	}

	////printf(":::APP_OTBBCOM::: OTBA <-- OTBB ::: %s\r\n", (char*)rx_packet->BODY);

	return APP_ERROR_OK;
}


/*******************************************************************************
* Function Name: app_otbbcom_handle_comm
********************************************************************************
Description: Handles OTBBCOM RX
Inputs:
OTS_APPLICATION_BUNDLE_PTR - OTS Application bundle pointer
Outputs:
IO_ERROR_OTBBCOM__OK - No errors
*******************************************************************************/
int32_t app_otbbcom_handle_comm(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	OTBBCOM_PACKET rx_packet;
	int32_t err = APP_ERROR_OK;
	 if ((OTBB1_Ethernet!=Ethernet_OK) &&  (OTBB2_Ethernet!=Ethernet_OK))
	 {
		 return 0;
	 }

	 //xSemaphoreTake( xSemaphore_OTBB_A_COM_CONNECT, ( TickType_t ) pdMS_TO_TICKS (0));
	err = io_ioctl(OTS_APP->DRIVERS.OTBBCOM, IO_IOCTL_OTBBCOM__IS_CONNECTED,
											&OTS_APP->OTBBCOM.is_connected);
	//xSemaphoreGive(xSemaphore_OTBB_A_COM_CONNECT);

	if (err != IO_ERROR_OTBBCOM__OK)
	{
		return err;
	}

	if (!OTS_APP->OTBBCOM.is_connected)
	{
		return APP_ERROR_OK;
	}

	err = io_read(OTS_APP->DRIVERS.OTBBCOM, (uint8_t*)&rx_packet, 1);
	if (err == IO_ERROR_OTBBCOM__NO_AVAILABLE_MESSAGES)
	{
		return APP_ERROR_OK;
	}
	else if (err != IO_ERROR_OTBBCOM__OK)
	{
		return err;
	}

	return _app_otbbcom_handle_packet(OTS_APP, &rx_packet);
}
