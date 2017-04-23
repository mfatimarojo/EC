#include "44b.h"
#include "gpio.h"

/* Port B interface implementation */

int portB_conf(int pin, enum port_mode mode)
{
	int ret = 0;
	if (pin < 0 || pin > 10)
		return -1; // indica error

	if (mode == SIGOUT)
		// COMPLETAR: poner en rPCONB el bit indicado por pin a 1 para que por
		// dicho pin en el puerto B salga la seÃ±al correspondiente del
		// controlador de memoria
		rPCONB |= (0x1 << pin); //Realizar OR entre rPCONB y 1 desplazado pin veces a la izquierda


	else if (mode == OUTPUT)
		// COMPLETAR: poner en rPCONB el bit indicado por pin a 0 para que dicho
		// pin sea un pin de salida
		rPCONB &= ~(0x1 << pin); //Realizar AND entre rPCONB y la negación de desplazar pin veces 1 a la izquierda

	else
		ret = -1; // indica error

	return ret;
}

int portB_write(int pin, enum digital val)
{
	if (pin < 0 || pin > 10)
		return -1; // indica error

	if (val < 0 || val > 1)
		return -1; // indica error

	if (val)
		// COMPLETAR: poner en rPDATB el bit indicado por pin a 1
		rPDATB |= (0x1 << pin);
	else
		// COMPLETAR: poner en rPDATB el bit indicado por pin a 0
		rPDATB &= ~(0x1 << pin);

	return 0;
}

/* Port G interface implementation */

int portG_conf(int pin, enum port_mode mode)
{
	int pos = pin*2;

	if (pin < 0 || pin > 7)
		return -1; // indica error

	switch (mode) {
		case INPUT:
			// COMPLETAR: poner en rPCONG 00 a partir de la posiciÃ³n pos para
			// configurar como pin de entrada el pin indicado por el parÃ¡metro pin
			rPCONG &= ~(0x3 << pos);
			break;
		case OUTPUT:
			// COMPLETAR: poner en rPCONG 01 a partir de la posiciÃ³n pos para
			// configurar como pin de salida el pin indicado por el parÃ¡metro pin
			rPCONG &= ~(0x3 << pos); //Ponemos los bits a 0
			rPCONG |= (0x1 << pos);  //Y posteriormente a 1 el correspondiente
			break;
		case SIGOUT:
			// COMPLETAR: poner en rPCONG 10 a partir de la posiciÃ³n pos para
			// que salga la seÃ±al interna correspondiente por el pin indicado
			// por el parÃ¡metro pin
			rPCONG &= ~(0x3 << pos);
			rPCONG |= (0x2 << pos);
			break;
		case EINT:
			// COMPLETAR: poner en rPCONG 11 a partir de la posiciÃ³n pos para
			// habilitar la generaciÃ³n de interrupciones externas por el pin
			// indicado por el parÃ¡metro pin
			rPCONG |= (0x3 << pos);
			break;
		default:
			return -1;
	}

	return 0;
}

int portG_eint_trig(int pin, enum trigger trig)
{
	// A COMPLETAR EN LA SEGUNDA PARTE DE LA PRÃ�CTICA
	return 0;
}

int portG_write(int pin, enum digital val)
{
	int pos = pin*2;

	if (pin < 0 || pin > 7)
		return -1; // indica error

	if (val < 0 || val > 1)
		return -1; // indica error

	if ((rPCONG & (0x3 << pos)) != (0x1 << pos))
		return -1; // indica error

	if (val)
		// COMPLETAR: poner en rPDATG el bit indicado por pin a 1
		rPDATG |= (0x1 << pin);
	else
		// COMPLETAR: poner en rPDATG el bit indicado por pin a 0
		rPDATG &= ~(0x1 << pin);

	return 0;
}

int portG_read(int pin, enum digital* val)
{
	int pos = pin*2;

	if (pin < 0 || pin > 7)
		return -1; // indica error

	if (rPCONG & (0x3 << pos))
		return -1; // indica error

	if (rPDATG & (0x1 << pin))
		*val = HIGH;
	else
		*val = LOW;

	return 0;
}

int portG_conf_pup(int pin, enum enable st)
{
	if (pin < 0 || pin > 7)
		return -1; // indica error

	if (st != ENABLE && st != DISABLE)
		return -1; // indica error

	if (st == ENABLE)
		// COMPLETAR: poner el pin de rPUPG indicado por el parametro pin al valor adecuado,
		// para activar la resistencia de pull-up
		rPUPG &= ~(0x1 << pin); //Entrada a 0 si activada
	else
		// COMPLETAR: poner el pin de rPUPG indicado por el parametro pin al valor adecuado,
		// para desactivar la resistencia de pull-up
		rPUPG |= (0x1 << pin); //Entrada a 1 si desconectada

	return 0;
}

