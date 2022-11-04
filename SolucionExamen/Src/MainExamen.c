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
#include "PwmDriver.h"
#include "DriverRTC.h"
#include "LCD2004A.h"

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
char Buffer[64]       = {0};//En esta variable se almacenarán mensajes
char* diaSemana       = {0};//En esta variable se almacena el día de la semana (arreglo, se almacena un string)
char greetingMsg[]    = "SIUU \n\r"; //Mensaje que se imprime

//Definición de la cabecera de las funciones que se crean para el desarrollo de los ejercicios
void initSystem(void);       				    //Función para inicializar el sistema
void updateRGB(uint8_t xAccel, uint8_t yAccel); //Función para actualizar el RGB
uint32_t absValue(int32_t);						//Función para obtener valor absoluto
void printHour(uint8_t hours, uint8_t minutes, uint8_t seconds);

int main(void) {

	initSystem();  //Se inicializa el sistema, con la configuración de los periféricos que se van a usar
	writeChar(&handlerUsart2,' ');
	displayDataLCD(&handlerI2CLCD, 'M');
	printStringLCD(&handlerI2CLCD, "hola mundo");
	printStringLCD(&handlerI2CLCD, "  Me llamo Laura");
	returnHomeLCD(&handlerI2CLCD);


	while (1) {

		/*if(accFlag == 1){
			accFlag = 0;
			accX = getXData(&handlerAccel);
			sprintf(Buffer, "\n\rACCx: %d\n", accX);
			writeMsg(&handlerUsart2, Buffer);

			accY = getYData(&handlerAccel);
			sprintf(Buffer, "ACCy: %d\n", accY);
			writeMsg(&handlerUsart2, Buffer);
		}

		if(updateRGBFlag == 1){
			updateRGBFlag = 0;
			updateRGB(accX, accY);
		}*/

		if(timeFlag == 1){
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
		}

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

			/*else if(rxData == 'x'){

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
			}*/

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
	GPIO_Config(&handlerSDAPinLCD);

	//Se configura el SCL del I2C de la LCD
	handlerSCLPinLCD.pGPIOx 							= GPIOB;
	handlerSCLPinLCD.GPIO_PinConfig.GPIO_PinNumber 	    = PIN_6;
	handlerSCLPinLCD.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;    //Función alternativa
	handlerSCLPinLCD.GPIO_PinConfig.GPIO_PinOPType 	    = GPIO_OTYPE_OPENDRAIN;
	handlerSCLPinLCD.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerSCLPinLCD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerSCLPinLCD.GPIO_PinConfig.GPIO_PinAltFunMode  = AF4;	              //AF04: I2C1 SCL

	//Se carga la configuración
	GPIO_Config(&handlerSCLPinLCD);

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
	//i2c_config(&handlerI2CAccel);

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
	//init_ADXL345(&handlerAccel);

	//Se configura el I2C de la LCD
	handlerI2CLCD.slaveAddress = LCD_ADDR;
	handlerI2CLCD.modeI2C 	   = I2C_MODE_FM;
	handlerI2CLCD.ptrI2Cx	   = I2C1;

	//Se carga la configuración
	i2c_config(&handlerI2CLCD);

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

	handlerHourDateConfig.PM_AM_Format = PM_FORMAT;
	handlerHourDateConfig.Hours        = 12; //Por defecto se pone la hora 00:00:00
	handlerHourDateConfig.Minutes      = 59;
	handlerHourDateConfig.Seconds      = 0;
	handlerHourDateConfig.Month        = 11;
	handlerHourDateConfig.DayOfWeek    = 4;
	handlerHourDateConfig.NumberOfDay  = 3;
	handlerHourDateConfig.Year		   = 22;

	enableRTC(&handlerHourDateConfig);

	initLCD(&handlerI2CLCD);

}

//Función Callback del BlinkyTimer
void BasicTimer2_Callback(void) {
	//Blinky del LED de estado
	GPIOxTooglePin(&handlerBlinkyPin);
	accFlag = 1;
	updateRGBFlag = 1;
	timeFlag = 1;
}

/*Función Callback de la recepción del USART2
El puerto es leído en la ISR para bajar la bandera de la interrupción
El carácter que se lee es devuelto por la función getRxData*/
void usart2Rx_Callback(void){
	//Leemos el valor del registro DR, donde se almacena el dato que llega.
	//Esto además debe bajar la bandera de la interrupción
	rxData = getRxData();
}

void updateRGB(uint8_t xAccel, uint8_t yAccel){

	uint8_t newDuttyR = 5*(absValue(xAccel)) + 150;
	uint8_t newDuttyB = 5*(absValue(yAccel)) + 150;
	uint8_t newDuttyG = absValue(-5*(absValue(yAccel)) + 150);

	updateDuttyCycle(&handlerPWMTimerR, newDuttyR);
	updateDuttyCycle(&handlerPWMTimerG, newDuttyG);
	updateDuttyCycle(&handlerPWMTimerB, newDuttyB);
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







