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
#include "ADXL345.h"
#include "I2CDriver.h"


//Definición de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin          = {0}; //Handler para el USER_LED
GPIO_Handler_t handlerTxPin              = {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerRxPin              = {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerSDAPin			 = {0}; //Handler para el PIN DATA del I2C del acelerómetro
GPIO_Handler_t handlerSCLPin			 = {0}; //Handler para el PIN CLOCK del I2C del acelerómetro
USART_Handler_t handlerUsart2            = {0}; //Handler para el USART2
BasicTimer_Handler_t handlerBlinkyTimer  = {0}; //Handler para el TIMER2, con este se hará el Blinky
ADXL345_Handler_t handlerAccel   		 = {0}; //Handler para el acelerómetro
I2C_Handler_t handlerI2CAccel			 = {0}; //Handler para el I2C del acelerómetro

//Definición de otras variables necesarias para el desarrollo de los ejercicios:
uint8_t rxData     = 0;      //Datos de recepción
int16_t xData	   = 0;
int16_t yData	   = 0;
int16_t zData	   = 0;
char Buffer[64]    = {0};    //En esta variable se almacenarán mensajes
char greetingMsg[] = "SIUU \n\r"; //Mensaje que se imprime

//Definición de la cabecera de las funciones que se crean para el desarrollo de los ejercicios
void initSystem(void);       //Función para inicializar el sistema

int main(void) {

	initSystem();  //Se inicializa el sistema, con la configuración de los periféricos que se van a usar
	writeChar(&handlerUsart2,' ');

	while (1) {
		//El sistema siempre está verificando si el valor de rxData ha cambiado
		//(lo cual sucede en la ISR de la recepción)
		//Si este valor dejó de ser '\0' significa que se recibió un carácter
		//por lo tanto entra en el bloque if para analizar que se recibió
		if(rxData != '\0'){
			//Se imprime el carácter recibido
			writeChar(&handlerUsart2, rxData);

			if(rxData == 'm'){
				//Se envía un mensaje
				writeMsg(&handlerUsart2,greetingMsg);
			}

			else if(rxData == 'x'){

				//Se leen los datos x del acelerómetro
				int8_t xDataL = i2c_readSingleRegister(&handlerI2CAccel, DATAX0_R);
				int8_t xDataH = i2c_readSingleRegister(&handlerI2CAccel, DATAX1_R);

				xData = (xDataH << 8  | xDataL);
				sprintf(Buffer, "\n\rACCx: %d\n", xData);
				writeMsg(&handlerUsart2, Buffer);
			}

			else if(rxData == 'y'){

				//Se leen los datos x del acelerómetro
				int8_t yDataL = i2c_readSingleRegister(&handlerI2CAccel, DATAY0_R);
				int8_t yDataH = i2c_readSingleRegister(&handlerI2CAccel, DATAY1_R);

				yData = (yDataH << 8 | yDataL);
				sprintf(Buffer, "\n\rACCy: %d\n", yData);
				writeMsg(&handlerUsart2, Buffer);
			}

			else if(rxData == 'z'){

				//Se leen los datos x del acelerómetro
				int8_t zDataL = i2c_readSingleRegister(&handlerI2CAccel, DATAZ0_R);
				int8_t zDataH = i2c_readSingleRegister(&handlerI2CAccel, DATAZ1_R);

				zData = (zDataH << 8 | zDataL);
				sprintf(Buffer, "\n\rACCz: %d\n", zData);
				writeMsg(&handlerUsart2, Buffer);
			}

			rxData = '\0';
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

	//Se configura el SDA del I2C del acelerómetro
	handlerSDAPin.pGPIOx 							 = GPIOB;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_7;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinMode 		 = GPIO_MODE_ALTFN;    //Función alternativa
	handlerSDAPin.GPIO_PinConfig.GPIO_PinOPType 	 = GPIO_OTYPE_OPENDRAIN;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinSpeed 		 = GPIO_OSPEED_FAST;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinAltFunMode  = AF4;	              //AF04: I2C1 SDA

	//Se carga la configuración
	GPIO_Config(&handlerSDAPin);

	//Se configura el SCL del I2C del acelerómetro
	handlerSCLPin.pGPIOx 							 = GPIOB;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_6;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinMode 		 = GPIO_MODE_ALTFN;    //Función alternativa
	handlerSCLPin.GPIO_PinConfig.GPIO_PinOPType 	 = GPIO_OTYPE_OPENDRAIN;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinSpeed 		 = GPIO_OSPEED_FAST;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinAltFunMode  = AF4;	              //AF04: I2C1 SCL

	//Se carga la configuración
	GPIO_Config(&handlerSCLPin);

	//Se configura el BlinkyTimer
	handlerBlinkyTimer.ptrTIMx 					= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 	= 2500; //Update period= 100us*2500 = 250000us = 250ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerBlinkyTimer);

	//Se configura el USART 2
	handlerUsart2.ptrUSARTx					     = USART2;                	  //USART 2
	handlerUsart2.USART_Config.USART_mode 	     = USART_MODE_RXTX;       	  //Modo de Recepción y transmisión
	handlerUsart2.USART_Config.USART_baudrate    = USART_BAUDRATE_115200; 	  //115200 bps
	handlerUsart2.USART_Config.USART_parity      = USART_PARITY_NONE;         //Parity:NONE, acá viene configurado el tamaño de dato
	handlerUsart2.USART_Config.USART_stopbits    = USART_STOPBIT_1;	          //Un stopbit
	handlerUsart2.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE; //Interrupción de recepción del usart habilitada

	//Se carga la configuración del USART
	USART_Config(&handlerUsart2);

	//Se configura el I2C del acelerómetro
	handlerI2CAccel.slaveAddress = DEVICE_ADDR;
	handlerI2CAccel.modeI2C 	 = I2C_MODE_SM;
	handlerI2CAccel.ptrI2Cx		 = I2C1;

	//Se carga la configuración
	i2c_config(&handlerI2CAccel);

	//Se configura el acelerómetro
	handlerAccel.ptrI2CHandler  					= &handlerI2CAccel;
	handlerAccel.ADXL345_Config.powerModeOp			= POWER_MODE_NORMAL_OP;
	handlerAccel.ADXL345_Config.outputDataRate 		= OUTPUT_DATA_RATE_100;
	handlerAccel.ADXL345_Config.sleepMode			= NORMAL_MODE_OP;
	handlerAccel.ADXL345_Config.measureMode         = MEASURE_MODE_OP;
	handlerAccel.ADXL345_Config.selfTest			= SELF_TEST_DISABLED;
	handlerAccel.ADXL345_Config.resolution			= FULL_RES_MODE;
	handlerAccel.ADXL345_Config.justifyDataFormat	= RIGHT_WSING_JUSTIFY_MODE;
	handlerAccel.ADXL345_Config.rangeOp				= FOURG_RANGE;

	//Se carga la configuración
	init_ADXL345(&handlerAccel);

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








