/**
 ******************************************************************************
 * @file    app_cli.c
 * @brief   OTS Application CLI public functions implementations
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
#include "../inc/app_cli.h"

/* Global variables */

/* Functions definitions */

static void print_rx_packet(SERCOM_PACKET_PTR packet)
{
	printf(":::CLI::: PC --> OTS\t\tOPCODE=%d; SIZE=%d\n",
		   packet->OPCODE,
		   packet->SIZE);
}

static void print_tx_packet(SERCOM_PACKET_PTR packet)
{
	printf(":::CLI::: PC <-- OTS\t\tOPCODE=%d; SIZE=%d\n",
		   packet->OPCODE,
		   packet->SIZE);
}

static int32_t send_packet(OTS_APPLICATION_BUNDLE_PTR OTS_APP, SERCOM_PACKET_PTR tx_packet)
{
	if (tx_packet == NULL)
	{
		return APP_ERROR__INVALID_CLI_PACKET;
	}

	print_tx_packet(tx_packet);
	return io_write(OTS_APP->DRIVERS.SERCOM, (uint8_t*)tx_packet, 1);
}


static GPIO_TypeDef* _app_sercom_select_port(char p)
{
	switch (p)
	{
		case 'a':
		case 'A':
		case 0x0:
			return GPIOA;
		case 'b':
		case 'B':
		case 0x1:
			return GPIOB;
		case 'c':
		case 'C':
		case 0x2:
			return GPIOC;
		case 'd':
		case 'D':
		case 0x3:
			return GPIOD;
		case 'e':
		case 'E':
		case 0x4:
			return GPIOE;
		case 'f':
		case 'F':
		case 0x5:
			return GPIOF;
		case 'g':
		case 'G':
		case 0x6:
			return GPIOG;
		case 'h':
		case 'H':
		case 0x7:
			return GPIOH;
		case 'i':
		case 'I':
		case 0x8:
			return GPIOI;
		case 'j':
		case 'J':
		case 0x9:
			return GPIOJ;
		case 'k':
		case 'K':
		case 0xA:
			return GPIOK;
		default: return NULL;
	}
}


static int32_t _app_sercom_handle_rx_packet(OTS_APPLICATION_BUNDLE_PTR OTS_APP, SERCOM_PACKET_PTR rx_packet)
{
	SERCOM_PACKET tx_packet;
	int32_t err;

	switch (rx_packet->OPCODE)
	{
		case OTS_APP_CLI_OPCODE__SPI_WRITE:
			err = io_write(OTS_APP->DRIVERS.FPGA_SPI, rx_packet->PAYLOAD, rx_packet->SIZE);
			if (err != IO_ERROR_SPI_IO__OK)
			{
				return err;
			}

			tx_packet.OPCODE = OTS_APP_CLI_OPCODE__SPI_DATA;
			tx_packet.SIZE = rx_packet->SIZE;
			err = io_read(OTS_APP->DRIVERS.FPGA_SPI, tx_packet.PAYLOAD, rx_packet->SIZE);
			if (err == IO_ERROR_SPI_IO__OK)
			{
				err = send_packet(OTS_APP, &tx_packet);
			}
			break;

		case OTS_APP_CLI_OPCODE__SPI_READ:
			tx_packet.OPCODE = OTS_APP_CLI_OPCODE__SPI_DATA;
			tx_packet.SIZE = rx_packet->PAYLOAD[0];
			err = io_read(OTS_APP->DRIVERS.FPGA_SPI, tx_packet.PAYLOAD, rx_packet->PAYLOAD[0]);
			if (err == IO_ERROR_SPI_IO__OK)
			{
				err = send_packet(OTS_APP, &tx_packet);
			}
			break;

		case OTS_APP_CLI_OPCODE__PORT_WRITE:
		{
			GPIO_TypeDef * port = _app_sercom_select_port(rx_packet->PAYLOAD[0]);
			uint16_t pin = 0x0001 << rx_packet->PAYLOAD[1];
			uint8_t value = rx_packet->PAYLOAD[2] ? GPIO_PIN_SET : GPIO_PIN_RESET;

			HAL_GPIO_WritePin(port, pin, value);
			break;
		}

		case OTS_APP_CLI_OPCODE__PORT_READ:
		{
			GPIO_TypeDef * port = _app_sercom_select_port(rx_packet->PAYLOAD[0]);
			uint16_t pin = 0x0001 << rx_packet->PAYLOAD[1];
			GPIO_PinState value = HAL_GPIO_ReadPin(port, pin);

			tx_packet.OPCODE = OTS_APP_CLI_OPCODE__PORT_DATA;
			tx_packet.SIZE = 1;
			tx_packet.PAYLOAD[0] = value;
			err = send_packet(OTS_APP, &tx_packet);
			break;
		}

		case OTS_APP_CLI_OPCODE__FPGA_REG_WRITE:
		{
#if defined(IO_IOCTL_FPGA_OTBA__WRITE_REG)
			FPGA_OTBA_RW_REG reg;
			reg.address = rx_packet->PAYLOAD[0];
			reg.value = rx_packet->PAYLOAD[1];
			err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__WRITE_REG, &reg);
			if (err != IO_ERROR_FPGA_OTBA__OK)
			{
				return err;
			}
#elif defined(IO_IOCTL_FPGA_OTBC__WRITE_REG)
			FPGA_OTBC_RW_REG reg;
			reg.address = rx_packet->PAYLOAD[0];
			reg.value = rx_packet->PAYLOAD[1];
			err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBC__WRITE_REG, &reg);
			if (err != IO_ERROR_FPGA_OTBC__OK)
			{
				return err;
			}
#endif

			err = send_packet(OTS_APP, &tx_packet);

			break;
		}

		case OTS_APP_CLI_OPCODE__FPGA_REG_READ:
		{
#if defined(IO_IOCTL_FPGA_OTBA__READ_REG)
			FPGA_OTBA_RW_REG reg;
			reg.address = rx_packet->PAYLOAD[0];
			reg.value = 0;

			xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
			err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__READ_REG, &reg);
			xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);

			//err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__READ_REG, &reg);


			if (err != IO_ERROR_FPGA_OTBA__OK)
			{
				return err;
			}
#elif defined(IO_IOCTL_FPGA_OTBC__READ_REG)
			FPGA_OTBC_RW_REG reg;
			reg.address = rx_packet->PAYLOAD[0];
			reg.value = 0;

			xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
			err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__READ_REG, &reg);
			xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);


			//err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBC__READ_REG, &reg);

			if (err != IO_ERROR_FPGA_OTBC__OK)
			{
				return err;
			}
#endif

			memcpy(&tx_packet, rx_packet, sizeof(SERCOM_PACKET));
			tx_packet.OPCODE = rx_packet->OPCODE;
			tx_packet.PAYLOAD[1] = reg.value;

			err = send_packet(OTS_APP, &tx_packet);

			break;
		}

		default:
			err = APP_ERROR__INVALID_CLI_OPCODE;
	}

	return err;
}


/*******************************************************************************
* Function Name: app_cli_handle_comm
********************************************************************************
Description: Handles CLI RX
Inputs:
OTS_APPLICATION_BUNDLE_PTR - OTS Application bundle pointer
Outputs:
APP_ERROR_OK - No errors
*******************************************************************************/
int32_t app_cli_handle_comm(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	SERCOM_PACKET rx_packet;
	int32_t err;

	err = io_read(OTS_APP->DRIVERS.SERCOM, (uint8_t*)&rx_packet, 1);
	if (err == IO_ERROR_SERCOM__OK)
	{
		print_rx_packet(&rx_packet);
		return _app_sercom_handle_rx_packet(OTS_APP, &rx_packet);
	}
	else if (err == IO_ERROR_SERCOM__PACKET_NOT_READY)
	{
		return APP_ERROR_OK;
	}
	else
	{
		return err;
	}
}
