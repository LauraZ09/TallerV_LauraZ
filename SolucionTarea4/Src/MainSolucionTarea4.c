/**
 *********************************************************************************************************
 *********************************************************************************************************
 * @file           : MainSolucionTarea4.c
 * @author         : Laura Alejandra Zuluaga Moreno - lazuluagamo@unal.edu.co
 * @brief          : Solución Tarea 4
 *
 *********************************************************************************************************
 */

/* DESARROLLO:
 * 1. LED de Estado: Para la implementación del LED de estado, se hace un Toogle en el Callback
 * del Timer2, este timer tiene un período de update de 250 ms.
 *
 * 2. Interrupción del EXTI para el PinClock: Se conectó la señal Clock del encoder al GPIOC11,
 * y se llama la función callback_extINT11, en este callback se pone en 1 la bandera PinClockFlag.
 * Esta interrupción se usó con Flancos de Subida, la razón de esto es que las salidas del encoder
 * tienen resistencias de PullUp por dentro, esto quiere decir que cuando no haya ningún giro,
 * la señal de salida estará en 1 lógico, así, cuando haya un giro, la señal estará en 0 lógico y
 * volverá a subir a 1 lógico, el primer flanco de la señal es de bajada, sin embargo, el Schmitt
 * Trigger invierte esta señal, así, para que la interrupción lea ese primer flanco, se debe configurar
 * con flancos de subida.
 *
 * 3. Identificación de giro del encoder: Para la identificación del Giro del encoder, dentro del
 * ciclo while del main, se tiene un if. Si la bandera PinClockFlag == 1 (hay un giro en el encoder),
 * se lee el estado del PinData (pin conectado a la señal DT del encoder). Se debe tener en cuenta
 * que cuando el giro es CW el PinData y el PinClock están desfasados: en la señal no invertida esto
 * es que mientras PinClock esté en 1, PinData está en 0. Sin embargo, al invertir la señal,se tiene
 * que al girar CW, en el flanco de la interrupción PinClock está en 0 y PinData también. Así, en el
 * While se lee el estado del PinData y si PinData == 0, entonces el giro es CW, sino, es CCW.
 *
 * 4. Controlar los display de 7 segmentos:
 *  -Para almacenar los números: Para el control del display se creó la variable Counter_i, en esta
 *   variable se almacena el número de giros. Cuando el giro es CW se le suma 1 a esta variable, si
 *   el giro es  CCW se le resta 1, esto se hace en las estructuras if mencionadas anteriormente.
 *   -Para encender alternadamente ambos displays: En las interrupciones del Timer3 se tiene un
 *   Toogle para el pin que va a la base de cada transistor, así, cada que salta la interrupción
 *   (período de update de 10ms) la base de uno de los transistores va a GND y el otro va a VCC
 *   y se prenderá el display del transistor que va a tierra.
 *   -Para mostrar el número: En el ciclo while se lee el estado de estos pines que van a los
 *    transistores y hay un if adicional: si PinUnitsTransistor == 0 (pin que va a la base del
 *    transistor del display que presenta las unidades del número), entonces se muestra el número
 *    correspondiente a las unidades, sino se muestra el número que corresponde a las decenas.
 *    Para mostrar los números se creó la función displayNumber, a esta función le entra un número
 *    y este número entra en un switch case en el que están listados los números del 0 al 9 con sus
 *    correspondientes pines encendidos y apagados (para encender un segmento del display, este debe
 *    ir a GND). Esta función se ejecuta con dos variables: CounterTens que es el contador que
 *    almacena las decenas del número, el número de decenas se obtiene así: CounterTens = Counter_i/10.
 *    La otra variable es CounterUnits, en esta  variable se almacenan las unidades de Counter_i, el
 *    número de unidades se obtiene de la siguiente forma: CounterUnits = Counter_i - (Counter_i/10)*10.
 *    Esta forma de obtener las unidades y las decenas funciona al no tener la FPU activada.
 *
 *5. Imprimir un mensaje cada vez que la posición del encoder cambia. Para esto, se usa un buffer
 *   que se llena con el número de la variable y la dirección del giro, según sea el caso. Posteriormente
 *   se usa la función WriteMsg y se envía por el Usart2 el contenido del Buffer
 *
 *6. Imprimir un mensaje cualquiera, cada vez que se presione el botón. Se conectó un botón externo al
 *   GPIOC6. Este Botón tiene una resistencia de PullUp externa y está conectado a un schmitt Trigger
 *   inversor, así, al MCU le está entrando la señal invertida del botón. Por ello, cuando se oprime
 *   el botón en la señal invertida habrá un flanco de subida. A este botón se le configuró una interrupción
 *   del EXTI con flancos de subida. Cuando salta la interrupción se sube una bandera y en el main,
 *   hay un bloque if, si la bandera del botón está subida, se envía un mensaje por el Usart2 con la
 *   función WriteMsg.
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
uint8_t TensTransistorState  = 0; //En esta variable se almacena la lectura del estado del TensTransistor

char Buffer[64]      = {0};       //En esta variable se almacenará el mensaje con el número y la dirección
char MessageToSend[] = "SIUUU \n";//Mensaje a enviar

//Definición de la cabecera de las funciones que se crean para el desarrollo de los ejercicios
void initSystem(void);               //Función para inicializar el sistema
void displayNumber(uint8_t counter); //Función para mostrar el número en el display

int main(void) {

	initSystem();  //Se inicializa el sistema, con la configuración de los periféricos que se van a usar

	while (1) {

		//Se guarda el estado de los pines de los transistores en variables:
		TensTransistorState  = GPIO_ReadPin(&handlerPinTensTransistor);
		UnitsTransistorState = GPIO_ReadPin(&handlerPinUnitsTransistor);

		//Con este if se cambia el display de 7 segmentos cada vez que se enciende la bandera del PinClock
		if (PinClockFlag == 1) {
			//Se Guarda el estado del PinData en la variable
			PinDataState = GPIO_ReadPin(&handlerPinData);

			//Cuando salta la interrupción y PinData == 0 el giro es horario, se pone Counter_i < 50 para
			//que el contador tenga un límite en 50
			if (( PinDataState == 0) & (Counter_i < 50)){
				Counter_i++;
				//Se envía el mensaje por USART:
				sprintf(Buffer,"El giro es CW %u \n",Counter_i);
				writeMsg(&handlerUsart2, Buffer);
			}

			//Cuando salta la interrupción y PinData == 1 el giro es antihorario, se pone Counter_i  > 0 para
			//que el contador tenga un límite en 50
			else if ((PinDataState == 1) & (Counter_i  > 0)) {
				Counter_i--;
				//Se envía el mensaje por USART:
				sprintf(Buffer, "El giro es CCW %u \n", Counter_i);
				writeMsg(&handlerUsart2, Buffer);
			}

			//Cuando el giro es CCW pero el contador ya llegó a 0, igual se sigue enviando el mensaje, pero
			//el número no cambia
			else if ((PinDataState == 1) & (Counter_i == 0)) {
				//Se envía el mensaje por USART:
				sprintf(Buffer, "El giro es CCW %u \n", Counter_i);
				writeMsg(&handlerUsart2, Buffer);
			}

			//Cuando el giro es CW pero el contador ya llegó a 50, igual se sigue enviando el mensaje, pero
			//el número no cambia
			else if ((PinDataState == 0) & (Counter_i == 50)) {
				//Se envía el mensaje por USART:
				sprintf(Buffer, "El giro es CW %u \n", Counter_i);
				writeMsg(&handlerUsart2, Buffer);
			}

			else {
				__NOP();
			}

			//Se baja la bandera para poder recibir la próxima interrupción
			PinClockFlag = 0;
		}

		//En caso de que la que se haya levantado la bandera ButtonFlag, esta se baja y se envía un mensaje
		if (ButtonFlag == 1) {
			writeMsg(&handlerUsart2, MessageToSend);
			ButtonFlag = 0;
		}
		else {
			__NOP();
		}

		//Acá se analiza el estado de los transistores para saber qué número mostrar
		if (TensTransistorState == 0) {
			CounterTens = Counter_i/10;
			displayNumber(CounterTens);
		}
		else if (UnitsTransistorState == 0) {
			CounterUnits = Counter_i - (Counter_i/10)*10;
			displayNumber(CounterUnits);
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

	//Se configura el Button: Se debe tener en cuenta que el modo entrada está configurado en el ExtiDriver
	handlerButton.pGPIOx 							 = GPIOA;
	handlerButton.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_6;
	handlerButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se configura el EXTI del botón
	ButtonExtiConfig.pGPIOHandler = &handlerButton;
	ButtonExtiConfig.edgeType 	  = EXTERNAL_INTERRUPT_RISING_EDGE;
	//Se carga la configuración: al cargar la configuración del exti, se carga también la del GPIO
	extInt_Config(&ButtonExtiConfig);

	//Se configura el PinClock: Se debe tener en cuenta que el modo entrada está configurado en el ExtiDriver
	handlerPinClock.pGPIOx  							= GPIOC;
	handlerPinClock.GPIO_PinConfig.GPIO_PinNumber 		= PIN_11;
	handlerPinClock.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;

	//Configurando el Exti
	PinClockExtiConfig.pGPIOHandler = &handlerPinClock;
	PinClockExtiConfig.edgeType 	= EXTERNAL_INTERRUPT_RISING_EDGE;
	//Se carga la configuración: al cargar la configuración del exti, se carga también la del GPIO
	extInt_Config(&PinClockExtiConfig);

	//Se configura el PinData
	handlerPinData.pGPIOx 							  = GPIOC;
	handlerPinData.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_10;
	handlerPinData.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_IN;
	handlerPinData.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinData);

	//Se configura el PinTensTransistor
	handlerPinTensTransistor.pGPIOx 				         	= GPIOC;
	handlerPinTensTransistor.GPIO_PinConfig.GPIO_PinNumber 	  	= PIN_2;
	handlerPinTensTransistor.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	handlerPinTensTransistor.GPIO_PinConfig.GPIO_PinOPType 	  	= GPIO_OTYPE_PUSHPULL;
	handlerPinTensTransistor.GPIO_PinConfig.GPIO_PinSpeed 	  	= GPIO_OSPEED_FAST;

	//Se carga la configuración
	GPIO_Config(&handlerPinTensTransistor);
	GPIO_WritePin(&handlerPinTensTransistor, SET);

	//Se configura el PinUnitsTransistor
	handlerPinUnitsTransistor.pGPIOx 				         	 = GPIOC;
	handlerPinUnitsTransistor.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_12;
	handlerPinUnitsTransistor.GPIO_PinConfig.GPIO_PinMode 		 = GPIO_MODE_OUT;
	handlerPinUnitsTransistor.GPIO_PinConfig.GPIO_PinOPType 	 = GPIO_OTYPE_PUSHPULL;
	handlerPinUnitsTransistor.GPIO_PinConfig.GPIO_PinSpeed 	  	 = GPIO_OSPEED_FAST;

	//Se carga la configuración
	GPIO_Config(&handlerPinUnitsTransistor);
	GPIO_WritePin(&handlerPinUnitsTransistor, RESET);

	//Se configura el PinSegmentA
	handlerPinSegmentA.pGPIOx 				         	  = GPIOA;
	handlerPinSegmentA.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_15;
	handlerPinSegmentA.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentA.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentA.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentA);
	GPIO_WritePin(&handlerPinSegmentA, SET);

	//Se configura el PinSegmentB
	handlerPinSegmentB.pGPIOx 				         	  = GPIOB;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_7;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;
	handlerPinSegmentB.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentB);
	GPIO_WritePin(&handlerPinSegmentB, RESET);


	//Se configura el PinSegmentC
	handlerPinSegmentC.pGPIOx 				         	  = GPIOC;
	handlerPinSegmentC.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_7;
	handlerPinSegmentC.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentC.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentC.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentC);
	GPIO_WritePin(&handlerPinSegmentC, SET);

	//Se configura el PinSegmentD
	handlerPinSegmentD.pGPIOx 				         	  = GPIOB;
	handlerPinSegmentD.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_4;
	handlerPinSegmentD.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentD.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentD.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentD);
	GPIO_WritePin(&handlerPinSegmentD, SET);

	//Se configura el PinSegmentE
	handlerPinSegmentE.pGPIOx 				         	  = GPIOB;
	handlerPinSegmentE.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_5;
	handlerPinSegmentE.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentE.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentE.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentE);
	GPIO_WritePin(&handlerPinSegmentE, SET);

	//Se configura el PinSegmentF
	handlerPinSegmentF.pGPIOx 				         	  = GPIOC;
	handlerPinSegmentF.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_3;
	handlerPinSegmentF.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentF.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentF.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;

	//Se carga la configuración
	GPIO_Config(&handlerPinSegmentF);
	GPIO_WritePin(&handlerPinSegmentF, SET);

	//Se configura el PinSegmentG
	handlerPinSegmentG.pGPIOx 				         	  = GPIOA;
	handlerPinSegmentG.GPIO_PinConfig.GPIO_PinNumber 	  = PIN_12;
	handlerPinSegmentG.GPIO_PinConfig.GPIO_PinMode 		  = GPIO_MODE_OUT;
	handlerPinSegmentG.GPIO_PinConfig.GPIO_PinOPType 	  = GPIO_OTYPE_PUSHPULL;
	handlerPinSegmentG.GPIO_PinConfig.GPIO_PinSpeed 	  = GPIO_OSPEED_FAST;

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
	handlerAuxTimer.TIMx_Config.TIMx_period = 100;    //Update period= 100us*100 = 10000us = 10ms

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
	//Blinky del LED de estado
	GPIOxTooglePin(&handlerBlinkyPin);
}

void BasicTimer3_Callback(void) {
//Se switchean los transistores, de forma que se muestre un número y luego el otro lo suficientemente rápido
	GPIOxTooglePin(&handlerPinUnitsTransistor);
	GPIOxTooglePin(&handlerPinTensTransistor);
	// Hacer esto así genera un efecto en el que se observa una pequeña sombra en fondo del display
}

/*Función Callback de la EXTI del PinClock: Esta interrupción está configurada en flanco de subida, así
  siempre que haya un flanco de subida en la señal del PinClock invertida, se sube la bandera*/
void callback_extInt11(void) {
//Se sube la bandera del PinClock a 1
	PinClockFlag = 1;
}

/*Función Callback de la EXTI del Button: Esta interrupción está configurada en flanco de subida, así
  siempre que haya un flanco de subida en la señal invertida del Botón (es decir, cuando este es presionado),
  se sube la bandera*/
void callback_extInt6(void) {
	//Se sube la bandera del Button a 1
	ButtonFlag = 1;
}

/*Función para mostrar los números, esta función recibe un entero de 8 bits y según el valor de este entero
 * enciende o apaga ciertos segmentos determinados
 */
void displayNumber(uint8_t counter){

	switch(counter) {

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



