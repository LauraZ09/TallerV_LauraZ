/*
 * USARTxDriver.c
 *
 *  Created on: Apr 6, 2022
 *      Author: namontoy
 */

#include <stm32f4xx.h>
#include "USARTxDriver.h"

/**
 * Configurando el puerto Serial...
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periferico que se está utilizando.
 */
void USART_Config(USART_Handler_t *ptrUsartHandler){
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

	// 2.2 Configuracion del Parity: PREGUNTA, ACÁ SE DEBE PONER QUE AUTOMÁTICAMENTE SE LLENE EL TAMAÑO DE DATO?
	// Verificamos si el parity está activado o no
    // Tenga cuidado, el parity hace parte del tamaño de los datos...
	if(ptrUsartHandler->USART_Config.USART_parity != USART_PARITY_NONE){

		// Verificamos si se ha seleccionado ODD or EVEN
		if(ptrUsartHandler->USART_Config.USART_parity == USART_PARITY_EVEN){
			
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE; //Enable
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PS;  //Parity Selection: 1 Even.

		} else{
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE; //Enable
			ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_PS; //Parity Selection: 0 Odd.
		}

	} else{
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_PCE; //disable
	}

	// 2.3 Configuramos el tamaño del dato

	//Para 8 Bits:
	if(ptrUsartHandler->USART_Config.USART_datasize == USART_DATASIZE_8BIT){
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_M; //Para 8 bits se pone el bit M en 0
		}

	//Para 9 Bits (con bit de paridad):
	else if (ptrUsartHandler->USART_Config.USART_datasize == USART_DATASIZE_9BIT){
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_M; //Para 9 bits se pone el bit M en 1
		}

	else {
		__NOP();
	}

	// 2.4 Configuramos los stop bits (SFR USART_CR2) //PREGUNTAR LO DEL OR CUANDO HAY 0 Y 1
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
		// Valor a cargar 0x341
		// Configurando el Baudrate generator para una velocidad de 19200bps
		ptrUsartHandler->ptrUSARTx->BRR = 0x0341;
	}

	else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
		// El valor a cargar es 8.6875 -> Mantiza = 8,fraction = 0.6875
		// Mantiza = 8 = 0x8, fraction = 16 * 0.6875 = 11
		// Valor a cargar 0x811
		// Configurando el Baudrate generator para una velocidad de 115200bps
		ptrUsartHandler->ptrUSARTx->BRR = 0x0811;
	}

	// 2.6 Configuramos el modo: TX only, RX only, RXTX, disable
	switch(ptrUsartHandler->USART_Config.USART_mode){

	/*Bit 3 TE: Transmitter enable
This bit enables the transmitter. It is set and cleared by software.
0: Transmitter is disabled
1: Transmitter is enabled
Note: During transmission, a “0” pulse on the TE bit (“0” followed by “1”) sends a preamble
(idle line) after the current word, except in smartcard mode.
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
		// Desactivamos ambos canales PREGUNTAR SI ES NECESARIO DESACTIVAR LOS DOS CANALES O ES SUFICIENTE CON LA USART
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;  //0: USART prescaler and outputs disabled
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;  //0: Transmitter is disabled
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;  //0: Receiver is disabled
		break;
	}
	
	default:
	{
		// Actuando por defecto, desactivamos ambos canales EL USART TAMBIÉN?
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;  //0: Receiver is disabled
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;  //0: Transmitter is disabled

		break;
	}

	}

	// 2.7 Activamos el modulo serial. ACÁ ES EDITAR SOLO EL 13 O QUÉ PEDO
	if(ptrUsartHandler->USART_Config.USART_mode != USART_MODE_DISABLE){
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_UE;   //1: USART enabled
	}

	else {
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;  //0: USART prescaler and outputs disabled
	}
}

/* función para escribir un solo char */
int writeChar(USART_Handler_t *ptrUsartHandler, int dataToSend ){
	while( !(ptrUsartHandler->ptrUSARTx->SR & USART_SR_TXE)){
		__NOP();
	}

	ptrUsartHandler->ptrUSARTx->DR |= dataToSend;

	return dataToSend;
}
