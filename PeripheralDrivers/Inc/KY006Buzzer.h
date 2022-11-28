/*
 * KY006Buzzer.h
 *
 *  Created on: Month XX, 2022
 *      Author: namontoy
 */

#ifndef INC_KY006Buzzer_H_
#define INC_KY006Buzzer_H_

#include "stm32f4xx.h"
#include "PwmDriver.h"

void Tone(PWM_Handler_t *ptrPwmHandler, uint32_t frequency);
void noTone(PWM_Handler_t *ptrPwmHandler);

#endif /* INC_KY006Buzzer_H_ */
