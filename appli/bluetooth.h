/*
 * bluetooth.h
 *
 *  Created on: 18 janv. 2022
 *      Author: Tristan
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include "macro_types.h"

void BLUETOOTH_handler(void);
bool_e BLUETOOTH_get_flag(void);
void BLUETOOTH_set_flag(bool_e flag);
void reception(void);

#endif /* BLUETOOTH_H_ */
