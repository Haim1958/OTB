/**
 ******************************************************************************
 * @file    io_handler.h
 * @brief   IO Handler definition
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


	 
	 
#ifndef __IO_HANDLER_H__
#define __IO_HANDLER_H__

/* Includes */

/* Definitions */

/* IO HANDLER errors */
#define IO_ERROR_HANDLER__OK					IO_ERROR__OK
#define IO_ERROR_HANDLER__NULL					IO_ERROR_HANDLER(0x00000001)
#define IO_ERROR_HANDLER__UNDEFINED_TYPE		IO_ERROR_HANDLER(0x00000002)
#define IO_ERROR_HANDLER__OUT_OF_HEAP			IO_ERROR_HANDLER(0x00000003)



/* Typedefs */
	 
typedef enum {
	IO_HANDLER_TYPE__TCPCOM,
	IO_HANDLER_TYPE__SPI_IO,
	IO_HANDLER_TYPE__ADS8681_OTBA,
	IO_HANDLER_TYPE__FPGA_OTBA,
	IO_HANDLER_TYPE__FPGA_OTBB,
	IO_HANDLER_TYPE__FPGA_OTBC,
	IO_HANDLER_TYPE__UART_IO,
	IO_HANDLER_TYPE__TELNET,
	IO_HANDLER_TYPE__CLI,
	IO_HANDLER_TYPE__SERCOM,
	IO_HANDLER_TYPE__CPU_TEMP,
	IO_HANDLER_TYPE__I2C_IO,
	IO_HANDLER_TYPE__M24512,
	IO_HANDLER_TYPE__SETTINGS,
	IO_HANDLER_TYPE__DAC61408,
	IO_HANDLER_TYPE__DAC7311,
	IO_HANDLER_TYPE__OPT_ADC,
	IO_HANDLER_TYPE__LTC2666,
	IO_HANDLER_TYPE__OTBBCOM,
	IO_HANDLER_TYPE__OPTMOD_FUNC,
	IO_HANDLER_TYPE__RF_FUNC,
	IO_HANDLER_TYPE_COUNT
} IO_HANDLER_TYPE;

typedef struct io_handler IO_HANDLER, *IO_HANDLER_PTR;
struct io_handler {
	/* From Public data */
	IO_HANDLER_TYPE	type;												// Handler type
	
	/* Interface functions */
	int32_t			(*open_fcn)(IO_HANDLER_PTR, void*);					// Read function pointer
	int32_t			(*read_fcn)(IO_HANDLER_PTR, uint8_t*, uint32_t);	// Read function pointer
	int32_t			(*write_fcn)(IO_HANDLER_PTR, uint8_t*, uint32_t);	// Write function pointer
	int32_t			(*ioctl_fcn)(IO_HANDLER_PTR, uint32_t, void*);		// Control function pointer
	
	/* Private data */
	void*			bundle;												// Handler bundle data
	void*			instance;											// Handler private data
//	uint8_t			lock;												// Object lock flag
};


typedef struct io_handler_init_struct {
	/* Interface functions */
	int32_t			(*open_fcn)(IO_HANDLER_PTR, void*);					// Open function pointer
	int32_t			(*read_fcn)(IO_HANDLER_PTR, uint8_t*, uint32_t);	// Read function pointer
	int32_t			(*write_fcn)(IO_HANDLER_PTR, uint8_t*, uint32_t);	// Write function pointer
	int32_t			(*ioctl_fcn)(IO_HANDLER_PTR, uint32_t, void*);		// Control function pointer
	void*			bundle;												// Handler private bundle data
} IO_HANDLER_INSTALL_STRUCT, * IO_HANDLER_INSTALL_STRUCT_PTR;




/* Declarations */

#ifdef __cplusplus
 extern "C" {
#endif

extern IO_HANDLER _io_handlers[IO_HANDLER_TYPE_COUNT];
	 
#ifdef __cplusplus
}
#endif


#endif //__IO_HANDLER_H__
