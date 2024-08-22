/*
 * API_delay.h
 *
 *  Created on: Jul 5, 2024
 *      Author: Mirko Serra
 */

#ifndef API_INC_API_DELAY_H_
#define API_INC_API_DELAY_H_

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t tick_t; // Tipo de dato usado para los ticks
typedef bool bool_t; // Booleano

/* Estructura que representa un timer */
typedef struct {
    tick_t startTime;
    tick_t duration;
    bool_t running;
} delay_t;

/* Inicializa una estructura delay_t con una duración */
void delayInit(delay_t* delay, tick_t duration);
/* Inicia la cuenta de una estructura delay_t si no está corriendo y verifica si
 * el timer ha caducado, devolviendo verdadero cuando este es el caso
 */
bool_t delayRead(delay_t* delay);
/* Modifica la duración del timer */
void delayWrite(delay_t* delay, tick_t duration);
bool_t delayIsRunning(delay_t * delay);

#ifdef __cplusplus
}
#endif

#endif /* API_INC_API_DELAY_H_ */
