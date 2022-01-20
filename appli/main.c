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
#include "bluetooth.h"

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
	WAIT,
	BUTTON_READING,
	TFT_UPDATE
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

//D�tecteur d'appui sur un bouton
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
	//Cette ligne doit rester la premi�re �tape de la fonction main().
	HAL_Init();


	//Initialisation de l'UART2 � la vitesse de 115200 bauds/secondes (92kbits/s) PA2 : Tx  | PA3 : Rx.
		//Attention, les pins PA2 et PA3 ne sont pas reli�es jusqu'au connecteur de la Nucleo.
		//Ces broches sont redirig�es vers la sonde de d�bogage, la liaison UART �tant ensuite encapsul�e sur l'USB vers le PC de d�veloppement.
	UART_init(UART1_ID,9600);

	//"Indique que les printf sortent vers le p�riph�rique UART2."
	//SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	//Initialisation du port de la led Verte (carte Nucleo)
	BSP_GPIO_PinCfg(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port de la led PCB rouge
	BSP_GPIO_PinCfg(LED_PCB_RED_GPIO, LED_PCB_RED_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port du bouton bleu (carte Nucleo)
	BSP_GPIO_PinCfg(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port du bouton du PCB
	BSP_GPIO_PinCfg(PCB_BUTTON_GPIO, PCB_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	//On ajoute la fonction process_ms � la liste des fonctions appel�es automatiquement chaque ms par la routine d'interruption du p�riph�rique SYSTICK
	Systick_add_callback_function(&process_ms);

	// Initialisation de l'�cran TFT
	TFT_init();

	// Initialisation de la vanne
	VANNE_init();

	// Initialisation du d�bim�tre
	DEBIMETRE_init();

	state_machine_id state = WAIT;
	//static uint16_t previous_flow = 0;

	while(1)	//boucle de t�che de fond
	{
		BLUETOOTH_get_data();
		switch(state){
			case WAIT:
				// La lecture du bouton est prioritaire sur la mise � jour de l'�cran
				if(flags[0]){
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
				}
				state = WAIT;
				break;
			// Mise � jour de l'�cran toutes les 500 ms
			case TFT_UPDATE:
				// Acquittement du flag
				flags[1] = FALSE;
				TFT_update_info();

				// Si le d�bitm�tre est inactif, on set la valeur du d�bit courant � 0
				if(!DEBIMETRE_get_flag()){
					DEBIMETRE_set_flow(0);
				}
				DEBIMETRE_set_flag(FALSE);

				state = WAIT;
				break;
			default:
				break;
		}
	}
}
