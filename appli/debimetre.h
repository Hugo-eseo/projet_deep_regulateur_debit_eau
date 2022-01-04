/*
 * debimetre.h
 *
 *  Created on: 16 déc. 2021
 *      Author: hugob
 */

#ifndef DEBIMETRE_H_
#define DEBIMETRE_H_

void DEBIMETRE_init(void);
void DEBIMETRE_handler(void);

uint16_t DEBIMETRE_get_flow(void);
uint32_t DEBIMETRE_get_consumption(void);

#endif /* DEBIMETRE_H_ */
