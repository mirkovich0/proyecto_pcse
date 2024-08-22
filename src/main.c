/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum
{
    IDLE,
    DESACTIVADO,
    IRRIGACION,
} estado_valvula_t;

typedef enum
{
    IDLE_TECLADO,
    MENSAJE,
    ENTRADA_TECLADO,
    PROCESAR_ENTRADA,
} estado_entrada_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define DURACION_MENSAJE 2000
#define TIEMPO_MEDICION 1000
#define TIEMPO_SIN_IRRIGAR 10000
#define MAXIMA_DURACION_IRRIGANDO 10000
#define MAXIMA_DURACION_SIN_TECLAS 5000

#define DESACTIVACION '*'
#define MEDICION '#'
#define ENTER  'D'
#define BACKSPACE 'A'

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* Hago overflow a 0 cuando la variable llega al límite dado */
#define INC_SATURADO(variable, limite) do { \
    (variable)++; \
    if ((variable) >= (limite)) { \
        (variable) = 0; \
    } \
} while (0)

/* Devuelvo el tamaño de un array */
#define DIM(x) (sizeof(x)/sizeof(x[0]))

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static const tick_t LED_TICKS = 100;
static estado_entrada_t estado_entrada;
static estado_valvula_t estado_valvula;

static int limite_humedad = 50;
static int humedad = 100;

static delay_t timer_entrada;
static delay_t timer_mensaje;
static delay_t timer_medicion;
static delay_t timer_irrigacion;
static delay_t timer_sin_irrigar;
static bool hay_mensaje;
static bool mensaje_mostrado = false;
static bool desactivado = false;
static bool mostrar_medicion = false;
static bool puede_irrigar = true;
static char mensaje[32];
static char entrada[2];
static int len_entrada;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void encender_Valvula()
{
    BSP_LED_On(LED2);
    uartSendString((const uint8_t*) "Enciendo valvula\r\n");
}

void apagar_Valvula()
{
    BSP_LED_Off(LED2);
    uartSendString((const uint8_t*) "Apago valvula\r\n");
}

void set_mensaje(const char* msg)
{
    strncpy(mensaje, msg, sizeof(mensaje)-1);
    delayWrite(&timer_mensaje, DURACION_MENSAJE);
    hay_mensaje = true;
}

void procesar_Valvula()
{
    switch (estado_valvula) {
    case IDLE:
        if (!puede_irrigar) {
            puede_irrigar = delayRead(&timer_sin_irrigar);
        }
        if (delayRead(&timer_medicion)) {
            if (sensor_HayValores()) {
                int nuevo_valor = sensor_ReadHumidity();
                if (nuevo_valor != humedad) {
                    humedad = nuevo_valor;
                    if (mostrar_medicion) {
                        uint8_t msg[32];
                        snprintf((char*) msg, sizeof(msg), "Medido: %d%%\r\n", humedad);
                        uartSendString(msg);
                    }
                }
            }
        }
        if (humedad < limite_humedad && puede_irrigar) {
            delayInit(&timer_irrigacion, MAXIMA_DURACION_IRRIGANDO);
            estado_valvula = IRRIGACION;
            encender_Valvula();
        }
        if (desactivado) {
            estado_valvula = DESACTIVADO;
        }
        break;
    case DESACTIVADO:
        if (!desactivado) {
            estado_valvula = IDLE;
        }
        break;
    case IRRIGACION:
        puede_irrigar = false;
        delayInit(&timer_sin_irrigar, TIEMPO_SIN_IRRIGAR);
        if (delayRead(&timer_medicion)) {
            humedad = sensor_HayValores() ? sensor_ReadHumidity() : 100;
        }
        if (desactivado) {
            estado_valvula = DESACTIVADO;
            apagar_Valvula();
        } else if (humedad > limite_humedad) {
            estado_valvula = IDLE;
            apagar_Valvula();
        } else if (delayRead(&timer_irrigacion)) {
            estado_valvula = IDLE;
            apagar_Valvula();
        }
        break;
    default:
        estado_valvula = IDLE;
    }
}

void procesar_Entrada()
{
    switch (estado_entrada)
    {
    case IDLE_TECLADO:
        if (hay_mensaje) {
            display_On(true);
            display_Clear();
            display_Print("%s", mensaje);
            display_Cursor(false, false);
            estado_entrada = MENSAJE;
            hay_mensaje = false;
        } else if (keyboard_KeyPressed()) {
            display_On(true);
            display_Clear();
            display_Print("Limite: %d%%", limite_humedad);
            display_SetLine(1);
            display_Print("Nuevo valor: ");
            display_Cursor(true, true);
            delayInit(&timer_entrada, MAXIMA_DURACION_SIN_TECLAS);
            estado_entrada = ENTRADA_TECLADO;
            len_entrada = 0;
        }
        break;
    case MENSAJE:
        mensaje_mostrado = true;
        if (keyboard_KeyPressed()) {
            keyboard_ReadKey();
            estado_entrada = IDLE_TECLADO;
            mensaje_mostrado = false;
            display_On(false);
        }
        if (delayRead(&timer_mensaje)) {
            estado_entrada = IDLE_TECLADO;
            mensaje_mostrado = false;
            display_On(false);
        }
        break;
    case ENTRADA_TECLADO:
        if (keyboard_KeyPressed()) {
            delayInit(&timer_entrada, MAXIMA_DURACION_SIN_TECLAS);

            int key = keyboard_ReadKey();

            if (key >= '0' && key <= '9') {
                if (len_entrada < sizeof(entrada)) {
                    entrada[len_entrada++] = key;
                }
                display_SetLine(1);
                display_Print("Nuevo valor: %2s%%", entrada);
            } else if (key == DESACTIVACION) {
                desactivado = !desactivado;
                if (desactivado) {
                    set_mensaje("Desactivando");
                } else {
                    set_mensaje("Activando");
                }
                len_entrada = 0;
                estado_entrada = IDLE_TECLADO;
                display_On(false);
            } else if (key == MEDICION) {
                mostrar_medicion = !mostrar_medicion;
                len_entrada = 0;
                estado_entrada = IDLE_TECLADO;
                display_On(false);
            } else if (key == BACKSPACE) {
                if (len_entrada > 0) {
                    len_entrada--;
                    entrada[len_entrada] = 0;
                }
                display_SetLine(1);
                display_Print("Nuevo valor: %2s%%", entrada);
            } else if (key == ENTER) {
                display_Cursor(false, false);
                if (len_entrada > 0) {
                    estado_entrada = PROCESAR_ENTRADA;
                } else {
                    len_entrada = 0;
                    estado_entrada = IDLE_TECLADO;
                    display_On(false);
                }
            }
        } else if (delayRead(&timer_entrada)) {
            estado_entrada = IDLE_TECLADO;
            display_On(false);
        }
        break;
    case PROCESAR_ENTRADA:
    {
        int valor = 0;
        for (int i = 0; i < len_entrada; i++) {
            valor *= 10;
            valor += entrada[i] - '0';
        }
        limite_humedad = valor;
        set_mensaje("Limite fijado");
        estado_entrada = IDLE_TECLADO;
        len_entrada = 0;
        display_On(false);
        break;
    }
    default:
        estado_entrada = IDLE_TECLADO;
    }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART3_UART_Init();
    MX_I2C1_Init();
    MX_I2C2_Init();
    /* USER CODE BEGIN 2 */

    /* Init code */
    BSP_LED_Init(LED1);
    BSP_LED_Init(LED2);
    BSP_LED_Init(LED3);
    BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

    assert(uartInit());

    delay_t led_delay;
    delayInit(&led_delay, LED_TICKS);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    uartSendString((const uint8_t*) "Iniciando programa\r\n");

    display_Init();
    keyboard_Init();
    sensor_Init();

    delayInit(&timer_medicion, TIEMPO_MEDICION);

    display_Clear();

    delay_t temp_delay;
    delayInit(&temp_delay, 2000);

    set_mensaje("Iniciado");

    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        display_Process();
        keyboard_Process();
        sensor_Process();

        procesar_Entrada();
        procesar_Valvula();

        // Led de indicación de vida
        if (delayRead(&led_delay)) {
            BSP_LED_Toggle(LED1);
        }
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                    | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
