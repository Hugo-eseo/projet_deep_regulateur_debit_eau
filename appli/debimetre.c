/*
 * debimetre.c
 *
 *  Created on: 16 déc. 2021
 *      Author: hugob
 */

#include "debimetre.h"
#include "config.h"

void DEBIMETRE_init(void){
	// Initialisation de la LED du débimètre
	BSP_GPIO_PinCfg(DEBIMETRE_LED_GPIO, DEBIMETRE_LED_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
	HAL_GPIO_WritePin(DEBIMETRE_LED_GPIO, DEBIMETRE_LED_PIN, 1);
}
