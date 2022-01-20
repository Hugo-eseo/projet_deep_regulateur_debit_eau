/*
 * bluetooth.c
 *
 *  Created on: 18 janv. 2022
 *      Author: Tristan
 */
#include "stm32f1xx_hal.h"
#include "stm32f1_uart.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"
#include <string.h>

void get_data(void)
{
	#define DEMO_TAB_SIZE 128

	static uint8_t tab[DEMO_TAB_SIZE];
	static uint16_t index = 0;
	uint8_t c;
		if(UART_data_ready(UART1_ID))
		{
			c = UART_getc(UART1_ID);			//lecture du prochain caract�re
			tab[index] = c;						//On m�morise le caract�re dans le tableau
			if(c=='Z')							//Si c'est la fin de la chaine
			{
				tab[index+1] = 0; 				//fin de chaine, en �crasant le caract�re suivant par un 0
				UART_puts(UART1_ID, c, 0);	//on renvoie la chaine re�ue.
				index = 0;						//Remise � z�ro de l'index
			}
			else if(index < DEMO_TAB_SIZE - 2)
			{									//Pour tout caract�re diff�rent de \n
				index++;						//on incr�mente l'index (si < TAB_SIZE -2 !)
			}
		}
	}
