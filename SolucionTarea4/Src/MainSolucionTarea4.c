/**
 *********************************************************************************************************
 *********************************************************************************************************
 * @file           : MainSolucionTarea4.c
 * @author         : Laura Alejandra Zuluaga Moreno - lazuluagamo@unal.edu.co
 * @brief          : Solución Tarea 4
 *
 *********************************************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "ExtiDriver.h"

//Definición de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin          = {0}; //Handler para el USER_LED
GPIO_Handler_t handlerButton        	 = {0}; //Handler para el Botón
GPIO_Handler_t handlerPinClock		     = {0}; //Handler para el Encoder entrada A
GPIO_Handler_t handlerPinData		     = {0}; //Handler para el Encoder entrada B
GPIO_Handler_t handlerTxPin              = {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerPinTensTransistor  = {0}; //Handler para el PIN del transistor que switchea las decenas
GPIO_Handler_t handlerPinUnitsTransistor = {0}; //Handler para el PIN del transistor que switchea las unidades
GPIO_Handler_t handlerPinSegmentA        = {0}; //Handler para el PIN del segmento A del display
GPIO_Handler_t handlerPinSegmentB        = {0}; //Handler para el PIN del segmento B del display
GPIO_Handler_t handlerPinSegmentC        = {0}; //Handler para el PIN del segmento C del display
GPIO_Handler_t handlerPinSegmentD        = {0}; //Handler para el PIN del segmento D del display
GPIO_Handler_t handlerPinSegmentE        = {0}; //Handler para el PIN del segmento E del display
GPIO_Handler_t handlerPinSegmentG        = {0}; //Handler para el PIN del segmento G del display
GPIO_Handler_t handlerPinSegmentF        = {0}; //Handler para el PIN del segmento F del display
USART_Handler_t handlerUsart2            = {0}; //Handler para el USART2
BasicTimer_Handler_t handlerBlinkyTimer  = {0}; //Handler para el TIMER2, con este se hará el Blinky
BasicTimer_Handler_t handlerAuxTimer 	 = {0}; //Handler para el TIMER3, con este se controla el Display
EXTI_Config_t PinClockExtiConfig         = {0}; //Exti Configuration para el PinClock
EXTI_Config_t ButtonExtiConfig           = {0}; //Exti Configuration para el Button

//Definición de otras variables necesarias para el desarrollo de los ejercicios:
uint8_t PinClockFlag = 0;  //Bandera del PinClock
uint8_t ButtonFlag   = 0;  //Bandera del Button
uint8_t CounterTens	 = 0;  //En esta variable se almacenan las decenas del contador
uint8_t CounterUnits = 0;  //En esta variable se almacenan las unidades del contador
uint8_t Counter_i 	 = 0;  //En esta variable se almacena el contador que controla el número del display
						   //y cuenta las vueltas del encoder
uint8_t PinDataState = 0;  //En esta variable se almacena la lectura del estado del PinData
uint8_t UnitsTransistorState = 0; //En esta variable se almacena la lectura del estado del UnitsTransistor
uint8_t TensTransistorState = 0;  //En esta variable se almacena la lectura del estado del TensTransistor
char Buffer[32]      = {0};       //En esta variable se almacenará el mensaje con el número y la dirección
char MessageToSend[] = "El botón está siendo presionado."; //Mensaje a enviar

//Definición de la cabecera de las funciones que se crean para el desarrollo de los ejercicios
void initSystem(void);               //Función para inicializar el sistema
void displayTens(uint8_t counter);   //Función para encender las decenas
void displayUnits(uint8_t counter);  //Función para encender las unidades

int main(void) {

	initSystem();  //Se inicializa el sistema, con la configuración de los periféricos que se van a usar

	while (1) {

		TensTransistorState  = GPIO_ReadPin(&handlerPinTensTransistor); //Se guarda el estado del TensTransistor
																		//en la variable
		UnitsTransistorState = GPIO_ReadPin(&handlerPinUnitsTransistor);//Se guarda el estado del TensTransistor
																		//en la variable


		//Con este if se cambia el display de 7 segmentos cada vez que se enciende la bandera del PinClock
		if (PinClockFlag == 1) {
			//Se Guarda el estado del PinData en la variable
			PinDataState = GPIO_ReadPin(&handlerPinData);

			//Si
			if (( PinDataState == 1) & (Counter_i < 50)){
				Counter_i++;
				//Se envía el mensaje por USART:
				sprintf(Buffer, "El giro es CW %u \n", Counter_i );
				writeMsg(&handlerUsart2, Buffer);
			}

			else if ((PinDataState == 0) & (Counter_i  > 0)) {
				Counter_i--;
				//Se envía el mensaje por USART:
				sprintf(Buffer, "El giro es CCW %u \n", Counter_i);
				writeMsg(&handlerUsart2, Buffer);
			}

			else {
				__NOP();
			}

			PinClockFlag = 0;
		}

		//En caso de que la que se haya encendido sea la bandera ButtonFlag, se envía un mensaje y se baja la bandera

		if (ButtonFlag == 1) {
			writeMsg(&handlerUsart2, MessageToSend);
			GPIO_WritePin(&handlerPinSegmentA, SET);
			GPIO_WritePin(&handlerPinSegmentB, RESET);
			GPIO_WritePin(&handlerPinSegmentC, RESET);
			GPIO_WritePin(&handlerPinSegmentD, SET);
			GPIO_WritePin(&handlerPinSegmentE, SET);
			GPIO_WritePin(&handlerPinSegmentF, RESET);
			GPIO_WritePin(&handlerPinSegmentG, RESET);
			ButtonFlag = 0;
		}
		else {
			__NOP();
		}

		if (TensTransistorState == 0) {
			displayTens(Counter_i);
		}
		else if (UnitsTransistorState == 0) {
			displayUnits(Counter_i);
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
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración y se enciende el LED
	GPIO_Config(&handlerBlinkyPin);
	GPIO_WritePin(&handlerBlinkyPin, SET);

	//Se configura el TxPin (PIN por el cual se hace la transmisión)
	//Este PIN se configura en la función alternativa AF08 que para el PIN A2 corresponde al USART2
	handlerTxPin.pGPIOx 							= GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN; //Función alternativa
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;	//AF07 para PIN A2

	//Se carga la configuración
	GPIO_Config(&handlerTxPin);

	//Se configura el Button

	handlerButton.pGPIOx 							 = GPIOC;
	handlerButton.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_6;
	handlerButton.GPIO_PinConfig.GPIO_PinMode 		 = GPIO_MODE_IN;
	handlerButton.GPIO_PinConfig.GPIO_PinOPType 	 = GPIO_OTYPE_PUSHPULL;
	handlerButton.GPIO_PinConfig.GPIO_PinSpeed 		 = GPIO_OSPEED_MEDIUM;
	handlerButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP; //Se le pone un PullUp

	ButtonExtiConfig.pGPIOHandler = &handlerButton;
	ButtonExtiConfig.edgeType 	  = EXTERNAL_INTERRUPT_FALLING_EDGE;

	//Se carga la configuración
	GPIO_Config(&handlerButton);
	extInt_Config(&ButtonExtiConfig);

	//Se configura el PinClock
	handlerPinClock.pGPIOx  							= GPIOB;
	handlerPinClock.GPIO_PinConfig.GPIO_PinNumber 		= PIN_3;
	handlerPinClock.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_IN;
	handlerPinClock.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinClock.GPIO_PinConfig.GPIO_PinSpeed 	 	= GPIO_OSPEED_FAST;
	handlerPinClock.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_PULLUP;
	//Configurando el Exti:
	PinClockExtiConfig.pGPIOHandler = &handlerPinClock;
	PinClockExtiConfig.edgeType 	= EXTERNAL_INTERRUPT_FALLING_EDGE;

	//Se carga la configuración
	//GPIO_Config(&handlerPinClock); //TODO ESTA LÍNEA ME LA PUEDO SALTAR?
	extInt_Config(&PinClockExtiConfig);

	//Se configura el PinData
	handlerPinData.pGPIOx 							  = GPIOC;
	handlerPinData.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_8;
	handlerPinData.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_IN;
	handlerPinData.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinData.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinData.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;

	//Se carga la configuración
	GPIO_Config(&handlerPinData);

	//Se configura el PinData
	handlerPinTensTransistor.pGPIOx 				         	= GPIOC;
	handlerPinTensTransistor.GPIO_PinConfig.GPIO_PinNumber 	  	= PIN_5;
	handlerPinTensTransistor.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	handlerPinTensTransistor.GPIO_PinConfig.GPIO_PinOPType 	  	= GPIO_OTYPE_PUSHPULL;
	handlerPinTensTransistor.GPIO_PinConfig.GPIO_PinSpeed 	  	= GPIO_OSPEED_FAST;
	handlerPinTensTransistor.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinTensTransistor);
	GPIO_WritePin(&handlerPinTensTransistor, SET);

	//Se configura el PinData
	handlerPinUnitsTransistor.pGPIOx 				         	 = GPIOC;
	handlerPinUnitsTransistor.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_4;
	handlerPinUnitsTransistor.GPIO_PinConfig.GPIO_PinMode 		 = GPIO_MODE_OUT;
	handlerPinUnitsTransistor.GPIO_PinConfig.GPIO_PinOPType 	 = GPIO_OTYPE_PUSHPULL;
	handlerPinUnitsTransistor.GPIO_PinConfig.GPIO_PinSpeed 	  	 = GPIO_OSPEED_FAST;
	handlerPinUnitsTransistor.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinUnitsTransistor);
	GPIO_WritePin(&handlerPinUnitsTransistor, RESET);

	//Se configura el PinSegmentA
	handlerPinSegmentA.pGPIOx 				         	  = GPIOA;
	handlerPinSegmentA.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_6;
	handlerPinSegmentA.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentA.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentA.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentA);
	GPIO_WritePin(&handlerPinSegmentA, SET);

	//Se configura el PinSegmentB
	handlerPinSegmentB.pGPIOx 				         	  = GPIOA;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_7;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentB);
	GPIO_WritePin(&handlerPinSegmentB, SET);


	//Se configura el PinSegmentC
	handlerPinSegmentC.pGPIOx 				         	  = GPIOA;
	handlerPinSegmentC.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_8;
	handlerPinSegmentC.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentC.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentC.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentC.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentC);
	GPIO_WritePin(&handlerPinSegmentC, SET);

	//Se configura el PinSegmentD
	handlerPinSegmentD.pGPIOx 				         	  = GPIOA;
	handlerPinSegmentD.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_9;
	handlerPinSegmentD.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentD.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentD.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentD);
	GPIO_WritePin(&handlerPinSegmentD, SET);

	//Se configura el PinSegmentE
	handlerPinSegmentE.pGPIOx 				         	  = GPIOA;
	handlerPinSegmentE.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_10;
	handlerPinSegmentE.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentE.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentE.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentE.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentE);
	GPIO_WritePin(&handlerPinSegmentE, SET);

	//Se configura el PinSegmentF
	handlerPinSegmentF.pGPIOx 				         	  = GPIOA;
	handlerPinSegmentF.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_11;
	handlerPinSegmentF.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentF.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentF.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentF.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentF);
	GPIO_WritePin(&handlerPinSegmentF, SET);

	//Se configura el PinSegmentG
	handlerPinSegmentG.pGPIOx 				         	  = GPIOA;
	handlerPinSegmentG.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_12;
	handlerPinSegmentG.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentG.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentG.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentG.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentG);
	GPIO_WritePin(&handlerPinSegmentG, SET);

	//Se configura el BlinkyTimer
	handlerBlinkyTimer.ptrTIMx 					= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 	= 2500; //Update period= 100us*2500 = 250000us = 250ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerBlinkyTimer);

	//Se configura el Timer Auxiliar
	handlerAuxTimer.ptrTIMx 				= TIM3;
	handlerAuxTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerAuxTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_100us;
	handlerAuxTimer.TIMx_Config.TIMx_period = 100; //Update period= 100us*100 = 10000us = 10ms

	//Se carga la configuración del AuxTimer
	BasicTimer_Config(&handlerAuxTimer);

	//Se configura el USART 2
	handlerUsart2.ptrUSARTx					  = USART2;                //USART 2
	handlerUsart2.USART_Config.USART_mode 	  = USART_MODE_RXTX;       //Modo de solo transmisión
	handlerUsart2.USART_Config.USART_baudrate = USART_BAUDRATE_115200; //115200 bps
	handlerUsart2.USART_Config.USART_parity   = USART_PARITY_NONE;     //Parity:NONE, acá viene configurado el tamaño de dato
	handlerUsart2.USART_Config.USART_stopbits = USART_STOPBIT_1;	   //Un stopbit

	//Se carga la configuración del USART
	USART_Config(&handlerUsart2);

}

//Función Callback del BlinkyTimer
void BasicTimer2_Callback(void) {
	GPIOxTooglePin(&handlerBlinkyPin); //Blinky del LED de estado
}

/*Función Callback de la EXTI del PinClock: Esta interrupción está configurada en flanco de bajada, así
  siempre que haya un flanco de bajada en la señal del PinClock, se sube la bandera*/

void BasicTimer3_Callback(void) {
//Se switchean los transistores, de forma que se muestre un número y luego el otro lo suficientemente rápido
	GPIOxTooglePin(&handlerPinUnitsTransistor);
	GPIOxTooglePin(&handlerPinTensTransistor);
}

void callback_extInt3(void) {
	PinClockFlag = 1; //Se sube la bandera del PinClock a 1
}

/*Función Callback de la EXTI del Button: Esta interrupción está configurada en flanco de bajada, así
  siempre que haya un flanco de bajada en el Botón (es decir, cuando este es presionado),se sube la
  bandera*/
void callback_extInt6(void) {
	ButtonFlag = 1;  //Se sube la bandera del ButtonFlag a 1
}

void displayUnits(uint8_t counter){

	CounterUnits = counter - (counter/10)*10;

	switch(CounterUnits) {

	case 0: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 1: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 2: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 3: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 4: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 5: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 6: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 7: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 8: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 9: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	default: {
		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, SET);
	}

	}
}


void displayTens(uint8_t counter){

	CounterTens = counter/10;

	switch(CounterTens) {

	case 0: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 1: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 2: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 3: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 4: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 5: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 6: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 7: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 8: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 9: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	default: {
		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, SET);
	}
	}
}
