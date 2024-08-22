/*
 * API_keyboard.h
 *
 *  Created on: Aug 21, 2024
 *      Author: Mirko Serra
 */

#ifndef API_INC_API_KEYBOARD_H_
#define API_INC_API_KEYBOARD_H_

#include <stdbool.h>
#include <stdint.h>

void keyboard_Init();
int keyboard_ReadKey();
bool keyboard_KeyPressed();
void keyboard_Process();

#endif /* API_INC_API_KEYBOARD_H_ */
