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

//La siguiente función envía un comando
void sendCommandOLED(I2C_Handler_t *ptrHandlerI2C, uint8_t command){

	/* 1. Generamos la condición de satrt*/
	i2c_startTransaction(ptrHandlerI2C);

	/*2. Enviamos la dirección del esclavo y la indicación ESCRIBIR*/
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress,
			I2C_WRITE_DATA);

	//3. Se envía el control Byte, este es como si fuera la dirección del registro
	i2c_sendDataByte(ptrHandlerI2C, OLED_CONTROLBYTE_COMMAND);

	/*3. Enviamos el comando que deseamos enviar*/
	i2c_sendDataByte(ptrHandlerI2C, command);

	/*5. Generamos la condición de Stop, para que el slave se detenga después de 1 byte*/
	i2c_stopTransaction(ptrHandlerI2C);
}

//Con esta función se envía un arreglo de bytes de información para desplegar en pantalla, se
//debe tener en cuenta que un byte es una sola columna (una sola línea), así, para formar
//un carácter completo se debe tener un arreglo de bytes, con la cantidad de columnas que se quiera de tamaño
//(fontsize), en cada byte se escribe los bits de la pantalla que se quieren iluminar de la columna y así se
//forman líneas
void sendByteOLED (I2C_Handler_t *ptrHandlerI2C, char *dataToSend){
	//En este caso tomamos 8 Bytes ya que se desea utilizar una fuente de 5x6 con espaciamiento entre carácteres
	//de 3 bits.

	/* 1. Generamos la condición de satrt*/
	i2c_startTransaction(ptrHandlerI2C);

	/*2. Enviamos la dirección del esclavo y la indicación ESCRIBIR*/
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress,
			I2C_WRITE_DATA);

	//3. Se envía el control Byte
	i2c_sendDataByte(ptrHandlerI2C, OLED_CONTROLBYTE_DATA);

	/*3. Enviamos la posición 0 del arreglo a enviar, nótese que el arreglo, byte 0(primera columna del carácter)*/
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

//Función de inicialización, se siguen los pasos del datasheet y se inicia en la columna 0, página 0.
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
	clearAllScreen(ptrHandlerI2C);
	clearAllScreen(ptrHandlerI2C);
}

//Esta función envía un arreglo de arreglos jajajaja, es decir, un arreglo es un carácter,
//un arreglo de arreglos son varios carácteres.
void sendBytesArray(I2C_Handler_t *ptrHandlerI2C, char **dataToSend){

	for(uint8_t counterOLED = 0; counterOLED < 16; counterOLED++){
		sendByteOLED(ptrHandlerI2C, *dataToSend);
		dataToSend++;
		//Se pasa al siguiente carácter (arreglo), recordar que el nombre es un putero a la primera
		//posición del array, al hacer dataToSend++ se le suma uno y se tiene un puntero a la segunda
		//posición
	}
}

//Esta función nos envía a la página en la que queremos escribir
void setPageOLED(I2C_Handler_t *ptrHandlerI2C, uint8_t pageNumber){

	uint8_t pageCommand = 0;

	pageCommand = (0xB << 4) | pageNumber;       //De la lista de comandos de arriba se tiene que los bits del 4 al 7 deben ser 0xB
	sendCommandOLED(ptrHandlerI2C, pageCommand);

}

//Esta función nos envía a la columna en la que queremos escribir
void setColumn(I2C_Handler_t *ptrHandlerI2C, uint8_t columnNumber){
	uint8_t columnCommandH = 0;
	uint8_t columnCommandL = 0;

	columnCommandH = (0x1 << 4) | (columnNumber >> 4);
	columnCommandL = (0x0 << 4) | (columnNumber & 0x0F);

	sendCommandOLED(ptrHandlerI2C, columnCommandH);
	sendCommandOLED(ptrHandlerI2C, columnCommandL);
}

//Con esta función se borra todo el display, los arreglos se hacen de 16 carácteres, porque es lo máximo que una fila almacena
void whiteScreenOLED(I2C_Handler_t *ptrHandlerI2C){
	char* whiteSpace[16] = {whiteLineChar(),whiteLineChar(),whiteLineChar(),whiteLineChar(),whiteLineChar(),whiteLineChar(),whiteLineChar(),whiteLineChar(),
			whiteLineChar(),whiteLineChar(),whiteLineChar(),whiteLineChar(),whiteLineChar(),whiteLineChar(),whiteLineChar(),whiteLineChar()};

	for(uint8_t counterPage = 0; counterPage < 8; counterPage++){
		setPageOLED(ptrHandlerI2C,counterPage);
		sendBytesArray(ptrHandlerI2C,whiteSpace);
	}
}

//Con esta función se limpia una fila
void clearOLED(I2C_Handler_t *ptrHandlerI2C){

	printBytesArray(ptrHandlerI2C, "                ");
}

void clearAllScreen(I2C_Handler_t *ptrHandlerI2C){

	for(uint8_t clearCounter = 0; clearCounter < 8; clearCounter++){
		setPageOLED(ptrHandlerI2C,clearCounter);
		setColumn(ptrHandlerI2C, 0x02);
		printBytesArray(ptrHandlerI2C, "                ");
	}


}

//Esta función toma el mensaje a enviar en código ascii y lo convierte a los carácteres definidos en este driver,
//es decir, lo convierte a su respectivo dibujo. La función lee el ascii y según el valor que obtiene
//retorna un patrón(retorna un char*, es decir, retorna el puntero a un arreglo, recordar que el carácter
//es un arreglo de bytes).
char* stringToChar(char character){
	switch(character){
	case(' '):
	{return spaceChar();
	break;}
	case('A'):
	{return AChar();
	break;}
	case('B'):
	{return BChar();
	break;}
	case('C'):
	{return CChar();
	break;}
	case('D'):
	{return DChar();
	break;}
	case('E'):
	{return EChar();
	break;}
	case('F'):
	{return FChar();
	break;}
	case('G'):
	{return GChar();
	break;}
	case('H'):
	{return HChar();
	break;}
	case('I'):
	{return IChar();
	break;}
	case('J'):
	{return JChar();
	break;}
	case('K'):
	{return KChar();
	break;}
	case('L'):
	{return LChar();
	break;}
	case('M'):
	{return MChar();
	break;}
	case('N'):
	{return NChar();
	break;}
	case('O'):
	{return OChar();
	break;}
	case('P'):
	{return PChar();
	break;}
	case('Q'):
	{return QChar();
	break;}
	case('R'):
	{return RChar();
	break;}
	case('S'):
	{return SChar();
	break;}
	case('T'):
	{return TChar();
	break;}
	case('U'):
	{return UChar();
	break;}
	case('V'):
	{return VChar();
	break;}
	case('W'):
	{return WChar();
	break;}
	case('X'):
	{return XChar();
	break;}
	case('Y'):
	{return YChar();
	break;}
	case('Z'):
	{return ZChar();
	break;}
	case('0'):
	{return ZEROChar();
	break;}
	case('1'):
	{return ONEChar();
	break;}
	case('2'):
	{return TWOChar();
	break;}
	case('3'):
	{return THREEChar();
	break;}
	case('4'):
	{return FOURChar();
	break;}
	case('5'):
	{return FIVEChar();
	break;}
	case('6'):
	{return SIXChar();
	break;}
	case('7'):
	{return SEVENChar();
	break;}
	case('8'):
	{return EIGHTChar();
	break;}
	case('9'):
	{return NINEChar();
	break;}
	case(':'):
	{return TwoPointsChar();
	case('.'):
	{return dotChar();}
	case('-'):
	{return minusChar();
	}
	case('/'):
	{return slashChar();
	}
	default:
	{return spaceChar();
	break;}
	}
}
}

void printSingleByte(I2C_Handler_t *ptrHandlerI2C, char singleByte){
	sendByteOLED(ptrHandlerI2C, stringToChar(singleByte));
}

void printBytesArray(I2C_Handler_t *ptrHandlerI2C, char* bytesArray){

	for(uint8_t j = 0; j < 16; j++){
			sendByteOLED(ptrHandlerI2C, stringToChar(*bytesArray)); //Recordar que esta función manda de a carácter
			bytesArray++;
		}
}

//Carácteres, para el alfabeto y los números, como se ve el espaciado entre carácteres es de 3 columnas y
// la letra es de tamaño 5x7
char characterA[8] = {0x00, 0b11111100, 0b00010010, 0b00010010, 0b00010010, 0b11111100, 0x00, 0x00};
char characterB[8] = {0x00, 0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b01101100, 0x00, 0x00};
char characterC[8] = {0x00, 0b01111100, 0b10000010, 0b10000010, 0b10000010, 0b01000100, 0x00, 0x00};
char characterD[8] = {0x00, 0b11111110, 0b10000010, 0b10000010, 0b10000010, 0b01111100, 0x00, 0x00};
char characterE[8] = {0x00, 0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b10000010, 0x00, 0x00};
char characterF[8] = {0x00, 0b11111110, 0b00010010, 0b00010010, 0b00010010, 0b00000010, 0x00, 0x00};
char characterG[8] = {0x00, 0b01111100, 0b10000010, 0b10000010, 0b10100010, 0b01100100, 0x00, 0x00};
char characterH[8] = {0x00, 0b11111110, 0b00010000, 0b00010000, 0b00010000, 0b11111110, 0x00, 0x00};
char characterI[8] = {0x00, 0b10000010, 0b10000010, 0b11111110, 0b10000010, 0b10000010, 0x00, 0x00};
char characterJ[8] = {0x00, 0b01100000, 0b10000000, 0b10000000, 0b10000000, 0b01111110, 0x00, 0x00};
char characterK[8] = {0x00, 0b11111110, 0b00010000, 0b00101000, 0b01000100, 0b10000010, 0x00, 0x00};
char characterL[8] = {0x00, 0b11111110, 0b10000000, 0b10000000, 0b10000000, 0b00000000, 0x00, 0x00};
char characterM[8] = {0x00, 0b11111110, 0b00000100, 0b00001000, 0b00000100, 0b11111110, 0x00, 0x00};
char characterN[8] = {0x00, 0b11111110, 0b00001000, 0b00010000, 0b00100000, 0b11111110, 0x00, 0x00};
char characterO[8] = {0x00, 0b01111100, 0b10000010, 0b10000010, 0b10000010, 0b01111100, 0x00, 0x00};
char characterP[8] = {0x00, 0b11111110, 0b00010010, 0b00010010, 0b00010010, 0b00001100, 0x00, 0x00};
char characterQ[8] = {0x00, 0b01111100, 0b10000010, 0b10100010, 0b01000010, 0b10111100, 0x00, 0x00};
char characterR[8] = {0x00, 0b11111110, 0b00010010, 0b00010010, 0b00010010, 0b11101100, 0x00, 0x00};
char characterS[8] = {0x00, 0b01001100, 0b10010010, 0b10010010, 0b10010010, 0b01100100, 0x00, 0x00};
char characterT[8] = {0x00, 0b00000010, 0b00000010, 0b11111110, 0b00000010, 0b00000010, 0x00, 0x00};
char characterU[8] = {0x00, 0b01111110, 0b10000000, 0b10000000, 0b10000000, 0b01111110, 0x00, 0x00};
char characterV[8] = {0x00, 0b00111110, 0b01000000, 0b10000000, 0b01000000, 0b00111110, 0x00, 0x00};
char characterW[8] = {0x00, 0b01111110, 0b10000000, 0b01100000, 0b10000000, 0b01111110, 0x00, 0x00};
char characterX[8] = {0x00, 0b11000110, 0b00101000, 0b00010000, 0b00101000, 0b11000110, 0x00, 0x00};
char characterY[8] = {0x00, 0b00000110, 0b00001000, 0b11110000, 0b00001000, 0b00000110, 0x00, 0x00};
char characterZ[8] = {0x00, 0b11000010, 0b10100010, 0b10010010, 0b10001010, 0b10000110, 0x00, 0x00};
char character0[8] = {0x00, 0b01111100, 0b10100010, 0b10010010, 0b10001010, 0b01111100, 0x00, 0x00};
char character1[8] = {0x00, 0b00000000, 0b10000100, 0b11111110, 0b10000000, 0b00000000, 0x00, 0x00};
char character2[8] = {0x00, 0b11000100, 0b10100010, 0b10010010, 0b10010010, 0b10001100, 0x00, 0x00};
char character3[8] = {0x00, 0b01000100, 0b10000010, 0b10010010, 0b10010010, 0b01101100, 0x00, 0x00};
char character4[8] = {0x00, 0b00110000, 0b00101000, 0b00100100, 0b00100010, 0b11111110, 0x00, 0x00};
char character5[8] = {0x00, 0b01001110, 0b10001010, 0b10001010, 0b10001010, 0b01110010, 0x00, 0x00};
char character6[8] = {0x00, 0b01111100, 0b10010010, 0b10010010, 0b10010010, 0b01100100, 0x00, 0x00};
char character7[8] = {0x00, 0b00000010, 0b11100010, 0b00010010, 0b00001010, 0b00000110, 0x00, 0x00};
char character8[8] = {0x00, 0b01101100, 0b10010010, 0b10010010, 0b10010010, 0b01101100, 0x00, 0x00};
char character9[8] = {0x00, 0b01001100, 0b10010010, 0b10010010, 0b10010010, 0b01111100, 0x00, 0x00};
char characterSpace[8] 	     = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char characterWhiteLine[8]   = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
char characterTwoPoints[8]   = {0x00, 0x00, 0x00, 0b00101000, 0x00, 0x00, 0x00, 0x00};
char characterDot[8]         = {0x00, 0x00, 0x00, 0b10000000, 0x00, 0x00, 0x00, 0x00};
char characterMinus[8]       = {0x00, 0x00, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0x00, 0x00};
char characterSlash[8]		 = {0x00, 0b10000000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0x00, 0x00};

//Para poder obtener el puntero a los arreglos, creamos una función que retorne el puntero
char* slashChar(void){
	return characterSlash;
}
char* minusChar(void){
	return characterMinus;
}
char* dotChar(void){
	return characterDot;
}
char* TwoPointsChar(void){
	return characterTwoPoints;
}
char* AChar(void){
	return characterA;
}
char* BChar(void){
	return characterB;
}
char* CChar(void){
	return characterC;
}
char* DChar(void){
	return characterD;
}
char* EChar(void){
	return characterE;
}
char* FChar(void){
	return characterF;
}
char* GChar(void){
	return characterG;
}
char* HChar(void){
	return characterH;
}
char* IChar(void){
	return characterI;
}
char* JChar(void){
	return characterJ;
}
char* KChar(void){
	return characterK;
}
char* LChar(void){
	return characterL;
}
char* MChar(void){
	return characterM;
}
char* NChar(void){
	return characterN;
}
char* OChar(void){
	return characterO;
}
char* PChar(void){
	return characterP;
}
char* QChar(void){
	return characterQ;
}
char* RChar(void){
	return characterR;
}
char* SChar(void){
	return characterS;
}
char* TChar(void){
	return characterT;
}
char* UChar(void){
	return characterU;
}
char* VChar(void){
	return characterV;
}
char* WChar(void){
	return characterW;
}
char* XChar(void){
	return characterX;
}
char* YChar(void){
	return characterY;
}
char* ZChar(void){
	return characterZ;
}
char* ZEROChar(void){
	return character0;
}
char* ONEChar(void){
	return character1;
}
char* TWOChar(void){
	return character2;
}
char* THREEChar(void){
	return character3;
}
char* FOURChar(void){
	return character4;
}
char* FIVEChar(void){
	return character5;
}
char* SIXChar(void){
	return character6;
}
char* SEVENChar(void){
	return character7;
}
char* EIGHTChar(void){
	return character8;
}
char* NINEChar(void){
	return character9;
}
char* spaceChar(void){
	return characterSpace;
}
char* whiteLineChar(void){
	return characterWhiteLine;
}
