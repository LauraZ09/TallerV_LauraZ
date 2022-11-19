/*
 * CaptureFrecDriver.c
 *
 *  Created on: Month XX, 2022
 *      Author: namontoy
 */
#include "CaptureFrecDriver.h"

void capture_Config(Capture_Handler_t *ptrCaptureHandler){
	
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

		//Configuramos el filtro(sin filtro)

	}
	}
}
