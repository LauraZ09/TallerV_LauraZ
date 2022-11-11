/*
 * BasicTimer.h
 *
 *  Created on: Apr 18, 2022
 *      Author: namontoy
 */

#ifndef INC_BASICTIMER_H_
#define INC_BASICTIMER_H_

#include "stm32f4xx.h"

#define BTIMER_MODE_UP		0
#define BTIMER_MODE_DOWN	1

#define BTIMER_SPEED_1us	15
#define BTIMER_SPEED_10us	159
#define BTIMER_SPEED_100us	1599
#define BTIMER_SPEED_1ms	15999

//El bus que alimenta los relojes máximo va a 50M
#define BTIMER_SPEED_100M_20ns		0
#define BTIMER_SPEED_100M_40ns		1
#define BTIMER_SPEED_100M_80ns		3
#define BTIMER_SPEED_100M_800ns		39
#define BTIMER_SPEED_100M_8us		399
#define BTIMER_SPEED_100M_80us		3999
#define BTIMER_SPEED_100M_100us		39999
#define BTIMER_SPEED_100M_1ms		49999

/* Estructura que contiene la configuración mínima necesaria para el manejo del Timer.*/
typedef struct
{
	uint8_t		TIMx_mode; 		// Up or dowm
	uint32_t	TIMx_speed;		// A qué velocidad se incrementa el Timer
	uint32_t	TIMx_period;	// Valor en ms del periodo del Timer
	uint8_t		TIMx_interruptEnable;	// Activa o desactiva el modo interrupción del timer.
}BasicTimer_Config_t;

/* Handler para el Timer*/
typedef struct
{
	TIM_TypeDef			*ptrTIMx;
	BasicTimer_Config_t	TIMx_Config;
}BasicTimer_Handler_t;

void BasicTimer_Config(BasicTimer_Handler_t *ptrBTimerHandler);
void BasicTimer2_Callback(void); /* Esta función debe ser sobre-escrita en el main para que el sistema funcione*/
void BasicTimer3_Callback(void);
void BasicTimer4_Callback(void);
void BasicTimer5_Callback(void);
void delayus(uint32_t delayTime);
void delayms(uint32_t delayTime);

#endif /* INC_BASICTIMER_H_ */
