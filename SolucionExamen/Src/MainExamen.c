/**
 *********************************************************************************************************
 *********************************************************************************************************
 * @file           : MainADC.c
 * @author         : Laura Alejandra Zuluaga Moreno - lazuluagamo@unal.edu.co
 * @brief          : ADC
 *
 *********************************************************************************************************
 */
//TODO: agregar OLED en todos los modos, verificar todo los modos, arreglar HSV

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
#include "SH1106OLED.h"

//Definición de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin          		= {0}; //Handler para el USER_LED
GPIO_Handler_t handlerTxPin              		= {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerRxPin              		= {0}; //Handler para el PIN por el cual se hará la transmisión
GPIO_Handler_t handlerSDAPin			 		= {0}; //Handler para el PIN DATA del I2C1 del acelerómetro y la OLED
GPIO_Handler_t handlerSCLPin			 		= {0}; //Handler para el PIN CLOCK del I2C1 del acelerómetro y la OLED
GPIO_Handler_t handlerSDAPinLCD					= {0}; //Handler para el PIN LCD del I2C de la LCD
GPIO_Handler_t handlerSCLPinLCD			 		= {0}; //Handler para el PIN CLOCK del I2C de la LCD
GPIO_Handler_t handlerBlueRGB            		= {0}; //Handler para el azul del RGB
GPIO_Handler_t handlerRedRGB            		= {0}; //Handler para el rojo del RGB
GPIO_Handler_t handlerGreenRGB           		= {0}; //Handler para el verde del RGB

USART_Handler_t handlerUsart6            		= {0}; //Handler para el USART6

PWM_Handler_t handlerPWMTimerB 	         		= {0}; //Handler para el PWM (Timer)
PWM_Handler_t handlerPWMTimerR 	         		= {0}; //Handler para el PWM (Timer)
PWM_Handler_t handlerPWMTimerG 	         		= {0}; //Handler para el PWM (Timer)

ADXL345_Handler_t handlerAccel   		 		= {0}; //Handler para el acelerómetro

I2C_Handler_t handlerI2CAccel			 		= {0}; //Handler para el I2C del acelerómetro
I2C_Handler_t handlerI2COLED 					= {0}; //Handler para el I2C de la OLED
I2C_Handler_t handlerI2CLCD					    = {0}; //Handler para el I2C de la LCD

BasicTimer_Handler_t handlerBlinkyTimer  		= {0}; //Handler para el TIMER2, con este se hará el Blinky
BasicTimer_Handler_t handlerRGBTimer 			= {0}; //Handler para el TIMER5, este actualiza el PWM del RGB

Hour_and_Date_Config_t handlerHourDateConfig	= {0}; //Handler para la configuración de la hora

//Definición de otras variables necesarias para el desarrollo de los ejercicios:
uint8_t segundos      = 0;	//Variable para almacenar los segundos
uint8_t minutos       = 0;  //Variable para almacenar los minutos
uint8_t horas		  = 0;  //Variable para almacenar las horas
uint8_t mes			  = 0;  //Variable en la que se almacena el mes
uint8_t year		  = 0;  //Variable en la que se almacena el año
uint8_t fecha		  = 0;  //Variable en la que se almacena la fecha

uint8_t timeFlag 	  			= 0;    //Bandera para la actualización de la hora
uint8_t updateRGBFlag 			= 0;	//Bandera para actualizar el PWM del LED RGB
uint8_t partyModeFlag 			= 0;	//Bandera para la activación del modo fiesta
uint8_t accelModeFlag 			= 0;	//Bandera para la activación del modo aceleración
uint16_t updateLCDFlag 			= 0;	//Bandera para la actualización de la LCD
uint8_t getHourFlag 		    = 0;	//Bandera para la activación del modo hora
uint8_t autodestructionModeFlag = 0;	//Bandera para la activación del modo autodestrucción
uint8_t rxDataFlag 				= 0;	//Bandera para la recepción de datos del usart6
uint8_t updatePartyOLED 		= 0;	//Bandera para la actualización del modo Fiesta en la OLED
bool stringComplete 			= false;//Bandera para la recepción de datos del usart6

uint8_t rxData        = 0;  //Datos de recepción

int16_t accX	      = 0;  //Variable para almacenar la aceleración en X
int16_t accY	      = 0;	//Variable para almacenar la aceleración en Y

uint8_t counterReception = 0; //Contador para la recepción de datos por el usart6


char Buffer[64]          = {0}; //En esta variable se almacenarán mensajes a enviar
char bufferReception[64] = {0}; //En esta variable se almacenan variables de recepción
char* diaSemana          = {0}; //En esta variable se almacena el día de la semana (arreglo, se almacena un string)
char userMsg[64]         = {0}; //En esta variable se almacenan mensajes ingresados por la terminal serial
char cmd[64]             = {0};	//En esta variable se almacenan los comandos ingresados por el usuario

unsigned int firstParameter  = 0; //En esta variable se almacena el número ingresado por la terminal serial
unsigned int secondParameter = 0; //En esta variable se almacena el segundo número ingresado por la terminal serial

//Definición de la cabecera de las funciones que se crean para el desarrollo de los ejercicios
void initSystem(void);       				    //Función para inicializar el sistema
void updateRGB(uint8_t xAccel, uint8_t yAccel); //Función para actualizar el RGB
uint32_t absValue(int32_t);						//Función para obtener valor absoluto
void parseCommands(char* ptrBufferReception);	//Función para evaluar los comandos que la consola recibe
void setRGBMode(void);							//función que elige el modo de encendido del LED RGB
void updateLCD(void);							//Función que actualiza la información de la pantalla LCD
void updateLCDAcc(void);						//Función que actualiza la información de la pantalla LCD en el modo Aceleración
void updatePartyOLEDFunction(void);				//Función que actualiza la pantalla OLED en el modo Party
void printHour(uint8_t hours, uint8_t minutes, uint8_t seconds); //Función que imprime la hora en un formato adecuado

int main(void) {

	initSystem();  //Se inicializa el sistema, con la configuración de los periféricos que se van a usar

	while (1) {

		//el sistema está constantemente verificando si se levanta la bandeta de recepción del USART6
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
					stringComplete = true;                    //Cuando se reconoce el final del comando, selevanta una bandera
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

		if(updateLCDFlag == 4){
			//Si el contador updateLCDFlag == 4, ha pasado un segundo y la bandera de gethour está levantada, se
			//actualiza la información de la pantalla LCD

			if (getHourFlag == 1) {

				updateLCD();
				updateLCDFlag = 0;
			}

			else if (accelModeFlag == 1) {
				//Si esta la bandera levantada del modo aceleración
				//Se obtiene la aceleración y se imprime
				updateLCDAcc();
				updateLCDFlag = 0;
			}
		}

		if(updatePartyOLED == 1){
			//Si está levantada la bandera del modo party en la OLED, se actualiza
			updatePartyOLEDFunction();
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
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_11;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;    //Función alternativa
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF8;	              //AF08: Usart6

	//Se carga la configuración
	GPIO_Config(&handlerTxPin);

	//Se configura el TxPin (PIN por el cual se hace la transmisión)
	//Este PIN se configura en la función alternativa AF08 que corresponde al USART6
	handlerRxPin.pGPIOx 							= GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_12;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;    //Función alternativa
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF8;	              //AF08: Usart6

	//Se carga la configuración
	GPIO_Config(&handlerRxPin);

	//Se configura el SDA del I2C del acelerómetro y la OLED
	handlerSDAPin.pGPIOx 							 = GPIOB;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_9;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinMode 		 = GPIO_MODE_ALTFN;    //Función alternativa
	handlerSDAPin.GPIO_PinConfig.GPIO_PinOPType 	 = GPIO_OTYPE_OPENDRAIN;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinSpeed 		 = GPIO_OSPEED_FAST;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerSDAPin.GPIO_PinConfig.GPIO_PinAltFunMode  = AF4;	              //AF04: I2C1 SDA

	//Se carga la configuración
	GPIO_Config(&handlerSDAPin);

	//Se configura el SCL del I2C del acelerómetro y la OLED
	handlerSCLPin.pGPIOx 							 = GPIOB;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_8;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinMode 		 = GPIO_MODE_ALTFN;    //Función alternativa
	handlerSCLPin.GPIO_PinConfig.GPIO_PinOPType 	 = GPIO_OTYPE_OPENDRAIN;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinSpeed 		 = GPIO_OSPEED_FAST;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerSCLPin.GPIO_PinConfig.GPIO_PinAltFunMode  = AF4;	              //AF04: I2C1 SCL

	//Se carga la configuración
	GPIO_Config(&handlerSCLPin);

	//Se configura el SDA del I2C de la LCD
	handlerSDAPinLCD.pGPIOx 							= GPIOA;
	handlerSDAPinLCD.GPIO_PinConfig.GPIO_PinNumber 	    = PIN_8;
	handlerSDAPinLCD.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;    //Función alternativa
	handlerSDAPinLCD.GPIO_PinConfig.GPIO_PinOPType 	    = GPIO_OTYPE_OPENDRAIN;
	handlerSDAPinLCD.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerSDAPinLCD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerSDAPinLCD.GPIO_PinConfig.GPIO_PinAltFunMode  = AF4;	              //AF04: I2C1 SDA

	//Se carga la configuración
	GPIO_Config(&handlerSDAPinLCD);

	//Se configura el SCL del I2C de la LCD
	handlerSCLPinLCD.pGPIOx 							= GPIOC;
	handlerSCLPinLCD.GPIO_PinConfig.GPIO_PinNumber 	    = PIN_9;
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

	//Se configura el RGBTimer
	handlerRGBTimer.ptrTIMx 				= TIM5;
	handlerRGBTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerRGBTimer.TIMx_Config.TIMx_speed 	= BTIMER_SPEED_1ms;
	handlerRGBTimer.TIMx_Config.TIMx_period = 100; //Update period= 1ms*10 = 10 ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerRGBTimer);

	//Se configura el USART 6
	handlerUsart6.ptrUSARTx					     = USART6;                	  //USART 2
	handlerUsart6.USART_Config.USART_mode 	     = USART_MODE_RXTX;       	  //Modo de Recepción y transmisión
	handlerUsart6.USART_Config.USART_baudrate    = USART_BAUDRATE_57600; 	  //57600 bps
	handlerUsart6.USART_Config.USART_parity      = USART_PARITY_EVEN;         //Parity:EVEN, acá viene configurado el tamaño de dato
	handlerUsart6.USART_Config.USART_stopbits    = USART_STOPBIT_1;	          //Un stopbit
	handlerUsart6.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE; //Interrupción de recepción del usart habilitada

	//Se carga la configuración del USART
	USART_Config(&handlerUsart6);

	//Se configura el I2C del acelerómetro
	handlerI2CAccel.slaveAddress = DEVICE_ADDR;
	handlerI2CAccel.modeI2C 	 = I2C_MODE_FM;
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
	handlerI2CLCD.modeI2C 	   = I2C_MODE_SM;
	handlerI2CLCD.ptrI2Cx	   = I2C3;

	//Se carga la configuración
    i2c_config(&handlerI2CLCD);

    //Se configura el HandlerI2C de la OLED
	handlerI2COLED.slaveAddress = OLED_ADD;
	handlerI2COLED.modeI2C 	    = I2C_MODE_FM; //Es necesario que sea en fastmode
	handlerI2COLED.ptrI2Cx	    = I2C1;

	//Se carga la configuración
	i2c_config(&handlerI2COLED);

	//Se configura el Timer del PWM Azul
	handlerPWMTimerB.ptrTIMx 		   = TIM3;
	handlerPWMTimerB.config.channel    = PWM_CHANNEL_1;
	handlerPWMTimerB.config.prescaler  = BTIMER_SPEED_1us;
	handlerPWMTimerB.config.periodo    = 255;
	handlerPWMTimerB.config.duttyCicle = 128;

	pwm_Config(&handlerPWMTimerB);

	//Se configura el Timer del PWM Rojo
	handlerPWMTimerR.ptrTIMx 			= TIM3;
	handlerPWMTimerR.config.channel 	= PWM_CHANNEL_2;
	handlerPWMTimerR.config.prescaler   = BTIMER_SPEED_1us;
	handlerPWMTimerR.config.periodo 	= 255;
	handlerPWMTimerR.config.duttyCicle  = 255;

	pwm_Config(&handlerPWMTimerR);

	//Se configura el Timer del PWM Verde
	handlerPWMTimerG.ptrTIMx 			= TIM3;
	handlerPWMTimerG.config.channel 	= PWM_CHANNEL_3;
	handlerPWMTimerG.config.prescaler   = BTIMER_SPEED_1us;
	handlerPWMTimerG.config.periodo 	= 255;
	handlerPWMTimerG.config.duttyCicle  = 0;

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

	//Se habilitan las salidas de los canales PWM
	enableOutput(&handlerPWMTimerG);
	enableOutput(&handlerPWMTimerR);
	enableOutput(&handlerPWMTimerB);

	//Se habilita la señal del PWM
	startPwmSignal(&handlerPWMTimerG);
	startPwmSignal(&handlerPWMTimerR);
	startPwmSignal(&handlerPWMTimerB);

	//configuración inicial del RTC
	handlerHourDateConfig.Hours        = 11; //Por defecto se pone la hora 00:00:00
	handlerHourDateConfig.Minutes      = 15;
	handlerHourDateConfig.Seconds      = 0;
	handlerHourDateConfig.Month        = 11;
	handlerHourDateConfig.DayOfWeek    = 4;
	handlerHourDateConfig.NumberOfDay  = 3;
	handlerHourDateConfig.Year		   = 22;

	//Se carga la configuración inicial del RTC
	enableRTC(&handlerHourDateConfig);

	//Se inicializan las pantallas
	initLCD(&handlerI2CLCD);
	initOLED(&handlerI2COLED);

}

//Función Callback del BlinkyTimer
void BasicTimer2_Callback(void) {
	//Blinky del LED de estado
	GPIOxTooglePin(&handlerBlinkyPin);
	updateLCDFlag++; //Contador para la actualización de la LCD (se actualiza cada segundo)
}

void BasicTimer5_Callback(void){
	setRGBMode();   //Cada 10ms se actualiza el LED RGB
}

void setRGBMode(void){

//Función de actualización del RGB

	if (partyModeFlag == 1){
		//Si se está en modo Party, se obtienen números aleatorios para el dutty RGB

		uint8_t numeroG = rand () % 256;
		uint8_t numeroB = rand () % 256;
		uint8_t numeroR = rand () % 256;

		//Se cargan los nuevos valores de dutty
		updateDuttyCycle(&handlerPWMTimerR, numeroR);
		updateDuttyCycle(&handlerPWMTimerG, numeroG);
		updateDuttyCycle(&handlerPWMTimerB, numeroB);
	}

	else if (accelModeFlag == 1){
		//Si se está en modo accel, se toman los datos del acelerómetro y se ingresan en la función updateRGB

		accX = getXData(&handlerAccel);
		accY = getYData(&handlerAccel);

		updateRGB(accX, accY);
	}

	else {

		__NOP();
	}

}

/*Función Callback de la recepción del USART2
El puerto es leído en la ISR para bajar la bandera de la interrupción
El carácter que se lee es devuelto por la función getRxData*/
void usart6Rx_Callback(void){
	//Activamos una bandera, dentro de la función main se lee el registro DR lo que baja la bandera de la interrupción
	rxDataFlag = 1;
}

void updateRGB(uint8_t xAccel, uint8_t yAccel){
	//Esta función actualiza el modo del LED RGB, según la posición del acelerómetro, para obtener colores
	//aproximados a los del espacio HSV se hace lo siguiente:

	uint8_t newDuttyR = 5*(absValue(xAccel)) + 150;
	uint8_t newDuttyB = 5*(absValue(yAccel)) + 150;
	uint8_t newDuttyG = absValue(-5*(absValue(yAccel)) + 150);

	updateDuttyCycle(&handlerPWMTimerR, newDuttyR);
	updateDuttyCycle(&handlerPWMTimerG, newDuttyG);
	updateDuttyCycle(&handlerPWMTimerB, newDuttyB);
}

uint32_t absValue(int32_t negValue){
	//Esta función obtiene el valor absoluto de un número

	uint32_t posValue;

	if( negValue < 0 ){
		//Si el número es negativo se multiplica por menos 1
		posValue = negValue*(-1);
	}

	else {
		posValue = negValue;
	}

	return posValue;
}

void printHour(uint8_t hours, uint8_t minutes, uint8_t seconds){
	//En esta función se imprime la hora de forma adecuada

	if((minutes/10 == 0) & (seconds/10 == 0)){
		//Si el número de minutos y segundos es menor que 10, se les pone un 0 adelante
		sprintf(Buffer, " \n\rHora:%d:0%d:0%d\n\r", horas, minutos, segundos);
	}

	else if((minutes/10 == 0)){
		//Si el número de minutos es menor que 10 se le pone un 0 adelante
		sprintf(Buffer, " \n\rHora:%d:0%d:%d\n\r", horas, minutos, segundos);
	}

	else if((seconds/10 == 0)){
		//Si el número de segundos es menor que 10 se le pone un 0 adelante
		sprintf(Buffer, " \n\rHora:%d:%d:0%d\n\r", horas, minutos, segundos);
	}
	else {
		//En otro caso se imprime los datos normales obtenidos de los registros del RTC
		sprintf(Buffer, " "
				" \n\rHora:%d:%d:%d\n\r", horas, minutos, segundos);
	}
}

void parseCommands(char* ptrBufferReception){
	//esta función lee lo obtenido por el puerto serial y toma decisiones en base a eso

	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);

	if (strcmp(cmd, "help") == 0){
		//Si el comando recibido es 0, primero se bajan las banderas de los modos:
		getHourFlag = 0;
		partyModeFlag = 0;
		accelModeFlag = 0;
		autodestructionModeFlag = 0;
		updatePartyOLED = 0;

		//Se limpia el display LCD y se rteorna el cursor al 0
		clearDisplayLCD(&handlerI2CLCD);
		returnHomeLCD(&handlerI2CLCD);

		//Se escribe el menú de comandos en la terminal serial
		writeMsg(&handlerUsart6, "Menu de Comandos e instrucciones:\n\r");
		writeMsg(&handlerUsart6, "El sistema consiste de un LED RGB el cual se enciende de acuerdo al modo configurado por el usuario.\n Hay 3 modos configurables:\n1.Modo "
				"fiesta, en el cual el LED parpadea de diferentes colores aleatorios.\n2.Modo aceleracion, en el cual el LED cambia de color \n"
				"con el cambio en la aceleracion percibida por el sensor\n3.Modo autodestruccion en el cual el LED hace una cuenta regresiva y luego se apaga.\n"
				"IMPORTANTE: por defecto ninguno de estos modos está configurado, por defecto el LED se enciende en un color rosado. Para desactivar alguno de los modos\n"
				"basta con activar otro modo.\n\r");
		writeMsg(&handlerUsart6, "COMANDOS:\n\r1).  help -- Este comando imprime este menu\n");
		writeMsg(&handlerUsart6, "2).  setHour #Horas #Minutos -- Este comando se utiliza para introducir la hora inicial (horas y minutos) en formato 24 horas\n");
		writeMsg(&handlerUsart6, "3).  setDate #day #month msg -- Este comando se utiliza para introducir la fecha inicial, en msg se debe escribir el dia de la semana que es (con minuscula todo)\n");
		writeMsg(&handlerUsart6, "4).  setYear #year -- Este comando se utiliza para introducir el year presente\n");
	    writeMsg(&handlerUsart6, "5).  setPartyMode -- Este comando enciende el LED en modo fiesta, para apagar el modo Fiesta, se debe activar alguno de los otros modos\n");
	    writeMsg(&handlerUsart6, "6).  setAccelMode -- Este comando enciende el LED en modo acelerómetro, es decir, el color del RGB cambia con el movimiento del acelerometro\n");
		writeMsg(&handlerUsart6, "7).  getHour -- Este comando devuelve la hora\n");
		writeMsg(&handlerUsart6, "8).  getDate -- Este comando devuelve la fecha\n");
		writeMsg(&handlerUsart6, "9). initLCD -- Este comando envia a la LCD un mensaje de prueba\n");
		writeMsg(&handlerUsart6, "10). initOLED -- Este comando envia a la OLED un mensaje de prueba\n");
		writeMsg(&handlerUsart6, "11). initAutodestruction -- Este comando pone el sistema en modo autodestruccion\n");

		//Se envía un mensaje a la LCD
		moveCursorToLCD(&handlerI2CLCD, 0x43);
		printStringLCD(&handlerI2CLCD, "INSTRUCCIONES");
	}

	else if (strcmp(cmd, "getHour") == 0){
		//Se bajan las banderas de los otros modos y se sube la del modo getHour
		partyModeFlag = 0;
		accelModeFlag = 0;
		updateLCDFlag = 0;
		autodestructionModeFlag = 0;
		getHourFlag = 1;
		updatePartyOLED = 0;

		//Se limpia la LCD y se mueve el cursor al origen
		clearDisplayLCD(&handlerI2CLCD);
		moveCursorToLCD(&handlerI2CLCD, 0x00);

		//Se escribe un mensaje por la terminal serial
		writeMsg(&handlerUsart6, "CMD: getHour\n");

		//Se obtienen los valores de la hora del RTC
		segundos = RTC_Get_Seconds();
		minutos = RTC_Get_Minutes();
		horas = RTC_Get_Hours();
		printHour(horas, minutos, segundos);

		//Se manda la hora por la terminal serial
		writeMsg(&handlerUsart6, Buffer);

		//Se imprime la hora en la LCD
		moveCursorToLCD(&handlerI2CLCD, 0x40);
		printStringLCD(&handlerI2CLCD, Buffer);
	}

	else if (strcmp(cmd, "getDate") == 0){
		//Se bajan las banderas de los otros modos
		getHourFlag = 0;
		partyModeFlag = 0;
		accelModeFlag = 0;
		updateLCDFlag = 0;
		autodestructionModeFlag = 0;
		updatePartyOLED = 0;

		//Se manda el mensaje por el serial
		writeMsg(&handlerUsart6, "CMD: getDate\n");

		//Se actualizan los valores de la fecha del RTC
		diaSemana = RTC_Get_WeekDay();
		fecha = RTC_Get_Date();
		year = RTC_Get_Year();
		mes = RTC_Get_Month();

		//Se imprime la fecha por el serial
		sprintf(Buffer, "%s", diaSemana);
		writeMsg(&handlerUsart6, Buffer);
		sprintf(Buffer, " Fecha:%d/%d/%d\n\r", fecha, mes, year);
		writeMsg(&handlerUsart6, Buffer);

		//Se limpia la LCD y se imprimen los datos de la fecha
		clearDisplayLCD(&handlerI2CLCD);
		moveCursorToLCD(&handlerI2CLCD, 0x40);
		printStringLCD(&handlerI2CLCD, diaSemana);
		moveCursorToLCD(&handlerI2CLCD, 0x17);
		printStringLCD(&handlerI2CLCD, Buffer);
	}

	else if (strcmp(cmd, "initLCD") == 0){
		//Se bajan las banderas de los otros modos
		getHourFlag = 0;
		partyModeFlag = 0;
		accelModeFlag = 0;
		updateLCDFlag = 0;
		autodestructionModeFlag = 0;
		updatePartyOLED = 0;

		//Se envía el comando por el serial
		writeMsg(&handlerUsart6, "CMD: initLCD\n\r");

		//Se imprime el mensaje de bienvenida de la LCD
		clearDisplayLCD(&handlerI2CLCD);
		moveCursorToLCD(&handlerI2CLCD, 0x40);
		printStringLCD(&handlerI2CLCD, "\n\r\n\rBIENVENIDO\n\r\n\r");

		//Se imprime mensaje en la OLED
		clearDisplayLCD(&handlerI2COLED);
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_2);
		setColumn(&handlerI2COLED, 0x10);
		printBytesArray(&handlerI2COLED, "LCD INICIALIZADA");
	}

	else if (strcmp(cmd, "initOLED") == 0){
		//Se bajan las banderas de los otros modos
		getHourFlag = 0;
		partyModeFlag = 0;
		accelModeFlag = 0;
		updateLCDFlag = 0;
		autodestructionModeFlag = 0;
		updatePartyOLED = 0;

		//Se envía el comando por el serial
		writeMsg(&handlerUsart6, "CMD: initOLED\n\r");

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


		//Se envía mensaje para la LCD
		clearDisplayLCD(&handlerI2CLCD);
		moveCursorToLCD(&handlerI2CLCD, 0x46);
		printStringLCD(&handlerI2CLCD, "InitOLED");
	}

	else if (strcmp(cmd, "setPartyMode") == 0){
		//Se bajan las banderas de los otros modos
		getHourFlag = 0;
		partyModeFlag = 1;
		accelModeFlag = 0;
		updateLCDFlag = 0;
		autodestructionModeFlag = 0;
		updatePartyOLED = 1;

		//Se imprime el comando en el terminal serial
		writeMsg(&handlerUsart6, "CMD: setPartyMode\n");
		writeMsg(&handlerUsart6, "Modo Fiesta On\n\r");

		//Se imprime en la LCD
		clearDisplayLCD(&handlerI2CLCD);
		moveCursorToLCD(&handlerI2CLCD, 0x43);
		printStringLCD(&handlerI2CLCD,"Modo Fiesta ON");
		}


	else if (strcmp(cmd, "setAccelMode") == 0){
		//Se apagan las banderas de los otros modos y se enciende el modo accelMode
		getHourFlag = 0;
		partyModeFlag = 0;
		accelModeFlag = 1;
		updateLCDFlag = 1; //Se pone en 1 esta bandera para estar actualizando constantemente los valores de acc en la LCD
		autodestructionModeFlag = 0;
		updatePartyOLED = 0;

		//Se envía el comando por el serial
		writeMsg(&handlerUsart6, "CMD: setAccelMode\n\r");

		//Se limpia la pantalla y nos movemos hacia donde se quiere imprimir la hora
		clearDisplayLCD(&handlerI2CLCD);
		returnHomeLCD(&handlerI2CLCD);

		//Se obtienen los valores de aceleración y se imprimen por el serial y en la LCD
		accX = getXData(&handlerAccel);
		sprintf(Buffer, "\n\rACCx: %d\n", accX);
		writeMsg(&handlerUsart6, Buffer);
		sprintf(Buffer, "ACCx: %d", accX);
		printStringLCD(&handlerI2CLCD, Buffer);

		moveCursorToLCD(&handlerI2CLCD, 0x40);
		accY = getYData(&handlerAccel);
		sprintf(Buffer, "ACCy: %d\n\r", accY);
		writeMsg(&handlerUsart6, Buffer);
		sprintf(Buffer, "ACCy: %d", accY);
		printStringLCD(&handlerI2CLCD, Buffer);
	}

	else if (strcmp(cmd, "initAutodestruction") == 0){
		//Se apagan las banderas de los otros modos y se enciende el modo autodestructionMode
		getHourFlag = 0;
		partyModeFlag = 0;
		accelModeFlag = 0;
		updateLCDFlag = 0;
		autodestructionModeFlag = 1;
		updatePartyOLED = 0;

		//Se limpia la LCD y la OLED
		clearDisplayLCD(&handlerI2CLCD);
		clearAllScreen(&handlerI2COLED);

		//Se envía mensaje por el serial y en la LCD
		writeMsg(&handlerUsart6, "CMD: initAutodestruction\n\r");

		//Cuenta regresiva
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		setColumn(&handlerI2COLED, 0x02);
		printBytesArray(&handlerI2COLED, "AUTODESTRUCTION");
		writeMsg(&handlerUsart6, "5\n\r");
		printStringLCD(&handlerI2CLCD, "5.....");
		updateRGB(30, 0); //El RGB cambia de color con la cuenta regresiva
		delayms(1000);


		writeMsg(&handlerUsart6, "4\n\r");
		printStringLCD(&handlerI2CLCD, "4.....");
		updateRGB(30, 30);
		delayms(1000);


		writeMsg(&handlerUsart6, "3\n\r");
		printStringLCD(&handlerI2CLCD, "3.....");
		updateRGB(0, 0);
		delayms(1000);


		writeMsg(&handlerUsart6, "2\n\r");
		printStringLCD(&handlerI2CLCD, "2.....");
		updateRGB(15, 15);
		delayms(1000);


		writeMsg(&handlerUsart6, "1\n\r");
		printStringLCD(&handlerI2CLCD, "1.....");
		updateRGB(15, -15);
		delayms(1000);


		writeMsg(&handlerUsart6, "0\n\r");
		printStringLCD(&handlerI2CLCD, "0.....");
		updateRGB(23, 17);
		delayms(1000);

		writeMsg(&handlerUsart6, "------------------------\n\r");


		clearDisplayLCD(&handlerI2CLCD);
		returnHomeLCD(&handlerI2CLCD);
		printStringLCD(&handlerI2CLCD, "BYE BYE BYE BYE BYE ");
		moveCursorToLCD(&handlerI2CLCD, 0x40);
		printStringLCD(&handlerI2CLCD, "BYE BYE BYE BYE BYE ");
		moveCursorToLCD(&handlerI2CLCD, 0x14);
		printStringLCD(&handlerI2CLCD, "BYE BYE BYE BYE BYE ");
		moveCursorToLCD(&handlerI2CLCD, 0x54);
		printStringLCD(&handlerI2CLCD, "BYE BYE BYE BYE BYE ");

		clearAllScreen(&handlerI2COLED);
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_1);
		printBytesArray(&handlerI2COLED, "          BYEEE ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_3);
		printBytesArray(&handlerI2COLED, "BYEEE           ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_5);
		printBytesArray(&handlerI2COLED, "     BYEEE      ");
		setPageOLED(&handlerI2COLED, OLED_PAGE_NUMBER_7);
		printBytesArray(&handlerI2COLED,"           BYEEE ");

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


		//Se apaga el RGB
		updateDuttyCycle(&handlerPWMTimerG, 0);
		updateDuttyCycle(&handlerPWMTimerR, 0);
		updateDuttyCycle(&handlerPWMTimerB, 0);
		sendCommandLCD(&handlerI2CLCD, 0x00);
	}

	else if (strcmp(cmd, "setHour") == 0){
		//Se apagan las banderas de los otros modos
		getHourFlag = 0;
		partyModeFlag = 0;
		accelModeFlag = 0;
		updateLCDFlag = 0;
		autodestructionModeFlag = 0;
		updatePartyOLED = 0;

		//Se envía el comando por el serial
		writeMsg(&handlerUsart6, "CMD: setHour\n\r");

		//Se llena la configuración del RTC con los parámetros recibidos por el serial
		handlerHourDateConfig.Hours   = firstParameter;
		handlerHourDateConfig.Minutes = secondParameter;
		handlerHourDateConfig.Seconds = 0;

		//Se activa el RTC para poder escribir en los registros la config ingresada
		enableRTC(&handlerHourDateConfig);

		//Se obtienen los valores de los registros
		segundos = RTC_Get_Seconds();
		minutos = RTC_Get_Minutes();
		horas = RTC_Get_Hours();
		printHour(horas, minutos, segundos);

		//Se imprime la hora ingresada por el serial
		writeMsg(&handlerUsart6, Buffer);

		//Se envía mensaje por la LCD
		clearDisplayLCD(&handlerI2CLCD);
		returnHomeLCD(&handlerI2CLCD);
		printStringLCD(&handlerI2CLCD, "Hora ingresada");
		moveCursorToLCD(&handlerI2CLCD, 0x41);
		printStringLCD(&handlerI2CLCD, Buffer);
	}

	else if (strcmp(cmd, "setDate") == 0){
		//Se apagan las banderas de los otros modos
		getHourFlag = 0;
		partyModeFlag = 0;
		accelModeFlag = 0;
		updateLCDFlag = 0;
		autodestructionModeFlag = 0;
		updatePartyOLED = 0;

		//Se envía el comando por el serial
		writeMsg(&handlerUsart6, "CMD: setDate\n\r");

		//Se llena la configuración del RTC con los parámetros ingresados
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
			writeMsg(&handlerUsart6, "ERROR\n\r");
		}

		//Se obtienen los valores de los registros
		fecha = RTC_Get_Date();
		mes = RTC_Get_Month();

		//Se envía la fecha por el serial
		sprintf(Buffer, "%s", diaSemana);
		writeMsg(&handlerUsart6, Buffer);
		sprintf(Buffer, " Fecha:%d/%d\n\r", fecha, mes);
		writeMsg(&handlerUsart6, Buffer);

		//Se envía la fecha por la LCD
		clearDisplayLCD(&handlerI2CLCD);
		returnHomeLCD(&handlerI2CLCD);
		printStringLCD(&handlerI2CLCD, diaSemana);
		moveCursorToLCD(&handlerI2CLCD, 0x43);
		printStringLCD(&handlerI2CLCD, Buffer);
	}

	else if (strcmp(cmd, "setYear") == 0){
		//Se apagan las banderas de los otros modos
		getHourFlag = 0;
		partyModeFlag = 0;
		accelModeFlag = 0;
		updateLCDFlag = 0;
		autodestructionModeFlag = 0;
		updatePartyOLED = 0;

		//Se envía el comando por el serial
		writeMsg(&handlerUsart6, "CMD: setYear\n\r");

		//Se llena la configuración del RTC con el año ingresado
		handlerHourDateConfig.Year  = firstParameter;
		enableRTC(&handlerHourDateConfig);

		//Se imprime el año ingresado en el serial
		sprintf(Buffer, "Year: %d\n\r", firstParameter);
		writeMsg(&handlerUsart6, Buffer);

		//Se obtiene el año de los registros RTC
		year = RTC_Get_Year();

		//Se imprime el año ingresado en la LCD
		clearDisplayLCD(&handlerI2CLCD);
		moveCursorToLCD(&handlerI2CLCD, 0x41);
		sprintf(Buffer, "Year ingresado:%d", year);
		printStringLCD(&handlerI2CLCD, Buffer);
	}

	else { //Si no se reconoce ningún comando

		//Se apagan todas las banderas
		getHourFlag = 0;
		partyModeFlag = 0;
		accelModeFlag = 0;
		updateLCDFlag = 0;
		autodestructionModeFlag = 0;
		updatePartyOLED = 0;

		//El LED parpadea rojo
		updateDuttyCycle(&handlerPWMTimerG, 0);
		updateDuttyCycle(&handlerPWMTimerB, 0);
		updateDuttyCycle(&handlerPWMTimerR, 190);

		//Se escribe el mensaje de error por el serial y en la LCD
		writeMsg(&handlerUsart6, "ERROR\n\r");

		clearDisplayLCD(&handlerI2CLCD);
		moveCursorToLCD(&handlerI2CLCD, 0x47);
		printStringLCD(&handlerI2CLCD, "ERROR!");

		delayms(1500);

		//El LED vuelve a su color rosado por default
		updateDuttyCycle(&handlerPWMTimerG, 0);
		updateDuttyCycle(&handlerPWMTimerB, 128);
		updateDuttyCycle(&handlerPWMTimerR, 255);
	}
}

void updateLCD(void){
		//returnHomeLCD(&handlerI2CLCD);
		//Se obtiene la hora de los registros
		horas = RTC_Get_Hours();
		minutos = RTC_Get_Minutes();
		segundos = RTC_Get_Seconds();
		printHour(horas, minutos, segundos);

		//Se imprime por el serial
		writeMsg(&handlerUsart6, Buffer);

		//Se imprime en la LCD
		clearDisplayLCD(&handlerI2CLCD);
		moveCursorToLCD(&handlerI2CLCD, 0x40);
		printStringLCD(&handlerI2CLCD, Buffer);

}

void updateLCDAcc(void){

	//Se obtiene la aceleración y se imprime
	clearDisplayLCD(&handlerI2CLCD);
	returnHomeLCD(&handlerI2CLCD);
	accX = getXData(&handlerAccel);
	sprintf(Buffer, "\n\rACCx: %d\n", accX);
	writeMsg(&handlerUsart6, Buffer);
	sprintf(Buffer, "ACCx: %d", accX);
	printStringLCD(&handlerI2CLCD, Buffer);

	accY = getYData(&handlerAccel);
	sprintf(Buffer, "ACCy: %d\n\r", accY);
	moveCursorToLCD(&handlerI2CLCD, 0x40);
	writeMsg(&handlerUsart6, Buffer);
	sprintf(Buffer, "ACCy: %d", accY);
	printStringLCD(&handlerI2CLCD, Buffer);
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






