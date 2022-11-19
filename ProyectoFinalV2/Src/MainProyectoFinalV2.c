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

#define TIM_FREQ 	100   	  //Frecuencia del Timer en MHz
#define T0H 		16        //Este es el valor que se debe poner en el CCx para alcanzar 0.4u
#define T1H 		8         //Este es el valor que se debe poner en el CCx para alcanzar 0.8u
#define T0L 		9	      //Este es el valor que se debe poner en el CCx para alcanzar 0.85u
#define T1L 		17        //Este es el valor que se debe poner en el CCx para alcanzar 0.45u
#define TRESET 		25         //Este es el valor que se debe poner en el CCx para alcanzar 50u

PWM_Handler_t handlerPWMTimer 	        = { 0 };  //Handler para el PWM (Timer)

BasicTimer_Handler_t handlerBlinkyTimer = { 0 };  //Handler para el BlinkyTimer
BasicTimer_Handler_t handlerIntTimer    = { 0 };

GPIO_Handler_t handlerBlinkyPin 	    = { 0 };  //Handler para el LED de estado
GPIO_Handler_t handlerMCO_2  			= { 0 };  //Handler para el PIN de salida del Clock
GPIO_Handler_t handlerPWMOutput 		= { 0 };  //Handler para la salida del PWM

uint8_t flag            = 0;
uint8_t counterLEDs		= 0;
uint8_t counterBits 	= 0;
uint8_t counterBytes	= 0;

uint8_t mask = 0b10000000;

uint8_t LED_data[180] = {0}; //En este arreglo se almacenan los números para la intensidad del RGB
					   //el arreglo es de 180 porque son 60 LEDS y 3 bytes por LED

void initSystem(void);

int main(void)
{
	setTo100M();
	initSystem();
	startPwmSignal(&handlerPWMTimer);
	enableOutput(&handlerPWMTimer);

	for (uint8_t i = 0; i < 180; i += 9) {
		LED_data[i] = 255; //Se usan valores bajos para no gastar mucha corriente
		LED_data[i + 4] = 255; //Se usan valores bajos para no gastar mucha corriente
		LED_data[i + 8] = 255;
		//LED_data[i] = 255; //Se usan valores bajos para no gastar mucha corriente
	}

	while (1)
	{


	//Primero se llena el arreglo con una secuencia consecutiva de verde, rojo y azul

		if(flag == 1){

			flag = 0;

			if (counterBytes < 180) {

				if (mask != 1){

					if (LED_data[counterBytes] & mask) {
						TIM5->CCR1 = T1H;
						mask = mask >> 1;
					}

					else {
						TIM5->CCR1 = T0H;
						mask = mask >> 1;
					}
				}

				else {
					mask = 0b10000000;
					counterBytes++;
				}
			}

			else{
				updateDuttyCycle(&handlerPWMTimer, 0);
				counterBytes = 0;
			}
		}

	}

	return 0;
}

void BasicTimer2_Callback(void) {
	GPIOxTooglePin(&handlerBlinkyPin);
	}

void BasicTimer3_Callback(void){
	flag = 1;
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

	//Se configura el BlinkyTimer
	handlerBlinkyTimer.ptrTIMx 					= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_100M_05ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 	= 500; //Update period= 1ms*250 = 250ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerBlinkyTimer);


	//Se configura el BlinkyTimer
	handlerIntTimer.ptrTIMx 					= TIM3;
	handlerIntTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerIntTimer.TIMx_Config.TIMx_speed 	= 4;
	handlerIntTimer.TIMx_Config.TIMx_period 	= 25; //Update period= 1ms*250 = 250ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerIntTimer);

	//Se configura el Timer del PWM
	handlerPWMTimer.ptrTIMx            = TIM5;
	handlerPWMTimer.config.channel     = PWM_CHANNEL_1;
	handlerPWMTimer.config.prescaler   = 4;
	handlerPWMTimer.config.periodo     = 25;
	handlerPWMTimer.config.duttyCicle  = 25;

	pwm_Config(&handlerPWMTimer);

	//Se configura la salida del PWM
	handlerPWMOutput.pGPIOx 						      = GPIOA;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinNumber 		  = PIN_0;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinMode 	      = GPIO_MODE_ALTFN;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinOPType 		  = GPIO_OTYPE_PUSHPULL;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinSpeed 	      = GPIO_OSPEED_FAST;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinPuPdControl	  = GPIO_PUPDR_NOTHING;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinAltFunMode 	  = AF2; //AF02: TIM5_CH1;

	GPIO_Config(&handlerPWMOutput);

	handlerMCO_2.pGPIOx 						    = GPIOC;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinNumber 		= PIN_9;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinAltFunMode  = AF0;

	GPIO_Config(&handlerMCO_2);

}
