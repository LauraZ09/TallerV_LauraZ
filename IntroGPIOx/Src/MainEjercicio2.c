/*
 * GPIOxDriverMain.c
 *
 *  Created on: 28/08/2022
 *      Author: Laura Zuluaga
 */

#include <stdint.h>
#include "stm32f411xx.h"
#include "GPIOxDriver.h"

int main (void) {

	//Definimos el handler para el pin que deseamos configurar
	GPIO_Handler_t handlerUserLedPin = {0};
	GPIO_Handler_t handlerPushButton = {0};

	//Deseamos trabajar con el puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode 		    = GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed 		    = GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;

	//Deseamos trabajar con el puerto GPIOC
	handlerPushButton.pGPIOx = GPIOC;
	handlerPushButton.GPIO_PinConfig.GPIO_PinNumber 		= PIN_13;
	handlerPushButton.GPIO_PinConfig.GPIO_PinMode 		    = GPIO_MODE_IN;
	handlerPushButton.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerPushButton.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_PULLUP;
	handlerPushButton.GPIO_PinConfig.GPIO_PinSpeed 		    = GPIO_OSPEED_MEDIUM;
	handlerPushButton.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;

	//Cargamos la configuración del PIN específico
	GPIO_Config(&handlerUserLedPin);
	GPIO_Config(&handlerPushButton);

	//Hacemos que el pin a5 quede encendido
	GPIO_WritePin (&handlerUserLedPin, 0);

	//Este es el ciclo principal, donde se ejecuta todo el programa
	while (1){
		unsigned long long i = 0;
		unsigned char pinState = 0;
		pinState = GPIO_ReadPin (&handlerPushButton);

		if (pinState == 0) {
			GPIO_WritePin (&handlerUserLedPin, 1);
			for (i = 0; i <= 100000; i++);
			GPIO_WritePin (&handlerUserLedPin, 0);
			for (i = 0; i <= 100000; i++);

		}
		else if (pinState == 1) {
			GPIO_WritePin (&handlerUserLedPin, 1);
			for (i = 0; i <= 200000; i++);
			GPIO_WritePin (&handlerUserLedPin, 0);
			for (i = 0; i <= 200000; i++);
			//Debe durar 200 ms
			}
	}
}
