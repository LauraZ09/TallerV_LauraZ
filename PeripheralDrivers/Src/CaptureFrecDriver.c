/*
 * CaptureFrecDriver.c
 *
 *  Created on: Month XX, 2022
 *      Author: namontoy
 */
#include "CaptureFrecDriver.h"

void capture_Config(Capture_Handler_t *ptrCaptureHandler){
	
	__disable_irq();

	/*1. Activar la señal de reloj del periférico requerido*/
	if(ptrCaptureHandler->ptrTIMx == TIM2){
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM3){
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM4){
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM5){
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}
	else{
		__NOP();
	}

	/*2.A. Estamos en UP Mode, el límite se carga en ARR y se comienza en 0 */
	ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_DIR;

	/*3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo entrada
	 *
	 * 4. Además, en el mismo case se configura el modo de captura de frecuencia y su polaridad
	 *
	 * 5.
	 */

	switch(ptrCaptureHandler->config.channel){
	case CAPTURE_CHANNEL_1:{
		//Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC1S);
		//Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (1 << TIM_CCMR1_CC1S_Pos);

		//Configuramos el filtro
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR1_IC1F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= 0x3 << TIM_CCMR1_IC1F_Pos;

		//Configuramos el prescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 << TIM_CCMR1_IC1PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= ptrCaptureHandler->config.prescalerCapture << TIM_CCMR1_IC1PSC_Pos;

		//Configuramos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			//Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1NP);
		}

		else{
			//Configuración 01 para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC1P;
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1NP);
		}

		//Activamos el módulo de captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;

		break;
	}

	case CAPTURE_CHANNEL_2: {
		//Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC2S);
		//Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (1 << TIM_CCMR1_CC2S_Pos);

		//Configuramos el filtro
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR1_IC2F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= 0x3 << TIM_CCMR1_IC2F_Pos;

		//Configuramos el prescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 << TIM_CCMR1_IC2PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= ptrCaptureHandler->config.prescalerCapture << TIM_CCMR1_IC2PSC_Pos;

		//Configuramos el flanco que deseamos capturar
		if (ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE) {
			//Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2NP);
		}

		else {
			//Configuración 01 para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC2P;
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2NP);
		}

		//Activamos el módulo de captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;

		break;
	}

	case CAPTURE_CHANNEL_3: {
		//Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(TIM_CCMR2_CC3S);
		//Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (1 << TIM_CCMR2_CC3S_Pos);

		//Configuramos el filtro
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0xF << TIM_CCMR2_IC3F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= 0x3 << TIM_CCMR2_IC3F_Pos;

		//Configuramos el prescaler
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0x3 << TIM_CCMR2_IC3PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= ptrCaptureHandler->config.prescalerCapture << TIM_CCMR2_IC3PSC_Pos;

		//Configuramos el flanco que deseamos capturar
		if (ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE) {
			//Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3NP);
		}

		else {
			//Configuración 01 para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC3P;
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3NP);
		}

		//Activamos el módulo de captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;

		break;
	}

	case CAPTURE_CHANNEL_4: {
		//Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(TIM_CCMR2_CC4S);
		//Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (1 << TIM_CCMR2_CC4S_Pos);

		//Configuramos el filtro
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0xF << TIM_CCMR2_IC4F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= 0x3 << TIM_CCMR2_IC4F_Pos;

		//Configuramos el prescaler
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0x3 << TIM_CCMR2_IC4PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= ptrCaptureHandler->config.prescalerCapture << TIM_CCMR2_IC3PSC_Pos;

		//Configuramos el flanco que deseamos capturar
		if (ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE) {
			//Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4NP);
		}

		else {
			//Configuración 01 para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC4P;
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4NP);
		}

		//Activamos el módulo de captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC4E;

		break;
	}

	default: {
		__NOP();
	}

	}//Fin switch-case

	//Configuramos el prescaler del timer, el cual define a qué velocidad se incrementa nuestro timer
	ptrCaptureHandler->ptrTIMx->PSC = ptrCaptureHandler->config.timerSpeed;


	/*Activamos la interrupción debida a la captura de frecuencia: Modificar el registro encargado de activar la
	 * interrupcion generada por el canal de captura*/

	//Primero se desactiva la interrupción generada por el timer
	ptrCaptureHandler->ptrTIMx->DIER &= ~TIM_DIER_UIE;

	//Ahora, se hace un switch case, dependiendo del canal:

	switch(ptrCaptureHandler->config.channel){
		case CAPTURE_CHANNEL_1:{
			ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC1IE;

			break;
		}

		case CAPTURE_CHANNEL_2:{
			ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC2IE;

			break;
		}

		case CAPTURE_CHANNEL_3:{
			ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC3IE;

			break;
		}

		case CAPTURE_CHANNEL_4:{
			ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC4IE;

			break;
		}

		default: {
			__NOP();
		}
	}//Fin switch-case

	//Ahora, Activamos el canal del sistema NVIC para que lea la interrupción

		if (ptrCaptureHandler->ptrTIMx == TIM2) {
			// Activando en NVIC para la interrupción del TIM2
			NVIC_EnableIRQ(TIM2_IRQn);
		} else if (ptrCaptureHandler->ptrTIMx == TIM3) {
			// Activando en NVIC para la interrupción del TIM3
			NVIC_EnableIRQ(TIM3_IRQn);
		} else if (ptrCaptureHandler->ptrTIMx == TIM4) {
			// Activando en NVIC para la interrupción del TIM3
			NVIC_EnableIRQ(TIM4_IRQn);
		} else if (ptrCaptureHandler->ptrTIMx == TIM5) {
			// Activando en NVIC para la interrupción del TIM3
			NVIC_EnableIRQ(TIM5_IRQn);
		} else {
			__NOP();
		}

	/* 7. Volvemos a activar las interrupciones del sistema */
	__enable_irq();
}

__attribute__((weak)) void CaptureFreqTimer2_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}
__attribute__((weak)) void CaptureFreqTimer3_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}
__attribute__((weak)) void CaptureFreqTimer4_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}
__attribute__((weak)) void CaptureFreqTimer5_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}

/*Esta función se encarga de lanzar la captura de frecuencia, en este caso funcion
 * con pulling, examinando cuando se levanta la bandera del evento de captura*/

uint32_t getPeriodFrec(Capture_Handler_t *ptrCaptureHandler){

	uint32_t timestamp1 = 0;
	uint32_t timestamp2 = 0;
	uint32_t deltaTimestamp = 0;

	//Reiniciamos el contador del Timer
	ptrCaptureHandler->ptrTIMx->CNT = 0;

	/*Todo lo que se sigue de acá en adelante depende del canal que se ha seleccionado,
	 * por lo cual se escribe cada código dentro del "case" específico.
	 * Antes de lanzar la captura (encender el Timer para que cuente),
	 * debemos borrar el valor de los registros CCP, de forma que arranquen limpios*/

	switch (ptrCaptureHandler->config.channel){

	case CAPTURE_CHANNEL_1:{
		//Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR1 = 0;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~(TIM_SR_CC1IF);

		//Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Esperamos a que se de el primer evento:
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC1IF)){
		}

		//Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		//Capturamos el valor del tiempo almacenado en el CCRx
		timestamp1 = ptrCaptureHandler->ptrTIMx->CCR1;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= TIM_SR_CC1IF;

		//Encendemos el timer para que empiece a contar de nuevo
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Esperamos a que se de el segundo evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC1IF)){
			}

		//Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		//Capturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado después de la
		// primer captura)
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR1;

		deltaTimestamp = timestamp2 - timestamp1;

		break;
	}

	case CAPTURE_CHANNEL_2: {
		//Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR2 = 0;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~(TIM_SR_CC2IF);

		//Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Esperamos a que se de el primer evento:
		while (!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC2IF)) {
		}

		//Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		//Capturamos el valor del tiempo almacenado en el CCRx
		timestamp1 = ptrCaptureHandler->ptrTIMx->CCR2;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= TIM_SR_CC2IF;

		//Encendemos el timer para que empiece a contar de nuevo
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Esperamos a que se de el segundo evento
		while (!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC2IF)) {
		}

		//Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		//Capturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado después de la
		// primer captura)
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR2;

		deltaTimestamp = timestamp2 - timestamp1;

		break;
	}


	case CAPTURE_CHANNEL_3: {
		//Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR3 = 0;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~(TIM_SR_CC3IF);

		//Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Esperamos a que se de el primer evento:
		while (!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC3IF)) {
		}

		//Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		//Capturamos el valor del tiempo almacenado en el CCRx
		timestamp1 = ptrCaptureHandler->ptrTIMx->CCR3;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= TIM_SR_CC3IF;

		//Encendemos el timer para que empiece a contar de nuevo
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Esperamos a que se de el segundo evento
		while (!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC3IF)) {
		}

		//Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		//Capturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado después de la
		// primer captura)
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR3;

		deltaTimestamp = timestamp2 - timestamp1;

		break;
	}

	case CAPTURE_CHANNEL_4: {
		//Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR4 = 0;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~(TIM_SR_CC4IF);

		//Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Esperamos a que se de el primer evento:
		while (!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC4IF)) {
		}

		//Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		//Capturamos el valor del tiempo almacenado en el CCRx
		timestamp1 = ptrCaptureHandler->ptrTIMx->CCR4;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= TIM_SR_CC4IF;

		//Encendemos el timer para que empiece a contar de nuevo
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Esperamos a que se de el segundo evento
		while (!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC4IF)) {
		}

		//Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		//Capturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado después de la
		// primer captura)
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR4;

		deltaTimestamp = timestamp2 - timestamp1;

		break;
	}

	default: {
		break;
	}
	}

	return deltaTimestamp;
}

void captureFreqInterruptModeEnable(Capture_Handler_t *ptrCaptureHandler){
	ptrCaptureHandler->ptrTIMx->CNT = 0;
	ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

	//Se limpian todas las banderas:
	ptrCaptureHandler->ptrTIMx->SR = 0;
}

void captureFreqClearCNT(Capture_Handler_t *ptrCaptureHandler){

	switch (ptrCaptureHandler->config.channel){

	case CAPTURE_CHANNEL_1:{

		//Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR1 = 0;
	}

	case CAPTURE_CHANNEL_2:{

		//Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR2 = 0;
	}

	case CAPTURE_CHANNEL_3:{

		//Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR3 = 0;
	}

	case CAPTURE_CHANNEL_4:{

		//Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR4 = 0;
	}

	default:{

	}

	}
	ptrCaptureHandler->ptrTIMx->CNT &= ~(0xFFFFFFFF);
}


