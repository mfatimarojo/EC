/*-------------------------------------------------------------------
 **
 **  Fichero:
 **    intcontroller.c  3/3/2016
 **
 **    Estructura de Computadores
 **    Dpto. de Arquitectura de Computadores y Autom谩tica
 **    Facultad de Inform谩tica. Universidad Complutense de Madrid
 **
 **  Prop贸sito:
 **    Contiene las implementaci贸n del m贸dulo intcontroller
 **
 **-----------------------------------------------------------------*/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "intcontroller.h"

void ic_init(void) {
	/* Configuraci贸n por defector del controlador de interrupciones:
	 *    Lineas IRQ y FIQ no habilitadas
	 *    Linea IRQ en modo no vectorizado
	 *    Todo por la l铆nea IRQ
	 *    Todas las interrupciones enmascaradas
	 **/
	rINTMOD = 0x0; // Configura las linas como de tipo IRQ
	rINTCON = 0x7; // IRQ y FIQ enmascaradas, IRQ en modo no vectorizado
	rINTMSK = ~(0x0); // Enmascara todas las lineas
}

int ic_conf_irq(enum enable st, enum int_vec vec) {
	int conf = rINTCON;

	if (st != ENABLE && st != DISABLE)
		return -1;

	if (vec == VEC)
		//COMPLETAR: poner la linea IRQ en modo vectorizado
		conf &= ~(INT_BIT(2));  //Pone a 0 el bit 2 para el modo vectorizado
	//Si hubiera que dejar la l韓ea FIQ enmascarada (1)??????

	else
		//COMPLETAR: poner la linea IRQ en modo no vectorizado
		conf |= INT_BIT(2);		//Pone a 1 el bit 2 para el modo no vectorizado

	if (st == ENABLE)
		//COMPLETAR: habilitar la linea IRQ
		conf &= ~(INT_BIT(1));	//Pone a 0 el bit 1 para habilitar IRQ

	else
		//COMPLETAR: deshabilitar la linea IRQ
		conf |= INT_BIT(1);		//Pone a 1 el bit 1 para deshabilitar IRQ

	rINTCON = conf;
	return 0;
}

int ic_conf_fiq(enum enable st) {
	int ret = 0;

	int conf = rINTCON;

	if (st == ENABLE)
		//COMPLETAR: habilitar la linea FIQ
		conf &= ~(INT_BIT(0));	//Pone a 0 el bit 0 para habilitar FIQ

	else if (st == DISABLE)
		//COMPLETAR: deshabilitar la linea FIQ
		conf |= INT_BIT(0);	//Pone a 1 el bit 0 para habilitar FIQ

	else
		ret = -1;

	rINTCON = conf;

	return ret;
}

int ic_conf_line(enum int_line line, enum int_mode mode) {
	unsigned int bit = INT_BIT(line);

	if (line < 0 || line > 26)
		return -1;

	if (mode != IRQ && mode != FIQ)
		return -1;

	if (mode == IRQ)
		//COMPLETAR: poner la linea line en modo IRQ
		rINTMOD &= ~bit;	//0 activa la linea IRQ
	else
		//COMPLETAR: poner la linea line en modo FIQ
		rINTMOD |= bit;	//1 activa la linea FIQ

	return 0;
}

int ic_enable(enum int_line line) {
	if (line < 0 || line > 26)
		return -1;

	//COMPLETAR: habilitar las interrupciones por la linea line
	//0 habilita, 1 enmascara
	rINTMSK &= ~INT_BIT(26);  //Habilitamos el bit global
	rINTMSK &= ~INT_BIT(line);	//Habilitamos la interrupcion por la l韓ea line

	return 0;
}

int ic_disable(enum int_line line) {
	if (line < 0 || line > 26)
		return -1;

	//COMPLETAR: enmascarar las interrupciones por la linea line
	rINTMSK |= INT_BIT(line);  //Enmascaramos la interrupci髇 por la l韓ea line

	return 0;
}

int ic_cleanflag(enum int_line line) {
	int bit;

	if (line < 0 || line > 26)
		return -1;

	bit = INT_BIT(line);

	if (rINTMOD & bit)
	//COMPLETAR: borrar el flag de interrupcion correspondiente a la linea line
	//con la linea configurada por FIQ
	//0 habilita, 1 deshabilita
		rF_ISPC = bit;
	else
	//COMPLETAR: borrar el flag de interrupcion correspondiente a la linea line
	//con la linea configurada por IRQ
	//0 habilita, 1 deshabilita
		rI_ISPC = bit;
	return 0;
}

