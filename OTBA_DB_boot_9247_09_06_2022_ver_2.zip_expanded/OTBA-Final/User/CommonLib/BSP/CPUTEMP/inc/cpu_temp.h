/**
 ******************************************************************************
 * @file    cpu_temp.h
 * @brief   CPU Temperature public declarations and definitions
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


#ifndef __CPU_TEMP_H__
#define __CPU_TEMP_H__

/* Includes */


/* Definitions */

// Error definitions
#define IO_ERROR_CPU_TEMP__OK							IO_ERROR__OK
#define IO_ERROR_CPU_TEMP__OUT_OF_HEAP					IO_ERROR_CPU_TEMP(0x01)
#define IO_ERROR_CPU_TEMP__UNKNOWN_HAL_DRIVER			IO_ERROR_CPU_TEMP(0x02)
#define IO_ERROR_CPU_TEMP__DRV_INDEX_OUT_OF_RANGE		IO_ERROR_CPU_TEMP(0x03)
#define IO_ERROR_CPU_TEMP__UNKNOWN_IOCTL				IO_ERROR_CPU_TEMP(0x04)
#define IO_ERROR_CPU_TEMP__FAILED_TO_CONF_PHY			IO_ERROR_CPU_TEMP(0x05)
#define IO_ERROR_CPU_TEMP__FAILED_TO_READ_ADC			IO_ERROR_CPU_TEMP(0x06)


// IOCTL definitions
#define IO_IOCTL_CPU_TEMP__SET_LOW					IO_IOCTL_CPU_TEMP(0x01)
#define IO_IOCTL_CPU_TEMP__SET_HIGH					IO_IOCTL_CPU_TEMP(0x02)
#define IO_IOCTL_CPU_TEMP__GET_LOW					IO_IOCTL_CPU_TEMP(0x03)
#define IO_IOCTL_CPU_TEMP__GET_HIGH					IO_IOCTL_CPU_TEMP(0x04)



/* Typedefs */

typedef struct {
	uint32_t						temp;
} CPU_TEMP_INSTALL_STRUCT, * CPU_TEMP_INSTALL_STRUCT_PTR;


typedef struct {
	ADC_HandleTypeDef*				HAL_ADC;			// ST HAL handler of ADC
	float							VREF_MV;
} CPU_TEMP_INIT_STRUCT, * CPU_TEMP_INIT_STRUCT_PTR;

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_cpu_temp(CPU_TEMP_INSTALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif


#endif	//__CPU_TEMP_H__
