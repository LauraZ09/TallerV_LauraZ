/**
 ******************************************************************************
 * @file           		: Tarea 1. Mi edad en otros números
 * @author         		: Laura Alejandra Zuluaga, lazuluagamo@unal.edu.co
 * Fecha de nacimiento	: 09/01/2000
 ******************************************************************************
 ******************************************************************************
 */

/*Ejercicio 1: Definiendo algunas variables.
 *
 * A cerca de la elección de los tamaños de las variables:
 *
 * 1. Variable age: Esta es la variable donde se almacenará mi edad en años. Se define una
 * varible de tipo char, ya que con 8 bits es más que suficiente para almecenar la edad de
 * cualquier persona (con 8 bits se pueden almacenar en la variable números entre el 0
 * y el 255).
 *
 * 2. Variable days_per_year: En esta variable se almacenará la cantidad de días por año.
 * Se define como una variable tipo short, ya que cada año cuenta con 365 días;
 * al ser el número 365 mayor a 255, se necesita un tamaño más grande que 8 bits, por tanto
 * se define como una variable tipo short con un tamaño de 16 bits.
 *
 * 3. Variable seconds_per_hour: En esta variable se almacenará la cantidad de segundos
 * por hora. Esta variable se define como una variable de tipo short, ya que al ser 3600
 * segundos por hora 8 bits serán insuficientes para almacenar esta variable pero 16
 * bits serán suficientes (3600 es un número menor a (2^16-1)).*/

unsigned char age = 0;              //Se define variable para mi edad en años.
unsigned short days_per_year = 0;   //Se define variable para la cantidad de días por año.
unsigned short seconds_per_hour = 0;//Se define variable para la cantidad de segundos por hora.

/* Como se puede ver en las 3 líneas anteriores las variables se definen e inmediatamente se
 * les asigna un valor de 0, para inicializarlas adecuadamente.*/

/* Ejercicio 2: Definiendo los días que han pasado desde la fecha de nacimiento.
 *
 * A cerca de la elección del tamaño de la variable:
 *
 * Variable days_since_born: Esta variable almacena el número de días que han pasado
 * desde la fecha de nacimiento. Se le asigna un tamaño short, ya que el número de días
 * que han pasado desde la fecha de nacimiento es mayor a 255 (el valor que puede
 * almacenar una variable tipo char) y menor a 2^16-1 (el valor que puede almacenar una
 * variable tipo short).
 *
 * Con respecto al cálculo de los días que han pasado: para este ejercicio se asume que
 * cada año cuenta con 365 días y se trabaja con la siguiente fecha y hora de nacimiento:
 * 09 de Enero/2000 a las 09:00 horas. Para realizar el cálculo, se tienen en cuenta entonces
 * los siguientes aspectos:
 *
 * 1. Días que pasaron desde el 09/01/2000 hasta el 09/01/2022: A las 09:00 horas del
 * 09/01/2022 se cumplen 22 años desde la fecha de nacimiento, así, el número de días desde
 * la fecha de nacimiento hasta el 09/01/2022 será igual a: age*days_per_year, es decir, es
 * la multiplicación de los años que han pasado por el número que representa mi edad en años.
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

/* Ejercicio 4: Definiendo los segundos que han pasado desde la fecha de nacimiento.
 *
 * A cerca de la elección del tamaño de la variable:
 *
 * 1.Variable seconds_since_born: En esta variable se almacenan el número de segundos pasados
 * desde la fecha de nacimiento. Este valor se estima que es mayor al valor 2^16 - 1,
 * por lo tanto se define una variable de tipo int y con un tamaño de 32 bits.
 *
 * Para calcular el número de segundos que han pasado: El cálculo que se hace es el
 * siguiente: seconds_since_born = hours_since_born*seconds_per_hour.*/

unsigned int seconds_since_born = 0; //Se define variable para los segundos desde el nacimiento.


/*A continuación se definen algunas variables auxiliares que se usarán más adelante para
 * realizar algunos cálculos*/

unsigned short bin_days_since_born = 0;        //Esta es una variable auxiliar en la que se almacenarán
										       //datos que serán posteriormente descartados y será de utilidad
                                               //a la hora de realizar operaciones binarias con la variable
                                               //days_since_born.
unsigned int bin_hours_since_born = 0; 	       //Variable auxiliar para realizar operaciones binarias con la variable
										       //hours_since_born.
unsigned int hex_seconds_since_born = 0;       //Variable auxiliar para realizar operaciones con la variable
										       //seconds_since_born.
unsigned int int_bin_days_since_born = 0;      //Variable auxiliar de tipo int para realizar operaciones con la variable
										       //days_since_born.
unsigned short not_days_since_born = 0;        //Variable para almacenar el valor inverso de la variable days_since_born.
unsigned int pos1_pos5_seconds_since_born = 0; //Variable para almacenar las posiciones 1 y 5 del valor Hex de la variable
                                               //seconds_since_born.
unsigned char bin_seconds_per_hour = 0;        //Variable auxiliar para realizar operaciones bitwise con la variable
											   //seconds_per_hour.

int main(void) //Se crea la función main.

{
	/*A continuación, se le asigna un valor distinto de 0 a las variables creadas en el
	 * ejercicio 1: Estas asignaciones a las variables se hacen dentro del main.*/

	age = 22;
	days_per_year = 365;
	seconds_per_hour = 3600;

    /*A continuación, se le asigna un valor distinto de 0 a la variable creada para el
	* ejercicio 2, teniendo en cuenta los cálculos anteriormente descritos:*/

	days_since_born = (age*days_per_year) + (31-9) + 181 + 17;
	// Laura, acá te sobra un dia, no deberia ser 17 sino 16 porque del 17 solo han pasado 12 horas...
	// En total debian ser 219 días + 12h y no 220 dias +12h porque el 17 aun no ha terminado....

	/*A continuación, se le asigna un valor distinto de 0 a las variables creadas para el
	* ejercicio 3, teniendo en cuenta los cálculos anteriormente descritos:*/

	hours_per_day = 24;
	hours_since_born = hours_per_day*days_since_born + 3;

	/*A continuación, se le asigna un valor distinto de 0 a la variable creada para el
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
	 * valor 2^n, donde n es el número de divisiones que se han hecho desde la segunda
	 * hasta obtener nuevamente el residuo 1.
	 *
	 * 3. Se debe seguir con el procedimiento anterior hasta obtener de las divisiones un
	 * número menor que 1.
	 *
	 * Para clarificar un poco el procedimiento se deja el siguiente ejemplo: sea el número
	 * decimal 13. Al hacer la operación 13/2, se tiene como resultado entero el 6, con un
	 * residuo de 1, este residuo se da porque el número es impar, así, ya se sabe que
	 * en la descomposición del número debe sumarse un 2^0. Luego, se sigue dividiendo: 6/2=3,
	 * como no se obtiene ningún residuo se sigue dividiendo, 3/2=1, esta operación sí tiene
	 * residuo, esto quiere decir que a la descomposición se le debe sumar el valor 2^2. Luego
	 * se sigue dividiendo: 1/2, donde ya se obtiene un número menor a 1 y también se tiene
	 * un residuo, por lo tanto se deberá sumar también 2^3. Así, la descomposición del número
	 * es la siguiente: 13 = 2^0 + 2^2 + 2^3.
	 *
	 * Al tener la descomposición del número, lo único que se debe hacer después es poner 1
	 * en las posiciones que representan las potencias obtenidas en la operación anterior,
	 * así, para el ejemplo anterior se pondría 1 en las posiciones 0, 2 y 3, quedando el número
	 * binario: 1101.
	 *
	 * Realizando el procedimiento anterior para las variables days_since_born y hours_since_born su
	 * correspondiente valor en binario es: days_since_born = 0b10000000111010 y
	 * hours_since_born = 0b110000010101110011;
	 */

	bin_days_since_born = 0b10000000111010;
	bin_hours_since_born = 0b110000010101110011;
	// Esto no lo deberias hacer porque estas sobreescribiendo las variables. Los valores deberian venir de un calculo
	// Estos valores deberian estar mostrados en un comentario.
	// Además, tienes un error de 1 dia, o sea 24 horas, o sea un monton de segundos de mas.

	/*Ahora, para expresar el número de segundos en Hexadecimal:
	 * Básicamente el procedimiento es el mismo explicado anteriormente, ya que cuando se pasa
	 * de decimal a binario se está cambiando el número de base 10 a base 2, al pasar de decimal a
	 * hexadecimal, se hace el mismo procedimiento, pero ya teniendo en cuenta que se va a pasar de
	 * base 10 a base 16, así, se debe dividir en vez de entre 2, entre 16 y ahora los posibles residuos
	 * serán números entre el 0 y F (A = 10, B = 11, C = 12, D = 13, E = 14, F = 15).
	 *
	 * Realizando este procedimiento, se obtiene el número: hex_seconds_since_born = 0x2A7CA130*/

	hex_seconds_since_born = 0x2A7CA130;
	// De nuevo, la nota de arriba.

	/*A cerca del tipo de variable:
	 *
	 * Como se puede ver, se definieron las siguientes variables: bin_days_since_born, bin_hours_since_born
	 * y hex_seconds_since_born. Estas variables se asignan con los mismos datos que days_since_born,
	 * hours_since_born y seconds_since_born, respectivamente; al almacenar las mismas cantidades (en binario,
	 * hexagonal o hexadecimal la cantidad será la misma, solo cambia la representación) no es necesario hacer
	 * un cambio en el tipo de variable con el que se definen (short, int y int, respectivamente).*/


	/*Ejercicio 6: Operación Shift izquierda.
	 *
	 * Al aplicarla una vez: Al aplicar la operación una vez, el valor de la variable se multiplica por 2,
	 * esto pasa porque la variable tiene el siguiente valor: 0b0010000000111010, al aplicarle la operación,
	 * todos los valores se correrán hacia la izquierda, es decir, quedará como resultado el número
	 * 0b0100000001110100 y todos los 1 pasarán a multiplicar por una potencia de 2 un número más alta.
	 *
	 * Al aplicarla dos veces: La segunda vez que se aplica, el número vuelve a duplicarse, ya que los números
	 * volverán a moverse una posición y quedará el número 0b0100000001110100.
	 *
	 * Al aplicarla cuatro veces seguidas: Al aplicar la operación cuatro veces seguidas se observa un comportamiento
	 * bastante particular: el número binario que queda es el 0b1110100000, esto pasa porque al realizar el tercer shift
	 * hay un desbordamiento, entonces como la variable tiene una cantidad de bits limitados ese 1 se pierde, así, al valor
	 * decimal de la variable se le restará el valor 2^n, donde n es la posición que tenía el número que se perdió por desbordamiento
	 * y el valor restante se multiplicará por dos, ya que el resto de números se moverán hacia la izquierda y quedarán en una
	 * potencia de 2 más alta (valor obtenido por el tercer shift = (valor anterior - 2^n)*2). Al aplicar el cuarto shift se volverá
	 * al patrón anterior de multiplicar por 2, ya que ahora sí se tiene el espacio para mover los números sin generar un
	 * desbordamiento*/

	bin_days_since_born = days_since_born << 1;
	bin_days_since_born = bin_days_since_born << 1;
	bin_days_since_born = bin_days_since_born << 1;
	bin_days_since_born = bin_days_since_born << 1;

	/*Si se desea multiplicar por 2 sin tener el problema del overflow, lo que se debe hacer es darle un tamaño más
	 * grande a la variable con la que se opera, así no habrá ningún desbordamiento y se podrá multiplicar por dos
	 * los datos la cantidad de veces que se requiera (hasta alcanzar el tamaño máximo de la nueva variable), a continuación,
	 * se realiza la operación nuevamente, pero con la variable int_bin_days_since_born, la cual se definió con un tamaño mayor.*/

	int_bin_days_since_born = days_since_born << 1;
	int_bin_days_since_born = int_bin_days_since_born << 1;
	int_bin_days_since_born = int_bin_days_since_born << 1;
	int_bin_days_since_born = int_bin_days_since_born << 1;

	/*En esta ocasión, se puede observar que al aplicar la operación 4 veces se obtiene el número original multiplicado
	 * por el valor 2^4.*/
	// Excelente!


	/*Ejercicio 7: Operación Shift derecha.
	 *
	 * Al realizar la operación una vez se divide el valor original entre dos, al hacerla una segunda
	 * vez, se divide el valor entre 2 una segunda vez y al hacerlo 4 veces seguidas, el valor original
	 * queda divido entre el valor 2^4 y se podría seguir haciendo la operación hasta obtener el mínimo valor.*/

	bin_hours_since_born = hours_since_born >> 1;
	bin_hours_since_born = bin_hours_since_born >> 1;
	bin_hours_since_born = bin_hours_since_born >> 1;
	bin_hours_since_born = bin_hours_since_born >> 1;

    /*Ejercicio 8: Al aplicar el operador NOT a la variable, sumarle 1 y luego sumarle la variable original
     * se obtiene un resultado de 0. De esto se puede interpretar que, al aplicarle not a un valor y sumarle 1
     * se está tomando la representación negativa del número y por ello al sumar con la variable original
     * se obtiene el 0. Sin embargo, más precisamente lo que pasa es que al invertir el valor y sumarle 1 estamos
     * encontrando el complemento, es decir, se está encontrando la cantidad que le hace falta a la variable para
     * llegar a la cantidad máxima que esos bits pueden almacenar, así, cuando se le suma el valor original
     * hay un debordamiento (se está alcanzando la máxima cantidad que la variable está en capacidad de almacerna)
     * y la variable queda en 0.*/

	not_days_since_born = ~ days_since_born + 1;
	bin_days_since_born = not_days_since_born + days_since_born;

	/*Al sumarle un valor diferente de 1 ya el resultado no es 0 sino que será igual al valor sumado - 1.*/

	not_days_since_born = ~ days_since_born + 2;
	bin_days_since_born = not_days_since_born + days_since_born;

	/*Ejercicio 9: El valor en hexadecimal es 0x2A7CA130, la cuarta posición tiene el valor C (equivalente a 12), así que es par.
	 * Se aplica entonces la máscara para obtener 1 y 5. La máscara elegida es la 0x00F000F0, esta máscara se elige de tal forma
	 * que si no se conoce cuál es el valor del hexadecimal en las posiciones 1 y 5, igual se pueda obtener (al poner la F se
	 * está aplicando la máscara binaria 1111 a esa posición hexadecimal) y se pone la máscara con el operador and.*/

	pos1_pos5_seconds_since_born = seconds_since_born & 0x00F000F0;
	// Y cual es el resultado?
	// Ademas, tu valor es incorrecto, en el caso correcto el número es 0cB y todo cambia...

	/*Ejercicio 10: Se le aplica la máscara 0x040 a la variable número de segundos.
	 *
	 * Al aplicar esta máscara, se debe tener en cuenta que se está aplicando bit a bit, es decir que esta máscara es equivalente
	 * a aplicar la máscara: 0b000001000000. El resultado obtenido es igual a 0, por lo tanto se aplica una máscara para eliminar
	 * la posición 3 del hexadecimal. La máscara elegida es 0xFFFF0FFF (se eligen las F por lo que se explicó anteriormente).*/

	hex_seconds_since_born = seconds_since_born & 0x040;
	hex_seconds_since_born = seconds_since_born & 0xFFFF0FFF;
	// De nuevo, cual es el resultado? solo lo dejas indicado...
	// Además, segun m

	/*Ejercicio 11: Se utilizará la variable seconds_per_hour.
	 *
	 *Al aplicarle el operandor a la variable una vez lo que se obtiene es un 0 y una segunda vez se obtiene un 1. Este es un
	 *un operador booleano, es decir, solo tiene dos valores posible como resultado y en vez de trabajar con los valores de los bits,
	 *este operador toma la cantidad de la variable completa. Este operador en particular, lo que hace es retornar False (0) cuando la
	 *entrada sea true (número diferente de 0) y retornar True (1) cuando la entrada sea false (0), se podría decir que es como el
	 *operador booleano que hace los papeles de NOT.*/

	bin_seconds_per_hour = ! seconds_per_hour;
	bin_seconds_per_hour = ! bin_seconds_per_hour;

	while (1) { //Se le pone el ciclo infinito
		;
	}
}
