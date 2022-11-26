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
#include <stdlib.h>
#include "RccConfig.h"
#include "ExtiDriver.h"
#include "PWMDriver.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "WS2812bDriver.h"
#include "USARTxDriver.h"
#include <stdbool.h>
#include "string.h"

//Buffer auxiliar para almacenar los colores de la cinta
uint8_t buffer[180] = {0};

//Contadores de la posición de los jugadores en la pista(número de LED en el que están)
uint8_t posP1 = 1; //Jugador 1
uint8_t posP2 = 15; //Jugador 2
uint8_t posP4 = 5; //Jugador 3
uint8_t posP3 = 10; //Jugador 4

//Contador de vueltas para los jugadores
uint8_t lapCounterP1 = 0; //Jugador 1
uint8_t lapCounterP2 = 0; //Jugador 2
uint8_t lapCounterP3 = 0; //Jugador 3
uint8_t lapCounterP4 = 0; //Jugador 4

//Banderas auxiliares para la activavión y actualización del modo Party
uint8_t partyModeFlag 		 = 0;
uint8_t partyModeUpdateFlag  = 0;

//Bandera auxiliar para el modo race
uint8_t raceModeFlag = 0;

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

//Handler de los GPIOs usados
GPIO_Handler_t handlerBlinkyPin 	    = { 0 };  //Handler para el LED de estado
GPIO_Handler_t handlerMCO_2  			= { 0 };  //Handler para el PIN de salida del Clock
GPIO_Handler_t handlerPWMOutput 		= { 0 };  //Handler para la salida del PWM
GPIO_Handler_t handlerUserButton 		= { 0 };
GPIO_Handler_t handlerButton 		    = { 0 };
GPIO_Handler_t handlerTxPin             = { 0 }; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerRxPin             = { 0 }; //Handler para el PIN por el cual se hará la transmisión

//Handler para el USART2
USART_Handler_t handlerUsart2           = { 0 }; //Handler para el USART2

//Handler de las interrupciones externas
EXTI_Config_t ButtonExtiConfig          = { 0 };
EXTI_Config_t UserButtonExtiConfig      = { 0 };

//Handler para la carrera de LEDS
raceLED handlerRaceLED = { 0 };

//Funciones definidas para el desarrollo del proyecto
void initSystem(void); //Inicialización del sistema
void parseCommands(char *ptrBufferReception); //Recepción de comandos

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

		else {

		}

		//MODO PARTY:
		if(partyModeFlag & partyModeUpdateFlag){
			//Si las dos banderas están levantadas, estamos en modo party y se da una actualización

			partyModeUpdateFlag = 0; //Se baja la bandera de actualización

			//Se llena el arreglo con número aleatorios entre el 0 y el 255
			for (uint8_t i = 0; i < 180; i++) {
				buffer[i] = rand() % 256;
			}

			//Se prende la cinta con los colores del arreglo
			for (uint8_t i = 0; i < 180; i++) {
				colorByte(buffer[i], &handlerPWMOutput);
			}

			//Se envía un reset, para que se encienda adecacuadamente la cinta
			ResetTime(&handlerPWMOutput);

		}

		//MODO RACE 2 PLAYERS:
		if ((flagP1 == 1)|(flagP2 == 1)){

			if(raceModeFlag == 1){

				flagP1 = 0;
				flagP2 = 0;

				moveCarsFourPlayers(posP1, posP2, posP3, posP4, &handlerPWMOutput);

				if (posP1 == 52) {
					posP1 = 0;
					lapCounterP1++;
				}

				if (posP2 == 52) {
					posP2 = 0;
					lapCounterP2++;
				}

				if (lapCounterP2 == handlerRaceLED.numberOfLaps) {
					raceModeFlag = 0;
					clearLEDS(60, &handlerPWMOutput);
				}

				else if (lapCounterP1 == handlerRaceLED.numberOfLaps) {
					raceModeFlag = 0;
					clearLEDS(60, &handlerPWMOutput);
					ResetTime(&handlerPWMOutput);
				}

				else{
					__NOP();
				}

			}

			else{
				__NOP();
			}
		}
	}

	return 0;
}

void BasicTimer2_Callback(void) {
	GPIOxTooglePin(&handlerBlinkyPin);
	partyModeUpdateFlag = 1;
	}

void callback_extInt13(void) {
//Se sube la bandera del PinClock a 1
	posP1++;
	flagP1 = 1;
}

void callback_extInt8(void) {
//Se sube la bandera del PinClock a 1
	posP2++;
	flagP2 = 1;
}

void usart2Rx_Callback(void){
	//Activamos una bandera, dentro de la función main se lee el registro DR lo que baja la bandera de la interrupción
	rxDataFlag = 1;
}

void parseCommands(char *ptrBufferReception) {
	//esta función lee lo obtenido por el puerto serial y toma decisiones en base a eso

	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter,
			&secondParameter, userMsg);

	if (strcmp(cmd, "help") == 0) {

		raceModeFlag  = 0;
		partyModeFlag = 0;

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

	}

	else if (strcmp(cmd, "setRaceMode") == 0) {

		raceModeFlag  = 0;
		partyModeFlag = 0;

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
		}

	}

	else if (strcmp(cmd, "initRace") == 0){

		clearLEDS(60, &handlerPWMOutput);

		writeMsg(&handlerUsart2, "La carrera iniciara en...:\n");
		writeMsg(&handlerUsart2, "5...:\n");
		delayms(1000);
		writeMsg(&handlerUsart2, "4...:\n");
		delayms(1000);
		writeMsg(&handlerUsart2, "3...:\n");
		delayms(1000);
		writeMsg(&handlerUsart2, "2...:\n");
		delayms(1000);
		writeMsg(&handlerUsart2, "1...:\n");
		delayms(1000);
		writeMsg(&handlerUsart2, "GO!:\n");
		delayms(1000);

		posP1 = 1;
		posP2 = 15;
		posP3 = 37;
		posP4 = 27;

		raceModeFlag  = 1;
		partyModeFlag = 0;

		lapCounterP1 = 0;
		lapCounterP2 = 0;
		lapCounterP3 = 0;
		lapCounterP4 = 0;
	}

	else if (strcmp(cmd, "setPartyMode") == 0){

		partyModeFlag = 1;
		raceModeFlag  = 0;

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
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinNumber 		  = PIN_0;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinMode 	      = GPIO_MODE_OUT;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinOPType 		  = GPIO_OTYPE_PUSHPULL;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinSpeed 	      = GPIO_OSPEED_FAST;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinPuPdControl	  = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPWMOutput);

	//Se pone el PIN en 0:
	GPIO_WritePin(&handlerPWMOutput, RESET);

	//Se configura el BlinkyTimer
	handlerBlinkyTimer.ptrTIMx 					= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_100M_05ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 	= 500; //Update period= 0.05ms*500 = 250ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerBlinkyTimer);

	//Se configura el BlinkyTimer
	handlerIntTimer.ptrTIMx 					= TIM3;
	handlerIntTimer.TIMx_Config.TIMx_mode 	    = BTIMER_MODE_UP;
	handlerIntTimer.TIMx_Config.TIMx_speed 	    = 0;
	handlerIntTimer.TIMx_Config.TIMx_period 	= 1; //Update period = 10ns

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerIntTimer);


	handlerMCO_2.pGPIOx 						    = GPIOC;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinNumber 		= PIN_9;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinAltFunMode  = AF0;

	GPIO_Config(&handlerMCO_2);

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
	handlerButton.pGPIOx 							 = GPIOA;
	handlerButton.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_8;
	handlerButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;

	//Se configura el EXTI del botón
	ButtonExtiConfig.pGPIOHandler = &handlerButton;
	ButtonExtiConfig.edgeType     = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Se carga la configuración: al cargar la configuración del exti, se carga también la del GPIO
	extInt_Config(&ButtonExtiConfig);

	//Se configura el Button: Se debe tener en cuenta que el modo entrada está configurado en el ExtiDriver
	handlerUserButton.pGPIOx							 = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber      = PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se configura el EXTI del botón
	UserButtonExtiConfig.pGPIOHandler = &handlerUserButton;
	UserButtonExtiConfig.edgeType     = EXTERNAL_INTERRUPT_RISING_EDGE;

	//Se carga la configuración: al cargar la configuración del exti, se carga también la del GPIO
	extInt_Config(&UserButtonExtiConfig);

	//Se limpia la cinta de LEDs
	clearLEDS(52, &handlerPWMOutput);
}



