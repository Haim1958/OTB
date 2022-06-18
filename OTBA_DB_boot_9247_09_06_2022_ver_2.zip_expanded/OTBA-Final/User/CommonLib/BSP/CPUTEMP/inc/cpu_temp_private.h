/**
 ******************************************************************************
 * @file    cpu_temp_private.h
 * @brief   CPU Temperature private declarations and definitions
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

 
#ifndef __CPU_TEMP_PRIVATE_H__
#define __CPU_TEMP_PRIVATE_H__

/* Includes */


/* Definitions */



/* Typedefs */

typedef struct cpu_temp_driver_generic {
	uint32_t						raw_reading;
	float							calibrated_temperature;
	uint8_t							cool_start_temp;
	uint8_t							cool_stop_temp;
} CPU_TEMP_DRIVER_GENERIC, * CPU_TEMP_DRIVER_GENERIC_PTR;

typedef struct cpu_temp_driver_phy {
	ADC_HandleTypeDef*				HAL_ADC;						// ST ADC driver handler
	float							VREF_MV;

	float							CAL_ADC_TEMP_3V3_30C;
	float							CAL_ADC_TEMP_3V3_110C;
} CPU_TEMP_DRIVER_PHY, * CPU_TEMP_DRIVER_PHY_PTR;

typedef struct cpu_temp_driver_bundle {
	CPU_TEMP_DRIVER_PHY				PHY;
	CPU_TEMP_DRIVER_GENERIC			g;
} CPU_TEMP_DRIVER_BUNDLE, * CPU_TEMP_DRIVER_BUNDLE_PTR;


/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif
	 
int32_t _cpu_temp_open(IO_HANDLER_PTR, void*);
int32_t _cpu_temp_read(IO_HANDLER_PTR, uint8_t*, uint32_t);
int32_t _cpu_temp_write(IO_HANDLER_PTR, uint8_t*, uint32_t);
int32_t _cpu_temp_ioctl(IO_HANDLER_PTR, uint32_t, void*);

int32_t _cpu_temp_init(CPU_TEMP_DRIVER_BUNDLE_PTR driver_bundle);
int32_t _cpu_temp_sample_temperature(CPU_TEMP_DRIVER_BUNDLE_PTR driver_bundle);

#ifdef __cplusplus
}
#endif

#endif //__CPU_TEMP_PRIVATE_H__
