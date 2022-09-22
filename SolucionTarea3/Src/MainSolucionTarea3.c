/**
 *********************************************************************************************************
 *********************************************************************************************************
 * @file           : MainSolucionTarea3.c
 * @author         : Laura Alejandra Zuluaga Moreno - lazuluagamo@unal.edu.co
 * @brief          : Solución Tarea 3:
 * 					 -Hacer un Blinky de 250ms con el USER_LED
 * 					 -Enviar un carácter cada 250ms
 * 					 -Hacer que el carácter cambie si el botón USER_BUTTON es presionado
 *********************************************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"

//Definición de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin         = {0}; //Handler para el USER_LED
GPIO_Handler_t handlerUserButton        = {0}; //Handler para el USER_BUTTON
GPIO_Handler_t handlerTxPin             = {0}; //Handler para el PIN por el cual se hará la transmisión
BasicTimer_Handler_t handlerBlinkyTimer = {0}; //Handler para el TIMER2, con este se hará el Blinky
USART_Handler_t handlerUsart6           = {0}; //Handler para el USART

//Definición de otras variables
char dataToSend1[] = "Princess Consuela BananaHammock";

//Definición de funciones
void initSystem(void);


int main(void) {

	initSystem(); //Se inicializa el sistema, con la configuración de los periféricos que se van a usar

	/* Loop forever */
	while (1) {
	}
	return 0;
}

//Función que inicializa el sistema con la configuración de los periféricos a usar
void initSystem(void) {

	//Se configura el BlinkyPin
		handlerBlinkyPin.pGPIOx 							= GPIOA;
		handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_5;
		handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
		handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

		//Se carga la configuración y se enciende el LED
		GPIO_Config(&handlerBlinkyPin);
		GPIO_WritePin(&handlerBlinkyPin, RESET);

	//Se configura el TxPin (PIN por el cual se hace la transmisión).
		//Este PIN se configura en la función alternativa AF08 que para el PIN C6 corresponde al USART6
		handlerTxPin.pGPIOx 							= GPIOC;
		handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_6;
		handlerTxPin.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;    //Función alternativa
		handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
		handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
		handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
		handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode  = AF8;				  //AF08 para PIN C6

		//Se carga la configuración
		GPIO_Config(&handlerTxPin);

	//Se configura el BlinkyTimer
		handlerBlinkyTimer.ptrTIMx 					= TIM2;
		handlerBlinkyTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
		handlerBlinkyTimer.TIMx_Config.TIMx_speed	= BTIMER_SPEED_100us;
		handlerBlinkyTimer.TIMx_Config.TIMx_period 	= 2500;//Update period= 100us*2500 = 250000us = 250ms

		//Se carga la configuración del Timer
		BasicTimer_Config(&handlerBlinkyTimer);

	//Se configura el USART 6
		handlerUsart6.ptrUSARTx 				  = USART6;                //USART 6
		handlerUsart6.USART_Config.USART_mode     = USART_MODE_TX;         //Modo de solo transmisión
		handlerUsart6.USART_Config.USART_baudrate = USART_BAUDRATE_9600;   //9600 bps
		handlerUsart6.USART_Config.USART_datasize = USART_DATASIZE_9BIT;   //Size: 9 bits(8 + 1 parity bit)
		handlerUsart6.USART_Config.USART_parity   = USART_PARITY_EVEN;     //Parity:EVEN
		handlerUsart6.USART_Config.USART_stopbits = USART_STOPBIT_1;	   //Un stopbit

		//Se carga la configuración del USART
		USART_Config(&handlerUsart6);
}


/*EJERCICIO 1:Blinky de 250ms. Para el desarrollo de este ejercicio, se utiliza la Función Callback del
 * timer. La cabecera de esta función ya se encuentra definida en la librería PeripheralDrivers, así, no
 * es necesario definirla en este archivo MainSolucionTarea3.c.
 *
 * Para desarrollar el Blinky lo que se hace es que en la función callback se le indica al programa que
 * con cada update (es decir, cada 250ms) se cambie el estado del BlinkyPin, para esto se usa la función
 * GPIOxTooglePin desarrollada con anterioridad (Tarea 2) y la cual está ya definida en la librería.
 */

/*EJERCICIO 2:Enviar un carácter cada 250ms. Al igual que en el ejercicio anterior, el desarrollo de este
 * ejercicio también será con la función Callback el timer. Para esto, dentro de la función de callback se
 * escribe el siguiente código:
 *
 * writeChar(&handlerUsart6, dataToSend1);
 *
 * Este código escribe el Char en el DR del USART6. Así, se da la transmisión y al estar dentro del
 * callback, el Char se escribirá cada 250ms que es el período de update.
 */

void BasicTimer2_Callback(void) {
	//En la siguiente línea se le indica al programa que con cada update se cambie el estado del BlinkyPin
	GPIOxTooglePin(&handlerBlinkyPin);
	//A continuación se le indica al programa que con cada update escriba dataToSend1 en el DR del USART6
	writeChar(&handlerUsart6, dataToSend1[0]);
}



