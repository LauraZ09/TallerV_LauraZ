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

#define RED   	 1
#define GREEN 	 2
#define BLUE     3
#define MAGENTA  4

/*Función Principal del programa*/

BasicTimer_Handler_t handlerTimer3 = { 0 };
GPIO_Handler_t handlerBlinky = { 0 };
uint32_t counter_i_1 = 0;
uint32_t counter_i_2 = 0;

void BasicPWM (uint32_t dutyCicle, uint32_t period, uint32_t repetitions);
//void NeoPixelLEDColorOn (uint8_t color);

int main(void) {

	handlerBlinky.pGPIOx 								= GPIOA;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber 		= PIN_5;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	GPIO_Config(&handlerBlinky);
	GPIO_WritePin(&handlerBlinky, RESET);

	handlerTimer3.ptrTIMx 					= TIM3;
	handlerTimer3.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerTimer3.TIMx_Config.TIMx_speed	= BTIMER_SPEED_100us;
	handlerTimer3.TIMx_Config.TIMx_period 	= 2500; //

	BasicTimer_Config(&handlerTimer3);

	BasicPWM (8, 2, 2);
	BasicPWM (4, 1, 3);
	NeoPixelLEDColorOn(1);

	while (1) {
	}
}

void BasicPWM (uint32_t upCicles, uint32_t downCicles, uint32_t repetitions) {

	counter_i_1 = 0;
	counter_i_2 = 0;

	//upCicles es el número de interrupciones que va a estar arriba
	//El period es el número de interrupciones de la señal completa
	uint32_t period = upCicles + downCicles;
	//repetitions: número de períodos de la señal


	while (counter_i_2 < repetitions){

		if (counter_i_1 <= upCicles){
			GPIO_WritePin(&handlerBlinky, SET);}

		else if (counter_i_1 > upCicles && counter_i_1 <= period){
			GPIO_WritePin(&handlerBlinky, RESET);}

		else if (counter_i_1 > period){
			counter_i_1 = 0;
			counter_i_2++;}
	}
}

/*void NeoPixelLEDColorOn (uint8_t color) { //Está con porcentaje

	if (color == 1) {
		BasicPWM (64, 25000, 8);
		counter_i_1 = 0;
		counter_i_2 = 0;

    	BasicPWM (32, 25000, 8);
		counter_i_1 = 0;
		counter_i_2 = 0;

    	BasicPWM (32, 25000, 8);
		counter_i_1 = 0;
		counter_i_2 = 0;

    	BasicPWM  (0, 25000, 1);
		counter_i_1 = 0;
		counter_i_2 = 0;
	}

	else if (color == 2) {
    	BasicPWM (64, 25000, 8);
    	BasicPWM (32, 25000, 8);
    	BasicPWM (32, 25000, 8);
    	BasicPWM  (0, 25000, 1);
	}

	else if (color == 3) {
    	BasicPWM (32, 25000, 8);
    	BasicPWM (32, 25000, 8);
    	BasicPWM (64, 25000, 8);
    	BasicPWM  (0, 25000, 1);
	}

	else if (color == 4) {
    	BasicPWM (64, 25000, 8);
    	BasicPWM (32, 25000, 8);
    	BasicPWM (64, 25000, 8);
    	BasicPWM  (0, 25000, 1);
	}
}

void BasicTimer3_Callback(void) {
	//GPIOxTooglePin(&handlerBlinky)
	counter_i_1++;}




	/*switch (color) {

	    case 1:
	    	BasicPWM (64, 25000, 8);
	    	BasicPWM (32, 25000, 8);
	    	BasicPWM (32, 25000, 8);
	    	BasicPWM  (0, 25000, 1);
	    break;

	    case 2:
	    	BasicPWM (32, 25000, 8);
	    	BasicPWM (64, 25000, 8);
	    	BasicPWM (32, 25000, 8);
	    	BasicPWM  (0, 25000, 1);
	      break;

	    case 3:
	    	BasicPWM (32, 25000, 8);
	    	BasicPWM (32, 25000, 8);
	    	BasicPWM (64, 25000, 8);
	    	BasicPWM  (0, 25000, 1);
	      break;

	    case 4:
	    	BasicPWM (64, 25000, 8);
	    	BasicPWM (32, 25000, 8);
	    	BasicPWM (64, 25000, 8);
	    	BasicPWM  (0, 25000, 1);
	      break;

	    default:
	    	__NOP();
	      break;*/










