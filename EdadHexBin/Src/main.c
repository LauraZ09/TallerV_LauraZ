/**
 ******************************************************************************
 * @file           : Tarea 1. Mi edad en otros números
 * @author         : Laura Alejandra Zuluaga
 * @brief          :????? preguntar qué es el brief
 ******************************************************************************
 ******************************************************************************
 */

#include <stdint.h>

/*Ejercicio 1: Definiendo algunas variables.
 *
 * A cerca de la elección de los tamaños de las variables:
 *
 * 1. Variable age: Esta es la variable donde se almacenará mi edad en años. Se asigna una
 * varible tipo char, ya que con 8 bits es más que suficiente para almecenar la edad de
 * cualquier persona (con 8 bits se pueden almacenar en la variable números entre el 0
 * y el 255).
 *
 * 2. Variable days_per_year: En esta variable se almacenará la cantidad de días por año.
 * Se asigna define como una variable tipo short, ya que cada año cuenta con 365 días;
 * al ser el número 365 mayor a 255, se necesita un tamaño más grande que 8 bits, por tanto
 * se define la variable tipo short con un tamaño de 16 bits y con la capacidad de almacenar
 * números entre el 0 y (2^16-1).
 *
 * 3. Variable seconds_per_hour: En esta variable se almacenará la cantidad de segundos
 * por hora. Esta variable se define como una variable de tipo short, ya que al ser 3600
 * segundos por hora 8 bits serán insuficientes para almacenar esta variable pero 16
 * bits serán suficientes (3600 es un número menor a (2^16-1)).*/

unsigned char age = 0;              //Se define variable para mi edad en años.
unsigned short days_per_year = 0;   //Se define variable para la cantidad de días por año.
unsigned short seconds_per_hour = 0;//Se define variable para la cantidad de segundos por hora.

/*Como se puede ver en las 3 líneas anteriores las variables se definen e inmediatamente se
 * les asigna un valor de 0, para inicializarlas adecuadamente.*/

/*Ejercicio 2: Definiendo los días que han pasado desde la fecha de nacimiento.
 *
 * A cerca de la elección del tamaño de la variable:
 *
 * Variable days_since_born: Esta variable almacena el número de días que han pasado
 * desde la fecha de nacimiento. Se le asigna un tamaño short, ya que el número de días
 * que han pasado desde la fecha de nacimiento es mayor a 255 (el valor que puede
 * almacenar una variable tipo char) y menor a 2^16-1 (el valor que puede almacenar una
 * variable tipo short).
 *
 * Con respecto al cálculo de los días que han pasado: para este ejercicio, se asume que
 * cada año cuenta con 365 días y se trabaja con la siguiente fecha y hora de nacimiento:
 * 09 de Enero/2000 a las 09:00 horas. Para realizar el cálculo, se tienen en cuenta entonces
 * los siguientes aspectos:
 *
 * 1. Días que pasaron desde el 09/01/2000 hasta el 09/01/2022: A las 09:00 horas del
 * 09/01/2022 se cumplen 22 años desde la fecha de nacimiento, así el número de días desde
 * la fecha de nacimiento hasta el 09/01/2022 será igual a: age*days_per_year, es decir, es
 * la multiplicación de los años que han pasado, por el número que representa mi edad en años.
 *
 * 2. Días que pasaron desde el 09/01/2022 hasta el 31/01/2022: Esto es para calcular
 * los días restantes del mes de Enero, para esto solo se resta: 31-9 = 22. (Esta cuenta
 * se hace hasta las 09:00 Horas del 31 de Enero de este año).
 *
 * 3. Días que pasaron desde el 31/01/2022 hasta el 31/07/2022: Esta operación es para
 * calcular los días que han pasado desde que terminó Enero hasta antes de empezar
 * Agosto. Para esto se tiene en cuenta que: los meses de Febrero, Marzo, Abril, Mayo,
 * Junio y Julio tienen respectivamente 28, 31, 30, 31, 30 y 31 días. Así, desde Febrero
 * hasta las 09:00 horas del 31 de Julio pasan 181 días.
 *
 * 4. Días que pasaron desde el 31/07/2022 hasta el 17/08/2022: Para esto, se tiene también
 * en cuenta el día 17, ya que la hora exacta de nacimiento es a las 09:00 horas, sin embargo,
 * no se tienen en cuenta las 3 horas adicionales que hay entre las 09:00 y las 12:00m de este
 * día.
 *
 * Teniendo en cuenta lo anterior, la operación que se realiza para este cálculo es
 * la siguiente:(age*days_per_year) + (31-9) + 181 + 17.
 */

unsigned short days_since_born = 0;    //Se define variable para los días que han pasado desde
                                       // el nacimiento.


/*Ejercicio 3: Definiendo las horas que han pasado desde la fecha de nacimiento.
 *PREGUNTAR SI PUEDO CREAR UNA VARIABLE CON EL NÚMERO DE HORAS POR DÍA, O SI ES
 *INNECESARIO
 *
 * A cerca de la elección del tamaño de las variables:
 *
 * 1.Variable hours_since_born: En esta variable se almacenan el número de horas pasadas
 * desde la fecha de nacimiento. Este valor se estima que es mayor al valor 2^16 - 1,
 * por lo tanto se crea una variable de tipo int y con un tamaño de 32 bits, los cuales son
 * suficientes para almacenar la variable.
 *
 * 2.Variable hours_per_day: En esta variable se almacenan las horas por día, ya que un
 * día tiene 24 horas, esta variable puede ser almacenada perfectamente en una variable de
 * tipo char.
 *
 * Para calcular el número de horas que han pasado: para esto se considera las 09:00
 * horas como la hora de nacimiento. De esta forma, el cálculo que se hace es el
 * siguiente: hours_since_born = days_since_born*hours_per_day + 3. Esta última adición
 * corresponde a las 3 horas que pasan desde las 09:00 horas del 17 de Agosto hasta las
 * 12:00m de este mismo día.*/

unsigned char hours_per_day = 0;   //Se define la  variable para almacenar las horas por día.
unsigned int hours_since_born = 0; //Se define variable para las horas desde el nacimiento.

//PREGUNTAR SI ES MEJOR ASIGNARLE VALOR A UNA VARIABLE DENTRO DEL MAIN O DESDE QUE SE
//DECLARA

/*Ejercicio 4: Definiendo los segundos que han pasado desde la fecha de nacimiento.
 * A cerca de la elección del tamaño de la variable:
 *
 * 1.Variable seconds_since_born: En esta variable se almacenan el número de segundos pasados
 * desde la fecha de nacimiento. Este valor se estima que es mayor al valor 2^16 - 1,
 * por lo tanto se define una variable de tipo int y con un tamaño de 32 bits.
 *
 * Para calcular el número de segundos que han pasado: El cálculo que se hace es el
 * siguiente: seconds_since_born = hours_since_born*seconds_per_hour.*/

unsigned int seconds_since_born = 0;//Se define variable para los segundos desde el nacimiento.

unsigned char bin_days_since_born  = 0;
unsigned char bin_hours_since_born = 0;
unsigned char hex_seconds_since_born = 0;


int main(void) //Se crea la función main.

{
	/*A continuación se le asigna un valor distinto de 0 a las variables creadas en el
	 * ejercicio 1: Estas asignaciones a las variables se hacen dentro del main.*/

	age = 22;
	days_per_year = 365;
	seconds_per_hour = 3600;

	/*A continuación se le asigna un valor distinto de 0 a la variable creada para el
	* ejercicio 2, teniendo en cuenta los cálculos anteriormente descritos:*/

	days_since_born = (age*days_per_year) + (31-9) + 181 + 17;

	/*A continuación se le asigna un valor distinto de 0 a las variables creadas para el
	* ejercicio 3, teniendo en cuenta los cálculos anteriormente descritos:*/

	hours_per_day = 24;
	hours_since_born = hours_per_day*days_since_born + 3;

	/*A continuación se le asigna un valor distinto de 0 a la variable creada para el
	* ejercicio 4, teniendo en cuenta los cálculos anteriormente descritos:*/

	seconds_since_born = hours_since_born * seconds_per_hour;

	/*Ejercicio 5:
	 *
	 * Para este ejercicio, para pasar de decimal a binario primero se debe descomponer el
	 * número decimal en la suma de diferentes potencias de 2, esto se logra con la siguiente
	 * serie de pasos:
	 *
	 * 1. Se toma el número decimal y se divide entre 2. Esta división solo puede tener
	 * dos posibles residuos: 1 o 0. Si el residuo que se obtiene es 1, esto quiere decir
	 * que el número es impar y por lo tanto uno de los factores de descomposición es el 1,
	 * lo que es equivalente a sumar 2^0.
	 *
	 * 2. El número entero obtenido por la división anterior se vuelve a dividir entre 2, en
	 * caso de que no se obtenga ningún residuo, se seguirá dividiendo hasta obtener un
	 * residuo nuevamente, cuando se obtenga el residuo, se sumará a la descomposición el
	 * valor 2^n, donde n es el número de divisiones que se han hecho desde la primera
	 * hasta obtener nuevamente el residuo.
	 *
	 * 3. Se debe seguir con el procedimiento anterior hasta obtener de las divisiones un
	 * número menor que 1.
	 *
	 * Para clarificar un poco el procedimiento, se deja el siguiente ejemplo: sea el número
	 * decimal 13. Al hacer la operación 13/2, se tiene como resultado entero el 6, con un
	 * residuo de 1, este residuo se da porque el número es impar, así, ya se sabe que
	 * en la descomposición del número debe sumarse un 2^0. Luego, se sigue dividiendo: 6/2=3,
	 * como no se obtiene ningún residuo se sigue dividiendo, 3/2=1, esta operación sí tiene
	 * residuo, esto quiere decir que a la descomposición se le debe sumar el valor 2^2. Luego
	 * se sigue dividiendo: 1/2, donde ya se obtiene un número menor a 1 y también se tiene
	 * un residuo, por lo tanto se deberá sumar también 2^3. Así, la descomposición del número
	 * es la siguiente: 13 = 2^0 + 2^2 + 2^3.
	 *
	 * Al tener la descomposición del número, lo único que se debe hacer después, es poner 1
	 * en las posiciones que representan las potencias obtenidas en la operación anterior,
	 * así para el ejemplo anterior se pondría 1 en las posiciones 0, 1 y 3, quedando el número
	 * binario: 1101.
	 *
	 * A continuación se le asigna a las variables days_since_born y hours_since_born su
	 * correspondiente valor en binario:
	 */

	//PREGUNTAR SI PRESENTAR ES IMPRIMIR O KHE?

	days_since_born=0b10000000111010;

	bin_days_since_born = 0b10000000111010; //Cuando se le pone el 0b, no hace bien la operación bitwise,
	//creo otra variable?
	bin_hours_since_born = 0b110000010101110011;

	/*Ahora, para expresar el número de segundos en Hexadecimal:
	 * Básicamente el procedimiento es el mismo explicado anteriormente, ya que cuando se pasa
	 * de decimal a binario se está cambiando el número de base 10 a base 2, al pasar de decimal a
	 * hexadecimal, se hace el mismo procedimiento, pero ya teniendo en cuenta que se va a pasar de
	 * base 16 a base 10, así, se debe dividir en vez de entre 2, entre 16 y se obteienen los factores
	 * de descomposición.*/

	hex_seconds_since_born = 0x2a7ca130;

	/*Ejercicio 6:*/

	days_since_born = days_since_born << 4; //se obtiene 16500
	//days_since_born = days_since_born << 1; //se obtiene 16500
	//days_since_born = days_since_born << 1; //se obtiene 16500
	//days_since_born = days_since_born << 1; //se obtiene 16500

	hours_since_born = hours_since_born >> 1;
	hours_since_born = hours_since_born >> 1;
	hours_since_born = hours_since_born >> 1;
	hours_since_born = hours_since_born >> 1;












	while (1) { //Se le pone el ciclo infinito
		;
	}
}
