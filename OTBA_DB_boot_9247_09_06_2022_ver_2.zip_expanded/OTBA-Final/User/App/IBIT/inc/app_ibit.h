/**
 ******************************************************************************
 * @file    app_ibit.h
 * @brief   Application IBIT operations public declarations and definitions
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


#ifndef _APP_IBIT_H_
#define _APP_IBIT_H_

/* Includes */

#include "app.h"

typedef enum {
	IBIT_STATE__IDLE, //GM TODO: change this enum when implementing the real IBIT
	IBIT_STATE__RF1,
	IBIT_STATE__RF2,
	IBIT_STATE__RF3,
	IBIT_STATE__DISCRETE_START1,
	IBIT_STATE__DISCRETE_START2,
	IBIT_STATE__DISCRETE_START3,
	IBIT_STATE__DISCRETE_END,
	IBIT_STATE__COMPLETE,

	IBIT_STATE_COUNT
} IBIT_STATE;


int32_t perform_IBIT_if_necessary(OTS_APPLICATION_BUNDLE_PTR OTS_APP);
int32_t perform_discrete_signal_test_IBIT(OTS_APPLICATION_BUNDLE_PTR OTS_APP);


#endif /* _APP_IBIT_H_ */
