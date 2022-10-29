/*
 * ExtiDriver.h
 *
 *  Created on: May 10, 2022
 *      Author: namontoy
 */

#ifndef EXTIDRIVER_H_
#define EXTIDRIVER_H_

#include "stm32f4xx.h"
#include "GPIOxDriver.h"

#define EXTERNAL_INTERRUPT_FALLING_EDGE		0
#define EXTERNAL_INTERRUPT_RISING_EDGE		1

#define EXTERNAL_EVENT_FALLING_EDGE		0
#define EXTERNAL_EVENT_RISING_EDGE		1

typedef struct
{
	GPIO_Handler_t *pGPIOHandler;	// Canal ADC que será utilizado para la conversión ADC
	uint8_t			edgeType;		// Se selecciona si se desea un tipo de flanco subiendo o bajando
}EXTI_Config_t;

typedef struct
{
	GPIO_Handler_t *pGPIOHandler;	// Canal ADC que será utilizado para la conversión ADC
	uint8_t			edgeType;
}EXTEN_Config_t;

void configExternalTrigger(EXTEN_Config_t *extenConfig);
void extInt_Config(EXTI_Config_t *extiConfig);
void callback_extInt0(void);
void callback_extInt1(void);
void callback_extInt2(void);
void callback_extInt3(void);
void callback_extInt4(void);
void callback_extInt5(void);
void callback_extInt6(void);
void callback_extInt7(void);
void callback_extInt8(void);
void callback_extInt9(void);
void callback_extInt10(void);
void callback_extInt11(void);
void callback_extInt12(void);
void callback_extInt13(void);
void callback_extInt14(void);
void callback_extInt15(void);


#endif /* EXTIDRIVER_H_ */
