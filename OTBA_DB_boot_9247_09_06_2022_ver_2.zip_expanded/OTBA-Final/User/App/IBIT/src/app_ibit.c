/**
 ******************************************************************************
 * @file    app_ibit.c
 * @brief   OTBA Application IBIT operations implementations
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

static int32_t perform_RF_test_IBIT(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	OTS_APP->IBIT.ibit_state = IBIT_STATE__RF1;
	return APP_ERROR_OK;
}

static int32_t start_discrete_test_IBIT(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	OTS_APP->IBIT.ibit_state = IBIT_STATE__DISCRETE_START1;
	return APP_ERROR_OK;
}

static int32_t end_discrete_test_IBIT(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	OTS_APP->IBIT.ibit_state = IBIT_STATE__DISCRETE_END;
	return APP_ERROR_OK;
}

int32_t perform_discrete_signal_test_IBIT(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	int32_t err;

	TCPCOM_PACKET_BODY__DISCRETE_RF_SIGNAL_TEST_PTR signal_test = &(OTS_APP->IBIT.test_request);
	if (OTS_APP->IBIT.ibit_state != IBIT_STATE__IDLE) // IBIT test is already running
		return IO_ERROR_TCPCOM__BAD_REQUEST;
	if (signal_test->RF)
		err = perform_RF_test_IBIT(OTS_APP);
	else if (signal_test->OTB_DISC_SIG_TEST_PROG == TEST_START)
		err = start_discrete_test_IBIT(OTS_APP);
	else if (signal_test->OTB_DISC_SIG_TEST_PROG == TEST_END)
		err = end_discrete_test_IBIT(OTS_APP);
	else
		return IO_ERROR_TCPCOM__BAD_REQUEST;

	if (APP_ERROR_OK != err)
		return err;

	return APP_ERROR_OK;
}

static int32_t FPGA_set_dis_test_start_state(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	int32_t err;
	FPGA_OTBA_RW_REG request;

	request.address = FPGA_OTBA_REG__LBJP_DISC_SET_CONTROL; // LBJP discrete control register (25)
	request.value = 0xDF; // 1101 1111: D6 & D7 remain 1; D0-D4 are now 1


	xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__WRITE_REG, &request);
	xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);
//	if (ERROR_OK != err)
//		return err;
//
//	request.address = FPGA_OTBA_REG__TRDS_DISC_SET_CONTROL; // TRDS discrete control register (27)
//	request.value = 0x40;
//
//	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__WRITE_REG, &request);

	return err;
}

static int32_t FPGA_set_dis_test_stop_state(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	int32_t err;
	FPGA_OTBA_RW_REG request;

	request.address = FPGA_OTBA_REG__LBJP_DISC_SET_CONTROL;
	request.value = 0xC0;

	xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__WRITE_REG, &request);
	xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);
	if (ERROR_OK != err)
		return err;

	request.address = FPGA_OTBA_REG__TRDS_DISC_SET_CONTROL;
	request.value = 0x40;

	xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__WRITE_REG, &request);
	xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);
	return err;
}

static int32_t write_output_discretes(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	int32_t err;
	FPGA_OTBA_RW_REG value, control;

	value.address = FPGA_OTBA_REG__LBJP_DISC_SET_VALUE;
	value.value = 0;

	control.address = FPGA_OTBA_REG__LBJP_DISC_SET_CONTROL;
	control.value = 0;

// structure refer to HW-SW ICD OTS Rev 4.0 (as of 06/12/2020) (HWSW ICD-OTBA-4.0) - registers 25 + 26

	if (OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TOBEUTEST.LBJP_RW_LB == 1)
	{
		control.value += 2;
		if(OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TEST.LBJP_RW_LB == 1)
			value.value += 2;

		else if(OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TEST.LBJP_RW_LB != 0)
			return APP_ERROR__IBIT_STATE_ERROR;
	}

	if (OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TOBEUTEST.LBJP_RW_MB == 1)
	{
		control.value += 1;
		if(OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TEST.LBJP_RW_MB == 1)
			value.value += 1;

		else if(OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TEST.LBJP_RW_MB != 0)
			return APP_ERROR__IBIT_STATE_ERROR;
	}

	if (OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TOBEUTEST.LBJP_TR_LB == 1)
	{
		control.value += 8;
		if(OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TEST.LBJP_TR_LB == 1)
			value.value += 8;

		else if(OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TEST.LBJP_TR_LB != 0)
			return APP_ERROR__IBIT_STATE_ERROR;
	}

	if (OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TOBEUTEST.LBJP_TR_MB == 1)
	{
		control.value += 4;
		if(OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TEST.LBJP_TR_MB == 1)
			value.value += 4;

		else if(OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TEST.LBJP_TR_MB != 0)
			return APP_ERROR__IBIT_STATE_ERROR;
	}

	if (OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TOBEUTEST.RSV2OTB == 1)
	{
		control.value += 16;
		if(OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TEST.RSV2OTB == 1)
			value.value += 16;

		else if(OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TEST.RSV2OTB != 0)
			return APP_ERROR__IBIT_STATE_ERROR;
	}

	printf("---IBIT TEST--- FPGA register 0x%02X written with value 0x%02X\n\r", value.address, value.value);

	xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__WRITE_REG, &value); //register 26
	xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);
	if (ERROR_OK != err)
		return err;

	control.value = 0x5F; // override -  D7 is now 0; D0-D4 & D6 are 1;  //register 25

	printf("---IBIT TEST--- FPGA register 0x%02X written with value 0x%02X\n\r", control.address, control.value);
	xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__WRITE_REG, &control);
	xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);
	return err;
}


static int32_t read_input_discretes(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	int32_t err;
	int32_t res;
	// res will get 3 registers: (MS) {[0], [REG3]FPGA_OTBA_REG__DISC_AC_2_OTBB_REG2, [REG2]FPGA_OTBA_REG__DISC_AC_2_OTBB_REG1, [REG4]FPGA_OTBA_REG__DISC_AC_2_OTBC_REG} (LS)
//	void *dis_sig_in = &(res);


    xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__READ_INPUT_DISCRETES, &res);
	xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);

	// parsing: values refer to HW-SW ICD OTS Rev 4.0 (as of 06/12/2020) (HWSW ICD-OTBA-4.0)
	OTS_APP->IBIT.test_report.OTB_DIS_SIG_MONI_VAL.RSV1OTB = (res & 0x10000) >> 16;//D16
	OTS_APP->IBIT.test_report.OTB_DIS_SIG_MONI_VAL.TRDS_T1_TE = (res & 0x100) >> 8;//D8
	OTS_APP->IBIT.test_report.OTB_DIS_SIG_MONI_VAL.TRDS_T2_TE = (res & 0x200) >> 9;//D9
	OTS_APP->IBIT.test_report.OTB_DIS_SIG_MONI_VAL.TRDS_TI = (res & 0x400) >> 10;//D10
	OTS_APP->IBIT.test_report.OTB_DIS_SIG_MONI_VAL.TRDS_TOP_GIC = (res & 0x800) >> 11;//D11
	OTS_APP->IBIT.test_report.OTB_DIS_SIG_MONI_VAL.TRDS_SPARE = (res & 0x1000) >> 12;//D12
	OTS_APP->IBIT.test_report.OTB_DIS_SIG_MONI_VAL.LBJP_RI_LB = (res & 0x4) >> 2;//D2
	OTS_APP->IBIT.test_report.OTB_DIS_SIG_MONI_VAL.LBJP_RI_MB = (res & 0x8) >> 3;//D3
	OTS_APP->IBIT.test_report.OTB_DIS_SIG_MONI_VAL.LBJP_TI_LB = (res & 0x1);//D0
	OTS_APP->IBIT.test_report.OTB_DIS_SIG_MONI_VAL.LBJP_TI_MB = (res & 0x2) >> 1;//D1
	OTS_APP->IBIT.test_report.OTB_DIS_SIG_MONI_VAL.CDFRALBJP = (res & 0x10) >> 4;//D4

	return err;
}


static int32_t progress_test_IBIT(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	int32_t err;
	FPGA_OTBA_RW_REG reg_request;
//	TickType_t start_time;
//	TickType_t now_time;

	switch (OTS_APP->IBIT.ibit_state)
	{
	case IBIT_STATE__RF1:
		OTS_APP->IBIT.ibit_state = IBIT_STATE__RF2;
		break;
	case IBIT_STATE__RF2:
		OTS_APP->IBIT.ibit_state = IBIT_STATE__RF3;
		break;
	case IBIT_STATE__RF3:
		// restore control registers (25 + 27) to IDLE mode
		err = FPGA_set_dis_test_stop_state(OTS_APP);
		if (ERROR_OK != err)
			return err;

		memset(&(OTS_APP->IBIT.test_report), 0, sizeof(OTS_APP->IBIT.test_report));
		OTS_APP->IBIT.test_report.OTB_DISC_SIG_TEST_PROG = 0;
		OTS_APP->IBIT.test_report.RF_STATUS = 1;
		OTS_APP->IBIT.test_report.RF_LEVEL = -90; //GM temp: dummy valid value, until we can read the real value.
		OTS_APP->IBIT.ibit_state = IBIT_STATE__COMPLETE;
		OTS_APP->IBIT.test_report.OTB_DISC_SIG_TEST_PROG = 1;
		break;
	case IBIT_STATE__DISCRETE_START1:
		OTS_APP->IBIT.ibit_state = IBIT_STATE__DISCRETE_START2;
		break;
	case IBIT_STATE__DISCRETE_START2:
		OTS_APP->IBIT.ibit_state = IBIT_STATE__DISCRETE_START3;
		break;
	case IBIT_STATE__DISCRETE_START3:
		if(OTS_APP->IBIT.is_test_in_progress) return ERROR_OK;
		OTS_APP->IBIT.is_test_in_progress = 1;

		err = FPGA_set_dis_test_start_state(OTS_APP);
		if (ERROR_OK != err)
		{
			OTS_APP->IBIT.is_test_in_progress = 0;
			return err;
		}

		memset(&(OTS_APP->IBIT.test_report), 0, sizeof(OTS_APP->IBIT.test_report));

		err = write_output_discretes(OTS_APP);
		if (ERROR_OK != err)
		{
			OTS_APP->IBIT.is_test_in_progress = 0;
			return err;
		}

		// for DEBUG: delay
//		osDelay(5);

		err = read_input_discretes(OTS_APP);
		if (ERROR_OK != err)
		{
			OTS_APP->IBIT.is_test_in_progress = 0;
			return err;
		}

		// Echo the discretes under test values to the test report:
		memcpy(&(OTS_APP->IBIT.test_report.OTB_SIG_UNDER_TEST), &(OTS_APP->IBIT.test_request.OTB_O_DISC_SIG_TEST), sizeof(LBJP_DISCRETE_TEST));
		OTS_APP->IBIT.ibit_state = IBIT_STATE__COMPLETE;
		OTS_APP->IBIT.test_report.OTB_DISC_SIG_TEST_PROG = 1;

		OTS_APP->IBIT.is_test_in_progress = 0;
		break;

	case IBIT_STATE__DISCRETE_END:
		// restore control registers (25 + 27) to IDLE mode
		err = FPGA_set_dis_test_stop_state(OTS_APP);
		if (ERROR_OK != err)
			return err;

		// reset data register (26) to all 0
		reg_request.address = FPGA_OTBA_REG__LBJP_DISC_SET_VALUE;
		reg_request.value = 0;

		xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
		err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__WRITE_REG, &reg_request);
		xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);

		if(ERROR_OK != err)
			return err;

		memset(&(OTS_APP->IBIT.test_report), 0, sizeof(OTS_APP->IBIT.test_report));
		OTS_APP->IBIT.ibit_state = IBIT_STATE__COMPLETE;
		break;

	default:
		return APP_ERROR__IBIT_STATE_ERROR;
	}

	return ERROR_OK;
}


int32_t perform_IBIT_if_necessary(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	int32_t err;

	// Perform ongoing IBIT operations, or send the IBIT result:
	if (OTS_APP->IBIT.ibit_state != IBIT_STATE__IDLE)
	{
		if (IBIT_STATE__COMPLETE == OTS_APP->IBIT.ibit_state)
		{	// Prepare the report and send the response to SPECTRA:
			TCPCOM_PACKET packet_to_send;

			memcpy(&(packet_to_send.BODY), &(OTS_APP->IBIT.test_report), sizeof(TCPCOM_PACKET_BODY__DISCRETE_TEST_REPORT));
			packet_to_send.HDR.OPCODE = TCPCOM_ICD_OPCODE__OTB_DESCRITE_SIGNAL_REPORT;
			err = io_write(OTS_APP->DRIVERS.TCPCOM, (uint8_t*)&packet_to_send, 1);
			if (APP_ERROR_OK != err)
			{
				return err;
			}

			OTS_APP->IBIT.ibit_state = IBIT_STATE__IDLE;
		}
		else
		{
			err = progress_test_IBIT(OTS_APP); // Perform the next step in the IBIT operarion
			if (APP_ERROR_OK != err)
			{
				return err;
			}
		}
	}

	return APP_ERROR_OK;
}
