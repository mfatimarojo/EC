#include <stdio.h>
#include "44b.h"
#include "leds.h"
#include "utils.h"
#include "D8Led.h"
#include "intcontroller.h"
#include "timer.h"
#include "gpio.h"
#include "keyboard.h"
#include "uart.h"

#define N 4 //Tamaño del buffer tmrbuffer
#define A 10
#define C 12
#define E 14
#define F 15
#define M 128 //Tamaño del buffer readlineBuf que se pasa como parámetro a la rutina readline
/* Variables para la gestión de la ISR del teclado
 * 
 * Keybuffer: puntero que apuntará al buffer en el que la ISR del teclado debe
 *            almacenar las teclas pulsadas
 * keyCount: variable en el que la ISR del teclado almacenará el número de teclas pulsadas
 */
volatile static char *keyBuffer = NULL;
volatile static int keyCount = 0;

/* Variables para la gestion de la ISR del timer
 * 
 * tmrbuffer: puntero que apuntará al buffer que contendrá los dígitos que la ISR del
 *            timer debe mostrar en el display de 8 segmentos
 * tmrBuffSize: usado por printD8Led para indicar el tamaño del buffer a mostrar
 */
volatile static char *tmrBuffer = NULL;
volatile static int tmrBuffSize = 0;

//Variables globales para la gestión del juego
static char passwd[N];  //Buffer para guardar la clave inicial
static char guess[N];   //Buffer para guardar la segunda clave
char readlineBuf[M];    //Buffer para guardar la linea leída del puerto serie

//Configuración de la uart
struct ulconf uconf = { .ired = OFF, .par = NONE, .stopb = 1, //¿¿Cambiar a 0 para que solo haya un bit de parada?
		.wordlen = EIGHT, .echo = ON, .baud = 115200, };

enum state {
	INIT = 0,     //Init:       Inicio del juego
	SPWD = 1,     //Show Pwd:   Mostrar password
	DOGUESS = 2,  //Do guess:   Adivinar contraseña
	SGUESS = 3,   //Show guess: Mostrar el intento
	GOVER = 4     //Game Over:  Mostrar el resultado
};
enum state gstate; //estado/fase del juego 

void timer_ISR(void) __attribute__ ((interrupt ("IRQ")));
void keyboard_ISR(void) __attribute__ ((interrupt ("IRQ")));

// Función que va guardando las teclas pulsadas
static void push_buffer(char *buffer, int key) {
	int i;
	for (i = 0; i < N - 1; i++)
		buffer[i] = buffer[i + 1];
	buffer[N - 1] = (char) key;
}

void timer_ISR(void) {
	static int pos = 0; //contador para llevar la cuenta del dígito del buffer que toca mostrar

	D8Led_digit(tmrBuffer[pos]);
	pos++;

	if (pos == tmrBuffSize) {
		pos = 0;
		tmr_stop(TIMER0);
		tmrBuffer = NULL;
	}

	ic_cleanflag(INT_TIMER0);
}

void printD8Led(char *buffer, int size) {
	//Esta rutina prepara el buffer que debe usar timer_ISR (tmrBuffer)
	tmrBuffer = buffer;
	tmrBuffSize = size;

	tmr_update(TIMER0);
	tmr_start(TIMER0);
	while (tmrBuffer != NULL ) {
	};
}

void keyboard_ISR(void) {
	int key;

	Delay(200);

	/* Escaneo de tecla */
	key = kb_scan();
	if (key != -1) {
		if (key == F) {
			ic_disable(INT_EINT1);
			keyBuffer = NULL;
		} else {
			push_buffer(keyBuffer, key);
			keyCount++;
		}

		/* Esperar a que la tecla se suelte, consultando el registro de datos rPDATG */
		while (!(rPDATG & 0x02))
			;
	}

	Delay(200);

	ic_cleanflag(INT_EINT1);
}

int read_kbd(char *buffer) {
	//Esta rutina prepara el buffer en el que keyboard_ISR almacenará las teclas 
	//pulsadas (keyBuffer) y pone a 0 el contador de teclas pulsadas
	keyBuffer = buffer;
	keyCount = 0;

	ic_enable(INT_EINT1);

	while (keyBuffer != NULL ) {
	};

	return keyCount;
}

int readline(char *buffer, int size) {
	int count = 0; //cuenta del número de bytes leidos
	char c;        //variable para almacenar el carácter leído

	if (size == 0)
		return 0;

	// COMPLETAR: Leer caracteres de la uart0 y copiarlos al buffer
	// hasta que llenemos el buffer (size) o el carácter leído sea
	// un retorno de carro '\r'
	// Los caracteres se leen de uno en uno, utilizando la interfaz
	// del módulo uart, definida en el fichero uart.h

	c = 0;
	while ((count < size) && (c != '\r')) {
		uart_getch(UART0, &c);
		buffer[count] = c;
		count++;
	}

	return count;
}

static int show_result() {
	int error = 0;
	int i = 0;
	char buffer[2] = { 0 };

	// COMPLETAR: poner error a 1 si las contraseñas son distintas
	while (i < N && !error) {
		if (passwd[i] != guess[i])
			error = 1;
		i++;
	}

	// COMPLETAR
	// MODIFICAR el código de la parte1 para que además de mostrar A o E en el
	// display de 8 segmentos se envíe por el puerto serie uart0 la cadena "\nCorrecto\n"
	// o "\nError\n" utilizando el interfaz del puerto serie definido en uart.h

	if (!error) {
		buffer[0] = 10;
		buffer[1] = 10;
		uart_send_str(UART0, "\nCorrecto\n");
	} else {
		buffer[0] = 14;
		buffer[1] = 14;
		uart_send_str(UART0, "\nError\n");
	}
	printD8Led(buffer, 2);

	// COMPLETAR: esperar a que la ISR del timer indique que se ha terminado
	ic_cleanflag(INT_TIMER0);

	// COMPLETAR: Devolver el valor de error para indicar si se ha acertado o no
	return error;
}

void irq_ISR(void) {
	//CODIGO DE LA PRÁCTICA 2 PARTE 2 MODIFICADO PARA TECLADO EN LUGAR DE BUTTON
	int bit = rI_ISPR;

	//Debemos ver si la interrupción que debemos atender (bit) es la
	//de la línea INT_TIMER0 and INT_EINT1. Si es la del timer se invocarÁ la
	//función timer_ISR y después se borrará el flag de interrupción utilizando
	//el interfaz definido en intcontroller.h. Si es la de EINT1 se invocará
	//la función keyboard_ISR y se borrará el flag correspondiente utilizando el
	//mismo interfaz.

	if (bit == INT_TIMER0) {
		timer_ISR();
		ic_cleanflag(INT_TIMER0);

	} else if (bit == INT_EINT1) {
		keyboard_ISR();
		ic_cleanflag(INT_EINT1);
	}

}

int setup(void) {

	D8Led_init();

	/* COMPLETAR: Configuración del timer0 para interrumpir cada segundo */

	// Código de la parte1
	tmr_set_prescaler(TIMER0, 255);
	tmr_set_divider(TIMER0, D1_4);
	tmr_set_count(TIMER0, 62500, 31250);
	tmr_update(TIMER0);
	tmr_set_mode(TIMER0, RELOAD);
	tmr_stop(TIMER0);
	/********************************************************************/
	portG_conf(1, EINT);//El pin 1 activa la línea EINT1 cuando se genere un flanco de bajada
	portG_eint_trig(1, FALLING);
	portG_conf_pup(1, ENABLE);

	// COMPLETAR: Registramos las ISRs
	// Código de la parte1
	pISR_EINT1 = (unsigned) keyboard_ISR;	//Para el teclado
	pISR_TIMER0 = (unsigned) timer_ISR;		//Para el timer

	/* Configuración del controlador de interrupciones*/
	ic_init();

	// Código de la parte1
	ic_conf_irq(ENABLE, VEC);
	ic_conf_fiq(DISABLE);
	ic_conf_line(INT_TIMER0, IRQ);
	ic_enable(INT_TIMER0);
	ic_conf_line(INT_EINT1, IRQ);
	ic_enable(INT_EINT1);
	/***************************************************/

	/***************************************************/
	//COMPLETAR: Configuración de la uart0
	//Hay que:
	uart_init();					//1. inicializar el módulo
	uart_lconf(UART0, &uconf);		//2. Configurar el modo linea de la uart0 usando la variable global uconf
	uart_conf_rxmode(UART0, POLL);	//3. Configurar el modo de recepción (POLL o INT) de la uart0
	uart_conf_txmode(UART0, INT);	//4. Configurar el modo de transmisión (POLL o INT) de la uart0

	/***************************************************/

	Delay(0);

	/* Inicio del juego */
	gstate = INIT;
	D8Led_digit(C);

	return 0;
}

static char ascii2digit(char c) {
	char d = -1;

	if ((c >= '0') && (c <= '9'))
		d = c - '0';
	else if ((c >= 'a') && (c <= 'f'))
		d = c - 'a' + 10;
	else if ((c >= 'A') && (c <= 'F'))
		d = c - 'A' + 10;

	return d;
}

int loop(void) {
	int count; //número de teclas pulsadas
	int error;

	//Máquina de estados

	switch (gstate) {
	case INIT:
		do {
			D8Led_digit(C);
			count = read_kbd(passwd);
			if (count < N)
				D8Led_digit(E);
		} while (count < N);
		gstate = SPWD;
		break;

	case SPWD:
		printD8Led(passwd, N);
		Delay(10000);
		gstate = DOGUESS;
		break;

	case DOGUESS:
		Delay(10000);
		do {
			//COMPLETAR:
			/*
			 * 1. Mandar por el puerto serie uart0 la cadena "Introduzca passwd: "
			 *    usando el interfaz definido en uart.h
			 */
			char cadena[] = "Introduzca passwd: ";
			uart_send_str(UART0, cadena);
			/*
			 * 2. Mostrar una F en el display
			 */
			D8Led_digit(F);
			/*
			 * 3. Llamar a la rutina readline para leer una línea del puerto
			 *    serie en el buffer readlineBuf, almacenando en count el
			 *    valor devuelto (número de caracteres leídos)
			 */
			count = readline(readlineBuf, M);

			/*
			 * 4. Si el último caracter leído es un '\r' decrementamos count
			 *    para no tenerlo en cuenta
			 */
			if (readlineBuf[count - 1] == '\r')
				count--;

			/*
			 * 5. Si count es menor de 4 la clave no es válida, mostramos
			 *    una E (digito 14) en el display de 8 segmentos y esperamos
			 *    1 segundo con Delay.
			 */
			if (count < N) {
				D8Led_digit(E);
				Delay(10000);
			}
		} while (count < N);

		/* COMPLETAR: debemos copiar los 4 últimos caracteres de readline en
		 * el buffer guess, haciendo la conversión de ascii-hexadecimal a valor
		 * decimal. Para ello podemos utilizar la función ascii2digit
		 * definida más arriba.
		 */

		//DUDA: ¿4 últimos caracteres posiciones 127, 126, 125, 124, o viceversa?
		int i;
		for (i = 0; i < N; i++) {
			guess[i] = ascii2digit(readlineBuf[count - N + i]);
		}
		gstate = SGUESS;
		break;

	case SGUESS:
		printD8Led(guess, N);
		Delay(10000);
		gstate = GOVER;
		break;

	case GOVER:
		error = show_result();
		Delay(10000);
		if (error)
			gstate = DOGUESS;
		else
			gstate = INIT;

		break;
	}
	return 0;
}

int main(void) {
	setup();

	while (1) {
		loop();
	}
}
