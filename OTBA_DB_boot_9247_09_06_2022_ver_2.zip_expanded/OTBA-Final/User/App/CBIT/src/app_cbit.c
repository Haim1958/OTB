/**
 ******************************************************************************
 * @file    app_cbit.c
 * @brief   OTBA Application CBIT operations implementations
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



typedef enum {
	                OPER_STATE__OFF,
					OPER_STATE__SEEK,
					OPER_STATE__LOCK,
				} OPMOD_OPER_STATE ;

	typedef struct {
		OPMOD_OPER_STATE 		state;
		int32_t					curr_pwr;
		int32_t					curr_error;
		int32_t 				duration;					// duration of operation since start or set
		int32_t 				seek_duration;				// duration of current SEEK state
	} *OP_MOD_OPER_STATUS_PTR  ;


OPTMOD_STRUCT_PTR OPTMOD;
static int32_t perform_CBIT__OPTMOD(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{

	//if(OPTMOD->OPER.STATUS.state)
	  OPTMOD = (OPTMOD_STRUCT_PTR)OPTMOD_GLB;
	// Handle LONG_SEEK condition
		if (OPTMOD->OPER.STATUS.state == OPTMOD_OPER_STATE__SEEK &&
				OPTMOD->OPER.STATUS.seek_duration > CBIT_OPTMOD__LONG_SEEK_DURATION_TICKS &&
			!OTS_APP->CBIT.optics_long_seek_error_sent)
		{
			//printf("RF Optics UNLOCKED\r\n");
			OTS_APP->CBIT.optics_long_seek_error_sent = 1u;
		}
		else if (OPTMOD->OPER.STATUS.state != OPTMOD_OPER_STATE__SEEK)
		{
			OTS_APP->CBIT.optics_long_seek_error_sent = 0u;
		}

		// Handle LOW_POWER condition
		if (OPTMOD->OPER.STATUS.curr_pwr < CBIT_OPTMOD__LOW_POWER_THRESHOLD &&
			!OTS_APP->CBIT.optics_low_power_error_sent)
		{
		//	printf("RF Optics DISCONNECTED\r\n");
			OTS_APP->CBIT.optics_low_power_error_sent = 1u;
		}
		else if (OPTMOD->OPER.STATUS.curr_pwr > CBIT_OPTMOD__LOW_POWER_THRESHOLD)
		{
			OTS_APP->CBIT.optics_low_power_error_sent = 0u;
		}
     // }
		return APP_ERROR_OK;
}

static int32_t perform_CBIT(OTS_APPLICATION_BUNDLE_PTR OTS_APP, TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR test_result)
{
	int32_t err;
	FPGA_OTBA_RW_REG fpga_reg;



	Update_OTB_test_Staus ( OTS_APP,test_result);


	return ERROR_OK;
}
/*
if (OTBC_Ethernet!=0)
		{
//	read FPGA register 6 for discrete errors
	fpga_reg.value = 0x00;
	fpga_reg.address = FPGA_OTBA_REG__SERDES_SFP_STAT;

	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__READ_REG, &fpga_reg);
	if (IO_ERROR_FPGA_OTBA__OK != err)
	{
		return err;
	}

//	check if d6 or d7 are 1
	// rev 1.11 check if d6 ==1  haim 29/08/2021
	if (fpga_reg.value & 0x40)
	{
	//	LB temp: report in NC_PFL and MFL1 - OPTC_PW_LVL_OTB_A_C2_2 and OPTC_PW_LVL_OTB_A_C1_2
		test_result->OTB_NON_CRITICAL_PFL = 0x280;
		test_result->OTB_MFL_1 = 0x28;
	}

}


	if(OTS_APP->OTBBCOM.otbb_disc_rx_err)
	{
		//	LB temp: report in NC_PFL and MFL1 - OPTC_PW_LVL_OTB_A_B2_2 and OPTC_PW_LVL_OTB_A_B1_2
							test_result->OTB_NON_CRITICAL_PFL |=(BIT_13 |BIT_11);
							test_result->OTB_NON_CRITICAL_PFL &= !OTB_NON_CRITICAL_PFL_STATUS;
							test_result->OTB_MFL_1 |=(BIT_7| BIT_9) ;
							test_result->OTB_MFL_1 &=!OTB_NON_CRITICAL_PFL_STATUS;
							OTS_APP->OTBBCOM.otbb_disc_rx_err=0;
	}


	return ERROR_OK;
}
*/

static void prepare_otb_test_result_body(TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR cbit_result, OTB_STATE state)
{
	cbit_result->OTB_CURRENT_STATE = state;
	cbit_result->OTB_EQUIPMENT_NAME[0] = 'O';
	cbit_result->OTB_EQUIPMENT_NAME[1] = 'T';
	cbit_result->OTB_EQUIPMENT_NAME[2] = 'B';
	cbit_result->OTB_EQUIPMENT_NAME[3] = 'A';
	memcpy(cbit_result->OTB_SOFTWARE_REFERENCE, APPLICATION_SOFTWARE_VERSION, sizeof(APPLICATION_SOFTWARE_VERSION));
	memcpy(cbit_result->OTB_HARDWARE_REFERENCE, FPGA_HARDWARE_VERSION, sizeof(FPGA_HARDWARE_VERSION));
}


int32_t perform_CBIT_if_necessary(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
	int32_t err;

	// If it's time, perform CBIT again, and send the results to SPECTRA:
	if (100 != OTS_APP->CBIT.cbit_counter) // This means 1 second has passed.
	{
		return APP_ERROR_OK;
	}

	OTS_APP->CBIT.cbit_counter = 0;
	OTS_APP->OTBBCOM.otbb_err_counter++;
	err = perform_CBIT__OPTMOD(OTS_APP);
	if (err != APP_ERROR_OK)
	{
		return err;
	}

	TCPCOM_PACKET packet_to_send;
	TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR cbit_result = (TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR)packet_to_send.BODY;

	prepare_otb_test_result_body(cbit_result, get_active_state(&(OTS_APP->FSM)));

	err = perform_CBIT(OTS_APP, cbit_result);
	if (APP_ERROR_OK != err)
	{
		return err; //GM TODO: is this the right error handling?
	}

	// Get FPGA HW version
	uint8_t fpga_rev = 0;
	io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__GET_HW_REV, &fpga_rev);
	fpga_rev = fpga_rev != 0 ? (int)fpga_rev : 1;
	char charBuffer[2];
	sprintf(charBuffer, "%x", fpga_rev);
	cbit_result->OTB_HARDWARE_REFERENCE[0] = toupper(charBuffer[0]); // Write to outgoing cBit
	cbit_result->OTB_HARDWARE_REFERENCE[1] = toupper(charBuffer[1]);

	packet_to_send.HDR.OPCODE = TCPCOM_ICD_OPCODE__OTB_TEST_RESULT;

	err = io_ioctl(OTS_APP->DRIVERS.TCPCOM, IO_IOCTL_TCPCOM__IS_CONNECTED_AC, &OTS_APP->TCP.is_connected);
	if (err != IO_ERROR_TCPCOM__OK)
	{
		return err;
	}

	if (!OTS_APP->TCP.is_connected)
	{
		return APP_ERROR_OK;
	}

	return io_write(OTS_APP->DRIVERS.TCPCOM, (uint8_t*)&packet_to_send, 1);
}
