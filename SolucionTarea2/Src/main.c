/******************************************************************************
 * @file           :Solución Tarea 2
 * @author         :Laura Alejandra Zuluaga Moreno,
 *  			   :lazuluagamo@unal.edu.co
 ******************************************************************************
 *En el siguiente documento se encuentra la solución a la Tarea 2
 ******************************************************************************/

#include <stdint.h>
#include "stm32f411xx.h"
#include "GPIOxDriver.h"

int main(void)
{
	/* PUNTO 1: La función ReadPin estaba definida como se muestra a continuación:

	 * uint32_t GPIO_ReadPin (GPIO_Handler_t *pPinHandler) {
	   //Creamos una variable auxiliar la cual luego retornaremos:

		uint32_t pinValue = 0;

		//Cargamos el valor específico del registro IDR, desplazado a la derecha tantas veces como la ubicación
		//del pin específico:

		pinValue = (pPinHandler ->pGPIOx ->IDR  >> pPinHandler->GPIO_PinConfig.GPIO_PinNumber);

		return pinValue;}

	 * A. El error es el siguiente: cuando se carga el valor específico del registro IDR, no se "limpia" el valor
	 * 	  del registro, así, cuando se desplaza a la derecha la cantidad de posiciones correspondientes al PIN, se
	 * 	  podría copiar en la variable pinValue un valor diferente al estado del PIN(0 o 1), ya que se le suman los valores que
	 * 	  puedan haber en las otras posiciones del IDR (posiciones correspondientes a otros PINES), así, el problema en
	 * 	  particular se presentará al haber varios PINES del mismo puerto  activados al tiempo. Así, por ejemplo, si el PINA5
	 * 	  se encuentra en 0, pero el PINA6 está en 1, la función leerá el valor 0b10 que es equivalente a 2 en decimal.
	 *
	 * B. El problema podría solucionarse haciendo el siguiente procedimiento: Limpiando primero el registro en todas
	 *    las posiciones excepto la posición correspondiente al PIN que se quiere leer.Esto se puede lograr implementando
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
	 * 	  Se escriben los pines PIN_C11 y PIN_C12 en 1 con la función GPIO_WritePin, luego, se aplica la
	 * 	  función a ellos y se comprueba con el debugger si la función sí retorna el estado correcto. Este
	 * 	  procedimiento se realiza a continuación*/

	   //Se definen los handler para los pines que deseamos configurar

	    GPIO_Handler_t handlerPinC6  = {0};
	    GPIO_Handler_t handlerPinC10 = {0};
		GPIO_Handler_t handlerPinC11 = {0};
		GPIO_Handler_t handlerPinC12 = {0};
		GPIO_Handler_t handlerPinC13 = {0};
		GPIO_Handler_t handlerPinA5  = {0};

		//Se hace la configuración de los pines

	    handlerPinC6.pGPIOx = GPIOC;
	    handlerPinC6.GPIO_PinConfig.GPIO_PinNumber 			= PIN_6;
	    handlerPinC6.GPIO_PinConfig.GPIO_PinMode 		    = GPIO_MODE_IN;
	    handlerPinC6.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	    handlerPinC6.GPIO_PinConfig.GPIO_PinPuPdControl    	= GPIO_PUPDR_PULLDOWN;
	    handlerPinC6.GPIO_PinConfig.GPIO_PinSpeed 		    = GPIO_OSPEED_MEDIUM;
	    handlerPinC6.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF0;

	    handlerPinC10.pGPIOx = GPIOC;
	    handlerPinC10.GPIO_PinConfig.GPIO_PinNumber 		= PIN_10;
	    handlerPinC10.GPIO_PinConfig.GPIO_PinMode 		    = GPIO_MODE_OUT;
	    handlerPinC10.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	    handlerPinC10.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	    handlerPinC10.GPIO_PinConfig.GPIO_PinSpeed 		    = GPIO_OSPEED_MEDIUM;
	    handlerPinC10.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;

	    handlerPinC11.pGPIOx = GPIOC;
	    handlerPinC11.GPIO_PinConfig.GPIO_PinNumber 		= PIN_11;
	    handlerPinC11.GPIO_PinConfig.GPIO_PinMode 		    = GPIO_MODE_OUT;
	    handlerPinC11.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	    handlerPinC11.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	    handlerPinC11.GPIO_PinConfig.GPIO_PinSpeed 		    = GPIO_OSPEED_MEDIUM;
	    handlerPinC11.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;

	    handlerPinC12.pGPIOx = GPIOC;
	    handlerPinC12.GPIO_PinConfig.GPIO_PinNumber 		= PIN_12;
	    handlerPinC12.GPIO_PinConfig.GPIO_PinMode 		    = GPIO_MODE_OUT;
	    handlerPinC12.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	    handlerPinC12.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	    handlerPinC12.GPIO_PinConfig.GPIO_PinSpeed 		    = GPIO_OSPEED_MEDIUM;
	    handlerPinC12.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;

	    handlerPinC13.pGPIOx = GPIOC;
	    handlerPinC13.GPIO_PinConfig.GPIO_PinNumber 		= PIN_13;
	    handlerPinC13.GPIO_PinConfig.GPIO_PinMode 		    = GPIO_MODE_IN;
	    handlerPinC13.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	    handlerPinC13.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_PULLUP;
	    handlerPinC13.GPIO_PinConfig.GPIO_PinSpeed 		    = GPIO_OSPEED_MEDIUM;
	    handlerPinC13.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;


	    handlerPinA5.pGPIOx = GPIOA;
	    handlerPinA5.GPIO_PinConfig.GPIO_PinNumber 		    = PIN_5;
	    handlerPinA5.GPIO_PinConfig.GPIO_PinMode 		    = GPIO_MODE_OUT;
	    handlerPinA5.GPIO_PinConfig.GPIO_PinOPType 		    = GPIO_OTYPE_PUSHPULL;
	    handlerPinA5.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	    handlerPinA5.GPIO_PinConfig.GPIO_PinSpeed 		    = GPIO_OSPEED_MEDIUM;
	    handlerPinA5.GPIO_PinConfig.GPIO_PinAltFunMode 	    = AF0;


		//Cargamos la configuración del PIN específico

		GPIO_Config(&handlerPinC6);
		GPIO_Config(&handlerPinC10);
		GPIO_Config(&handlerPinC11);
		GPIO_Config(&handlerPinC12);
		GPIO_Config(&handlerPinC13);
		GPIO_Config(&handlerPinA5);


		//Se definen las variables donde se guarda el estado del PIN
		uint8_t pinC11Value = 0;
		(void) pinC11Value;
		uint8_t pinC12Value = 0;
		(void) pinC12Value;

		//Se escriben los pines en 1
		GPIO_WritePin (&handlerPinC10, SET);
		GPIO_WritePin (&handlerPinC11, SET);
		GPIO_WritePin (&handlerPinC12, SET);

        //Se leen los pines
		pinC11Value = GPIO_ReadPin (&handlerPinC11);
		pinC12Value = GPIO_ReadPin (&handlerPinC12);

		//Se vuelven a poner los pines en 0
		GPIO_WritePin (&handlerPinC10, RESET);
		GPIO_WritePin (&handlerPinC11, RESET);
		GPIO_WritePin (&handlerPinC12, RESET);


		/* PUNTO 2:	 Cree una nueva función llamada GPIOxTooglePin, la cual reciba como parámetro solamente un elemento
		 *
		 *del tipo GPIO_Handler_t y su función sea cambiar el estado de un PinX (seleccionado en el handler y debidamente
		 *del  configurado). Cambiar el estado significa: sí está encendido que pase a apagado, si está apagado que pase a
		 *del  encendido. Hint: La operación XOR puede ser útil.*/

		GPIO_WritePin (&handlerPinA5, SET);//Se escribe el PINA5 se escribe en 1 (el LED se prende)
		uint32_t i;
		for (i = 0; i <= 600000; i++);
		GPIOxTooglePin (&handlerPinA5);		//Al aplicar la función una vez debe pasar a apagado
		for (i = 0; i <= 600000; i++);
		GPIOxTooglePin (&handlerPinA5);		//Al aplicar la función una segunda vez debe prenderse otra vez
		for (i = 0; i <= 600000; i++);
		GPIOxTooglePin (&handlerPinA5);
		for (i = 0; i <= 600000; i++);
		GPIOxTooglePin (&handlerPinA5);
		for (i = 0; i <= 600000; i++);

		/*Como se puede observar, se aplica la función unas cuantas veces y después de aplicarla cada vez se pone un ciclo for,
		 * este ciclo es un delay para poder alcanzar a ver cada cambio en el estado del PIN, en la práctica este código al
		 * cargarlo en el micro debe verse como un blinky de unas cuantas repeticiones.
		 */

    /* Loop forever */ //Preguntar lo de in y out
	while (1) {

		/* PUNTO 3:	 Utilice el “USER_BUTTON” (azul) con la nueva función GPIOxToogle, de forma que el USER_LED (Led verde)
		 *  cambie de estado cada vez que pulsa USER_BUTTON..*/

		if (GPIO_ReadPin (&handlerPinC13)==0) {
			GPIOxTooglePin (&handlerPinA5);
			for (i = 0; i <= 600000; i++);

		}

		/*Agregue un botón externo al PIN_C6, configurado en modo Pull_down. La acción de este botón debe hacer que al pulsar
		 * este botón, se enciendan tres Leds (PC10, PC11 y PC12) de forma simultánea y luego de 5 segundos se apaguen en
		 * cascada:
			Se apaga primero PC12.
			Dos segundos después se apaga PC11.
			Un segundo después se apaga PC10.
		 *
		 */
		if (GPIO_ReadPin (&handlerPinC6)==1) {
			GPIO_WritePin (&handlerPinC10,SET);
			GPIOxTooglePin (&handlerPinC11);
			GPIOxTooglePin (&handlerPinC12);
			for (i = 0; i <= 600000; i++);
			GPIOxTooglePin (&handlerPinC12);
			for (i = 0; i <= 600000; i++);
			GPIOxTooglePin (&handlerPinC11);
			for (i = 0; i <= 600000; i++);
			GPIOxTooglePin (&handlerPinC10);
		}
	}
}



