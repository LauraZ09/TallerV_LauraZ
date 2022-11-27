/**
 *********************************************************************************************************
 *********************************************************************************************************
 * @file           : MainADC.c
 * @author         : Laura Alejandra Zuluaga Moreno - lazuluagamo@unal.edu.co
 * @brief          : ADC
 *
 *********************************************************************************************************
 */


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"
#include "ExtiDriver.h"
#include "PwmDriver.h"
#include "RccConfig.h"


//Definición de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin          = {0}; //Handler para el USER_LED
GPIO_Handler_t handlerUserButton         = {0}; //Handler para el Botón
GPIO_Handler_t handlerTxPin              = {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerRxPin              = {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerPinTriggerAdc		 = {0}; //Handler para el Trigger externo
USART_Handler_t handlerUsart2            = {0}; //Handler para el USART2
BasicTimer_Handler_t handlerBlinkyTimer  = {0}; //Handler para el TIMER2, con este se hará el Blinky
ADC_Config_t adcConfig 					 = {0}; //Configuración del ADC
ADC_Config_Event_t adcConfigEvent		 = {0}; //Configuración del evento externo
PWM_Handler_t handlerPWMTimer 	         = {0}; //Handler para el PWM (Timer)

//Definición de otras variables necesarias para el desarrollo de los ejercicios:
uint8_t i          = 0;      //Contador
uint8_t rxData     = 0;      //Datos de recepción
uint8_t channels[4]= {ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_4, ADC_CHANNEL_8};
uint16_t adcData[4]= {0};  //Datos del ADC
uint8_t adcFlag	   = 0;	     //Bandera del ADC+
bool adcIsComplete = false;  //Bandera para la interrupción
char Buffer[64]    = {0};    //En esta variable se almacenarán mensajes
char greetingMsg[] = "SIUU \n\r"; //Mensaje que se imprime

//Definición de la cabecera de las funciones que se crean para el desarrollo de los ejercicios
void initSystem(void);       //Función para inicializar el sistema

int main(void) {

	setTo100M();
	initSystem();  //Se inicializa el sistema, con la configuración de los periféricos que se van a usar
	enableOutput(&handlerPWMTimer);
	enableEvent(&handlerPWMTimer);
	startPwmSignal(&handlerPWMTimer);
	writeChar(&handlerUsart2,' ');

	while (1) {
		//El sistema siempre está verificando si el valor de rxData ha cambiado
		//(lo cual sucede en la ISR de la recepción)
		//Si este valor dejó de ser '\0' significa que se recibió un carácter
		//por lo tanto entra en el bloque if para analizar que se recibió
		/*if(rxData != '\0'){
			//Se imprime el carácter recibido
			writeChar(&handlerUsart2, rxData);

			if(rxData == 'm'){
				//Se envía un mensaje
				writeMsg(&handlerUsart2,greetingMsg);
			}

			if(rxData == 's'){
				//Se lanza una nueva conversión ADC
				startSingleADC();
			}

			rxData = '\0';
		}*/

		if (adcIsComplete == true) {

			if(i == 1){
				adcIsComplete = false;
				adcData[0] = getADC();
			}

			else if (i == 2){
				adcIsComplete = false;
				adcData[1] = getADC();
			}

			else if (i == 3){
				adcIsComplete = false;
				adcData[2] = getADC();
			}

			else if (i == 4){
				adcIsComplete = false;
				adcData[3] = getADC();

				sprintf(Buffer, "\n\rADC1: %u\n", adcData[0]);
				writeMsg(&handlerUsart2, Buffer);
				sprintf(Buffer, "ADC2: %u\n", adcData[1]);
				writeMsg(&handlerUsart2, Buffer);
				sprintf(Buffer, "ADC3: %u\n", adcData[2]);
				writeMsg(&handlerUsart2, Buffer);
				sprintf(Buffer, "ADC4: %u\n\r", adcData[3]);
				writeMsg(&handlerUsart2, Buffer);

				i = 0;
			}

			else {
			adcIsComplete = false;
			i = 0;
			}
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
	//Este PIN se configura en la función alternativa AF07 que corresponde al USART2
	handlerTxPin.pGPIOx 							= GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;    //Función alternativa
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;	              //AF07: Usart2

	//Se carga la configuración
	GPIO_Config(&handlerTxPin);

	//Se configura el TxPin (PIN por el cual se hace la transmisión)
	//Este PIN se configura en la función alternativa AF07 que corresponde al USART2
	handlerRxPin.pGPIOx 							= GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_3;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;    //Función alternativa
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;	              //AF07: Usart2

	//Se carga la configuración
	GPIO_Config(&handlerRxPin);

	//Se configura el TxPin (PIN por el cual se hace la transmisión)
	//Este PIN se configura en la función alternativa AF07 que corresponde al USART2
	handlerUserButton.pGPIOx 							 = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode 		 = GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerUserButton);

	//Se configura el PIN del evento externo
	handlerPinTriggerAdc.pGPIOx 							= GPIOA;
	handlerPinTriggerAdc.GPIO_PinConfig.GPIO_PinNumber 	 	= PIN_11;
	handlerPinTriggerAdc.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_IN;
	handlerPinTriggerAdc.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;

	//Se carga la configuración
	GPIO_Config(&handlerPinTriggerAdc);

	//Se configura el BlinkyTimer
	handlerBlinkyTimer.ptrTIMx 					= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_100M_05ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 	= 500; //Update period= 100us*2500 = 250000us = 250ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerBlinkyTimer);

	//Se configura el USART 2
	handlerUsart2.ptrUSARTx					     = USART2;                	  //USART 2
	handlerUsart2.USART_Config.USART_mode 	     = USART_MODE_RXTX;       	  //Modo de Recepción y transmisión
	handlerUsart2.USART_Config.USART_baudrate    = USART_BAUDRATE_57600; 	  //115200 bps
	handlerUsart2.USART_Config.USART_parity      = USART_PARITY_EVEN;         //Parity:NONE, acá viene configurado el tamaño de dato
	handlerUsart2.USART_Config.USART_stopbits    = USART_STOPBIT_1;	          //Un stopbit
	handlerUsart2.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE; //Interrupción de recepción del usart habilitada

	//Se carga la configuración del USART
	USART_Config(&handlerUsart2);

	//Se configura la conversión ADC
	adcConfig.channels          = channels;
	adcConfig.numberOfChannels  = ADC_NUMBER_OF_CHANNELS_4;
	adcConfig.dataAlignment		= ADC_ALIGNMENT_RIGHT;
	adcConfig.resolution		= ADC_RESOLUTION_12_BIT;
	adcConfig.samplingPeriod	= ADC_SAMPLING_PERIOD_480_CYCLES;

	//Se carga la configuración, así la interrupción se activa por defecto
	adc_Config(&adcConfig);

	//Se configura el evento externo
	adcConfigEvent.extEventTrigger    = ADC_EXT_TRIG_FALLING_EDGE;
	adcConfigEvent.extEventTypeSelect = ADC_EXT_EVENT_TIM5_CC3;

	//Se carga la configuración
	adcConfigExternal(&adcConfigEvent);

	//Se configura el Timer del PWM
	handlerPWMTimer.ptrTIMx 		  = TIM5;
	handlerPWMTimer.config.channel 	  = PWM_CHANNEL_3;
	handlerPWMTimer.config.prescaler  = BTIMER_SPEED_100M_05ms;
	handlerPWMTimer.config.periodo 	  = 2000;
	handlerPWMTimer.config.duttyCicle = 500;

	pwm_Config(&handlerPWMTimer);

}

//Función Callback del BlinkyTimer
void BasicTimer2_Callback(void) {
	//Blinky del LED de estado
	GPIOxTooglePin(&handlerBlinkyPin);
}

/*Función Callback de la recepción del USART2
El puerto es leído en la ISR para bajar la bandera de la interrupción
El carácter que se lee es devuelto por la función getRxData*/
void usart2Rx_Callback(void){
	//Leemos el valor del registro DR, donde se almacena el dato que llega.
	//Esto además debe bajar la bandera de la interrupción
	rxData = getRxData();
}

/*Esta función se ejecuta luego de una conversión ADC
 * (es llamada por la ISR de la conversión ADC)
 */
void adcComplete_Callback(void){
	adcIsComplete = true;
	i++;
}







