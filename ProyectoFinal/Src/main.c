/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Laura Zuluaga
 * @brief          : Pruebas Proyecto
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "RccConfig.h"
#include "PWMDriver.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"

#define TIM_FREQ 40   //Frecuencia del Timer en MHz
#define T0H 16        //Este es el valor que se debe poner en el CCx para alcanzar 0.4u
#define T1H 32        //Este es el valor que se debe poner en el CCx para alcanzar 0.8u
#define T0L 34	      //Este es el valor que se debe poner en el CCx para alcanzar 0.85u
#define T1L 18        //Este es el valor que se debe poner en el CCx para alcanzar 0.45u
#define TRESET 2000   //Este es el valor que se debe poner en el CCx para alcanzar 50u

PWM_Handler_t handlerPWMTimer 	        = { 0 };  //Handler para el PWM (Timer)
BasicTimer_Handler_t handlerBlinkyTimer = { 0 };  //Handler para el BlinkyTimer
GPIO_Handler_t handlerBlinkyPin 	    = { 0 };  //Handler para el LED de estado
GPIO_Handler_t handlerMCO_2  			= { 0 };  //Handler para el PIN de salida del Clock
GPIO_Handler_t handlerPWMOutput 		= { 0 };  //Handler para la salida del PWM



uint8_t LED_data[180]; //En este arreglo se almacenan los números para la intensidad del RGB
					   //el arreglo es de 180 porque son 60 LEDS y 3 bytes por LED
uint16_t position;
uint8_t mask = 0B10000000;
uint8_t lastbit;
long double period;
uint16_t low_CCR1, low_ARR, high_CCR1, high_ARR, treset_ARR;

void Neopixel_setup(void);
void show_neopixels(void);
uint8_t neopixel_transmitting(void);



void initSystem(void); //Función para inicializar el sistema

int main(void)
{
	Neopixel_setup(); //Se cargan los valores
	setTo40M();
	initSystem();
	startPwmSignal(&handlerPWMTimer);
	enableOutput(&handlerPWMTimer);

	while (1)
	{
		//Primero se llena el arreglo con una secuencia consecutiva de verde, rojo y azul
		for (uint8_t i = 0; i < 180; i += 9) {
			LED_data[i] = 25;      //Se usan valores bajos para no gastar mucha corriente
			LED_data[i + 4] = 25;  //Se usan valores bajos para no gastar mucha corriente
			LED_data[i + 8] = 25;  //Se usan valores bajos para no gastar mucha corriente
		}

		show_neopixels();  //transmit the data to the neopixel strip.

		HAL_Delay(1000);



	}

	return 0;
}

void Neopixel_setup(void){

	period = 1 / TIM_FREQ;
	low_CCR1 = T0H;
	low_ARR = 50;
	high_CCR1 = T1H;
	high_ARR = 50;
	treset_ARR = 2000;
}

void show_neopixels(void){
	position = 0;           //set the interupt to start at first byte
	lastbit = 0;
	mask = 0B10000000;      //set the interupt to start at second bit
}

void initSystem(void) {

	//Se configura el LED de estado
	handlerBlinkyPin.pGPIOx 								= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber 		    = PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType 		    = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl	    = GPIO_PUPDR_NOTHING;

	//Se carga la configuración
	GPIO_Config(&handlerBlinkyPin);

	//Se configura el BlinkyTimer
	handlerBlinkyTimer.ptrTIMx 					= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 	= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 	= PWMTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 	= 250; //Update period= 1ms*250 = 250ms

	//Se carga la configuración del BlinkyTimer
	BasicTimer_Config(&handlerBlinkyTimer);

	//Se configura el Timer del PWM
	handlerPWMTimer.ptrTIMx            = TIM5;
	handlerPWMTimer.config.channel     = PWM_CHANNEL_1;
	handlerPWMTimer.config.prescaler   = PWMTIMER_SPEED_0_025us;
	handlerPWMTimer.config.periodo     = 50;
	handlerPWMTimer.config.duttyCicle  = 34;

	//Se carga la configuración
	pwm_Config(&handlerPWMTimer);

	//Se configura la salida del PWM
	handlerPWMOutput.pGPIOx 						      = GPIOA;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinNumber 		  = PIN_0;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinMode 	      = GPIO_MODE_ALTFN;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinOPType 		  = GPIO_OTYPE_PUSHPULL;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinSpeed 	      = GPIO_OSPEED_FAST;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinPuPdControl	  = GPIO_PUPDR_NOTHING;
	handlerPWMOutput.GPIO_PinConfig.GPIO_PinAltFunMode 	  = AF2; //AF02: TIM5_CH1;

	GPIO_Config(&handlerPWMOutput);

	handlerMCO_2.pGPIOx 						    = GPIOC;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinNumber 		= PIN_9;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinMode 		= GPIO_MODE_ALTFN;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinSpeed 		= GPIO_OSPEED_FAST;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerMCO_2.GPIO_PinConfig.GPIO_PinAltFunMode  = AF0;

	GPIO_Config(&handlerMCO_2);

}

void BasicTimer2_Callback(void) {
	GPIOxTooglePin(&handlerBlinkyPin);
	}

void BasicTimer5_Callback(void){

		if(position < sizeof(LED_data)){

			if(LED_data[position] & mask){
				TIM4->CCR1 = high_CCR1;
				TIM4->ARR  = high_ARR;
			}else{
				TIM4->CCR1 = low_CCR1;
				TIM4->ARR  = low_ARR;
			}
			if(mask == 1){
				mask = 0B10000000;
				position++;
			}else mask = mask >> 1;

		}else{
			TIM4->CCR1 = 0; //set to zero so that pin stays low
			TIM4->ARR = TRESET; //set to timing for reset LEDs
		}
}

