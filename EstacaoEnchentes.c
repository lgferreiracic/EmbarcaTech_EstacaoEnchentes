#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include <stdio.h>

#include "lib/ssd1306.h"
#include "lib/font.h"
#include "lib/matrix.h"
#include "lib/buzzer.h"
#include "lib/button.h"
#include "lib/leds.h"
#include "lib/joystick.h"

#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

ssd1306_t ssd; // Variável para o display OLED SSD1306
QueueHandle_t xQueueMatrixData;
QueueHandle_t xQueueLedData;
QueueHandle_t xQueueDisplayData;

void vJoystickTask(void *params){

}

void vLEDsTask(void *params){

}

void vDisplayTask(void *params){

}

void vMatrixTask(void *params){

}

void vBuzzerTask(void *params){

}

int main(){
    stdio_init_all();
    button_init(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    xQueueMatrixData = xQueueCreate(5, sizeof(joystick_data_t));
    xQueueLedData = xQueueCreate(5, sizeof(joystick_data_t));
    xQueueDisplayData = xQueueCreate(5, sizeof(joystick_data_t));

    xTaskCreate(vJoystickTask, "Joystick Task", 256, NULL, 1, NULL);
    xTaskCreate(vDisplayTask, "Display Task", 512, NULL, 1, NULL);
    xTaskCreate(vMatrixTask, "Matrix Task", 512, NULL, 1, NULL);
    xTaskCreate(vLEDsTask, "LED Task", 512, NULL, 1, NULL);
    xTaskCreate(vBuzzerTask, "Buzzer Task", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    panic_unsupported();
}
