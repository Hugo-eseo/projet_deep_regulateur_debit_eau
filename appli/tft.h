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
void TFT_set_vanne(bool_e state);
void TFT_set_connexion(bool_e state);
void TFT_set_shower(int state);

const int16_t TFT_img[];

#endif /* TFT_H_ */
