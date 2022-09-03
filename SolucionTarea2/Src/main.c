/******************************************************************************
 * @file           : Solución Tarea 2
 * @author         : Laura Alejandra Zuluaga Moreno
 ******************************************************************************
 *En el siguiente documento se encuentra la solución a la Tarea 2
 ******************************************************************************/

#include <stdint.h>
#include "stm32f411xx.h"
#include "GPIOxDriver.h"

int main(void)
{

	/* PUNTO 1:
	 *
	 * uint32_t GPIO_ReadPin (GPIO_Handler_t *pPinHandler) {
		//Creamos una variable auxiliar la cual luego retornaremos
		uint32_t pinValue = 0;

		//Cargamos el valor específico del registro IDR, desplazado a la derecha tantas veces como la ubicación
		//del pin específico
		pinValue = (pPinHandler ->pGPIOx ->IDR  >> pPinHandler->GPIO_PinConfig.GPIO_PinNumber);

		return pinValue;
	}
	 * //Cargamos el valor específico del registro IDR, desplazado a la derecha tantas veces como la ubicación
		//del pin específico
	 * A. El error es el siguiente: cuando se carga el valor específico del registro IDR, no se limpia el valor
	 * 	  del registro, así, cuando se desplaza a la derecha la cantidad de posiciones correspondientes al PIN, se
	 * 	  copia en la variable pinValue un valor diferente al estado del pin, ya que se le suman los valores que
	 * 	  puedan haber en las otras posiciones.
	 *
	 * B. El problema podría solucionarse haciendo el siguiente procedimiento: Limpiando primero el registro en todas
	 *    las posiciones excepto la posición correspondiente al pin que se quiere leer.Esto se puede lograr implementando
	 *    las siguientes líneas de código:
	 *
	 *    mask = 0b1 << pPinHandler->GPIO_PinConfig.GPIO_PinNumber
	 *    pinValue = (pPinHandler ->pGPIOx ->IDR & mask)>> pPinHandler->GPIO_PinConfig.GPIO_PinNumber
	 *
	 *    La primera línea es la creación de una máscara, la cual tiene un uno en la posición correspondiente al
	 *    número del pin. La segunda línea corresponde a la aplicación de la másacara con un and, así, se conserva
	 *    únicamente el valor correspondiente a ese bit. Luego, con el operador shift derecha, se mueve el valor de
	 *    este bit la cantidad de posiciones correspondiente al pin, de esta forma queda en la posición 0.
	 *
	 * C. Se corrige el código, reemplazando por las líneas que se escribieron en el enunciado anterior. Y para
	 * 	  comprobar que el código queda correcto se realiza el siguiente ejercicio:
	 *
	 * 	  Se escriben los pines PIN_A5 y PIN_A6 en 1 con la función GPIO_WritePin, luego, se aplica la
	 * 	  función a ellos y se comprueba con el debugger si la función sí retorna el estado correcto. Este
	 * 	  procedimiento se realiza a continuación*/

	   //Se definen los handler para los pines que deseamos configurar
		GPIO_Handler_t handlerPinC11 = {0};
		GPIO_Handler_t handlerPinC12 = {0};

		//Se hace la configuración de los pines

	    handlerPinC11.pGPIOx = GPIOC;
	    handlerPinC11.GPIO_PinConfig.GPIO_PinNumber 		= PIN_11;
	    handlerPinC11.GPIO_PinConfig.GPIO_PinMode 		    = GPIO_MODE_IN;
	    handlerPinC11.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	    handlerPinC11.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_PULLUP;
	    handlerPinC11.GPIO_PinConfig.GPIO_PinSpeed 		    = GPIO_OSPEED_MEDIUM;
	    handlerPinC11.GPIO_PinConfig.GPIO_PinAltFunMode 	    = AF0;

	    handlerPinC12.pGPIOx = GPIOC;
	    handlerPinC12.GPIO_PinConfig.GPIO_PinNumber 		= PIN_12;
	    handlerPinC12.GPIO_PinConfig.GPIO_PinMode 		    = GPIO_MODE_IN;
	    handlerPinC12.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	    handlerPinC12.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_PULLUP;
	    handlerPinC12.GPIO_PinConfig.GPIO_PinSpeed 		    = GPIO_OSPEED_MEDIUM;
	    handlerPinC12.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;

		//Cargamos la configuración del PIN específico
		GPIO_Config(&handlerPinC11);
		GPIO_Config(&handlerPinC12);

		//Se definen las variables donde se guarda el estado del PIN
		uint8_t pinC11Value = 0;
		(void) pinC11Value;
		uint8_t pinC12Value = 0;
		(void) pinC12Value;


    /* Loop forever */ //Preguntar lo de in y out
	while (1) {

		//Se escriben los PINES en 1
		GPIO_WritePin (&handlerPinC11, SET);
		GPIO_WritePin (&handlerPinC12, SET);

		//Se lee el estado de los PINES:

		pinC11Value = GPIO_ReadPin (&handlerPinC11);
		pinC12Value = GPIO_ReadPin (&handlerPinC12);

	/*Al debuggear el ejercicio anterior y mirar las variables pinC11Value y pinC12Value, se
	 * observa que la función lee correctamente el estado del PIN
	 */

	/* PUNTO 2:	 Cree una nueva función llamada GPIOxTooglePin, la cual reciba como parámetro solamente un elemento
	 *
	 *del tipo GPIO_Handler_t y su función sea cambiar el estado de un PinX (seleccionado en el handler y debidamente
	 *del  configurado). Cambiar el estado significa: sí está encendido que pase a apagado, si está apagado que pase a
	 *del  encendido. Hint: La operación XOR puede ser útil.*/

		//uint8_t GPIOxTooglePin(GPIO_Handler_t *pPinHandler)
		//{

		//}


	}
}

