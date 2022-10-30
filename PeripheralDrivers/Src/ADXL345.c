/*
 * ADXL345.h
 *
 *  Created on: Octubre 29, 2022
 *      Author: Laura Zuluaga
 */

#include "ADXL345.h"
#include "I2CDriver.h"
#include "I2CDriver.c"
#include <stm32f4xx.h>

uint8_t BW_RATE_R_VALUE = 0;
uint8_t POWER_CTL_R_VALUE = 0;
uint8_t DATA_FORMAT_R_VALUE = 0;
uint8_t DEVID_R_VALUE = 0;

uint8_t init_ADXL345(ADXL345_Handler_t *ptrADXL345Handler){

	//1. Se edita el registro BW_RATE:

	/*LOW_POWER: A ‘0’ = Normal operation and a ‘1’ = Reduced
	power operation with somewhat higher noise. (See Power
	Modes section for details).*/

	/*RATE: Selects device bandwidth and output data rate. See Table
     5 and Table 6 for details. Default value is 0x0A, or 100 Hz
     Output Data Rate. An Output Data Rate should be selected that
     is appropriate for the communication protocol and frequency
     selected.*/

	BW_RATE_R_VALUE = (ptrADXL345Handler->ADXL345_Config.powerModeOp)|
			(ptrADXL345Handler->ADXL345_Config.outputDataRate);

	i2c_writeSingleRegister(ptrADXL345Handler->ptrI2CHandler, BW_RATE_R, BW_RATE_R_VALUE);


	//2. Se edita el registro DATA_FORMAT:

	DATA_FORMAT_R_VALUE = (ptrADXL345Handler->ADXL345_Config.selfTest)|
			(ptrADXL345Handler->ADXL345_Config.resolution)|(ptrADXL345Handler->ADXL345_Config.justifyDataFormat)|
			(ptrADXL345Handler->ADXL345_Config.rangeOp);

	i2c_writeSingleRegister(ptrADXL345Handler->ptrI2CHandler, DATA_FORMAT_R,DATA_FORMAT_R_VALUE);

	//3. Se edita el registro POWER_CTL:

	i2c_writeSingleRegister(ptrADXL345Handler->ptrI2CHandler, POWER_CTL_R, 0);

	POWER_CTL_R_VALUE = (ptrADXL345Handler->ADXL345_Config.measureMode)|
			(ptrADXL345Handler->ADXL345_Config.sleepMode);
	
	i2c_writeSingleRegister(ptrADXL345Handler->ptrI2CHandler, POWER_CTL_R, POWER_CTL_R_VALUE);
	
	//4. Se lee el registro DEV para ver sí el dispositivo está bien:
	DEVID_R_VALUE = i2c_readSingleRegister(ptrADXL345Handler->ptrI2CHandler, DEVID_R);
	
	return DEVID_R_VALUE;
}
