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
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"
#include "ExtiDriver.h"
#include "PwmDriver.h"


//Definición de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin          = {0}; //Handler para el USER_LED
GPIO_Handler_t handlerTxPin              = {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerRxPin              = {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerBlueRGB            = {0}; //Handler para el azul del RGB
GPIO_Handler_t handlerRedRGB             = {0}; //Handler para el rojo del RGB
GPIO_Handler_t handlerGreenRGB           = {0}; //Handler para el verde del RGB
USART_Handler_t handlerUsart2            = {0}; //Handler para el USART2
BasicTimer_Handler_t handlerBlinkyTimer  = {0}; //Handler para el TIMER2, con este se hará el Blinky
PWM_Handler_t handlerPWMTimerB 	         = {0}; //Handler para el PWM (Timer)
PWM_Handler_t handlerPWMTimerR 	         = {0}; //Handler para el PWM (Timer)
PWM_Handler_t handlerPWMTimerG 	         = {0}; //Handler para el PWM (Timer)

//Definición de otras variables necesarias para el desarrollo de los ejercicios:
uint8_t rxData     		  = 0;      //Datos de recepción
uint32_t duttyCicleValueB = 0;
uint32_t duttyCicleValueG = 0;
uint32_t duttyCicleValueR = 0;

char greetingMsg[] = "SIUU \n\r"; //Mensaje que se imprime

//Definición de la cabecera de las funciones que se crean para el desarrollo de los ejercicios
void initSystem(void);       //Función para inicializar el sistema

int main(void) {

	initSystem();  //Se inicializa el sistema, con la configuración de los periféricos que se van a usar
	writeChar(&handlerUsart2,' ');

	while (1) {
		//El sistema siempre está verificando si el valor de rxData ha cambiado
		//(lo cual sucede en la ISR de la recepción)
		//Si este valor dejó de ser '\0' significa que se recibió un carácter
		//por lo tanto entra en el bloque if para analizar que se recibió
		if(rxData != '\0'){

			//Se imprime el carácter recibido
			writeChar(&handlerUsart2, rxData);

			duttyCicleValueR = getDuttyCycleValue(&handlerPWMTimerR);
			duttyCicleValueG = getDuttyCycleValue(&handlerPWMTimerG);
			duttyCicleValueB = getDuttyCycleValue(&handlerPWMTimerB);

			switch (rxData) {

			case 'm': {

				writeMsg(&handlerUsart2, greetingMsg);

				break;
			}

			case 'B': {
				if (duttyCicleValueB < 256) {
					updateDuttyCycle(&handlerPWMTimerB, duttyCicleValueB + 1);
				}

				else {
					__NOP();
				}

				break;
			}

			case 'b': {
				if (0 < duttyCicleValueB) {
					updateDuttyCycle(&handlerPWMTimerB, duttyCicleValueB - 1);
				}

				else {
					__NOP();
				}

				break;
			}

			case 'R': {
				if (duttyCicleValueR < 256) {
					updateDuttyCycle(&handlerPWMTimerR, duttyCicleValueR + 1);
				}

				else {
					__NOP();
				}

				break;
			}

			case 'r': {
				if (0 < duttyCicleValueR) {
					updateDuttyCycle(&handlerPWMTimerR, duttyCicleValueR - 1);
				}

				else {
					__NOP();
				}

				break;
			}

			case 'G': {
				if (duttyCicleValueG < 256) {
					updateDuttyCycle(&handlerPWMTimerG, duttyCicleValueG + 1);
				}

				else {
					__NOP();
				}

				break;
			}

			case 'g': {
				if (0 < duttyCicleValueG) {
					updateDuttyCycle(&handlerPWMTimerG, duttyCicleValueG - 1);
				}

				else {
					__NOP();
				}

				break;
			}

			default: {
				__NOP();

				break;
			}
			}

			rxData = '\0';
		}

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

	//Se configura el TxPin (PIN por el cual se hace la transmisión)
	//Este PIN se configura en la función alternativa AF07 que corresponde al USART2
	handlerTxPin.pGPIOx 							= GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;    //Función alternativa
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;	              //AF07: Usart2

	//Se carga la configuración
	GPIO_Config(&handlerTxPin);

	//Se configura el TxPin (PIN por el cual se hace la transmisión)
	//Este PIN se configura en la función alternativa AF07 que corresponde al USART2
	handlerRxPin.pGPIOx 							= GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_3;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;    //Función alternativa
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;	              //AF07: Usart2

	//Se carga la configuración
	GPIO_Config(&handlerRxPin);

	//Se configura el BlinkyTimer
	handlerBlinkyTimer.ptrTIMx 					= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 	= 2500; //Update period= 100us*2500 = 250000us = 250ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerBlinkyTimer);

	//Se configura el USART 2
	handlerUsart2.ptrUSARTx					     = USART2;                	  //USART 2
	handlerUsart2.USART_Config.USART_mode 	     = USART_MODE_RXTX;       	  //Modo de Recepción y transmisión
	handlerUsart2.USART_Config.USART_baudrate    = USART_BAUDRATE_115200; 	  //115200 bps
	handlerUsart2.USART_Config.USART_parity      = USART_PARITY_NONE;         //Parity:NONE, acá viene configurado el tamaño de dato
	handlerUsart2.USART_Config.USART_stopbits    = USART_STOPBIT_1;	          //Un stopbit
	handlerUsart2.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE; //Interrupción de recepción del usart habilitada

	//Se carga la configuración del USART
	USART_Config(&handlerUsart2);

	//Se configura el Timer del PWM Azul
	handlerPWMTimerB.ptrTIMx 		   = TIM3;
	handlerPWMTimerB.config.channel    = PWM_CHANNEL_1;
	handlerPWMTimerB.config.prescaler  = BTIMER_SPEED_10us;
	handlerPWMTimerB.config.periodo    = 100;
	handlerPWMTimerB.config.duttyCicle = 50;

	pwm_Config(&handlerPWMTimerB);

	//Se configura el Timer del PWM Rojo
	handlerPWMTimerR.ptrTIMx 			= TIM3;
	handlerPWMTimerR.config.channel 	= PWM_CHANNEL_2;
	handlerPWMTimerR.config.prescaler   = BTIMER_SPEED_10us;
	handlerPWMTimerR.config.periodo 	= 100;
	handlerPWMTimerR.config.duttyCicle  = 10;

	pwm_Config(&handlerPWMTimerR);

	//Se configura el Timer del PWM Verde
	handlerPWMTimerG.ptrTIMx 			= TIM3;
	handlerPWMTimerG.config.channel 	= PWM_CHANNEL_3;
	handlerPWMTimerG.config.prescaler   = BTIMER_SPEED_10us;
	handlerPWMTimerG.config.periodo 	= 100;
	handlerPWMTimerG.config.duttyCicle  = 10;

	pwm_Config(&handlerPWMTimerG);

	//Se configura la salida del PWM Azul:
	handlerBlueRGB.pGPIOx 						      = GPIOC;
	handlerBlueRGB.GPIO_PinConfig.GPIO_PinNumber      = PIN_6;
	handlerBlueRGB.GPIO_PinConfig.GPIO_PinMode 	      = GPIO_MODE_ALTFN;
	handlerBlueRGB.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerBlueRGB.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerBlueRGB.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerBlueRGB.GPIO_PinConfig.GPIO_PinAltFunMode  = AF2; //AF02: TIM5_CH1;

	GPIO_Config(&handlerBlueRGB);

	//Se configura la salida del PWM Verde:
	handlerGreenRGB.pGPIOx 						      = GPIOC;
	handlerGreenRGB.GPIO_PinConfig.GPIO_PinNumber     = PIN_8;
	handlerGreenRGB.GPIO_PinConfig.GPIO_PinMode 	  = GPIO_MODE_ALTFN;
	handlerGreenRGB.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerGreenRGB.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerGreenRGB.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_NOTHING;
	handlerGreenRGB.GPIO_PinConfig.GPIO_PinAltFunMode = AF2; //AF02: TIM5_CH1;

	GPIO_Config(&handlerGreenRGB);

	//Se configura la salida del PWM Rojo:
	handlerRedRGB.pGPIOx 						      = GPIOC;
	handlerRedRGB.GPIO_PinConfig.GPIO_PinNumber       = PIN_7;
	handlerRedRGB.GPIO_PinConfig.GPIO_PinMode 	      = GPIO_MODE_ALTFN;
	handlerRedRGB.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerRedRGB.GPIO_PinConfig.GPIO_PinSpeed 	      = GPIO_OSPEED_FAST;
	handlerRedRGB.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerRedRGB.GPIO_PinConfig.GPIO_PinAltFunMode   = AF2; //AF02: TIM5_CH1;

	GPIO_Config(&handlerRedRGB);

	enableOutput(&handlerPWMTimerG);
	enableOutput(&handlerPWMTimerR);
	enableOutput(&handlerPWMTimerB);

	startPwmSignal(&handlerPWMTimerG);
	startPwmSignal(&handlerPWMTimerR);
	startPwmSignal(&handlerPWMTimerB);
}

//Función Callback del BlinkyTimer
void BasicTimer2_Callback(void) {
	//Blinky del LED de estado
	GPIOxTooglePin(&handlerBlinkyPin);
}

/*Función Callback de la recepción del USART2
El puerto es leído en la ISR para bajar la bandera de la interrupción
El carácter que se lee es devuelto por la función getRxData*/
void usart2Rx_Callback(void){
	//Leemos el valor del registro DR, donde se almacena el dato que llega.
	//Esto además debe bajar la bandera de la interrupción
	rxData = getRxData();
}









