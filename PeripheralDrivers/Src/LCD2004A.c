/*
 * ADXL345.h
 *
 *  Created on: Octubre 29, 2022
 *      Author: Laura Zuluaga
 */

#include "I2CDriver.h"
#include "BasicTimer.h"
#include "LCD2004A.h"
#include <stdint.h>
#include <stdio.h>


void initLCD(I2C_Handler_t *ptrHandlerI2C){
	//Inicialización en modo de 4bits:

	//1. Se debe esperar más de 40 ms, entonces esperamos 50 ms:
	delayms(50);

	//2. Se debe poner el Bit RS: 0, RW: 0 (es decir se va a escribir en el registro IR) y E:0 LED:0 y se envía el dato 0x03
	sendCommandLCD(ptrHandlerI2C, 0x00);
	sendCommandLCD(ptrHandlerI2C, 0x03);

	//3. Se esperan más de 4.1 ms, esperamos 5 ms:
	delayms(5);

	//4. Se debe poner el Bit RS: 0, RW: 0 (es decir se va a escribir en el registro IR) y E:0 LED:0 y se envía el dato 0x03
	sendCommandLCD(ptrHandlerI2C, 0x00);
	sendCommandLCD(ptrHandlerI2C, 0x03);

	//5. Se esperan más de 100 us, esperamos 150us:
	delayms(1);

	//6. Se envía por tercera vez el dato 0x03:
	sendCommandLCD(ptrHandlerI2C, 0x00);
	sendCommandLCD(ptrHandlerI2C, 0x03);

	//7. Se esperan más de 100 us, esperamos 150us:
	delayms(1);

	//8. Ahora sí, se elige el modo de 4 bits:
	sendCommandLCD(ptrHandlerI2C, 0x00);
	sendCommandLCD(ptrHandlerI2C, 0x02);
	//sendCommandLCD(ptrHandlerI2C, 0x14);

	//9. Se configuran el número de filas que se quieren usar y el tamaño de la fuente: para todas las líneas N: 1 (bit 7)
	// fuente 5x8: F: 0 (bit 6)
	sendCommandLCD(ptrHandlerI2C, 0x28); //00101000
	//sendCommandLCD(ptrHandlerI2C, 0x06); //00001000
	sendCommandLCD(ptrHandlerI2C, 0x08); //00001000

	//sendCommandLCD(ptrHandlerI2C, 0x0F);
	//10. Se apaga el display
	sendCommandLCD(ptrHandlerI2C, 0x01);

	sendCommandLCD(ptrHandlerI2C, 0x06); //00001000

	//10. Se apaga el display
	//sendCommandLCD(ptrHandlerI2C, 0x01);


	//10. Se apaga el display
	sendCommandLCD(ptrHandlerI2C, 0x0C);

}//Inicialización terminada

void sendByteLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToSend){
	/* 1. Generamos la condición de satrt*/
	i2c_startTransaction(ptrHandlerI2C);
	/*2. Enviamos la dirección del esclavo y la indicación ESCRIBIR*/
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress,
			I2C_WRITE_DATA);
	/*3. Enviamos el valor que deseamos escribir en el registro deseado*/
	i2c_sendDataByte(ptrHandlerI2C, dataToSend);
	/*5. Generamos la condición de Stop, para que el slave se detenga después de 1 byte*/
	i2c_stopTransaction(ptrHandlerI2C);
}
void sendCommandLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t command){
	uint8_t commandH = (command & 0xf0);
	uint8_t commandL = (command << 4);
	//Primero se debe enviar la parte alta (H) del dato, no se debe olvidar la primera parte del registro,
	//correspondiente a los bits RS, RW, LED, E:
	sendByteLCD(ptrHandlerI2C,(commandH | LED_ON | ENABLE_ON)); //Primero se carga el comando
	sendByteLCD(ptrHandlerI2C,(commandH | LED_ON ));			//Luego, se escribe en el registro
	//Enviamos la parte baja del comando (L):
	sendByteLCD(ptrHandlerI2C, (commandL | LED_ON | ENABLE_ON )); //Primero se carga el comando
	sendByteLCD(ptrHandlerI2C, (commandL | LED_ON ));//Luego, se escribe en el registro

	delayms(1);
}

void displayDataLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t data){
	uint8_t dataH = (data & 0xf0);
	uint8_t dataL = (data << 4);
	//Primero se debe enviar la parte alta (H) del dato, no se debe olvidar la primera parte del registro,
	//correspondiente a los bits RS, RW, LED, E:
	sendByteLCD(ptrHandlerI2C, (dataH | LED_ON | DISPLAY_DATA | ENABLE_ON )); //Primero se carga el DATO
	sendByteLCD(ptrHandlerI2C, (dataH | LED_ON | DISPLAY_DATA)); //Luego, se escribe en el registro
	//Enviamos la parte baja del comando (L):
	sendByteLCD(ptrHandlerI2C, (dataL | LED_ON | ENABLE_ON |DISPLAY_DATA )); //Primero se carga el DATO
	sendByteLCD(ptrHandlerI2C, (dataL | LED_ON )); //Luego, se escribe en el registro
}

void clearDisplayLCD(I2C_Handler_t *ptrHandlerI2C){

	sendCommandLCD(ptrHandlerI2C, 0b1);
	//delayms(100);
}

void printStringLCD(I2C_Handler_t *ptrHandlerI2C,char* string){
	uint8_t i = 0; //Variable para recorrer el arreglo
	while(*(string + i) != '\0'){
		displayDataLCD(ptrHandlerI2C, *(string + i));
		i++;

		if (i == 20){
			moveCursorToLCD(ptrHandlerI2C, 0x40);
		}
	}
	//delayms(100);
}

void returnHomeLCD(I2C_Handler_t *ptrHandlerI2C){

	sendCommandLCD(ptrHandlerI2C, 0b10);
	//delayms(100);
}

void moveCursorToLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t position){
	sendCommandLCD(ptrHandlerI2C, 0x80 + position);
	//delayms(100);
}
