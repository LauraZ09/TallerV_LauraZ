/*
 * DriverRTC.h
 *
 *  Created on: Octubre 29, 2022
 *      Author: Laura Zuluaga
 */


#ifndef RTC_H_
#define RTC_H_

#include "stm32f4xx.h"

void enableRTC(void);
uint8_t RTC_get_time(void);

#endif /*RTC_H_ */
