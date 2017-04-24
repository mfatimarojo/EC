/*-------------------------------------------------------------------
 **
 **  Fichero:
 **    button.c  10/6/2014
 **
 **    Estructura de Computadores
 **    Dpto. de Arquitectura de Computadores y Autom�tica
 **    Facultad de Inform�tica. Universidad Complutense de Madrid
 **
 **  Prop�sito:
 **    Contiene las implementaciones de las funciones
 **    para la gesti�n de los pulsadores de la placa de prototipado
 **
 **  Notas de dise�o:
 **
 **-----------------------------------------------------------------*/

#include "44b.h"
#include "utils.h"
#include "button.h"
#include "leds.h"
#include "gpio.h"

unsigned int read_button(void) {
	unsigned int buttons = 0;
	enum digital val;

	//COMPLETAR: tomar el código de la primera parte
	portG_read(6, &val);

	if (val == HIGH) {	//Si el pin 6 est� a 1, ponemos el bit 0 de buttons a 1
		buttons |= BUT1;
	}

	portG_read(7, &val); //Si el pin 7 est� a 1, ponemos el bit 1 de buttons a 1
	if (val == HIGH) {
		buttons |= BUT2;
	}
	return buttons;
}

