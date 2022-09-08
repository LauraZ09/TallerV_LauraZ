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
	 * 	  función a ellos y se comprueba con el debugger si la función sí retorna el estado correcto, para esto
	 * 	  se definen unas variables en las que se almacenará el valor que retorne la función, así, en el debugger
	 * 	  se debe hacer seguimiento al cambio de estas variables. Este procedimiento se realiza a continuación*/

		//Se definen las variables donde se guarda el estado del PIN
		uint8_t pinC11Value = 0;
		(void) pinC11Value;
		uint8_t pinC12Value = 0;
		(void) pinC12Value;

		//Se define la variable counter_i, la cual servirá como contador para varios ciclos que se usarán en el programa.
		uint32_t counter_i;

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

		//Se escriben los pines en 1
		GPIO_WritePin (&handlerPinC11, SET);
		GPIO_WritePin (&handlerPinC12, SET);

        //Se leen los pines (A pesar de que son PINES de salida, las especificaciones del IDR dan a entender que también
		//pueden leerse los PINES en modo OUTPUT)
		pinC11Value = GPIO_ReadPin (&handlerPinC11);
		pinC12Value = GPIO_ReadPin (&handlerPinC12);

		//Se vuelven a poner los pines en 0
		GPIO_WritePin (&handlerPinC11, RESET);
		GPIO_WritePin (&handlerPinC12, RESET);

		/* PUNTO 2: Función para cambiar el Estado del PIN:
		 * void GPIOxTooglePin(GPIO_Handler_t *pPinHandler) {
	            pPinHandler->pGPIOx->ODR ^= (0b1 << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);}

	    Como se puede ver, se le aplica una máscara de 1 en la posición del PIN en el registro ODR,
	    y esta máscara se aplica con el operador XOR, así, si el valor del PIN es 1, entonces lo cambiará
	    a 0 y si es 0 lo cambiará a 1. Esta función se escribió en el GPIOxDriver.c y se definió en el GPIOxDriver.h,
	    el código a continuación es una forma de probar su funcionamiento. Primero se escribe en 1 el PINA5, y posteriormente
	    se le pone un delay para poder ver con el ojo el cambio en el PIN, después se cambia el estado del PIN con la función
	    GPIOxTooglePin y se le aplica el mismo delay para poder ver el cambio en el estado del PIN, este procedimiento se realiza
	    unas cuantas veces. Así, cuando se cargue el programa al microcontrolador lo que se debe observar es que el USER_LED de la board
	    titilará varias veces hasta quedarse nuevamente encendido.*/

		GPIO_WritePin (&handlerPinA5, SET);		//Se escribe el PINA5 se escribe en 1 (el LED se prende)
		for (counter_i = 0; counter_i <= 600000; counter_i++)			//Este for funciona como Delay
		{NOP ();}
		GPIOxTooglePin (&handlerPinA5);			//Al aplicar la función una vez debe pasar a apagado
		for (counter_i = 0; counter_i <= 600000; counter_i++)
		{NOP ();}
		GPIOxTooglePin (&handlerPinA5);         //Al aplicar la función una segunda vez debe prenderse otra vez
		for (counter_i = 0; counter_i <= 600000; counter_i++)
		{NOP ();}
		GPIOxTooglePin (&handlerPinA5);
		for (counter_i = 0; counter_i <= 600000; counter_i++)
		{NOP ();}
		GPIOxTooglePin (&handlerPinA5);
		for (counter_i = 0; counter_i <= 600000; counter_i++)
		{NOP ();}

    /* Loop forever */
	while (1) {

		/* PUNTO 3: Cambiar el USER LED cada que se pulsa el USER BUTTON. A continuación se puede ver el código para lograr este objetivo.
		 * Como se puede ver se lee el estado del PINC13, el cual corresponde al USER BUTTON, si el estado está en 0 (el botón está apretado),
		 * entonces se da la orden de que se cambie el estado del PINA5, después de esto se pone un delay para darle tiempo al dedo de retirarse del
		 * botón y poder ver el cambio en el estado del PIN.
		 * Este código se escribe dentro del ciclo while, ya que se quiere que el sistema esté constantemente revisando cuál es el estado del PIN para
		 * poder actuar.*/

		if (GPIO_ReadPin (&handlerPinC13)==0) {
			GPIOxTooglePin (&handlerPinA5);
			for (counter_i = 0; counter_i <= 600000; counter_i++)
			{NOP ();}
		}

		/* PUNTO 4: Se agrega un botón externo en el PINC6 y se configura en modo Pull Down, se lee el estado de este PIN. Si el estado del PIN es 1
		 * (apretado), se le da la orden al programa de que encienda los LED de PC10, PC11 y PC12, luego se pone un delay en tiempo con un ciclo for,
		 * se apaga el primer LED, se pone otro delay se apaga el segundo LED, se pone otro delay se apaga el tercer LED.
		 * Este código se escribe dentro del ciclo while, ya que se quiere que el sistema esté constantemente revisando cuál es el estado del PIN para
		 * poder actuar.*/

		if (GPIO_ReadPin (&handlerPinC6)==1) {
			GPIOxTooglePin (&handlerPinC10);
			GPIOxTooglePin (&handlerPinC11);
			GPIOxTooglePin (&handlerPinC12);
			for (counter_i = 0; counter_i <= 600000; counter_i++)
			{NOP ();}
			GPIOxTooglePin (&handlerPinC12);
			for (counter_i = 0; counter_i <= 600000; counter_i++)
			{NOP ();}
			GPIOxTooglePin (&handlerPinC11);
			for (counter_i = 0; counter_i <= 600000; counter_i++)
			{NOP ();}
			GPIOxTooglePin (&handlerPinC10);
		}
	}
}



