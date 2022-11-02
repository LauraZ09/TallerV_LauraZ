/*
 * ADXL345.h
 *
 *  Created on: No, 2022
 *      Author: Laura Zuluaga
 */

#include "DriverRTC.h"

void enableRTC(void){

	//1. Se habilita la señal del reloj para el periférico RTC (Señal del reloj para el APB1)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;


	//3. Ahora, se debe habilitar la escritura para los registros del RTC:
	/*After system reset, the RTC registers are protected against parasitic write access with the
	 * DBP bit of the PWR power control register (PWR_CR). The DBP bit must be set to enable
	 * RTC registers write access.*/

	/*Bit 8 DBP: Disable backup domain write protection
	 In reset state, the RCC_BDCR register, the RTC registers (including the backup registers),
	 and the BRE bit of the PWR_CSR register, are protected against parasitic write access. This
	 bit must be set to enable write access to these registers.
	 0: Access to RTC and RTC Backup registers.
	 1: Access to RTC and RTC Backup registers.*/

	PWR->CR |= PWR_CR_DBP;

	//2. Habilitamos el LSE: se pone el bit LSEON del registro RCC_BDCR en 1 y esperamos a que esté listo:
	RCC->BDCR |= RCC_BDCR_LSEON;




	//4. Se selecciona el LSE como fuente del RTC: Esto es en el registro RCC_BDCR

	/* RTCSEL[1:0]: RTC clock source selection:
	 * Set by software to select the clock source for the RTC. Once the RTC clock source has been
	 selected, it cannot be changed anymore unless the Backup domain is reset. The BDRST bit
	 can be used to reset them.
	 00: No clock
	 01: LSE oscillator clock used as the RTC clock
	 10: LSI oscillator clock used as the RTC clock
	 11: HSE oscillator clock divided by a programmable prescaler (selection through the
	 RTCPRE[4:0] bits in the RCC clock configuration register (RCC_CFGR)) used as the RTC
	 clock*/

	RCC->BDCR |= RCC_BDCR_RTCSEL_0;
	//11. Se habilita el RTC:
	RCC->BDCR |= RCC_BDCR_RTCEN;

	//5. Se ingresan los valores de la llave para habilitar la escritura en los registros:

	/*The following steps are required to unlock the write protection on all the RTC registers
	 except for RTC_ISR[13:8], RTC_TAFCR, and RTC_BKPxR.
	 1. Write ‘0xCA’ into the RTC_WPR register.
	 2. Write ‘0x53’ into the RTC_WPR register.*/

	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;

	//6. Ahora se pone en modo inicialización:

	//6.a. Primero se pone en 1 el bit init del RTC_ISR, esto hace que se pueda hacer la actualización
	RTC->ISR |= RTC_ISR_INIT;

	//Se espera a que la inicialización esté lista:
	while(!(RTC->ISR & RTC_ISR_INITF))

	//6.c. Se ingresan los valores de los prescaler: 128 el asíncrono y 256 el síncrono, para 1 Hz
	RTC->PRER |= RTC_PRER_PREDIV_A; //Acá se está poniendo el asíncrono en 128. Se usa la máscara de CMSIS
	RTC->PRER |= 0xFF << RTC_PRER_PREDIV_S_Pos; //Se está poniendo el síncrono en 256

	//7. Se escriben valores de tiempo y fecha:

	//7. a. Se escribe el año: YT[3:0]: Year tens in BCD format (registro DR)
	RTC->DR |= RTC_DR_YT_1; //0010 = 2 en BCD
	RTC->DR |= RTC_DR_YU_1;

	//7. b. Se escribe el día de la semana
	RTC->DR |= RTC_DR_WDU_0;
	RTC->DR |= RTC_DR_WDU_1;

	//7. c. Se escribe el número del mes
	RTC->DR |= RTC_DR_MT;
	RTC->DR |= RTC_DR_MU_0;

	//7. d. Se escribe el número del día
	RTC->DR |= ~(0b1111 << RTC_DR_DT_Pos);
	RTC->DR |= RTC_DR_MU_1;

	//Se configura la hora:
	RTC->TR |= 0x130000;
	RTC->TR |= 0x5700;

	//8. Se pone el bit BYPSHAD en 1 para que se lean los datos del contador y no de los registros sombra:
	RTC->CR |= RTC_CR_BYPSHAD;


    //9. Se sale del modo inicialización:
	/*Exit the initialization mode by clearing the INIT bit. The actual calendar counter value is
	 then automatically loaded and the counting restarts after 4 RTCCLK clock cycles.*/
	RTC->ISR &= ~RTC_ISR_INIT;

	//10. Se desactiva la edición de los registros:
	PWR->CR &= ~PWR_CR_DBP;
}

uint8_t RTC_get_time(void){
	uint8_t Seconds = (((RTC->TR & 0x7f) >> 4)*10) + (RTC->TR & 0xf);
	return Seconds;
}





