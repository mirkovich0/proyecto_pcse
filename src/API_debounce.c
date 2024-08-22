/*
 * API_debounce.c
 *
 *  Created on: Jul 22, 2024
 *      Author: Mirko Serra
 */

#include "API_debounce.h"
#include "API_delay.h"

#include "stm32f4xx_nucleo_144.h"   /* <- BSP include */

typedef enum{
    BUTTON_UP,
    BUTTON_FALLING,
    BUTTON_DOWN,
    BUTTON_RAISING,
} debounceState_t;

static bool_t key_pressed = false;

static const tick_t DEBOUNCING_TICKS = 40;
static debounceState_t machine_state = BUTTON_UP;
static delay_t debouncing_delay;

static callback_t buttonPressed = NULL;
static callback_t buttonReleased = NULL;

bool_t readKey()
{
    bool_t retval = key_pressed;
    key_pressed = false;
    return retval;
}

/* Carga el estado inicial */
void debounceFSM_init(callback_t buttonPressedCallback, callback_t buttonReleasedCallback)
{
    buttonPressed = buttonPressedCallback;
    buttonReleased = buttonReleasedCallback;
    machine_state = BUTTON_UP;
    key_pressed = false;
    delayInit(&debouncing_delay, DEBOUNCING_TICKS);
}

/* debe leer las entradas, resolver la lógica de
   transición de estados y actualizar las salidas */
void debounceFSM_update()
{
    bool button_state = BSP_PB_GetState(BUTTON_USER);
    switch (machine_state) {
    case BUTTON_UP:
        if (button_state) {
            machine_state = BUTTON_FALLING;
            delayInit(&debouncing_delay, DEBOUNCING_TICKS);
        }
        break;
    case BUTTON_FALLING:
        if (delayRead(&debouncing_delay)) {
            if (button_state) {
                machine_state = BUTTON_DOWN;
                key_pressed = true;
                if (buttonPressed) {
                    buttonPressed();
                }
            } else {
                machine_state = BUTTON_UP;
            }
        }
        break;
    case BUTTON_DOWN:
        if (!button_state) {
            machine_state = BUTTON_RAISING;
            delayInit(&debouncing_delay, DEBOUNCING_TICKS);
        }
        break;
    case BUTTON_RAISING:
        if (delayRead(&debouncing_delay)) {
            if (!button_state) {
                machine_state = BUTTON_UP;
                if (buttonReleased) {
                    buttonReleased();
                }
            } else {
                machine_state = BUTTON_DOWN;
            }
        }
        break;
    default:
        machine_state = BUTTON_UP;
    }
}

