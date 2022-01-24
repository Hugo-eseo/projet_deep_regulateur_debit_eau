/*
 * tft.h
 *
 *  Created on: 16 déc. 2021
 *      Author: hugob
 */

#ifndef TFT_H_
#define TFT_H_

#include "macro_types.h"

void TFT_init(void);
void TFT_update_info(void);
void TFT_home_screen(void);
void TFT_set_vanne(bool_e state);
void TFT_set_connexion(bool_e state);
void TFT_set_shower(uint8_t state);
void TFT_add_console(char * string);

int16_t * TFT_get_image();
//const int16_t TFT_img[];

#endif /* TFT_H_ */
