/*
 * vanne.c
 *
 *  Created on: 16 d?c. 2021
 *      Author: hugob
 */

#include "vanne.h"
#include "config.h"
#include "stm32f1_gpio.h"

void VANNE_init(void){
	//Initialisation du port de la vanne
	BSP_GPIO_PinCfg(VANNE_GPIO, VANNE_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
}

void VANNE_open(void){
	HAL_GPIO_WritePin(VANNE_GPIO, VANNE_PIN, 1);
	HAL_GPIO_WritePin(LED_PCB_RED_GPIO, LED_PCB_RED_PIN, 1);
	TFT_set_vanne(TRUE);
}

void VANNE_close(void){
	HAL_GPIO_WritePin(VANNE_GPIO, VANNE_PIN, 0);
	HAL_GPIO_WritePin(LED_PCB_RED_GPIO, LED_PCB_RED_PIN, 0);
	TFT_set_vanne(FALSE);
}

void VANNE_switch_position(void){
	HAL_GPIO_TogglePin(VANNE_GPIO, VANNE_PIN);
	HAL_GPIO_TogglePin(LED_PCB_RED_GPIO, LED_PCB_RED_PIN);
}
