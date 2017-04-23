#include <stdio.h>
#include "44b.h"
#include "button.h"
#include "leds.h"
#include "utils.h"
#include "D8Led.h"
#include "intcontroller.h"
#include "timer.h"
#include "gpio.h"
#include "keyboard.h"

struct RLstat {
	int moving;
	int speed;
	int direction;
	int position;
};

static struct RLstat RL = { .moving = 0, .speed = 5, .direction = 0, .position =
		0, };

void timer_ISR(void) __attribute__ ((interrupt ("IRQ")));
void button_ISR(void) __attribute__ ((interrupt ("IRQ")));
void keyboard_ISR(void) __attribute__ ((interrupt ("IRQ")));

void timer_ISR(void) {
	if (RL.direction) {
		if (RL.position == 5)
			RL.position = 0; //Rotatorio
		else
			RL.position++;
	} else {
		if (RL.position == 0)
			RL.position = 5; //Rotatorio
		else
			RL.position--;
	}

	D8Led_segment(RL.position);

	ic_cleanflag(INT_TIMER0);
}

void button_ISR(void) {
	unsigned int whicheint = rEXTINTPND;
	unsigned int buttons = (whicheint >> 2) & 0x3;

	if (buttons & BUT1) {
		led1_switch();
		if (RL.direction)
			RL.direction = 0;
		else
			RL.direction = 1;

	}

	if (buttons & BUT2) {
		led2_switch();
		if (RL.moving) {
			RL.moving = 0;
			tmr_stop(TIMER0);
		}

		else {
			RL.moving = 1;
			tmr_update(TIMER0);
			tmr_start(TIMER0);

		}
	}

	// eliminamos rebotes
	Delay(2000);
	// borramos el flag en extintpnd
	//rEXTINTPND = //COMPLETAR: tomar el código de la parte 2

	//Seg�n la documentaci�n, si una interrupci�n se activa, EXINTPNDn se pone a 1
	//por tanto, si no queremos desactivarlas, habr� que escribir un 0 en el bit correspondiente

	/*if (buttons & BUT1) {	//Si se puls� el bot�n 1, poner a 0 el bit 0
		rEXTINTPND &= ~(0x1 << 0);
	}
	if (buttons & BUT2) {	//Si se puls� el bot�n 2, poner a 0 el bit 1
		rEXTINTPND &= ~(0x1 << 1);
	}*/

	rEXTINTPND = whicheint;

	ic_cleanflag(INT_EINT4567);

}

void keyboard_ISR(void) {
	int key;

	/* Eliminar rebotes de presión */
	Delay(200);

	/* Escaneo de tecla */
	key = kb_scan();

	if (key != -1) {
		/* Visualizacion en el display */
		//COMPLETAR: mostrar la tecla en el display utilizando el interfaz
		//definido en D8Led.h
		D8Led_digit(key);

		switch (key) {
		case 0:
			//COMPLETAR: poner en timer0 divisor 1/8 y contador 62500
			tmr_set_divider(TIMER0, D1_8);
			tmr_set_count(TIMER0, 362500, 31250);
			break;
		case 1:
			//COMPLETAR: poner en timer0 timer divisor 1/8 y contador 31250
			tmr_set_divider(TIMER0, D1_8);
			tmr_set_count(TIMER0, 31250, 15625);
			break;
		case 2:
			//COMPLETAR: poner en timer0 timer divisor 1/8 y contador 15625
			tmr_set_divider(TIMER0, D1_8);
			tmr_set_count(TIMER0, 15625, 7812);
			break;
		case 3:
			//COMPLETAR: poner en timer0 timer divisor 1/4 y contador 15625
			tmr_set_divider(TIMER0, D1_4);
			tmr_set_count(TIMER0, 15625, 7812);
			break;
		default:
			break;
		}

		/* Esperar a que la tecla se suelte, consultando el registro de datos */
		while (!(rPDATG & 0x02)) {
		};
	}

	/* Eliminar rebotes de depresión */
	Delay(200);

	/* Borrar interrupciones pendientes */
	//COMPLETAR
	//borrar la interrupción por la línea EINT1 en el registro rI_ISPC
	ic_cleanflag(INT_EINT1);
}

int setup(void) {
	leds_init();
	D8Led_init();
	D8Led_segment(RL.position);

	/* Port G: configuración para generación de interrupciones externas,
	 *         botones y teclado
	 **/

	//COMPLETAR: utilizando el interfaz para el puerto G definido en gpio.h
	//configurar los pines 1, 6 y 7 del puerto G para poder generar interrupciones
	//externas por flanco de bajada por ellos y activar las correspondientes
	//resistencias de pull-up.
	portG_conf(1, EINT);
	portG_eint_trig(1, FALLING);
	portG_conf_pup(1, ENABLE);

	portG_conf(6, EINT);
	portG_eint_trig(6, FALLING);
	portG_conf_pup(6, ENABLE);

	portG_conf(7, EINT);
	portG_eint_trig(7, FALLING);
	portG_conf_pup(7, ENABLE);

	/********************************************************************/

	/* Configuración del timer */

	tmr_set_prescaler(TIMER0, 255);			//valor de prescalado a 255
	tmr_set_divider(TIMER0, D1_8);			//valor del divisor 1/8
	tmr_set_count(TIMER0, 62500, 31250);//valor de cuenta 62500 y cualquier valor de comparación entre 1 y 62499
	tmr_update(TIMER0);	//actualizar el contador con estos valores (update)
	tmr_set_mode(TIMER0, RELOAD);		//poner el contador en modo RELOAD
	tmr_stop(TIMER0);						//dejar el contador parado

	//int tmr_isrunning(enum tmr_timer t);

	if (RL.moving)
		tmr_start(TIMER0);
	/***************************/

	// Registramos las ISRs
	pISR_TIMER0 = (unsigned) timer_ISR;	//COMPLETAR: registrar la RTI del timer
	pISR_EINT4567 = (unsigned) button_ISR;//COMPLETAR: registrar la RTI de los botones
	pISR_EINT1 = (unsigned) keyboard_ISR;//COMPLETAR: registrar la RTI del teclado

	/* Configuración del controlador de interrupciones
	 * Habilitamos la línea IRQ, en modo vectorizado y registramos una ISR para
	 * la línea IRQ
	 * Configuramos el timer 0 en modo IRQ y habilitamos esta línea
	 * Configuramos la línea EINT4567 en modo IRQ y la habilitamos
	 * Configuramos la línea EINT1 en modo IRQ y la habilitamos
	 */

	ic_init();
	//COMPLETAR: utilizando el interfaz definido en intcontroller.h
	ic_conf_irq(ENABLE, VEC);			// habilitar la línea IRQ en modo  vectorizado
	ic_conf_fiq(DISABLE);				// deshabilitar la línea FIQ
	ic_conf_line(INT_TIMER0, IRQ);		// configurar la línea INT_TIMER0 en modo IRQ
	ic_conf_line(INT_EINT4567, IRQ);	// configurar la línea INT_EINT4567 en modo IRQ
	ic_conf_line(INT_EINT1, IRQ);		// configurar la línea INT_EINT1 en modo IRQ

	ic_enable(INT_TIMER0);				// habilitar la línea INT_TIMER0
	ic_enable(INT_EINT4567);			// habilitar la línea INT_EINT4567
	ic_enable(INT_EINT1);				// habilitar la línea INT_EINT1

	/***************************************************/

	Delay(0);
	return 0;
}

int loop(void) {
	return 0;
}

int main(void) {
	setup();

	while (1) {
		loop();
	}
}
