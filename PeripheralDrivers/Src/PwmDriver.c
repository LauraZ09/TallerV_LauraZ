/*
 * PwmDriver.c
 *
 *  Created on: XXXX , 2022
 *      Author: namontoy
 */
#include "PwmDriver.h"

uint32_t duttyCicleValue;

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

		//Configuramos la polaridad:
        //CC1P: Capture/Compare 1 output Polarity.
        //CC1 channel configured as output:
        //0: OC1 active high
        //1: OC1 active low
        //CC1 channel configured
		//Se pone en 0 active high

		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC1P;


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

		//Configuramos la polaridad:
        //CC1P: Capture/Compare 1 output Polarity.
        //CC1 channel configured as output:
        //0: OC1 active high
        //1: OC1 active low
        //CC1 channel configured
		//Se pone en 0 active high

		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC2P;

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

		//Configuramos la polaridad:
        //CC1P: Capture/Compare 1 output Polarity.
        //CC1 channel configured as output:
        //0: OC1 active high
        //1: OC1 active low
        //CC1 channel configured
		//Se pone en 0 active high

		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC3P;

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

		//Configuramos la polaridad:
        //CC1P: Capture/Compare 1 output Polarity.
        //CC1 channel configured as output:
        //0: OC1 active high
        //1: OC1 active low
        //CC1 channel configured
		//Se pone en 0 active high

		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC4P;

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
	ptrPwmHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN; //TODO
}

/* Función encargada de activar cada uno de los canales con los que cuenta el TimerX */
void enableOutput(PWM_Handler_t *ptrPwmHandler) {
	switch (ptrPwmHandler->config.channel) {
	case PWM_CHANNEL_1: {
		// Activamos la salida del canal 1
	    //CC1E: Capture/Compare 1 output enable.
	    //CC1 channel configured as output:
	    //0: Off - OC1 is not active
	    //1: On - OC1 signal is output on the corresponding output pin
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;

	    break;
	}

	case PWM_CHANNEL_2: {
		// Activamos la salida del canal 1
	    //CC1E: Capture/Compare 1 output enable.
	    //CC1 channel configured as output:
	    //0: Off - OC1 is not active
	    //1: On - OC1 signal is output on the corresponding output pin
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;

	    break;
	}

	case PWM_CHANNEL_3: {
			// Activamos la salida del canal 1
		    //CC1E: Capture/Compare 1 output enable.
		    //CC1 channel configured as output:
		    //0: Off - OC1 is not active
		    //1: On - OC1 signal is output on the corresponding output pin
			ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;

		    break;
		}

	case PWM_CHANNEL_4: {
			// Activamos la salida del canal 1
		    //CC1E: Capture/Compare 1 output enable.
		    //CC1 channel configured as output:
		    //0: Off - OC1 is not active
		    //1: On - OC1 signal is output on the corresponding output pin
			ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC4E;

		    break;
		}

	default: {
		break;
	}

	}
}

/* Función encargada de desactivar cada uno de los canales con los que cuenta el TimerX */
void disableOutput(PWM_Handler_t *ptrPwmHandler) {
	switch (ptrPwmHandler->config.channel) {
	case PWM_CHANNEL_1: {
		// Activamos la salida del canal 1
	    //CC1E: Capture/Compare 1 output enable.
	    //CC1 channel configured as output:
	    //0: Off - OC1 is not active
	    //1: On - OC1 signal is output on the corresponding output pin
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC1E;

	    break;
	}

	case PWM_CHANNEL_2: {
		// Activamos la salida del canal 1
	    //CC1E: Capture/Compare 1 output enable.
	    //CC1 channel configured as output:
	    //0: Off - OC1 is not active
	    //1: On - OC1 signal is output on the corresponding output pin
		ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC2E;

	    break;
	}

	case PWM_CHANNEL_3: {
			// Activamos la salida del canal 1
		    //CC1E: Capture/Compare 1 output enable.
		    //CC1 channel configured as output:
		    //0: Off - OC1 is not active
		    //1: On - OC1 signal is output on the corresponding output pin
			ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC3E;

		    break;
		}

	case PWM_CHANNEL_4: {
			// Activamos la salida del canal 1
		    //CC1E: Capture/Compare 1 output enable.
		    //CC1 channel configured as output:
		    //0: Off - OC1 is not active
		    //1: On - OC1 signal is output on the corresponding output pin
			ptrPwmHandler->ptrTIMx->CCER &= ~TIM_CCER_CC4E;

		    break;
		}

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
	ptrPwmHandler->ptrTIMx->PSC = ptrPwmHandler->config.prescaler;

	// Cargamos el valor del ARR, el cual es el límite de incrementos del Timer
	// antes de hacer un update y reload.
	ptrPwmHandler->ptrTIMx->ARR = ptrPwmHandler->config.periodo - 1;
}

/* Función para actualizar la frecuencia, funciona de la mano con setFrequency */
void updateFrequency(PWM_Handler_t *ptrPwmHandler, uint16_t newFreq){
	// Actualizamos el registro que manipula el periodo
	ptrPwmHandler->config.periodo = newFreq;

	// Llamamos a la función que cambia la frecuencia
	setFrequency(ptrPwmHandler);
}

/* El valor del dutty debe estar dado en valores de %, entre 0% y 100%*/
void setDuttyCycle(PWM_Handler_t *ptrPwmHandler){

	// Seleccionamos el canal para configurar su dutty
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		ptrPwmHandler->ptrTIMx->CCR1 = ptrPwmHandler->config.duttyCicle;
		break;
	}

	case PWM_CHANNEL_2:{
		ptrPwmHandler->ptrTIMx->CCR2 = ptrPwmHandler->config.duttyCicle;
		break;
	}

	case PWM_CHANNEL_3:{
		ptrPwmHandler->ptrTIMx->CCR3 = ptrPwmHandler->config.duttyCicle;
		break;
	}

	case PWM_CHANNEL_4:{
		ptrPwmHandler->ptrTIMx->CCR4 = ptrPwmHandler->config.duttyCicle;
		break;
	}

	default:{
		break;
	}

	}// fin del switch-case

}

/* Función para actualizar el Dutty, funciona de la mano con setDuttyCycle */
void updateDuttyCycle(PWM_Handler_t *ptrPwmHandler, uint16_t newDutty){
	// Actualizamos el registro que manipula el dutty
	// Seleccionamos el canal para configurar su dutty
		ptrPwmHandler->config.duttyCicle = newDutty;

	// Llamamos a la fucnión que cambia el dutty y cargamos el nuevo valor
		setDuttyCycle(ptrPwmHandler);
}

void enableEvent(PWM_Handler_t *ptrPwmHandler){
	switch (ptrPwmHandler->config.channel) {

	case PWM_CHANNEL_1: {
		//Activamos el evento en el canal 1
		ptrPwmHandler->ptrTIMx->EGR |= TIM_EGR_CC1G;
		break;
	}

	case PWM_CHANNEL_2: {
		//Activamos el evento en el canal 1
		ptrPwmHandler->ptrTIMx->EGR |= TIM_EGR_CC2G;
		break;
	}

	case PWM_CHANNEL_3: {
		//Activamos el evento en el canal 1
		ptrPwmHandler->ptrTIMx->EGR |= TIM_EGR_CC3G;
		break;
	}

	case PWM_CHANNEL_4: {
		//Activamos el evento en el canal 1
		ptrPwmHandler->ptrTIMx->EGR |= TIM_EGR_CC4G;
		break;
	}

	default: {
		break;
	}
	}
}

void disableEvent(PWM_Handler_t *ptrPwmHandler){
	switch (ptrPwmHandler->config.channel) {

	case PWM_CHANNEL_1: {
		//Activamos el evento en el canal 1
		ptrPwmHandler->ptrTIMx->EGR &= ~TIM_EGR_CC1G;
		break;
	}

	case PWM_CHANNEL_2: {
		//Activamos el evento en el canal 1
		ptrPwmHandler->ptrTIMx->EGR &= ~TIM_EGR_CC2G;
		break;
	}

	case PWM_CHANNEL_3: {
		//Activamos el evento en el canal 1
		ptrPwmHandler->ptrTIMx->EGR &= ~TIM_EGR_CC3G;
		break;
	}

	case PWM_CHANNEL_4: {
		//Activamos el evento en el canal 1
		ptrPwmHandler->ptrTIMx->EGR &= ~TIM_EGR_CC4G;
		break;
	}

	default: {
		break;
	}
	}
}

uint32_t getDuttyCycleValue(PWM_Handler_t *ptrPwmHandler){

	switch(ptrPwmHandler->config.channel){

		case PWM_CHANNEL_1:{
			duttyCicleValue = ptrPwmHandler->ptrTIMx->CCR1;
			break;
		}

		case PWM_CHANNEL_2:{
			duttyCicleValue = ptrPwmHandler->ptrTIMx->CCR2;
			break;
		}

		case PWM_CHANNEL_3:{
			duttyCicleValue = ptrPwmHandler->ptrTIMx->CCR3;
			break;
		}

		case PWM_CHANNEL_4:{
			duttyCicleValue = ptrPwmHandler->ptrTIMx->CCR4;
			break;
		}

		default:{
			break;
		}
		}

	return duttyCicleValue;
}








