/*
 * API_keyboard.c
 *
 *  Created on: Aug 21, 2024
 *      Author: Mirko Serra
 */

#include "API_keyboard.h"

#include "API_queue.h"
#include "API_delay.h"

#include "gpio.h"

static queue_t* queue;

#define KEYBOARD_QUEUE_SIZE (16)

#define ROWS (4)
#define COLS (4)

static delay_t delay;

void keyboard_Init()
{
    if (!queue) {
        queue = queue_New(KEYBOARD_QUEUE_SIZE);
    }
    delayInit(&delay, 1);
}

int keyboard_ReadKey()
{
    assert(queue);
    if (keyboard_KeyPressed()) {
        return queue_Get(queue);
    }
    return -1;
}

bool keyboard_KeyPressed()
{
    assert(queue);
    return !queue_Empty(queue);
}

static const char KEYMAP[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'},
};

typedef enum
{
    RELEASED,
    PRESSING,
    PRESSED,
    RELEASING,
} keystate_t;

static bool inputs[ROWS][COLS];
static keystate_t keys[ROWS][COLS];
static int cur_col;

static int readRow()
{
    bool row0 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2) == GPIO_PIN_RESET;
    bool row1 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == GPIO_PIN_RESET;
    bool row2 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5) == GPIO_PIN_RESET;
    bool row3 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6) == GPIO_PIN_RESET;
    int sum = ((int)row0) + ((int)row1) + ((int)row2) + ((int)row3);
    if (sum > 1) {
        return -1;
    } else if (row0) {
        return 0;
    } else if (row1) {
        return 1;
    } else if (row2) {
        return 2;
    } else if (row3) {
        return 3;
    }
    return -1;
}

static void setCol(int col)
{
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, (col == 3)? GPIO_PIN_RESET: GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, (col == 2)? GPIO_PIN_RESET: GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, (col == 1)? GPIO_PIN_RESET: GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, (col == 0)? GPIO_PIN_RESET: GPIO_PIN_SET);
}

void keyboard_Process()
{
    assert(queue);

    if (!delayRead(&delay)) {
        return;
    }

    // Read inputs
    int row = readRow();
    for (int i = 0; i < ROWS; i++) {
        inputs[i][cur_col] = false;
    }
    if (row >= 0 && row < ROWS) {
        inputs[row][cur_col] = true;
    }

    // Set state
    for (int i = 0; i < ROWS; i++) {
        switch(keys[i][cur_col]) {
        case RELEASED:
            if (inputs[i][cur_col]) {
                keys[i][cur_col] = PRESSING;
            }
            break;
        case PRESSING:
            if (inputs[i][cur_col]) {
                keys[i][cur_col] = PRESSED;
                queue_Put(queue, KEYMAP[i][cur_col]);
            } else {
                keys[i][cur_col] = RELEASED;
            }
            break;
        case PRESSED:
            if (!inputs[i][cur_col]) {
                keys[i][cur_col] = RELEASING;
            }
            break;
        case RELEASING:
            if (!inputs[i][cur_col]) {
                keys[i][cur_col] = RELEASED;
            } else {
                keys[i][cur_col] = PRESSED;
            }
            break;
        }
    }

    // Prepare next row
    cur_col++;
    if (cur_col >= COLS) {
        cur_col = 0;
    }
    setCol(cur_col);
}

