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
GPIO_Handler_t handlerButton        	 = {0}; //Handler para el botón
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
GPIO_Handler_t handlerPinSegmentF        = {0}; //Handler para el PIN del segmento A del display
USART_Handler_t handlerUsart2            = {0}; //Handler para el USART
BasicTimer_Handler_t handlerBlinkyTimer  = {0}; //Handler para el TIMER2, con este se hará el Blinky
BasicTimer_Handler_t handlerAuxTimer 	 = {0}; //Handler para el TIMER3, con este se controla el Display
EXTI_Config_t PinClockExtiConfig         = {0}; //Exti Configuration para el PinClock
EXTI_Config_t ButtonExtiConfig           = {0}; //Exti Configuration para el Button





//Definición de otras variables necesarias para el desarrollo de los ejercicios:
uint8_t PinClockFlag = 0; //Bandera del PinClock
uint8_t ButtonFlag   = 0; //Bandera del Button
uint8_t CounterTens	 = 0; //En esta variable se almacenan las decenas del contador
uint8_t CounterUnits = 0; //En esta variable se almacenan las unidades del contador
uint8_t Counter_i 	 = 0; //En esta variable se almacena el contador que controla el número del display
						  //y cuenta las vueltas del encoder
uint8_t PinDataState;     //En esta variable se almacena la lectura del estado del PinData
char MessageToSend[] = "El botón está siendo presionado."; //Mensaje a enviar
char Buffer[20]      = {0};


//Definición de la cabecera de las funciones que se crean para el desarrollo de los ejercicios
void initSystem(void);               //Función para inicializar el sistema
void displayNumber(uint8_t counter); //Función para encender el número en el display
void displayTens(uint8_t counter);   //Función para encender las decenas
void displayUnits(uint8_t counter);  //Función para encender las unidades


int main(void) {

	initSystem();  //Se inicializa el sistema, con la configuración de los periféricos que se van a usar

	while (1) {

		//Con este if se cambia el display de 7 segmentos cada vez que se enciende la bandera del PinClock
		if (PinClockFlag == 1) {
			PinDataState = GPIO_ReadPin(&handlerPinData);

			if (( PinDataState== 1) & (Counter_i < 50)){
				Counter_i++;

				sprintf(Buffer, "El giro es CW %u", Counter_i );
				writeMsg(&handlerUsart2, Buffer);

				//writeMsg(&handlerUsart2,); //TODO CÓMO ENVIAR UN MENSAJE CON EL VALOR DE UNA VARIABLE
				}
			else if ((PinClockFlag == 0) & (Counter_i  > 50)) {
				Counter_i--;

			}
			else {
				__NOP();
			}

			displayTens(Counter_i);
			displayUnits(Counter_i);
			PinClockFlag = 0;
		}

		//En caso de que la que se haya encendido sea la bandera ButtonFlag, se envía un mensaje y se baja la bandera
		else if (ButtonFlag == 1){
			writeMsg(&handlerUsart2, MessageToSend);
			ButtonFlag = 0;
		}

		else {
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
	handlerButton.GPIO_PinConfig.GPIO_PinSpeed 		 = GPIO_OSPEED_FAST;
	handlerButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP; //Se le pone un PullUp

	ButtonExtiConfig.pGPIOHandler = &handlerButton;
	ButtonExtiConfig.edgeType 	  = EXTERNAL_INTERRUPT_FALLING_EDGE;

	//Se carga la configuración
	GPIO_Config(&handlerButton);
	extInt_Config(&ButtonExtiConfig);

	//Se configura el PinClock
	handlerPinClock.pGPIOx  							= GPIOC;
	handlerPinClock.GPIO_PinConfig.GPIO_PinNumber 		= PIN_11;
	handlerPinClock.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_IN;
	handlerPinClock.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinClock.GPIO_PinConfig.GPIO_PinSpeed 	 	= GPIO_OSPEED_FAST;
	handlerPinClock.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	//Configurando el Exti:
	PinClockExtiConfig.pGPIOHandler = &handlerPinClock;
	PinClockExtiConfig.edgeType 	= EXTERNAL_INTERRUPT_FALLING_EDGE;

	//Se carga la configuración
	GPIO_Config(&handlerPinClock); //TODO ESTA LÍNEA ME LA PUEDO SALTAR?
	extInt_Config(&PinClockExtiConfig);

	//Se configura el PinData
	handlerPinData.pGPIOx 							  = GPIOC;
	handlerPinData.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_12;
	handlerPinData.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_IN;
	handlerPinData.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinData.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinData.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

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

	//Se configura el PinData
	handlerPinUnitsTransistor.pGPIOx 				         	 = GPIOC;
	handlerPinUnitsTransistor.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_4;
	handlerPinUnitsTransistor.GPIO_PinConfig.GPIO_PinMode 		 = GPIO_MODE_OUT;
	handlerPinUnitsTransistor.GPIO_PinConfig.GPIO_PinOPType 	 = GPIO_OTYPE_PUSHPULL;
	handlerPinUnitsTransistor.GPIO_PinConfig.GPIO_PinSpeed 	  	 = GPIO_OSPEED_FAST;
	handlerPinUnitsTransistor.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinUnitsTransistor);

	//Se configura el PinSegmentA
	handlerPinSegmentA.pGPIOx 				         	  = GPIOC;
	handlerPinSegmentA.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_4;
	handlerPinSegmentA.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentA.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentA.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentA);

	//Se configura el PinSegmentB
	handlerPinSegmentB.pGPIOx 				         	  = GPIOC;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_4;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentB);


	//Se configura el PinSegmentC
	handlerPinSegmentC.pGPIOx 				         	  = GPIOC;
	handlerPinSegmentC.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_4;
	handlerPinSegmentC.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentC.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentC.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentC.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentC);

	//Se configura el PinSegmentD
	handlerPinSegmentD.pGPIOx 				         	  = GPIOC;
	handlerPinSegmentD.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_4;
	handlerPinSegmentD.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentD.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentD.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentD);

	//Se configura el PinSegmentE
	handlerPinSegmentE.pGPIOx 				         	  = GPIOC;
	handlerPinSegmentE.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_4;
	handlerPinSegmentE.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentE.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentE.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentE.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentE);

	//Se configura el PinSegmentF
	handlerPinSegmentF.pGPIOx 				         	  = GPIOC;
	handlerPinSegmentF.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_4;
	handlerPinSegmentF.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentF.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentF.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentF.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentF);

	//Se configura el PinSegmentG
	handlerPinSegmentG.pGPIOx 				         	  = GPIOC;
	handlerPinSegmentG.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_4;
	handlerPinSegmentG.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentG.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentG.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentG.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentG);

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
	handlerAuxTimer.TIMx_Config.TIMx_period = 10; //Update period= 100us*10 = 1000us = 1ms

	//Se carga la configuración del AuxTimer
	BasicTimer_Config(&handlerAuxTimer);

	//Se configura el USART 2
	handlerUsart2.ptrUSARTx					  = USART2;                //USART 2
	handlerUsart2.USART_Config.USART_mode 	  = USART_MODE_TX;         //Modo de solo transmisión
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
	GPIOxTooglePin(&handlerPinTensTransistor);
	GPIOxTooglePin(&handlerPinUnitsTransistor);

}

void callback_extInt11(void) {
	PinClockFlag++; //Se sube la bandera del PinClock a 1
}

/*Función Callback de la EXTI del Button: Esta interrupción está configurada en flanco de bajada, así
  siempre que haya un flanco de bajada en el Botón (es decir, cuando este es presionado),se sube la
  bandera*/
void callback_extInt6(void) {
	ButtonFlag++;  //Se sube la bandera del ButtonFlag a 1
}

void displayTens(uint8_t counter){
	CounterUnits = counter - (counter/10)*10;

	switch(CounterUnits) {

	case 0: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 1: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 2: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 3: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 4: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 5: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 6: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 7: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 8: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 9: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	default: {
		__NOP();
	}

	}

}

void displayUnits(uint8_t counter){

	CounterTens = counter/10;

	switch(CounterTens) {

	case 0: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 1: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 2: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, RESET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 3: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 4: {

		GPIO_WritePin(&handlerPinSegmentA, RESET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 5: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 6: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, RESET);

		break;
	}

	case 7: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, RESET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, RESET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, RESET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 8: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, SET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	case 9: {

		GPIO_WritePin(&handlerPinSegmentA, SET);
		GPIO_WritePin(&handlerPinSegmentB, SET);
		GPIO_WritePin(&handlerPinSegmentC, SET);
		GPIO_WritePin(&handlerPinSegmentD, SET);
		GPIO_WritePin(&handlerPinSegmentE, RESET);
		GPIO_WritePin(&handlerPinSegmentF, SET);
		GPIO_WritePin(&handlerPinSegmentG, SET);

		break;
	}

	default: {
		__NOP();
	}

	}
}
