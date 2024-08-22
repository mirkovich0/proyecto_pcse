/*
 * API_sensor.c
 *
 *  Created on: Aug 21, 2024
 *      Author: Mirko Serra
 */

#include "i2c.h"
#include "stm32f4xx_hal_i2c.h"

#include "API_sensor.h"

#include "API_delay.h"

#include <assert.h>

extern I2C_HandleTypeDef hi2c2;

static float last_temperature = 0.0f;
static float last_humidity = 0.0f;
static bool hay_valores = false;
static bool initted = false;

static delay_t delay;

#define DEV_ADDRESS (0x44 << 1)

static bool startMeasuring()
{
    uint8_t buf[2] = { 0x20, 0x32 };
    return HAL_OK == HAL_I2C_Master_Transmit(&hi2c2, DEV_ADDRESS, buf, sizeof(buf), 100000);
}

bool sensor_HayValores()
{
    return hay_valores;
}

static bool readMeasures()
{
    uint8_t buf[2] = { 0xE0, 0x00 };
    if (HAL_OK != HAL_I2C_Master_Transmit(&hi2c2, DEV_ADDRESS, buf, sizeof(buf), 100000)) {
        startMeasuring();
        return false;
    }
    uint8_t measurement[6];
    if (HAL_OK != HAL_I2C_Master_Receive(&hi2c2, DEV_ADDRESS, measurement, sizeof(measurement), 100000)) {
        return false;
    }
    int temp = (measurement[0] << 8) | measurement[1];
    int humidity = (measurement[3] << 8) | measurement[4];
    last_temperature = -45.0 + 175.0 * ((float)temp) / 65535.0;
    last_humidity = 100.0 * ((float)humidity) / 65535.0;
    hay_valores = true;

    return true;
}

void sensor_Init()
{
    initted = true;
    delayInit(&delay, 500);
    startMeasuring();
}

float sensor_ReadTempFloat()
{
    return last_temperature;
}

float sensor_ReadHumidityFloat()
{
    return last_humidity;
}

int sensor_ReadTemp()
{
    return last_temperature;
}

int sensor_ReadHumidity()
{
    return last_humidity;
}

void sensor_Process()
{
    assert(initted);
    if (!delayRead(&delay)) {
        return ;
    }
    readMeasures();
}
