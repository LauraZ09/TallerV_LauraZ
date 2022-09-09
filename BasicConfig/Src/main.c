/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Laura Zuluaga
 * @brief          : Configuración básica de un proyecto
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
#include "GPIOxDriver.h"
#include "BasicTimer.h"

/*Función Principal del programa*/

BasicTimer_Handler_t handlerTimer2 ={0};
GPIO_Handler_t handlerBlinky = {0};
uint8_t blinkState = 0;


int main(void)
{

	handlerBlinky.pGPIOx								=GPIOA;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber			=PIN_5;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode			=GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType			=GPIO_OSPEED_MEDIUM;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed			=GPIO_OSPEED_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl	=GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerBlinky);
	GPIO_WritePin(&handlerBlinky, SET);

	handlerTimer2.ptrTIMx					= TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period 	= 2500; //periodo de 250ms

	BasicTimer_Config(&handlerTimer2);

	while(1){

	}
 return 0;
}

/*Callback de la interrupción del timer2*/

void BasicTimer2_Callback(void) {
	blinkState = !blinkState;

	if (blinkState){
		GPIO_WritePin(&handlerBlinky, SET);
	}else{
		GPIO_WritePin(&handlerBlinky, RESET);
	}
}




