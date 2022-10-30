/*
 * ADXL345.h
 *
 *  Created on: Octubre 29, 2022
 *      Author: Laura Zuluaga
 */

#include "I2CDriver.h"

#ifndef ADXL345_H_
#define ADXL345_H_

#define DEVID_R  	     (0x00)		//Posición del registro DEVID 0x00
#define DEVICE_ADDR      (0x53)	    //Dirección del esclavo

#define DATAX0_R         (0x32)		//Posición del registro DATAX0, registro de solo lectura
#define DATAX1_R         (0x33)		//Posición del registro DATAX1, registro de solo lectura
#define DATAY0_R         (0x34)		//Posición del registro DATAY0, registro de solo lectura
#define DATAY1_R		 (0x35)		//Posición del registro DATAY1, registro de solo lectura
#define DATAZ0_R		 (0x36)		//Posición del registro DATAZ0, registro de solo lectura
#define DATAZ1_R		 (0x37)		//Posición del registro DATAZ1, registro de solo lectura
#define BW_RATE_R 		 (0x2C)		//Posición del registro BW_RATE 0x2C
#define POWER_CTL_R      (0x2D)		//Posición del registro POWER_CTL 0x2D
#define DATA_FORMAT_R    (0x31)		//Posición del registro DATA_FORMAT 0x31

#define POWER_MODE_NORMAL_OP   (0b0 << 4)
#define POWER_MODE_REDUCED_OP  (0b1 << 4)

#define OUTPUT_DATA_RATE_50	   (0b1001)
#define OUTPUT_DATA_RATE_100   (0b1010)
#define OUTPUT_DATA_RATE_200   (0b1011)
#define OUTPUT_DATA_RATE_400   (0b1100)
#define OUTPUT_DATA_RATE_800   (0b1101)
#define OUTPUT_DATA_RATE_1600  (0b1110)
#define OUTPUT_DATA_RATE_3200  (0b1111)

#define NORMAL_MODE_OP   (0b0 << 2)
#define SLEEP_MODE_OP    (0b1 << 2)

#define MEASURE_MODE_OP   (0b1 << 3)
#define STANDBY_MODE_OP   (0b0 << 3)

#define SELF_TEST_ENABLED   (0b1 << 7)
#define SELF_TEST_DISABLED  (0b0 << 7)

#define FULL_RES_MODE		 (0b1 << 3)
#define TEN_BITS_RES_MODE 	 (0b0 << 3)

#define LEFT_JUSTIFY_MODE		  (0b1 << 2)
#define RIGHT_WSING_JUSTIFY_MODE  (0b0 << 2)

#define TWOG_RANGE		(0b11)
#define FOURG_RANGE     (0b01)
#define EIGHTG_RANGE    (0b10)
#define SIXTEENG_RANGE  (0b11)

typedef struct
{
	uint8_t			powerModeOp;
	uint8_t			outputDataRate;
	uint8_t			sleepMode;
	uint8_t			measureMode;
	uint8_t			selfTest;
	uint8_t			resolution;
	uint8_t			justifyDataFormat;
	uint8_t			rangeOp;

}ADXL345_Config_t;

typedef struct
{
	I2C_Handler_t		*ptrI2CHandler;
	ADXL345_Config_t	ADXL345_Config;
}ADXL345_Handler_t;


uint8_t init_ADXL345(ADXL345_Handler_t *ptrADXL345Handler);

#endif /* ADXL345_H_ */
