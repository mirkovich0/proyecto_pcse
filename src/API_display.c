/*
 * API_Display.c
 *
 *  Created on: Aug 20, 2024
 *      Author: Mirko Serra
 */

#include "i2c.h"
#include "stm32f4xx_hal_i2c.h"

#include "API_display.h"

#include "API_delay.h"
#include "API_queue.h"

#include "error.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

extern I2C_HandleTypeDef hi2c1;

#define DATA_BUFFER_SIZE 512

#define DEV_ADDRESS 0x7e

#define LCD_PIN_RS   (1 << 0)
#define LCD_PIN_RW  (1 << 1)
#define LCD_PIN_E  (1 << 2)

#define LCD_PIN_DISP_ON (1 << 3)

#define LCD_PIN_DB4 (1 << 4)
#define LCD_PIN_DB5 (1 << 5)
#define LCD_PIN_DB6 (1 << 6)
#define LCD_PIN_DB7 (1 << 7)

#define LINES 2
#define COLS 16

static char disp_on;
static size_t line;

typedef enum {
    FIRST_STAGE,
    SET_HIGHER_DATA = FIRST_STAGE, RAISE_E, LOWER_E,
    SET_LOWER_DATA, RAISE_E_2, LOWER_E_2,
    LAST_STAGE
} stage_t;

static stage_t stage;
static queue_t* queue;
static delay_t process_timer;

static bool sendI2CChar(uint8_t chr)
{
    return HAL_OK == HAL_I2C_Master_Transmit(&hi2c1, DEV_ADDRESS, &chr, 1, 100000);
}

static void lcdCommand(uint8_t command)
{
    sendI2CChar((command << 4) | disp_on);
    sendI2CChar((command << 4) | LCD_PIN_E | disp_on);
    sendI2CChar((command << 4) | disp_on);
}

void display_Clear()
{
    lcdCommand(0x00);
    lcdCommand(0x01);
    HAL_Delay(10);
    display_SetLine(0);
}

static void set_Address(int new_address)
{
    int command = 0x80 | (new_address & 0x7f);
    lcdCommand(command >> 4);
    lcdCommand(command & 0x0f);
}

void display_Cursor(bool on, bool blinking)
{
    lcdCommand(0x00);
    lcdCommand(0x0C | (on? 0x02: 0x00) | (blinking? 0x01: 0x00));
    HAL_Delay(1);
}

void display_SetLine(int new_line)
{
    assert(line >= 0 && line <= LINES);
    while (!queue_Empty(queue)) {
        display_Process();
    }

    line = new_line;
    set_Address(0x40 * line);
}

void display_On(bool on)
{
    if ((disp_on && on) || (!disp_on && !on)) {
        return;
    }
    disp_on = on? LCD_PIN_DISP_ON : 0;
    display_Clear();
}

void Display_Reset()
{
    sendI2CChar(0x00);
    HAL_Delay(15);

    disp_on = LCD_PIN_DISP_ON;

    HAL_Delay(15);
    lcdCommand(0x03);  //000011
    HAL_Delay(5);
    lcdCommand(0x03);  //000011
    HAL_Delay(1);
    lcdCommand(0x03);  //000011
    HAL_Delay(1);
    lcdCommand(0x02);  //000010   Sets to 4-bit operation
    HAL_Delay(1);
    lcdCommand(0x02);
    lcdCommand(0x08);
    HAL_Delay(1);
    lcdCommand(0x00);
    lcdCommand(0x0c);
    HAL_Delay(1);
    lcdCommand(0x00);
    lcdCommand(0x06);
    HAL_Delay(1);
    display_Clear();
    display_Cursor(false, false);
    HAL_Delay(10);
    stage = FIRST_STAGE;
}

void display_Init()
{
    if (!queue) {
        queue = queue_New(DATA_BUFFER_SIZE);
    }

    Display_Reset();

    delayInit(&process_timer, 2);
}

// Data consumer
void display_Process()
{
    assert(queue);

    if (!delayRead(&process_timer)) {
        return;
    }
    if (queue_Empty(queue)) {
        return;
    }
    char data = queue_Peek(queue);

    switch (stage) {
    case SET_HIGHER_DATA:
        data = (data & 0xf0) | LCD_PIN_RS | disp_on;
        break;
    case RAISE_E:
        data = (data & 0xf0) | LCD_PIN_RS | disp_on | LCD_PIN_E;
        break;
    case LOWER_E:
        data = (data & 0xf0) | LCD_PIN_RS | disp_on;
        break;
    case SET_LOWER_DATA:
        data = (data << 4) | LCD_PIN_RS | disp_on;
        break;
    case RAISE_E_2:
        data = (data << 4) | LCD_PIN_RS | disp_on | LCD_PIN_E;
        break;
    case LOWER_E_2:
        data = (data << 4) | LCD_PIN_RS | disp_on;
        queue_RemoveOne(queue);
        break;
    case LAST_STAGE:
    default:
        Handle_Error("Must not get here", 0);
    }
    sendI2CChar(data);
    stage++;
    if (stage == LAST_STAGE) { // Skip this state
        stage = FIRST_STAGE;
    }
}

static void enqueueData(char data)
{
    while (queue_Full(queue)) {
        // No hay lugar, llamar al consumidor
        display_Process();
    }
    queue_Put(queue, data);
}

void display_Print(const char* fmt, ...)
{
    assert(queue);
    assert(fmt);

    char buffer[256];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);

    const char* s = buffer;
    while (*s) {
        if (*s >= ' ') {
            enqueueData(*s);
        }
        s++;
    }

    va_end (args);
}
