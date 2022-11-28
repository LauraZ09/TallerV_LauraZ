/*
 * KY006Buzzer.h
 *
 *  Created on: Month XX, 2022
 *      Author: namontoy
 */
#include "KY006Buzzer.h"

void Tone(PWM_Handler_t *ptrPwmHandler, uint32_t frequency){

	//Se actualiza el valor del período del PWM
	updateFrequency(ptrPwmHandler,2*frequency);

	//Se actualiza el duttyCicle para que quede en el 50%
	updateDuttyCycle(ptrPwmHandler,frequency);

	//Se activa la salida del PWM
	enableOutput(ptrPwmHandler);
	startPwmSignal(ptrPwmHandler);

}

void noTone(PWM_Handler_t *ptrPwmHandler){

	//Se desactiva la salida del PWM
	stopPwmSignal(ptrPwmHandler);
	disableOutput(ptrPwmHandler);

}

