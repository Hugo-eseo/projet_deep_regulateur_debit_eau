/*
 * debimetre.h
 *
 *  Created on: 16 déc. 2021
 *      Author: hugob
 */

#ifndef DEBIMETRE_H_
#define DEBIMETRE_H_

#include "macro_types.h"

void DEBIMETRE_init(void);
void DEBIMETRE_handler(void);

uint32_t DEBIMETRE_get_flow(void);
void DEBIMETRE_set_flow(uint16_t flow);

uint32_t DEBIMETRE_get_stop_value(void);
void DEBIMETRE_set_stop_value(uint16_t value);

uint32_t DEBIMETRE_get_consumption(void);
void DEBIMETRE_set_consumption(uint16_t value);

bool_e DEBIMETRE_get_flag();
void DEBIMETRE_set_flag(bool_e flag);

#endif /* DEBIMETRE_H_ */
