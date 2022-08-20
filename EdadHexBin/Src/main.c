/**
 ******************************************************************************
 * @file           : Tarea 1. Mi edad en otros números
 * @author         : Laura Alejandra Zuluaga
 * @brief          :????? preguntar qué es el brief
 ******************************************************************************
 ******************************************************************************
 */

//#include <stdint.h> Esto es necesario?

/*Ejercicio 1: Definiendo algunas variables.
 *
 * A cerca de la elección de los tamaños de las variables:
 *
 * 1. Variable age: Esta es la variable donde se almacenará mi edad en años. Se le
 * asigna un tamaño char, ya que con 8 bits es más que suficiente para la edad de
 * cualquier persona (con 8 bits se pueden almacenar en la variable números entre el 0
 * y el 255).
 *
 * 2. Variable days_per_year: En esta variable se almacenará la cantidad de días por año.
 * Se le asigna un tamaño short, ya que cada año cuenta con 365 días, al ser el número
 * 365 mayor a 255, se necesita un tamaño más grande que el char, por tanto se asigna
 * con un tamaño de dato short.
 *
 * 3. Variables seconds_per_hour: En esta variable se almacenará la cantidad de segundos
 * por hora. Se le asigna a la variable un tamaño char, ya que al ser 60 segundos por
 * hora, 8 bits serán suficientes para almacenar esta variable.
 */

unsigned char age = 0;                 //Variable para mi edad en años.
unsigned short days_per_year = 0;      //Variable para la cantidad de días por año.
unsigned char seconds_per_hour = 0;    //Variable para la cantidad de segundos por hora.

/*Ejercicio 2: Definiendo los días que han pasado desde la fecha de nacimiento.
 *
 * A cerca de la elección del tamaño de la variable:
 *
 * Variable days_since_born: Esta variable almacena el número de días que han pasado
 * desde la fecha de nacimiento. Se le asigna un tamaño short, ya que el número de días
 * que han pasado desde la fecha de nacimiento es mayor a 255 (el valor que puede
 * almacenar un tipo char) y menor a 2^16.
 *
 * Para calcular el número de días que han pasado: tengo 22 años, cumplidos el 09 de
 * Enero de este año. Se tienen en cuenta entonces los siguientes aspectos:
 *
 * 1. Días que pasaron desde el 09/01/2000 hasta el 09/01/2022: Para calcular esto se
 * asume que todos los años pasados han tenido 365 días; así, se hace la siguiente
 * operación: 365*22 = 8030.
 * 2. Días que pasaron desde el 09/01/2022 hasta el 31/01/2022: Esto es para calcular
 * los días restantes del mes de Enero, para esto solo se resta: 31-9 = 22.
 * 3. Días que pasaron desde el 31/01/2022 hasta el 31/07/2022: Esta operación es para
 * calcular los días que han pasado desde que terminó Enero hasta antes de empezar
 * Agosto. Para esto se tiene en cuenta que: los meses de Febrero, Marzo, Abril, Mayo,
 * Junio y Julio tienen respectivamente 28, 31, 30, 31, 30 y 31 días. Así, desde Febrero
 * hasta Julio pasan 181 días.
 * 4. Días que pasaron desde el 31/01/2022 hasta el 17/08/2022: Para esto no se tiene en
 * cuenta el día 17, ya que es hasta el medio día. Así, en Agosto pasan 16 días.
 *
 * Teniendo en cuenta lo anterior, la operación que se realiza para este cálculo es
 * la siguiente:(365*22) + (31-9) + 181 + 16 , así days_since_born = 8249.
 */

unsigned short days_since_born = 0;    //Variable para los días que han pasado desde el
                                       // nacimiento.


/*Ejercicio 3: Definiendo las horas que han pasado desde la fecha de nacimiento.
 *PREGUNTAR SI PUEDO CREAR UNA VARIABLE CON EL NÚMERO DE HORAS POR DÍA, O SI ES
 *INNECESARIO
 *
 * A cerca de la elección del tamaño de las variables:
 *
 * 1.Variable hours_since_born: En esta variable se almacenan el número de horas pasadas
 * desde la fecha de nacimiento. Este valor se estima que es mayor al valor 2^16 - 1,
 * por lo tanto se le asigna un tamaño de variable int, así, 32 bits son más que
 * suficientes para almacenar la variable.
 *
 * 2.Variable hours_per_day: En esta variable se almacenan las horas por día, ya que un
 * día tiene 24 horas, esta variable puede ser almacenada perfectamente en una variable
 * tipo char.
 *
 * Para calcular el número de horas que han pasado: para esto se considera las 09:00
 * horas como la hora de nacimiento. De esta forma, el cálculo que se hace es el
 * siguiente: hours_since_born = days_since_born* + 3. Este último 3 se le suma, ya que
 * desde las 09:00 horas del 17 de Agosto, hasta las 12:00 pasan 3 horas.
 */

unsigned char hours_per_day = 0;
unsigned int hours_since_born = 0;


//PREGUNTAR SI ES MEJOR ASIGNARLE VALOR A UNA VARIABLE DENTRO DEL MAIN O DESDE QUE SE
//DECLARA


int main(void) //Se crea la función main.

{
	//Se le asigna un valor distinto de 0 a las variables creadas anteriormente.

	age = 22;
	days_per_year = 365;
	days_since_born = 8249;
	hours_per_day = 24;
	hours_since_born = hours_per_day*days_since_born - 3;


	while (1) { //Se le pone el ciclo infinito
		;
	}
}
