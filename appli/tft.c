/*
 * tft.c
 *
 *  Created on: 16 d�c. 2021
 *      Author: hugob
 */

#include "tft.h"
#include "config.h"

// Biblioth�que de l'�cran TFT
#include "tft_ili9341/stm32f1_ili9341.h"

static uint8_t TFT_add_line(FontDef_t *font, uint16_t spacing);
static uint8_t TFT_add_sentence(uint8_t line_number, uint16_t color, uint16_t spacing, char * string);
static void TFT_edit_sentence(uint8_t line_number, uint8_t sentence_order, uint16_t color, char * string);

static uint16_t TFT_get_sentence_size(uint8_t line_number, uint8_t sentence_order);
static void TFT_update_screen(void);
static void TFT_create_rectangle_line(uint8_t line_number, uint8_t line_amount, uint16_t spacing, uint16_t color, uint8_t width, uint16_t padding_left, uint16_t padding_right);

/* Principe de fonctionnement de l'affichage :
*		- L'�cran est divis� en plusieurs 'lignes'. Chaque ligne cr��e l'une � la suite de l'autre poss�de ses propores caract�risiques
*		- Chaque ligne est ensuite subdivis�e en plusieurs 'phrases' ayant �galement ses propres caract�ristiques.
*		Cela permet d'afficher et d'effacer facilement des informations sur l'ecran.
*		- Enfin, les 'rectangles d�coratifs' sont ind�pendant du syst�me pr�cedent et vinnent encadrer une ou plusieurs lignes.
*/

// Maximum 5 phrases/textes diff�rents par ligne !
#define MAX_SENTENCE_PER_LINE	5
#define SCREEN_WIDTH			320
#define SCREEN_HEIGHT			240
#define SENTENCE_MAX_LEN		45
#define IMG_WIDTH 				40
#define IMG_HEIGHT			 	37
#define FONT_USED 				&Font_7x10

// 'Phrase'
typedef struct{
	uint16_t color; // Couleur
	uint8_t order; // Ordre d'�criture sur la ligne, comme pour un tableau, d�marre � 0 !!
	uint16_t spacing; // Espacement sur x avant la phrase
	uint16_t x_cord; // Emplacement exact de la phrase
	uint16_t size; // Taille du tableau de caract�res
	uint16_t previous_size; // Taille pr�c�dente de string en cas d'update
	bool_e updated; // Si la line doit �tre mise � jour sur l ecran
	char string[SENTENCE_MAX_LEN]; // Phrase de maximum 45 caract�res
}sentence;

// 'Ligne'
typedef struct{
	uint16_t spacing; // Espacement sur y avant la ligne
	FontDef_t font; // Police d'�criture de la ligne
	uint16_t y_cord; // Emplacement exact de la ligne (top left corner)
	sentence sentences[MAX_SENTENCE_PER_LINE]; // Phrases � afficher sur la ligne (max 5)
	uint8_t sentences_amount; // Nombre de phrase sur la ligne
	bool_e updated; // Si la line doit �tre mise � jour sur l ecran
}line;

// L'�cran peut contenir au maximum 24 lignes (calcul� en fonction de sa taille : 24 lignes en police minimale
static line screen_display[24];
// Derni�re ligne cr�e
static uint8_t current_line = 0;
static bool_e line_deleted = FALSE;

/*
 * @brief Renvoie en pixel la taille d'une phrase
 * @param line_number : num�ro de la ligne consern�e
 * @param sentence_ordrer : ordre de la phrase concern�e
 * @ret La taille de la phrase en pixel
 */
static uint16_t TFT_get_sentence_size(uint8_t line_number, uint8_t sentence_order){
	line * line_ref = &screen_display[line_number];
	sentence * sentence_ref = &line_ref->sentences[sentence_order];
	return(sentence_ref->size*line_ref->font.FontWidth);
}

/*
 * @brief Ajoute une ligne dans le stockage de l'�cran (attention, cette fonction n'affiche rien !
 * @param *font : Pointer to @ref FontDef_t used font, s'applique � toute la ligne !
 * @param spacing : Espacement AVANT l'�criture du texte (interligne sup�rieure)
 * @ret Le num�ro de la ligne qui devra �tre utilis� pour 'add_sentence'
 */
static uint8_t TFT_add_line(FontDef_t *font, uint16_t spacing){
	line * line_ref = &screen_display[current_line];
	uint16_t y = current_line==0?0:screen_display[current_line-1].y_cord+screen_display[current_line-1].font.FontHeight;
	line_ref->font = *font;
	line_ref->spacing = spacing;
	line_ref->y_cord = y += spacing;
	line_ref->sentences_amount = 0;
	current_line += 1;
	return (current_line-1);
}

/*
 * @biref Ajoute du texte sur le ligne donn�e (attention, cette fonction ne met pas � jours l'�cran !)
 * @param line_number : num�ro de la ligne consern�e
 * @param color : couleur du texte souhait�e
 * @param spacing : espacement en x avec la phrase pr�cedente ou le bord de l'�ran
 * @param * string[] : tableau de caract�res (String) contenant la phrase � �crire
 * @ret L'ordre de la phrase dans la ligne (de 1 � MAX_SENTENCE_PER_LINE inclus)
 */
static uint8_t TFT_add_sentence(uint8_t line_number, uint16_t color, uint16_t spacing, char * string){
	line * line_ref = &screen_display[line_number];
	uint8_t order = line_ref->sentences_amount;

	// Si l'on d�passe le nombre de phrases par ligne, on ne fait rien
	if(order > MAX_SENTENCE_PER_LINE) return MAX_SENTENCE_PER_LINE;

	sentence * sentence_ref = &line_ref->sentences[order];

	uint16_t x = order==0?0:line_ref->sentences[order-1].x_cord + TFT_get_sentence_size(line_number, order-1);
	sentence_ref->color = color;
	sentence_ref->order = order;
	sentence_ref->spacing = spacing;
	sentence_ref->x_cord = x + spacing;
	sentence_ref->updated = TRUE;

	uint8_t i=0;
	while(*string){
		sentence_ref->string[i] = *string++;
		i++;
	}
	sentence_ref->size = i;
	sentence_ref->previous_size = i;

	line_ref->sentences_amount += 1;

	return order;
}

/*
 * @brief Modifie une phrase d�j� cr��e et ajuste la disposition des phrases suivantes
 * @param line_number : num�eo de la ligne consern�e
 * @param sentence_ordrer : ordre de la phrase concern�e
 * @param color : couleur du texte souhait�e
 * @param * string[] : tableau de caract�res (String) contenant la phrase � �crire
 */

static void TFT_edit_sentence(uint8_t line_number, uint8_t sentence_order, uint16_t color, char * string){
	line * line_ref = &screen_display[line_number];
	sentence * sentence_ref = &line_ref->sentences[sentence_order];
	sentence_ref->color = color;
	sentence_ref->updated = TRUE;

	uint8_t i=0;
	while(*string){
		sentence_ref->string[i] = *string++;
		i++;
	}
	sentence_ref->size = i;
	while(i<sentence_ref->previous_size){
		sentence_ref->string[i] = 0;
		i++;
	}

	for(uint8_t i=1; i<=line_ref->sentences_amount-sentence_order; i++){
		sentence_ref = &line_ref->sentences[sentence_order+i];
		sentence * previous_sentence_ref = &line_ref->sentences[sentence_order+i-1];
		sentence_ref->previous_size += (sentence_ref->x_cord - previous_sentence_ref->x_cord)/line_ref->font.FontWidth;
		sentence_ref->x_cord = previous_sentence_ref->x_cord + TFT_get_sentence_size(line_number, previous_sentence_ref->order) + sentence_ref->spacing;
		sentence_ref->updated = TRUE;
	}
}

/*
 * @brief Supprime une phrase d'une ligne et d�calle les suivantes
 * @param line_number : num�ro de la ligne consern�e
 * @param sentence_ordrer : ordre de la phrase concern�e
 */
static void TFT_delete_sentence(uint8_t line_number, uint8_t sentence_order){
	line * line_ref = &screen_display[line_number];
	sentence * sentence_ref = &line_ref->sentences[sentence_order];

	uint16_t x = sentence_ref->x_cord - sentence_ref->spacing;

	for(uint8_t i=sentence_order+1; i<line_ref->sentences_amount; i++){
		sentence_ref = &line_ref->sentences[i];
		sentence_ref->x_cord = x + sentence_ref->spacing;
		x += sentence_ref->spacing + TFT_get_sentence_size(line_number, i);
		sentence_ref->updated = TRUE;
	}

	for(uint8_t i=sentence_order; i<line_ref->sentences_amount-1; i++){
		line_ref->sentences[i] = line_ref->sentences[i+1];
	}

	line_ref->sentences_amount --;
	line_ref->updated = TRUE;
}

/*
 * @brief Supprime tout le contenu de l'�cran. ATTENTION : effet imm�diat !
 */
static void TFT_delete_all(void){
	for(uint8_t i=0; i<current_line; i++){
		line * line_ref = &screen_display[i];
		for(uint8_t j=0; j<line_ref->sentences_amount; j++){
			sentence * sentence_ref = &line_ref->sentences[j];
			for(uint8_t k=0; k<sentence_ref->size; k++){
				sentence_ref->string[k]=0;
			}
		}
		line_ref->sentences_amount = 0;
	}
	current_line = 0;
	ILI9341_Fill(ILI9341_COLOR_WHITE);
}

/*
 * @brief Met � jour l'�cran (si il y a des mises � jours)
 */
static void TFT_update_screen(void){
	for(uint8_t i=0; i<current_line; i++){
		line * line_ref = &screen_display[i];
		uint16_t y = line_ref->y_cord;
		if(line_ref->updated){
			ILI9341_DrawFilledRectangle(0, y, SCREEN_WIDTH, y+line_ref->font.FontHeight, ILI9341_COLOR_WHITE);
			line_ref->updated = FALSE;
		}
		for(uint8_t j=0; j<line_ref->sentences_amount; j++){
			sentence * sentence_ref = &line_ref->sentences[j];
			if(sentence_ref->updated){
				uint16_t x = sentence_ref->x_cord;
				if(sentence_ref->size != sentence_ref->previous_size){
					ILI9341_DrawFilledRectangle(x, y, x+(sentence_ref->previous_size*line_ref->font.FontWidth), y+line_ref->font.FontHeight, ILI9341_COLOR_WHITE);
					sentence_ref->previous_size = sentence_ref->size;
				}
				ILI9341_Puts(x, y, &sentence_ref->string[0], &line_ref->font, sentence_ref->color, ILI9341_COLOR_WHITE);
				sentence_ref->updated = FALSE;
			}
		}
	}
	if(line_deleted){
		line * line_ref = &screen_display[current_line+1];
		ILI9341_DrawFilledRectangle(0, line_ref->y_cord, SCREEN_WIDTH, line_ref->y_cord+line_ref->font.FontHeight, ILI9341_COLOR_WHITE);
		line_ref->updated = FALSE;
	}
}

/*
 * @biref Dessine un rectangle d�coratif sur l'�cran.
 * @param line_number : num�ro de la ligne consern�e
 * @param line_amount : nombre de ligne englob�es par le rectangle (de 1 � 24), 1�re ligne incluse
 * @param spacing : espacement vertical entre le texte et le bord int�rieur du rectangle
 * @param color : couleur du rectangle
 * @param padding right : espacement horizontal sur la droite du rectangle
 * @param padding left : espacement horizontal sur la gauche du rectangle
 */
static void TFT_create_rectangle_line(uint8_t line_number, uint8_t line_amount, uint16_t spacing, uint16_t color, uint8_t width, uint16_t padding_left, uint16_t padding_right){
	line * line_ref = &screen_display[line_number];
	uint16_t x0 = padding_left;
	uint16_t y0 = line_ref->y_cord - spacing;
	uint16_t x1 = SCREEN_WIDTH - padding_right;
	uint16_t y1 = line_ref->y_cord + line_ref->font.FontHeight + spacing;
	for(uint8_t i=1; i<line_amount; i++){
		line * line_ref = &screen_display[line_number+i];
		y1 += line_ref->spacing + line_ref->font.FontHeight;
	}
	ILI9341_DrawFilledRectangle(x0, y0-width, x1, y0, color);
	ILI9341_DrawFilledRectangle(x0, y0, x0+width, y1, color);
	ILI9341_DrawFilledRectangle(x1-width, y0, x1, y1, color);
	ILI9341_DrawFilledRectangle(x0, y1, x1, y1+-width, color);
}

void TFT_home_screen(){
	TFT_delete_all();

	// Logo Regucolo
	ILI9341_putImage(20, 10, IMG_WIDTH, IMG_HEIGHT, TFT_get_image(), IMG_WIDTH*IMG_HEIGHT);
	// Information sur l'application
	TFT_add_sentence(TFT_add_line(&Font_11x18, 20), ILI9341_COLOR_BLACK, IMG_WIDTH+30, "Regucolo");
	TFT_add_sentence(TFT_add_line(FONT_USED, 0), ILI9341_COLOR_BLACK, 260, "v1.2.4");

	//Premier rectangle d'informations
	TFT_add_sentence(TFT_add_line(FONT_USED, 10), ILI9341_COLOR_BLACK, 6, "Etat de la vanne :");
	TFT_add_sentence(current_line-1, ILI9341_COLOR_RED, 7, "FERMEE");
	TFT_add_sentence(TFT_add_line(FONT_USED, 3), ILI9341_COLOR_BLACK, 6, "Connexion avec l'application :");
	TFT_add_sentence(current_line-1, ILI9341_COLOR_GREEN, 7, "ON");
	TFT_create_rectangle_line(2, 2, 3, ILI9341_COLOR_RED, 2, 3, 3);

	//Deuxi�me rectangle d'informations
	TFT_add_sentence(TFT_add_line(FONT_USED, 15), ILI9341_COLOR_BLUE2, 6, "Consommation en cours");
	TFT_add_sentence(TFT_add_line(FONT_USED, 7), ILI9341_COLOR_BLACK, 6, "Debit d'eau courant :");
	TFT_add_sentence(current_line-1, ILI9341_COLOR_BLACK, 7, "0");
	TFT_add_sentence(current_line-1, ILI9341_COLOR_BLACK, 7, "mL/min");
	TFT_add_sentence(TFT_add_line(FONT_USED, 3), ILI9341_COLOR_BLACK, 6, "Eau consommee :");
	TFT_add_sentence(current_line-1, ILI9341_COLOR_BLACK, 7, "0");
	TFT_add_sentence(current_line-1, ILI9341_COLOR_BLACK, 7, "mL");
	TFT_add_sentence(TFT_add_line(FONT_USED, 3), ILI9341_COLOR_BLACK, 6, "Type de douche selectionnee :");
	TFT_add_sentence(current_line-1, ILI9341_COLOR_BLACK, 6, "AUCUNE");
	TFT_create_rectangle_line(4, 4, 3, ILI9341_COLOR_BLUE2, 2, 3, 3);

	//Troisi�me rectangle d'information
	TFT_add_sentence(TFT_add_line(FONT_USED, 15), ILI9341_COLOR_MAGENTA, 6, "Console");
	TFT_add_line(FONT_USED, 7);
	TFT_add_line(FONT_USED, 3);
	TFT_add_line(FONT_USED, 3);
	TFT_add_line(FONT_USED, 3);
	TFT_add_console("Initialisation de la console");
	TFT_create_rectangle_line(8, 5, 3, ILI9341_COLOR_MAGENTA, 2, 3, 3);

	TFT_update_screen();

}

void TFT_add_console(char * string){
	static uint8_t command_amount = 0;
	if(command_amount<4){
		TFT_add_sentence(9+command_amount, ILI9341_COLOR_BLACK, 6, string);
		command_amount+=1;
	}
	else{
		for(uint8_t i=0; i<3; i++){
			line * line_ref = &screen_display[10+i];
			sentence * sentence_ref = &line_ref->sentences[0];
			TFT_edit_sentence(9+i, 0, ILI9341_COLOR_BLACK, &sentence_ref->string[0]);
		}
		TFT_edit_sentence(12, 0, ILI9341_COLOR_BLACK, string);
	}
}

void TFT_init(void){
	// Initialisation de la planche de dessin
	ILI9341_Init();
	ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
	ILI9341_Fill(ILI9341_COLOR_WHITE);

	// Logo Regucolo
	ILI9341_putImage(140, 63, IMG_WIDTH, IMG_HEIGHT, TFT_get_image(), IMG_WIDTH*IMG_HEIGHT);

	TFT_add_sentence(TFT_add_line(&Font_11x18, 100), ILI9341_COLOR_BLACK, 39, "Bienvenue sur Regucolo");
	TFT_add_sentence(TFT_add_line(FONT_USED, 5), ILI9341_COLOR_BLACK, 34, "Pour commencer, lancez l'application" );
	TFT_add_sentence(TFT_add_line(FONT_USED, 3), ILI9341_COLOR_BLACK, 9, "Reguloco sur votre smartphone et connectez-");
	TFT_add_sentence(TFT_add_line(FONT_USED, 3), ILI9341_COLOR_BLACK, 90, "vous au regulateur !");

	TFT_update_screen();
}

/*
 * @brief Switch le texte indiquant l'�tat de la vanne
 * @param state : TRUE = OUVERTE, FALSE = FERMEE
 */
void TFT_set_vanne(bool_e state){
	uint16_t color = state?ILI9341_COLOR_GREEN:ILI9341_COLOR_RED;
	char * text = state?"OUVERTE":"FERMEE";
	TFT_edit_sentence(2, 1, color, &text[0]);
	text = state?"Ouverture de la vanne":"Fermeture de la vanne";
	TFT_add_console(text);
	TFT_update_screen();
}

/*
 * @brief Switch le texte indiquant l'�tat de la connexion
 * @param state : TRUE = ON, FALSE = OFF
 */
void TFT_set_connexion(bool_e state){
	uint16_t color = state?ILI9341_COLOR_GREEN:ILI9341_COLOR_RED;
	char * text = state?"ON":"OFF";
	TFT_edit_sentence(3, 1, color, &text[0]);
	text = state?"Connexion bluetooth etablie":"Perte de la connexion bluetooth";
	TFT_add_console(text);
	TFT_update_screen();
}

/*
 * @brief Modifie le texte indiquant le type de douche choisie
 * @param state :  200 = DIEUX, 150 = CLASSIQUE, 90 = ECOLOGIQUE, Autre = AUCUNE
 */
void TFT_set_shower(uint8_t value){
	uint16_t color;
	char * text;
	if(value==0){
		color = ILI9341_COLOR_BLACK;
		text = "AUCUNE";
	}
	else if(value<150){
		color = ILI9341_COLOR_GREEN;
		text = "ECOLOGIQUE";
	}
	else if(value<200){
		color = ILI9341_COLOR_BLUE2;
		text = "CLASSIQUE";
	}
	else{
		color = ILI9341_COLOR_RED;
		text = "DIEUX";
	}
	TFT_edit_sentence(7, 1, color, &text[0]);
	TFT_update_screen();
}

void TFT_update_info(void){
	// R�cup�re de d�bit d'eau courant
	static uint16_t flow[2] = {0, 1};
	static uint16_t consumption[2] = {0, 1};

	flow[0] = DEBIMETRE_get_flow();
	char tab_flow[5];
	sprintf(tab_flow, "%d", DEBIMETRE_get_flow());

	// R�cup��e la quantit� d'eau consomm�e
	consumption[0] = DEBIMETRE_get_consumption();
	char tab_consumption[10];
	sprintf(tab_consumption, "%d", DEBIMETRE_get_consumption());

	// Affiche le d�bit d'eau courant
	if(flow[0] != flow[1]){
		TFT_edit_sentence(5, 1, ILI9341_COLOR_BLACK, &tab_flow);
		flow[1] = flow[0];
	}

	// Affiche la quanti� d'eau consomm�e
	if(consumption[0] != consumption[1]){
		TFT_edit_sentence(6, 1, ILI9341_COLOR_BLACK, &tab_consumption);
		consumption[1] = consumption[0];
	}
	TFT_update_screen();
}
