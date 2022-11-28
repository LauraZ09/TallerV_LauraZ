/**
 *********************************************************************************************************
 *********************************************************************************************************
 * @file           : MainADC.c
 * @author         : Laura Alejandra Zuluaga Moreno - lazuluagamo@unal.edu.co
 * @brief          : ADC
 *
 *********************************************************************************************************
 */


#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "PwmDriver.h"
#include "RccConfig.h"
#include "KY006Buzzer.h"

//Definición de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin          = {0}; //Handler para el USER_LED
BasicTimer_Handler_t handlerBlinkyTimer  = {0}; //Handler para el TIMER2, con este se hará el Blinky
PWM_Handler_t handlerPWMTimer 	         = {0}; //Handler para el PWM (Timer)
GPIO_Handler_t handlerPinPWMChannel      = {0}; //Handler para el PIN por el cual sale la señal PWM



//Definición de la cabecera de las funciones que se crean para el desarrollo de los ejercicios
void initSystem(void);       //Función para inicializar el sistema

int main(void) {

	setTo100M();
	initSystem();  //Se inicializa el sistema, con la configuración de los periféricos que se van a usar

	Tone(&handlerPWMTimer, 1);
	delayms(500);
	noTone(&handlerPWMTimer);
	delayms(1000);

	Tone(&handlerPWMTimer, 1);
	delayms(500);
	noTone(&handlerPWMTimer);
	delayms(1000);

	Tone(&handlerPWMTimer, 1);
	delayms(500);
	noTone(&handlerPWMTimer);
	delayms(500);

	Tone(&handlerPWMTimer, 2);
	delayms(1000);
	noTone(&handlerPWMTimer);
	delayms(1000);


	while (1) {

	}
	return 0;
}

//Función que inicializa el sistema con la configuración de los periféricos a usar
void initSystem(void) {

	//Se configura el BlinkyPin
	handlerBlinkyPin.pGPIOx 							= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;

	//Se carga la configuración
	GPIO_Config(&handlerBlinkyPin);
	GPIO_WritePin(&handlerBlinkyPin, SET);

	//Se configura el BlinkyTimer
	handlerBlinkyTimer.ptrTIMx 					= TIM10;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_100M_05ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 	= 500; //Update period= 100us*2500 = 250000us = 250ms

	//Se carga la configuración del BlinkyTimer
	Timer10_Config(&handlerBlinkyTimer);

	//Se configura el Timer del PWM
	handlerPWMTimer.ptrTIMx 		  = TIM2;
	handlerPWMTimer.config.channel 	  = PWM_CHANNEL_3;
	handlerPWMTimer.config.prescaler  = BTIMER_SPEED_100M_05ms;
	handlerPWMTimer.config.periodo 	  = 5;
	handlerPWMTimer.config.duttyCicle = 3;

	pwm_Config(&handlerPWMTimer);

    //Se configura el PIN por el que sale la señal del PWM
	handlerPinPWMChannel.pGPIOx 					        = GPIOB;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinNumber 	    = PIN_10;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinMode 	    = GPIO_MODE_ALTFN;    //Función alternativa
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinOPType 	    = GPIO_OTYPE_PUSHPULL;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinSpeed 	    = GPIO_OSPEED_FAST;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF1;

	//Se carga la configuración
	GPIO_Config(&handlerPinPWMChannel);

}

//Función Callback del BlinkyTimer
void BasicTimer10_Callback(void) {
	//Blinky del LED de estado
	GPIOxTooglePin(&handlerBlinkyPin);
}









