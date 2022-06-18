/**
 ******************************************************************************
 * @file    app_sm_states.c
 * @brief   Application Main State machine states implementations
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

#include <string.h>
#include "UserLib.h"
#include "app.h"
#include "../inc/app_sm_private.h"


/*! \brief Implementation of the initialization of INIT state.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */

   typedef enum {
					OPMOD_OPER_STATE__OFF,
					OPMOD_OPER_STATE__SEEK,
					OPMOD_OPER_STATE__LOCK,
				} OPMOD_OPER_STATE ;

	typedef struct {
		OPMOD_OPER_STATE 		state;
		int32_t					curr_pwr;
		int32_t					curr_error;
		int32_t 				duration;					// duration of operation since start or set
		int32_t 				seek_duration;				// duration of current SEEK state
	} *OP_MOD_OPER_STATUS_PTR  ;


int32_t _app_sm__state_INIT__init(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;
	OPTMOD_STRUCT_PTR OPTMOD = OPTMOD_GLB;
	int32_t err;

	// Disable SFP to OTB-C
	HAL_GPIO_WritePin(P9_0_TXDIS_GPIO_Port, P9_0_TXDIS_Pin, GPIO_PIN_SET);


	//err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_INIT_CREATED,	NULL);


	err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_INIT_ADC,(IO_HANDLER_PTR)OTS_APP->DRIVERS.OPTIC_ADC);
	err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_INIT_DAC,(IO_HANDLER_PTR)OTS_APP->DRIVERS.OPTIC_DAC);
	err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_INIT,	NULL);
	//err = optmod_init(OPTMOD,OTS_APP->DRIVERS.OPTIC_ADC, OTS_APP->DRIVERS.OPTIC_DAC);
	//err = optmod_init(&OTS_APP->OPTMOD, OTS_APP->DRIVERS.OPTIC_ADC, OTS_APP->DRIVERS.OPTIC_DAC);//***
	if (err != OPTMOD_ERROR__OK)
	{
		return err;
	}
	err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_OPER_START_RE_CREATED,	NULL);
	//err = optmod_operation_start(&OTS_APP->OPTMOD);//***
	if (err != OPTMOD_ERROR__OK)
	{
		return err;
	}

	OTS_APP->CBIT.optics_init_error_sent = 0u;
    /** jump to rf_init app_init.c **/
	//..err = app_rf_init(OTS_APP, OTS_APP->DRIVERS.RF_DAC, OTS_APP->DRIVERS.CPU_TEMP);
	//..if (err != RF_ERROR__OK)
	//..{
	//..return err;
	//..}


	//err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_OPER_START_RE_CREATED,NULL);
	//if (err != RF_ERROR__OK)
	//{
	  // return err;
	//}

	return APP_ERROR_OK;
}


/*! \brief Implementation of the operation of INIT state.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__state_INIT__operation(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;

	OPTMOD_STRUCT_PTR OPTMOD = OPTMOD_GLB;

	OPTMOD_OPER_STATUS_PTR  pOPTMOD = &OPTMOD->OPER.STATUS;

	// If we are here for more than 10 seconds, send message

	io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPER_STATUS_DURATION,pOPTMOD);

	if ((pOPTMOD->duration) > CBIT_OPTMOD__LONG_SEEK_DURATION_TICKS &&
			!OTS_APP->CBIT.optics_init_error_sent)
	{
	//	printf("RF Optics UNLOCKED\r\n");
		OTS_APP->CBIT.optics_init_error_sent = 0u;
	}

	return APP_ERROR_OK;
}


static int32_t fpga_dis_sig_state_IDLE(IO_HANDLER_PTR FPGA_DRIVER)
{
	int32_t err;
	FPGA_OTBA_RW_REG request;
	request.value = 0xC0;
	request.address = FPGA_OTBA_REG__LBJP_DISC_SET_CONTROL;

	err = io_ioctl(FPGA_DRIVER, IO_IOCTL_FPGA_OTBA__WRITE_REG, &request);
	if (IO_ERROR_FPGA_OTBA__OK != err)
		return err;

	request.value = 0x40;
	request.address = FPGA_OTBA_REG__TRDS_DISC_SET_CONTROL;

	err = io_ioctl(FPGA_DRIVER, IO_IOCTL_FPGA_OTBA__WRITE_REG, &request);
	if (IO_ERROR_FPGA_OTBA__OK != err)
		return err;

	request.value = 0x00;
	request.address = FPGA_OTBA_REG__LBJP_DISC_SET_VALUE;

	err = io_ioctl(FPGA_DRIVER, IO_IOCTL_FPGA_OTBA__WRITE_REG, &request);
	return err;
}


/*! \brief Implementation of the initialization of IDLE state.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__state_IDLE__init(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;
	int32_t err;

	OTS_APP->TCP.state_init_succeeded = 0;

	// Disable SFP to OTB-C
	HAL_GPIO_WritePin(P9_0_TXDIS_GPIO_Port, P9_0_TXDIS_Pin, GPIO_PIN_SET);


	err = fpga_dis_sig_state_IDLE(OTS_APP->DRIVERS.FPGA);
	if (IO_ERROR_FPGA_OTBA__OK != err)
	{
		return err;
	}

	if (OTB_STATE_COUNT == OTS_APP->TCP.phasis_control_state) // This means the state transition was not a result of SPECTRA request, but the end if INIT state.
	{
		OTS_APP->TCP.if_send_response_status = 1;
	}

	OTS_APP->TCP.state_init_succeeded = 1;

	return APP_ERROR_OK;
}


/*! \brief Implementation of the operation of IDLE state.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
// goto operation if CBIT and tcpcom is OK
int32_t _app_sm__state_IDLE__operation(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;
	int32_t err;

	if (OTS_APP->TCP.state_init_succeeded)
	{
		if (OTS_APP->TCP.if_send_response_status)
		{
			OTS_APP->TCP.if_send_response_status = 0;

			// send status
			TCPCOM_PACKET packet_to_send;
			packet_to_send.HDR.OPCODE = TCPCOM_ICD_OPCODE__OTB_STATUS;
			((TCPCOM_PACKET_BODY__STATUS_PTR)packet_to_send.BODY)->CUR_STATE = OTB_STATE__IDLE;
			io_write(OTS_APP->DRIVERS.TCPCOM, (uint8_t*)&packet_to_send, 1);
		}
	}
	else
	{
		return APP_ERROR_OK;
	}

	err = app_tcpcom_handle_comm(OTS_APP);
	if (err != APP_ERROR_OK)
		return err;

	err = perform_CBIT_if_necessary(OTS_APP);
	if (err != APP_ERROR_OK)
		return err;



//	The requirement to communicate with OTBB has been canceled

	err = app_otbbcom_handle_comm(OTS_APP);
	//if (err != APP_ERROR_OK)
//			return err;

	return APP_ERROR_OK;
}


/*! \brief Implementation of the initialization of IBIT state.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__state_IBIT__init(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;
//	int32_t err;

	OTS_APP->TCP.state_init_succeeded = 0;

//	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__SET_STATE, FPGA_State__IBIT);
//	if (err != IO_ERROR_FPGA_OTBA__OK)
//	{
//		return err;
//	}

	OTS_APP->IBIT.is_test_in_progress = 0;

	OTS_APP->TCP.state_init_succeeded = 1;

	return APP_ERROR_OK;
}


/*! \brief Implementation of the operation of IBIT state.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__state_IBIT__operation(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;
	int32_t err;
	err = perform_IBIT_if_necessary(OTS_APP);
	if (OTS_APP->TCP.state_init_succeeded)
	{
		if (OTS_APP->TCP.if_send_response_status)
		{
			OTS_APP->TCP.if_send_response_status = 0;

			// send status
			TCPCOM_PACKET packet_to_send;
			packet_to_send.HDR.OPCODE = TCPCOM_ICD_OPCODE__OTB_STATUS;
			((TCPCOM_PACKET_BODY__STATUS_PTR)packet_to_send.BODY)->CUR_STATE = OTB_STATE__IBIT;
			io_write(OTS_APP->DRIVERS.TCPCOM, (uint8_t*)&packet_to_send, 1);
		}
	}
	else
	{
		OTS_APP->TCP.phasis_control_state = OTB_STATE__IDLE;
		return APP_ERROR_OK;
	}

	err = app_tcpcom_handle_comm(OTS_APP);
	if (err != APP_ERROR_OK)
	{
		return err;
	}

	err = perform_CBIT_if_necessary(OTS_APP);
	if (err != APP_ERROR_OK)
		return err;

	err = perform_IBIT_if_necessary(OTS_APP);
	if (err != APP_ERROR_OK)
		return err;

//	Requirement to communicate with OTBB has been canceled
	err = app_otbbcom_handle_comm(OTS_APP);
//	if (err != APP_ERROR_OK)
//			return err;



	return APP_ERROR_OK;
}


static int32_t fpga_dis_sig_state_OPERATE(IO_HANDLER_PTR FPGA_DRIVER)
{
	int32_t err;
	uint8_t i, res;
	FPGA_OTBA_RW_REG request;

	for(i = 0; i < 3; i++)
	{
		res = 0x00;
		request.value = 0x00;
		request.address = FPGA_OTBA_REG__TRDS_DISC_SET_CONTROL;

		err = io_ioctl(FPGA_DRIVER, IO_IOCTL_FPGA_OTBA__WRITE_REG, &request);
		if (IO_ERROR_FPGA_OTBA__OK != err)
			return err;

		request.value = 0x01;
		err = io_ioctl(FPGA_DRIVER, IO_IOCTL_FPGA_OTBA__READ_REG, &request);
		if (IO_ERROR_FPGA_OTBA__OK != err)
		{
			return err;
		}

		res |= request.value;

		request.address = FPGA_OTBA_REG__LBJP_DISC_SET_CONTROL;


		request.value = 0x00;
		err = io_ioctl(FPGA_DRIVER, IO_IOCTL_FPGA_OTBA__WRITE_REG, &request);
		if (IO_ERROR_FPGA_OTBA__OK != err)
		{
			return err;
		}

		request.value = 0x01;
		err = io_ioctl(FPGA_DRIVER, IO_IOCTL_FPGA_OTBA__READ_REG, &request);
		if (IO_ERROR_FPGA_OTBA__OK != err)
		{
			return err;
		}

		res |= request.value;

		if(res == 0x00) break;
	}

	if(res != 0x00) err = IO_ERROR_FPGA_OTBA__STATE_CHANGE_FAILED;
	return err;
}


/*! \brief Implementation of the initialization of OPERATION state.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__state_OPERATE__init(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;
	int32_t err;

	OTS_APP->TCP.state_init_succeeded = 0;


	/** disable RF***/

	//err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_OPER_TERMNT_RE_DELETED,NULL);
	err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_OPER_START_RE_CREATED,NULL);

	err = fpga_dis_sig_state_OPERATE(OTS_APP->DRIVERS.FPGA);
	if(err == IO_ERROR_FPGA_OTBA__STATE_CHANGE_FAILED)
	{
		//LB: failed to write make_discrete_trransparent to FPGA
		//does fsm handles this by itself?
		return IO_ERROR_FPGA_OTBA__OK;
	}
	else if (IO_ERROR_FPGA_OTBA__OK != err)
	{
		return err;
	}

	// Enable SFP to OTB-C -
	HAL_GPIO_WritePin(P9_0_TXDIS_GPIO_Port, P9_0_TXDIS_Pin, GPIO_PIN_RESET);


	OTS_APP->TCP.state_init_succeeded = 1;

	return APP_ERROR_OK;
}


/*! \brief Implementation of the operation of OPERATION state.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__state_OPERATE__operation(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;
	int32_t err;

	if (OTS_APP->TCP.state_init_succeeded)
	{
		if (OTS_APP->TCP.if_send_response_status)
		{
			OTS_APP->TCP.if_send_response_status = 0;

			// send status
			TCPCOM_PACKET packet_to_send;
			packet_to_send.HDR.OPCODE = TCPCOM_ICD_OPCODE__OTB_STATUS;
			((TCPCOM_PACKET_BODY__STATUS_PTR)packet_to_send.BODY)->CUR_STATE = OTB_STATE__OPERATE;
			io_write(OTS_APP->DRIVERS.TCPCOM, (uint8_t*)&packet_to_send, 1);
		}
	}
	else
	{
		OTS_APP->TCP.phasis_control_state = OTB_STATE__IDLE;
		return APP_ERROR_OK;
	}

	err = app_tcpcom_handle_comm(OTS_APP);
	if (err != APP_ERROR_OK)
	{
		return err;
	}

	err = perform_CBIT_if_necessary(OTS_APP);
	if (err != APP_ERROR_OK)
		return err;


	//err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_OPER_START_RE_CREATED,NULL);
	//if (err != RF_ERROR__OK)
	//{
	 //return err;
	//}

	//	Requirement to communicate with OTBB has been canceled
		err = app_otbbcom_handle_comm(OTS_APP);
	//	if (err != APP_ERROR_OK)
//				return err;



	return APP_ERROR_OK;
}


/*! \brief Implementation of the initialization of LASEROFF state.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__state_LASEROFF__init(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;
	int32_t err;


	err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_OPER_TERMNT_RE_DELETED,NULL);
	//err = optmod_operation_terminate(&OTS_APP->OPTMOD);//***

	if (err != OPTMOD_ERROR__OK)
	{
		return err;
	}

	//err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_OPER_UNTIL_DONE,NULL);
	//optmod_operation_block_until_done(&OTS_APP->OPTMOD); //***

	OTS_APP->TCP.state_init_succeeded = 0;

//	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__SET_STATE, FPGA_State__LASER_OFF);
//	if (err != IO_ERROR_FPGA_OTBA__OK)
//	{
//		return err;
//	}

	OTS_APP->TCP.state_init_succeeded = 1;

	return APP_ERROR_OK;
}


/*! \brief Implementation of the operation of LASEROFF state.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__state_LASEROFF__operation(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;
	int32_t err;

	if (OTS_APP->TCP.state_init_succeeded)
	{
		if (OTS_APP->TCP.if_send_response_status)
		{
			OTS_APP->TCP.if_send_response_status = 0;

			// send status
			TCPCOM_PACKET packet_to_send;
			packet_to_send.HDR.OPCODE = TCPCOM_ICD_OPCODE__OTB_STATUS;
			((TCPCOM_PACKET_BODY__STATUS_PTR)packet_to_send.BODY)->CUR_STATE = OTB_STATE__LASER_OFF;
			io_write(OTS_APP->DRIVERS.TCPCOM, (uint8_t*)&packet_to_send, 1);
		}
	}
	else
	{
		OTS_APP->TCP.phasis_control_state = OTB_STATE__IDLE;
		return APP_ERROR_OK;
	}

	err = app_tcpcom_handle_comm(OTS_APP);
	if (err != APP_ERROR_OK)
	{
		return err;
	}

	err = perform_CBIT_if_necessary(OTS_APP);
	if (err != APP_ERROR_OK)
		return err;


	//	Requirement to communicate with OTBB has been canceled
		err = app_otbbcom_handle_comm(OTS_APP);
//		if (err != APP_ERROR_OK)
	//			return err;



	return APP_ERROR_OK;
}


/*! \brief Implementation of the initialization of ERROR state.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__state_ERROR__init(void* param)
{
    OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;
    int32_t inputParamErrorCode;
    int32_t errorCode;
	
//	APPLY_UI_DESCRIPTOR(OTS_APP, ui_descriptor__error);
    
    /* Check for raised errors */
    inputParamErrorCode = fsm_get_error(OTS_APP->FSM.fsm, &errorCode);
    
    /* Check validity of input parameter */
    if(inputParamErrorCode == FSM_OK)
    {
        /* Write error to log */
        if (errorCode != APP_ERROR_OK) 
        {
//            LOG(&OTS_APP->FS.LOG, APP_LOGGER_LOG_LEVEL__ERROR, "Error was raised: %d", errorCode, NULL);
        }
    }
    
    return APP_ERROR_OK;
}


/*! \brief Implementation of the operation of ERROR state.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__state_ERROR__operation(void* param)
{
	return APP_ERROR_OK;
}


