/**
 ******************************************************************************
 * @file    app_inc.h
 * @brief   OTS Application includes
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


#ifndef __APP_INC_H__
#define __APP_INC_H__


/*! \brief Application Bundle structure.
 *
 ###  Detailed description:
 *	Must be declared before all the includes as these are using it.
 *	Must be defined somewhere.
 *
 *  @param OTS_APP - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
typedef struct _OTS_APPLICATION_BUNDLE OTS_APPLICATION_BUNDLE, * OTS_APPLICATION_BUNDLE_PTR;


#include "UserLib.h"
#include "App_Version.h"
#include "../SM/inc/app_sm.h"
#include "../Tasks/inc/OTSTasks.h"
#include "../IBIT/inc/app_ibit.h"
#include "../CBIT/inc/app_cbit.h"
#include "../RF/inc/app_rf.h"
#include "../CBIT/inc/app_cbit_alert.h"


#endif // __APP_INC_H__
