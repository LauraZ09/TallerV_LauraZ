/*
 * SetTo40M.c
 *
 * Creado por: Laura Zuluaga
 * Descripción: A continuación se crea una función, la cual cambia la frecuencia a la que
 *				el micro trabaja
 */
#include "stm32f4xx.h"
#include "RccConfig.h"
//#include "system_stm32f4xx.h"

void setTo40M(void)
{
	//Se deben configurar el RCC y el PLL mientras el PLL está deshabilitado:

	//Primero se limpian los registros:
	//Primero se limpia el registro RCC->PLLCFGR TODO PUEDO LIMPIAR TAMBIÉN EL Q]
	//RCC->PLLCFGR = 0; QUÉ PASA CON LOS RESERVADOS SI HAGO ESTO
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;

	/*0. Se deshabilita el PLL para hacer la configuración: RCC_CR
	 * 0: PLL OFF
	 * 1: PLL ON
	 */
	RCC->CR &= ~RCC_CR_PLLON;

	/*1. Se enciende el HSI(este será el que alimenta el PLL): RCC_CR
	 * 0: HSI oscillator OFF
	 * 1: HSI oscillator ON
	 */
	RCC->CR |= RCC_CR_HSION;

	/*2. Se configura la fuente del PLL: RCC_PLLCFGR
	 * 0: HSI clock selected as PLL and PLLI2S clock entry
	 * 1: HSE oscillator clock selected as PLL and PLLI2S clock entry
	 */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;

	/*3. Se configura los divisores y multiplicadores M, N, P: RCC_PLLCFGR*/

	/*3.a. Se configura el divisor M: VCOINPUT = PLLINPUT / M, este valor de VCOINPUT
	 * debe estar entre 1 y 2 MHz. En este caso se va a configurar M = 16, se debe cargar
	 * en el registro el valor 0b10000*/
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_4;

	/*3.b. Se configura el multiplicador N: VCOOUTPUT = VCOINPUT * M, este valor de
	 * VCOOUTPUT debe estar entre 100 y 432 MHz. En este caso se va a configurar N = 160,
	 * se debe cargar en el registro el valor 0b10100000, es decir, se ponen en 1 los bit
	 * RCC_PLLCFGR_PLLN5 y RCC_PLLCFGR_PLLN7 */
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_5;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_7;

	/*3.c. Se configura el divisor P: PLLOUTPUT = VCOOUTPUT / P, este valor de
	 * PLLOUTPUT debe ser menor a 100 MHz. En este caso se va a configurar P = 4.
	 * 00: PLLP = 2
	 * 01: PLLP = 4
	 * 10: PLLP = 6
	 * 11: PLLP = 8 */
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0;

	/*4. Se configuran los prescaler de los buses: AHB, APB1 y APB2: */


	/* PPRE2: APB high-speed prescaler (APB2)
	 * Set and cleared by software to control APB high-speed clock division factor.
	 * Caution: The software has to set these bits correctly not to exceed 100 MHz on
	 * this domain. The clocks are divided with the new prescaler factor from 1 to 16
	 * AHB cycles after PPRE2 write.
	 * 0xx: AHB clock not divided
	 * 100: AHB clock divided by 2
	 * 101: AHB clock divided by 4
	 * 110: AHB clock divided by 8
	 * 111: AHB clock divided by 16
	 */
	RCC->CFGR &= ~RCC_CFGR_PPRE2;

	/*PPRE1: APB Low speed prescaler (APB1)
	 *Set and cleared by software to control APB low-speed clock division factor.
	 *Caution: The software has to set these bits correctly not to exceed 50 MHz on
	 *this domain. The clocks are divided with the new prescaler factor from 1 to 16
	 *AHB cycles after PPRE1 write.
	 *0xx: AHB clock not divided
	 *100: AHB clock divided by 2
	 *101: AHB clock divided by 4
	 *110: AHB clock divided by 8
	 *111: AHB clock divided by 16*/
	RCC->CFGR &= ~RCC_CFGR_PPRE1;

	/*HPRE: AHB prescaler
	 * Set and cleared by software to control AHB clock division factor.
	 * Caution: The clocks are divided with the new prescaler factor from 1 to 16 AHB cycles after
	 HPRE write.
	 0xxx: system clock not divided
	 1000: system clock divided by 2
	 1001: system clock divided by 4
	 1010: system clock divided by 8
	 1011: system clock divided by 16
	 1100: system clock divided by 64
	 1101: system clock divided by 128
	 1110: system clock divided by 256
	 1111: system clock divided by 512
	 */
	RCC->CFGR &= ~RCC_CFGR_HPRE;

	//TODO SE SUPONE QUE YA DEBERÍA FUNCIONAR

	/*5.Se activa el PLL: RCC_CR
	 * 0: PLL OFF
	 * 1: PLL ON
	 */
	RCC->CR |= RCC_CR_PLLON;
	//Se hace un delay mientras el PLL está listo para usarse:
	while (!(RCC->CR & RCC_CR_PLLRDY));

	/*6. Se selecciona el PLL como System Clock: RCC_CFGR
	 *00: HSI oscillator selected as system clock
	 *01: HSE oscillator selected as system clock
	 *10: PLL selected as system clock
	 *11: not allowed*/
	//Se limpian los bits en el registro RCC_CFGR:
	RCC->CFGR &= ~RCC_CFGR_SW;
	//Se selecciona el system clock:
	RCC->CFGR |= RCC_CFGR_SW_1;
	//Se hace un delay mientras el PLL está listo para usarse como system clock
	while (!(RCC->CFGR & RCC_CFGR_SWS_1));

	//SystemCoreClockUpdate(); TODO
	//(void) SystemCoreClock;

}

