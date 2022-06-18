/**
 ******************************************************************************
 * @file    fpga_otba_reg.h
 * @brief   FPGA Registers definitions (commands, addresses and bitmaps)
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


#ifndef __FPGA_OTBA_REG_H__
#define __FPGA_OTBA_REG_H__

/* Includes */

/* Typedefs */

/* SPI commands */
#define FPGA_OTBA_COMMAND__RREG							(0x80u)
#define FPGA_OTBA_COMMAND__WREG							(0x00u)


/* FPGA register addresses */

#define FPGA_OTBA_REG__TEST								(0u)
#define FPGA_OTBA_REG__HW_REV							(1u)
#define FPGA_OTBA_REG__DISC_AC_2_OTBB_REG1				(2u)
#define FPGA_OTBA_REG__DISC_AC_2_OTBB_REG2				(3u)
#define FPGA_OTBA_REG__DISC_AC_2_OTBC_REG				(4u)
#define FPGA_OTBA_REG__DISC_TST_C_RD					(5u)
#define FPGA_OTBA_REG__SERDES_SFP_STAT					(6u)
#define FPGA_OTBA_REG__RS422_TEST_SIG					(16u)
#define FPGA_OTBA_REG__LEDS_REG							(17u)
#define FPGA_OTBA_REG__SER1_CONTROL						(18u)
#define FPGA_OTBA_REG__SER2_CONTROL						(19u)
#define FPGA_OTBA_REG__DISC_OUT_WRITE					(20u)
#define FPGA_OTBA_REG__PHD_TEC_CONT						(21u)
#define FPGA_OTBA_REG__SFP_TEC_CONT						(22u)
//#define FPGA_OTBA_REG__DISC_SET_ENABLE				(23u)
//#define FPGA_OTBA_REG__DISC_OUT_SET_ENABLE_WRITE		(24u)
#define FPGA_OTBA_REG__LBJP_DISC_SET_CONTROL			(25u)
#define FPGA_OTBA_REG__LBJP_DISC_SET_VALUE				(26u)
#define FPGA_OTBA_REG__TRDS_DISC_SET_CONTROL			(27u)

#define FPGA_OTBA_REG__SERDES2_OTBB_ERROR_STATUS		(31u)



/* Registers bitmap */

// FPGA_REG__HW_REV
#define FPGA_OTBA_REG__HW_REV__BOARD_REV_MASK				(0x0F)
#define FPGA_OTBA_REG__HW_REV__BOARD_REV_SHIFT				(0u)
#define FPGA_OTBA_REG__HW_REV__FW_REV_MASK					(0xF0)
#define FPGA_OTBA_REG__HW_REV__FW_REV_SHIFT					(4u)

// FPGA_REG__HW_TEST
#define FPGA_OTBA_REG__HW_TEST__TEST_MASK					(0xFF)
#define FPGA_OTBA_REG__HW_TEST__TEST_SHIFT					(0u)



/* Typedefs */



/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif
	 



#ifdef __cplusplus
}
#endif


#endif	//__FPGA_OTBA_REG_H__
