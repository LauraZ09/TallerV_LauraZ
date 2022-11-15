/*
 * USARTxDriver.c
 *
 *  Created on: Apr 6, 2022
 *      Author: namontoy
 */

#include <stm32f4xx.h>
#include "USARTxDriver.h"

uint8_t auxRxData = 0;

/**
 * Configurando el puerto Serial...
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periferico que se está utilizando.
 */
void USART_Config(USART_Handler_t *ptrUsartHandler){

	/* 0.Desactivamos las interrupciones primero*/
	__disable_irq();

	/* 1. Activamos la señal de reloj que viene desde el BUS al que pertenece el periferico */
	/* Lo debemos hacer para cada uno de las posibles opciones que tengamos (USART1, USART2, USART6) */

    /* 1.1 Configuramos el USART1 */
	if(ptrUsartHandler->ptrUSARTx == USART1){
		//Registro del RCC que nos activa la señal del reloj para el USART1
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	}
    /* 1.2 Configuramos el USART2 */
	else if(ptrUsartHandler->ptrUSARTx == USART2){
		//Registro del RCC que nos activa la señal del reloj para el USART2
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	}
    /* 1.3 Configuramos el USART6 */
	else if(ptrUsartHandler->ptrUSARTx == USART6){
		// Registro del RCC que nos activa la señal de reloj para el TIM4
		RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
	}
	else{
		__NOP();
	}

	/* 2. Configuramos el tamaño del dato, la paridad y los bit de parada */
	/* En el CR1 están parity (PCE y PS) y tamaño del dato (M) */
	/* Mientras que en CR2 están los stopbit (STOP)*/
	/* Configuración del Baudrate (registro BRR) */
	/* Configuramos el modo: only TX, only RX, o RXTX */
	/* Por ultimo activamos el modulo USART cuando todo esta correctamente configurado */

	// 2.1 Comienzo por limpiar los registros, para cargar la configuración desde cero
	ptrUsartHandler->ptrUSARTx->CR1 = 0;
	ptrUsartHandler->ptrUSARTx->CR2 = 0;

	// 2.2 Configuracion del Parity:
	// Verificamos si el parity está activado o no
    // Tenga cuidado, el parity hace parte del tamaño de los datos...
	if(ptrUsartHandler->USART_Config.USART_parity != USART_PARITY_NONE){

		// Verificamos si se ha seleccionado ODD or EVEN
		if(ptrUsartHandler->USART_Config.USART_parity == USART_PARITY_EVEN){
			
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE; //Enable
			ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_PS; //Parity Selection: 0 Even.
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_M;   //Tamaño de datos: Para 9 bits se pone el bit M del CR1 en 1

		}

		else{

			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE; //Enable
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PS;  //Parity Selection: 1 Odd.
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_M;   //Tamaño de datos: Para 9 bits se pone el bit M del CR1 en 1

		}
	}

	else{
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_PCE; //disable
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_M;   //Tamaño de datos:Para 8 bits se pone el bit M en 0
	}

	// 2.3 Configuramos el tamaño del dato: Ya se configuró en el parity

	//Para 8 Bits:
	//if(ptrUsartHandler->USART_Config.USART_datasize == USART_DATASIZE_8BIT){
		//ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_M; //Para 8 bits se pone el bit M en 0
		//}

	//Para 9 Bits (con bit de paridad):
	//else if (ptrUsartHandler->USART_Config.USART_datasize == USART_DATASIZE_9BIT){
		//ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_M; //Para 9 bits se pone el bit M en 1
		//}

	//else {
	//	__NOP();
	//}

	// 2.4 Configuramos los stop bits (SFR USART_CR2)
	switch(ptrUsartHandler->USART_Config.USART_stopbits){

	case USART_STOPBIT_1: {
		// Debemos cargar el valor 0b00 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 &= ~USART_CR2_STOP;
		break;
	}

	case USART_STOPBIT_0_5: {
		// Debemos cargar el valor 0b01 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP_0;
		break;
	}

	case USART_STOPBIT_2: {
		// Debemoscargar el valor 0b10 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP_1;
		break;
	}

	case USART_STOPBIT_1_5: {
		// Debemoscargar el valor 0b11 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP;
		break;
	}

	default: {
		// En el caso por defecto seleccionamos 1 bit de parada
		ptrUsartHandler->ptrUSARTx->CR2 &= ~USART_CR2_STOP;
		break;
	}

	}

	// 2.5 Configuracion del Baudrate (SFR USART_BRR)
	// Ver tabla de valores (Tabla 73), Frec = 16MHz, overr = 0;

	if(ptrUsartHandler->USART_Config.clock_freq == 16){

		if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
				// El valor a cargar es 104.1875 -> Mantiza = 104,fraction = 0.1875
				// Mantiza = 104 = 0x68, fraction = 16 * 0.1875 = 3
				// Valor a cargar 0x0683
				// Configurando el Baudrate generator para una velocidad de 9600bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x0683;
			}

			else if (ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200) {
				// El valor a cargar es 52.0625 -> Mantiza = 52,fraction = 0.0625
				// Mantiza = 52 = 0x34, fraction = 16 * 0.0625 = 1
				// Valor a cargar 0x0341
				// Configurando el Baudrate generator para una velocidad de 19200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x0341;
			}

			else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
				// El valor a cargar es 8.6875 -> Mantiza = 8,fraction = 0.6875
				// Mantiza = 8 = 0x8, fraction = 16 * 0.6875 = 11
				// Valor a cargar 0x8B
				// Configurando el Baudrate generator para una velocidad de 115200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x08B;
			}

			else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_57600){
				// El valor a cargar es 17.375 -> Mantiza = 17,fraction = 0.6875
				// Mantiza = 17 = 0x11, fraction = 16 * 0.375 = 6
				// Valor a cargar 0x116
				// Configurando el Baudrate generator para una velocidad de 115200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x116;
			}
	}

	else if (ptrUsartHandler->USART_Config.clock_freq == 100){

		if(ptrUsartHandler->ptrUSARTx == USART1){

		// 2.5 Configuracion del Baudrate (SFR USART_BRR)
			// Ver tabla de valores (Tabla 73), Frec = 100MHz, overr = 0;
			if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
				// El valor a cargar es 651.0625 -> Mantiza = 651,fraction = 0.0625
				// Mantiza = 651 = 0x28B, fraction = 16 * 0.041667 = 1
				// Valor a cargar 0x28B1
				// Configurando el Baudrate generator para una velocidad de 9600bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x28B1;
			}

			else if (ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200) {
				// El valor a cargar es 325.5000 -> Mantiza = 325,fraction = 0.5000
				// Mantiza = 325 = 0x145, fraction = 16 * 0.520833 = 8
				// Valor a cargar 0x1458
				// Configurando el Baudrate generator para una velocidad de 19200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x1458;
			}

			else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
				// El valor a cargar es 54.2500 -> Mantiza = 54,fraction = 0.2500
				// Mantiza = 54 = 0x36, fraction = 16 * 0.2500 = 4
				// Valor a cargar 0x364
				// Configurando el Baudrate generator para una velocidad de 115200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x364;
			}

			else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_57600){
				// El valor a cargar es 108.5000 -> Mantiza = 108,fraction = 0.5000
				// Mantiza = 108 = 0x6C, fraction = 16 * 0.5000 = 8
				// Valor a cargar 0x6C8
				// Configurando el Baudrate generator para una velocidad de 115200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x6C8;
			}
		}

		else if (ptrUsartHandler->ptrUSARTx == USART6) {

			// 2.5 Configuracion del Baudrate (SFR USART_BRR)
			// Ver tabla de valores (Tabla 73), Frec = 16MHz, overr = 0;
			if (ptrUsartHandler->USART_Config.USART_baudrate
					== USART_BAUDRATE_9600) {
				// El valor a cargar es 651.0625 -> Mantiza = 651,fraction = 0.0625
				// Mantiza = 651 = 0x28B, fraction = 16 * 0.041667 = 1
				// Valor a cargar 0x28B1
				// Configurando el Baudrate generator para una velocidad de 9600bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x28B1;
			}

			else if (ptrUsartHandler->USART_Config.USART_baudrate
					== USART_BAUDRATE_19200) {
				// El valor a cargar es 325.5000 -> Mantiza = 325,fraction = 0.5000
				// Mantiza = 325 = 0x145, fraction = 16 * 0.520833 = 8
				// Valor a cargar 0x1458
				// Configurando el Baudrate generator para una velocidad de 19200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x1458;
			}

			else if (ptrUsartHandler->USART_Config.USART_baudrate
					== USART_BAUDRATE_115200) {
				// El valor a cargar es 54.2500 -> Mantiza = 54,fraction = 0.2500
				// Mantiza = 54 = 0x36, fraction = 16 * 0.2500 = 4
				// Valor a cargar 0x364
				// Configurando el Baudrate generator para una velocidad de 115200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x364;
			}

			else if (ptrUsartHandler->USART_Config.USART_baudrate
					== USART_BAUDRATE_57600) {
				// El valor a cargar es 108.5000 -> Mantiza = 108,fraction = 0.5000
				// Mantiza = 108 = 0x6C, fraction = 16 * 0.5000 = 8
				// Valor a cargar 0x6C8
				// Configurando el Baudrate generator para una velocidad de 115200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x6C8;
			}
		}
		//El usart2 está conectado al APB1 el cual va a 50M
		else if (ptrUsartHandler->ptrUSARTx == USART2) {

			// 2.5 Configuracion del Baudrate (SFR USART_BRR)
			// Ver tabla de valores (Tabla 73), Frec = 50MHz, overr = 0;
			if (ptrUsartHandler->USART_Config.USART_baudrate
					== USART_BAUDRATE_9600) {
				// El valor a cargar es 325.5208 -> Mantiza = 325,fraction = 0.5208
				// Mantiza = 325 = 0x145, fraction = 16 * 0.5208 = 8
				// Valor a cargar 0x1458
				// Configurando el Baudrate generator para una velocidad de 9600bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x1458;
			}

			else if (ptrUsartHandler->USART_Config.USART_baudrate
					== USART_BAUDRATE_19200) {
				// El valor a cargar es 162.7604 -> Mantiza = 162,fraction = 0.7604
				// Mantiza = 162 = 0xAC, fraction = 16 * 0.7604 = C
				// Valor a cargar 0xA2C
				// Configurando el Baudrate generator para una velocidad de 19200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0xA2C;
			}

			else if (ptrUsartHandler->USART_Config.USART_baudrate
					== USART_BAUDRATE_115200) {
				// El valor a cargar es 27.1267 -> Mantiza = 27,fraction = 0.1267
				// Mantiza = 27 = 0x1B, fraction = 16 * 0.1267 = 2
				// Valor a cargar 0x1B2
				// Configurando el Baudrate generator para una velocidad de 115200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x1B2;
			}

			else if (ptrUsartHandler->USART_Config.USART_baudrate
					== USART_BAUDRATE_57600) {
				// El valor a cargar es 54.2534 -> Mantiza = 54,fraction = 0.2534
				// Mantiza = 54 = 0x36, fraction = 16 * 0.2534 = 4
				// Valor a cargar 0x6C8
				// Configurando el Baudrate generator para una velocidad de 115200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x364;
			}
		}




	}


	// 2.6 Configuramos el modo: TX only, RX only, RXTX, disable
	switch(ptrUsartHandler->USART_Config.USART_mode){

	/*Bit 3 TE: Transmitter enable. This bit enables the transmitter. It is set and cleared by software.
		0: Transmitter is disabled
		1: Transmitter is enabled
		When TE is set, there is a 1 bit-time delay before the transmission starts.

		Bit 2 RE: Receiver enable
		This bit enables the receiver. It is set and cleared by software.
		0: Receiver is disabled
		1: Receiver is enabled and begins searching for a start bit*/

	case USART_MODE_TX:
	{
		// Activamos la parte del sistema encargada de enviar (Transmitter enable)
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;  //1: Transmitter is enabled
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE; //0: Receiver is disabled
		break;
	}

	case USART_MODE_RX:
	{
		// Activamos la parte del sistema encargada de recibir (Receiver enable)
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;  //0: Transmitter is disabled
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;   //1: Receiver is enabled
		break;
	}

	case USART_MODE_RXTX:
	{
		// Activamos ambas partes, tanto transmisión como recepción
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;  //1: Transmitter is enabled
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;  //1: Receiver is enabled
		break;
	}

	case USART_MODE_DISABLE:
	{

		/*Bit 13 UE: USART enable
		When this bit is cleared, the USART prescalers and outputs are ped and the end of the
		current byte transfer in order to reduce power consumption. This bit is set and cleared by
		software.
		0: USART prescaler and outputs disabled
		1: USART enabled*/

		// Desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;  //0: USART prescaler and outputs disabled
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;  //0: Transmitter is disabled
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;  //0: Receiver is disabled
		break;
	}
	
	default:
	{
		// Actuando por defecto, desactivamos ambos canales EL USART TAMBIÉN?
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;  //0: USART prescaler and outputs disabled
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;  //0: Receiver is disabled
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;  //0: Transmitter is disabled

		break;
	}
	}

	//2.8 Verificamos la configuración de las interrupciones
	//2.8a Interrupción por recepción
	if(ptrUsartHandler->USART_Config.USART_enableIntRX == USART_RX_INTERRUPT_ENABLE){
		//Como está activda, debemos configurar la interrupción por recepción
		/*Debemos activar la interrupciónRX en la configuración del USART*/
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RXNEIE;

		/*Debemos matricular la interupción en el NVIC*/
		/*Lo debemos hacer para cada una de las posibles opciones que tengamos
		/ USART1, USART2, USART6*/
		if(ptrUsartHandler->ptrUSARTx == USART1){
			__NVIC_EnableIRQ(USART1_IRQn);
		}

		else if(ptrUsartHandler->ptrUSARTx == USART2){
			__NVIC_EnableIRQ(USART2_IRQn);
		}

		else if(ptrUsartHandler->ptrUSARTx == USART6){
			__NVIC_EnableIRQ(USART6_IRQn);
		}
	}
	else {
		__NOP();

	}
	// 2.7 Activamos el modulo serial.
	if(ptrUsartHandler->USART_Config.USART_mode != USART_MODE_DISABLE){
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_UE;   //1: USART enabled
	}
	else {
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;  //0: USART prescaler and outputs disabled
	}

	/* Volvemos a activar las interrupciones del sistema*/
	__enable_irq();
}


__attribute__((weak)) void usart2Rx_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}
__attribute__((weak)) void usart6Rx_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}
__attribute__((weak)) void usart1Rx_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}


/* Esta es la función a la que apunta el sistema en el vector de interrupciones.
 * Se debe utilizar usando exactamente el mismo nombre definido en el vector de interrupciones,
 * Al hacerlo correctamente, el sistema apunta a esta función y cuando la interrupción se lanza
 * el sistema inmediatamente salta a este lugar en la memoria*/

/* función para escribir un solo char */
int writeChar(USART_Handler_t *ptrUsartHandler, int dataToSend ){
	while( !(ptrUsartHandler->ptrUSARTx->SR & USART_SR_TXE)){
		__NOP();
	}

	ptrUsartHandler->ptrUSARTx->DR = dataToSend;

	return dataToSend;
}

/*Función para escribir un mensaje*/
void writeMsg(USART_Handler_t *ptrUsartHandler, char *msgToSend ){
	while(*msgToSend != '\0'){
		writeChar(ptrUsartHandler, *msgToSend);
		msgToSend++;
	}
}

/*Función para obtener el dato*/
uint8_t getRxData(void){
	return auxRxData;
}

/*Handler de la interrupción del USART.
 * Acá deben estar todas las interrupciones asociadas: TX, RX, PE,...
 */
void USART2_IRQHandler(void){
	//Evaluamos si la interrupción que se dio es por RX
	if(USART2->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART2->DR;
		usart2Rx_Callback();
	}
}

void USART6_IRQHandler(void){
	//Evaluamos si la interrupción que se dio es por RX
	if(USART6->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART6->DR;
		usart6Rx_Callback();
	}
}

void USART1_IRQHandler(void){
	//Evaluamos si la interrupción que se dio es por RX
	if(USART1->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART1->DR;
		usart1Rx_Callback();
	}
}

