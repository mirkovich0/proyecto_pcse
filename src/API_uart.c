/*
 * API_uart.c
 *
 *  Created on: Jul 26, 2024
 *      Author: Mirko Serra
 */

#include "stm32f4xx_hal.h"

#include "API_uart.h"
#include "error.h"

#include <string.h>

static const uint16_t MAX_TX_SIZE_SOFT = 255;
static const uint16_t MAX_TX_SIZE_HARD = 65535;
static const uint32_t TIMEOUT = 1000;
static const uint32_t TIMEOUT_PER_CHAR = 150;

static UART_HandleTypeDef UartHandle;

static UART_HandleTypeDef* huart = NULL;

bool_t uartInit()
{
    assert(huart == NULL);

    UartHandle.Instance = USART3;

    UartHandle.Init.BaudRate = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode = UART_MODE_TX_RX;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&UartHandle) != HAL_OK) {
        return false;
    }
    huart = &UartHandle;

    return true;
}

void uartSendString(const uint8_t* pstring)
{
    assert(huart);
    assert(pstring != NULL);

    size_t size = strlen((const char*) pstring);

    assert(size <= MAX_TX_SIZE_HARD);

    return uartSendStringSize(pstring, size);
}

void o(const void*) {}

void uartSendStringSize(const uint8_t* pstring, uint16_t size)
{
    assert(huart);
    assert(pstring != NULL);
    assert(size >= 0);
    assert(size <= MAX_TX_SIZE_SOFT);

    if (size == 0) {
        return;
    }

    HAL_StatusTypeDef ret = HAL_UART_Transmit(huart, pstring, size, TIMEOUT + size * TIMEOUT_PER_CHAR);
    if (HAL_OK != ret) {
        Handle_Error("HAL_UART_Transmit not ok", (void*) ret);
    }
}

void uartReceiveStringSize(uint8_t* pstring, uint16_t size)
{
    assert(huart);
    assert(pstring != NULL);
    assert(size > 0);

    size_t s = size;
    while (s--) {
        pstring[s] = 0;
    }

    HAL_StatusTypeDef ret = HAL_UART_Receive(huart, pstring, size, TIMEOUT + size * TIMEOUT_PER_CHAR);
    if (HAL_OK != ret && HAL_TIMEOUT != ret) {
        Handle_Error("HAL_UART_Receive not ok", (void*) ret);
    }
}
