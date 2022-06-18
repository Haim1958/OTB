/**
 ******************************************************************************
 * @file    fpga_otba_private.h
 * @brief   FPGA private declarations and definitions
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
 
#ifndef __FPGA_OTBA_PRIVATE_H__
#define __FPGA_OTBA_PRIVATE_H__

/* Includes */

#include "fpga_otba_reg.h"


/* Definitions */


/* Typedefs */

typedef struct fpga_otba_driver_phy {
	IO_HANDLER_PTR					driver;											// SPI driver
} FPGA_OTBA_DRIVER_PHY, * FPGA_OTBA_DRIVER_PHY_PTR;

typedef struct fpga_otba_driver_bundle {
	FPGA_OTBA_DRIVER_PHY					PHY;									// Physical parameters (ports, interrupts)
} FPGA_OTBA_DRIVER_BUNDLE, * FPGA_OTBA_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t _fpga_otba_read_register(FPGA_OTBA_DRIVER_BUNDLE_PTR driver_bundle, uint8_t reg_addr, uint8_t *reg_val);
int32_t _fpga_otba_write_register(FPGA_OTBA_DRIVER_BUNDLE_PTR driver_bundle, uint8_t reg_addr, uint8_t reg_val);

#ifdef __cplusplus
}
#endif

#endif //__FPGA_OTBA_PRIVATE_H__
