/*
 * SetTo100M.c
 *
 * Creado por: Laura Zuluaga
 * Descripción: A continuación se crea una función, la cual cambia la frecuencia a la que
 *				el micro trabaja
 */
#include "stm32f4xx.h"
#include "RccConfig.h"
//#include "system_stm32f4xx.h"

void setTo100M(void)
{
	//Se deben configurar el RCC y el PLL mientras el PLL está deshabilitado:

	//Primero se limpian los registros:
	//Primero se limpia el registro RCC->PLLCFGR
	//RCC->PLLCFGR = 0; QUÉ PASA CON LOS RESERVADOS SI HAGO ESTO
	//RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
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
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC; //Se elige el HSI

	/*3. Se configura los divisores y multiplicadores M, N, P: RCC_PLLCFGR*/

	/*3.a. Se configura el divisor M: VCOINPUT = PLLINPUT / M, este valor de VCOINPUT
	 * debe estar entre 1 y 2 MHz. En este caso se va a configurar M = 16, se debe cargar
	 * en el registro el valor 0b10000*/
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_4;

	/*3.b. Se configura el multiplicador N: VCOOUTPUT = VCOINPUT * N, este valor de
	 * VCOOUTPUT debe estar entre 100 y 432 MHz. En este caso se va a configurar N = 400,
	 * se debe cargar en el registro el valor 0b 0001 1001 0000, es decir, se ponen en 1 los bit
	 * RCC_PLLCFGR_PLLN4, RCC_PLLCFGR_PLLN8 y RCC_PLLCFGR_PLLN7 */
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_4;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_7;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_8;
	//RCC->PLLCFGR |= 0xC8 << RCC_PLLCFGR_PLLN_Pos ;


	/*3.c. Se configura el divisor P: PLLOUTPUT = VCOOUTPUT / P, este valor de
	 * PLLOUTPUT debe ser menor o igual a 100 MHz. En este caso se va a configurar P = 4.
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
	RCC->CFGR &= ~RCC_CFGR_PPRE2; //No se divide

	/*PPRE1: APB Low speed prescaler (APB1)
	 *Set and cleared by software to control APB low-speed clock division factor.
	 *Caution: The software has to set these bits correctly not to exceed 50 MHz on
	 *this domain. The clocks are divided with the new prescaler factor from 1 to 16
	 *AHB cycles after PPRE1 write.
	 *0xx: AHB clock not divided
	 *100: AHB clock divided by 2
	 *101: AHB clock divided by 4
	 *110: AHB clock divided by 8
	 *111: AHB clock divided by 16
	 *Como no debe pasar los 50 se divide entre 2*/
	//RCC->CFGR &= ~RCC_CFGR_PPRE1;
	RCC->CFGR |= RCC_CFGR_PPRE1_2;  //Se carga el valor 100, es decir, se divide entre 2

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
	RCC->CFGR &= ~RCC_CFGR_HPRE; //No se divide

	/*5. Se activan los pines para ver el reloj: MCO2[1:0]: Microcontroller clock output 2
	Set and cleared by software. Clock source selection may generate glitches on MCO2. It is
	highly recommended to configure these bits only after reset before enabling the external
	oscillators and the PLLs.
	00: System clock (SYSCLK) selected
	01: PLLI2S clock selected
	10: HSE oscillator clock selected
	11: PLL clock selected*/
	RCC->CFGR &= ~RCC_CFGR_MCO2; //Primero se limpian los bits
	RCC->CFGR |=  RCC_CFGR_MCO2; //Se ponen en 11 : PLL

	/*Ahora el prescaler:
	 * Set and cleared by software to configure the prescaler of the MCO2. Modification of this
	 prescaler may generate glitches on MCO2. It is highly recommended to change this
	 prescaler only after reset before enabling the external oscillators and the PLLs.
	 0xx: no division
	 100: division by 2
	 101: division by 3
	 110: division by 4
	 111: division by 5
	 */

	RCC->CFGR &= ~RCC_CFGR_MCO2PRE;   //Primero se limpian los bits
	RCC->CFGR |=  RCC_CFGR_MCO2PRE; //Se ponen en 111 : División x5, así queda en 20M la salida de este pin
	//RCC->CFGR |=  RCC_CFGR_MCO2PRE_1;
	//RCC->CFGR |=  RCC_CFGR_MCO2PRE_2;


	/*6.Se activa el PLL: RCC_CR
	 * 0: PLL OFF
	 * 1: PLL ON
	 */
	RCC->CR |= RCC_CR_PLLON;

	//Se hace un delay mientras el PLL está listo para usarse:
	while (!(RCC->CR & RCC_CR_PLLRDY));

	//Se cambia la flash para poder acceder a ella
	FLASH->ACR |= FLASH_ACR_LATENCY_3WS;

	/*7. Se selecciona el PLL como System Clock: RCC_CFGR
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

	//SystemCoreClockUpdate();// TODO
	//(void) SystemCoreClock;

}

