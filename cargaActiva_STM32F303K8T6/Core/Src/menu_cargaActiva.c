/*
 * menu_cargaActiva.c
 *
 *  Created on: 5 may. 2023
 *      Author: Luciano Salvatore
 */

#include "menu_cargaActiva.h"


//variables pantalla
T_PANTALLA* pantallaActual;
T_PANTALLA* pantallaAux;
//variables de mediciones
uint16_t setPoint_potencia;
uint16_t setPoint_corriente;
uint16_t medida_potencia;
uint16_t medida_corriente;


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

} //fin init_menuPrincipal()


void init_menuCorriente (void){

} //fin init_menuCorriente()


void init_menuPotencia (void){

} //fin init_menuPotencia()


void init_medicionCorriente (void){

} //fin init_medicionCorriente()


void init_medicionPotencia (void){

} //fin init_medicionPotencia()

/////////////////////////////////////////
//            ACCIONES                 //
/////////////////////////////////////////

void acc_menuPrincipal (void){

} //fin acc_menuPrincipal()


void acc_menuCorriente (void){

} //fin acc_menuCorriente()


void acc_menuPotencia (void){

} //fin acc_menuPotencia()


void acc_medicionCorriente (void){

} //fin acc_medicionCorriente()


void acc_medicionPotencia (void){

} //fin acc_medicionPotencia()
