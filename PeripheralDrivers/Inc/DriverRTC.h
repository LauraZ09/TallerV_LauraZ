/*
 * DriverRTC.h
 *
 *  Created on: Octubre 29, 2022
 *      Author: Laura Zuluaga
 */


#ifndef RTC_H_
#define RTC_H_

#include "stm32f4xx.h"

#define PM_FORMAT 0b1;
#define AM_FORMAT 0b0;

typedef struct
{
	uint16_t Hours;
	uint16_t Minutes;
	uint16_t Seconds;
	uint16_t DayOfWeek;
	uint16_t NumberOfDay;
	uint16_t Month;
	uint16_t Year;
	uint16_t PM_AM_Format;

}Hour_and_Date_Config_t;

void enableRTC(Hour_and_Date_Config_t *Hour_and_Date_Config);
uint8_t RTC_Get_Seconds(void);
uint8_t RTC_Get_Minutes(void);
uint8_t RTC_Get_Hours(void);
uint16_t Decimal_To_BCD(uint16_t decimalValue);
uint8_t BCD_To_Decimal(uint8_t BCDValue);
uint8_t RTC_Get_Date(void);
uint8_t RTC_Get_Month(void);
uint8_t RTC_Get_Year(void);
char* RTC_Get_WeekDay(void);

#endif /*RTC_H_ */
