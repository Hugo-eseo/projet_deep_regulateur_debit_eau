/*
 * tft.c
 *
 *  Created on: 16 déc. 2021
 *      Author: hugob
 */

#include "tft.h"
#include "config.h"

// Bibliothèque de l'écran TFT
#include "tft_ili9341/stm32f1_ili9341.h"

static uint8_t TFT_add_line(FontDef_t font, uint8_t spacing);
static uint8_t TFT_add_sentence(uint8_t line_number, uint16_t color, uint8_t spacing, char * string[]);
static void TFT_edit_sentence(uint8_t line_number, uint8_t sentence_order, uint16_t color, char * string[]);
static uint16_t TFT_get_sentence_size(uint8_t line_number, uint8_t sentence_order);
static void TFT_update_screen(void);
static void TFT_create_rectangle_line(uint8_t line_number, uint8_t line_amount, uint8_t spacing, uint16_t color, uint8_t width);

/* Principe de fonctionnement de l'affichage :
*		- L'écran est divisé en plusieurs 'lignes'. Chaque ligne crée l'une à la suite de l'autre possède ses propores caractérisiques
*		- Chaque ligne est ensuite subdivisée en plusieurs 'phrases' ayant également ses propres caractéristiques.
*		Cela permet d'afficher et d'effacer facilement des informations sur l'ecran.
*		- Enfin, les 'rectangles décoratifs' sont indépendant du système précedent et vinnent encadrer une ou plusieurs lignes.
*/

// Maximum 5 phrases/textes différents par ligne !
#define MAX_SENTENCE_PER_LINE 5
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// 'Phrase'
typedef struct{
	uint16_t color; // Couleur
	uint8_t order; // Ordre d'écriture sur la ligne, comme pour un tableau, démarre à 1 !!
	uint8_t spacing; // Espacement sur x avant la phrase
	uint16_t x_cord; // Emplacement exact de la phrase
	uint8_t size; // Taille du tableau de caractères
	bool_e updated; // Si la line doit être mise à jour sur l ecran
	char * string[]; // Phrase de maximum 30 caractères
}sentence;

// 'Ligne'
typedef struct{
	uint8_t spacing; // Espacement sur y avant la ligne
	FontDef_t font; // Police d'écriture de la ligne
	uint16_t y_cord; // Emplacement exact de la ligne (top left corner)
	sentence sentences[MAX_SENTENCE_PER_LINE]; // Phrases à afficher sur la ligne (max 5)
	uint8_t sentences_amount;
}line;

// L'écran peut contenir au maximum 24 lignes (calculé en fonction de sa taille : 24 lignes en police minimale
static line screen_display[24];
static uint8_t current_line = 0;

/*
 * @brief Ajoute une ligne dans le stockage de l'écran (attention, cette fonction n'affiche rien !
 * @param *font : Pointer to @ref FontDef_t used font, s'applique à toute la ligne !
 * @param spacing : Espacement AVANT l'écriture du texte (interligne supérieure)
 * @ret Le numéro de la ligne qui devra être utilisé pour 'add_sentence'
 */
static uint8_t TFT_add_line(FontDef_t font, uint8_t spacing){
	line * line_ref = &screen_display[current_line];
	uint16_t y = current_line==0?0:screen_display[current_line-1].y_cord+screen_display[current_line-1].font.FontHeight;
	current_line += 1;
	line_ref->font = font;
	line_ref->spacing = spacing;
	line_ref->y_cord = y += spacing;
	line_ref->sentences_amount = 0;
	return current_line;
}

/*
 * @biref Ajoute du texte sur le ligne donnée (attention, cette fonction ne met pas à jours l'écran !)
 * @param line_number : numéro de la ligne consernée
 * @param color : couleur du texte souhaitée
 * @param spacing : espacement en x avec la phrase précedente ou le bord de l'écran
 * @param * string[] : tableau de caractères (String) contenant la phrase à écrire
 * @ret L'ordre de la phrase dans la ligne (de 1 à MAX_SENTENCE_PER_LINE inclus)
 */
static uint8_t TFT_add_sentence(uint8_t line_number, uint16_t color, uint8_t spacing, char * string[]){
	line * line_ref = &screen_display[line_number];
	line_ref->sentences_amount += 1;
	uint8_t order = line_ref->sentences_amount;

	// Si l'on dépasse le nombre de phrases par ligne, on ne fait rien
	if(order > MAX_SENTENCE_PER_LINE) return;

	sentence * sentence_ref = &line_ref->sentences[order];
	uint16_t x = order==1?0:line_ref->sentences[order-1].x_cord + TFT_get_sentence_size(line_number, line_ref->sentences[order-1].order);
	sentence_ref->color = color;
	sentence_ref->order = order;
	sentence_ref->spacing = spacing;
	sentence_ref->x_cord = x + spacing;
	sentence_ref->size = sizeof(string);
	sentence_ref->updated = TRUE;
	for(int i=0; i<sentence_ref->size; i++){
		sentence_ref->string[i] = string[i];
	}
	return order;
}

/*
 * @brief Modifie une phrase déjà créer et ajuste la disposition des phrases suivantes
 * @param line_number : numéro de la ligne consernée
 * @param sentence_ordrer : ordre de la phrase concernée
 * @param color : couleur du texte souhaitée
 * @param * string[] : tableau de caractères (String) contenant la phrase à écrire
 */

static void TFT_edit_sentence(uint8_t line_number, uint8_t sentence_order, uint16_t color, char * string[]){
	line * line_ref = &screen_display[line_number];
	sentence * sentence_ref = &line_ref->sentences[sentence_order];
	sentence_ref->color = color;
	sentence_ref->updated = TRUE;
	for(int i=0; i<sentence_ref->size; i++){
		sentence_ref->string[i] = string[i];
	}
	sentence_ref->size = sizeof(string);
	for(int i=1; i<=line_ref->sentences_amount-sentence_order; i++){
		sentence_ref = &line_ref->sentences[sentence_order+i];
		sentence * previous_sentence_ref = &line_ref->sentences[sentence_order+i-1];
		sentence_ref->x_cord = previous_sentence_ref->x_cord + TFT_get_sentence_size(line_number, previous_sentence_ref->order) + sentence_ref->spacing;
		sentence_ref->updated = TRUE;
	}
}

/*
 * @brief Renvoie en pixel la taille d'une phrase
 * @param line_number : numéro de la ligne consernée
 * @param sentence_ordrer : ordre de la phrase concernée
 * @ret La taille de la phrase en pixel
 */
static uint16_t TFT_get_sentence_size(uint8_t line_number, uint8_t sentence_order){
	line * line_ref = &screen_display[line_number];
	sentence * sentence_ref = &line_ref->sentences[sentence_order];
	return(sentence_ref->size*line_ref->font.FontWidth);
}

// A terminer !
/*
static void TFT_delete_sentence(void){
	// A compléter
}

static void TFT_delete_line(void){
	// A compléter
}*/

static void TFT_update_screen(void){
	for(int i=0; i<current_line+1; i++){
		line * line_ref = &screen_display[i];
		for(int j=0; j<MAX_SENTENCE_PER_LINE; i++){
			sentence * sentence_ref = &line_ref->sentences[j];
			if(sentence_ref->updated){
				ILI9341_DrawFilledRectangle(sentence_ref->x_cord, line_ref->y_cord, sentence_ref->x_cord+(sentence_ref->size*line_ref->font.FontWidth), line_ref->font.FontHeight, sentence_ref->color);
				ILI9341_Puts(sentence_ref->x_cord, line_ref->y_cord, &sentence_ref->string[0], &line_ref->font, sentence_ref->color, ILI9341_COLOR_WHITE);
			}
		}
	}
}

static void TFT_create_rectangle_line(uint8_t line_number, uint8_t line_amount, uint8_t spacing, uint16_t color, uint8_t width){
	line * line_ref = &screen_display[line_number];
	uint8_t x0 = spacing;
	uint8_t y0 = line_ref->y_cord - spacing;
	uint8_t x1 = SCREEN_WIDTH - spacing;
	uint8_t y1 = line_ref->y_cord + line_ref->font.FontHeight + spacing;
	for(int i=1; i<=line_amount; i++){
		line * line_ref = &screen_display[line_number+i];
		y1 += line_ref->spacing + line_ref->font.FontHeight;
	}
	ILI9341_DrawFilledRectangle(x0, y0, x1, y0+width, color);
	ILI9341_DrawFilledRectangle(x0, y0, x0+width, y1, color);
	ILI9341_DrawFilledRectangle(x1-width, y0, x1, y1, color);
	ILI9341_DrawFilledRectangle(x0, y1-width, x1, y1, color);
}

void TFT_init(void){
	#define IMG_WIDTH 40
	#define IMG_HEIGHT 37
	#define LINES_SPACE 5
	#define FONT_USED &Font_7x10
	#define FONT_WIDTH 7
	#define FONT_HEIGHT 10
	#define FIRST_RECTANGLE_LINE 2
	#define SECOND_RECTANGLE_LINE 5
	#define THIRD_RECTANGLE_LINE 2

	static const char *FIRST_RECTANGLE[FIRST_RECTANGLE_LINE] = {
		"Etat de la vanne : OUVERTE",
		"Connexion avec l'application : OFF",
	};

	static const char *SECOND_RECTANGLE[SECOND_RECTANGLE_LINE] = {
		"Consommation en cours",
		"Debit d'eau courant : 0 L/min",
		"Eau consommee : 0 L",
		"Type de douche selectionnee :",
		"ECOLOGIQUE"
	};

	static const char *THIRD_RECTANGLE[SECOND_RECTANGLE_LINE] = {
		"Historique",
		"11/01/22 : 12L"
	};

	static uint16_t line = 10;
	static uint16_t saved_line = 10;

	// Initialisation de la planche de dessin
	ILI9341_Init();
	ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
	ILI9341_Fill(ILI9341_COLOR_WHITE);

	// Logo Regucolo
	ILI9341_putImage(20, line, IMG_WIDTH, IMG_HEIGHT, TFT_img, IMG_WIDTH*IMG_HEIGHT);
	// Information sur l'application
	ILI9341_Puts(IMG_WIDTH+30, 20, "Regucolo", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(260, 28, "v0.1.1", FONT_USED, ILI9341_COLOR_RED, ILI9341_COLOR_WHITE);

	// Premier cadre d'information sur l'état courant du système
	saved_line = line + IMG_HEIGHT + 5;
	line += IMG_HEIGHT + 7 + LINES_SPACE;
	for(int i=0; i<FIRST_RECTANGLE_LINE; i++){
		ILI9341_Puts(FONT_WIDTH + 4, line, &FIRST_RECTANGLE[i][0], FONT_USED, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		line += FONT_HEIGHT + LINES_SPACE;
	}
	// TFT_draw_fat_rectangle(FONT_WIDTH, saved_line, 320 - FONT_WIDTH, line + LINES_SPACE, ILI9341_COLOR_RED, 2);

	// Second cadre d'information sur la consommation en cours
	saved_line = line + 2*LINES_SPACE;
	line += 2 + 3*LINES_SPACE;
	for(int i=0; i<SECOND_RECTANGLE_LINE; i++){
		ILI9341_Puts(FONT_WIDTH + 4, line, &SECOND_RECTANGLE[i][0], FONT_USED, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		line += FONT_HEIGHT + LINES_SPACE;
	}
	// TFT_draw_fat_rectangle(FONT_WIDTH, saved_line, 320 - FONT_WIDTH, line + LINES_SPACE, ILI9341_COLOR_BLUE, 2);

	// Troisième cadre contenant l'historique
	saved_line = line + 2*LINES_SPACE;
	line += 2 + 3*LINES_SPACE;
	for(int i=0; i<THIRD_RECTANGLE_LINE; i++){
		ILI9341_Puts(FONT_WIDTH + 4, line, &THIRD_RECTANGLE[i][0], FONT_USED, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		line += FONT_HEIGHT + LINES_SPACE;
	}
	// TFT_draw_fat_rectangle(FONT_WIDTH, saved_line, 320 - FONT_WIDTH, line + LINES_SPACE, ILI9341_COLOR_MAGENTA, 2);
}

void TFT_update_info(void){
	// Récupère de débit d'eau courant
	static uint16_t flow[2] = {0, 1};
	static uint16_t consumption[2] = {0, 1};

	flow[0] = DEBIMETRE_get_flow();
	char tab_flow[5];
	sprintf(tab_flow, "%d", DEBIMETRE_get_flow());

	// Récupère la quantité d'eau consommée
	consumption[0] = DEBIMETRE_get_consumption();
	char tab_consumption[10];
	sprintf(tab_consumption, "%d", DEBIMETRE_get_consumption());

	/*
	// Affiche le débit d'eau courant
	if(flow[0] != flow[1]){
		ILI9341_INT_Fill(262, 100, 317, 118, ILI9341_COLOR_WHITE);
		ILI9341_Puts(20, 100, "Debit d'eau courant : ", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		ILI9341_Puts(262, 100, &tab_flow[0], &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		flow[1] = flow[0];
	}

	// Affiche la quantité d'eau consommée
	if(consumption[0] != consumption[1]){
		ILI9341_INT_Fill(207, 136, 317, 154, ILI9341_COLOR_WHITE);
		ILI9341_Puts(20, 136, "Eau consommee : ", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		ILI9341_Puts(207, 136, &tab_consumption[0], &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		consumption[1] = consumption[0];
	}
	*/
}
