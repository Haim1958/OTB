/**
 ******************************************************************************
 * @file    app_cbit_alert.c
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
extern uint8_t CBIT_OPERATE_state;
void Update_OTB_test_Staus (OTS_APPLICATION_BUNDLE_PTR OTS_APP,TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR test_result)
{


	  test_result->OTB_NON_CRITICAL_PFL = OTB_NON_CRITICAL_PFL_STATUS;
		test_result->OTB_CRITICAL_PFL = OTB_CRITICAL_PFL_STATUS;
		test_result->OTB_MFL_1 = OTB_MFL_1_STATUS;
		test_result->OTB_MFL_2 = OTB_MFL_2_STATUS;;
		test_result->OTB_CRITICAL_MFL_3 = 0;
		test_result->OTB_MAINT_SPARE1 = 0;
		test_result->OTB_MAINT_SPARE2 = 0;
		test_result->OTB_MAINT_SPARE3 = 0;
		test_result->OTB_MAINT_SPARE4 = 0;
		test_result->OTB_MAINT_SPARE5 = 0;


	    temperature_mointor ( OTS_APP,test_result);
	    Power_Monitor_test ( OTS_APP,test_result);
	    FPGA_OTBA_REG__SERDES_SFP_STAT_test ( OTS_APP, test_result);
        //	 RF_mointor  ( OTS_APP, test_result);


}
 Power_Monitor_test (OTS_APPLICATION_BUNDLE_PTR OTS_APP,TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR test_result)
{
	uint16_t DATA=0;
	 if (HAL_GPIO_ReadPin(P5V0_PG_GPIO_Port, P5V0_PG_Pin)==GPIO_PIN_RESET) 		{DATA|=PG_P5V0_Error_BIT ;}
	 if (HAL_GPIO_ReadPin(M5V0_PG_GPIO_Port, M5V0_PG_Pin)==GPIO_PIN_RESET) 		{DATA|=PG_M5V0_Error_BIT ;}
	 if (HAL_GPIO_ReadPin(P3V3_PG_GPIO_Port, P3V3_PG_Pin)==GPIO_PIN_RESET) 		{DATA|=PG_P3V3_Error_BIT ;}
	 if (HAL_GPIO_ReadPin(M3V3_PG_GPIO_Port, M3V3_PG_Pin)==GPIO_PIN_RESET) 		{DATA|=PG_M3V3_Error_BIT ;}
	 if (HAL_GPIO_ReadPin(P5V0_PG_GPIO_Port, P5V0_PG_Pin)==GPIO_PIN_RESET) 		{DATA|=PG_P5V0_Error_BIT ;}
	 if (HAL_GPIO_ReadPin(P5V0_PG_GPIO_Port, P5V0_PG_Pin)==GPIO_PIN_RESET) 		{DATA|=PG_P5V0_Error_BIT ;}
	 if (HAL_GPIO_ReadPin(PG_2V5_GPIO_Port, PG_2V5_Pin)==GPIO_PIN_RESET) 		{DATA|=PG_2V5_Error_BIT ;}
//	 if (HAL_GPIO_ReadPin(PG_1V8_GPIO_Port, PG_1V8_Pin)==GPIO_PIN_RESET) 		{DATA|=PG_1V8_Error_BIT ;}
	 if (HAL_GPIO_ReadPin(PG_1V5_GPIO_Port, PG_1V5_Pin)==GPIO_PIN_RESET) 		{DATA|=PG_1V5_Error_BIT ;}
	 if (HAL_GPIO_ReadPin(PG_1V1_GPIO_Port, PG_1V1_Pin)==GPIO_PIN_RESET) 		{DATA|=PG_1V1_Error_BIT ;}
	 if (DATA!=0)
	  {
		 test_result->OTB_MFL_1|=MFL_1_INT_VOLTAGE_FAILURE;
		 test_result->OTB_MFL_1&= ERROR_BIT_MASK;
		 test_result->OTB_CRITICAL_PFL|=INT_VOLTAGE_FAILURE;
		 test_result->OTB_CRITICAL_PFL&= ERROR_BIT_MASK;

	  }

}
////	read FPGA register 6 for discrete errors
 void FPGA_OTBA_REG__SERDES_SFP_STAT_test (OTS_APPLICATION_BUNDLE_PTR OTS_APP,TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR test_result)
{
	int16_t Data;
	 Data=Read_FPGA (OTS_APP,FPGA_OTBA_REG__SERDES_SFP_STAT);
	 if ((OTBC_Ethernet==Ethernet_OK)&&(CBIT_OPERATE_state==1))
	 {
		 Data=Read_FPGA (OTS_APP,FPGA_OTBA_REG__SERDES_SFP_STAT);
	 //	 report in NC_PFL and MFL1 - OPTC_PW_LVL_OTB_A_C2_2 and OPTC_PW_LVL_OTB_A_C1_2
		 if	(( Data & BIT_6)!=0)
		 {
		 test_result->OTB_NON_CRITICAL_PFL|=OPTC_PW_LVL_OTB_A_C2_2|OPTC_PW_LVL_OTB_A_C1_2;
		 test_result->OTB_NON_CRITICAL_PFL&= ERROR_BIT_MASK;
	     test_result->OTB_MFL_1|=  MFL_1_OPTC_PW_LVL_OTB_A_C2_2| MFL_1_OPTC_PW_LVL_OTB_A_C1_2;
	     test_result->OTB_MFL_1&= ERROR_BIT_MASK;
		 }



     }
if (CBIT_OPERATE_state==0){OTS_APP->OTBBCOM.otbb_err_counter=0;}

	 if ((OTBB1_Ethernet==Ethernet_OK)||(OTBB2_Ethernet==Ethernet_OK))
		 {
		 if ((OTS_APP->OTBBCOM.otbb_disc_rx_err)&&(CBIT_OPERATE_state==1))

		 	 {
			 OTS_APP->OTBBCOM.otbb_disc_rx_err=0;
		if(OTS_APP->OTBBCOM.otbb_err_counter>=2)
				{
		 		//	LB temp: report in NC_PFL and MFL1 - OPTC_PW_LVL_OTB_A_B1_1 and OPTC_PW_LVL_OTB_A_B2_1
		 			test_result->OTB_NON_CRITICAL_PFL |=(BIT_14 |BIT_12);
		 			test_result->OTB_NON_CRITICAL_PFL &= ERROR_BIT_MASK;
		 			test_result->OTB_MFL_1 |=(BIT_10| BIT_8) ;
		 			test_result->OTB_MFL_1 &=ERROR_BIT_MASK;

		 			OTS_APP->OTBBCOM.otbb_err_counter =3;

				}
		 	 }

		 }
}

void temperature_mointor (OTS_APPLICATION_BUNDLE_PTR OTS_APP,TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR test_result)
{
		IO_HANDLER_PTR handler = (IO_HANDLER_PTR)OTS_APP->DRIVERS.CPU_TEMP;
		float temperature = 0.0f;

		io_read(handler, (uint8_t*)&temperature, 1);
		 if (temperature>Temperature_Warning_Alert)
		 {
			 test_result->OTB_MFL_1|=MFL_1_TEMPERATURE_ALERT;
			 test_result->OTB_MFL_1&= ERROR_BIT_MASK;
			 test_result->OTB_NON_CRITICAL_PFL|=TEMPERATURE_ALERT;
			 test_result->OTB_CRITICAL_PFL&= ERROR_BIT_MASK;
		 }

		 if (temperature>Temperature_Critical_Alert)
		 {test_result->OTB_MFL_1|=MFL_1_FATAL_TEMPERATURE;
		 test_result->OTB_MFL_1&= ERROR_BIT_MASK;
		 test_result->OTB_CRITICAL_PFL|=FATAL_TEMPERATURE;
		 test_result->OTB_CRITICAL_PFL&= ERROR_BIT_MASK;

		 }


}

void RF_mointor (OTS_APPLICATION_BUNDLE_PTR OTS_APP, TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR test_result)
{

	int16_t ADC_VAL[10];
	ADC_ChannelConfTypeDef sConfig;
		 sConfig.Channel = Photodiode;
		 io_read(OTS_APP->DRIVERS.OPTIC_ADC, (uint8_t*)&ADC_VAL, 1);
	//	printf(buf, 100-1,"photodiode_mVolt =%d" , ADC_VAL[0]);
		if (ADC_VAL[0] > photodiode_mVolt_Min_Lavel )
		{
			test_result->OTB_CRITICAL_PFL	|=BIT_13;
		}
}

int32_t Read_FPGA(OTS_APPLICATION_BUNDLE_PTR OTS_APP,uint8_t address)

{
	int32_t err=IO_ERROR_FPGA_OTBA__OK;

	FPGA_OTBA_RW_REG reg;


	reg.address = address;

	xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__READ_REG, &reg);
	xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);

if (err != IO_ERROR_FPGA_OTBA__OK)
 { return 0; }

	return reg.value;

}
int32_t Write_FPGA(OTS_APPLICATION_BUNDLE_PTR OTS_APP,uint8_t address,uint8_t data)

{
	int32_t err=IO_ERROR_FPGA_OTBA__OK;

	FPGA_OTBA_RW_REG reg;
	reg.address = address;
	 reg.value=data;
	 xSemaphoreTake( xSemaphore_OTBA_FPGA_READ, ( TickType_t ) pdMS_TO_TICKS (0));
	err = io_ioctl(OTS_APP->DRIVERS.FPGA, IO_IOCTL_FPGA_OTBA__WRITE_REG, &reg);
	xSemaphoreGive( xSemaphore_OTBA_FPGA_READ);
 if (err != IO_ERROR_FPGA_OTBA__OK)
 { return 0; }

	return err;

}

int32_t Read_ADS8681(OTS_APPLICATION_BUNDLE_PTR OTS_APP)

{
	int32_t err=IO_ERROR_ADS8681_OTBA__OK;
	int16_t RETVAL;
	ADS8681_OTBA_RW_REG reg;


	reg.address = 0;

	err = io_ioctl(OTS_APP->DRIVERS.ADS8681, IO_IOCTL_ADS8681_OTBA__READ_REG, &reg);
	RETVAL=RETVAL<<8;
	RETVAL+=reg.value;
	return RETVAL;

}
