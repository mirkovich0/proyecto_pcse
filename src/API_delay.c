/*
 * API_delay.c
 *
 *  Created on: Jul 5, 2024
 *      Author: Mirko Serra
 */

#include "API_delay.h"

#include "stm32f4xx_hal.h"

void delayInit(delay_t* delay, tick_t duration)
{
    assert(delay);
    delay->duration = duration;
    delay->running = false;
}

bool_t delayRead(delay_t* delay)
{
    assert(delay);
    if (!delay->running) {
        delay->running = true;
        delay->startTime = HAL_GetTick();
    }
    if (delay->startTime + delay->duration <= HAL_GetTick()) {
        delay->running = false;
        return true;
    }
    return false;
}

void delayWrite(delay_t* delay, tick_t duration)
{
    assert(delay);
    delay->duration = duration;
}

bool_t delayIsRunning(delay_t* delay)
{
    assert(delay);
    return delay->running;
}
