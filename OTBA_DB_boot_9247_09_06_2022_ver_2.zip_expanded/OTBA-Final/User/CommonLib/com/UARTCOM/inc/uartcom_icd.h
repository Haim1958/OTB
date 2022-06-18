/**
 ******************************************************************************
 * @file    uartcom_icd.h
 * @brief   OTS UART ICD public definitions and declarations,
			related to the icd: messages' structure etc.
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

#ifndef _UARTCOM_ICD_H_
#define _UARTCOM_ICD_H_

/* Typedefs */

typedef struct __attribute__((__packed__)) uart_message {
	uint16_t 	SYNC; 				// Offset 0: Start of message mark: 0xAAAA
	uint8_t 	FRAME_LENGTH; 		// Offset 2: The size of the whole message, including the checksum.
	uint16_t	FRAME_COUNTER; 		// Offset 3: A cyclic counter of the frames being sent
	uint8_t 	EQUIPMET_NAME[4]; 	// Offset 5: "OTBB" or "OTBC"
	uint8_t 	SW_VERSION[10]; 	// Offset 9: SW version "xxyyddmmyy" xx=version yy=revision dd=day mm=month yy=year last two digits
	uint8_t 	HW_VERSION[10]; 	// Offset 19: HW version "xxyyddmmyy" xx=version yy=revision dd=day mm=month yy=year last two digits
	uint8_t 	SERIAL_NUMBER[8];	// Offset 29:
	uint8_t		PBIT_RES; 			// Offset 37: Power up BIT result. 0 = pass, 1 = fail.
	uint8_t		CBIT_RES; 			// Offset 38: Continuous BIT result. 0 = pass, 1 = fail.
	uint8_t		VOLTAGE_BITS;		// Offset 39: Bit for each monitor voltage – 0-pass,1-Fail BD: detailed bits
	uint8_t		REGULATOR_VOL_AVG;	// Offset 40: Voltage regulator reading Avg [Volt].
	uint8_t		UHF_POWER;			// Offset 41: UHF 125MHz detected power
	uint8_t		RF_VOLTAGE;			// Offset 42: RF detected output voltage [dBm]
	uint8_t		RF_LEVEL;			// Offset 43: Received RF signal level [dBm]
	uint8_t		RF_REF_VOLT;		// Offset 44: RF reference voltage
	uint8_t		DISCRETES_OUTPUT;	// Offset 45: TBD:Bit for each discrete, correctness of discretes output signals
	uint8_t		RF_CURRENT;			// Offset 46: RF module current consumption correctness
	uint8_t		RF_POWER_MODE;		// Offset 47: RF Power mode, current OTB-B RF output mode. "1" – Full RF output
	uint8_t		RF_EN_STATE;		// Offset 48: Received at the OTB-B input pin.
	uint8_t		OTB_ENABLED;		// Offset 49: "1" – OTB operational.
	uint8_t		IP_RIGHT_OR_CENTER;	// Offset 50: "1" – Right wing for TRDS, center for LBJP.
	uint8_t		IP_LEFT_OR_WING;	// Offset 51: "1" – Left wing for TRDS, wing for LBJP.
	uint8_t		LOS_SFP_OUT;		// Offset 52: LOS SFP output signal
	uint8_t		TEMPERATURE;		// Offset 53: Temperature [C]
	uint8_t		LASER_VALID;		// Offset 54: LASER Connectivity valid
	uint8_t		MEMORY_RES;			// Offset 55: 0-pass, 1-Fail
	uint8_t		ETH_SWITCH_BITS; 	// Offset 56: TBD: Bit for each element ethernet switch (lock, connection, data…)
	uint8_t		SPARE_1;			// Offset 57: Spare byte. Value = 0.
	uint8_t		SPARE_2;			// Offset 58: Spare byte. Value = 0.
	uint8_t		SPARE_3;			// Offset 59: Spare byte. Value = 0.
	uint32_t 	CHECKSUM; 			// Offset 60: CRC32 Sum add 32 bit
} UART_MESSAGE, * UART_MESSAGE_PTR;

#endif /* _UARTCOM_ICD_H_ */
