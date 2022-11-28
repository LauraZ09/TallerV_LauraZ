/*
 * BasicTimer.c
 *
 *  Created on: Apr 18, 2022
 *      Author: namontoy
 */

#include "BasicTimer.h"
#include "CaptureFrecDriver.h"


/* Función en la que cargamos la configuración del Timer
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periférico que se está utilizando.
 * Además, en este caso, debemos ser cuidadosos al momento de utilizar las interrupciones.
 * Los Timer están conectados directamente al elemento NVIC del Cortex-Mx
 * Debemos configurar y/o utilizar:
 *  - TIMx_CR1  (control Register 1)
 *  - TIMx_SMCR ( slave mode control register) -> mantener en 0 para modo Timer Básico
 *  - TIMx_DIER (DMA and Interrupt enable register)
 *  - TIMx_SR (Status register)
 *  - TIMx_CNT (Counter)
 *  - TIMx_PSC (Pre-scaler)
 *  - TIMx_ARR  (Auto-reload register)
 *
 *  Como vamos a trabajar con interrupciones, antes de configurar una nueva, debemos desactivar
 *  el sistema global de interrupciones, activar la IRQ específica y luego volver a encender
 *  el sistema.
 */
void Timer10_Config(BasicTimer_Handler_t *ptrBTimerHandler){
	// Guardamos una referencia al periferico que estamos utilizando...

	/* 0. Desactivamos las interrupciones globales mientras configuramos el sistema.*/
	__disable_irq();

	//Se configura el rgistro RCC_DCKCFGR: el bit TIMPRE configura si la frecuencia del TIMER es la misma del bus o se multiplica
	/*TIMPRE: Timers clocks prescalers selection
	 Set and reset by software to control the clock frequency of all the timers connected to APB1
	 and APB2 domain.
	 0: If the APB prescaler (PPRE1, PPRE2 in the RCC_CFGR register) is configured to a
	 division factor of 1, TIMxCLK = HCKL . Otherwise, the timer clock frequencies are set to
	 twice to the frequency of the APB domain to which the timers are connected:
	 TIMxCLK = 2xPCLKx.
	 1:If the APB prescaler ( PPRE1, PPRE2 in the RCC_CFGR register) is configured to a
	 division factor of 1 or 2, TIMxCLK = HCKL. Otherwise, the timer clock frequencies are set to
	 four times to the frequency of the APB domain to which the timers are connected:
	 TIMxCLK = 4xPCLKx.*/

	RCC->DCKCFGR &= ~RCC_DCKCFGR_TIMPRE;

	/* 1. Activar la señal de reloj del periférico requerido */

	if (ptrBTimerHandler->ptrTIMx == TIM10) {
		// Registro del RCC que nos activa la señal de reloj para el TIM2
		RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
	}

	else{
		__NOP();
	}

	/* 2. Configuramos el Pre-scaler
	 * Recordar que el prescaler nos indica la velocidad a la que se incrementa el counter, de forma que
	 * periodo_incremento * veces_incremento_counter = periodo_update
	 * Modificar el valor del registro PSC en el TIM utilizado
	 */
	ptrBTimerHandler->ptrTIMx->PSC = ptrBTimerHandler->TIMx_Config.TIMx_speed;

	/* 3a. Estamos en UP_Mode, el limite se carga en ARR y se comienza en 0 */
	// Configurar el registro que nos controla el modo up or down
	/* 3b. Configuramos el Auto-reload. Este es el "limite" hasta donde el CNT va a contar */
	ptrBTimerHandler->ptrTIMx->ARR = ptrBTimerHandler->TIMx_Config.TIMx_period - 1;

	/* 3c. Reiniciamos el registro counter*/
	ptrBTimerHandler->ptrTIMx->CNT = 0;

	/* 4. Activamos el Timer (el CNT debe comenzar a contar)*/
	ptrBTimerHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

	/* 5. Activamos la interrupción debida al Timerx Utilizado
	 * Modificar el registro encargado de activar la interrupcion generada por el TIMx*/
	ptrBTimerHandler->ptrTIMx->DIER |= TIM_DIER_UIE;

	/* 6. Activamos el canal del sistema NVIC para que lea la interrupción*/
	if(ptrBTimerHandler->ptrTIMx == TIM10){
		// Activando en NVIC para la interrupción del TIM2
		NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	}

	else{
		__NOP();
	}

	/* 7. Volvemos a activar las interrupciones del sistema */
	__enable_irq();
}

__attribute__((weak)) void BasicTimer10_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}

/* Esta es la función a la que apunta el sistema en el vector de interrupciones.
 * Se debe utilizar usando exactamente el mismo nombre definido en el vector de interrupciones,
 * Al hacerlo correctamente, el sistema apunta a esta función y cuando la interrupción se lanza
 * el sistema inmediatamente salta a este lugar en la memoria*/
void TIM1_UP_TIM10_IRQHandler(void){

	if (TIM10->SR & TIM_SR_UIF){

		/* Limpiamos la bandera que indica que la interrupción se ha generado */
		TIM10->SR &= ~TIM_SR_UIF;

		/* LLamamos a la función que se debe encargar de hacer algo con esta interrupción*/
		BasicTimer10_Callback();
	}

	else{
		__NOP();
	}

}



