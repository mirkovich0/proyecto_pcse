/*
 * API_Display.h
 *
 *  Created on: Aug 20, 2024
 *      Author: Mirko Serra
 */

#ifndef API_INC_API_DISPLAY_H_
#define API_INC_API_DISPLAY_H_

#include <stdbool.h>
#include <stdint.h>

void display_Init();
void display_Clear();
void display_Cursor(bool on, bool blinking);
void display_SetLine(int new_line);
void display_On(bool on);
void display_Process();
void display_Print(const char* fmt, ...);

#endif /* API_INC_API_DISPLAY_H_ */
