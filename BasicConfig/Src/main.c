/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Laura Zuluaga
 * @brief          : Configuración básica de un proyecto
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include <stdint.h>

/*Función Principal del programa*/

uint16_t data = 0;
uint16_t *ptrEjemplo;
uint32_t valorPuntero = 0;

int main(void)
{
	//Inicializando el valor
	data = 57;

	//Cambiando el valor de data en +32
	data += 32;

	//Cargamos la posición de la memoria de data
	ptrEjemplo = &data;

	//Para ver la posición de memoria
	valorPuntero = (uint32_t) ptrEjemplo;

	//Escribir el valor en esa posición de memoria
	*ptrEjemplo+=2;
	//Moverse de posición de memoria
	ptrEjemplo++;
	//Escribo en la nueva posición
	*ptrEjemplo+=0xAC;

    /* Loop forever */
	while (1) {

	}

	return 0;
}
