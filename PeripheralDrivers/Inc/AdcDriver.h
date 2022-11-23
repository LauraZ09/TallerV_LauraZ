/*
 * AdcDriver.h
 *
 *  Created on: Month XX, 2022
 *      Author: namontoy
 */

#ifndef INC_ADCDRIVER_H_
#define INC_ADCDRIVER_H_

#include "stm32f4xx.h"

#define ADC_CHANNEL_0		0
#define ADC_CHANNEL_1		1
#define ADC_CHANNEL_2		2
#define ADC_CHANNEL_3		3
#define ADC_CHANNEL_4		4
#define ADC_CHANNEL_5		5
#define ADC_CHANNEL_6		6
#define ADC_CHANNEL_7		7
#define ADC_CHANNEL_8		8
#define ADC_CHANNEL_9		9
#define ADC_CHANNEL_10		10
#define ADC_CHANNEL_11		11
#define ADC_CHANNEL_12		12
#define ADC_CHANNEL_13		13
#define ADC_CHANNEL_14		14
#define ADC_CHANNEL_15		15
#define ADC_CHANNEL_16		16

#define ADC_RESOLUTION_12_BIT	0
#define ADC_RESOLUTION_10_BIT	1
#define ADC_RESOLUTION_8_BIT	2
#define ADC_RESOLUTION_6_BIT	3

#define ADC_ALIGNMENT_RIGHT		0
#define ADC_ALIGNMENT_LEFT		1

#define ADC_SAMPLING_PERIOD_3_CYCLES	0b000;
#define ADC_SAMPLING_PERIOD_15_CYCLES	0b001;
#define ADC_SAMPLING_PERIOD_28_CYCLES	0b010;
#define ADC_SAMPLING_PERIOD_56_CYCLES	0b011;
#define ADC_SAMPLING_PERIOD_84_CYCLES	0b100;
#define ADC_SAMPLING_PERIOD_112_CYCLES	0b101;
#define ADC_SAMPLING_PERIOD_144_CYCLES	0b110;
#define ADC_SAMPLING_PERIOD_480_CYCLES	0b111;

#define ADC_NUMBER_OF_CHANNELS_1		0b0000;
#define ADC_NUMBER_OF_CHANNELS_2		0b0001;
#define ADC_NUMBER_OF_CHANNELS_3		0b0010;
#define ADC_NUMBER_OF_CHANNELS_4		0b0011;
#define ADC_NUMBER_OF_CHANNELS_5		0b0100;
#define ADC_NUMBER_OF_CHANNELS_6		0b0101;
#define ADC_NUMBER_OF_CHANNELS_7		0b0110;
#define ADC_NUMBER_OF_CHANNELS_8		0b0111;
#define ADC_NUMBER_OF_CHANNELS_9		0b1000;
#define ADC_NUMBER_OF_CHANNELS_10		0b1001;
#define ADC_NUMBER_OF_CHANNELS_11		0b1010;
#define ADC_NUMBER_OF_CHANNELS_12		0b1011;
#define ADC_NUMBER_OF_CHANNELS_13		0b1100;
#define ADC_NUMBER_OF_CHANNELS_14		0b1101;
#define ADC_NUMBER_OF_CHANNELS_15		0b1110;
#define ADC_NUMBER_OF_CHANNELS_16		0b1111;

#define ADC_EXT_TRIG_DISABLED		0b00;
#define ADC_EXT_TRIG_RISING_EDGE	0b01;
#define ADC_EXT_TRIG_FALLING_EDGE	0b10;
#define ADC_EXT_TRIG_BOTH_EDGES		0b11;

#define ADC_EXT_EVENT_TIM1_CC1		0b0000;
#define ADC_EXT_EVENT_TIM1_CC2		0b0001;
#define ADC_EXT_EVENT_TIM1_CC3		0b0010;
#define ADC_EXT_EVENT_TIM2_CC2		0b0011;
#define ADC_EXT_EVENT_TIM2_CC3		0b0100;
#define ADC_EXT_EVENT_TIM2_CC4		0b0101;
#define ADC_EXT_EVENT_TIM3_CC1		0b0111;
#define ADC_EXT_EVENT_TIM4_CC4		0b1001;
#define ADC_EXT_EVENT_TIM5_CC1		0b1010
#define ADC_EXT_EVENT_TIM5_CC2		0b1011;
#define ADC_EXT_EVENT_TIM5_CC3		0b1100;
#define ADC_EXT_EVENT_EXTI_L11		0b1111;

typedef struct
{
	uint8_t		numberOfChannels;// Número de canales
	uint8_t     *channels;       // Canales ADC
	uint8_t		resolution;		 // Precisión con la que el ADC hace la adquisición del dato
	uint16_t	samplingPeriod;	 // Tiempo deseado para hacer la adquisición del dato
	uint8_t		dataAlignment;	 // Alineación a la izquierda o a la derecha
	uint16_t	adcData;		 //Dato de la conversión
}ADC_Config_t;

typedef struct
{
	uint8_t extEventTrigger;    //Trigger del evento externo por exti
	uint8_t extEventTypeSelect; //Tipo de evento externo
}ADC_Config_Event_t;



void adcConfigExternal(ADC_Config_Event_t *adcConfigEvent);
void adc_Config(ADC_Config_t *adcConfig);
void configAnalogPin(uint8_t adcChannel);
void adcComplete_Callback(void);
void startSingleADC(void);
void startContinousADC(void);
uint16_t getADC(void);

//void ADC_ConfigMultichannel (ADC_Config_t *adcConfig, uint8_t numeroDeCanales);

#endif /* INC_ADCDRIVER_H_ */
