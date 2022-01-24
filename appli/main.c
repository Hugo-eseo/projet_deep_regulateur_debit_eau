/**
  ******************************************************************************
  * @file    main.c
  * @author  Nirgal
  * @date    03-July-2019
  * @brief   Default main function.
  ******************************************************************************
*/
#include "stm32f1xx_hal.h"
#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"
#include "systick.h"

// Fichiers du projet
#include "tft.h"
#include "vanne.h"
#include "debimetre.h"
#include "bluetooth.h"


/*
bool_e readButton(void)
{
	return !HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN);
}
*/

// Timers pour la lecture du bonton et la mise à jour de l'écran
#define TIMER_AMOUT 2
// {BUTTON_READING, TFT_UPDATE}
static volatile bool_e flags[TIMER_AMOUT] = {FALSE, FALSE};
static volatile uint16_t timer[TIMER_AMOUT] = {0, 0};
static const uint16_t TIMER_VALUE[TIMER_AMOUT] = {10, 500};

static bool_e button_pressed = FALSE;

// Machine à état principale
typedef enum
{
	INIT,
	WAITING_CONNEXION,
	WAITING_INSTRUCTIONS,
	DELIVERY,
	MANUAL_DELIVERY,
	EMERGENCY_STOP,
	STOP_DELIVERY
}state_machine_id;

void state_machine(void){
	static state_machine_id state = INIT;
	static state_machine_id previous_state;

	switch(state){
		// Première connexion au Reguloco, on guide l'utilisateur
		case INIT:
			// Dès qu'il se connecte
			if(BLUETOOTH_get_status()==CONNECTED){
				// On change d'écran et on attend les instructions
				TFT_home_screen();
				state = WAITING_INSTRUCTIONS;
			}
			previous_state = INIT;
			break;
		// Si la première initialisation a eu lieu mais que l'utilisateur s'est deconnecté depuis
		case WAITING_CONNEXION:
			// Si il se connecte
			if(BLUETOOTH_get_status()==CONNECTED){
				state = WAITING_INSTRUCTIONS;
			}
			// Si il appuie sur le boutton (déclanchement manuel)
			else if(button_pressed){
				state = MANUAL_DELIVERY;
			}
			previous_state = WAITING_CONNEXION;
			break;
		// Si l'utilisateur est connecté
		case WAITING_INSTRUCTIONS:
			// Première entrée
			if(state != previous_state){
				TFT_set_connexion(TRUE);
			}
			// Si il demande un 'delivery' (activation de la vanne)
			else if(BLUETOOTH_get_flag()){
				state = DELIVERY;
			}
			// Si le bluetooth est déconnecté
			else if(BLUETOOTH_get_status()==DISCONNECTED){
				state = WAITING_CONNEXION;
			}
			previous_state = WAITING_INSTRUCTIONS;
			break;
		// Si l'utilisateur lance un 'delivery' depuis l'application
		case DELIVERY:
			// Première entrée
			if(state != previous_state){
				BLUETOOTH_set_flag(FALSE);
				DEBIMETRE_set_consumption(0);
				TFT_set_shower(DEBIMETRE_get_stop_value());
				VANNE_open();
			}
			// Si un arrêt d'urgence est demandé
			if(button_pressed || BLUETOOTH_get_flag()){
				state = EMERGENCY_STOP;
			}
			// Si l'on atteint la valeur d'arrêt
			else if(DEBIMETRE_get_consumption()>=DEBIMETRE_get_stop_value()){
				state = STOP_DELIVERY;
			}
			previous_state = DELIVERY;
			break;
		// Si il ouvre la vanne manuellement avec le bouton (uniquement si il n'est pas connecté en bluetooth)
		case MANUAL_DELIVERY:
			// Première entrée
			if(state != previous_state){
				DEBIMETRE_set_stop_value(-1);
				TFT_set_shower(0);
				VANNE_open();
			}
			// Si arrêt par l'utilisateur
			if(button_pressed){
				state = EMERGENCY_STOP;
			}
			previous_state = MANUAL_DELIVERY;
			break;
		// En cas d'arrêt manuel soit depuis le bouton soit depuis l'application
		case EMERGENCY_STOP:
			// On ferme la vanne
			VANNE_close();
			BLUETOOTH_set_flag(FALSE);
			// On l'indique dans la console
			TFT_add_console("Arrêt d'urgence par l'utilisateur");
			// On revient à l'état de départ
			state = (previous_state == MANUAL_DELIVERY?WAITING_CONNEXION:WAITING_INSTRUCTIONS);
			previous_state = EMERGENCY_STOP;
			break;
		// Si l'arrêt est automatique par l'application
		case STOP_DELIVERY:
			// On ferme la vanne
			VANNE_close();
			// On l'indique dans la console
			TFT_add_console("Consigne atteinte");
			state = WAITING_INSTRUCTIONS;
			previous_state = STOP_DELIVERY;
			break;
		default:
			break;
	}
}

void process_ms(void)
{
	for(uint i=0; i<TIMER_AMOUT; i++){
		if(timer[i]){
			timer[i]--;
		}
		else{
			flags[i] = TRUE;
			timer[i] = TIMER_VALUE[i];
		}
	}
}

void writeLED(bool_e b)
{
	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, b);
}

//Détecteur d'appui sur un bouton
bool_e button_press_event(void)
{
	static bool_e previous_state = FALSE;
	bool_e current_state;
	bool_e ret;
	current_state = !HAL_GPIO_ReadPin(PCB_BUTTON_GPIO,PCB_BUTTON_PIN);
	ret = current_state && !previous_state;
	previous_state = current_state;
	return ret;
}


int main(void)
{
	//Initialisation de la couche logicielle HAL (Hardware Abstraction Layer)
	//Cette ligne doit rester la première étape de la fonction main().
	HAL_Init();

	//Initialisation de l'UART2 à la vitesse de 9600 bauds/secondes (92kbits/s) PA2 : Tx  | PA3 : Rx.
	//Attention, les pins PA2 et PA3 ne sont pas reliées jusqu'au connecteur de la Nucleo.
	//Ces broches sont redirigées vers la sonde de débogage, la liaison UART étant ensuite encapsulée sur l'USB vers le PC de développement.
	UART_init(UART2_ID,9600);

	//"Indique que les printf sortent vers le périphérique UART2."
	// SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	//Initialisation du port de la led Verte (carte Nucleo)
	BSP_GPIO_PinCfg(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port de la led PCB rouge
	BSP_GPIO_PinCfg(LED_PCB_RED_GPIO, LED_PCB_RED_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port du bouton bleu (carte Nucleo)
	BSP_GPIO_PinCfg(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port du bouton du PCB
	BSP_GPIO_PinCfg(PCB_BUTTON_GPIO, PCB_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	//On ajoute la fonction process_ms à la liste des fonctions appelées automatiquement chaque ms par la routine d'interruption du périphérique SYSTICK
	Systick_add_callback_function(&process_ms);

	// Initialisation de l'écran TFT
	TFT_init();

	// Initialisation de la vanne
	VANNE_init();

	// Initialisation du débimètre
	DEBIMETRE_init();

	while(1)	//boucle de tâche de fond
	{
		// Lecture de l'uart
		BLUETOOTH_get_data();

		// Lecture du boutton toutes les 10ms
		if(flags[0]){
			// Acquittement du flag
			flags[0] = FALSE;
			button_pressed = button_press_event();
		}
		// Maj de l'écran toutes les 500 ms
		else if(flags[1]){
			// Acquittement du flag
			flags[1] = FALSE;

			// Si le débitmètre est inactif, on set la valeur du débit courant à 0
			if(!DEBIMETRE_get_flag()){
				DEBIMETRE_set_flow(0);
			}
			TFT_update_info(); // Maj
			DEBIMETRE_set_flag(FALSE);
		}
	}
}
