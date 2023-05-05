/*
 * menu_cargaActiva.h
 *
 *  Created on: 5 may. 2023
 *      Author: Luciano Salvatore
 */

#ifndef INC_MENU_CARGAACTIVA_H_
#define INC_MENU_CARGAACTIVA_H_

#include "stm32f3xx_hal.h"

typedef enum{
	MENU_PRINCIPAL,
	MENU_POTENCIA,
	MENU_CORRIENTE,
	MEDICION_POTENCIA,
	MEDICION_CORRIENTE,
	SIZE_PANTALLA_NOMBRE,
} T_PANTALLA_NOMBRE;

typedef struct t_pantalla {
	T_PANTALLA_NOMBRE nombre;
	//struct t_pantalla* menuAnterior;
	void (*inicia_menu) (void);
	void (*accion) (void);
} T_PANTALLA;

//funciones publicas
void start_menu (void);
void check_menu (void);
void check_pulsadores (void);
void check_duracionPulsadores (void);

#endif /* INC_MENU_CARGAACTIVA_H_ */
