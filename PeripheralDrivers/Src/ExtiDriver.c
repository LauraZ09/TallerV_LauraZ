/*
 * ExtiDriver.c
 *
 *  Created on: XXX, 2022
 *      Author: namontoy
 */

#include "stm32f4xx.h"
#include "ExtiDriver.h"
#include "GPIOxDriver.h"

void extInt_Config(EXTI_Config_t *extiConfig){

	/* 1.0 Se carga la configuración, que debe ser el PINx como entrada "simple" */
	extiConfig->pGPIOHandler->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIO_Config(extiConfig->pGPIOHandler);

	/* 2.0 Activamos el acceso al SYSCFG */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* 3.0  Asignamos el canal EXTI que corresponde al PIN_y del puerto GPIO_X
	 * Debemos activar la línea PIN_Xy (Y = A, B, C... y x = 0, 1, 2, 3...)
	 * en el módulo EXTI */
	switch (extiConfig->pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber) {

	/* Configurando para el todos los pines GPIOX_0*/
	case 0: {
		/* SYSCFG_EXTICR1 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[0] &= ~(0xF << SYSCFG_EXTICR1_EXTI0_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PH);

		} else {
			__NOP();
		}

		break;
	}

	/* Configurando para el todos los pines GPIOX_1*/
	case 1: {
		/* SYSCFG_EXTICR1 */
		//Se limpia la posición que se va a cnfigurar
		SYSCFG->EXTICR[0] &= ~(0xF << SYSCFG_EXTICR1_EXTI1_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PH);

		} else {
			__NOP();
		}

		break;
	}

		/* Configurando para el todos los pines GPIOX_2*/
	case 2: {
		/* SYSCFG_EXTICR1 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[0] &= ~(0xF << SYSCFG_EXTICR1_EXTI2_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_2, ó GPIOB_2, ó GPIOC_2, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PH);

		} else {
			__NOP();
		}

		break;
	}

		/* Configurando para el todos los pines GPIOX_3*/
	case 3: {
		/* SYSCFG_EXTICR1 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[0] &= ~(0xF << SYSCFG_EXTICR1_EXTI3_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_3, ó GPIOB_3, ó GPIOC_3, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PH);

		} else {
			__NOP();
		}

		break;
	}

		/* Configurando para el todos los pines GPIOX_4*/
	case 4: {
		/* SYSCFG_EXTICR2 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[1] &= ~(0xF << SYSCFG_EXTICR2_EXTI4_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_4, ó GPIOB_4, ó GPIOC_4, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PH);

		} else {
			__NOP();
		}

		break;
	}

	/* Configurando para el todos los pines GPIOX_5*/
	case 5: {
		/* SYSCFG_EXTICR2 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[1] &= ~(0xF << SYSCFG_EXTICR2_EXTI5_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_5, ó GPIOB_5, ó GPIOC_5, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PH);

		} else {
			__NOP();
		}

		break;
	}

		/* Configurando para el todos los pines GPIOX_6*/
	case 6: {
		/* SYSCFG_EXTICR2 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[1] &= ~(0xF << SYSCFG_EXTICR2_EXTI6_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_6, ó GPIOB_6, ó GPIOC_6, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PH);

		} else {
			__NOP();
		}

		break;
	}

		/* Configurando para el todos los pines GPIOX_7*/
	case 7: {
		/* SYSCFG_EXTICR2 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[1] &= ~(0xF << SYSCFG_EXTICR2_EXTI7_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_7, ó GPIOB_7, ó GPIOC_7, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PH);

		} else {
			__NOP();
		}

		break;
	}

	/* Configurando para el todos los pines GPIOX_8*/
	case 8: {
		/* SYSCFG_EXTICR2 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[2] &= ~(0xF << SYSCFG_EXTICR3_EXTI8_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_8, ó GPIOB_8, ó GPIOC_8, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PH);

		} else {
			__NOP();
		}

		break;
	}

	/* Configurando para el todos los pines GPIOX_9*/
	case 9: {
		/* SYSCFG_EXTICR2 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[2] &= ~(0xF << SYSCFG_EXTICR3_EXTI9_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_9, ó GPIOB_9, ó GPIOC_9, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PH);

		} else {
			__NOP();
		}

		break;
	}

	/* Configurando para el todos los pines GPIOX_10*/
	case 10: {
		/* SYSCFG_EXTICR2 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[2] &= ~(0xF << SYSCFG_EXTICR3_EXTI10_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_10, ó GPIOB_10, ó GPIOC_10, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PH);

		} else {
			__NOP();
		}

		break;
	}

	/* Configurando para el todos los pines GPIOX_11*/
	case 11: {
		/* SYSCFG_EXTICR2 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[2] &= ~(0xF << SYSCFG_EXTICR3_EXTI11_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_11, ó GPIOB_11, ó GPIOC_11, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PH);

		} else {
			__NOP();
		}

		break;
	}

	/* Configurando para el todos los pines GPIOX_12*/
	case 12: {
		/* SYSCFG_EXTICR2 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI12_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_12, ó GPIOB_12, ó GPIOC_12, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PH);

		} else {
			__NOP();
		}

		break;
	}

	/* Configurando para el todos los pines GPIOX_13*/
	case 13: {
		/* SYSCFG_EXTICR2 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI13_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_13, ó GPIOB_13, ó GPIOC_13, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PH);

		} else {
			__NOP();
		}

		break;
	}

	/* Configurando para el todos los pines GPIOX_14*/
	case 14: {
		/* SYSCFG_EXTICR2 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI14_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_14, ó GPIOB_14, ó GPIOC_14, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PH);

		} else {
			__NOP();
		}

		break;
	}

	/* Configurando para el todos los pines GPIOX_15 */
	case 15: {
		/* SYSCFG_EXTICR4 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI15_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PH);

		} else {
			__NOP();
		}
		break;
	}

	default: {
		__NOP();
		break;
	}

	}// Fin del switch-case

	/* 4.0 Seleccionamos el tipo de flanco */
	if(extiConfig->edgeType == EXTERNAL_INTERRUPT_FALLING_EDGE){
		/* Falling Trigger selection register:
		 * TRx: Falling trigger event configuration bit of line x
		 0: Falling trigger disabled (for Event and Interrupt) for input line
		 1: Falling trigger enabled (for Event and Interrupt) for input line.*/

		switch (extiConfig->pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber) {

		/* Configurando para todos los pines GPIOX_0*/
		case 0: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR0;
			break;
		}

		/* Configurando para todos los pines GPIOX_1*/
		case 1: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR1;
			break;
		}

		/* Configurando para todos los pines GPIOX_2*/
		case 2: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR2;
			break;
		}

		case 3: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR3;
			break;
		}

		case 4: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR4;
			break;
		}

		case 5: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR5;
			break;
		}

		case 6: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR6;
			break;
		}

		case 7: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR7;
			break;
		}

		case 8: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR8;
			break;
		}

		case 9: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR9;
			break;
		}

		case 10: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR10;
			break;
		}

		case 11: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR11;
			break;
		}

		case 12: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR12;
			break;
		}

		case 13: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR13;
			break;
		}

		case 14: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR14;
			break;
		}

		case 15: {
			/* EXTI_FTSR */
			EXTI->FTSR |= EXTI_FTSR_TR15;
			break;
		}

		default: {
				__NOP();
				break;
		}
	}

	}

	else if (extiConfig->edgeType == EXTERNAL_INTERRUPT_RISING_EDGE){

		switch (extiConfig->pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber) {

		/* Configurando para todos los pines GPIOX_0*/
		case 0: {
			/* EXTI_RTSR */
			EXTI->RTSR |= EXTI_RTSR_TR0;
			break;
		}

			/* Configurando para todos los pines GPIOX_1*/
		case 1: {
			/* EXTI_RTSR */
			EXTI->RTSR |= EXTI_RTSR_TR1;
			break;
		}

			/* Configurando para todos los pines GPIOX_2*/
		case 2: {
			/* EXTI_FTSR */
			EXTI->RTSR |= EXTI_RTSR_TR2;
			break;
		}

		case 3: {
			/* EXTI_FTSR */
			EXTI->RTSR |= EXTI_RTSR_TR3;
			break;
		}

		case 4: {
			/* EXTI_FTSR */
			EXTI->RTSR |= EXTI_RTSR_TR4;
			break;
		}

		case 5: {
			/* EXTI_FTSR */
			EXTI->RTSR |= EXTI_RTSR_TR5;
			break;
		}

		case 6: {
			/* EXTI_FTSR */
			EXTI->RTSR |= EXTI_RTSR_TR6;
			break;
		}

		case 7: {
			/* EXTI_FTSR */
			EXTI->RTSR |= EXTI_RTSR_TR7;
			break;
		}

		case 8: {
			/* EXTI_FTSR */
			EXTI->RTSR |= EXTI_RTSR_TR8;
			break;
		}

		case 9: {
			/* EXTI_FTSR */
			EXTI->RTSR |= EXTI_RTSR_TR9;
			break;
		}

		case 10: {
			/* EXTI_FTSR */
			EXTI->RTSR |= EXTI_RTSR_TR10;
			break;
		}

		case 11: {
			/* EXTI_FTSR */
			EXTI->RTSR |= EXTI_RTSR_TR11;
			break;
		}

		case 12: {
			/* EXTI_FTSR */
			EXTI->RTSR |= EXTI_RTSR_TR12;
			break;
		}

		case 13: {
			/* EXTI_FTSR */
			EXTI->RTSR |= EXTI_RTSR_TR13;
			break;
		}

		case 14: {
			/* EXTI_FTSR */
			EXTI->RTSR |= EXTI_RTSR_TR14;
			break;
		}

		case 15: {
			/* EXTI_FTSR */
			EXTI->RTSR |= EXTI_RTSR_TR15;
			break;
		}

		default: {
			__NOP();
			break;
		}
		}

	}

	else{
		__NOP();
	}

	/* 5.0 Desactivo primero las interrupciones globales */
	__disable_irq();

	/* 6.0 Activamos la interrupción del canal que estamos configurando */
	// Interrupt Mask register

	//Ahora, se configura según el número del PIN
	switch (extiConfig->pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber) {

	/* Configurando para todos los pines GPIOX_0*/
	case 0: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR0;
		break;
	}

		/* Configurando para todos los pines GPIOX_1*/
	case 1: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR1;
		break;
	}

		/* Configurando para todos los pines GPIOX_2*/
	case 2: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR2;
		break;
	}

	case 3: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR3;
		break;
	}

	case 4: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR4;
		break;
	}

	case 5: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR5;
		break;
	}

	case 6: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR6;
		break;
	}

	case 7: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR7;
		break;
	}

	case 8: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR8;
		break;
	}

	case 9: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR9;
		break;
	}

	case 10: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR10;
		break;
	}

	case 11: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR11;
		break;
	}

	case 12: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR12;
		break;
	}

	case 13: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR13;
		break;
	}

	case 14: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR14;
		break;
	}

	case 15: {
		/* EXTI_IMR */
		EXTI->IMR |= EXTI_IMR_MR15;
		break;
	}

	default: {
		__NOP();
		break;
	}
	}


	/* 6.1 Matriculamos la interrupción en el NVIC para el canal correspondiente,
	 * donde el canal 0 corresponde al EXTI_0, canal 1 al EXTI_1, etc.
	 *
	 * NOTA: Observar que algunos canales EXTI comparten un mismo vector de interrupción
	 * */
	switch (extiConfig->pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber) {
	case 0: {
		__NVIC_EnableIRQ(EXTI0_IRQn);
		break;
	}

	case 1: {
		__NVIC_EnableIRQ(EXTI1_IRQn);
		break;
	}

	case 2: {
		__NVIC_EnableIRQ(EXTI2_IRQn);
		break;
	}

	case 3: {
		__NVIC_EnableIRQ(EXTI3_IRQn);
		break;
	}

	case 4: {
		__NVIC_EnableIRQ(EXTI4_IRQn);
		break;
	}

	case 5: {
		__NVIC_EnableIRQ(EXTI9_5_IRQn);
		break;
	}

	case 6: {
		__NVIC_EnableIRQ(EXTI9_5_IRQn);
		break;
	}

	case 7: {
		__NVIC_EnableIRQ(EXTI9_5_IRQn);
		break;
	}

	case 8: {
		__NVIC_EnableIRQ(EXTI9_5_IRQn);
		break;
	}

	case 9: {
		__NVIC_EnableIRQ(EXTI9_5_IRQn);
		break;
	}

	case 10: {
		__NVIC_EnableIRQ(EXTI15_10_IRQn);
		break;
	}

	case 11: {
		__NVIC_EnableIRQ(EXTI15_10_IRQn);
		break;
	}

	case 12: {
		__NVIC_EnableIRQ(EXTI15_10_IRQn);
		break;
	}

	case 13: {
		__NVIC_EnableIRQ(EXTI15_10_IRQn);
		break;
	}

	case 14: {
		__NVIC_EnableIRQ(EXTI15_10_IRQn);
		break;
	}


	case 15: {
		__NVIC_EnableIRQ(EXTI15_10_IRQn);
		break;
	}

	default: {
		__NOP();
		break;
	}
	}

	/* 7.0 Volvemos a activar las interrupciones globales */
	__enable_irq();
}

/**/
__attribute__ ((weak)) void callback_extInt0(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt1(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt2(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt3(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt4(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt5(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt6(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt7(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt8(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt9(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt10(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt11(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt12(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt13(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt14(void){
	__NOP();
}

__attribute__ ((weak)) void callback_extInt15(void){
	__NOP();
}

/* ISR de la interrupción canal 0*/
void EXTI0_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_0 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR0){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR0;

		// llamamos al callback
		callback_extInt0();
	}
}

void EXTI1_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_1 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR1){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR1;

		// llamamos al callback
		callback_extInt1();
	}
}

void EXTI2_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_2 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR2){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR2;

		// llamamos al callback
		callback_extInt2();
	}
}

void EXTI3_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_3 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR3){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR3;

		// llamamos al callback
		callback_extInt3();
	}
}

void EXTI4_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_4 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR4){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR4;

		// llamamos al callback
		callback_extInt4();
	}
}

/* ISR de la interrupción canales 9_5
 * Observe que debe agregar totos los posibles casos, los cuales
 * son identificados por un bloque if() y el analisis de la bandera
 * (pending register -> EXTI_PR)
 */
void EXTI9_5_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_5 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR5){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR5;

		// llamamos al callback
		callback_extInt5();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_6 del GPIO_X
	else if(EXTI->PR & EXTI_PR_PR6){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR6;

		// llamamos al callback
		callback_extInt6();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_7 del GPIO_X
	else if(EXTI->PR & EXTI_PR_PR7){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR7;

		// llamamos al callback
		callback_extInt7();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_8 del GPIO_X
	else if(EXTI->PR & EXTI_PR_PR8){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR8;

		// llamamos al callback
		callback_extInt8();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_9 del GPIO_X
	else if(EXTI->PR & EXTI_PR_PR9){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR9;

		// llamamos al callback
		callback_extInt9();
	}

	else {
		__NOP();
	}
}

/* ISR de la interrupción canales 15_10
 * Observe que debe agregar totos los posibles casos, los cuales
 * son identificados por un bloque if() y el analisis de la bandera
 * (pending register -> EXTI_PR)
 */
void EXTI15_10_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_10
	if(EXTI->PR & EXTI_PR_PR10){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR10;

		// llamamos al callback
		callback_extInt10();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_11
	else if(EXTI->PR & EXTI_PR_PR11){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR11;

		// llamamos al callback
		callback_extInt11();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_12
	else if(EXTI->PR & EXTI_PR_PR12){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR12;

		// llamamos al callback
		callback_extInt12();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_13
	else if(EXTI->PR & EXTI_PR_PR13){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR13;

		// llamamos al callback
		callback_extInt13();
	}

	// Evaluamos si la interrupción que se lanzo corresponde al PIN_14
	else if(EXTI->PR & EXTI_PR_PR14){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR14;

		// llamamos al callback
		callback_extInt14();
	}
	
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_15
	else if(EXTI->PR & EXTI_PR_PR15){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR15;

		// llamamos al callback
		callback_extInt15();
	}
	
	else {
		__NOP();
	}

}

void configExternalTrigger(EXTEN_Config_t *extenConfig){

	//Se activa el acceso al SYSCFG:
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	//Asignamos el canal EXTI que corresponde al PIN_11 del puerto GPIO_X:
	if(extenConfig->pGPIOHandler->pGPIOx == GPIOA) {
		SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PA);

	} else if (extenConfig->pGPIOHandler->pGPIOx == GPIOB) {
		SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PB);

	} else if (extenConfig->pGPIOHandler->pGPIOx == GPIOC) {
		SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PC);

	} else if (extenConfig->pGPIOHandler->pGPIOx == GPIOD) {
		SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PD);

	} else if (extenConfig->pGPIOHandler->pGPIOx == GPIOE) {
		SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PE);

	} else if (extenConfig->pGPIOHandler->pGPIOx == GPIOH) {
		SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PH);

	} else {
		__NOP();
	}

	//Seleccionamos el tipo de flanco:
	if(extenConfig->edgeType == EXTERNAL_EVENT_FALLING_EDGE){
		EXTI->FTSR |= EXTI_FTSR_TR11;
	}

	else if(extenConfig->edgeType == EXTERNAL_EVENT_RISING_EDGE){
		EXTI->RTSR |= EXTI_RTSR_TR11;
	}

	else {
	__NOP();
	}

	//Activamos el evento del canal 11:
	EXTI->EMR |= EXTI_EMR_MR11;
}



