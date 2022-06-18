/**
 ******************************************************************************
 * @file    opt_adc.h
 * @brief   Optical ADC public declarations and definitions
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


#ifndef __OPT_ADC_H__
#define __OPT_ADC_H__

/* Includes */


/* Definitions */

// Error definitions
#define IO_ERROR_OPT_ADC__OK						IO_ERROR__OK
#define IO_ERROR_OPT_ADC__OUT_OF_HEAP				IO_ERROR_OPT_ADC(0x01)
#define IO_ERROR_OPT_ADC__UNKNOWN_HAL_DRIVER		IO_ERROR_OPT_ADC(0x02)
#define IO_ERROR_OPT_ADC__DRV_INDEX_OUT_OF_RANGE	IO_ERROR_OPT_ADC(0x03)
#define IO_ERROR_OPT_ADC__UNKNOWN_IOCTL				IO_ERROR_OPT_ADC(0x04)
#define IO_ERROR_OPT_ADC__FAILED_TO_CONF_PHY		IO_ERROR_OPT_ADC(0x05)
#define IO_ERROR_OPT_ADC__FAILED_TO_READ_ADC		IO_ERROR_OPT_ADC(0x06)

#define Photodiode ADC_CHANNEL_6
#define Laser_Volt ADC_CHANNEL_5

// IOCTL definitions
//#define IO_IOCTL_OPT_ADC__SET_LOW					IO_IOCTL_OPT_ADC(0x01)



/* Typedefs */

typedef struct {
	uint32_t						temp;
} OPT_ADC_INSTALL_STRUCT, * OPT_ADC_INSTALL_STRUCT_PTR;


typedef struct {
	ADC_HandleTypeDef*				HAL_ADC;			// ST HAL handler of ADC
	ADC_ChannelConfTypeDef			CH_CONFIG;
	float							VREF_MV;
} OPT_ADC_INIT_STRUCT, * OPT_ADC_INIT_STRUCT_PTR;

/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

int32_t install_opt_adc(OPT_ADC_INSTALL_STRUCT_PTR);
int32_t ADC3_channel_select(uint32_t ADC_CHANNEL);
#ifdef __cplusplus
}
#endif


#endif	//__OPT_ADC_H__
