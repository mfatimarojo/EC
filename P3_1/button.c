/*-------------------------------------------------------------------
 **
 **  Fichero:
 **    button.c  10/6/2014
 **
 **    Estructura de Computadores
 **    Dpto. de Arquitectura de Computadores y Automï¿½tica
 **    Facultad de Informï¿½tica. Universidad Complutense de Madrid
 **
 **  Propï¿½sito:
 **    Contiene las implementaciones de las funciones
 **    para la gestiï¿½n de los pulsadores de la placa de prototipado
 **
 **  Notas de diseï¿½o:
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

	//COMPLETAR: tomar el cÃ³digo de la primera parte
	portG_read(6, &val);

	if (val == HIGH) {	//Si el pin 6 está a 1, ponemos el bit 0 de buttons a 1
		buttons |= BUT1;
	}

	portG_read(7, &val); //Si el pin 7 está a 1, ponemos el bit 1 de buttons a 1
	if (val == HIGH) {
		buttons |= BUT2;
	}
	return buttons;
}

