/*
 * ADXL345.h
 *
 *  Created on: Octubre 29, 2022
 *      Author: Laura Zuluaga
 */




#include "I2CDriver.h"

#ifndef LCD2004A_H_
#define LCD2004A_H_

#define LCD_ADDR         (0x4E)	    //Dirección del esclavo

#define POS_1_LINE_1     (0x00)		//Posición 1 de la fila 1
#define POS_1_LINE_2	 (0x40)	    //Posición 2 de la fila 1
#define POS_1_LINE_3
#define POS_1_LINE_4


void initLCD(I2C_Handler_t *ptrHandlerI2C);

#endif
