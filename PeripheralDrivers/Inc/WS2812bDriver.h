/*
 * WS2812bDriver.h
 *
 *  Created on: Month XX, 2022
 *      Author: namontoy
 */

#ifndef INC_WS2812b_H_
#define INC_WS2812b_H_

#include "stm32f4xx.h"
#include "GPIOxDriver.h"

void LogicOne(GPIO_Handler_t *pGPIOHandler);
void LogicZero(GPIO_Handler_t *pGPIOHandler);
void ResetTime(GPIO_Handler_t *pGPIOHandler);
void colorByte(uint8_t byte, GPIO_Handler_t *pGPIOHandler);
void setColorLED(uint8_t RED, uint8_t GREEN, uint8_t BLUE, GPIO_Handler_t *pGPIOHandler);
void clearLEDS(uint8_t numberOfLEDS,GPIO_Handler_t *pGPIOHandler);
void setColorNumberLED(uint8_t RED, uint8_t GREEN, uint8_t BLUE, uint8_t numberLED,GPIO_Handler_t *pGPIOHandler);

#endif /* INC_WS2812b_H_ */
