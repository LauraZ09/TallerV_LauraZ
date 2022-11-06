/*
 * SH1104OLED.c
 *
 *  Created on: Noviembre 06, 2022
 *      Author: Laura Zuluaga
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "SH1106OLED.h"
#include "I2CDriver.h"
#include <stm32f4xx.h>
#include "BasicTimer.h"

/* Antes de enviar un dataByte, ya sea de información o de instrucción, se debe enviar un  controlByte, estos bytes
 * tienen la siguiente estructura: C0 DC 0 0 0 0 0 0, en donde el bit C0 es para indicar si se está o no en el último
 * controlByte y el bit DC determina si se va a escribir un comando (0) o un dato a desplegar (1). Estos controlBytes son,
 * entonces, como la dirección de memoria del registro a escribir (por así decirlo).
 *
 * La OLED a usar cuenta con 7 páginas, cada página tiene 132 columnas y 8 filas. Para especificar en qué página me voy a
 * parar se debe enviar un comando con la dirección de la página. Lo mismo para la columna, lo mismo para la fila.
 *
 * COMANDOS:
 *
 * 1. Set Page Address:
 *
 *       A0  E  RW     D7 D6 D5 D4 D3 D2 D1 D0
 *       Estos bits    1  0  1  1  A3 A2 A1 A0   En las A se pone la página:
 *       están en                  0  0  0  0    página 0
 *       el byte de                0  0  0  1    página 1
 *       control				   0  0  1  0    página 2 y así sucesivamente
 *
 * 2. Set Display Start Line:
 *
 *
 *       A0  E  RW     D7 D6 D5 D4 D3 D2 D1 D0
 *       Estos bits    0  1  A5 A4 A3 A2 A1 A0   En las A se pone la página:
 *       están en            0  0  0  0  0  0    fila 0
 *       el byte de                0  0  0  1    fila 1
 *       control				   0  0  1  0    fila 2 y así sucesivamente
 *       			   1  1  1  1  1  1  1  1    fila 64
 *
 * 3. Set Column Address: para este hay dos comandos, uno para la parte alta y otro para la baja:
 *
 *       A0  E  RW     D7 D6 D5 D4 D3 D2 D1 D0
 *       Estos bits    0  0  0  1  A7 A6 A5 A4   En las A se pone la página:
 *       están en      0  0  0  0  A3 A2 A1 A0   fila 0
 *       el byte de
 *       control
 *
 *       			   A7 A6 A5 A4 A3 A2 A1 A0
 *       			   0  0	 0  0  0  0  0  0  columna 0
 *       			   y así hasta llegar a la última
 *       			   1  0  0  0  0  0  1  1  columna 131
 *
 * 4. Normal o invertido:
 *
 * 		 A0  E  RW     D7 D6 D5 D4 D3 D2 D1 D0
 *       Estos bits    1  0  1  0  0  1  1  D    En las D: 1 - NORMAL D: 0 - INVERTIDO
 *       están en
 *       el byte de
 *       control
 *
 * 5. Write Display Data: la dirección de la columna se aumenta automáticamente en 1 después de cada dato de display,
 * el dato se escribe en todo el byte.
 *
 * INICIALIZACIÓN:
 *
 *  1. Esperar más de 10us
 *  2. Comando 0xAE---- Set display Off
 *  3. Comando 0xD5---- initial Config (osc, prescaler)
 *  4. Comando 0x80---- initial Config (osc, prescaler)
 *  5. Comando 0xA8, 0x3F---Multiplex ratio
 *  6. Comando 0xD3, 0x00---Display Offset
 *  7. Comando 0x40---- Display Start Line
 *  8. Comando 0xad, 0x8b---Set charge Pump
 *  9. Comando 0xA1 ----set Segment Re Map
 *  10. Comando 0xC8
 *  11. 0xDA 0x12
 *  12. 0x81 0xbf --- contrast
 *  13. 0xd9 0x22 ---
 *  14. 0xdb 0x40
 *  15. 0x32
 *  16. 0xa6
 *  17. Clear display
 *  18. 0xAF (display on)
 *  19. 100 ms delay
 */

void sendCommandOLED(I2C_Handler_t *ptrHandlerI2C, uint8_t command){

	/* 1. Generamos la condición de satrt*/
	i2c_startTransaction(ptrHandlerI2C);

	/*2. Enviamos la dirección del esclavo y la indicación ESCRIBIR*/
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress,
			I2C_WRITE_DATA);

	//3. Se envía el control Byte
	i2c_sendDataByte(ptrHandlerI2C, OLED_CONTROLBYTE_COMMAND);

	/*3. Enviamos el comando que deseamos enviar*/
	i2c_sendDataByte(ptrHandlerI2C, command);

	/*5. Generamos la condición de Stop, para que el slave se detenga después de 1 byte*/
	i2c_stopTransaction(ptrHandlerI2C);
}

void sendByteOLED (I2C_Handler_t *ptrHandlerI2C, char *dataToSend){
	//Esta función toma los bytes de un arreglo de 8 y los envía, nótese que un arreglo representa un
	//solo carácter, en donde cada byte del arreglo es una columna del display. Son 8 Bytes ya que
	//se utiliza una fuente de 8x8 bits.

	/* 1. Generamos la condición de satrt*/
	i2c_startTransaction(ptrHandlerI2C);

	/*2. Enviamos la dirección del esclavo y la indicación ESCRIBIR*/
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress,
			I2C_WRITE_DATA);

	//3. Se envía el control Byte
	i2c_sendDataByte(ptrHandlerI2C, OLED_CONTROLBYTE_DATA);

	/*3. Enviamos la posición 0 del arreglo a enviar, nótese que el arreglo es un arreglo de arreglos*/
	i2c_sendDataByte(ptrHandlerI2C, *dataToSend);

	/*3. Posición 1*/
	i2c_sendDataByte(ptrHandlerI2C, *(dataToSend+1));

	/*3. Posición 2*/
	i2c_sendDataByte(ptrHandlerI2C, *(dataToSend+2));

	/*3. Posición 3*/
	i2c_sendDataByte(ptrHandlerI2C, *(dataToSend+3));

	/*3. Posición 4*/
	i2c_sendDataByte(ptrHandlerI2C, *(dataToSend+4));

	/*3. Posición 5*/
	i2c_sendDataByte(ptrHandlerI2C, *(dataToSend+5));
	
	/*3. Posición 6*/
	i2c_sendDataByte(ptrHandlerI2C, *(dataToSend+6));
	
	/*3. Posición 7*/
	i2c_sendDataByte(ptrHandlerI2C, *(dataToSend+7));

	/*5. Generamos la condición de Stop, para que el slave se detenga después de 1 byte*/
	i2c_stopTransaction(ptrHandlerI2C);
	
}

void sendBytesArray(I2C_Handler_t *ptrHandlerI2C, char **dataToSend){
	//Esta función envía un arreglo de arreglos jajajaja, es decir, un arreglo es un carácter,
	//un arreglo de arreglos son varios carácteres. En cada fila caben únicamente 16 carácteres, por
	//ello el arreglo ** se recorre solo hasta 15.

	for(uint8_t counterOLED = 0; counterOLED < 16; counterOLED++){
		sendByteOLED(ptrHandlerI2C, *dataToSend);
		dataToSend++; //Se pasa al siguiente carácter (arreglo)
	}
}

void setPageOLED(I2C_Handler_t *ptrHandlerI2C, uint8_t pageNumber){

	uint8_t pageCommand = 0;

	pageCommand = (0xB << 4) | pageNumber;       //De la lista de comandos de arriba se tiene que los bits del 4 al 7 deben ser 0xB
	sendCommandOLED(ptrHandlerI2C, pageCommand);

}

void initOLED(I2C_Handler_t *ptrHandlerI2C){
	delayms(1);
	sendCommandOLED(ptrHandlerI2C, 0xAE);
	sendCommandOLED(ptrHandlerI2C, 0xD5);
	sendCommandOLED(ptrHandlerI2C, 0x80);
	sendCommandOLED(ptrHandlerI2C, 0xA8);
	sendCommandOLED(ptrHandlerI2C, 0x3F);
	sendCommandOLED(ptrHandlerI2C, 0xD3);
	sendCommandOLED(ptrHandlerI2C, 0x00);
	sendCommandOLED(ptrHandlerI2C, 0x40);
	sendCommandOLED(ptrHandlerI2C, 0xAD);
	sendCommandOLED(ptrHandlerI2C, 0x8B);
	sendCommandOLED(ptrHandlerI2C, 0xA1);
	sendCommandOLED(ptrHandlerI2C, 0xC8);
	sendCommandOLED(ptrHandlerI2C, 0xDA);
	sendCommandOLED(ptrHandlerI2C, 0x12);
	sendCommandOLED(ptrHandlerI2C, 0x81);
	sendCommandOLED(ptrHandlerI2C, 0xBF);
	sendCommandOLED(ptrHandlerI2C, 0xD9);
	sendCommandOLED(ptrHandlerI2C, 0x22);
	sendCommandOLED(ptrHandlerI2C, 0xDB);
	sendCommandOLED(ptrHandlerI2C, 0x40);
	sendCommandOLED(ptrHandlerI2C, 0x32);
	sendCommandOLED(ptrHandlerI2C, 0xA6);
	sendCommandOLED(ptrHandlerI2C, 0xAF);
	sendCommandOLED(ptrHandlerI2C, 0x00);
	sendCommandOLED(ptrHandlerI2C, 0x10);
	sendCommandOLED(ptrHandlerI2C, 0x40);
	delayms(100);
	OLED_Clean(ptrHandlerI2C);
	OLED_Clean2(ptrHandlerI2C);
	//OLED_Clean(ptrHandlerI2C);
}

char characterA[8] = {0x00, 0b11111100, 0b00010010, 0b00010010, 0b00010010, 0b11111100, 0x00, 0x00}; //Está escrita en líneas verticales y horizontales
char blankSpace[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
char blackSpace[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char* OLED_A(void){ return characterA; }
char* OLEDNull(void){return blankSpace;}
char* OLEDBlack(void){return blackSpace;}

void OLED_Clean(I2C_Handler_t *ptrHandlerI2C){
	char* pageClean[16] = {OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),
			OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull()};
	for(uint8_t j = 0; j < 8; j++){
		setPageOLED(ptrHandlerI2C,j);
		sendBytesArray(ptrHandlerI2C,pageClean);

	}
}
void OLED_Clean2(I2C_Handler_t *ptrHandlerI2C){
	char* pageClean2[16] = {OLEDBlack(),OLEDBlack(),OLEDBlack(),OLEDBlack(),OLEDBlack(),OLEDBlack(),OLEDBlack(),OLEDBlack(),OLEDBlack(),OLEDBlack(),
			OLEDBlack(),OLEDBlack(),OLEDBlack(),OLEDBlack(),OLEDBlack(),OLEDBlack()};
	for(uint8_t j = 0; j < 8; j++){
		setPageOLED(ptrHandlerI2C,j);
		sendBytesArray(ptrHandlerI2C,pageClean2);

	}
}





