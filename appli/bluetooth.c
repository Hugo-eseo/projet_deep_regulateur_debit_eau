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
#include <string.h>

static bool_e BLUETOOTH_flag = FALSE; //Flag pour indiquer la présence de données

bool_e BLUETOOTH_get_flag(void)
{
	return BLUETOOTH_flag;
}

void BLUETOOTH_set_flag(bool_e flag)
{
	BLUETOOTH_flag = flag;
}

void BLUETOOTH_get_data(void)
{
	#define DEMO_TAB_SIZE	128

	static uint8_t tab[DEMO_TAB_SIZE];
	static uint16_t index = 0;
	uint8_t data = 0;
		if(UART_data_ready(UART1_ID))
		{
			data = UART_getc(UART1_ID);			//lecture du prochain caract�re
			tab[index] = data;						//On m�morise le caract�re dans le tableau
			if(data=='Z')						//Si c'est la fin de la chaine
			{
				uint8_t trame = tab[0]*1 - 48 + tab[1]*10 - 48 + tab[2]*100 - 48;
				BLUETOOTH_flag = TRUE;			//indique que la trame de donnée est prête
				tab[index+1] = 0; 				//fin de chaine, en �crasant le caract�re suivant par un 0
				index = 0;						//Remise � z�ro de l'index
				//DEBIMETRE_set_stop_value(trame);
			}
			else if(index < DEMO_TAB_SIZE - 2)
					{									//Pour tout caract�re diff�rent de \n
						index++;						//on incr�mente l'index (si < TAB_SIZE -2 !)
					}
		}
	}
