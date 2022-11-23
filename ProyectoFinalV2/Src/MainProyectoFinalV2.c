/**
 ******************************************************************************
 * @file           : MainProyectiFinalV2.c
 * @author         : Laura Zuluaga
 * @brief          : Pruebas Proyecto
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "RccConfig.h"
#include "PWMDriver.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "WS2812bDriver.h"

#define NUMBER_OF_LEDS 60;

BasicTimer_Handler_t handlerBlinkyTimer = { 0 };  //Handler para el BlinkyTimer
BasicTimer_Handler_t handlerIntTimer    = { 0 };

GPIO_Handler_t handlerBlinkyPin 	    = { 0 };  //Handler para el LED de estado
GPIO_Handler_t handlerMCO_2  			= { 0 };  //Handler para el PIN de salida del Clock
GPIO_Handler_t handlerPWMOutput 		= { 0 };  //Handler para la salida del PWM

void initSystem(void);

int main(void)
{
	setTo100M();
	initSystem();

	clearLEDS(55, &handlerPWMOutput); //Se ponen en negro los 60 LEDS
	setColorLED(255, 0, 0, &handlerPWMOutput); //se envía el primer LED en rojo
	ResetTime(&handlerPWMOutput);
	setColorNumberLED(255, 255, 0, 3, &handlerPWMOutput); //Se pone en amarillo el tercer LED
	ResetTime(&handlerPWMOutput);
	//setColorNumberLED(255, 0, 255, 2, &handlerPWMOutput);
	//ResetTime(&handlerPWMOutput);

	while (1)
	{


	}

	return 0;
}

void BasicTimer2_Callback(void) {
	GPIOxTooglePin(&handlerBlinkyPin);
	}


void initSystem(void) {

	//Se configura el LED de estado
	handlerBlinkyPin.pGPIOx 								= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber 		    = PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType 		    = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerBlinkyPin);

	//Se configura el PIN "PWM"
	handlerPWMOutput.pGPIOx 						      = GPIOA;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinNumber 		  = PIN_0;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinMode 	      = GPIO_MODE_OUT;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinOPType 		  = GPIO_OTYPE_PUSHPULL;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinSpeed 	      = GPIO_OSPEED_FAST;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinPuPdControl	  = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPWMOutput);

	//Se pone el PIN en 0:
	GPIO_WritePin(&handlerPWMOutput, RESET);


	//Se configura el BlinkyTimer
	handlerBlinkyTimer.ptrTIMx 					= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_100M_05ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 	= 500; //Update period= 0.05ms*500 = 250ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerBlinkyTimer);


	//Se configura el BlinkyTimer
	handlerIntTimer.ptrTIMx 					= TIM3;
	handlerIntTimer.TIMx_Config.TIMx_mode 	    = BTIMER_MODE_UP;
	handlerIntTimer.TIMx_Config.TIMx_speed 	    = 0;
	handlerIntTimer.TIMx_Config.TIMx_period 	= 1; //Update period = 10ns

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerIntTimer);


	handlerMCO_2.pGPIOx 						    = GPIOC;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinNumber 		= PIN_9;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinAltFunMode  = AF0;

	GPIO_Config(&handlerMCO_2);

}



