/*
 * SH1104OLED.h
 *
 *  Created on: Noviembre 06, 2022
 *      Author: Laura Zuluaga
 */

#include "I2CDriver.h"

#ifndef SH1104OLED_H_
#define SH1104OLED_H_

#define OLED_ADD				   0x3C

#define OLED_CONTROLBYTE_COMMAND   0x00
#define OLED_CONTROLBYTE_DATA	   0x40

#define OLED_PAGE_NUMBER_0		   0x00
#define OLED_PAGE_NUMBER_1	       0x01
#define OLED_PAGE_NUMBER_2		   0x02
#define OLED_PAGE_NUMBER_3	       0x03
#define OLED_PAGE_NUMBER_4	       0x04
#define OLED_PAGE_NUMBER_5		   0x05
#define OLED_PAGE_NUMBER_6	       0x06
#define OLED_PAGE_NUMBER_7	       0x07

void sendCommandOLED(I2C_Handler_t *ptrHandlerI2C, uint8_t command);
void sendByteOLED (I2C_Handler_t *ptrHandlerI2C, char *dataToSend);
void sendBytesArray(I2C_Handler_t *ptrHandlerI2C, char **dataToSend);

void setPageOLED(I2C_Handler_t *ptrHandlerI2C, uint8_t pageNumber);
void initOLED(I2C_Handler_t *ptrHandlerI2C);
void clearOLED(I2C_Handler_t *ptrHandlerI2C);
void setColumn(I2C_Handler_t *ptrHandlerI2C, uint8_t columnnNumber);

void printBytesArray(I2C_Handler_t *ptrHandlerI2C, char* bytesArray);
void printSingleByte(I2C_Handler_t *ptrHandlerI2C, char singleByte);

char* stringToChar(char character);

char* AChar(void);
char* spaceChar(void);
char* whiteLineChar(void);

#endif /* SH1104OLED_H_ */
