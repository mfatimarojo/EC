/*-------------------------------------------------------------------
**
**  Fichero:
**    intcontroller.c  3/3/2016
**
**    Estructura de Computadores
**    Dpto. de Arquitectura de Computadores y Automática
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las implementación del módulo intcontroller
**
**-----------------------------------------------------------------*/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "intcontroller.h"

void ic_init(void)
{
	/* Configuración por defector del controlador de interrupciones:
	 *    Lineas IRQ y FIQ no habilitadas
	 *    Linea IRQ en modo no vectorizado
	 *    Todo por la línea IRQ
	 *    Todas las interrupciones enmascaradas
	 **/
	rINTMOD = 0x0; // Configura las linas como de tipo IRQ
	rINTCON = 0x7; // IRQ y FIQ enmascaradas, IRQ en modo no vectorizado
	rINTMSK = ~(0x0); // Emascara todas las lineas
}

int ic_conf_irq(enum enable st, enum int_vec vec)
{
	int conf = rINTCON;

	if (st != ENABLE && st != DISABLE)
		return -1;

	if (vec == VEC)
		conf &= ~(INT_BIT(2));
	//Si hubiera que dejar la l�nea FIQ enmascarada (1)??????

	else
		conf |= INT_BIT(2);
	//Si hubiera que dejar la l�nea FIQ enmascarada (1)??????


	if (st == ENABLE)
		conf &= ~(INT_BIT(1));
	//Si hubiera que dejar la l�nea FIQ enmascarada (1)??????

	else
		conf |= INT_BIT(1);
	//Si hubiera que dejar la l�nea FIQ enmascarada (1)??????


	rINTCON = conf;
	return 0;
}

int ic_conf_fiq(enum enable st)
{
	int ret = 0;

	int conf = rINTCON;

	if (st == ENABLE)
		conf &= ~(INT_BIT(0));
	else if (st == DISABLE)
		conf |= INT_BIT(0);
	else
		ret = -1;

	rINTCON = conf;

	return ret;
}

int ic_conf_line(enum int_line line, enum int_mode mode)
{
	unsigned int bit = INT_BIT(line);

	if (line < 0 || line > 26)
		return -1;

	if (mode != IRQ && mode != FIQ)
		return -1;

	if (mode == IRQ)
		rINTMOD &= ~bit;
	else
		rINTMOD |= bit;

	return 0;
}

int ic_enable(enum int_line line)
{
	if (line < 0 || line > 26)
		return -1;

	//0 habilita, 1 enmascara
	rINTMSK &= ~INT_BIT(26);  //Habilitamos el bit global
	rINTMSK &= ~INT_BIT(line);	//Habilitamos la interrupcion por la l�nea line

	return 0;
}

int ic_disable(enum int_line line)
{
	if (line < 0 || line > 26)
		return -1;

	rINTMSK |= INT_BIT(line);  //Enmascaramos la interrupci�n por la l�nea line

	
	return 0;
}

int ic_cleanflag(enum int_line line)
{
	int bit;

	if (line < 0 || line > 26)
		return -1;

	bit = INT_BIT(line);

	if (rINTMOD & bit)
		rF_ISPC |= bit;
	else
		rI_ISPC |= bit;

	return 0;
}



