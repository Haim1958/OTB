/**
 ******************************************************************************
 * @file    tcpcom_icd.h
 * @brief   OTS TCP ICD public definitions and declarations,
			related to the icd: packet structure, message IDs, etc...
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


#ifndef __TCPCOM_ICD_H__
#define __TCPCOM_ICD_H__

/* Includes */

/* Definitions */

#define TCPCOM_ICD_MAX_PACKET_BODY_SIZE						(100u)			// Maximum possible packet body size

#define UINT32_MAX_MSG_COUNTER								(4294967295)

// ID Numbers
#define TCPCOM_ICD_ID__SPECTRA					 			(5u)			// Aircraft
#define TCPCOM_ICD_ID__OTBA						    		(128u)			// OTS main block
#define TCPCOM_ICD_ID__OTBC1					    		(150u)			// LBJP
#define TCPCOM_ICD_ID__OTBC2					    		(180u)			// LBJP
#define TCPCOM_ICD_ID__OTBB1					    		(200u)			// TRDS
#define TCPCOM_ICD_ID__OTBB2					    		(201u)			// TRDS
#define TCPCOM_ICD_ID__INSTRUMENTATION			    		(250u)			//

// IP Addresses
#define IP_ADDRESS_SPECTRA		"192.168.3.10"
#define IP_ADDRESS_OTBA 		"192.168.3.20"
#define IP_ADDRESS_DEFAULT_OTBB "192.168.3.25"
#define IP_ADDRESS_OTBC 		"192.168.3.26"
#define IP_ADDRESS_LEFT_OTBB 	"192.168.3.23"
#define IP_ADDRESS_RIGHT_OTBB 	"192.168.3.24"
#define IP_MASK					"255.255.255.0"
#define IP_DEFAULT_GATEWAY		"0.0.0.0"

// Ports
#define TCP_OTBA_PORT_FOR_SPECTRA			(20620u)
#define TCP_SPECTRA_PORT_FOR_OTBA			(20520u)
#define TCP_OTBA_PORT_FOR_OTB				(20621u)
#define OTBBCOM_CLIENT_IP_ALLOC_PORT_OTBB   (5440u)

// Opcode values
typedef enum
{
	TCPCOM_ICD_OPCODE__PHASIS_CONTROL =	2u,
    TCPCOM_ICD_OPCODE__OTB_DESCRITE_SIGNAL_TEST	= 20u
} ReceivedMsgCode;

typedef enum
{
	TCPCOM_ICD_OPCODE__OTB_TEST_RESULT = 2u,
	TCPCOM_ICD_OPCODE__OTB_DESCRITE_SIGNAL_REPORT =	20u,
	TCPCOM_ICD_OPCODE__OTB_STATUS =	1u
} SentMsgCode;

/* Typedefs */

// Packet Header definition
// All fields are in host endianess (ARM32 - LITTLE)

typedef struct __attribute__((__packed__)) tcpcom_header {
	uint32_t				SIZE;				// = sizeof(TCPCOM_HEADER) - sizeof(TCPCOM_HEADER.SIZE) + sizeof(BODY)
	uint8_t					SOURCE;				// ID of sender
	uint8_t					DESTINATION;		// ID of destination. May be "All Computers"
	uint16_t				OPCODE;				// Opcodes 0-255 are reserved
	uint32_t				TIMESTAMP;			// System time (LSB=10us) just before sending the message
	uint32_t				MSGCOUNTER;			// Communication counter for the packet loss detection algorithm.
												// Incremented when message is sent.
												// One message counter for each TCP connection and each UDP port.
												// Incremented independently of the message opcode.
												// Reset to value 0 upon power-up or reset or overflow.
	uint32_t				RES1;				// Reserved for future use, value 0
	uint32_t				RES2;				// Reserved for future use, value 0
	uint32_t				RES4;				// Reserved for future use, value 0
	uint32_t				RES5;				// Reserved for future use, value 0
} TCPCOM_HEADER, * TCPCOM_HEADER_PTR;



typedef struct __attribute__((__packed__)) tcpcom_packet {
	TCPCOM_HEADER			HDR;
	uint8_t					BODY[TCPCOM_ICD_MAX_PACKET_BODY_SIZE];
} TCPCOM_PACKET, * TCPCOM_PACKET_PTR;

typedef enum {
	TEST_START,
	TEST_END
} TEST_PROG;

typedef enum {
	TEST_NOT_IN_PROGRESS,
	TEST_IN_PROGRESS
} REPORT_TEST_PROG;

typedef enum
{
	FPGA_IBIT_DISCRETES_WRITE_0,
	FPGA_IBIT_DISCRETES_WRITE_1,
	FPGA_IBIT_DISCRETES_DO_NOT_WRITE
} FPGADiscretesWriteStatus;

typedef struct __attribute__((__packed__)) {
	uint32_t				OTB_STATE;
} TCPCOM_PACKET_BODY__PHASIS_CONTROL, * TCPCOM_PACKET_BODY__PHASIS_CONTROL_PTR;

typedef struct __attribute__((__packed__)) {
	uint32_t				CUR_STATE;
} TCPCOM_PACKET_BODY__STATUS, *TCPCOM_PACKET_BODY__STATUS_PTR;

typedef struct __attribute__((__packed__)) {
	uint8_t					LBJP_RW_LB;
	uint8_t					LBJP_RW_MB;
	uint8_t					LBJP_TR_LB;
	uint8_t					LBJP_TR_MB;
	uint8_t					RSV2OTB;
} LBJP_DISCRETE_TEST, *LBJP_DISCRETE_TEST_PTR;

typedef struct __attribute__((__packed__)) {
	uint8_t					RSV1OTB;
	uint8_t					TRDS_T1_TE;
	uint8_t					TRDS_T2_TE;
	uint8_t					TRDS_TI;
	uint8_t					TRDS_TOP_GIC;
	uint8_t					TRDS_SPARE;
	uint8_t					LBJP_RI_LB;
	uint8_t					LBJP_RI_MB;
	uint8_t					LBJP_TI_LB;
	uint8_t					LBJP_TI_MB;
	uint8_t					CDFRALBJP;
} OTB_IN_DISC_SIG, * OTB_IN_DISC_SIG_PTR;

typedef struct __attribute__((__packed__)) {
	uint32_t				OTB_DISC_SIG_TEST_PROG;
	OTB_IN_DISC_SIG			OTB_IN_DISC_SIG_TOBEUTEST;
	uint8_t					RF;
	LBJP_DISCRETE_TEST		OTB_O_DISC_SIG_TOBEUTEST;
	LBJP_DISCRETE_TEST		OTB_O_DISC_SIG_TEST;
} TCPCOM_PACKET_BODY__DISCRETE_RF_SIGNAL_TEST, *TCPCOM_PACKET_BODY__DISCRETE_RF_SIGNAL_TEST_PTR;

typedef struct __attribute__((__packed__)) {
	uint32_t				OTB_DISC_SIG_TEST_PROG;
	LBJP_DISCRETE_TEST		OTB_SIG_UNDER_TEST;
	OTB_IN_DISC_SIG			OTB_DIS_SIG_MONI_VAL;
	int32_t					RF_LEVEL;
	uint32_t				RF_STATUS;
} TCPCOM_PACKET_BODY__DISCRETE_TEST_REPORT, *TCPCOM_PACKET_BODY__DISCRETE_TEST_REPORT_PTR;

typedef struct __attribute__((__packed__)) {
	uint32_t				OTB_CURRENT_STATE;
	uint16_t				OTB_NON_CRITICAL_PFL;
	uint16_t 				OTB_CRITICAL_PFL;
	uint16_t 				OTB_MFL_1;
	uint16_t 				OTB_MFL_2;
	uint16_t 				OTB_CRITICAL_MFL_3;
	uint32_t 				OTB_MAINT_SPARE1;
	uint32_t				OTB_MAINT_SPARE2;
	uint32_t 				OTB_MAINT_SPARE3;
	uint32_t 				OTB_MAINT_SPARE4;
	uint32_t 				OTB_MAINT_SPARE5;
	uint8_t 				OTB_EQUIPMENT_NAME[4];
	uint8_t 				OTB_SOFTWARE_REFERENCE[16]; // SW version "xxyyddmmyyyy    " xx=version yy=subversion (xx.yy) dd=day mm=month yyyy=year
	uint8_t 				OTB_HARDWARE_REFERENCE[12];
} TCPCOM_PACKET_BODY__OTB_TEST_RESULT, *TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR;


/* Declarations */

#ifdef __cplusplus
extern "C" {
#endif


// void _tcpcom_icd_signal_test_ntoh(TCPCOM_PACKET_BODY__DISCRETE_RF_SIGNAL_TEST_PTR signal_test);
// void _tcpcom_icd_test_result_hton(TCPCOM_PACKET_BODY__OTB_TEST_RESULT_PTR test_result);
// void _tcpcom_icd_signal_discrete_test_report_hton(TCPCOM_PACKET_BODY__DISCRETE_TEST_REPORT_PTR test_report);
// void _tcpcom_icd_status_hton(TCPCOM_PACKET_BODY__STATUS_PTR status);
// void _tcpcom_icd_phasis_control_ntoh(TCPCOM_PACKET_BODY__PHASIS_CONTROL_PTR phasis_control);


#ifdef __cplusplus
}
#endif

#endif //__TCPCOM_ICD_H__
