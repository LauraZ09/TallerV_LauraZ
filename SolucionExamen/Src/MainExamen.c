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
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "ADXL345.h"
#include "I2CDriver.h"
#include "PwmDriver.h"
#include "DriverRTC.h"
#include "LCD2004A.h"
#include "string.h"

//Definición de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin          		= {0}; //Handler para el USER_LED
GPIO_Handler_t handlerTxPin              		= {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerRxPin              		= {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerSDAPin			 		= {0}; //Handler para el PIN DATA del I2C del acelerómetro
GPIO_Handler_t handlerSCLPin			 		= {0}; //Handler para el PIN CLOCK del I2C del acelerómetro
GPIO_Handler_t handlerSDAPinLCD					= {0}; //Handler para el PIN LCD del I2C de la LCD
GPIO_Handler_t handlerSCLPinLCD			 		= {0}; //Handler para el PIN CLOCK del I2C de la LCD
GPIO_Handler_t handlerBlueRGB            		= {0}; //Handler para el azul del RGB
GPIO_Handler_t handlerRedRGB            		= {0}; //Handler para el rojo del RGB
GPIO_Handler_t handlerGreenRGB           		= {0}; //Handler para el verde del RGB
USART_Handler_t handlerUsart2            		= {0}; //Handler para el USART2
PWM_Handler_t handlerPWMTimerB 	         		= {0}; //Handler para el PWM (Timer)
PWM_Handler_t handlerPWMTimerR 	         		= {0}; //Handler para el PWM (Timer)
PWM_Handler_t handlerPWMTimerG 	         		= {0}; //Handler para el PWM (Timer)
ADXL345_Handler_t handlerAccel   		 		= {0}; //Handler para el acelerómetro
I2C_Handler_t handlerI2CAccel			 		= {0}; //Handler para el I2C del acelerómetro
I2C_Handler_t handlerI2CLCD					    = {0}; //Handler para el I2C de la LCD
BasicTimer_Handler_t handlerBlinkyTimer  		= {0}; //Handler para el TIMER2, con este se hará el Blinky
BasicTimer_Handler_t handlerRGBTimer 			= {0}; //Handler para el TIMER5, este actualiza el PWM del RGB
Hour_and_Date_Config_t handlerHourDateConfig	= {0}; //Handler para la configuración de la hora

//Definición de otras variables necesarias para el desarrollo de los ejercicios:
uint8_t timeFlag 	  = 0;  //Bandera para la actualización de la hora
uint8_t segundos      = 0;	//Variable para almacenar los segundos
uint8_t minutos       = 0;  //Variable para almacenar los minutos
uint8_t horas		  = 0;  //Variable para almacenar las horas
uint8_t mes			  = 0;  //Variable en la que se almacena el mes
uint8_t year		  = 0;  //Variable en la que se almacena el año
uint8_t fecha		  = 0;  //Variable en la que se almacena la fecha
uint8_t updateRGBFlag = 0;	//Bandera para actualizar el PWM del LED RGB
uint8_t rxData        = 0;  //Datos de recepción
int16_t accX	      = 0;  //Variable para almacenar la aceleración en X
int16_t accY	      = 0;	//Variable para almacenar la aceleración en Y
uint8_t accFlag	      = 0;	//Bandera para actualizar los datos de la aceleración
uint8_t counterReception = 0;
uint8_t partyModeFlag = 0;
uint8_t accelModeFlag = 0;
uint8_t autodestructionModeFlag = 0;

bool stringComplete = false;
char Buffer[64]       = {0};//En esta variable se almacenarán mensajes
char bufferReception[64] = {0};
char* diaSemana       = {0};//En esta variable se almacena el día de la semana (arreglo, se almacena un string)
char greetingMsg[]    = "SIUU \n\r"; //Mensaje que se imprime
char userMsg[64] = {0};
char cmd[16];
unsigned int firstParameter;
unsigned int secondParameter;
uint8_t rxDataFlag = 0;

//Definición de la cabecera de las funciones que se crean para el desarrollo de los ejercicios
void initSystem(void);       				    //Función para inicializar el sistema
void updateRGB(uint8_t xAccel, uint8_t yAccel); //Función para actualizar el RGB
uint32_t absValue(int32_t);						//Función para obtener valor absoluto
void printHour(uint8_t hours, uint8_t minutes, uint8_t seconds);
void parseCommands(char* ptrBufferReception);
void setRGBMode(void);

int main(void) {

	initSystem();  //Se inicializa el sistema, con la configuración de los periféricos que se van a usar
	writeChar(&handlerUsart2,' ');
	/*clearDisplayLCD(&handlerI2CLCD);
	delayms(100);
	printStringLCD(&handlerI2CLCD, "Hoy termino esto, si senor");
	delayms(1000);
	returnHomeLCD(&handlerI2CLCD);
	delayms(1000);
	clearDisplayLCD(&handlerI2CLCD);
	delayms(1000);
	printStringLCD(&handlerI2CLCD, " gonorrea");
	delayms(1000);
	printStringLCD(&handlerI2CLCD, " 123456");
	delayms(1000);
	printStringLCD(&handlerI2CLCD, " suerte people");
	delayms(1000);
	clearDisplayLCD(&handlerI2CLCD);
	delayms(1000);
	moveCursorToLCD(&handlerI2CLCD, 0x40);
	delayms(1000);
	printStringLCD(&handlerI2CLCD, "NEAAAAAAAaaa");
	delayms(1000);*/


	while (1) {

		/*if(accFlag == 1){
			accFlag = 0;
			accX = getXData(&handlerAccel);
			//sprintf(Buffer, "\n\rACCx: %d\n", accX);
			//writeMsg(&handlerUsart2, Buffer);

			accY = getYData(&handlerAccel);
			//sprintf(Buffer, "ACCy: %d\n", accY);
			//writeMsg(&handlerUsart2, Buffer);
		}

		if(updateRGBFlag == 1){
			updateRGBFlag = 0;
			updateRGB(accX, accY);
		}*/

		/*if(timeFlag == 1){
			timeFlag  = 0;
			segundos  = RTC_Get_Seconds();
			minutos   = RTC_Get_Minutes();
			horas     = RTC_Get_Hours();
			diaSemana = RTC_Get_WeekDay();
			fecha	  = RTC_Get_Date();
			year 	  = RTC_Get_Year();
			mes		  = RTC_Get_Month();
			printHour(horas, minutos, segundos);
			writeMsg(&handlerUsart2, Buffer);
			sprintf(Buffer, "%s", diaSemana);
			writeMsg(&handlerUsart2, Buffer);
			sprintf(Buffer, " Fecha:%d/%d/%d\n\r", fecha, mes, year);
			writeMsg(&handlerUsart2, Buffer);
		}*/

		//El sistema siempre está verificando si el valor de rxData ha cambiado
		//(lo cual sucede en la ISR de la recepción)
		//Si este valor dejó de ser '\0' significa que se recibió un carácter
		//por lo tanto entra en el bloque if para analizar que se recibió

		if (rxDataFlag == 1) {
			rxDataFlag = 0;
			rxData = getRxData();
			if (rxData != '\0') {
				for (uint8_t j; j <= 64; j++) {
					bufferReception[j] = 0;
				}
				bufferReception[counterReception] = rxData;
				counterReception++;

				if (rxData == '@') {
					stringComplete = true;
					bufferReception[counterReception] = '\0';
					counterReception = 0;
				}

				rxData = '\0';
			}

			if (stringComplete) {
				parseCommands(bufferReception);
				stringComplete = false;
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
	handlerSDAPin.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_9;
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


	//Se configura el SDA del I2C de la LCD
	handlerSDAPinLCD.pGPIOx 							= GPIOB;
	handlerSDAPinLCD.GPIO_PinConfig.GPIO_PinNumber 	    = PIN_9;
	handlerSDAPinLCD.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;    //Función alternativa
	handlerSDAPinLCD.GPIO_PinConfig.GPIO_PinOPType 	    = GPIO_OTYPE_OPENDRAIN;
	handlerSDAPinLCD.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerSDAPinLCD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerSDAPinLCD.GPIO_PinConfig.GPIO_PinAltFunMode  = AF4;	              //AF04: I2C1 SDA

	//Se carga la configuración
	//GPIO_Config(&handlerSDAPinLCD);

	//Se configura el SCL del I2C de la LCD
	handlerSCLPinLCD.pGPIOx 							= GPIOB;
	handlerSCLPinLCD.GPIO_PinConfig.GPIO_PinNumber 	    = PIN_6;
	handlerSCLPinLCD.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;    //Función alternativa
	handlerSCLPinLCD.GPIO_PinConfig.GPIO_PinOPType 	    = GPIO_OTYPE_OPENDRAIN;
	handlerSCLPinLCD.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerSCLPinLCD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerSCLPinLCD.GPIO_PinConfig.GPIO_PinAltFunMode  = AF4;	              //AF04: I2C1 SCL

	//Se carga la configuración
	//GPIO_Config(&handlerSCLPinLCD);

	//Se configura el BlinkyTimer
	handlerBlinkyTimer.ptrTIMx 					= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 	= 2500; //Update period= 100us*2500 = 250000us = 250ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerBlinkyTimer);

	//Se configura el RGBTimer
	handlerRGBTimer.ptrTIMx 				= TIM5;
	handlerRGBTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerRGBTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_1ms;
	handlerRGBTimer.TIMx_Config.TIMx_period = 100; //Update period= 1ms*100 = 100 ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerRGBTimer);

	//Se configura el USART 2
	handlerUsart2.ptrUSARTx					     = USART2;                	  //USART 2
	handlerUsart2.USART_Config.USART_mode 	     = USART_MODE_RXTX;       	  //Modo de Recepción y transmisión
	handlerUsart2.USART_Config.USART_baudrate    = USART_BAUDRATE_115200; 	  //115200 bps
	handlerUsart2.USART_Config.USART_parity      = USART_PARITY_EVEN;         //Parity:NONE, acá viene configurado el tamaño de dato
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
	handlerAccel.ADXL345_Config.resolution			= TEN_BITS_RES_MODE;
	handlerAccel.ADXL345_Config.justifyDataFormat	= RIGHT_WSING_JUSTIFY_MODE;
	handlerAccel.ADXL345_Config.rangeOp				= TWOG_RANGE;

	//Se carga la configuración
	init_ADXL345(&handlerAccel);

	//Se configura el I2C de la LCD
	handlerI2CLCD.slaveAddress = LCD_ADDR;
	handlerI2CLCD.modeI2C 	   = I2C_MODE_FM;
	handlerI2CLCD.ptrI2Cx	   = I2C1;

	//Se carga la configuración
	//i2c_config(&handlerI2CLCD);

	//Se configura el Timer del PWM Azul
	handlerPWMTimerB.ptrTIMx 		   = TIM3;
	handlerPWMTimerB.config.channel    = PWM_CHANNEL_1;
	handlerPWMTimerB.config.prescaler  = BTIMER_SPEED_1us;
	handlerPWMTimerB.config.periodo    = 255;
	handlerPWMTimerB.config.duttyCicle = 50;

	pwm_Config(&handlerPWMTimerB);

	//Se configura el Timer del PWM Rojo
	handlerPWMTimerR.ptrTIMx 			= TIM3;
	handlerPWMTimerR.config.channel 	= PWM_CHANNEL_2;
	handlerPWMTimerR.config.prescaler   = BTIMER_SPEED_1us;
	handlerPWMTimerR.config.periodo 	= 255;
	handlerPWMTimerR.config.duttyCicle  = 10;

	pwm_Config(&handlerPWMTimerR);

	//Se configura el Timer del PWM Verde
	handlerPWMTimerG.ptrTIMx 			= TIM3;
	handlerPWMTimerG.config.channel 	= PWM_CHANNEL_3;
	handlerPWMTimerG.config.prescaler   = BTIMER_SPEED_1us;
	handlerPWMTimerG.config.periodo 	= 255;
	handlerPWMTimerG.config.duttyCicle  = 10;

	pwm_Config(&handlerPWMTimerG);

	//Se configura la salida del PWM Azul:
	handlerBlueRGB.pGPIOx 						      = GPIOB;
	handlerBlueRGB.GPIO_PinConfig.GPIO_PinNumber      = PIN_4;
	handlerBlueRGB.GPIO_PinConfig.GPIO_PinMode 	      = GPIO_MODE_ALTFN;
	handlerBlueRGB.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerBlueRGB.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerBlueRGB.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerBlueRGB.GPIO_PinConfig.GPIO_PinAltFunMode  = AF2; //AF02: TIM5_CH1;

	GPIO_Config(&handlerBlueRGB);

	//Se configura la salida del PWM Verde:
	handlerGreenRGB.pGPIOx 						      = GPIOB;
	handlerGreenRGB.GPIO_PinConfig.GPIO_PinNumber     = PIN_0;
	handlerGreenRGB.GPIO_PinConfig.GPIO_PinMode 	  = GPIO_MODE_ALTFN;
	handlerGreenRGB.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerGreenRGB.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerGreenRGB.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_NOTHING;
	handlerGreenRGB.GPIO_PinConfig.GPIO_PinAltFunMode = AF2; //AF02: TIM5_CH1;

	GPIO_Config(&handlerGreenRGB);

	//Se configura la salida del PWM Rojo:
	handlerRedRGB.pGPIOx 						      = GPIOB;
	handlerRedRGB.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
	handlerRedRGB.GPIO_PinConfig.GPIO_PinMode 	      = GPIO_MODE_ALTFN;
	handlerRedRGB.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerRedRGB.GPIO_PinConfig.GPIO_PinSpeed 	      = GPIO_OSPEED_FAST;
	handlerRedRGB.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerRedRGB.GPIO_PinConfig.GPIO_PinAltFunMode   = AF2; //AF02: TIM5_CH1;

	GPIO_Config(&handlerRedRGB);

	enableOutput(&handlerPWMTimerG);
	enableOutput(&handlerPWMTimerR);
	enableOutput(&handlerPWMTimerB);

	startPwmSignal(&handlerPWMTimerG);
	startPwmSignal(&handlerPWMTimerR);
	startPwmSignal(&handlerPWMTimerB);

	handlerHourDateConfig.Hours        = 12; //Por defecto se pone la hora 00:00:00
	handlerHourDateConfig.Minutes      = 59;
	handlerHourDateConfig.Seconds      = 0;
	handlerHourDateConfig.Month        = 11;
	handlerHourDateConfig.DayOfWeek    = 4;
	handlerHourDateConfig.NumberOfDay  = 3;
	handlerHourDateConfig.Year		   = 22;

	enableRTC(&handlerHourDateConfig);

   //initLCD(&handlerI2CLCD);

}

//Función Callback del BlinkyTimer
void BasicTimer2_Callback(void) {
	//Blinky del LED de estado
	GPIOxTooglePin(&handlerBlinkyPin);
	accFlag = 1;
	updateRGBFlag++;
	timeFlag = 1;
}

void BasicTimer5_Callback(void){
	setRGBMode();
}

void setRGBMode(void){

	if (partyModeFlag == 1){

		uint8_t numeroG = rand () % 256;
		uint8_t numeroB = rand () % 256;
		uint8_t numeroR = rand () % 256;


		updateDuttyCycle(&handlerPWMTimerR, numeroR);
		updateDuttyCycle(&handlerPWMTimerG, numeroG);
		updateDuttyCycle(&handlerPWMTimerB, numeroB);
	}

	else if (accelModeFlag == 1){


		accX = getXData(&handlerAccel);
		accY = getYData(&handlerAccel);

		updateRGB(accX, accY);
	}

	else if (autodestructionModeFlag == 1){

			__NOP();
		}

	else {
		__NOP();
	}

}


/*Función Callback de la recepción del USART2
El puerto es leído en la ISR para bajar la bandera de la interrupción
El carácter que se lee es devuelto por la función getRxData*/
void usart2Rx_Callback(void){
	//Leemos el valor del registro DR, donde se almacena el dato que llega.
	//Esto además debe bajar la bandera de la interrupción
	rxDataFlag = 1;
}

void updateRGB(uint8_t xAccel, uint8_t yAccel){

	uint8_t newDuttyR = 5*(absValue(xAccel)) + 150;
	uint8_t newDuttyB = 5*(absValue(yAccel)) + 150;
	uint8_t newDuttyG = absValue(-5*(absValue(yAccel)) + 150);

	updateDuttyCycle(&handlerPWMTimerR, newDuttyR);
	updateDuttyCycle(&handlerPWMTimerG, newDuttyG);
	updateDuttyCycle(&handlerPWMTimerB, newDuttyB);
}

void randomRGB(){

	uint8_t greenDutty = getDuttyCycleValue(&handlerPWMTimerG);
	uint8_t blueDutty = getDuttyCycleValue(&handlerPWMTimerB);
	uint8_t redDutty = getDuttyCycleValue(&handlerPWMTimerR);

	updateDuttyCycle(&handlerPWMTimerR, absValue(greenDutty - 100) );
	updateDuttyCycle(&handlerPWMTimerG, blueDutty);
	updateDuttyCycle(&handlerPWMTimerB, absValue(redDutty - 200));
}

uint32_t absValue(int32_t negValue){

	uint32_t posValue;

	if( negValue < 0 ){
		posValue = negValue*(-1);
	}

	else {
		posValue = negValue;
	}

	return posValue;
}

void printHour(uint8_t hours, uint8_t minutes, uint8_t seconds){
	if((minutes/10 == 0) & (seconds/10 == 0)){
		sprintf(Buffer, "\n\rHora: %d:0%d:0%d\n\r", horas, minutos, segundos);
	}

	else if((minutes/10 == 0)){
		sprintf(Buffer, "\n\rHora: %d:0%d:%d\n\r", horas, minutos, segundos);
	}

	else if((seconds/10 == 0)){
		sprintf(Buffer, "\n\rHora: %d:%d:0%d\n\r", horas, minutos, segundos);
	}
	else {
		sprintf(Buffer, "\n\rHora: %d:%d:%d\n\r", horas, minutos, segundos);
	}
}

void parseCommands(char* ptrBufferReception){

	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);

	if (strcmp(cmd, "help") == 0){

		writeMsg(&handlerUsart2, "Menu de Comandos e instrucciones:\n\r");
		writeMsg(&handlerUsart2, "El sistema consiste de un LED RGB el cual se enciende de acuerdo al modo configurado por el usuario.\n Hay 3 modos configurables:\n1.Modo "
				"fiesta, en el cual el LED parpadea de diferentes colores aleatorios.\n2.Modo aceleracion, en el cual el LED cambia de color \n"
				"con el cambio en la aceleracion percibida por el sensor\n3.Modo autodestruccion en el cual el LED hace una cuenta regresiva y luego se apaga.\n"
				"IMPORTANTE: por defecto ninguno de estos modos está configurado, por defecto el LED se enciende en un color azul. Para desactivar alguno de los modos\n"
				"basta con activar otro modo.\n\r");
		writeMsg(&handlerUsart2, "COMANDOS:\n\r1).  help -- Este comando imprime este menu\n");
		writeMsg(&handlerUsart2, "2).  setHour #Horas #Minutos -- Este comando se utiliza para introducir la hora inicial (horas y minutos) en formato 24 horas\n");
		writeMsg(&handlerUsart2, "3).  setDate #day #month msg -- Este comando se utiliza para introducir la fecha inicial, en msg se debe escribir el dia de la semana que es (con minuscula todo)\n");
		writeMsg(&handlerUsart2, "4).  setYear #year -- Este comando se utiliza para introducir el year presente\n");
	    writeMsg(&handlerUsart2, "5).  setPartyMode -- Este comando enciende el LED en modo fiesta, para apagar el modo Fiesta, se debe activar alguno de los otros modos\n");
	    writeMsg(&handlerUsart2, "6).  setAccelMode -- Este comando enciende el LED en modo acelerómetro, es decir, el color del RGB cambia con el movimiento del acelerometro\n");
		writeMsg(&handlerUsart2, "7).  getHour -- Este comando devuelve la hora\n");
		writeMsg(&handlerUsart2, "8).  getDate -- Este comando devuelve la fecha\n");
		writeMsg(&handlerUsart2, "9).  getAcc -- Este comando devuelve la aceleracion en X y en Y\n");
		writeMsg(&handlerUsart2, "10). initLCD -- Este comando envia a la LCD un mensaje de prueba\n");
		writeMsg(&handlerUsart2, "11). initOLED -- Este comando envia a la OLED un mensaje de prueba\n");
		writeMsg(&handlerUsart2, "12). initAutodestruction -- Este comando pone el sistema en modo autodestruccion\n");
	}

	else if (strcmp(cmd, "getHour") == 0){
		writeMsg(&handlerUsart2, "CMD: getHour\n");
		segundos = RTC_Get_Seconds();
		minutos = RTC_Get_Minutes();
		horas = RTC_Get_Hours();
		printHour(horas, minutos, segundos);
		writeMsg(&handlerUsart2, Buffer);
	}

	else if (strcmp(cmd, "getDate") == 0){
		writeMsg(&handlerUsart2, "CMD: getDate\n");
		diaSemana = RTC_Get_WeekDay();
		fecha = RTC_Get_Date();
		year = RTC_Get_Year();
		mes = RTC_Get_Month();
		sprintf(Buffer, "%s", diaSemana);
		writeMsg(&handlerUsart2, Buffer);
		sprintf(Buffer, " Fecha:%d/%d/%d\n\r", fecha, mes, year);
		writeMsg(&handlerUsart2, Buffer);
	}

	else if (strcmp(cmd, "getAcc") == 0){
		writeMsg(&handlerUsart2, "CMD: getAcc\n");
		accX = getXData(&handlerAccel);
		sprintf(Buffer, "\n\rACCx: %d\n", accX);
		writeMsg(&handlerUsart2, Buffer);
		accY = getYData(&handlerAccel);
		sprintf(Buffer, "ACCy: %d\n\r", accY);
		writeMsg(&handlerUsart2, Buffer);
	}

	else if (strcmp(cmd, "initLCD") == 0){
		writeMsg(&handlerUsart2, "CMD: initLCD\n\r");
	}

	else if (strcmp(cmd, "initOLED") == 0){
		writeMsg(&handlerUsart2, "CMD: initOLED\n\r");
	}

	else if (strcmp(cmd, "setPartyMode") == 0){
		partyModeFlag = 1;
		accelModeFlag = 0;
		autodestructionModeFlag = 0;
		writeMsg(&handlerUsart2, "CMD: setPartyMode\n");
		writeMsg(&handlerUsart2, "Modo Fiesta On\n\r");
	}

	else if (strcmp(cmd, "setAccelMode") == 0){
		writeMsg(&handlerUsart2, "CMD: setAccelMode\n\r");
		accelModeFlag = 1;
		autodestructionModeFlag = 0;
		partyModeFlag = 0;
	}

	else if (strcmp(cmd, "initAutodestruction") == 0){
		autodestructionModeFlag = 1;
		partyModeFlag = 0;
		accelModeFlag = 0;
		writeMsg(&handlerUsart2, "CMD: initAutodestruction\n\r");
		writeMsg(&handlerUsart2, "5\n\r");
		updateRGB(30, 0);
		delayms(1000);
		writeMsg(&handlerUsart2, "4\n\r");
		updateRGB(30, 30);
		delayms(1000);
		writeMsg(&handlerUsart2, "3\n\r");
		updateRGB(0, 0);
		delayms(1000);
		writeMsg(&handlerUsart2, "2\n\r");
		updateRGB(15, 15);
		delayms(1000);
		writeMsg(&handlerUsart2, "1\n\r");
		updateRGB(15, -15);
		delayms(1000);
		writeMsg(&handlerUsart2, "0\n\r");
		updateRGB(23, 17);
		delayms(1000);
		writeMsg(&handlerUsart2, "------------------------\n\r");
		updateDuttyCycle(&handlerPWMTimerG, 0);
		updateDuttyCycle(&handlerPWMTimerR, 0);
		updateDuttyCycle(&handlerPWMTimerB, 0);
	}

	else if (strcmp(cmd, "setHour") == 0){
		writeMsg(&handlerUsart2, "CMD: setHour\n\r");
		handlerHourDateConfig.Hours   = firstParameter; //Por defecto se pone la hora 00:00:00
		handlerHourDateConfig.Minutes = secondParameter;
		handlerHourDateConfig.Seconds = 0;
		enableRTC(&handlerHourDateConfig);
		segundos = RTC_Get_Seconds();
		minutos = RTC_Get_Minutes();
		horas = RTC_Get_Hours();
		printHour(horas, minutos, segundos);
		writeMsg(&handlerUsart2, Buffer);

	}

	else if (strcmp(cmd, "setDate") == 0){
		writeMsg(&handlerUsart2, "CMD: setDate\n\r");
		handlerHourDateConfig.Month        = secondParameter;
		handlerHourDateConfig.NumberOfDay  = firstParameter;

		if(strcmp(userMsg, "lunes") == 0){
			handlerHourDateConfig.DayOfWeek = 1;
			enableRTC(&handlerHourDateConfig);
			diaSemana = RTC_Get_WeekDay();
		}

		else if(strcmp(userMsg, "martes") == 0){
			handlerHourDateConfig.DayOfWeek = 2;
			enableRTC(&handlerHourDateConfig);
			diaSemana = RTC_Get_WeekDay();
		}

		else if(strcmp(userMsg, "miercoles") == 0){
			handlerHourDateConfig.DayOfWeek = 3;
			enableRTC(&handlerHourDateConfig);
			diaSemana = RTC_Get_WeekDay();
		}

		else if(strcmp(userMsg, "jueves") == 0){
			handlerHourDateConfig.DayOfWeek = 4;
			enableRTC(&handlerHourDateConfig);
			diaSemana = RTC_Get_WeekDay();
		}

		else if(strcmp(userMsg, "viernes") == 0){
			handlerHourDateConfig.DayOfWeek = 5;
			enableRTC(&handlerHourDateConfig);
			diaSemana = RTC_Get_WeekDay();
		}

		else if(strcmp(userMsg, "sabado") == 0){
			handlerHourDateConfig.DayOfWeek = 6;
			enableRTC(&handlerHourDateConfig);
			diaSemana = RTC_Get_WeekDay();
		}

		else if(strcmp(userMsg, "domingo") == 0){
			handlerHourDateConfig.DayOfWeek = 7;
			enableRTC(&handlerHourDateConfig);
			diaSemana = RTC_Get_WeekDay();
		}

		else {
			writeMsg(&handlerUsart2, "ERROR\n\r");
		}

		fecha = RTC_Get_Date();
		mes = RTC_Get_Month();
		sprintf(Buffer, "%s", diaSemana);
		writeMsg(&handlerUsart2, Buffer);
		sprintf(Buffer, " Fecha:%d/%d\n\r", fecha, mes);
		writeMsg(&handlerUsart2, Buffer);

	}

	else if (strcmp(cmd, "setYear") == 0){
		writeMsg(&handlerUsart2, "CMD: setYear\n\r");
		sprintf(Buffer, "Year: %d\n\r", firstParameter);
		writeMsg(&handlerUsart2, Buffer);
	}

	else {
		writeMsg(&handlerUsart2, "ERROR\n\r");
	}
}







