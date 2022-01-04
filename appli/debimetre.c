/*
 * debimetre.c
 *
 *  Created on: 16 déc. 2021
 *      Author: hugob
 */

#include "debimetre.h"
#include "config.h"
#include "stm32f1_extit.h"
#include "stm32f1_timer.h"

static const timer_id_e TIMER = TIMER1_ID;

static void DEBIMETRE_timer_start(void);
static uint16_t DEBIMETRE_timer_stop(void);
static void DEBIMETRE_calculation(uint16_t time);

static uint16_t DEBIMETRE_flow; // En mililitre/mins
static uint32_t DEBIMETRE_consumption; //En mililitre, 4M L max avant overflow

typedef enum
{
	WAIT,
	RISING_EDGE
}state_machine_id;

// Accesseurs
uint16_t DEBIMETRE_get_flow(void){
	return(DEBIMETRE_flow);
}

uint32_t DEBIMETRE_get_consumption(void){
	return(DEBIMETRE_consumption);
}

void DEBIMETRE_init(void){
	// Initialisation de la LED du débimètre
	BSP_GPIO_PinCfg(DEBIMETRE_LED_GPIO, DEBIMETRE_LED_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
	HAL_GPIO_WritePin(DEBIMETRE_LED_GPIO, DEBIMETRE_LED_PIN, 1);

	//Autorise le périphérique GPIO à lever une requête d'interruption
	BSP_GPIO_PinCfg(DEBIMETRE_READER_GPIO, DEBIMETRE_READER_PIN, GPIO_MODE_IT_FALLING, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH);
	//Autorise la demande d'interruption correspondante à interrompre le processeur
	uint8_t pin_number = EXTI_gpiopin_to_pin_number(DEBIMETRE_READER_GPIO);
	EXTIT_enable(pin_number);
	//Definition de la fonction de callback
	EXTIT_set_callback(DEBIMETRE_handler, pin_number, TRUE);
}

void DEBIMETRE_handler(void){
	static state_machine_id state = WAIT;
	static uint16_t time;
	static uint8_t pulse = 0;
	uint16_t period = 0;
	// L'aquitement du flag d'interruption est effectute en amont

	switch(state)
	{
		case WAIT:
			state = RISING_EDGE;
			DEBIMETRE_timer_start();
			break;
		case RISING_EDGE:
			period = DEBIMETRE_timer_stop();
			DEBIMETRE_calculation(period);
			pulse ++;
			state = WAIT;

			if(pulse == 23){
				pulse = 0;
				DEBIMETRE_consumption += 50;
			}
			break;
		default:
			break;
	}
}

static void DEBIMETRE_timer_start(void){
	// Initialisation et lancement du timer à 0us, les IT sont désactivés
	TIMER_run_us(TIMER, 0, FALSE);
}

static uint16_t DEBIMETRE_timer_stop(void){
	TIMER_stop(TIMER);
	return TIMER_read(TIMER);
}

static void DEBIMETRE_calculation(uint16_t time){
	// Calcul du débit et mise à jour du pointeur du main
	DEBIMETRE_flow = uint16_t (time*3.25); // En mililitre/mins
}
