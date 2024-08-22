/*
 * API_debounce.h
 *
 *  Created on: Jul 22, 2024
 *      Author: Mirko Serra
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#include <stdbool.h>

typedef bool bool_t;

typedef void (*callback_t)();

void debounceFSM_init(callback_t buttonPressedCallback, callback_t buttonReleasedCallback);
void debounceFSM_update();
bool_t readKey();

#endif /* API_INC_API_DEBOUNCE_H_ */
