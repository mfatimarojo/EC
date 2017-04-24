

#include "44b.h"
#include "utils.h"
#include "button.h"
#include "leds.h"
#include "gpio.h"

unsigned int read_button(void)
{
	unsigned int buttons = 0;
	enum digital val;

	//COMPLETAR utilizando el interfaz del puerto G de gpio.h debemos leer los
	//pines 6 y 7 del puerto G (portG_read) debemos devolver un valor (buttons)
	//en el que el bit 0 (el menos significativo) representa el estado del botÃ³n
	//del pin 6 y el bit 1 representa el estado del botÃ³n del pin 7 (a 1 si
	//estÃ¡n pulsados a 0 si no lo estÃ¡n).

	portG_read(6, &val);
	if (!val) {	//Si el pin 6 está a 1, ponemos el bit 0 de buttons a 1
		buttons |= BUT1;
	}
	else {
		buttons &= ~BUT1;
	}

	portG_read(7, &val);  //Si el pin 7 está a 1, ponemos el bit 1 de buttons a 1
	if (!val) {
		buttons |= BUT2;
	}
	else {
		buttons &= ~BUT2;
	}

	return buttons;
}


