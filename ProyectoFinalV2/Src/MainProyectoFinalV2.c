/**
 ******************************************************************************
 * @file           : MainProyectiFinalV2.c
 * @author         : Laura Zuluaga
 * @brief          : Proyecto Final Taller V
 ******************************************************************************
 *
 ******************************************************************************
 */
/* El proyecto consiste en un juego de carreras que cuenta con varios modos de funcionamiento.
 * Para ver las instrucciones de juego se debe enviar por la terminal serial el comando:
 * help @.
 */
//Se incluyen algunas librerías de C
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//Se incluyen los drivers para los periféricos de la board de STM
#include "I2CDriver.h"
#include "RccConfig.h"
#include "PWMDriver.h"
#include "AdcDriver.h"
#include "ExtiDriver.h"
#include "BasicTimer.h"
#include "GPIOxDriver.h"
#include "USARTxDriver.h"

//Se incluyen los drivers para los elementos externos
#include "SH1106OLED.h"
#include "KY006Buzzer.h"
#include "WS2812bDriver.h"

/****************************************************************************************************************************/

//DEFINICIÓN DE VARIABLES:

//Buffer auxiliar para almacenar los colores de la cinta
uint8_t buffer[540] = {0};

//Contadores de la posición de los jugadores en la pista(número de LED en el que están)
uint8_t posP1 = 1;  //Jugador 1
uint8_t posP2 = 15; //Jugador 2
uint8_t posP3 = 25; //Jugador 3
uint8_t posP4 = 37; //Jugador 4

//Contador de vueltas para los jugadores
uint8_t lapCounterP1 = 0; //Jugador 1
uint8_t lapCounterP2 = 0; //Jugador 2
uint8_t lapCounterP3 = 0; //Jugador 3
uint8_t lapCounterP4 = 0; //Jugador 4

//Banderas auxiliares para la activavión y actualización del modo Party
uint8_t partyModeFlag 		 = 0;  //Bandera para la activación del modo Party
uint8_t partyModeUpdateFlag  = 0;  //Bandera para la actualización del modo Party

//Banderas auxiliares para la activación y actualización del modo joyStick
uint8_t joyStickModeFlag 	   = 0; //Bandera para la activación del modo JoyStick
uint8_t colorJoyStickMode      = 3; //Variable para el almacenamiento del color en el modo joystick
uint8_t joyStickModePosition   = 1; //Variable para el almacenamiento de la posición en el modo joystick
uint8_t updateJoyStickModeFlag = 0; //Bandera para la actualización del modo JoyStick

//Banderas auxiliares para el modo race
uint8_t raceModeFlagP2     = 0; //Bandera para activar el Modo de juego: 2 jugadores
uint8_t raceModeFlagP4     = 0; //Bandera para activar el Modo de juego: 4 jugadores
uint8_t updateRaceModeFlag = 0; //Bandera para la actualización de la cinta de LEDs en modo Race
uint8_t updateOLEDRaceMode = 0; //Bandera para la actualización de la OLED en el modo Race

//Banderas auxiliares para la conversión ADC
uint8_t counterADC = 0;     //Variable para almacenar el número de conversión de la secuencia
bool adcIsComplete = false; //Bandera que se levanta cuando se da una conversión ADC

//Banderas auxiliares para la recepción de comandos por el USART
uint8_t rxData           = 0;     //Datos de recepción
uint8_t rxDataFlag       = 0;	  //Bandera para la recepción de datos del usart2
uint8_t counterReception = 0;     //Contador para la recepción de datos por el usart2
bool stringComplete 	 = false; //Bandera para la recepción de datos del usart2

//Variables para la variación de la intensidad del color
uint8_t intensityConfigFlag = 0;  //Bandera para la activación del modo intensityConfig
char intensityColorCars[4]  = {255, 255, 255, 255}; //Arreglo para almacenar las intensidades

//Arreglos auxiliares
char bufferReception[200] = {0}; //En esta variable se almacenan variables de recepción
char bufferTx[200]        = {0}; //En esta variable se almacenan variables de recepción
char userMsg[64]          = {0}; //En esta variable se almacenan mensajes ingresados por la terminal serial
char cmd[64]              = {0}; //En esta variable se almacenan los comandos ingresados por el usuario

//Arreglos para la conversión ADC
uint8_t channels[6]= {ADC_CHANNEL_1, ADC_CHANNEL_4, ADC_CHANNEL_8, ADC_CHANNEL_10, ADC_CHANNEL_6, ADC_CHANNEL_7}; //Secuencia ADC
uint16_t adcData[6]= {0};  //Datos del ADC

//Variables auxiliares para la recepción de comandos
unsigned int firstParameter  = 0; //En esta variable se almacena el número ingresado por la terminal serial
unsigned int secondParameter = 0; //En esta variable se almacena el segundo número ingresado por la terminal serial

/****************************************************************************************************************************/

//DEFINICIÓN DE HANDLERS

//Handler de los timers usados
BasicTimer_Handler_t handlerBlinkyTimer = { 0 };  //Handler para el BlinkyTimer
BasicTimer_Handler_t handlerIntTimer    = { 0 };  //Handler para el Timer que actualiza el estado de la cinta LED

//Handler para el PWM de los eventos ADC
PWM_Handler_t handlerPWMTimer 			= { 0 };  //Handler para el PWM de los eventos ADC
PWM_Handler_t handlerPWMTimerBuzzer     = { 0 };  //Handler para el PWM del Buzzer

//Handler de los GPIOs usados
GPIO_Handler_t handlerBlinkyPin 	    = { 0 };  //Handler para el LED de estado
GPIO_Handler_t handlerPWMOutput 		= { 0 };  //Handler para la salida del PWM manual
GPIO_Handler_t handlerPlayer1 		    = { 0 };  //Handler para el botón del Player 1
GPIO_Handler_t handlerPlayer2 		    = { 0 };  //Handler para el botón del Player 2
GPIO_Handler_t handlerPlayer3			= { 0 };  //Handler para el botón del Player 3
GPIO_Handler_t handlerPlayer4			= { 0 };  //Handler para el botón del Player 4
GPIO_Handler_t handlerTxPin             = { 0 };  //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerRxPin             = { 0 };  //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerSCLPin			= { 0 };  //Handler para el PIN CLOCK del I2C de la OLED
GPIO_Handler_t handlerSDAPin			= { 0 };  //Handler para el PIN CLOCK del I2C de la OLED
GPIO_Handler_t handlerPinPWMChannel	    = { 0 };  //Handler para el PWM del Buzzer

//Handler para el USART2
USART_Handler_t handlerUsart2           = { 0 };  //Handler para el USART2

//Handler de las interrupciones externas
EXTI_Config_t Player1ExtiConfig         = { 0 };  //Handler para la interrupción externa del Player 1
EXTI_Config_t Player2ExtiConfig         = { 0 };  //Handler para la interrupción externa del Player 2
EXTI_Config_t Player3ExtiConfig 		= { 0 };  //Handler para la interrupción externa del Player 3
EXTI_Config_t Player4ExtiConfig 		= { 0 };  //Handler para la interrupción externa del Player 4

//Handler para el I2C de la OLED
I2C_Handler_t handlerI2COLED 			= { 0 };  //Handler para el I2C de la OLED

//Handlers para la conversión ADC y los eventos
ADC_Config_t adcConfig 					= { 0 };  //Configuración del ADC
ADC_Config_Event_t adcConfigEvent		= { 0 };  //Configuración del evento externo

//Handler para la carrera de LEDS
raceLED handlerRaceLED 					= { 0 };  //Handler para la carrera de LEDS

/****************************************************************************************************************************/

//FUNCIONES

void initSystem(void); 											//Inicialización del sistema
void parseCommands(char *ptrBufferReception); 					//Recepción de comandos
void updateRaceModeOLED(void); 									//Actualización de la OLED en modo Race
void updatePartyOLEDFunction(void);								//Actualización de la OLED en modo Party
void updateJoyStickModeOLED(void);								//Actualización de la OLED en modo JoyStick
char* intensityColorCarsFunction (uint16_t* carsIntensityADC); 	//Intensidad de los carros

/****************************************************************************************************************************/

//MAIN

int main(void)
{
	setTo100M();  //Se pone la CPU a 100MHz
	initSystem(); //Se inicializan los periféricos

	while (1)
	{
		//el sistema está constantemente verificando si se levanta la bandera de recepción del USART2
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
					stringComplete = true;                    //Cuando se reconoce el final del comando, se levanta una bandera
					bufferReception[counterReception] = '\0';
					counterReception = 0;
				}

				rxData = '\0';
			}

			if (stringComplete) {                //Si el string está completo, se aplica la función parseCommands al buffer, para anlizar el comando recibido
				delayms(10);
				parseCommands(bufferReception);
				stringComplete = false;			 //Se baja la bandera
			}
			rxData = '\0';
		}

		//CONVERSIÓN ADC PARA LA INTENSIDAD DE LOS COLORES:
		if ((adcIsComplete == true) & (intensityConfigFlag == 1)) {
			//Si la bandera intensityConfigFlag está en 1, se está en modo configuración y se están dando las conversiones ADC

			if (counterADC == 1) {
				//Si counterADC == 1 es porque se dio la primera conversión de la secuencia
				adcIsComplete = false;
				adcData[0] = getADC();
			}

			else if (counterADC == 2) {
				adcIsComplete = false;
				adcData[1] = getADC();
			}

			else if (counterADC == 3) {
				adcIsComplete = false;
				adcData[2] = getADC();
			}

			else if (counterADC == 4) {
				adcIsComplete = false;
				adcData[3] = getADC();

			}

			else if (counterADC == 5) {
				adcIsComplete = false;
				adcData[4] = getADC();

			}

			else if (counterADC == 6) {
				adcIsComplete = false;
				adcData[5] = getADC();

				//Se pone en 0 el contador
				counterADC = 0;

				//Se llama la función que da la intensidad de los carritos, según la conversión ADC
				intensityColorCarsFunction(adcData);
				//Se muestran los carritos con las intensidades recién calculadas
				showFourCarsToConfig(1, 15, 25, 35, intensityColorCars[0], intensityColorCars[1], intensityColorCars[2],
						intensityColorCars[3], &handlerPWMOutput);

			}

			else {
				adcIsComplete = false;
				counterADC = 0;
			}

		}

		//MODO PARTY:
		if(partyModeFlag & partyModeUpdateFlag){
			//Si las dos banderas están levantadas, estamos en modo party y se da una actualización

			//Se llama la función que actualiza la OLED en modo Party
			updatePartyOLEDFunction();

			partyModeUpdateFlag = 0; //Se baja la bandera de actualización

			//Se llena el arreglo con número aleatorios entre el 0 y el 255
			for (uint16_t i = 0; i < 540; i++) {
				buffer[i] = rand() % 256;
			}

			//Se prende la cinta con los colores del arreglo
			for (uint16_t i = 0; i < 540; i++) {
				colorByte(buffer[i], &handlerPWMOutput);
			}

			//Se envía un reset, para que se encienda adecacuadamente la cinta
			ResetTime(&handlerPWMOutput);
		}

		//MODO JOYSTICK:
		if((adcIsComplete == true) & (joyStickModeFlag == 1)) {
			//Si la bandera joyStickModeFlag está en 1, se está en modo joyStick y se están dando las conversiones ADC

			if (counterADC == 1) {
				adcIsComplete = false;
				adcData[0] = getADC();
			}

			else if (counterADC == 2) {
				adcIsComplete = false;
				adcData[1] = getADC();
			}

			else if (counterADC == 3) {
				adcIsComplete = false;
				adcData[2] = getADC();
			}

			else if (counterADC == 4) {
				adcIsComplete = false;
				adcData[3] = getADC();

			}

			else if (counterADC == 5) {
				adcIsComplete = false;
				adcData[4] = getADC();

			}

			else if (counterADC == 6) {
				adcIsComplete = false;
				adcData[5] = getADC();

				counterADC = 0;

				//Se analiza las conversiones ADC en los canales del Joystick en x y en Y

				//El carro se mueve hacia adelante:
				if ((3200 < adcData[5]) & (adcData[5] < 5000)) {

					if(joyStickModePosition < 179){
						joyStickModePosition++;
					}

					else if(joyStickModePosition == 179){
						joyStickModePosition = 1;
					}

				}

				//El carro se mueve hacia atrás
				else if ((0 < adcData[5]) & (adcData[5] < 500)) {

					if(joyStickModePosition == 0){
						joyStickModePosition = 179;
					}

					else if(joyStickModePosition > 0){
						joyStickModePosition--;
					}
				}

				else {

					__NOP();

				}

				//Se cambia el color del carrito:
				if ((3200 < adcData[4]) & (adcData[4] < 5000)) {

					colorJoyStickMode = (rand() % 6) + 1;

				}

				else if ((0 < adcData[4]) & (adcData[4] < 500)) {

					colorJoyStickMode = (rand() % 6) + 1;

				}

				else {

					__NOP();

				}

				//Se mueve el carrito según la posición y el color dados por las condiciones anteriores
				moveCarJoyStickMode (joyStickModePosition, colorJoyStickMode, &handlerPWMOutput);
				//Se actualiza la OLED con la posición del carrito
				updateJoyStickModeOLED();
			}

			else {
				adcIsComplete = false;
				counterADC = 0;
			}
		}

		//MODO RACE 2 PLAYERS:
		if (updateRaceModeFlag) {
			//si está levantada la bandera de modo carrera:

			updateRaceModeFlag = 0; //Se baja la bandera

			if(raceModeFlagP4){
				//Si se levanta la bbandera raceModeFlagP4 es porque se está en modo 4 jugadores

				//Se llama la función que enciende la cinta de LEDS con las respectivas posiciones e intensidades
				moveCarsFourPlayers (posP1, posP2, posP3, posP4, intensityColorCars[0], intensityColorCars[1], intensityColorCars[2],
												intensityColorCars[3], &handlerPWMOutput);

				//Se actualiza la OLED con el número de vueltas de cada jugador
				updateRaceModeOLED();

				//Si la posición del jugador, es la final, se suma 1 al número de vueltas
				if (posP1 == 179) {
					posP1 = 0;
					lapCounterP1++;
				}

				if (posP2 == 179) {
					posP2 = 0;
					lapCounterP2++;
				}

				if (posP3 == 179) {
					posP3 = 0;
					lapCounterP3++;
				}

				if (posP4 == 179) {
					posP4 = 0;
					lapCounterP4++;
				}
			}

			else if(raceModeFlagP2){
				//Si se levanta la bbandera raceModeFlagP2 es porque se está en modo 2 jugadores

				moveCarsTwoPlayers (posP1, posP2, intensityColorCars[0], intensityColorCars[1], &handlerPWMOutput);

				updateRaceModeOLED();

				if (posP1 == 179) {
					posP1 = 0;
					lapCounterP1++;
				}

				if (posP2 == 179) {
					posP2 = 0;
					lapCounterP2++;
				}
			}
		}

		//Si el número de vueltas que el jugador lleva, es igual a las totales configuradas, se acaba la carrera
		if (lapCounterP2 == handlerRaceLED.numberOfLaps) {
			raceModeFlagP2 = 0;
			raceModeFlagP4 = 0;
			lapCounterP2 = 0;

			clearAllStrip(&handlerPWMOutput);
			ResetTime(&handlerPWMOutput);

			for (uint16_t i = 0; i < 540; i++) {
				buffer[i] = 0;
			}

			for (uint16_t i = 0; i < 540; i += 3) {
				buffer[i] = 255;
			}

			//Se prende la cinta con el color ganador
			for (uint16_t i = 0; i < 540; i++) {
				colorByte(buffer[i], &handlerPWMOutput);
			}

			//Se escribe en la OLED el color ganador
			sprintf(bufferTx, " GANADOR: VERDE     ");
			clearAllScreen(&handlerI2COLED);
			setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
			setColumn(&handlerI2COLED, 0x01);
			printBytesArray(&handlerI2COLED, bufferTx);

			delayms(1000);

			clearAllStrip(&handlerPWMOutput);
		}

		if (lapCounterP1 == handlerRaceLED.numberOfLaps) {
			raceModeFlagP2 = 0;
			raceModeFlagP4 = 0;
			lapCounterP1 = 0;

			for (uint16_t i = 0; i < 540; i++) {
				buffer[i] = 0;
			}

			clearAllStrip(&handlerPWMOutput);
			ResetTime(&handlerPWMOutput);

			for (uint16_t i = 1; i < 540; i += 3) {
				buffer[i] = 255;
			}

			//Se prende la cinta con los colores del arreglo
			for (uint16_t i = 0; i < 540; i++) {
				colorByte(buffer[i], &handlerPWMOutput);
			}

			sprintf(bufferTx, " GANADOR: ROJO     ");
			clearAllScreen(&handlerI2COLED);
			setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
			setColumn(&handlerI2COLED, 0x01);
			printBytesArray(&handlerI2COLED, bufferTx);

			delayms(1000);

			clearAllStrip(&handlerPWMOutput);
		}

		if (lapCounterP3 == handlerRaceLED.numberOfLaps) {
			raceModeFlagP2 = 0;
			raceModeFlagP4 = 0;
			lapCounterP3 = 0;

			clearAllStrip(&handlerPWMOutput);
			ResetTime(&handlerPWMOutput);

			for (uint16_t i = 0; i < 540; i++) {
				buffer[i] = 0;
			}

			for (uint16_t i = 1; i < 540; i += 3) {
				buffer[i] = 255;
			}

			for (uint16_t i = 2; i < 540; i += 3) {
				buffer[i] = (255 * 30) / 100;
			}

			//Se prende la cinta con los colores del arreglo
			for (uint16_t i = 0; i < 540; i++) {
				colorByte(buffer[i], &handlerPWMOutput);
			}

			sprintf(bufferTx, " GANADOR: ROSA     ");
			clearAllScreen(&handlerI2COLED);
			setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
			setColumn(&handlerI2COLED, 0x01);
			printBytesArray(&handlerI2COLED, bufferTx);

			delayms(1000);

			clearAllStrip(&handlerPWMOutput);
		}

		if (lapCounterP4 == handlerRaceLED.numberOfLaps) {
			raceModeFlagP2 = 0;
			raceModeFlagP4 = 0;
			lapCounterP4 = 0;

			clearAllStrip(&handlerPWMOutput);
			ResetTime(&handlerPWMOutput);

			for (uint16_t i = 0; i < 540; i++) {
				buffer[i] = 0;
			}

			for (uint16_t i = 1; i < 540; i += 3) {
				buffer[i] = 255;
			}

			for (uint16_t i = 2; i < 540; i += 3) {
				buffer[i] = 255;
			}

			//Se prende la cinta con los colores del arreglo
			for (uint16_t i = 0; i < 540; i++) {
				colorByte(buffer[i], &handlerPWMOutput);
			}

			sprintf(bufferTx, "GANADOR: MAGENTA     ");
			clearAllScreen(&handlerI2COLED);
			setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
			setColumn(&handlerI2COLED, 0x01);
			printBytesArray(&handlerI2COLED, bufferTx);

			delayms(1000);

			clearAllStrip(&handlerPWMOutput);
		}

	}

	return 0;
}

/****************************************************************************************************************************/

//CALLBACKS

//Callback de la conversión ADC
void adcComplete_Callback(void){
	adcIsComplete = true;
	counterADC++;
}

//Callback del Timer10
void BasicTimer10_Callback(void) {
	GPIOxTooglePin(&handlerBlinkyPin);
	partyModeUpdateFlag = 1;
}

//Callback del Timer3
void BasicTimer3_Callback(void){
	updateRaceModeFlag = 1;
	updateJoyStickModeFlag = 1;
}

//Callback del ExtiP4
void callback_extInt2(void) {
	posP4++;
}

//Callback del ExtiP3
void callback_extInt7(void) {
	posP3++;
}

//Callback del ExtiP2
void callback_extInt10(void) {
//Se sube la bandera del PinClock a 1
	posP2++;
}

//Callback del ExtiP1
void callback_extInt15(void) {
//Se sube la bandera del PinClock a 1
	posP1++;
}

//Callback de la recepción Usart
void usart2Rx_Callback(void){
	//Activamos una bandera, dentro de la función main se lee el registro DR lo que baja la bandera de la interrupción
	rxDataFlag = 1;
}

/****************************************************************************************************************************/

//FUNCIONES

//Función que da la intensidad de los carros en un arrreglo
char* intensityColorCarsFunction (uint16_t* carsIntensityADC){

	intensityColorCars[0] = carsIntensityADC[0]/16;
	intensityColorCars[1] = carsIntensityADC[1]/16;
	intensityColorCars[2] = carsIntensityADC[2]/16;
	intensityColorCars[3] = carsIntensityADC[3]/16;

	return intensityColorCars;

}

//Función que actualiza la OLED en modo PARTY
void updatePartyOLEDFunction(void){

	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_0);
	printBytesArray(&handlerI2COLED, "          PARTY ");
	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
	printBytesArray(&handlerI2COLED, "PARTY           ");
	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_4);
	printBytesArray(&handlerI2COLED, "     PARTY      ");
	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_6);
	printBytesArray(&handlerI2COLED,"          PARTY ");
}

//Funcióin que actualiza la OLED en modo RACE
void updateRaceModeOLED(void){

	if(handlerRaceLED.numberOfPlayers == 2){

		sprintf(bufferTx, "PLAYER 1 LAPS: %d", lapCounterP1);

		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
		setColumn(&handlerI2COLED, 0x01);
		printBytesArray(&handlerI2COLED, bufferTx);

		sprintf(bufferTx, "PLAYER 2 LAPS: %d", lapCounterP2);

		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		setColumn(&handlerI2COLED, 0x01);
		printBytesArray(&handlerI2COLED, bufferTx);

	}

	else if(handlerRaceLED.numberOfPlayers == 4){

		sprintf(bufferTx, "PLAYER 1 LAPS: %d", lapCounterP1);

		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
		setColumn(&handlerI2COLED, 0x01);
		printBytesArray(&handlerI2COLED, bufferTx);

		sprintf(bufferTx, "PLAYER 2 LAPS: %d", lapCounterP2);

		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		setColumn(&handlerI2COLED, 0x01);
		printBytesArray(&handlerI2COLED, bufferTx);

		sprintf(bufferTx, "PLAYER 3 LAPS: %d", lapCounterP3);

		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_4);
		setColumn(&handlerI2COLED, 0x01);
		printBytesArray(&handlerI2COLED, bufferTx);

		sprintf(bufferTx, "PLAYER 4 LAPS: %d", lapCounterP4);

		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_5);
		setColumn(&handlerI2COLED, 0x01);
		printBytesArray(&handlerI2COLED, bufferTx);
	}
}

//Función que actualiza la OLED en modo JoyStick
void updateJoyStickModeOLED(void){

	if(joyStickModePosition < 10){

		sprintf(bufferTx, " POSITION: 00%d", joyStickModePosition);
	}

	else if(joyStickModePosition < 100){

		sprintf(bufferTx, " POSITION: 0%d", joyStickModePosition);

	}

	else {

		sprintf(bufferTx, " POSITION: %d", joyStickModePosition);

	}


	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
	setColumn(&handlerI2COLED, 0x01);
	printBytesArray(&handlerI2COLED, bufferTx);


}

//Función que analiza lo obtenido por el serial
void parseCommands(char *ptrBufferReception) {
	//esta función lee lo obtenido por el puerto serial y toma decisiones en base a eso

	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter,
			&secondParameter, userMsg);

	if (strcmp(cmd, "help") == 0) {

		joyStickModeFlag        = 0;
		partyModeFlag		    = 0;
		raceModeFlagP2     = 0;
		raceModeFlagP4     = 0;
		intensityConfigFlag     = 0;

		//Se escribe el menú de comandos en la terminal serial
		writeMsg(&handlerUsart2, "\n\rInstrucciones de juego:\n\r");
		writeMsg(&handlerUsart2,
				"Este juego es un juego de carreras, en el que cada competidor debe oprimir su respectivo boton\n"
				"lo mas rapido posible, en pro de que su respectivo color llegue al final de la carrera en primer lugar.\n\r"
				"El juego cuenta con dos posibles modos de juego seleccionables:\n\r"
				"1. Modo 2 Jugadores: en este modo solo habran dos competidores. Para este modo es posible configurar\n"
				"con cuantas vueltas de la pista contara la carrera.\n"
				"2. Modo 4 Jugadores: en este modo seran 4 competidores. Para este modo es posible configurar con cuantas vueltas\n"
				"de la pista contara la carrera.\n"
				"En caso de no hacerse la configuracion del modo de juego antes de iniciar la carrera, por defecto el juego se\n"
				"inicilizara en modo de 2 jugadores y 1 vuelta.\n\r");

		writeMsg(&handlerUsart2, "Para configurar el modo de juego:\n"
				"Con el comando setRaceMode #numeroDeJugadores #numeroDeVueltas @ se configura el modo de juego.\n"
				"Por ejemplo,para 2 jugadores y 5 vueltas: envíe el comando setPlayersMode 2 5 @, configure la intensidad\n"
				"deseada con las perillas y posteriormente envíe el comando initRace @ para iniciar la carrera. \n"
				"La intensidad de los colores de los carros, solo se podra configurar en el momento de configuracion, es decir \n"
				"al enviar el comando setRaceMode # #.\n\r"
				);

		writeMsg(&handlerUsart2, "Para iniciar la carrera:\n"
				"La carrera se inicia con el comando initRace @, este comando empezara a hacer una cuenta reegresiva\n"
				"y posteriormente se dara inicio a la carrera.\n\r");

		writeMsg(&handlerUsart2, "Al finalizar la carrera se mostrará en la pantalla el color ganador, adicionalmente, la pista se pintara\n"
				"toda de este color.\n\r");

		writeMsg(&handlerUsart2, "Otros modos:\n"
				"Adicional al juego de carreras, la pista de LEDs se puede poner en otros modos:\n"
				"1.Modo Fiesta: se inicializa con el comando setPartyMode @, al enviar este comando, la pista se encendera aleatoriamente de\n"
				"diferentes colores.\n\r"
				"2.Modo JoyStick: Se inicializa con el comando joyStickMode @, en este modo aparece en la pista un carrp \n"
				"el cual se mueve según la posición del joyStick: para avanzar se debe mover el joyStick en la direccion positiva en x,\n"
				"para retroceder se debe mover en la dirección negativa en x y para cambiar de color se debe mover en cualquier direccion en y.\n\r"
				"3.Modo autodestruccion: al llamar el comando initAutodestruction @, la pista hace una cuenta regresiva y posteriormente se\n"
				"queda encendida en color rosado.\n\r"
				);

		//Se limpia la oled y se envía el mensaje
		clearOLED(&handlerI2COLED);
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
		setColumn(&handlerI2COLED, 0x10);
		printBytesArray(&handlerI2COLED, " BIENVENIDO ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_4);
		setColumn(&handlerI2COLED, 0x02);
		printBytesArray(&handlerI2COLED, "HECHO POR LAURA");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_5);
		setColumn(&handlerI2COLED, 0x19);
		printBytesArray(&handlerI2COLED, " ZULUAGA        ");

	}

	else if (strcmp(cmd, "setRaceMode") == 0) {

		handlerRaceLED.numberOfPlayers = firstParameter;
		handlerRaceLED.numberOfLaps    = secondParameter;

		//Se enciende la conversión ADC
		enableEvent(&handlerPWMTimer);
		enableOutput(&handlerPWMTimer);
		startPwmSignal(&handlerPWMTimer);

		//Se limpia la OLED
		clearAllScreen(&handlerI2COLED);

		//Se apagan las banderas de los otros modos
		joyStickModeFlag        = 0;
		partyModeFlag		    = 0;
		intensityConfigFlag		= 0;
		raceModeFlagP2    		= 0;
		raceModeFlagP4    		= 0;


		if ((firstParameter != 2) & (firstParameter != 4)) {
			writeMsg(&handlerUsart2, "Por favor, ingrese un numero de jugadores valido \n\r");
		}

		else if (secondParameter > 10){
			writeMsg(&handlerUsart2, "No es posible configurar mas de 10 vueltas\n\r");
		}

		else{

			//Se limpia la OLED y se envía mensaje
			clearAllScreen(&handlerI2COLED);
			setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
			setColumn(&handlerI2COLED, 0x01);
			printBytesArray(&handlerI2COLED, "MODO CONFIGURADO");
			sprintf(bufferTx, "JUGADORES: %d\n", firstParameter);
			setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
			setColumn(&handlerI2COLED, 0x10);
			printBytesArray(&handlerI2COLED, bufferTx);
			sprintf(bufferTx, "  VUELTAS: %d\n\r", secondParameter);
			setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_4);
			setColumn(&handlerI2COLED, 0x10);
			printBytesArray(&handlerI2COLED, bufferTx);

			//Se ponen unas posiciones predeterminadas para poder observar los jugadores y sus intensidades de color
			posP1 = 1;
			posP2 = 15;
			posP3 = 25;
			posP4 = 35;

			//Se muestran los carritos inicialmente en la máxima intensidad
			showFourCarsToConfig(posP1, posP2, posP3, posP4, 255, 255, 255, 255, &handlerPWMOutput);

			//Se sube la bandera del modo configuración
			intensityConfigFlag = 1;

			delayms(100);
		}

	}

	else if (strcmp(cmd, "initRace") == 0){

		//Se apaga la conversión ADC
		disableEvent(&handlerPWMTimer);
		disableOutput(&handlerPWMTimer);

		//Se limpia la OLED
		clearAllScreen(&handlerI2COLED);

		//Se apagan las banderas de los otros modos
		joyStickModeFlag        = 0;
		intensityConfigFlag     = 0;
		partyModeFlag		    = 0;
		raceModeFlagP2    		= 0;
		raceModeFlagP4     		= 0;

		//Se ponen en 0 las posiciones de los jugadores
		posP1 = 0;
		posP2 = 0;
		posP3 = 0;
		posP4 = 0;

		//Se ponen en 0 el número de vueltas de los jugadores
		lapCounterP1 = 0;
		lapCounterP2 = 0;
		lapCounterP3 = 0;
		lapCounterP4 = 0;

		//Se inicia el CONTEO REGRESIVO
		//3

		//Se limpia la cinta
		clearAllStrip(&handlerPWMOutput);
		//Se hace un delay
		delayms(100);
	    delayms(500);

		//Se llena la cinta de color verde
		for (uint16_t i = 0; i < 540; i++) {
			buffer[i] = 0;
		}

		for (uint16_t i = 0; i < 540; i += 3) {
			buffer[i] = 255;
		}

		//Se prende la cinta con los colores del arreglo
		for (uint16_t i = 0; i < 540; i++) {
			colorByte(buffer[i], &handlerPWMOutput);
		}

		//Se envía un reset, para que se encienda adecacuadamente la cinta
		ResetTime(&handlerPWMOutput);

		//Se pone el sonido
		Tone(&handlerPWMTimerBuzzer, 1);
		delayms(500);
		noTone(&handlerPWMTimerBuzzer);

		//Se limpia la cinta
		clearAllStrip(&handlerPWMOutput);
		delayms(100);
		delayms(1000);

		//CONTEO REGRESIVO
		//2

		//Se llena la cinta de color rojo
		for (uint16_t i = 0; i < 540; i++) {
			buffer[i] = 0;
		}

		for (uint16_t i = 1; i < 540; i += 3) {
			buffer[i] = 255;
		}

		//Se prende la cinta con los colores del arreglo
		for (uint16_t i = 0; i < 540; i++) {
			colorByte(buffer[i], &handlerPWMOutput);
		}

		//Se envía un reset, para que se encienda adecacuadamente la cinta
		ResetTime(&handlerPWMOutput);

		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
		setColumn(&handlerI2COLED, 0x01);

		//Se pone el sonido
		Tone(&handlerPWMTimerBuzzer, 1);
		delayms(500);
		noTone(&handlerPWMTimerBuzzer);
		clearAllStrip(&handlerPWMOutput);
		delayms(100);
		delayms(1000);

		//CONTEO REGRESIVO
		//1
		//Se llena la cinta de color azul
		for (uint16_t i = 0; i < 540; i++) {
			buffer[i] = 0;
		}

		for (uint16_t i = 2; i < 540; i += 3) {
			buffer[i] = 255;
		}

		//Se prende la cinta con los colores del arreglo
		for (uint16_t i = 0; i < 540; i++) {
			colorByte(buffer[i], &handlerPWMOutput);
		}

		//Se envía un reset, para que se encienda adecacuadamente la cinta
		ResetTime(&handlerPWMOutput);

		//Se pone el sonido
		Tone(&handlerPWMTimerBuzzer, 1);
		delayms(500);
		noTone(&handlerPWMTimerBuzzer);
		clearAllStrip(&handlerPWMOutput);
		delayms(100);
		delayms(1000);


		//CONTEO REGRESIVO
		//F
		//Se llena la cinta con el color Rosado
		for (uint16_t i = 0; i < 540; i++) {
			buffer[i] = 0;
		}

		for (uint16_t i = 2; i < 540; i += 3) {
			buffer[i] = 82;
		}

		for (uint16_t i = 1; i < 540; i += 3) {
			buffer[i] = 227;
		}


		//Se prende la cinta con los colores del arreglo
		for (uint16_t i = 0; i < 540; i++) {
			colorByte(buffer[i], &handlerPWMOutput);
		}

		//Se envía un reset, para que se encienda adecacuadamente la cinta
		ResetTime(&handlerPWMOutput);

		//Se pone el sonido de GO
		Tone(&handlerPWMTimerBuzzer, 2);

		//Se envía el GO por la OLED
		clearAllScreen(&handlerI2COLED);
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_1);
		printBytesArray(&handlerI2COLED, "          GOOOO ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		printBytesArray(&handlerI2COLED, "GOOOO           ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_5);
		printBytesArray(&handlerI2COLED, "     GOOOO      ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_7);
		printBytesArray(&handlerI2COLED, "           GOOOO ");

		delayms(300);

		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_1);
		printBytesArray(&handlerI2COLED, "GOOOO           ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		printBytesArray(&handlerI2COLED, "     GOOOO      ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_5);
		printBytesArray(&handlerI2COLED, "          GOOOO ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_7);
		printBytesArray(&handlerI2COLED, "GOOOO           ");

		delayms(300);

		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_1);
		printBytesArray(&handlerI2COLED, "     GOOOO      ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		printBytesArray(&handlerI2COLED, "          GOOOO ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_5);
		printBytesArray(&handlerI2COLED, "GOOOO           ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_7);
		printBytesArray(&handlerI2COLED, "     GOOOO      ");

		delayms(300);
		clearAllScreen(&handlerI2COLED);
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		printBytesArray(&handlerI2COLED, "                ");

		noTone(&handlerPWMTimerBuzzer);
		clearAllStrip(&handlerPWMOutput);
		delayms(100);
		delayms(100);


		//Se analiza el número de jugadores elegido por el usuario y según eso se levanta la bandera correspondiente
		if(handlerRaceLED.numberOfPlayers == 2){
			raceModeFlagP2       = 1;
		}

		else if(handlerRaceLED.numberOfPlayers == 4){
			raceModeFlagP4       = 1;
		}

	}

	else if (strcmp(cmd, "setPartyMode") == 0){

		//Se bajan las banderas de los otros modos
		joyStickModeFlag        = 0;
		partyModeFlag		    = 1;
		raceModeFlagP2     		= 0;
		raceModeFlagP4     		= 0;
		intensityConfigFlag     = 0;

	}

	else if (strcmp(cmd, "initAutodestruction") == 0){

		//Se bajan las banderas de los otros modos
		joyStickModeFlag        = 0;
		partyModeFlag		    = 0;
		raceModeFlagP2     		= 0;
		raceModeFlagP4    		= 0;
		intensityConfigFlag     = 0;

		//se hace el CONTEO REGRESIVO
		//3
		clearAllStrip(&handlerPWMOutput);
		delayms(100);
		delayms(500);

		//Se llena el arreglo con número aleatorios entre el 0 y el 255
		for (uint16_t i = 0; i < 540; i++) {
			buffer[i] = 0;
		}

		for (uint16_t i = 0; i < 540; i += 3) {
			buffer[i] = 255;
		}

		//Se prende la cinta con los colores del arreglo
		for (uint16_t i = 0; i < 540; i++) {
			colorByte(buffer[i], &handlerPWMOutput);
		}

		//Se envía un reset, para que se encienda adecacuadamente la cinta
		ResetTime(&handlerPWMOutput);

		delayms(1000);

		clearAllStrip(&handlerPWMOutput);
		delayms(100);
		delayms(500);

		//CONTEO REGRESIVO
		//2
		//Se llena el arreglo con número aleatorios entre el 0 y el 255
		for (uint16_t i = 0; i < 540; i++) {
			buffer[i] = 0;
		}

		for (uint16_t i = 1; i < 540; i += 3) {
			buffer[i] = 255;
		}

		//Se prende la cinta con los colores del arreglo
		for (uint16_t i = 0; i < 540; i++) {
			colorByte(buffer[i], &handlerPWMOutput);
		}

		//Se envía un reset, para que se encienda adecacuadamente la cinta
		ResetTime(&handlerPWMOutput);

		delayms(1000);

		clearAllStrip(&handlerPWMOutput);
		delayms(100);
		delayms(500);

		//CONTEO REGRESIVO
		//1
		//Se llena el arreglo con número aleatorios entre el 0 y el 255
		for (uint16_t i = 0; i < 540; i++) {
			buffer[i] = 0;
		}

		for (uint16_t i = 2; i < 540; i += 3) {
			buffer[i] = 255;
		}

		//Se prende la cinta con los colores del arreglo
		for (uint16_t i = 0; i < 540; i++) {
			colorByte(buffer[i], &handlerPWMOutput);
		}

		//Se envía un reset, para que se encienda adecacuadamente la cinta
		ResetTime(&handlerPWMOutput);

		delayms(1000);

		clearAllStrip(&handlerPWMOutput);
		delayms(100);
		delayms(500);

		Tone(&handlerPWMTimerBuzzer, 10);
		clearAllScreen(&handlerI2COLED);
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_1);
		printBytesArray(&handlerI2COLED, "          BYEEE ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		printBytesArray(&handlerI2COLED, "BYEEE           ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_5);
		printBytesArray(&handlerI2COLED, "     BYEEE      ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_7);
		printBytesArray(&handlerI2COLED, "           BYEEE ");

		delayms(300);

		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_1);
		printBytesArray(&handlerI2COLED, "BYEEE           ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		printBytesArray(&handlerI2COLED, "     BYEEE      ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_5);
		printBytesArray(&handlerI2COLED, "          BYEEE ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_7);
		printBytesArray(&handlerI2COLED, "BYEEE           ");

		delayms(300);

		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_1);
		printBytesArray(&handlerI2COLED, "     BYEEE      ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		printBytesArray(&handlerI2COLED, "          BYEEE ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_5);
		printBytesArray(&handlerI2COLED, "BYEEE           ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_7);
		printBytesArray(&handlerI2COLED, "     BYEEE      ");

		delayms(300);
		clearAllScreen(&handlerI2COLED);
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		printBytesArray(&handlerI2COLED, "                ");
		noTone(&handlerPWMTimerBuzzer);


		//CONTEO REGRESIVO
		//F
		//Se llena el arreglo con número aleatorios entre el 0 y el 255
		for (uint16_t i = 0; i < 540; i++) {
			buffer[i] = 0;
		}

		for (uint16_t i = 2; i < 540; i += 3) {
			buffer[i] = 82;
		}

		for (uint16_t i = 1; i < 540; i += 3) {
			buffer[i] = 227;
		}


		//Se prende la cinta con los colores del arreglo
		for (uint16_t i = 0; i < 540; i++) {
			colorByte(buffer[i], &handlerPWMOutput);
		}

		//Se envía un reset, para que se encienda adecacuadamente la cinta
		ResetTime(&handlerPWMOutput);

	}

	else if (strcmp(cmd, "joyStickMode") == 0) {

		//Se enciende la conversión ADC
		delayms(10);
		enableEvent(&handlerPWMTimer);
		enableOutput(&handlerPWMTimer);
		startPwmSignal(&handlerPWMTimer);
		delayms(10);

		//Se limpia la OLED
		clearAllScreen(&handlerI2COLED);

		//Se pone la posición del carro en 0
		joyStickModePosition 	= 0;

		//Se bajan las banderas de los otros modos
		joyStickModeFlag        = 1;
		partyModeFlag		    = 0;
		raceModeFlagP2     		= 0;
		raceModeFlagP4     		= 0;
		intensityConfigFlag     = 0;
	}

	else {

		//Se bajan todas las banderas de los modos
		joyStickModeFlag        = 0;
		partyModeFlag		    = 0;
		raceModeFlagP2          = 0;
		raceModeFlagP4          = 0;
		intensityConfigFlag     = 0;

		//Se llena la cinta con color rojo
		for (uint16_t i = 0; i < 540; i++) {
			buffer[i] = 0;
		}

		for (uint16_t i = 2; i < 540; i += 3) {
			buffer[i] = 0;
		}

		for (uint16_t i = 1; i < 540; i += 3) {
			buffer[i] = 255;
		}

		//Se prende la cinta con los colores del arreglo
		for (uint16_t i = 0; i < 540; i++) {
			colorByte(buffer[i], &handlerPWMOutput);
		}

		//Se envía un reset, para que se encienda adecacuadamente la cinta
		ResetTime(&handlerPWMOutput);

		//Se envía un mensaje de error en la OLED
		clearAllScreen(&handlerI2COLED);
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_1);
		printBytesArray(&handlerI2COLED, "          ERROR ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		printBytesArray(&handlerI2COLED, "ERROR           ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_5);
		printBytesArray(&handlerI2COLED, "     ERROR      ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_7);
		printBytesArray(&handlerI2COLED, "           ERROR ");

		delayms(300);

		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_1);
		printBytesArray(&handlerI2COLED, "ERROR           ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		printBytesArray(&handlerI2COLED, "     ERROR      ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_5);
		printBytesArray(&handlerI2COLED, "          ERROR ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_7);
		printBytesArray(&handlerI2COLED, "ERROR           ");

		delayms(300);

		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_1);
		printBytesArray(&handlerI2COLED, "     ERROR      ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		printBytesArray(&handlerI2COLED, "          ERROR ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_5);
		printBytesArray(&handlerI2COLED, "ERROR           ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_7);
		printBytesArray(&handlerI2COLED, "     ERROR      ");

		delayms(300);
		clearAllScreen(&handlerI2COLED);

		//Se limpia la cinta
		clearAllStrip(&handlerPWMOutput);
	}
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
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinNumber 		  = PIN_8;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinMode 	      = GPIO_MODE_OUT;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinOPType 		  = GPIO_OTYPE_PUSHPULL;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinSpeed 	      = GPIO_OSPEED_FAST;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinPuPdControl	  = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPWMOutput);

	//Se pone el PIN en 0:
	GPIO_WritePin(&handlerPWMOutput, RESET);

	//Se configura el BlinkyTimer
	handlerBlinkyTimer.ptrTIMx 					= TIM10;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_100M_05ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 	= 500; //Update period= 0.05ms*500 = 250ms

	//Se carga la configuración del BlinkyTimer
	Timer10_Config(&handlerBlinkyTimer);

	//Se configura el BlinkyTimer
	handlerIntTimer.ptrTIMx 					= TIM3;
	handlerIntTimer.TIMx_Config.TIMx_mode 	    = BTIMER_MODE_UP;
	handlerIntTimer.TIMx_Config.TIMx_speed 	    = BTIMER_SPEED_100M_05ms;
	handlerIntTimer.TIMx_Config.TIMx_period 	= 100; //Update period = 15ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerIntTimer);

	//Se configura el TxPin (PIN por el cual se hace la transmisión)
	//Este PIN se configura en la función alternativa AF07 que corresponde al USART2
	handlerTxPin.pGPIOx 							= GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber      = PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN; //Función alternativa
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode  = AF7;	      //AF07: Usart2

	//Se carga la configuración
	GPIO_Config(&handlerTxPin);

	//Se configura el TxPin (PIN por el cual se hace la transmisión)
	//Este PIN se configura en la función alternativa AF07 que corresponde al USART2
	handlerRxPin.pGPIOx 							= GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber      = PIN_3;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN; //Función alternativa
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode  = AF7;	      //AF07: Usart2

	//Se carga la configuración
	GPIO_Config(&handlerRxPin);

	//Se configura el USART 2
	handlerUsart2.ptrUSARTx					     = USART2;                	  //USART 2
	handlerUsart2.USART_Config.USART_mode 	     = USART_MODE_RXTX;       	  //Modo de Recepción y transmisión
	handlerUsart2.USART_Config.USART_baudrate    = USART_BAUDRATE_57600; 	  //115200 bps
	handlerUsart2.USART_Config.USART_parity      = USART_PARITY_EVEN;         //Parity:NONE, acá viene configurado el tamaño de dato
	handlerUsart2.USART_Config.USART_stopbits    = USART_STOPBIT_1;	          //Un stopbit
	handlerUsart2.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE; //Interrupción de recepción del usart habilitada

	//Se carga la configuración del USART
	USART_Config(&handlerUsart2);

	//Se configura la carrera de LEDs, por defecto se pone con 1 vuelta y 2 jugadore
	handlerRaceLED.numberOfLaps    = 1;
	handlerRaceLED.numberOfPlayers = 2;

	//Se configura el Button: Se debe tener en cuenta que el modo entrada está configurado en el ExtiDriver
	handlerPlayer2.pGPIOx 							  = GPIOC;
	handlerPlayer2.GPIO_PinConfig.GPIO_PinNumber	  = PIN_10;
	handlerPlayer2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se configura el EXTI del botón
	Player2ExtiConfig.pGPIOHandler = &handlerPlayer2;
	Player2ExtiConfig.edgeType     = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Se carga la configuración: al cargar la configuración del exti, se carga también la del GPIO
	extInt_Config(&Player2ExtiConfig);

	//Se configura el Button: Se debe tener en cuenta que el modo entrada está configurado en el ExtiDriver
	handlerPlayer1.pGPIOx        					  = GPIOA;
	handlerPlayer1.GPIO_PinConfig.GPIO_PinNumber      = PIN_15;
	handlerPlayer1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se configura el EXTI del botón
	Player1ExtiConfig.pGPIOHandler = &handlerPlayer1;
	Player1ExtiConfig.edgeType     = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Se carga la configuración: al cargar la configuración del exti, se carga también la del GPIO
	extInt_Config(&Player1ExtiConfig);

	//Se configura el Button: Se debe tener en cuenta que el modo entrada está configurado en el ExtiDriver
	handlerPlayer3.pGPIOx        					  = GPIOC;
	handlerPlayer3.GPIO_PinConfig.GPIO_PinNumber      = PIN_7;
	handlerPlayer3.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se configura el EXTI del botón
	Player3ExtiConfig.pGPIOHandler = &handlerPlayer3;
	Player3ExtiConfig.edgeType     = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Se carga la configuración: al cargar la configuración del exti, se carga también la del GPIO
	extInt_Config(&Player3ExtiConfig);


	//Se configura el Button: Se debe tener en cuenta que el modo entrada está configurado en el ExtiDriver
	handlerPlayer4.pGPIOx							  = GPIOB;
	handlerPlayer4.GPIO_PinConfig.GPIO_PinNumber      = PIN_2;
	handlerPlayer4.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se configura el EXTI del botón
	Player4ExtiConfig.pGPIOHandler = &handlerPlayer4;
	Player4ExtiConfig.edgeType     = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Se carga la configuración: al cargar la configuración del exti, se carga también la del GPIO
	extInt_Config(&Player4ExtiConfig);

	//Se configura el SDA del I2C de la OLED
	handlerSDAPin.pGPIOx 							 = GPIOB;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_9;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinMode 		 = GPIO_MODE_ALTFN; //Función alternativa
	handlerSDAPin.GPIO_PinConfig.GPIO_PinOPType 	 = GPIO_OTYPE_OPENDRAIN;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinSpeed 		 = GPIO_OSPEED_FAST;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinAltFunMode  = AF4;	    //AF04: I2C3 SDA

	//Se carga la configuración
	GPIO_Config(&handlerSDAPin);

	//Se configura el SCL del I2C del acelerómetro y la OLED
	handlerSCLPin.pGPIOx 							 = GPIOB;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinNumber	     = PIN_6;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinMode 		 = GPIO_MODE_ALTFN; //Función alternativa
	handlerSCLPin.GPIO_PinConfig.GPIO_PinOPType 	 = GPIO_OTYPE_OPENDRAIN;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinSpeed 		 = GPIO_OSPEED_FAST;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinAltFunMode  = AF4;	    //AF04: I2C3 SCL

	//Se carga la configuración
	GPIO_Config(&handlerSCLPin);

	 //Se configura el HandlerI2C de la OLED
	handlerI2COLED.slaveAddress = OLED_ADD;
	handlerI2COLED.modeI2C      = I2C_MODE_FM; //Es necesario que sea en fastmode
	handlerI2COLED.ptrI2Cx 		= I2C1;

	//Se carga la configuración
	i2c_config(&handlerI2COLED);

	//Se configura la conversión ADC
	adcConfig.channels          = channels;
	adcConfig.numberOfChannels  = ADC_NUMBER_OF_CHANNELS_6;
	adcConfig.dataAlignment		= ADC_ALIGNMENT_RIGHT;
	adcConfig.resolution		= ADC_RESOLUTION_12_BIT;
	adcConfig.samplingPeriod	= ADC_SAMPLING_PERIOD_480_CYCLES;

	//Se carga la configuración, así la interrupción se activa por defecto
	adc_Config(&adcConfig);

	//Se configura el evento externo
	adcConfigEvent.extEventTrigger    = ADC_EXT_TRIG_FALLING_EDGE;
	adcConfigEvent.extEventTypeSelect = ADC_EXT_EVENT_TIM5_CC1;

	//Se carga la configuración
	adcConfigExternal(&adcConfigEvent);

	//Se configura el Timer del PWM de los eventos ADC
	handlerPWMTimer.ptrTIMx 		  = TIM5;
	handlerPWMTimer.config.channel 	  = PWM_CHANNEL_1;
	handlerPWMTimer.config.prescaler  = BTIMER_SPEED_100M_05ms;
	handlerPWMTimer.config.periodo 	  = 250;
	handlerPWMTimer.config.duttyCicle = 125;

	pwm_Config(&handlerPWMTimer);

	//Se configura el Timer del PWM
	handlerPWMTimerBuzzer.ptrTIMx 		      = TIM2;
	handlerPWMTimerBuzzer.config.channel 	  = PWM_CHANNEL_3;
	handlerPWMTimerBuzzer.config.prescaler    = BTIMER_SPEED_100M_05ms;
	handlerPWMTimerBuzzer.config.periodo 	  = 4;
	handlerPWMTimerBuzzer.config.duttyCicle   = 2;

	pwm_Config(&handlerPWMTimerBuzzer);

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

	//Se inicializa la OLED y se envía un mensaje de bienvenida
	initOLED(&handlerI2COLED);
	clearOLED(&handlerI2COLED);
	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
	setColumn(&handlerI2COLED, 0x10);
	printBytesArray(&handlerI2COLED, " BIENVENIDO ");

	//Se limpia la cinta de LEDs
	clearAllStrip(&handlerPWMOutput);
	ResetTime(&handlerPWMOutput);
}



