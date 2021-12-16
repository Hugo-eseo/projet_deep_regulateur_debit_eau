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

void writeLED(bool_e b)
{
	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, b);
}

bool_e readButton(void)
{
	return !HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN);
}
static volatile bool_e flag_10ms = FALSE;
static volatile uint32_t t = 0;
void process_ms(void)
{
	static uint8_t t10 = 0;
	if(t)
		t--;

	if(t10)
	{
		t10--;
	}
	else
	{
		flag_10ms = TRUE;
		t10=10;
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

	//Initialisation du port de la led PCB rouge (carte Nucleo)
	BSP_GPIO_PinCfg(LED_PCB_RED_GPIO, LED_PCB_RED_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port du bouton bleu (carte Nucleo)
	BSP_GPIO_PinCfg(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port du bouton bleu (carte Nucleo)
	BSP_GPIO_PinCfg(PCB_BUTTON_GPIO, PCB_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	//On ajoute la fonction process_ms à la liste des fonctions appelées automatiquement chaque ms par la routine d'interruption du périphérique SYSTICK
	Systick_add_callback_function(&process_ms);

	// Initialisation de l'écran TFT
	TFT_init();

	// Initialisation de la vanne
	VANNE_init();

	VANNE_open();

	// Initialisation du débimètre
	DEBIMETRE_init();

	//HAL_GPIO_WritePin(LED_PCB_RED_GPIO, LED_PCB_RED_PIN, 1);

	while(1)	//boucle de tâche de fond
	{
		if(!t)
		{
			t = 200;
			HAL_GPIO_TogglePin(LED_GREEN_GPIO, LED_GREEN_PIN);
		}
		if(flag_10ms)
		{
			flag_10ms = FALSE;

			//lire le bouton !
			if(button_press_event()){
				HAL_GPIO_TogglePin(LED_PCB_RED_GPIO, LED_PCB_RED_PIN);
				VANNE_switch_position();
			}
		}
	}
}
