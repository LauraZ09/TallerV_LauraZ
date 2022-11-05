/*
 * ADXL345.h
 *
 *  Created on: No, 2022
 *      Author: Laura Zuluaga
 */

#include "DriverRTC.h"
#include <stdio.h>

char WeekDay[32] = {0};

void enableRTC(Hour_and_Date_Config_t *Hour_and_Date_Config){

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

	//4. Habilitamos el LSE: se pone el bit LSEON del registro RCC_BDCR en 1 y esperamos a que esté listo:
	RCC->BDCR |= RCC_BDCR_LSEON;
	while(!(RCC->BDCR & RCC_BDCR_LSERDY));

	//5. Se selecciona el LSE como fuente del RTC: Esto es en el registro RCC_BDCR

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

	//2. Se habilita el RTC:
	RCC->BDCR |= RCC_BDCR_RTCEN;

	//6. Se ingresan los valores de la llave para habilitar la escritura en los registros:

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
	while(!(RTC->ISR & RTC_ISR_INITF));

	//6.c. Se ingresan los valores de los prescaler: 128 el asíncrono y 256 el síncrono, para 1 Hz
	RTC->PRER |= RTC_PRER_PREDIV_A; //Acá se está poniendo el asíncrono en 128. Se usa la máscara de CMSIS
	RTC->PRER |= 0xFF << RTC_PRER_PREDIV_S_Pos; //Se está poniendo el síncrono en 256

	//7. Se escriben valores de tiempo y fecha por defecto:
	//Primero se apaga el RTC y se limpian los registros:
	RCC->BDCR &= ~RCC_BDCR_RTCEN;
	RTC->DR = 0;
	RTC->TR = 0;

	//7. a. Se escribe el año: YT[3:0]: Year tens in BCD format (registro DR)
	RTC->DR |= (Decimal_To_BCD(Hour_and_Date_Config->Year)) << RTC_DR_YU_Pos;


	//7. b. Se escribe el día de la semana
	RTC->DR |= Hour_and_Date_Config->DayOfWeek << RTC_DR_WDU_Pos;

	//7. c. Se escribe el número del mes
	RTC->DR |= (Decimal_To_BCD(Hour_and_Date_Config->Month)) << RTC_DR_MU_Pos;

	//7. d. Se escribe el número del día
	RTC->DR |= (Decimal_To_BCD(Hour_and_Date_Config->NumberOfDay));

	//8. Se configura la hora: por defecto: 00:00:00

	//8.a. Se le pone el formato 24 HORAS
	RTC->CR &= ~RTC_CR_FMT;

	//8.b. Se configuran las horas, primero se deben convertir las horas ingresadas a BCD.
	RTC->TR |= (Decimal_To_BCD(Hour_and_Date_Config->Hours)) << RTC_TR_HU_Pos;

	//8.c.Se escriben los minutos:
	RTC->TR |= (Decimal_To_BCD(Hour_and_Date_Config->Minutes)) << RTC_TR_MNU_Pos;

	//8.d.Se escriben los segundos:
	RTC->TR |= (Decimal_To_BCD(Hour_and_Date_Config->Seconds));

	//9. Se habilita el RTC para poder hacer actualizaciones
	RCC->BDCR |= RCC_BDCR_RTCEN;


	//10. Se pone el bit BYPSHAD en 1 para no tener que bajar la bandera después de cada lectura:
	RTC->CR |= RTC_CR_BYPSHAD;

    //11. Se sale del modo inicialización:
	/*Exit the initialization mode by clearing the INIT bit. The actual calendar counter value is
	 then automatically loaded and the counting restarts after 4 RTCCLK clock cycles.*/
	RTC->ISR &= ~RTC_ISR_INIT;

	//12. Se desactiva la edición de los registros:
	PWR->CR &= ~PWR_CR_DBP;
}

uint8_t RTC_Get_Seconds(void){
	uint8_t Seconds = BCD_To_Decimal(RTC->TR & 0x7F);
	return Seconds;
}

uint8_t RTC_Get_Minutes(void){
	uint8_t Minutes = BCD_To_Decimal((RTC->TR & 0x7F00) >> RTC_TR_MNU_Pos);
	return Minutes;
}

uint8_t RTC_Get_Hours(void){
	uint8_t Hours = BCD_To_Decimal((RTC->TR & 0x3F0000) >> RTC_TR_HU_Pos);
	return Hours;
}

uint8_t RTC_Get_Date(void){
	uint8_t Date = BCD_To_Decimal(RTC->DR & 0x3F);
	return Date;
}

uint8_t RTC_Get_Month(void){
	uint8_t Month = BCD_To_Decimal((RTC->DR & 0x1F00) >> RTC_DR_MU_Pos);
	return Month;
}

uint8_t RTC_Get_Year(void){
	uint8_t Year = BCD_To_Decimal((RTC->DR & 0xFF0000) >> RTC_DR_YU_Pos);
	return Year;
}

char* RTC_Get_WeekDay(void){
	uint8_t Day = (RTC->DR & 0xE000) >> RTC_DR_WDU_Pos;

		switch (Day){

		case 1: {
			sprintf(WeekDay, "Dia: Lunes");
			break;}

		case 2: {
			sprintf(WeekDay, "Dia: Martes");
			break;}

		case 3: {
			sprintf(WeekDay, "Dia: Miercoles");
			break;}

		case 4:  {
			sprintf(WeekDay, "Dia: Jueves");
			break;}

		case 5:  {
			sprintf(WeekDay, "Dia: Viernes");
			break;}

		case 6:  {
			sprintf(WeekDay, "Dia: Sabado");
			break;}

		case 7:  {
			sprintf(WeekDay, "Dia: Domingo");
			break;}
		}

		return WeekDay;
}


uint16_t Decimal_To_BCD(uint16_t decimalValue){
	uint8_t BCDValue = ((decimalValue/10) << 4) | (decimalValue%10);
	return BCDValue;
}

uint8_t BCD_To_Decimal(uint8_t BCDValue){
	uint8_t DecimalValue = (BCDValue/16*10) + (BCDValue%16);
	return DecimalValue;
}






