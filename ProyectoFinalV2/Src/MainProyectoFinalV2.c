/**
 ******************************************************************************
 * @file           : MainProyectiFinalV2.c
 * @author         : Laura Zuluaga
 * @brief          : Pruebas Proyecto
 ******************************************************************************
 *
 ******************************************************************************
 */

//Se incluyen algunas librerías de C
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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

//DEFINICIÓN DE VARIABLES:

//Buffer auxiliar para almacenar los colores de la cinta
uint8_t buffer[540] = {0};

//Contadores de la posición de los jugadores en la pista(número de LED en el que están)
uint8_t posP1 = 1; //Jugador 1
uint8_t posP2 = 15; //Jugador 2
uint8_t posP4 = 37; //Jugador 3
uint8_t posP3 = 25; //Jugador 4

//Contador de vueltas para los jugadores
uint8_t lapCounterP1 = 0; //Jugador 1
uint8_t lapCounterP2 = 0; //Jugador 2
uint8_t lapCounterP3 = 0; //Jugador 3
uint8_t lapCounterP4 = 0; //Jugador 4

//Banderas auxiliares para la activavión y actualización del modo Party
uint8_t partyModeFlag 		 = 0;
uint8_t partyModeUpdateFlag  = 0;

uint8_t joyStickModeFlag = 0;
uint8_t joyStickModePosition = 1;
uint8_t updateJoyStickModeFlag = 0;
uint8_t colorJoyStickMode = 3;

uint8_t autodestructionModeFlag = 0;

//Bandera auxiliar para el modo race
uint8_t raceModeFlag       = 0;
uint8_t counterRaceState   = 0;
uint8_t updateRaceModeFlag = 0;

uint8_t updateOLEDRaceMode = 0;

//Banderas auxiliares para la conversión ADC
uint8_t counterADC = 0;
bool adcIsComplete = false;

//Banderas auxiliares para la actualización de la posición de los jugadores
uint8_t flagP1 = 0; //Jugador 1
uint8_t flagP2 = 0; //Jugador 2
uint8_t flagP3 = 0; //Jugador 3
uint8_t flagP4 = 0; //Jugador 4

//Banderas auxiliares para la recepción de comandos por el USART
uint8_t rxData           = 0;     //Datos de recepción
uint8_t rxDataFlag       = 0;	  //Bandera para la recepción de datos del usart2
uint8_t counterReception = 0;     //Contador para la recepción de datos por el usart2
bool stringComplete 	 = false; //Bandera para la recepción de datos del usart2

//Arreglos para la conversión ADC
uint8_t channels[6]= {ADC_CHANNEL_1, ADC_CHANNEL_4, ADC_CHANNEL_8, ADC_CHANNEL_10, ADC_CHANNEL_6, ADC_CHANNEL_7};
uint16_t adcData[6]= {0};  //Datos del ADC

uint8_t intensityConfigFlag = 0;
char intensityColorCars[4] = {255, 255, 255, 255 };

//Arreglos auxiliares
char bufferReception[200] = {0}; //En esta variable se almacenan variables de recepción
char bufferTx[200]        = {0}; //En esta variable se almacenan variables de recepción
char userMsg[64]          = {0}; //En esta variable se almacenan mensajes ingresados por la terminal serial
char cmd[64]              = {0}; //En esta variable se almacenan los comandos ingresados por el usuario

//Variables auxiliares para la recepción de comandos
unsigned int firstParameter  = 0; //En esta variable se almacena el número ingresado por la terminal serial
unsigned int secondParameter = 0; //En esta variable se almacena el segundo número ingresado por la terminal serial



//Handler de los timers usados
BasicTimer_Handler_t handlerBlinkyTimer = { 0 };  //Handler para el BlinkyTimer
BasicTimer_Handler_t handlerIntTimer    = { 0 };

//Handler para el PWM de los eventos ADC
PWM_Handler_t handlerPWMTimer 			= { 0 };
PWM_Handler_t handlerPWMTimerBuzzer     = { 0 };

//Handler de los GPIOs usados
GPIO_Handler_t handlerBlinkyPin 	    = { 0 };  //Handler para el LED de estado
GPIO_Handler_t handlerMCO_2  			= { 0 };  //Handler para el PIN de salida del Clock
GPIO_Handler_t handlerPWMOutput 		= { 0 };  //Handler para la salida del PWM
GPIO_Handler_t handlerUserButton 		= { 0 };
GPIO_Handler_t handlerButton 		    = { 0 };
GPIO_Handler_t handlerTxPin             = { 0 }; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerRxPin             = { 0 }; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerSCLPin			= { 0 };
GPIO_Handler_t handlerSDAPin			= { 0 };
GPIO_Handler_t handlerPinPWMChannel	    = { 0 };

//Handler para el USART2
USART_Handler_t handlerUsart2           = { 0 }; //Handler para el USART2

//Handler de las interrupciones externas
EXTI_Config_t ButtonExtiConfig          = { 0 };
EXTI_Config_t UserButtonExtiConfig      = { 0 };

//Handler para el I2C de la OLED
I2C_Handler_t handlerI2COLED 			= { 0 };

//Handlers para la conversión ADC y los eventos
ADC_Config_t adcConfig 					= { 0 }; //Configuración del ADC
ADC_Config_Event_t adcConfigEvent		= { 0 }; //Configuración del evento externo

//Handler para la carrera de LEDS
raceLED handlerRaceLED 					= { 0 };

//Funciones definidas para el desarrollo del proyecto
void initSystem(void); //Inicialización del sistema
void parseCommands(char *ptrBufferReception); //Recepción de comandos
char* intensityColorCarsFunction (uint16_t* carsIntensityADC);
void updateRaceModeOLED(void);
void updatePartyOLEDFunction(void);
void updateJoyStickModeOLED(void);

int main(void)
{
	setTo100M();  //Se pone la CPU a 100MHz
	initSystem(); //Se inicializan los periféricos

	while (1)
	{
		//el sistema está constantemente verificando si se levanta la bandeta de recepción del USART2
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
				parseCommands(bufferReception);
				stringComplete = false;			 //Se baja la bandera
			}
			rxData = '\0';
		}

		//CONVERSIÓN ADC PARA LA INTENSIDAD DE LOS COLORES:
		if ((adcIsComplete == true) & (intensityConfigFlag == 1)) {

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

				sprintf(bufferTx, "\n\rADC1: %u\n", adcData[0]);
				writeMsg(&handlerUsart2, bufferTx);
				sprintf(bufferTx, "ADC2: %u\n", adcData[1]);
				writeMsg(&handlerUsart2, bufferTx);
				sprintf(bufferTx, "ADC3: %u\n", adcData[2]);
				writeMsg(&handlerUsart2, bufferTx);
				sprintf(bufferTx, "ADC4: %u\n", adcData[3]);
				writeMsg(&handlerUsart2, bufferTx);
				sprintf(bufferTx, "ADCx: %u\n", adcData[4]);
				writeMsg(&handlerUsart2, bufferTx);
				sprintf(bufferTx, "ADCy: %u\n\r", adcData[5]);
				writeMsg(&handlerUsart2, bufferTx);

				counterADC = 0;

				intensityColorCarsFunction(adcData);
				showFourCarsToConfig(posP1, posP2, posP3, posP4, intensityColorCars[0], intensityColorCars[1], intensityColorCars[2],
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

		if((adcIsComplete == true) & (joyStickModeFlag == 1)) {

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
				sprintf(bufferTx, "ADCx: %u\n", adcData[4]);
				writeMsg(&handlerUsart2, bufferTx);
				sprintf(bufferTx, "ADCy: %u\n\r", adcData[5]);
				writeMsg(&handlerUsart2, bufferTx);

				counterADC = 0;

				if ((3200 < adcData[5]) & (adcData[5] < 5000)) {

					if(joyStickModePosition < 179){
						joyStickModePosition++;
					}

					else if(joyStickModePosition == 179){
						joyStickModePosition = 1;
					}

				}

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

				if ((3200 < adcData[4]) & (adcData[4] < 5000)) {

					colorJoyStickMode = (rand() % 6) + 1;

				}

				else if ((0 < adcData[4]) & (adcData[4] < 500)) {

					colorJoyStickMode = (rand() % 6) + 1;

				}

				else {

					__NOP();

				}

				moveCarJoyStickMode (joyStickModePosition, colorJoyStickMode, &handlerPWMOutput);
				updateJoyStickModeOLED();
			}

			else {
				adcIsComplete = false;
				counterADC = 0;
			}


		}

		//MODO RACE 2 PLAYERS:
		if (updateRaceModeFlag) {

			updateRaceModeFlag = 0;

			if(raceModeFlag){

				moveCarsFourPlayers (posP1, posP2, posP3, posP4, intensityColorCars[0], intensityColorCars[1], intensityColorCars[2],
												intensityColorCars[3], &handlerPWMOutput);
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

		if (lapCounterP2 == handlerRaceLED.numberOfLaps) {
			raceModeFlag = 0;
			lapCounterP2 = 0;

			clearAllStrip(&handlerPWMOutput);
			ResetTime(&handlerPWMOutput);

			for (uint16_t i = 0; i < 540; i += 3) {
				buffer[i] = 255;
			}

			//Se prende la cinta con los colores del arreglo
			for (uint16_t i = 0; i < 540; i++) {
				colorByte(buffer[i], &handlerPWMOutput);
			}

			sprintf(bufferTx, " GANADOR: VERDE     ");
			clearAllScreen(&handlerI2COLED);
			setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
			setColumn(&handlerI2COLED, 0x01);
			printBytesArray(&handlerI2COLED, bufferTx);

			delayms(1000);

			clearAllStrip(&handlerPWMOutput);
		}

		if (lapCounterP1 == handlerRaceLED.numberOfLaps) {
			raceModeFlag = 0;
			lapCounterP1 = 0;

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
			raceModeFlag = 0;
			lapCounterP3 = 0;

			clearAllStrip(&handlerPWMOutput);
			ResetTime(&handlerPWMOutput);

			for (uint16_t i = 1; i < 540; i += 3) {
				buffer[i] = 255;
			}

			for (uint16_t i = 2; i < 540; i += 3) {
				buffer[i] = (255*30)/100;
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
			raceModeFlag = 0;
			lapCounterP4 = 0;

			clearAllStrip(&handlerPWMOutput);
			ResetTime(&handlerPWMOutput);

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

void adcComplete_Callback(void){
	adcIsComplete = true;
	counterADC++;
}

void BasicTimer10_Callback(void) {
	GPIOxTooglePin(&handlerBlinkyPin);
	partyModeUpdateFlag = 1;
	}

void BasicTimer3_Callback(void){
	updateRaceModeFlag = 1;
	updateJoyStickModeFlag = 1;
}

void callback_extInt10(void) {
//Se sube la bandera del PinClock a 1
	posP2++;
	flagP2 = 1;
}

void callback_extInt15(void) {
//Se sube la bandera del PinClock a 1
	posP1++;
	flagP1 = 1;
}

void usart2Rx_Callback(void){
	//Activamos una bandera, dentro de la función main se lee el registro DR lo que baja la bandera de la interrupción
	rxDataFlag = 1;
}

char* intensityColorCarsFunction (uint16_t* carsIntensityADC){

	intensityColorCars[0] = carsIntensityADC[0]/16;
	intensityColorCars[1] = carsIntensityADC[1]/16;
	intensityColorCars[2] = carsIntensityADC[2]/16;
	intensityColorCars[3] = carsIntensityADC[3]/16;

	return intensityColorCars;

}

void updatePartyOLEDFunction(void){

	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_0);
	printBytesArray(&handlerI2COLED, "          PARTY ");
	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
	printBytesArray(&handlerI2COLED, "PARTY           ");
	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_4);
	printBytesArray(&handlerI2COLED, "     PARTY      ");
	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_6);
	printBytesArray(&handlerI2COLED,"          PARTY ");

	delayms(300);

	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_0);
	printBytesArray(&handlerI2COLED, "PARTY           ");
	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
	printBytesArray(&handlerI2COLED, "     PARTY      ");
	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_4);
	printBytesArray(&handlerI2COLED, "          PARTY ");
	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_6);
	printBytesArray(&handlerI2COLED, "PARTY           ");

	delayms(300);

	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_0);
	printBytesArray(&handlerI2COLED, "     PARTY      ");
	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
	printBytesArray(&handlerI2COLED, "          PARTY ");
	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_4);
	printBytesArray(&handlerI2COLED, "PARTY           ");
	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_6);
	printBytesArray(&handlerI2COLED, "     PARTY      ");

	delayms(300);
}

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

void parseCommands(char *ptrBufferReception) {
	//esta función lee lo obtenido por el puerto serial y toma decisiones en base a eso

	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter,
			&secondParameter, userMsg);

	if (strcmp(cmd, "help") == 0) {

		disableEvent(&handlerPWMTimer);
		disableOutput(&handlerPWMTimer);
		stopPwmSignal(&handlerPWMTimer);

		joyStickModeFlag        = 0;
		partyModeFlag		    = 0;
		autodestructionModeFlag = 0;
		raceModeFlag			= 0;
		counterRaceState        = 0;
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
				"inicilizara en modo de 2 jugadores y 3 vueltas.\n\r");

		writeMsg(&handlerUsart2, "Para configurar el modo de juego:\n"
				"Con el comando setRaceMode #numeroDeJugadores #numeroDeVueltas @ se configura el modo de juego.\n"
				"Por ejemplo, para 2 jugadores y 5 vueltas: "
				"Con el comando setRaceMode #numeroDeJugadores #numeroDeVueltas @ se configura el modo de juego.\n"
				"Por ejemplo,para 2 jugadores y 5 vueltas: setPlayersMode 2 5 @\n\r"
				);

		writeMsg(&handlerUsart2, "Para iniciar la carrera:\n"
				"La carrera se inicia con el comando initRace @, este comando empezara a hacer una cuenta reegresiva de 5 segundos\n"
				"y posteriormente se dara inicio a la carrera.\n\r");

		writeMsg(&handlerUsart2, "Al finalizar la carrera se mostrará en la pantalla el color ganador, adicionalmente, la pista se pintara\n"
				"toda de este color.\n\r");

		writeMsg(&handlerUsart2, "Otros modos:\n"
				"Adicional al juego de carreras, la pista de LEDs se puede poner en otros modos:\n"
				"1.Modo Fiesta: se inicializa con el comando setPartyMode @, al enviar este comando, la pista se encendera aleatoriamente de\n"
				"diferentes colores.\n\r"
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

		clearAllScreen(&handlerI2COLED);
		joyStickModeFlag        = 0;
		partyModeFlag		    = 0;
		autodestructionModeFlag = 0;
		counterRaceState        = 0;
		intensityConfigFlag		= 0;
		raceModeFlag			= 0;
		//delayms(100);


		if ((firstParameter != 2) & (firstParameter != 4)) {
			writeMsg(&handlerUsart2, "Por favor, ingrese un numero de jugadores valido \n\r");
		}

		else if (secondParameter > 10){
			writeMsg(&handlerUsart2, "¿Seguro que desea configurar mas de 10 vueltas?\n\r");
		}

		else{

			writeMsg(&handlerUsart2, "Modo de juego configurado:\n");
			sprintf(bufferTx, "Numero de jugadores: %d\n", firstParameter);
			writeMsg(&handlerUsart2, bufferTx);
			sprintf(bufferTx, "Numero de vueltas: %d\n\r", secondParameter);
			writeMsg(&handlerUsart2, bufferTx);

			handlerRaceLED.numberOfPlayers = firstParameter;
			handlerRaceLED.numberOfLaps    = secondParameter;

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

			writeMsg(&handlerUsart2, "Configure la intensidad de los colores deseada, posteriormente escriba el comando\n"
					"initRace @ para iniciar la carrera.");

			posP1 = 1;
			posP2 = 15;
			posP3 = 25;
			posP4 = 35;

			showFourCarsToConfig(posP1, posP2, posP3, posP4, 255, 255, 255, 255, &handlerPWMOutput);

			enableEvent(&handlerPWMTimer);
			enableOutput(&handlerPWMTimer);
			startPwmSignal(&handlerPWMTimer);

			intensityConfigFlag = 1;

			delayms(100);
		}

	}

	else if (strcmp(cmd, "initRace") == 0){

		clearAllScreen(&handlerI2COLED);
		joyStickModeFlag        = 0;
		intensityConfigFlag     = 0;
		partyModeFlag		    = 0;
		autodestructionModeFlag = 0;
		raceModeFlag       		= 0;
		counterRaceState   		= 0;

		disableEvent(&handlerPWMTimer);
		disableOutput(&handlerPWMTimer);
		stopPwmSignal(&handlerPWMTimer);

		posP1 = 1;
		posP2 = 15;
		posP3 = 25;
		posP4 = 35;


		lapCounterP1 = 0;
		lapCounterP2 = 0;
		lapCounterP3 = 0;
		lapCounterP4 = 0;


		//CONTEO REGRESIVO
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

		//Se pone el sonido
		Tone(&handlerPWMTimerBuzzer, 1);
		delayms(500);
		noTone(&handlerPWMTimerBuzzer);
		clearAllStrip(&handlerPWMOutput);
		delayms(100);
		delayms(1000);

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

		//Se pone el sonido
		Tone(&handlerPWMTimerBuzzer, 1);
		delayms(500);
		noTone(&handlerPWMTimerBuzzer);
		clearAllStrip(&handlerPWMOutput);
		delayms(100);
		delayms(1000);


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

		//Se envía un resetRaceMode 2 10 set, para que se encienda adecacuadamente la cinta
		ResetTime(&handlerPWMOutput);

		Tone(&handlerPWMTimerBuzzer, 2);

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

		raceModeFlag       = 1;
		counterRaceState   = 1;
	}

	else if (strcmp(cmd, "setPartyMode") == 0){

		joyStickModeFlag        = 0;
		partyModeFlag		    = 1;
		autodestructionModeFlag = 0;
		raceModeFlag			= 0;
		counterRaceState        = 0;
		intensityConfigFlag     = 0;

	}

	else if (strcmp(cmd, "initAutodestruction") == 0){

		joyStickModeFlag        = 0;
		partyModeFlag		    = 0;
		autodestructionModeFlag = 1;
		raceModeFlag			= 0;
		counterRaceState        = 0;
		intensityConfigFlag     = 0;

		//CONTEO REGRESIVO
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

		clearAllScreen(&handlerI2COLED);

		enableEvent(&handlerPWMTimer);
		enableOutput(&handlerPWMTimer);
		startPwmSignal(&handlerPWMTimer);

		joyStickModeFlag        = 1;
		partyModeFlag		    = 0;
		autodestructionModeFlag = 0;
		raceModeFlag			= 0;
		counterRaceState        = 0;
		intensityConfigFlag     = 0;

	}

	else {

		joyStickModeFlag        = 0;
		partyModeFlag		    = 0;
		autodestructionModeFlag = 0;
		raceModeFlag			= 0;
		counterRaceState        = 0;
		intensityConfigFlag     = 0;

		//Se llena el arreglo
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
	handlerIntTimer.TIMx_Config.TIMx_period 	= 20; //Update period = 15ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerIntTimer);

	//Se configura el TxPin (PIN por el cual se hace la transmisión)
	//Este PIN se configura en la función alternativa AF07 que corresponde al USART2
	handlerTxPin.pGPIOx = GPIOA;
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
	handlerRxPin.pGPIOx = GPIOA;
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

	//Se configura la carrera de LEDs, por defecto se pone con 3 vueltas y 2 jugadore
	handlerRaceLED.numberOfLaps    = 1;
	handlerRaceLED.numberOfPlayers = 2;

	//Se configura el Button: Se debe tener en cuenta que el modo entrada está configurado en el ExtiDriver
	handlerButton.pGPIOx 							 = GPIOC;
	handlerButton.GPIO_PinConfig.GPIO_PinNumber		 = PIN_10;
	handlerButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se configura el EXTI del botón
	ButtonExtiConfig.pGPIOHandler = &handlerButton;
	ButtonExtiConfig.edgeType     = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Se carga la configuración: al cargar la configuración del exti, se carga también la del GPIO
	extInt_Config(&ButtonExtiConfig);

	//Se configura el Button: Se debe tener en cuenta que el modo entrada está configurado en el ExtiDriver
	handlerUserButton.pGPIOx        					 = GPIOA;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber      = PIN_15;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;

	//Se configura el EXTI del botón
	UserButtonExtiConfig.pGPIOHandler = &handlerUserButton;
	UserButtonExtiConfig.edgeType     = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Se carga la configuración: al cargar la configuración del exti, se carga también la del GPIO
	extInt_Config(&UserButtonExtiConfig);


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

	initOLED(&handlerI2COLED);
	clearOLED(&handlerI2COLED);
	setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
	setColumn(&handlerI2COLED, 0x10);
	printBytesArray(&handlerI2COLED, " BIENVENIDO ");

	//Se limpia la cinta de LEDs
	clearAllStrip(&handlerPWMOutput);
	ResetTime(&handlerPWMOutput);
}



