/*
 * menu_cargaActiva.c
 *
 *  Created on: 5 may. 2023
 *      Author: Luciano Salvatore
 */

#include "menu_cargaActiva.h"
#include "lcd_i2c_lfs.h"
#include "stdio.h"
#include "stdlib.h"


uint8_t arriba[8] = {
		0b00000,
		0b00100,
		0b01110,
		0b10101,
		0b00100,
		0b00100,
		0b00000,
		0b00000,
};

uint8_t abajo [8] = {
		0b00000,
		0b00000,
		0b00100,
		0b00100,
		0b10101,
		0b01110,
		0b00100,
		0b00000,
};


//variables externas
extern int32_t lecturaEnc;
extern uint8_t lectura_boton;
extern uint8_t last_boton;
extern uint8_t periodo_impMed;

#define PULSO (last_boton && !lectura_boton)

//variables pantalla
T_PANTALLA* pantallaActual;
T_PANTALLA* pantallaAux;
static char texto[50];
uint8_t cursor = 0;
uint8_t flag_setValor = 0;
//variables de mediciones
int32_t setPoint_potencia;
int32_t setPoint_corriente;
int32_t medida_potencia;
int32_t medida_corriente;


void acc_menuPrincipal (void);
void acc_menuCorriente (void);
void acc_menuPotencia (void);
void acc_medicionCorriente (void);
void acc_medicionPotencia (void);

void init_menuPrincipal (void);
void init_menuCorriente (void);
void init_menuPotencia (void);
void init_medicionCorriente (void);
void init_medicionPotencia (void);

T_PANTALLA pantalla[SIZE_PANTALLA_NOMBRE] = {
		{MENU_PRINCIPAL, init_menuPrincipal, acc_menuPrincipal}, //MENU_PRINCIPAL
		{MENU_POTENCIA, init_menuPotencia, acc_menuPotencia}, //MENU_POTENCIA
		{MENU_CORRIENTE, init_menuCorriente, acc_menuCorriente}, //MENU_CORRIENTE
		{MEDICION_POTENCIA, init_medicionPotencia, acc_medicionPotencia}, //MEDICION_POTENCIA
		{MEDICION_CORRIENTE, init_medicionCorriente, acc_medicionCorriente}, //MEDICION_CORRIENTE
};

/////////////////////////////////////////
//         FUNCIONES PUBLICAS          //
/////////////////////////////////////////

void start_menu (void){
	//init_pantalla();
	lcd_CustomChar_create(0, arriba);
	lcd_CustomChar_create(1, abajo);
	init_menuPrincipal();
	pantallaActual = &pantalla[MENU_PRINCIPAL];
} //fin start_menu()


void check_menu (void){
	pantallaActual->accion();
} //fin check_menu()

/////////////////////////////////////////
//          INICIALIZADORES            //
/////////////////////////////////////////

void init_menuPrincipal (void){
	lcd_clear();
	lcd_put_cur(1, 0);
	lcd_send_string("Modo Potencia");
	lcd_put_cur(1, 1);
	lcd_send_string("Modo Corriente");
	cursor = 0;
	lcd_put_cur(0, 0);
	lcd_send_data(0x7E); // ->
} //fin init_menuPrincipal()


void init_menuCorriente (void){
	lcd_clear();
	lcd_put_cur(1, 0);
	sprintf(texto, "C:  %d mA", setPoint_corriente );
	lcd_send_string(texto);
	lcd_put_cur(1, 1);
	lcd_send_string("INICIAR");
	lcd_put_cur(11, 1);
	lcd_send_string("ATRAS");
	lcd_put_cur(0, 0);
	lcd_send_data(0x7E); // ->
} //fin init_menuCorriente()


void init_menuPotencia (void){
	lcd_clear();
	lcd_put_cur(1, 0);
	sprintf(texto, "P:  %d mW", setPoint_potencia);
	lcd_send_string(texto);
	lcd_put_cur(1, 1);
	lcd_send_string("INICIAR");
	lcd_put_cur(11, 1);
	lcd_send_string("ATRAS");
	lcd_put_cur(0, 0);
	lcd_send_data(0x7E); // ->
} //fin init_menuPotencia()


void init_medicionCorriente (void){
	lcd_clear();
	lcd_put_cur(0, 0);
	sprintf(texto, "set: %d mA", setPoint_corriente);
	lcd_send_string(texto);
	lcd_put_cur(0, 1);
	sprintf(texto, "med: %d.%dmA", medida_corriente / 10, medida_corriente % 10 );
	lcd_send_string(texto);
	periodo_impMed = 0;
} //fin init_medicionCorriente()


void init_medicionPotencia (void){
	lcd_clear();
	lcd_put_cur(0, 0);
	sprintf(texto, "set: %d.%dmW", setPoint_potencia / 10, setPoint_potencia % 10 );
	lcd_send_string(texto);
	lcd_put_cur(0, 1);
	sprintf(texto, "med: %d.%dmW", medida_potencia / 10, medida_potencia % 10 );
	lcd_send_string(texto);

} //fin init_medicionPotencia()

/////////////////////////////////////////
//            ACCIONES                 //
/////////////////////////////////////////

void acc_menuPrincipal (void){
	switch (cursor){
		case 0:

			if (lecturaEnc > 0){
				cursor = 1;
				lcd_put_cur(0, 1);
				lcd_send_data(0x7E); // ->
				lcd_put_cur(0, 0);
				lcd_send_string(" ");
			}

			if (PULSO != 0){
				pantallaActual = &pantalla [MENU_POTENCIA];
				pantallaActual->inicia_menu();
			}
		break;
		case 1:

			if (lecturaEnc < 0){
				cursor = 0;
				lcd_put_cur(0, 0);
				lcd_send_data(0x7E); // ->
				lcd_put_cur(0, 1);
				lcd_send_string(" ");
			}

			if (PULSO != 0){
				cursor = 0;
				pantallaActual = &pantalla [MENU_CORRIENTE];
				pantallaActual->inicia_menu();
			}
		break;
		default:
		break;
	} //end switch cursor

} //fin acc_menuPrincipal()


void acc_menuCorriente (void){
	switch (cursor){
		case 0:

			if (flag_setValor != 0){

				if (PULSO != 0){
					lcd_put_cur(0, 0);
					lcd_send_data(0x7E); // ->
					lcd_put_cur(4, 0);
					lcd_send_string(" ");
					lcd_put_cur(14, 0);
					lcd_send_string(" ");
					flag_setValor = 0;
				} //fin if (PULSO != 0)

				if (lecturaEnc != 0){
					if (abs(lecturaEnc) > 10) lecturaEnc *= 10;
					setPoint_corriente += (lecturaEnc >> 1);
					lcd_put_cur(5, 0);
					sprintf(texto, "%d mA", setPoint_corriente);
					lcd_send_string(texto);
					lecturaEnc = 0;
				} //fin If (lecturaEnc != 0)

				break;
			} //fin if (flag_setValor != 0)

			if (lecturaEnc > 0){
				cursor = 1;
				lcd_put_cur(0, 1);
				lcd_send_data(0x7E); // ->
				lcd_put_cur(0, 0);
				lcd_send_string(" ");
				lcd_put_cur(10, 1);
				lcd_send_string(" ");
				lecturaEnc = 0;
			}

			if (PULSO != 0){
				lcd_put_cur(0, 0);
				lcd_send_string(" ");
				lcd_put_cur(4, 0);
				lcd_send_data(0x7F); // <-
				lcd_put_cur(14, 0);
				lcd_send_data(0x7E); // ->
				flag_setValor = 1;
			}
		break;
		case 1:

			if (lecturaEnc > 0){
				cursor = 2;
				lcd_put_cur(10, 1);
				lcd_send_data(0x7E); // ->
				lcd_put_cur(0, 0);
				lcd_send_string(" ");
				lcd_put_cur(0, 1);
				lcd_send_string(" ");
				lecturaEnc = 0;
			}

			if (lecturaEnc < 0){
				cursor = 0;
				lcd_put_cur(0, 0);
				lcd_send_data(0x7E); // ->
				lcd_put_cur(0, 1);
				lcd_send_string(" ");
				lcd_put_cur(10, 1);
				lcd_send_string(" ");
				lecturaEnc = 0;
			}

			if (PULSO){
				//empieza a medir potencia
				pantallaActual = &pantalla[MEDICION_CORRIENTE];
				pantallaActual->inicia_menu();
			}
		break;
		case 2:

			if (lecturaEnc < 0){
				cursor = 1;
				lcd_put_cur(0, 1);
				lcd_send_data(0x7E); // ->
				lcd_put_cur(0, 0);
				lcd_send_string(" ");
				lcd_put_cur(10, 1);
				lcd_send_string(" ");
				lecturaEnc = 0;
			}

			if (PULSO != 0){
				cursor = 0;
				pantallaActual = &pantalla [MENU_PRINCIPAL];
				pantallaActual->inicia_menu();
			}
		break;
		default:
		break;
	} //end switch cursor
} //fin acc_menuCorriente()


void acc_menuPotencia (void){
	switch (cursor){
		case 0:

			if (flag_setValor != 0){

				if (PULSO != 0){
					lcd_put_cur(0, 0);
					lcd_send_data(0x7E); // ->
					lcd_put_cur(4, 0);
					lcd_send_string(" ");
					lcd_put_cur(14, 0);
					lcd_send_string(" ");
					flag_setValor = 0;
				} //fin if (PULSO != 0)

				if (lecturaEnc != 0){
					if (abs(lecturaEnc) > 10) lecturaEnc *= 10;
					setPoint_potencia += (lecturaEnc >> 1);
					lcd_put_cur(5, 0);
					sprintf(texto, "%d mW", setPoint_potencia);
					lcd_send_string(texto);
					lecturaEnc = 0;
				} //fin If (lecturaEnc != 0)

				break;
			} //fin if (flag_setValor != 0)

			if (lecturaEnc > 0){
				cursor = 1;
				lcd_put_cur(0, 1);
				lcd_send_data(0x7E); // ->
				lcd_put_cur(0, 0);
				lcd_send_string(" ");
				lcd_put_cur(10, 1);
				lcd_send_string(" ");
				lecturaEnc = 0;
			}

			if (PULSO != 0){
				lcd_put_cur(0, 0);
				lcd_send_string(" ");
				lcd_put_cur(4, 0);
				lcd_send_data(0x7F); // <-
				lcd_put_cur(14, 0);
				lcd_send_data(0x7E); // ->
				flag_setValor = 1;
			}
		break;
		case 1:

			if (lecturaEnc > 0){
				cursor = 2;
				lcd_put_cur(10, 1);
				lcd_send_data(0x7E); // ->
				lcd_put_cur(0, 0);
				lcd_send_string(" ");
				lcd_put_cur(0, 1);
				lcd_send_string(" ");
				lecturaEnc = 0;
			}

			if (lecturaEnc < 0){
				cursor = 0;
				lcd_put_cur(0, 0);
				lcd_send_data(0x7E); // ->
				lcd_put_cur(0, 1);
				lcd_send_string(" ");
				lcd_put_cur(10, 1);
				lcd_send_string(" ");
				lecturaEnc = 0;
			}

			if (PULSO){
				//empieza a medir potencia
				pantallaActual = &pantalla[MEDICION_POTENCIA];
				pantallaActual->inicia_menu();
			}
		break;
		case 2:

			if (lecturaEnc < 0){
				cursor = 1;
				lcd_put_cur(0, 1);
				lcd_send_data(0x7E); // ->
				lcd_put_cur(0, 0);
				lcd_send_string(" ");
				lcd_put_cur(10, 1);
				lcd_send_string(" ");
				lecturaEnc = 0;
			}

			if (PULSO != 0){
				cursor = 0;
				pantallaActual = &pantalla [MENU_PRINCIPAL];
				pantallaActual->inicia_menu();
			}
		break;
		default:
		break;
	} //end switch cursor
} //fin acc_menuPotencia()


void acc_medicionCorriente (void){
	if (PULSO != 0){
		cursor = 0;
		pantallaActual = &pantalla [MENU_CORRIENTE];
		pantallaActual->inicia_menu();
		return;
	}

	if (periodo_impMed > 29){ //actualiza cada 300 ms
		lcd_put_cur(5, 1);
		sprintf(texto, "%d.%dmA ", medida_corriente / 10, medida_corriente % 10 );
		lcd_send_string(texto);
	}
} //fin acc_medicionCorriente()


void acc_medicionPotencia (void){
	if (PULSO != 0){
		cursor = 0;
		pantallaActual = &pantalla [MENU_POTENCIA];
		pantallaActual->inicia_menu();
		return;
	}

	if (periodo_impMed > 29){ //actualiza cada 300 ms
		lcd_put_cur(5, 1);
		sprintf(texto, "%d.%dmA ", medida_potencia / 10, medida_potencia % 10 );
		lcd_send_string(texto);
	}
} //fin acc_medicionPotencia()
