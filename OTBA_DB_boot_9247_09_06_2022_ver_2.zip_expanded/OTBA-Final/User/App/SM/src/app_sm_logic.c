/**
 ******************************************************************************
 * @file    app_sm_init.c
 * @brief   Application Main State machine initialization method
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


#include "UserLib.h"
#include "app.h"
#include "../inc/app_sm.h"
#include "../inc/app_sm_private.h"
#include <string.h>
#include "cmsis_os.h"


/*! \brief Constructs packet of type FIRMWARE_VERSION.
 *
 ###  Detailed description:
 *
 *  @param OTS_APP - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
extern SemaphoreHandle_t xSemaphore_OTBB_A_STATE_EXM ;
uint8_t logic_state=NO_Change;
uint8_t CBIT_OPERATE_state;
//extern USER_OTBA_ST USER_STATE_TEMP;

/*
This routine monitors system states and switches what is required.
it does not change system states just follows

*/
void SM_logic_app(OTS_APPLICATION_BUNDLE_PTR OTS_APP)
{
		void * current_state; //= fsm_get_active_state(OTS_APP->FSM.fsm);

		//xSemaphoreTake( xSemaphore_OTBB_A_STATE_EXM, ( TickType_t ) pdMS_TO_TICKS (0));
		current_state = fsm_get_active_state(OTS_APP->FSM.fsm);
		//xSemaphoreGive(xSemaphore_OTBB_A_STATE_EXM);


		if ((current_state == OTS_APP->FSM.state__INIT->obj )&& (logic_state!=logic_state__INIT_DONE))
		       {logic_state=logic_state__INIT;}
		else if ((current_state == OTS_APP->FSM.state__LASEROFF->obj )&& (logic_state!=logic_state__LASEROFF_DONE))
		  {logic_state=logic_state__LASEROFF;}
		else if ((current_state == OTS_APP->FSM.state__IBIT->obj )&& (logic_state!=logic_state__IBIT_DONE))
		  {logic_state=logic_state__IBIT;}
		else if ((current_state == OTS_APP->FSM.state__OPERATE->obj )&& (logic_state!=logic_state__OPERATE_DONE))
		{
			logic_state=logic_state__OPERATE;

		}
		else if ((current_state == OTS_APP->FSM.state__IDLE->obj )&& (logic_state!=logic_state__IDLE_DONE))
		 {logic_state=logic_state__IDLE;}

		if( (logic_state==logic_state__OPERATE)||(logic_state==logic_state__OPERATE_DONE))
			{CBIT_OPERATE_state=1;}
		else
			{CBIT_OPERATE_state=0;}




		switch (logic_state)
		{

			case  logic_state__INIT:
			        //		P9_n_TXDIS=0	PE0=TBD’0’.	Reg 26 & 25 & 27=0.

					logic_state=logic_state__INIT_DONE;

					 HAL_GPIO_WritePin (LASER_SHUT_DOWN_GPIO_Port, LASER_SHUT_DOWN_Pin,LASER_ON );//Laser Diode=on
					 HAL_GPIO_WritePin (P9_0_TXDIS_GPIO_Port, P9_0_TXDIS_Pin,GPIO_PIN_SET );// Disable SFP to OTB-C
	break ;
case  logic_state__IBIT:
	//		P9_n_TXDIS=0	PE0=TBD’0’.	Reg 26 & 25 & 27=0.

					logic_state=logic_state__IBIT_DONE;

					 HAL_GPIO_WritePin (P9_0_TXDIS_GPIO_Port, P9_0_TXDIS_Pin,GPIO_PIN_SET );// Disable SFP to OTB-C
					HAL_GPIO_WritePin (P9_1_TXDIS_GPIO_Port, P9_1_TXDIS_Pin,GPIO_PIN_RESET );// Enable SFP to OTB-B1
					HAL_GPIO_WritePin (P9_2_TXDIS_GPIO_Port, P9_2_TXDIS_Pin,GPIO_PIN_RESET );// Enable SFP to OTB-B2
					HAL_GPIO_WritePin (LASER_SHUT_DOWN_GPIO_Port, LASER_SHUT_DOWN_Pin,LASER_ON );
					Write_FPGA(OTS_APP,FPGA_OTBA_REG__SER1_CONTROL,0);
					Write_FPGA(OTS_APP,FPGA_OTBA_REG__SER2_CONTROL,0);
	break ;

case  logic_state__IDLE:
	//		P9_n_TXDIS=0	PE0=TBD’0’.	Reg 26 & 25 & 27=0.

					logic_state=logic_state__IDLE_DONE;

					HAL_GPIO_WritePin (P9_0_TXDIS_GPIO_Port, P9_0_TXDIS_Pin,GPIO_PIN_SET );// Disable SFP to OTB-C
					HAL_GPIO_WritePin (P9_1_TXDIS_GPIO_Port, P9_1_TXDIS_Pin,GPIO_PIN_RESET );// Enable SFP to OTB-B1
					HAL_GPIO_WritePin (P9_2_TXDIS_GPIO_Port, P9_2_TXDIS_Pin,GPIO_PIN_RESET );// Enable SFP to OTB-B2
					HAL_GPIO_WritePin (LASER_SHUT_DOWN_GPIO_Port, LASER_SHUT_DOWN_Pin,LASER_ON );
					Write_FPGA(OTS_APP,FPGA_OTBA_REG__SER1_CONTROL,0);
					Write_FPGA(OTS_APP,FPGA_OTBA_REG__SER2_CONTROL,0);
	break ;
case  logic_state__LASEROFF:
			 logic_state=logic_state__LASEROFF_DONE;
			 HAL_GPIO_WritePin (P9_0_TXDIS_GPIO_Port, P9_0_TXDIS_Pin,GPIO_PIN_SET );// Disable SFP to OTB-C
			 ////HAL_GPIO_WritePin (P9_1_TXDIS_GPIO_Port, P9_1_TXDIS_Pin,GPIO_PIN_SET );// Disable SFP to OTB-B1
			 ////HAL_GPIO_WritePin (P9_2_TXDIS_GPIO_Port, P9_2_TXDIS_Pin,GPIO_PIN_SET );// Disable SFP to OTB-B2
			 HAL_GPIO_WritePin (LASER_SHUT_DOWN_GPIO_Port, LASER_SHUT_DOWN_Pin,LASER_OFF );//Laser Diode=off
			 Write_FPGA(OTS_APP,FPGA_OTBA_REG__SER1_CONTROL,1);// Disable 	SERDES1(OTB-C)  Control Register SER1
			 Write_FPGA(OTS_APP,FPGA_OTBA_REG__SER2_CONTROL,1);// Disable1	SERDES2 (OTB-B) Control Register: ser2_control

             /*****************************************************************/


			 break ;
case  logic_state__OPERATE :

	//		SER1_ENABLE=0	SER2_ENABLE=0	P9_n_TXDIS=0	PE0=TBD’0’.	Reg 26 & 25 & 27=0.
				logic_state=logic_state__OPERATE_DONE;
				HAL_GPIO_WritePin (P9_0_TXDIS_GPIO_Port, P9_0_TXDIS_Pin,GPIO_PIN_RESET );// Enable SFP to OTB-C
				HAL_GPIO_WritePin (P9_1_TXDIS_GPIO_Port, P9_1_TXDIS_Pin,GPIO_PIN_RESET );// Enable SFP to OTB-B1
				HAL_GPIO_WritePin (P9_2_TXDIS_GPIO_Port, P9_2_TXDIS_Pin,GPIO_PIN_RESET );// Enable SFP to OTB-B2
				HAL_GPIO_WritePin (LASER_SHUT_DOWN_GPIO_Port, LASER_SHUT_DOWN_Pin,LASER_ON );
				Write_FPGA(OTS_APP,FPGA_OTBA_REG__LBJP_DISC_SET_CONTROL,0);
				Write_FPGA(OTS_APP,FPGA_OTBA_REG__LBJP_DISC_SET_VALUE,0);
				Write_FPGA(OTS_APP,FPGA_OTBA_REG__TRDS_DISC_SET_CONTROL,0);
				Write_FPGA(OTS_APP,FPGA_OTBA_REG__SER1_CONTROL,0);
				Write_FPGA(OTS_APP,FPGA_OTBA_REG__SER2_CONTROL,0);
				Write_FPGA(OTS_APP,FPGA_OTBA_REG__SERDES2_OTBB_ERROR_STATUS,1);// sync ORBB
				break ;

;
}

				xSemaphoreTake( xSemaphore_OTBB_Queue_Laser_Off, ( TickType_t ) pdMS_TO_TICKS (0));

			 	//memset(&OTS_APP->B_laser_state, 0, sizeof(OTS_APP->B_laser_state));

			 	OTS_APP->B_laser_state.pstate = current_state;
			 	OTS_APP->B_laser_state.uint_state = logic_state;

			 	xQueueOverwrite(OTS_APP->mailbox_laser_state, &OTS_APP->B_laser_state);

			 	xSemaphoreGive( xSemaphore_OTBB_Queue_Laser_Off);

}


