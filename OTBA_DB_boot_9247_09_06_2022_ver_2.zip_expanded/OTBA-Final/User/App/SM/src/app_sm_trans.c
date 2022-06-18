/**
 ******************************************************************************
 * @file    app_sm_trans.c
 * @brief   Application Main State machine transitions implementations
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


#include "UserLib.h"
#include "app.h"
#include "../inc/app_sm_private.h"

/*! \brief Function which checks whether the state transition shall be executed.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **TRUE** if the state transition shall be executed
 *  @return **FALSE** if the state transition shall not be executed
 */

OPTMOD_STRUCT_PTR OPTMOD_GLB;
int32_t _app_sm__trans_INIT_IDLE__check(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;
	/**
	int32_t err = IO_ERROR_OPTMOD_OK;

	err = io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_OPER_START_RE_CREATED,	NULL);
	return err;
   **/
	//return (OPTMOD_GLB->OPER.STATUS.state == OPTMOD_OPER_STATE__LOCK);

	return (OPTMOD_GLB->OPER.STATUS.state != OPTMOD_OPER_STATE__OFF);
	//return 1;
}


/*! \brief Implementation of the operation executed upon state transition execution.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__trans_INIT_IDLE__exit(void* param)
{
//	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;
	
	return APP_ERROR_OK;
}


/*! \brief Function which checks whether the state transition shall be executed.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **TRUE** if the state transition shall be executed
 *  @return **FALSE** if the state transition shall not be executed
 */
int32_t _app_sm__trans_IDLE_IBIT__check(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;

	return (OTS_APP->TCP.phasis_control_state == OTB_STATE__IBIT);
}


/*! \brief Implementation of the operation executed upon state transition execution.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__trans_IDLE_IBIT__exit(void* param)
{
//	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;

	return APP_ERROR_OK;
}


/*! \brief Function which checks whether the state transition shall be executed.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **TRUE** if the state transition shall be executed
 *  @return **FALSE** if the state transition shall not be executed
 */
int32_t _app_sm__trans_IBIT_IDLE__check(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;

	return (OTS_APP->TCP.phasis_control_state == OTB_STATE__IDLE);
}


/*! \brief Implementation of the operation executed upon state transition execution.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__trans_IBIT_IDLE__exit(void* param)
{
//	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;

	return APP_ERROR_OK;
}


/*! \brief Function which checks whether the state transition shall be executed.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **TRUE** if the state transition shall be executed
 *  @return **FALSE** if the state transition shall not be executed
 */
int32_t _app_sm__trans_IDLE_OPERATE__check(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;

	return (OTS_APP->TCP.phasis_control_state == OTB_STATE__OPERATE);
}


/*! \brief Implementation of the operation executed upon state transition execution.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__trans_IDLE_OPERATE__exit(void* param)
{
//	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;

	return APP_ERROR_OK;
}


/*! \brief Function which checks whether the state transition shall be executed.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **TRUE** if the state transition shall be executed
 *  @return **FALSE** if the state transition shall not be executed
 */
int32_t _app_sm__trans_OPERATE_IDLE__check(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;

	return (OTS_APP->TCP.phasis_control_state == OTB_STATE__IDLE);
}


/*! \brief Implementation of the operation executed upon state transition execution.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__trans_OPERATE_IDLE__exit(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;
	int32_t err;
	err = io_ioctl(OTS_APP->DRIVERS.RF_FUNC, IO_IOCTL_RF_OPER_TERMNT_RE_DELETED,NULL);
	return APP_ERROR_OK;
}


/*! \brief Function which checks whether the state transition shall be executed.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **TRUE** if the state transition shall be executed
 *  @return **FALSE** if the state transition shall not be executed
 */
int32_t _app_sm__trans_IDLE_LASEROFF__check(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;

	return (OTS_APP->TCP.phasis_control_state == OTB_STATE__LASER_OFF);
}


/*! \brief Implementation of the operation executed upon state transition execution.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__trans_IDLE_LASEROFF__exit(void* param)
{
//	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;

	return APP_ERROR_OK;
}


/*! \brief Function which checks whether the state transition shall be executed.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **TRUE** if the state transition shall be executed
 *  @return **FALSE** if the state transition shall not be executed
 */
int32_t _app_sm__trans_LASEROFF_IDLE__check(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;

	return (OTS_APP->TCP.phasis_control_state == OTB_STATE__IDLE);
}


/*! \brief Implementation of the operation executed upon state transition execution.
 *
 ###  Detailed description:
 *
 *  @param param - pointer to the OTS application structure.
 *  @return **APP_ERROR_OK** if the handling was OK
 *  @return **Error** if the initialization failed
 */
int32_t _app_sm__trans_LASEROFF_IDLE__exit(void* param)
{
	OTS_APPLICATION_BUNDLE_PTR OTS_APP = (OTS_APPLICATION_BUNDLE_PTR) param;

	//int32_t err = OPTMOD_ERROR__OK;//IO_ERROR_OPTMOD_OK;

	uint16_t err_return[1] = {0};


	OTS_APP->CBIT.optics_long_seek_error_sent = 0u;
	OTS_APP->CBIT.optics_low_power_error_sent = 0u;

	io_ioctl(OTS_APP->DRIVERS.OPTMOD_FUNC, IO_IOCTL_OPTMOD_OPER_START_RE_CREATED,	&err_return[0]);

	OTS_APP->TCP.phasis_control_state = OTB_STATE__IDLE;

	return err_return[0] = 0;//(*err_return);
	//return err;
}


