= Proyecto PCSE

En este repositorio se incluyen solamente los archivos generados por mí para el Proyecto.
Se excluyen todos los archivos generados por el IDE, así como todas las librerías de soporte.
Es así que si se quiere la version completa (que compila) del Proyecto, esta se encuentra en:

   https://github.com/mirkovich0/pdm/tree/main/Proyecto

= Módulos

== API DEBOUNCE

API_debounce.c y API_debounce.h: Una API para hacer debouncing del pulsador de usuario de la placa. Utiliza los GPIO.

    void debounceFSM_init(callback_t buttonPressedCallback, callback_t buttonReleasedCallback);

Esta función debe ser llamada para inicializar este driver. Al ser presionado el botón se llamará a buttonPressedCallback
y al ser soltado se llamará a buttonReleasedCallback.


    void debounceFSM_update();

Esta función debe ser llamada en el bucle principal del programa.


    bool_t readKey();

Esta función provee 


== API DELAY

API_delay.c y API_delay.h: Manejo de demoras inteligentes. Utiliza HAL_GetTick()

== API DISPLAY

API_display.c y API_display.h: Manejo del LCD. Utiliza el módulo I2C.

== API Keyboard

API_keyboard.c y API_keyboard.h: Manejo del teclado matricial 4x4. Utiliza el GPIO.

== API Queue

API_queue.c y API_queue.h: Cola circular sencilla. No tiene dependencias.

== API Sensor

API_sensor.c y API_sensor.h: Manejo del sensor de temperatura y humedad. Utiliza I2C.

== API UART

API_uart.c y API_uart.h: Manejo del puerto serie. Utiliza la HAL de UART.
