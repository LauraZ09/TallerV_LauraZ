/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "RccConfig.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"

BasicTimer_Handler_t handlerTimer3 = { 0 };
GPIO_Handler_t handlerBlinky = { 0 };
GPIO_Handler_t handlerMCO_2  = { 0 };

uint64_t LedFlagCounter = 0;


int main(void)
{
handlerBlinky.pGPIOx 								= GPIOA;
handlerBlinky.GPIO_PinConfig.GPIO_PinNumber 		= PIN_5;
handlerBlinky.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
handlerBlinky.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

GPIO_Config(&handlerBlinky);
GPIO_WritePin(&handlerBlinky, RESET);

handlerMCO_2.pGPIOx 						    = GPIOC;
handlerMCO_2.GPIO_PinConfig.GPIO_PinNumber 		= PIN_9;
handlerMCO_2.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;
handlerMCO_2.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
handlerMCO_2.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
handlerMCO_2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
handlerMCO_2.GPIO_PinConfig.GPIO_PinAltFunMode  = AF0;

GPIO_Config(&handlerMCO_2);


handlerTimer3.ptrTIMx 					= TIM3;
handlerTimer3.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
handlerTimer3.TIMx_Config.TIMx_speed	= BTIMER_SPEED_0_05us;
handlerTimer3.TIMx_Config.TIMx_period 	= 2500;

BasicTimer_Config(&handlerTimer3);

setTo40M();

while (1)
{
	/*if(LedFlagCounter >= 1000000) //Con este if se hace que el blinky sea en milisegundos
	{
		GPIOxTooglePin(&handlerBlinky);
		LedFlagCounter = 0;
	}
	else {
		__NOP();
	}*/
}

return 0;
}

void BasicTimer3_Callback(void) {
	//LedFlagCounter++;
	GPIOxTooglePin(&handlerBlinky);
	}




