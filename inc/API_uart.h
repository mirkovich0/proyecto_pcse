/*
 * API_uart.h
 *
 *  Created on: Jul 26, 2024
 *      Author: Mirko Serra
 */

#ifndef API_INC_API_UART_H_
#define API_INC_API_UART_H_

#include <stdbool.h>
#include <stdint.h>

typedef bool bool_t;

bool_t uartInit();
void uartSendString(const uint8_t * pstring);
void uartSendStringSize(const uint8_t * pstring, uint16_t size);
void uartReceiveStringSize(uint8_t * pstring, uint16_t size);

#endif /* API_INC_API_UART_H_ */
