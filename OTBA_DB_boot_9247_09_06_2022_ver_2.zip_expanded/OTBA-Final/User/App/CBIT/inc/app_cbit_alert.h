/**
 ******************************************************************************
 * @file    app_cbit_alert.h
 * @brief   Application CBIT operations public declarations and definitions
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


#ifndef _APP_CBIT_ALERT_H_
#define _APP_CBIT_ALERT_H_

/* Includes */

/* Definitions */

#define photodiode_mVolt_Min_Lavel 0
#define Temperature_Warning_Alert		130.0
#define Temperature_Critical_Alert		160.0
#define PWM_cooling_stop   30.0
#define PWM_cooling_start 130.0
#define Ethernet_OK 0
//OTB_NON_CRITICAL_PFL
typedef enum {
 TEMPERATURE_ALERT   =    	0x8000,//bit15
 OPTC_PW_LVL_OTB_A_B1_1	=	0x4000,//bit14
 OPTC_PW_LVL_OTB_A_B1_2	=	0x2000,//bit13
 OPTC_PW_LVL_OTB_A_B2_1	=	0x1000, //bit12
 OPTC_PW_LVL_OTB_A_B2_2	=	0x0800, //bit11
 OPTC_PW_LVL_OTB_A_C1_1	=	0x0400, //bit10
 OPTC_PW_LVL_OTB_A_C1_2	=	0x0200, //bit9
 OPTC_PW_LVL_OTB_A_C2_1	=	0x0100, //bit8
 OPTC_PW_LVL_OTB_A_C2_2	=	0x0080, //bit7
 FRAME_CNT_OTB_A_AC		=	0x0040, //bit6
 RESERVED10				=	0x0020, //bit5
 RESERVED11				=	0x0010, //bit4
 RESERVED12				=	0x0008, //bit3
 RESERVED13				=	0x0004, //bit2
 RESERVED14				=	0x0002, //bit1
 OTB_NON_CRITICAL_PFL_STATUS	=0x0001 //bit0
} OTB_NON_CRITICAL_PFL;

//OTB_CRITICAL_PFL
typedef enum {
INT_VOLTAGE_FAILURE   =    	0x8000,//bit15
FATAL_TEMPERATURE	=	0x4000,//bit14
RF_POWER_LEVEL	=	0x2000,//bit13
RESRPF3	=	0x1000, //bit12
RESRPF4	=	0x0800, //bit11
RESRPF5	=	0x0400, //bit10
RESRPF6	=	0x0200, //bit9
RESRPF7	=	0x0100, //bit8
RESRPF8	=	0x0080, //bit7
RESRPF9	=	0x0040, //bit6
 RESRPF10				=	0x0020, //bit5
 RESRPF11				=	0x0010, //bit4
 RESRPF12				=	0x0008, //bit3
 RESRPF13				=	0x0004, //bit2
 RESRPF14				=	0x0002, //bit1
 OTB_CRITICAL_PFL_STATUS	=0x0001 //bit0
} OTB_CRITICAL_PFL;



// OTB_MFL_1 bit map
typedef enum {
	MFL_1_INT_VOLTAGE_FAILURE       	=		0x8000,//bit15
	MFL_1_POWER_IN_VOLTAGE			=		0x4000,//bit14
	MFL_1_TEMPERATURE_ALERT		=		0x2000,//bit13
	MFL_1_FATAL_TEMPERATURE		=		0x1000, //bit12
	MFL_1_RF_POWER_LEVEL				=		0x0800, //bit11
  MFL_1_OPTC_PW_LVL_OTB_A_B1_1		=		0x0400, //bit10
  MFL_1_OPTC_PW_LVL_OTB_A_B1_2		=		0x0200, //bit9
  MFL_1_OPTC_PW_LVL_OTB_A_B2_1		=		0x0100, //bit8
  MFL_1_OPTC_PW_LVL_OTB_A_B2_2		=		0x0080, //bit7
  MFL_1_OPTC_PW_LVL_OTB_A_C1_1		=		0x0040, //bit6
  MFL_1_OPTC_PW_LVL_OTB_A_C1_2		=		0x0020, //bit5
  MFL_1_OPTC_PW_LVL_OTB_A_C2_1		=		0x0010, //bit4
  MFL_1_OPTC_PW_LVL_OTB_A_C2_2		=		0x0008, //bit3
  MFL_1_RESERVED13					=		0x0004, //bit2
  MFL_1_RESERVED14					=		0x0002, //bit1
  OTB_MFL_1_STATUS			=		0x0001, //bit0
} OTB_MFL_1;

// OTB_MFL_2 bit map
typedef enum {
MFL2_FRAME_CNT_OTB_A_AC =   0x8000,//bit15
MFL2_FRAME_CNT_OTB_A_B1=	0x4000,//bit14
MFL2_FRAME_CNT_OTB_A_C	=	0x2000,//bit13
MFL2_FRAME_CNT_OTB_A_B2	=	0x1000, //bit12
MFL2_RESERVED4			=	0x0800 ,//bit11
MFL2_RESERVED5			=	0x0400, //bit10
MFL2_RESERVED6			=	0x0200 ,//bit9
MFL2_RESERVED7			=	0x0100, //bit8
MFL2_RESERVED8			=	0x0080 ,//bit7
MFL2_RESERVED9			=	0x0040, //bit6
MFL2_RESERVED10			=	0x0020, //bit5
MFL2_RESERVED11			=	0x0010, //bit4
MFL2_RESERVED12			=	0x0008, //bit3
MFL2_RESERVED13			=	0x0004, //bit2
MFL2_RESERVED14			=	0x0002, //bit1
OTB_MFL_2_STATUS		=	0x0001, //bit0
} OTB_MFL_2;


typedef enum {
BIT_15 	=   0x8000,
BIT_14	=	0x4000,
BIT_13	=	0x2000,
BIT_12	=	0x1000,
BIT_11	=	0x0800,
BIT_10	=	0x0400,
BIT_9	=	0x0200,
BIT_8	=	0x0100,
BIT_7	=	0x0080,
BIT_6	=	0x0040,
BIT_5	=	0x0020,
BIT_4	=	0x0010,
BIT_3	=	0x0008,
BIT_2	=	0x0004,
BIT_1	=	0x0002,
BIT_0	=	0x0001
}bit;
#define ERROR_BIT_MASK	0xfffe
#define PG_P5V0_Error_BIT 	0x0002
#define PG_M5V0_Error_BIT 	0x0004
#define PG_P3V3_Error_BIT 	0x0008
#define PG_2V5_Error_BIT 	0x0010
#define PG_M3V3_Error_BIT 	0x0020
#define PG_1V5_Error_BIT 	0x0040
#define PG_1V8_Error_BIT 	0x0080
#define PG_1V1_Error_BIT 	0x0100




#define FPGA_OTBA_REG__DISC_SET_ENABLE_D7	0x80  //‘1’ = ethernet link between otba and otbb in error/timeout (timeout TBD mSec)
#define FPGA_OTBA_REG__DISC_SET_ENABLE_D6	0x40  //‘1’ = otbb detected re-sync pattern
#define FPGA_OTBA_REG__DISC_SET_ENABLE_D5	0x20  //‘1’ = otbb detected serdes error (“SER2_TX_TST0”, RXDV or RXERR error).
#define FPGA_OTBA_REG__DISC_SET_ENABLE_D4	0x10  //‘1’ = otbb is after power up

#define FPGA_OTBA_REG__SERDES_SFP_STAT_D7	0x80  //(C)SER1_LOS_DETECTED – ‘1’ means that there is optic energy (disconnected) in SER1 link (Clear On Read).
#define FPGA_OTBA_REG__SERDES_SFP_STAT_D6	0x40  //(C)SER1_ERR_DETECTED – ‘1’ means that there is sync loss or general error in SER1 link (Clear On Read).
#define FPGA_OTBA_REG__SERDES_SFP_STAT_D5	0x20  //(B)SER2_LOS_DETECTED – ‘1’ means that there is sync loss or general error in SER2 link (Clear On Read).
#define FPGA_OTBA_REG__SERDES_SFP_STAT_D4	0x80  //(B)SER2_ERR_DETECTED – ‘1’ means that there is optic energy (disconnected) in SER2 link (Clear On Read).


/* Declarations */
void temperature_mointor (OTS_APPLICATION_BUNDLE_PTR OTS_APP,TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR test_result);
void FPGA_OTBA_REG__SERDES_SFP_STAT_test (OTS_APPLICATION_BUNDLE_PTR OTS_APP,TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR test_result);
int32_t Write_FPGA(OTS_APPLICATION_BUNDLE_PTR OTS_APP,uint8_t address,uint8_t data);
int32_t Read_FPGA(OTS_APPLICATION_BUNDLE_PTR OTS_APP,uint8_t address);
void temperature_mointor (OTS_APPLICATION_BUNDLE_PTR OTS_APP,TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR test_result);
Power_Monitor_test (OTS_APPLICATION_BUNDLE_PTR OTS_APP,TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR test_result);
void Update_OTB_test_Staus (OTS_APPLICATION_BUNDLE_PTR OTS_APP,TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR test_result);
#endif /* _APP_CBIT_ALERT_H_ */
