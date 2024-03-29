/*
 * AdcDriver.c
 *
 *  Created on: Month XX, 2022
 *      Author: namontoy
 */
#include "AdcDriver.h"
#include "GPIOxDriver.h"

GPIO_Handler_t handlerAdcPin = {0};
uint16_t	adcRawData = 0;

void adc_Config(ADC_Config_t *adcConfig){
	/* 1. Configuramos el PinX para que cumpla la función de canal análogo deseado. */
	for(uint8_t i = 0; i <= (adcConfig->numberOfChannels); i++){
		configAnalogPin(*(adcConfig->channels+i));
	}

	/* 2. Activamos la señal de reloj para el periférico ADC1 (bus APB2)*/
	//Registro del RCC que nos activa la señal del reloj para el USART1
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	// Limpiamos los registros antes de comenzar a configurar
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;

	/* Comenzamos la configuración del ADC1 */
	/* 3. Resolución del ADC */
	switch(adcConfig->resolution){
	case ADC_RESOLUTION_12_BIT:
	{
		//Se ponen los Bits en 0b00
		ADC1->CR1 &= ~ADC_CR1_RES;
		break;
	}

	case ADC_RESOLUTION_10_BIT:
	{
		//Se ponen los Bits en 0b01
		ADC1->CR1 |= ADC_CR1_RES_0;
		break;
	}

	case ADC_RESOLUTION_8_BIT:
	{
		//Se ponen los Bits en 0b10
		ADC1->CR1 |= ADC_CR1_RES_1;

		break;
	}

	case ADC_RESOLUTION_6_BIT:
	{
		//Se ponen los Bits en 0b11
		ADC1->CR1 |= ADC_CR1_RES_0;
		ADC1->CR1 |= ADC_CR1_RES_1;
		break;
	}

	default:
	{
		//Por default se pone la resolución en 12bits: se ponen los Bits en 0b00
		ADC1->CR1 &= ~ADC_CR1_RES;
		break;
	}
	}

	/* 4. Configuramos el modo Scan como desactivado:
	 * 0: Scan mode disabled
	 * 1: Scan mode enabled */

	if((adcConfig->numberOfChannels + 1) == 1){
		ADC1->CR1 &= ~ADC_CR1_SCAN; //disabled
	}
	else{
		ADC1->CR1 |= ADC_CR1_SCAN;  //enabled
	}

	/* 5. Configuramos la alineación de los datos (derecha o izquierda):
	 * 0: Right alignment
	 * 1: Left alignment*/
	if(adcConfig->dataAlignment == ADC_ALIGNMENT_RIGHT){
		// Alineación a la derecha (esta es la forma "natural")
		ADC1->CR2 &= ~ADC_CR2_ALIGN;
	}
	else{
		// Alineación a la izquierda (para algunos cálculos matemáticos)
		ADC1->CR2 |= ADC_CR2_ALIGN;

	}

	/* 6. Desactivamos el "continuos mode"
	 * 0: Single conversion mode
	 * 1: Continuous conversion mode */
	ADC1->CR2 &= ~ADC_CR2_CONT;

	/* 7. Acá se debería configurar el sampling...*/
	//15 Ciclos 0b001 TODO TODO  cambiar a 100: 84 cycles

	ADC1->SMPR2 = 0;
	ADC1->SMPR1 = 0;

	//Primero limpio los bits:
	for(uint8_t i = 0; i <= (adcConfig->numberOfChannels); i++){

		if (*(adcConfig->channels+i) <= ADC_CHANNEL_9) {
			ADC1->SMPR2 |= adcConfig->samplingPeriod << (3 * (*(adcConfig->channels+i)));
		} else {
			ADC1->SMPR1 |= adcConfig->samplingPeriod << (3 * (*(adcConfig->channels+i)- 10));
		}
	}

	/* 8. Configuramos la secuencia y cuantos elementos hay en la secuencia */
	// Al hacerlo todo 0, estamos seleccionando solo 1 elemento en el conteo de la secuencia
	ADC1->SQR1 = 0;
	ADC1->SQR3 = 0;

	//Longitud de la secuencia
	ADC1->SQR1 |= adcConfig->numberOfChannels << ADC_SQR1_L_Pos;

	// Asignamos los canales a la secuencia
	for(uint8_t i = 0; i <= (adcConfig->numberOfChannels); i++){
		if (i <= 5) {
			ADC1->SQR3 |= (*(adcConfig->channels + i) << 5 * i);
		}

		else if((5 < i) & (i < 12)){
			ADC1->SQR2 |= (*(adcConfig->channels+i) << 5 * (i-6));
		}

		else {
			ADC1->SQR1 |= (*(adcConfig->channels+i) << 5 * (i-12));
		}
	}


	/* 9. Configuramos el preescaler del ADC en 2:1 (el mas rápido que se puede tener) */
	ADC->CCR |= ADC_CCR_ADCPRE;

	/* 10. Desactivamos las interrupciones globales */
	__disable_irq();


	/* 11. Activamos la interrupción debida a la finalización de una conversión EOC (CR1)*/
	// 0: EOC interrupt disabled
	// 1: EOC interrupt enabled. An interrupt is generated when the EOC bit is set.
	ADC1->CR1 |= ADC_CR1_EOCIE;

	//Se selecciona cuándo se hace la interrupción:
	//0: The EOC bit is set at the end of each sequence of regular conversions.
	//1: The EOC bit is set at the end of each regular conversion.
	ADC1->CR2 |= ADC_CR2_EOCS;

	//Se desactiva el DMA:
	//Primero en el DDS
	//0: No new DMA request is issued after the last transfer (as configured in the DMA controller)
	//1: DMA requests are issued as long as data are converted and DMA=1

	//Primero en el DDS
	//0: No new DMA request is issued after the last transfer (as configured in the DMA controller)
	//1: DMA requests are issued as long as data are converted and DMA=1
	ADC1->CR2 &= ~ADC_CR2_DDS;

	//Ahora en el DMA
	//0: DMA mode disabled
	//1: DMA mode enabled
	ADC1->CR2 &= ~ADC_CR2_DMA;

	/* 11a. Matriculamos la interrupción en el NVIC*/
	__NVIC_EnableIRQ(ADC_IRQn);
	
	/* 11b. Configuramos la prioridad para la interrupción ADC */

	/* 12. Activamos el modulo ADC */
	// 0: Disable ADC conversion and go to power down mode
	// 1: Enable ADC
	ADC1->CR2 |= ADC_CR2_ADON;

	/* 13. Activamos las interrupciones globales */
	__enable_irq();
}

/*
 * Esta función lanza la conversion ADC y si la configuración es la correcta, solo se hace
 * una conversion ADC.
 * Al terminar la conversion, el sistema lanza una interrupción y el dato es leido en la
 * función callback, utilizando la funciona getADC().
 * 
 * */
void startSingleADC(void){
	/* Desactivamos el modo continuo de ADC */
	//0: Single conversion mode
	//1: Continuous conversion mode
	ADC1->CR2 &= ~ADC_CR2_CONT;
	
	/* Limpiamos el bit del overrun (SR) */
	ADC1->SR &= ~ADC_SR_OVR;
	
	/* Iniciamos un ciclo de conversión ADC (CR2)*/
	/*SWSTART: Start conversion of regular channels
	This bit is set by software to start conversion and cleared by hardware as soon as the
	conversion starts.
	0: Reset state
	1: Starts conversion of regular channels
	Note: This bit can be set only when ADON = 1 */
	ADC1->CR2 |= ADC_CR2_SWSTART;
}

/* 
 * Esta función habilita la conversion ADC de forma continua.
 * Una vez ejecutada esta función, el sistema lanza una nueva conversion ADC cada vez que
 * termina, sin necesidad de utilizar para cada conversion el bit SWSTART del registro CR2.
 * Solo es necesario activar una sola vez dicho bit y las conversiones posteriores se lanzan
 * automaticamente.
 * */
void startContinousADC(void){

	/* Activamos el modo continuo de ADC */
	ADC1->CR2 |= ADC_CR2_CONT;

	/* Iniciamos un ciclo de conversión ADC */
	ADC1->CR2 |= ADC_CR2_SWSTART;

}

/* 
 * Función que retorna el ultimo dato adquirido por la ADC
 * La idea es que esta función es llamada desde la función callback, de forma que
 * siempre se obtiene el valor mas actual de la conversión ADC.
 * */
uint16_t getADC(void){
	// Esta variable es actualizada en la ISR de la conversión, cada vez que se obtiene
	// un nuevo valor.
	return adcRawData;
}

void adcConfigExternal(ADC_Config_Event_t *adcConfigEvent){
	//Se selecciona el flanco del evento

	//Primero se limpian los bits:
	ADC1->CR2 &= ~(0b11 << ADC_CR2_EXTEN_Pos);
	//Se selecciona el flanco:
	ADC1->CR2 |= ((adcConfigEvent -> extEventTrigger) << ADC_CR2_EXTEN_Pos);

	//Seleccionamos el tipo de evento externo

	//Primero se limpian los bits:
	ADC1->CR2 &= ~(0b1111 << ADC_CR2_EXTSEL_Pos);
	//Se selecciona el tipo de evento:
	ADC1->CR2 |= ((adcConfigEvent -> extEventTypeSelect) << ADC_CR2_EXTSEL_Pos);
}

/* 
 * Esta es la ISR de la interrupción por conversión ADC 
 */
void ADC_IRQHandler(void){
	// Evaluamos que se dio la interrupción por conversión ADC
	if(ADC1->SR & ADC_SR_EOC){
		// Leemos el resultado de la conversión ADC y lo cargamos en una variale auxiliar
		// la cual es utilizada en la función getADC()
		adcRawData = ADC1->DR;

		// Hacemos el llamado a la función que se ejecutará en el main
		adcComplete_Callback();
	}

}

/* Función debil, que debe ser sobreescrita en el main. */
__attribute__ ((weak)) void adcComplete_Callback(void){
	__NOP();
}

/* 
 * Con esta función configuramos que pin deseamos que funcione como canal ADC
 * Esta funcion trabaja con el GPIOxDriver, por lo cual requiere que se incluya
 * dicho driver.
 */
void configAnalogPin(uint8_t adcChannel) {

	// Con este switch seleccionamos el canal y lo configuramos como análogo.
	switch (adcChannel) {

	case ADC_CHANNEL_0: {
		// Es el pin PA0
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
		// Nota: Para el ejercicio inicial solo se necesita este canal, los demas
		// se necesitan para trabajos posteriores.
		break;
	}

	case ADC_CHANNEL_1: {
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
		break;
	}

	case ADC_CHANNEL_2: {
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
		break;
	}

	case ADC_CHANNEL_3: {
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
		break;
	}

	case ADC_CHANNEL_4: {
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
		break;
	}

	case ADC_CHANNEL_5: {
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
		break;
	}
	case ADC_CHANNEL_6: {
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_6;
		break;
	}
	case ADC_CHANNEL_7: {
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_7;
		break;
	}
	case ADC_CHANNEL_8: {
		//Es el pin PB0
		handlerAdcPin.pGPIOx 						= GPIOB;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
		break;
	}
	case ADC_CHANNEL_9: {
		handlerAdcPin.pGPIOx 						= GPIOB;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
		break;
	}
	case ADC_CHANNEL_10: {
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
		break;
	}
	case ADC_CHANNEL_11: {
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
		break;
	}
	case ADC_CHANNEL_12: {
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
		break;
	}
	case ADC_CHANNEL_13: {
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
		break;
	}
	case ADC_CHANNEL_14: {
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
		break;
	}
	case ADC_CHANNEL_15: {
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
		break;
	}
	default: { //CANAL0 POR DEFAULT
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;

		break;
	}

	}
	// Despues de configurar el canal adecuadamente, se define este pin como Analogo y se
	// carga la configuración con el driver del GPIOx
	handlerAdcPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
	GPIO_Config(&handlerAdcPin);
}
