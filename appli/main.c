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

void writeLED(bool_e b)
{
	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, b);
}

bool_e readButton(void)
{
	return !HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN);
}

static volatile uint32_t t = 0;
void process_ms(void)
{
	if(t)
		t--;
}

void testB(void)
{
	#define DEMO_TAB_SIZE 128

	static uint8_t tab[DEMO_TAB_SIZE];
	static uint16_t index = 0;
	uint8_t c;
		if(UART_data_ready(UART1_ID))
		{
			c = UART_getc(UART1_ID);			//lecture du prochain caract�re
			tab[index] = c;						//On m�morise le caract�re dans le tableau
			if(c=='\n')							//Si c'est la fin de la chaine
			{
				tab[index+1] = 0; 				//fin de chaine, en �crasant le caract�re suivant par un 0
				UART_puts(UART1_ID, "test", 0);	//on renvoie la chaine re�ue.
				index = 0;						//Remise � z�ro de l'index
			}
			else if(index < DEMO_TAB_SIZE - 2)
			{									//Pour tout caract�re diff�rent de \n
				index++;						//on incr�mente l'index (si < TAB_SIZE -2 !)
			}
		}
	}



int main(void)
{
	//Initialisation de la couche logicielle HAL (Hardware Abstraction Layer)
	//Cette ligne doit rester la premi�re �tape de la fonction main().
	HAL_Init();

	//Initialisation de l'UART2 � la vitesse de 115200 bauds/secondes (92kbits/s) PA2 : Tx  | PA3 : Rx.
		//Attention, les pins PA2 et PA3 ne sont pas reli�es jusqu'au connecteur de la Nucleo.
		//Ces broches sont redirig�es vers la sonde de d�bogage, la liaison UART �tant ensuite encapsul�e sur l'USB vers le PC de d�veloppement.
	//UART_init(UART2_ID,115200);

	UART_init(UART1_ID,9600);

	//"Indique que les printf sortent vers le p�riph�rique UART2."
	//SYS_set_std_usart(UART1_ID, UART1_ID, UART1_ID);

	//Initialisation du port de la led Verte (carte Nucleo)
	BSP_GPIO_PinCfg(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port du bouton bleu (carte Nucleo)
	BSP_GPIO_PinCfg(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	//On ajoute la fonction process_ms � la liste des fonctions appel�es automatiquement chaque ms par la routine d'interruption du p�riph�rique SYSTICK
	Systick_add_callback_function(&process_ms);

	//MAIN A

	while(1)	//boucle de t�che de fond
	{
		testB();
		if(!t)
		{
			//UART_puts(UART1_ID,"t",1);
			t = 200;
			HAL_GPIO_TogglePin(LED_GREEN_GPIO, LED_GREEN_PIN);
		}

	}
}
