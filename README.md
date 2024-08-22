= Proyecto PCSE

En este repositorio se incluyen solamente los archivos generados por mí para el Proyecto.
Se excluyen todos los archivos generados por el IDE, así como todas las librerías de soporte.
Es así que si se quiere la version completa (que compila) del Proyecto, esta se encuentra en:

   https://github.com/mirkovich0/pdm/tree/main/Proyecto

= Módulos

== API DEBOUNCE

API_debounce.c y API_debounce.h: Una API para hacer debouncing del pulsador de usuario de la placa. Utiliza los GPIO. Tiene dependencia con la api de delay.

    void debounceFSM_init(callback_t buttonPressedCallback, callback_t buttonReleasedCallback);

Esta función debe ser llamada para inicializar este driver. Al ser presionado el botón se llamará a buttonPressedCallback
y al ser soltado se llamará a buttonReleasedCallback.


    void debounceFSM_update();

Esta función debe ser llamada en el bucle principal del programa.


    bool_t readKey();

Esta función provee un mecanismo alternativo. Devolverá false a no ser que se pulse un botón, entonces devolverá true
solamente una vez hasta que no se vuelva a pulsar.


== API DELAY

API_delay.c y API_delay.h: Manejo de demoras inteligentes. Utiliza HAL_GetTick()

    void delayInit(delay_t* delay, tick_t duration);

Inicialización de un delay. El timer expirará luego de duration ticks (en nuestro caso, 1ms).


    bool_t delayRead(delay_t* delay);

Devuelve true si el timer ha expirado y lo resetea si ha expirado. Sino, devuelve false


    void delayWrite(delay_t* delay, tick_t duration);

Cambia el tiempo de un timer.


    bool_t delayIsRunning(delay_t * delay);

Devuelve true si el timer está activo.


== API DISPLAY

API_display.c y API_display.h: Manejo del LCD. Utiliza el módulo I2C y tiene dependencias con la api de timer y de cola.

    void display_Init();

Inicializa el display. Debe ser llamada en main.


    void display_Clear();

Borra el display.


    void display_Cursor(bool on, bool blinking);

Cambia el tipo del cursor.


    void display_SetLine(int new_line);

Setea la línea en la que display_Print() va a trabajar.


    void display_On(bool on);

Enciende o apaga el display LCD usando el pin del PCF8574A.


    void display_Process();

Función de procesamiento. Debe ser llamada en el bucle principal. Maneja la máquina de estados del display y va enviando
los caracteres del buffer a la pantalla.


    void display_Print(const char* fmt, ...);

Encola para su impresión un mensaje, siendo una función estilo printf.


== API Keyboard

API_keyboard.c y API_keyboard.h: Manejo del teclado matricial 4x4. Utiliza el GPIO. Depende de la API de delay y queue.


    void keyboard_Init();

Inicializa el teclado. Debe llamarse al menos una vez en main.


    int keyboard_ReadKey();

Devuelve -1 si no hay teclas presionadas, sino devuelve la primera tecla de la cola del teclado. Devuelve teclas en el
espacio 0123456789ABCD*# como caracteres ASCII dependiendo de la presión del teclado matricial.


    bool keyboard_KeyPressed();

Devuelve true si hay alguna tecla en la cola del teclado.


    void keyboard_Process();

Función de actualización que debe llamarse desde el bucle principal. Maneja la máquina de estados del teclado y chequea los GPIO.


== API Queue

API_queue.c y API_queue.h: Cola circular sencilla. No tiene dependencias con nuestros drivers.


    queue_t* queue_New(size_t queue_size);

Devuelve un puntero a una nueva cola. Utiliza malloc.


    void queue_Put(queue_t* queue, uint8_t data);

Inserta un elemento en la cola. Asume que hay espacio en la cola, sino, falla.


    uint8_t queue_Peek(queue_t* queue);

Devuelve el primer elemento de la cola sin removerlo. Asume que chequeamos que la cola no está vacía.


    uint8_t queue_Get(queue_t* queue);

Devuelve el primer elemento de la cola y lo quita.  Asume que chequeamos que la cola no está vacía.


    void queue_RemoveOne(queue_t* queue);

Remueve el primer elemento de la cola.


    bool queue_Full(const queue_t* queue);

Retorna true si la cola está llena.


    bool queue_Empty(const queue_t* queue);

Retorna true si la cola está vacía.


== API Sensor

API_sensor.c y API_sensor.h: Manejo del sensor de temperatura y humedad. Utiliza I2C. Depende de delay.
La idea de la existencia de funciones int y float es principalmente usar matemática de enteros en las primeras.
De momento las funciones son equivalentes, solo que con un casteo.


    void sensor_Init();

Inicializa el sensor.


    bool sensor_HayValores();

Retorna true si se ha podido comunicar con el sensor y las funciones sensor_Read devolverán valores buenos.


    int sensor_ReadTemp();

Devuelve un int con el último valor de temperatura leído en Celcius.


    int sensor_ReadHumidity();

Devuelve un int con el último valor de humedad leído en % de humedad relativa.


    float sensor_ReadTempFloat();

Devuelve un float con el último valor de temperatura leído en Celcius.


    float sensor_ReadHumidityFloat();

Devuelve un float con el último valor de humedad leído en % de humedad relativa.


    void sensor_Process();

Función de procesamiento. Debe ser llamada en el bucle principal para que se actualicen los valores.


== API UART

API_uart.c y API_uart.h: Manejo del puerto serie. Utiliza la HAL de UART.


    bool_t uartInit();

Función de inicialización de la UART.


    void uartSendString(const uint8_t * pstring);

Envía un string terminado en NUL por la UART.


    void uartSendStringSize(const uint8_t * pstring, uint16_t size);

Envía un buffer de datos con una longitud dada por la UART.


    void uartReceiveStringSize(uint8_t * pstring, uint16_t size);

Recibe de la UART.
