/*
 * ADXL345.h
 *
 *  Created on: Octubre 29, 2022
 *      Author: Laura Zuluaga
 */


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "I2CDriver.h"
#include "I2CDriver.c"
#include "BasicTimer.h"
#include "BasicTimer.c"
#include <stm32f4xx.h>

//Primero se crea el handler de un timer para poder hacer los delays
BasicTimer_Handler_t handlerDelayTimer;

//Se crea una variable auxiliar para los Delay


//Se pone la cabecera de la función que inicializa el timer
void initDelayTimer(void);


void initLCD(I2C_Handler_t *ptrHandlerI2C){

	//Primero se iniciliza el timer para hacer los delays
	initDelayTimer();

	//Inicialización en modo de 4bits:

	//1. Se debe esperar más de 40 ms, entonces esperamos 50 ms:
	delayms(50);

	//2. Se debe poner el Bit RS: 0, RW: 0 (es decir se va a escribir en el registro IR) y E:0 LED:0 y se envía el dato 0x03
	i2c_sendDataByte(ptrHandlerI2C, (0x03 << 4));

	//3. Se esperan más de 4.1 ms, esperamos 5 ms:
	delayms(5);

	//4. Se debe poner el Bit RS: 0, RW: 0 (es decir se va a escribir en el registro IR) y E:0 LED:0 y se envía el dato 0x03
	i2c_sendDataByte(ptrHandlerI2C, (0x03 << 4));

	//5. Se esperan más de 100 us, esperamos 150us:
	delayus(150);

	//6. Se envía por tercera vez el dato 0x03:
	i2c_sendDataByte(ptrHandlerI2C, (0x03 << 4));

	//7. Se esperan más de 100 us, esperamos 150us:
	delayus(150);

	//8. Ahora sí, se elige el modo de 4 bits:
	i2c_sendDataByte(ptrHandlerI2C, (0x02 << 4));

	//9. Se configuran el número de filas que se quieren usar y el tamaño de la fuente: para todas las líneas N: 1 (bit 7)
	// fuente 5x8: F: 0 (bit 6)
	i2c_sendDataByte(ptrHandlerI2C, (0x02 << 4));
	i2c_sendDataByte(ptrHandlerI2C, 0x80); //0x80 = 1000 0000

	//10. Se apaga el display
	i2c_sendDataByte(ptrHandlerI2C, 0x00);
	i2c_sendDataByte(ptrHandlerI2C, 0x80);

	//11. Se limpia el display:
	i2c_sendDataByte(ptrHandlerI2C, 0x00);
	i2c_sendDataByte(ptrHandlerI2C, (0x01 << 4));

	//12. Se activa el entry mode: se pone que el cursor se mueva de izq a derecha y que la info de la pantalla no se mueva
	i2c_sendDataByte(ptrHandlerI2C, 0x00);
	i2c_sendDataByte(ptrHandlerI2C, 0x60); 	//0110 000
}//Inicialización terminada






