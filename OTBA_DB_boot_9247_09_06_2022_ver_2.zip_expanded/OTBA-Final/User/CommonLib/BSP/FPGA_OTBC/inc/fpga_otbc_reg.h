/**
 ******************************************************************************
 * @file    fpga_otbc_reg.h
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


#ifndef __FPGA_OTBC_REG_H__
#define __FPGA_OTBC_REG_H__

/* Includes */

/* Typedefs */

/* SPI commands */
#define FPGA_COMMAND__RREG								(0x80u)
#define FPGA_COMMAND__WREG								(0x00u)


/* FPGA register addresses */

#define FPGA_OTBC_REG__TEST								(0u)
#define FPGA_OTBC_REG__HW_REV							(1u)
#define FPGA_OTBC_REG__DISCRETES_OUT_BIT				(2u)
#define FPGA_OTBC_REG__DISCRETES_IN_READ				(3u)
#define FPGA_OTBC_REG__SERDES_SFP_STATUS				(4u)
#define FPGA_OTBC_REG__LEDS_REGISTER					(16u)
#define FPGA_OTBC_REG__SERDES_CONTROL					(17u)

#define FPGA_OTBB_REG__SERDES_SFP_STATUS				(6u)



/* Registers bitmap */

// FPGA_OTBC_REG__HW_REV
#define FPGA_OTBC_REG__HW_REV__BOARD_REV_MASK			(0x0F)
#define FPGA_OTBC_REG__HW_REV__BOARD_REV_SHIFT			(0u)
#define FPGA_OTBC_REG__HW_REV__FW_REV_MASK				(0xF0)
#define FPGA_OTBC_REG__HW_REV__FW_REV_SHIFT				(4u)

// FPGA_OTBC_REG__HW_TEST
#define FPGA_OTBC_REG__HW_TEST__TEST_MASK				(0xFF)
#define FPGA_OTBC_REG__HW_TEST__TEST_SHIFT				(0u)



/* Typedefs */



/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif
	 



#ifdef __cplusplus
}
#endif


#endif	//__FPGA_OTBC_REG_H__
