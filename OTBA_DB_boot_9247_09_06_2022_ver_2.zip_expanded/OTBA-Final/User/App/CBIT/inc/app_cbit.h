/**
 ******************************************************************************
 * @file    app_cbit.h
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


#ifndef _APP_CBIT_H_
#define _APP_CBIT_H_

/* Includes */

/* Definitions */

#define CBIT_OPTMOD__LONG_SEEK_DURATION_TICKS		(10000u)
#define CBIT_OPTMOD__LOW_POWER_THRESHOLD			(160u)


/* Declarations */

int32_t perform_CBIT_if_necessary(OTS_APPLICATION_BUNDLE_PTR OTS_APP);


#endif /* _APP_CBIT_H_ */
