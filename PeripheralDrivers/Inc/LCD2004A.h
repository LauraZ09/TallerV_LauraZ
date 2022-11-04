/*
 * ADXL345.h
 *
 *  Created on: Octubre 29, 2022
 *      Author: Laura Zuluaga
 */

#include "I2CDriver.h"
#include <stm32f4xx.h>

#ifndef LCD2004A_H_
#define LCD2004A_H_

#define LCD_ADDR         (0x27)	    //Dirección del esclavo

#define LED_ON           (0b1 << 3)
#define ENABLE_ON		 (0b1 << 2)
#define DISPLAY_DATA     (0b1 << 0)

#define POS_1_LINE_1     (0x00)		//Posición 1 de la fila 1
#define POS_1_LINE_2	 (0x40)	    //Posición 2 de la fila 1
#define POS_1_LINE_3
#define POS_1_LINE_4


void initLCD(I2C_Handler_t *ptrHandlerI2C);
void sendCommandLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t command);
void displayDataLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t data);
void sendByteLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToSend);
void clearDisplayLCD(I2C_Handler_t *ptrHandlerI2C);
void returnHomeLCD(I2C_Handler_t *ptrHandlerI2C);
void printStringLCD(I2C_Handler_t *ptrHandlerI2C,char* string);
void moveCursorToLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t position);

#endif
