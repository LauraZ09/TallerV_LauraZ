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

//Definición de otras variables necesarias para el desarrollo de los ejercicios:
uint8_t positionDataToSend = 0;  //Esta variable almacenará la posición que se quiere transmitir del arreglo dataToSend1[]
uint8_t userButtonState    = 0;  //En esta variable se almacenará el estado del userButton

//Arreglo de datos a transmitir, cada posición del arreglo contiene un caracter de 8 bits que se envía individualmente:
char dataToSend1[]         = "Princess Consuela BananaHammock.";

//Definición de la cabecera de las funciones que se crean para el desarrollo de los ejercicios
void initSystem(void);             //Función para inicializar el sistema
void movePositionDataToSend(void); //Función que mueve la posición a enviar de arreglo de datos

int main(void) {

	//PREGUNTA: ESTÁ BIEN DEFINIDA?
	initSystem();  //Se inicializa el sistema, con la configuración de los periféricos que se van a usar

	//PREGUNTA: PUEDO BORRAR ESTE COMENTARIO?
	/* Loop forever */
	while (1) {
	}

	//PREGUNTA: CUÁNDO PUEDO QUITAR EL RETURN, PUES, EN QUÉ CASOS?
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

	    //Se configura el TxPin (PIN por el cual se hace la transmisión)
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

	    //Se configura el UserButton
		handlerUserButton.pGPIOx 							 = GPIOC;
		handlerUserButton.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_13;
		handlerUserButton.GPIO_PinConfig.GPIO_PinMode   	 = GPIO_MODE_IN;
		handlerUserButton.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
		handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed 		 = GPIO_OSPEED_FAST;
		handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;

		//Se carga la configuración
		GPIO_Config(&handlerUserButton);

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
		handlerUsart6.USART_Config.USART_baudrate = USART_BAUDRATE_115200; //115200 bps
		handlerUsart6.USART_Config.USART_datasize = USART_DATASIZE_9BIT;   //Size: 9 bits(8 + 1 parity bit)
		handlerUsart6.USART_Config.USART_parity   = USART_PARITY_EVEN;     //Parity:EVEN
		handlerUsart6.USART_Config.USART_stopbits = USART_STOPBIT_1;	   //Un stopbit

		//Se carga la configuración del USART
		USART_Config(&handlerUsart6);
}
//PREGUNTA: LA DISTRIBUCIÓN DEL ARCHIVO ESTÁ BIEN?

//DESARROLLO DE LA TAREA:

/*EJERCICIO 1:Blinky de 250ms. Para el desarrollo de este ejercicio, se utiliza la Función Callback del
 * timer. La cabecera de esta función ya se encuentra definida en la librería PeripheralDrivers, así, no
 * es necesario definirla en este archivo MainSolucionTarea3.c.
 *
 * Para desarrollar el Blinky lo que se hace es que en la función callback se le indica al programa que
 * con cada update (es decir, cada 250ms) se cambie el estado del BlinkyPin, para esto se usa la función
 * GPIOxTooglePin desarrollada con anterioridad (Tarea 2) y la cual está ya definida en la librería.
 */

/*EJERCICIO 2:Enviar un carácter cada 250ms. Al igual que en el ejercicio anterior, el desarrollo de este
 * ejercicio también será con la función Callback del timer. Para esto, dentro de la función de callback
 * seescribe el siguiente código:
 *
 * writeChar(&handlerUsart6, dataToSend1[positionDataToSend]);
 *
 * Este código escribe la posición correspondiente al valor de positionDataToSend del arreglo dataToSend1
 * en el Data Register (DR) del USART6. Así, se da la transmisión y al estar dentro del callback, el Char
 * se escribirá cada 250ms que es el período de update.
 */

/*EJERCICIO 3:Hacer que el carácter cambie si el botón USER_BUTTON es presionado. Para este ejercicio,
 * además de que se utilizará la función callback, se utilizará una función auxiliar para cambiar el
 * carácter que se envía, esta función es: movePositionDataToSend.
 *
 * La función movePositionDataToSend, es una función que no recibe ni retorna ningún dato y su función
 * es cambiar el carácter que se envía cuando el USER_BUTTON está presionado. Su funcionamiento es el
 * siguiente:
 *
 * 1. Primero la función debe verificar cuál es el estado del USER_BUTTON, para esto, se
 * lee el estado con la función GPIO_ReadPin que se encuentra definida en la librería PeripheralDrivers,
 * el estado del PIN se almacena en la variable userButtonState.
 *
 * 2. La función realizará diferentes acciones dependiendo del valor de las variables userButtonState y
 * positionDataToSend (recordar que en esta variable se almacena ela posición del arreglo de datos que
 * se quiere enviar por el PIN TX, esta variable inicialmente se inicializa en 0):
 *
 *   - Si el userButton está presionado (userButtonState == 0) y el mensaje completo aún no se ha enviado
 *     (es decir, la variable positionDataToSend aún no ha llegado a su valor máximo que es 31 ya que son
 *     32 caracteres), entonces la función le sumará 1 a la variable positionDataToSend, así, la función
 *     writeChar escribirá el caracter correspondiente a la siguiente posición en el DR.
 *
 *   - Si el userButton está presionado y ya se envió el mensaje completo (positionDataToSend>=31), se
 *     reinicia la variable positionDataToSend, es decir, se escribe en 0, así se vuelve a empezar a
 *     enviar el mensaje desde la posición 0.
 *
 *   - En cualquier otro caso, es decir cuando el botón no está presionado, la variable positionDataToSend
 *     se pone en 0. Así, cuando el botón no está presionado se envía repetidamente el carácter 0 del
 *     arreglo.
 *
 * A continuación se define esta función. Es importante resaltar que esta función es llamada en la función
 * callback del timer, así en cada update (cada 250ms) se anallizará el valor del userbutton y se decidirá
 * cómo es el envío de los carácteres.*/


void movePositionDataToSend(void) {
	userButtonState = GPIO_ReadPin(&handlerUserButton); //Se lee el estado del USER_BUTTON

	if (userButtonState == 0 && positionDataToSend<31){
		positionDataToSend++;   //Si está presionado se cambia al próximo carácter
	}
	else if (userButtonState == 0 && positionDataToSend>=31) {
		positionDataToSend = 0; //Si ya se envió el mensaje completo, se vuelve a empezar en la pos 0
	}
	else {
		positionDataToSend = 0; //Si no está presionado se envía repetidamente el carácter en la pos 0
	}
}

void BasicTimer2_Callback(void) {
	//En la siguiente línea se le indica al programa que con cada update se cambie el estado del BlinkyPin
	GPIOxTooglePin(&handlerBlinkyPin);
	//A continuación se le indica al programa que con cada update escriba la posición correspondiente
	//al valor de positionDataToSend del arreglo dataToSend en el DR del USART6
	writeChar(&handlerUsart6, dataToSend1[positionDataToSend]);
	//En la próxima línea se le indica al programa que en cada update lea el estado del USER_BUTTON para
	//decidir cómo es el envío de datos
	movePositionDataToSend();
}





