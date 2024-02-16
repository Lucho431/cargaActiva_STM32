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
extern uint8_t flag_enableDAC;
extern int32_t medida_potencia;
extern int32_t medida_corriente;
extern DAC_HandleTypeDef hdac1;

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
uint16_t valorDAC = 2047;


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
	sprintf(texto, "C:  %ld mA", setPoint_corriente );
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
	sprintf(texto, "P:  %ld mW", setPoint_potencia);
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
	sprintf(texto, "set: %ld mA", setPoint_corriente);
	lcd_send_string(texto);
	lcd_put_cur(0, 1);
	sprintf(texto, "med: %ld.%ldmA", medida_corriente / 10, medida_corriente % 10 );
	lcd_send_string(texto);
	periodo_impMed = 0;
	valorDAC = 2047;
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, valorDAC); //la mitad
//	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
} //fin init_medicionCorriente()


void init_medicionPotencia (void){
	lcd_clear();
	lcd_put_cur(0, 0);
	sprintf(texto, "set: %ld.%ldmW", setPoint_potencia / 10, setPoint_potencia % 10 );
	lcd_send_string(texto);
	lcd_put_cur(0, 1);
	sprintf(texto, "med: %ld.%ldmW", medida_potencia / 10, medida_potencia % 10 );
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
				flag_enableDAC = 1;
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
				flag_enableDAC = 1;
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
					if (setPoint_corriente < 0) setPoint_corriente = 0;
					lcd_put_cur(5, 0);
					sprintf(texto, "%ld mA   ", setPoint_corriente);
					lcd_send_string(texto);
					lcd_put_cur(14, 0);
					lcd_send_data(0x7E); // ->
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
					sprintf(texto, "%ld mW", setPoint_potencia);
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
		valorDAC = 0;
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, valorDAC);
//		HAL_DAC_Stop(&hdac2, DAC_CHANNEL_1);
		flag_enableDAC = 0;
		return;
	} //end if PULSO

	if (medida_corriente < setPoint_corriente){
		if (valorDAC < 4095){
			valorDAC++;
			HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, valorDAC);
		}
	}else if (medida_corriente > setPoint_corriente){
		if (valorDAC != 0){
			valorDAC--;
			HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, valorDAC);
		}
	} //end if medida_corriente...

	if (periodo_impMed > 29){ //actualiza cada 300 ms
		lcd_put_cur(5, 1);
		sprintf(texto, "%ldmA    ", medida_corriente);
		lcd_send_string(texto);
		periodo_impMed = 0;
	}
} //fin acc_medicionCorriente()


void acc_medicionPotencia (void){
	if (PULSO != 0){
		cursor = 0;
		pantallaActual = &pantalla [MENU_POTENCIA];
		pantallaActual->inicia_menu();
		valorDAC = 0;
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, valorDAC);
		flag_enableDAC = 0;
		return;
	}

	if (periodo_impMed > 29){ //actualiza cada 300 ms
		lcd_put_cur(5, 1);
		sprintf(texto, "%ld.%ldmA ", medida_potencia / 10, medida_potencia % 10 );
		lcd_send_string(texto);
	}
} //fin acc_medicionPotencia()
