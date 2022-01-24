/*
 * bluetooth.h
 *
 *  Created on: 18 janv. 2022
 *      Author: Tristan
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include "macro_types.h"

typedef enum
{
	DISCONNECTED,
	CONNECTED
}bluetooth_status;

bool_e BLUETOOTH_get_flag(void);
void BLUETOOTH_set_flag(bool_e flag);

bluetooth_status BLUETOOTH_get_status(void);

void BLUETOOTH_get_data(void);
void BLUETOOTH_send_data(char c);

#endif /* BLUETOOTH_H_ */
