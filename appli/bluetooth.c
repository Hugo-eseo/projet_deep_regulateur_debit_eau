/*
 * bluetooth.c
 *
 *  Created on: 18 janv. 2022
 *      Author: Tristan
 */
#include "bluetooth.h"
#include "stm32f1xx_hal.h"
#include "stm32f1_uart.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"
#include "stm32f1_timer.h"
#include "config.h"
#include <string.h>

#define DEMO_TAB_SIZE	128

static void BLUETOOTH_handler(void);

static const timer_id_e TIMER = TIMER2_ID;

static uint8_t tab[DEMO_TAB_SIZE];
static volatile bool_e BLUETOOTH_flag = FALSE; //Flag pour indiquer la pr?sence de donn?es

static volatile bluetooth_status status = DISCONNECTED;

static void BLUETOOTH_handler(void)
{
	switch(status)
	{
		case DISCONNECTED:
			/*if(tab[0]=='C'){ //On v?rifie si le bluetooth est connect?
				status = CONNECTED; //Si c'est le cas on passe ? l'?tat suivant
			}*/
			break;
		case CONNECTED:
			// Si il s'agit d'une demande d'arr?t d'urgence
			if(tab[0]=='S'){
				BLUETOOTH_flag = TRUE;
			}
			// Sinon c'est le d?but d'une douche qui est demand?e
			else{
				uint8_t trame = tab[0]-48;
				uint8_t i = 1;
				while(tab[i]!=90){
					i++;
				}
				for(uint8_t j=1; j<i; j++){
					trame *= 10;
					trame += tab[j]-48;
				}
				BLUETOOTH_flag = TRUE;							//indique que la trame de donn?e est pr?te
				DEBIMETRE_set_stop_value(trame);
			}
			break;
		default:
			break;
	}
}

bluetooth_status BLUETOOTH_get_status(void){
	return status;
}

void BLUETOOTH_set_status(bluetooth_status new_status){
	status = new_status;
}

bool_e BLUETOOTH_get_flag(void)
{
	return BLUETOOTH_flag;
}

void BLUETOOTH_set_flag(bool_e flag)
{
	BLUETOOTH_flag = flag;
}

void BLUETOOTH_send_data(char c)
{
	if(status == CONNECTED){
		UART_putc(UART2_ID, c);
	}
}

void BLUETOOTH_get_data(void)
{
	static uint16_t index = 0;
	uint8_t data = 0;

	if(UART_data_ready(UART2_ID)){
		data = UART_getc(UART2_ID);								//lecture du prochain caract?re
		tab[index] = data;										//On m?morise le caract?re dans le tableau
		if(data=='Z')										//Si c'est la fin de la chaine
		{
			//tab[index] = 0; 								//fin de chaine
			index = 0;
			BLUETOOTH_handler();
		}
		else if(index < DEMO_TAB_SIZE - 2)
		{												//Pour tout caract?re diff?rent de \n
				index++;									//on incr?mente l'index (si < TAB_SIZE -2 !)
		}
	}
}
