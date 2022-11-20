/**
 *********************************************************************************************************
 *********************************************************************************************************
 * @file           : MainTarea5.c
 * @author         : Laura Alejandra Zuluaga Moreno - lazuluagamo@unal.edu.co
 * @brief          : Tarea 5
 * NOTA: importar la librería PeripheralDrivers
 *********************************************************************************************************
 */
/* En esta tarea se cambia la velocidad del micro y se prueba la comunicación serial
 * y los TIMERS.
 *
 * Para el cambio de la velocidad del micro se creó una función setTo100M(), la cual
 * se llama en el main antes de la función initSystem, esta función se encuentra definida
 * en el driver setTo100M.c y cambia la frecuencia del micro a 100 MHz.
 *
 * Para comprobar el adecuado funcionamiento de la comunicación serial, cuando la función
 * initSystem llega a su fin, se envía un mensaje que dice "La funcion de iniciliazacion de
 * sistema se ha aplicado correctamente"; adicionalmente, se crearon los siguientes comandos:
 *
 * 1. help @: este comando imprime el menú de comandos
 * 2. setHour #Horas #minutos @: con este comando se puede configurar la hora inicial
 *    del RTC.
 * 3. getHour @: con este comando se obtiene la hora del RTC.
 *
 * Para comprobar el adecuado funcionamiento de los timers se hace un blinky de 250 ms con
 * el TIMER 2 y, adicionalmente, se usan las interrupciones de este timer para imprimir la
 * hora por el serial cada segundo (cuando se ha ingresado el comando getHour).
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "I2CDriver.h"
#include "PwmDriver.h"
#include "DriverRTC.h"
#include "LCD2004A.h"
#include "string.h"
#include "RccConfig.h"


//Definición de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin          		= {0}; //Handler para el USER_LED
GPIO_Handler_t handlerTxPin              		= {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerRxPin              		= {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerMCO_2 				    = {0}; //Handler para el PIN MCO2, por este pin se ve la señal del reloj

USART_Handler_t handlerUsart2            		= {0}; //Handler para el USART2

BasicTimer_Handler_t handlerBlinkyTimer  		= {0}; //Handler para el TIMER2, con este se hará el Blinky

Hour_and_Date_Config_t handlerHourDateConfig	= {0}; //Handler para la configuración de la hora

uint8_t horas 			 = 0;	 //Para almacenar las horas
uint8_t minutos 		 = 0; 	 //Para almacenar los minutos
uint8_t segundos 		 = 0; 	 //Para almacenar los segundos
uint8_t rxData        	 = 0;    //Datos de recepción
uint8_t rxDataFlag 		 = 0;	 //Bandera para la recepción de datos del usart6
uint8_t getHourFlag 	 = 0;    //Bandera para empezar a enviar la hora
uint8_t getHourCounter   = 0;    //Contador para actualizar la hora
uint8_t counterReception = 0;    //Contador para la recepción de datos por el usart2

bool stringComplete 	 = false;//Bandera para la recepción de datos del usart2

char Buffer[64]          = {0}; //En esta variable se almacenarán mensajes a enviar
char bufferReception[64] = {0}; //En esta variable se almacenan variables de recepción
char userMsg[64]         = {0}; //En esta variable se almacenan mensajes ingresados por la terminal serial
char cmd[64]             = {0};	//En esta variable se almacenan los comandos ingresados por el usuario

unsigned int firstParameter  = 0; //En esta variable se almacena el número ingresado por la terminal serial
unsigned int secondParameter = 0; //En esta variable se almacena el segundo número ingresado por la terminal serial


//Definición de la cabecera de las funciones que se crean para el desarrollo de los ejercicios
void initSystem(void);       				    //Función para inicializar el sistema
void parseCommands(char* ptrBufferReception);	//Función para evaluar los comandos que la consola recibe
void printHour(uint8_t hours, uint8_t minutes, uint8_t seconds); //Función que imprime la hora en un formato adecuado

int main(void) {

	setTo100M();
	initSystem();  //Se inicializa el sistema, con la configuración de los periféricos que se van a usar


	while (1) {

		//el sistema está constantemente verificando si se levanta la bandeta de recepción del USART6
		if (rxDataFlag == 1) {

			rxDataFlag = 0;         //Se baja la bandera
			rxData = getRxData();	//Se guarda la información recibida en una variable auxiliar
			if (rxData != '\0') {   //Si el carácter recibido es \0 estamos al final del string

				for (uint8_t j; j <= 64; j++) {  //Este ciclo llena el bufferReception de 0 para que quede limpio
					bufferReception[j] = 0;
				}

				bufferReception[counterReception] = rxData;  //Cuando se llega al final del string, se almacena en el buffer
				counterReception++;

				if (rxData == '@') {
					stringComplete = true;                    //Cuando se reconoce el final del comando, selevanta una bandera
					bufferReception[counterReception] = '\0';
					counterReception = 0;
				}

				rxData = '\0';
			}

			if (stringComplete) {                //Si el string está completo, se aplica la función parseCommands al buffer, para anlizar el comando recibido
				parseCommands(bufferReception);
				stringComplete = false;			 //Se baja la bandera
			}
			rxData = '\0';
		}

		else {

		}

		if((getHourCounter == 4) && (getHourFlag == 1)){

			getHourCounter = 0;

			//Se obtiene la hora de los registros
			horas = RTC_Get_Hours();
			minutos = RTC_Get_Minutes();
			segundos = RTC_Get_Seconds();
			printHour(horas, minutos, segundos);

			//Se imprime por el serial
			writeMsg(&handlerUsart2, Buffer);
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
	//Este PIN se configura en la función alternativa AF08 que corresponde al USART6
	handlerTxPin.pGPIOx 							= GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;    //Función alternativa
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;	              //AF08: Usart6

	//Se carga la configuración
	GPIO_Config(&handlerTxPin);

	//Se configura el TxPin (PIN por el cual se hace la transmisión)
	//Este PIN se configura en la función alternativa AF08 que corresponde al USART6
	handlerRxPin.pGPIOx 							= GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_3;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;    //Función alternativa
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;	              //AF08: Usart6

	//Se carga la configuración
	GPIO_Config(&handlerRxPin);

	//Se configura el PIN por donde se ve la señal del reloj
	handlerMCO_2.pGPIOx 						    = GPIOC;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinNumber 		= PIN_9;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinAltFunMode  = AF0;

	//Se carga la configuración
	GPIO_Config(&handlerMCO_2);

	//Se configura el BlinkyTimer
	handlerBlinkyTimer.ptrTIMx 					= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_100M_05ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 	= 500; //Update period= 2ms*125 = 250ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerBlinkyTimer);


	//Se configura el USART 2
	handlerUsart2.ptrUSARTx					     = USART2;                	  //USART 2
	handlerUsart2.USART_Config.USART_mode 	     = USART_MODE_RXTX;       	  //Modo de Recepción y transmisión
	handlerUsart2.USART_Config.USART_baudrate    = USART_BAUDRATE_115200; 	  //57600 bps
	handlerUsart2.USART_Config.USART_parity      = USART_PARITY_EVEN;         //Parity:EVEN, acá viene configurado el tamaño de dato
	handlerUsart2.USART_Config.USART_stopbits    = USART_STOPBIT_1;	          //Un stopbit
	handlerUsart2.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE; //Interrupción de recepción del usart habilitada

	//Se carga la configuración del USART
	USART_Config(&handlerUsart2);

	//configuración inicial del RTC
	handlerHourDateConfig.Hours        = 11; //Por defecto se pone la hora 00:00:00
	handlerHourDateConfig.Minutes      = 15;
	handlerHourDateConfig.Seconds      = 0;
	handlerHourDateConfig.Month        = 11;
	handlerHourDateConfig.DayOfWeek    = 4;
	handlerHourDateConfig.NumberOfDay  = 3;
	handlerHourDateConfig.Year		   = 22;

	//Se carga la configuración inicial del RTC
	enableRTC(&handlerHourDateConfig);

	//Se envía mensaje por el serial
	writeMsg(&handlerUsart2, "La funcion de iniciliazacion de sistema se ha aplicado correctamente\n\r");

}

//Función Callback del BlinkyTimer
void BasicTimer2_Callback(void) {
	//Blinky del LED de estado
	GPIOxTooglePin(&handlerBlinkyPin);
	getHourCounter++;
}

/*Función Callback de la recepción del USART6
El puerto es leído en la ISR para bajar la bandera de la interrupción
El carácter que se lee es devuelto por la función getRxData*/
void usart2Rx_Callback(void){
	//Activamos una bandera, dentro de la función main se lee el registro DR lo que baja la bandera de la interrupción
	rxDataFlag = 1;
}


void parseCommands(char* ptrBufferReception){
	//Esta función lee lo obtenido por el puerto serial y toma decisiones en base a eso

	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);

	if (strcmp(cmd, "help") == 0){
		//getHourFlag se pone en 1
		getHourFlag = 0;

		//Se escribe el menú de comandos en la terminal serial
				writeMsg(&handlerUsart2, "Menu de Comandos e instrucciones:\n\r");
				writeMsg(&handlerUsart2, "COMANDOS:\n\r1).  help -- Este comando imprime este menu\n");
				writeMsg(&handlerUsart2, "2).  setHour #Horas #Minutos -- Este comando se utiliza para introducir la hora inicial (horas y minutos) en formato 24 horas\n");
				writeMsg(&handlerUsart2, "3).  getHour -- Este comando se utiliza para imprimir la hora en formato 24 horas\n");
	}

	else if (strcmp(cmd, "setHour") == 0){
			//Se apagan las banderas de los otros modos
			getHourFlag = 0;

			//Se llena la configuración del RTC con los parámetros recibidos por el serial
			if ((0 > firstParameter) || (firstParameter > 24)) {
				writeMsg(&handlerUsart2, "Por favor, ingrese una hora valida \n\r");
			}

			else if ((0 > secondParameter) || (secondParameter > 60)){
				writeMsg(&handlerUsart2, "Por favor, ingrese una hora valida \n\r");
			}

			else{

				handlerHourDateConfig.Hours = firstParameter;
				handlerHourDateConfig.Minutes = secondParameter;
				handlerHourDateConfig.Seconds = 0;

				//Se activa el RTC para poder escribir en los registros la config ingresada
				enableRTC(&handlerHourDateConfig);

				//Se obtienen los valores de los registros
				segundos = RTC_Get_Seconds();
				minutos = RTC_Get_Minutes();
				horas = RTC_Get_Hours();
				printHour(horas, minutos, segundos);

				//Se imprime la hora ingresada por el serial
				writeMsg(&handlerUsart2, "La hora ingresada es\n");
				writeMsg(&handlerUsart2, Buffer);
			}
	}

	else if (strcmp(cmd, "getHour") == 0){
		//getHourFlag se pone en 1
		getHourFlag = 1;
		getHourCounter = 0;
	}

	else {

		getHourFlag = 0;
		//Se escribe el mensaje de error por el serial y en la LCD
		writeMsg(&handlerUsart2, "ERROR\n\r");

	}

}


void printHour(uint8_t hours, uint8_t minutes, uint8_t seconds){
	//En esta función se imprime la hora de forma adecuada

	if((minutes/10 == 0) & (seconds/10 == 0) & (horas/10 == 0)){
		//Si el número de minutos y segundos es menor que 10, se les pone un 0 adelante
		sprintf(Buffer, " \n\rHora:0%d:0%d:0%d\n\r", horas, minutos, segundos);
	}

	else if((minutes/10 == 0) & (horas/10 == 0)){
			//Si el número de minutos y segundos es menor que 10, se les pone un 0 adelante
			sprintf(Buffer, " \n\rHora:0%d:0%d:%d\n\r", horas, minutos, segundos);
		}


	else if((minutes/10 == 0) & (segundos/10 == 0)){
			//Si el número de minutos y segundos es menor que 10, se les pone un 0 adelante
			sprintf(Buffer, " \n\rHora:%d:0%d:0%d\n\r", horas, minutos, segundos);
		}

	else if((horas/10 == 0) & (segundos/10 == 0)){
			//Si el número de minutos y segundos es menor que 10, se les pone un 0 adelante
			sprintf(Buffer, " \n\rHora:0%d:%d:0%d\n\r", horas, minutos, segundos);
		}

	else if((minutes/10 == 0)){
		//Si el número de minutos es menor que 10 se le pone un 0 adelante
		sprintf(Buffer, " \n\rHora:%d:0%d:%d\n\r", horas, minutos, segundos);
	}

	else if((horas/10 == 0)){
		//Si el número de minutos es menor que 10 se le pone un 0 adelante
		sprintf(Buffer, " \n\rHora:0%d:%d:%d\n\r", horas, minutos, segundos);
	}

	else if((seconds/10 == 0)){
		//Si el número de segundos es menor que 10 se le pone un 0 adelante
		sprintf(Buffer, " \n\rHora:%d:%d:0%d\n\r", horas, minutos, segundos);
	}
	else {
		//En otro caso se imprime los datos normales obtenidos de los registros del RTC
		sprintf(Buffer," \n\rHora:%d:%d:%d\n\r", horas, minutos, segundos);
	}
}

