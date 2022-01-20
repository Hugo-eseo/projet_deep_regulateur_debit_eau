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

#include "tft.h"
#include "vanne.h"
#include "debimetre.h"

void writeLED(bool_e b)
{
	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, b);
}

bool_e readButton(void)
{
	return !HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN);
}

#define TIMER_AMOUT 2
// {BUTTON_READING, TFT_UPDATE}
static volatile bool_e flags[TIMER_AMOUT] = {FALSE, FALSE};
static volatile uint16_t timer[TIMER_AMOUT] = {0, 0};
static const uint16_t TIMER_VALUE[TIMER_AMOUT] = {10, 500};

typedef enum
{
	INIT,
	WAIT,
	BUTTON_READING,
	TFT_UPDATE,
	SHOWER_START,
	SHOWER_STOP
}state_machine_id;

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


	//Initialisation de l'UART2 à la vitesse de 115200 bauds/secondes (92kbits/s) PA2 : Tx  | PA3 : Rx.
		//Attention, les pins PA2 et PA3 ne sont pas reliées jusqu'au connecteur de la Nucleo.
		//Ces broches sont redirigées vers la sonde de débogage, la liaison UART étant ensuite encapsulée sur l'USB vers le PC de développement.
	UART_init(UART2_ID,115200);

	//"Indique que les printf sortent vers le périphérique UART2."
	SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

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

	state_machine_id state = INIT;
	//static uint16_t previous_flow = 0;

	static bool_e shower_on_duty = FALSE;
	static bool_e manual_stop = FALSE;

	while(1)	//boucle de tâche de fond
	{
		BLUETOOTH_get_data();
		switch(state){
			case INIT:
				if(TRUE){
					TFT_home_screen();
					state = WAIT;
				}
				break;
			case WAIT:
				if(BLUETOOTH_get_flag()){
					BLUETOOTH_set_flag(FALSE);
					if(!shower_on_duty){
						DEBIMETRE_set_flag(FALSE, 1);
						DEBIMETRE_set_consumption(0);
						state = SHOWER_START;
					}
				}
				else if(DEBIMETRE_get_flag(1)){
					state = SHOWER_STOP;
				}
				// La lecture du bouton est prioritaire sur la mise à jour de l'écran
				else if(flags[0]){
					state = BUTTON_READING;
				}
				else if(flags[1]){
					state = TFT_UPDATE;
				}
				break;
			// Lecture du boutton toutes les 10 ms
			case BUTTON_READING:
				// Acquittement du flag
				flags[0] = FALSE;
				// Lecture du bouton
				if(button_press_event()){
					HAL_GPIO_TogglePin(LED_PCB_RED_GPIO, LED_PCB_RED_PIN);
					VANNE_switch_position();
					TFT_set_vanne(HAL_GPIO_ReadPin(LED_PCB_RED_GPIO, LED_PCB_RED_PIN));
					// Si une douche est en cours, arrêt d'urgence de l'utilisateur
					if(shower_on_duty){
						state = SHOWER_STOP;
						manual_stop = TRUE;
					}
				}
				state = WAIT;
				break;
			// Mise à jour de l'écran toutes les 500 ms
			case TFT_UPDATE:
				// Acquittement du flag
				flags[1] = FALSE;
				TFT_update_info();

				// Si le débitmètre est inactif, on set la valeur du débit courant à 0
				if(!DEBIMETRE_get_flag(0)){
					DEBIMETRE_set_flow(0);
				}
				DEBIMETRE_set_flag(FALSE, 0);
				state = WAIT;
				break;
			// Lancement d'une douche depuis l'application
			case SHOWER_START:
				shower_on_duty = TRUE;
				DEBIMETRE_set_stop_value(0);
				TFT_add_console("Lancement de la douche");
				break;
			// Stop de la douche automatiquement par le régulateur ou manuellement par l'utilisateur
			case SHOWER_STOP:
				shower_on_duty = FALSE;
				DEBIMETRE_set_stop_value(-1);
				if(manual_stop){
					TFT_add_console("Arret manuel d'urgence de la douche");
				}
				else{
					TFT_add_console("Fin automatique de la douche");
					TFT_add_console("Vous avez consomme toute l'eau a votre disposition !");
				}
				TFT_set_shower(0);
				break;
			default:
				break;
		}
	}
}
