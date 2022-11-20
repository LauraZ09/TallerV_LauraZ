/**
 *********************************************************************************************************
 *********************************************************************************************************
 * @file           : MainTarea6.c
 * @author         : Laura Alejandra Zuluaga Moreno - lazuluagamo@unal.edu.co
 * @brief          : Tarea 6
 *NOTA: Importar la librería PeripheralDrivers
 *********************************************************************************************************
 */
#include "stm32f4xx.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "string.h"
#include "math.h"


#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "PwmDriver.h"
#include "CaptureFrecDriver.h"

//Definición de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin          		= {0}; //Handler para el USER_LED
GPIO_Handler_t handlerTxPin              		= {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerRxPin              		= {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerPinCaptureFrec            = {0}; //Handler para el PIN por el cual se hará la captura de frecuencia
GPIO_Handler_t handlerPinPWMChannel             = {0}; //Handler para el PIN por el cual sale la señal PWM


USART_Handler_t handlerUsart2            		= {0}; //Handler para el USART2

BasicTimer_Handler_t handlerBlinkyTimer  		= {0}; //Handler para el TIMER2, con este se hará el Blinky

PWM_Handler_t handlerSignalPWM					= {0}; //Handler para el PWM

Capture_Handler_t handlerCaptureFrec 			= {0}; //Handler para la captura de frecuencia


uint8_t rxData        	 = 0;    //Datos de recepción
uint8_t rxDataFlag 		 = 0;	 //Bandera para la recepción de datos del usart2
uint32_t captureFreqFlag = 0;	 //Bandera para la captura de frecuencia
uint32_t captureData 	 = 0;
uint8_t captureCounter   = 0;
uint32_t timeStamp1		 = 0;
uint32_t timeStamp2		 = 0;
uint32_t rawPeriod       = 0;    //Variable para la captura de frecuencia
uint16_t duttyValue		 = 5000; //Valor del dutty

char Buffer[64]          = {0}; //En esta variable se almacenarán mensajes a enviar

//Definición de la cabecera de las funciones que se crean para el desarrollo de los ejercicios
void initSystem(void);       				    //Función para inicializar el sistema

int main(void) {

	initSystem();  //Se inicializa el sistema, con la configuración de los periféricos que se van a usar
	captureFreqInterruptModeEnable(&handlerCaptureFrec);

	while (1) {

		//el sistema está constantemente verificando si se levanta la bandeta de recepción del USART2
		if (rxDataFlag == 1) {

			rxDataFlag = 0;         //Se baja la bandera
			rxData = getRxData();	//Se guarda la información recibida en una variable auxiliar

			if (rxData != '\0') {   //Si el carácter recibido es \0
				writeChar(&handlerUsart2, rxData);

				if (rxData == 'd') {
					duttyValue -= 100;
					//Lanzamos un nuevo ciclo de adquisición
					updateDuttyCycle(&handlerSignalPWM, duttyValue);
					sprintf(Buffer, "dutty = %u \n", (unsigned int) duttyValue);
					writeMsg(&handlerUsart2, Buffer);
					rxData = '\0';
				}

				else if (rxData == 'u') {
					duttyValue += 100;
					//Lanzamos un nuevo ciclo de adquisición
					updateDuttyCycle(&handlerSignalPWM, duttyValue);
					sprintf(Buffer, "dutty = %u \n", (unsigned int) duttyValue);
					writeMsg(&handlerUsart2, Buffer);
					rxData = '\0';
				}
			}

		}

		if((captureFreqFlag == 1) && (captureCounter == 0)){

			captureFreqClearCNT(&handlerCaptureFrec);
			captureFreqFlag = 0;
			timeStamp1 = getTimeStamp();
			captureCounter = 1;
		}

		else if ((captureFreqFlag == 1) && (captureCounter == 1)){
			captureFreqFlag = 0;
			timeStamp2 = getTimeStamp();
			captureCounter = 0;

			rawPeriod = abs(timeStamp2 - timeStamp1);

			sprintf(Buffer, "rawPeriod = %u ms \n",(unsigned int) rawPeriod);
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

    //Se configura el PIN por el que sale la señal del PWM
	handlerPinPWMChannel.pGPIOx 					        = GPIOC;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinNumber 	    = PIN_7;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinMode 	    = GPIO_MODE_ALTFN;    //Función alternativa
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinOPType 	    = GPIO_OTYPE_PUSHPULL;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinSpeed 	    = GPIO_OSPEED_FAST;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF2;

	//Se carga la configuración
	GPIO_Config(&handlerPinPWMChannel);

	//Se configura el BlinkyTimer
	handlerBlinkyTimer.ptrTIMx 					= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 	= 250;

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerBlinkyTimer);

	//Se configura el USART 2
	handlerUsart2.ptrUSARTx					     = USART2;                	  //USART 2
	handlerUsart2.USART_Config.USART_mode 	     = USART_MODE_RXTX;       	  //Modo de Recepción y transmisión
	handlerUsart2.USART_Config.USART_baudrate    = USART_BAUDRATE_115200; 	  //57600 bps
	handlerUsart2.USART_Config.USART_parity      = USART_PARITY_ODD;         //Parity:EVEN, acá viene configurado el tamaño de dato
	handlerUsart2.USART_Config.USART_stopbits    = USART_STOPBIT_1;	          //Un stopbit
	handlerUsart2.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE; //Interrupción de recepción del usart habilitada

	//Se carga la configuración del USART
	USART_Config(&handlerUsart2);

	//Se Configura el TIMER del PWM
	handlerSignalPWM.ptrTIMx		     = TIM3;
	handlerSignalPWM.config.channel	     = PWM_CHANNEL_2;
	handlerSignalPWM.config.duttyCicle   = duttyValue;
	handlerSignalPWM.config.periodo      = 20000;
	handlerSignalPWM.config.prescaler	 = 15;

	pwm_Config(&handlerSignalPWM);
	enableOutput(&handlerSignalPWM);
	startPwmSignal(&handlerSignalPWM);

	//Se configura el pin para la captura de la señal
	handlerPinCaptureFrec.pGPIOx 					         = GPIOB;
	handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_8;
	handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinMode 	     = GPIO_MODE_ALTFN;
	handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinOPType 	 = GPIO_OTYPE_PUSHPULL;
	handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinSpeed 	     = GPIO_OSPEED_FAST;
	handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinAltFunMode  = AF2;

	//Se carga la configuración
	GPIO_Config(&handlerPinCaptureFrec);

	//Se configura el TIMER para la captura de frecuencia
	handlerCaptureFrec.ptrTIMx			        = TIM4;
	handlerCaptureFrec.config.channel	        = CAPTURE_CHANNEL_3;
	handlerCaptureFrec.config.edgeSignal        = CAPTURE_FALLING_EDGE;
	handlerCaptureFrec.config.prescalerCapture  = CAPTURE_PRESCALER_1_1;
	handlerCaptureFrec.config.timerSpeed		= BTIMER_SPEED_1ms;

	//Se carga la configuración
	capture_Config(&handlerCaptureFrec);

	//Se envía mensaje por el serial
	writeMsg(&handlerUsart2, "La funcion de iniciliazacion de sistema se ha aplicado correctamente\n\r");

}

//Función Callback del BlinkyTimer
void BasicTimer2_Callback(void) {
	//Blinky del LED de estado
	GPIOxTooglePin(&handlerBlinkyPin);
}

void CaptureFreqTimer4_Callback(void){
	captureFreqFlag = 1;
}

/*Función Callback de la recepción del USART6
El puerto es leído en la ISR para bajar la bandera de la interrupción
El carácter que se lee es devuelto por la función getRxData*/
void usart2Rx_Callback(void){
	//Activamos una bandera, dentro de la función main se lee el registro DR lo que baja la bandera de la interrupción
	rxDataFlag = 1;
}


