/*
 * CaptureFrecDriver.h
 *
 *  Created on: Month XX, 2022
 *      Author: namontoy
 */

#ifndef CAPTUREFRECDRIVER_H_
#define CAPTUREFRECDRIVER_H_

#include "stm32f4xx.h"

#define CAPTURE_CHANNEL_1		0
#define CAPTURE_CHANNEL_2		1
#define CAPTURE_CHANNEL_3		2
#define CAPTURE_CHANNEL_4		3

#define CAPTURE_RISING_EDGE		0
#define CAPTURE_FALLING_EDGE	1

#define CAPTURE_PRESCALER_1_1	0
#define CAPTURE_PRESCALER_2_1	1
#define CAPTURE_PRESCALER_4_1	2
#define CAPTURE_PRESCALER_8_1	3

#define CAPTURE_TIMER_SPEED_1us		16
#define CAPTURE_TIMER_SPEED_10us	160
#define CAPTURE_TIMER_SPEED_100us	1600
#define CAPTURE_TIMER_SPEED_1ms		16000
#define CAPTURE_TIMER_SPEED_4ms		64000


typedef struct
{

	uint8_t channel;          //Canal PWM relacionado con el TIMER
	uint8_t edgeSignal;	   	  //Selección del tipo de flanco que se desea capturar
	uint32_t timerSpeed;	 	  //A qué velocidad se incrementa el Timer
	uint32_t prescalerCapture; //A qué velocidad se incrementa el Timer

}Capture_Config_t;

typedef struct
{
	TIM_TypeDef		*ptrTIMx; //Timer al que está asociado la captura
	Capture_Config_t config;  //Configuración inicial del PWM
}Capture_Handler_t;

/*Prototipos de las funciones*/
void capture_Config(Capture_Handler_t *ptrCaptureHandler);
void CaptureFreqTimer2_Callback(void); /* Esta función debe ser sobre-escrita en el main para que el sistema funcione*/
void CaptureFreqTimer3_Callback(void);
void CaptureFreqTimer4_Callback(void);
void CaptureFreqTimer5_Callback(void);
void captureFreqInterruptModeEnable(Capture_Handler_t *ptrCaptureHandler);
void captureFreqClearCNT(Capture_Handler_t *ptrCaptureHandler);
uint32_t getPeriodFrec(Capture_Handler_t *ptrCaptureHandler);

#endif /*CAPTUREFRECDRIVER_H_ */
