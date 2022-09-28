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

//TODO IMPORTANTE PREGUNTAR LO DEL PARITY BIT EN PULSE VIEW
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
uint8_t interruptionFlag   = 0;  //Esta variable se utilizará como bandera para poder trabajar con las interrupciones

//Arreglo de datos a transmitir, cada posición del arreglo contiene un caracter de 8 bits que se envía individualmente:
char dataToSend1[]         = "Princess Consuela BananaHammock.";

//Definición de la cabecera de las funciones que se crean para el desarrollo de los ejercicios
void initSystem(void);             //Función para inicializar el sistema
void movePositionDataToSend(void); //Función que mueve la posición a enviar del arreglo de datos


//DESARROLLO DE LA TAREA: TODO PREGUNTAR SI LE MOLESTA QUE ALL EL DESARROLLO ESTÉ ACÁ

/*EJERCICIO 1:Blinky de 250ms. Para el desarrollo de este ejercicio, se utiliza la Función Callback del
 * timer. La cabecera de esta función ya se encuentra definida en la librería PeripheralDrivers, así, no
 * es necesario definirla en este archivo MainSolucionTarea3.c.
 *
 * Para desarrollar el Blinky lo que se hace es que en la función callback se le indica al programa que
 * con cada update (es decir, cada 250ms) se cambie el estado del BlinkyPin, para esto se usa la función
 * GPIOxTooglePin desarrollada con anterioridad (Tarea 2) y la cual está ya definida en la librería.
 */

/* OBSERVACIÓN: Para el desarrollo de los ejercicios 2 y 3 de la tarea se definió la función
 * movePositionDataToSend, el funcionamiento de esta función es descrito a continuación.
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
 * positionDataToSend (recordar que en esta variable se almacena la posición del arreglo de datos que
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
 */

/* EJERCICIO 2:Enviar un carácter cada 250ms. Para el desarrollo de este ejercicio se usa la variable
 * interruptionFlag a modo de bandera, esta variable se inicializa en 0 al inicio de este código y
 * en el callback del timer se cambia este valor a 1, así, en cada update del timer se sube la bandera.
 *
 * En el ciclo infinito de la función main, se tiene un if en el cual, cuando la bandera es igual a 1
 * se corren las funciones movePositionDataToSend y writeChar, y luego, se restablece la bandera en 0.
 * De este modo, con cada update la bandera cambiará su estado a 1, así, se aplicará el código del main,
 * en el que al estar la bandera en 1 se activa el envío de datos y se restablecerá en 0 la bandera.
 *
 * Para el envío de datos se usa el siguiente código:
 *
 * 		movePositionDataToSend();
		writeChar(&handlerUsart6, dataToSend1[positionDataToSend]);
 *
 * Este código básicamente, primero aplica la función movePositionDataToSend en la que se define la
 * posición del carácter que se va a enviar y después escribe la posición correspondiente al valor de
 * positionDataToSend del arreglo dataToSend1 en el Data Register (DR) del USART6
 *
 * Al estarse activando la bandera cada 250ms, la transmisión también se hará cada 250ms.
 */

/*EJERCICIO 3:Hacer que el carácter cambie si el botón USER_BUTTON es presionado. Para este ejercicio,
 * se usan los mismos componentes usados para el ejercicio 2; el funcionamiento es el siguiente:
 *
 * En el callback del timer se pone la bandera en 1, así se empieza a correr el código del main.
 * Dentro del main, al estar la bandera en 1 se empieza a correr la función movePositionDataToSend. Esta
 * función cambiará el valor de la variable positionDataToSend según sea el correspondiente caso. Posteriormente
 * a la aplicación de esta función, se escribe en carácter elegido por la función anterior. Finalmente, la bandera
 * se vuelve a bajar.
 *
 *En el código a continuación se encuentra el desarrollo que se describió anteriormente */

int main(void) {

	initSystem();  //Se inicializa el sistema, con la configuración de los periféricos que se van a usar

	while (1) {

		//TODO PREGUNTAR SI ES MEJOR HACER ESTO CON UN SWICTH
		if(interruptionFlag == 1) { //Cuando se activa la bandera, se empieza a aplicar el código de abajo

			//En la próxima línea se le indica al programa que lea el estado del USER_BUTTON para decidir cómo es
			//el envío de datos
			//TODO PREGUNTAR SI ES MALO USAR DOS FUNCIONES SEGUIDAS, ES INEFICIENTE?
			movePositionDataToSend();
			//A continuación se le indica al programa que escriba la posición correspondiente al valor de
			//positionDataToSend del arreglo dataToSend en el DR del USART6
			writeChar(&handlerUsart6, dataToSend1[positionDataToSend]);

			interruptionFlag = 0; //Se baja la bandera
		}
		else{ //En otro caso, es decir cuando no se ha activado la bandera, no se hace nada
			__NOP();
		}
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
		handlerUsart6.USART_Config.USART_baudrate = USART_BAUDRATE_9600;   //115200 bps
		handlerUsart6.USART_Config.USART_datasize = USART_DATASIZE_8BIT;   //Size: 9 bits(8 + 1 parity bit)
		//TODO ELIMINO ESE DATASIZE? EN EL PARITY YA SE ESTÁ CONFIGURANDO AUTOMÁTICAMENTE
		handlerUsart6.USART_Config.USART_parity   = USART_PARITY_ODD;     //Parity:EVEN
		handlerUsart6.USART_Config.USART_stopbits = USART_STOPBIT_0_5;	   //Un stopbit

		//Se carga la configuración del USART
		USART_Config(&handlerUsart6);
}

//Función que mueve la posición que senvía del arreglo de datos
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

//Función Callback del timer
void BasicTimer2_Callback(void) {
	//En la siguiente línea se le indica al programa que con cada update se cambie el estado del BlinkyPin
	GPIOxTooglePin(&handlerBlinkyPin);
	//En la siguiente línea se le indica al programa que con cada update se suba la bandera
	interruptionFlag = 1;
}





