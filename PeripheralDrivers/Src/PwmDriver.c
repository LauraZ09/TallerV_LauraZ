/*
 * PwmDriver.c
 *
 *  Created on: XXXX , 2022
 *      Author: namontoy
 */
#include "PwmDriver.h"

/**/
void pwm_Config(PWM_Handler_t *ptrPwmHandler){

	/* 1. Activar la señal de reloj del periférico requerido */
	if (ptrPwmHandler->ptrTIMx == TIM2) {
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM3){
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}

	else if(ptrPwmHandler->ptrTIMx == TIM4){
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}

	else if(ptrPwmHandler->ptrTIMx == TIM5){
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}

	else{
		__NOP();
	}

	/* 1. Cargamos la frecuencia deseada */
	setFrequency(ptrPwmHandler);

	/* 2. Cargamos el valor del dutty-Cycle*/
	setDuttyCycle(ptrPwmHandler);

	/* 2a. Estamos en UP_Mode, el limite se carga en ARR y se comienza en 0 */
	ptrPwmHandler->ptrTIMx->CR1 &= ~TIM_CR1_DIR;

	/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo salida
	 * (para cada canal hay un conjunto CCxS)
	 * 00: CC1 channel is configured as output.
	 * 00: CC2 channel is configured as output
	 *
	 * 4. Además, en el mismo "case" podemos configurar el modo del PWM, su polaridad...
	 *
	 * 5. Y además activamos el preload bit, para que cada vez que exista un update-event
	 * el valor cargado en el CCRx será recargado en el registro "shadow" del PWM */

	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{

		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_CC1S;

		// Configuramos el canal como PWM
		//111: PWM mode 2 - In upcounting, channel 1 is inactive as long as TIMx_CNT<TIMx_CCR1
		//else active. In downcounting, channel 1 is active as long as TIMx_CNT>TIMx_CCR1 else
		//inactive.
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1M;
		ptrPwmHandler->ptrTIMx->



		// Activamos la funcionalidad de pre-load
		//1: Preload register on TIMx_CCR1 enabled.
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1PE;

		break;
	}

	case PWM_CHANNEL_2:{

		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~TIM_CCMR1_CC2S;

		// Configuramos el canal como PWM
		//111: PWM mode 2 - In upcounting, channel 1 is inactive as long as TIMx_CNT<TIMx_CCR1
		//else active. In downcounting, channel 1 is active as long as TIMx_CNT>TIMx_CCR1 else
		//inactive.
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2M;

		// Activamos la funcionalidad de pre-load
		//1: Preload register on TIMx_CCR1 enabled.
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2PE;

		break;
	}

	case PWM_CHANNEL_3: {

		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_CC3S;

		// Configuramos el canal como PWM
		//111: PWM mode 2 - In upcounting, channel 1 is inactive as long as TIMx_CNT<TIMx_CCR1
		//else active. In downcounting, channel 1 is active as long as TIMx_CNT>TIMx_CCR1 else
		//inactive.
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3M;

		// Activamos la funcionalidad de pre-load
		//1: Preload register on TIMx_CCR1 enabled.
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3PE;

		break;
	}

	case PWM_CHANNEL_4: {

		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~TIM_CCMR2_CC4S;

		// Configuramos el canal como PWM
		//111: PWM mode 2 - In upcounting, channel 1 is inactive as long as TIMx_CNT<TIMx_CCR1
		//else active. In downcounting, channel 1 is active as long as TIMx_CNT>TIMx_CCR1 else
		//inactive.
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4M;

		// Activamos la funcionalidad de pre-load
		//1: Preload register on TIMx_CCR1 enabled.
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4PE;

		break;
	}


	default:{
		__NOP();
		break;
	}

	/* 6. Activamos la salida seleccionada */
	enableOutput(ptrPwmHandler);

	}// fin del switch-case
}

/* Función para activar el Timer y activar todo el módulo PWM */
void startPwmSignal(PWM_Handler_t *ptrPwmHandler) {
	ptrPwmHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;
}

/* Función para desactivar el Timer y detener todo el módulo PWM*/
void stopPwmSignal(PWM_Handler_t *ptrPwmHandler) {
	/* agregue acá su código */
}

/* Función encargada de activar cada uno de los canales con los que cuenta el TimerX */
void enableOutput(PWM_Handler_t *ptrPwmHandler) {
	switch (ptrPwmHandler->config.channel) {
	case PWM_CHANNEL_1: {
		// Activamos la salida del canal 1
		/* agregue acá su código */
		break;
	}

	/* agregue acá su código para los otros tres casos */

	default: {
		break;
	}
	}
}

/* 
 * La frecuencia es definida por el conjunto formado por el preescaler (PSC)
 * y el valor límite al que llega el Timer (ARR), con estos dos se establece
 * la frecuencia.
 * */
void setFrequency(PWM_Handler_t *ptrPwmHandler){

	// Cargamos el valor del prescaler, nos define la velocidad (en ns) a la cual
	// se incrementa el Timer
	/* agregue acá su código */

	// Cargamos el valor del ARR, el cual es el límite de incrementos del Timer
	// antes de hacer un update y reload.
	/* agregue acá su código */
}


/* Función para actualizar la frecuencia, funciona de la mano con setFrequency */
void updateFrequency(PWM_Handler_t *ptrPwmHandler, uint16_t newFreq){
	// Actualizamos el registro que manipula el periodo
    /* agregue acá su código */

	// Llamamos a la fucnión que cambia la frecuencia
	/* agregue acá su código */
}

/* El valor del dutty debe estar dado en valores de %, entre 0% y 100%*/
void setDuttyCycle(PWM_Handler_t *ptrPwmHandler){

	// Seleccionamos el canal para configurar su dutty
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		ptrPwmHandler->ptrTIMx->CCR1 = ptrPwmHandler->config.duttyCicle;

		break;
	}

	/* agregue acá su código con los otros tres casos */

	default:{
		break;
	}

	}// fin del switch-case

}


/* Función para actualizar el Dutty, funciona de la mano con setDuttyCycle */
void updateDuttyCycle(PWM_Handler_t *ptrPwmHandler, uint16_t newDutty){
	// Actualizamos el registro que manipula el dutty
    /* agregue acá su código */

	// Llamamos a la fucnión que cambia el dutty y cargamos el nuevo valor
	/* agregue acá su código */
}




