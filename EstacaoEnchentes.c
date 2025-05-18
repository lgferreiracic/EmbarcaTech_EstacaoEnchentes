#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include <stdio.h>
#include <stdlib.h>

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
    joystick_init();
    joystick_data_t data;
    while(true){
        data = joystick_read();
        xQueueSend(xQueueMatrixData, &data, 0);
        xQueueSend(xQueueLedData, &data, 0);
        xQueueSend(xQueueDisplayData, &data, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vLEDsTask(void *params){
    led_init_all();
    joystick_data_t data;
    while(true){
        if(xQueueReceive(xQueueLedData, &data, portMAX_DELAY)){
            uint16_t water_level = get_percentage(data.x_pos);
            uint16_t rain_level = get_percentage(data.y_pos);

            if(water_level >= 70 || rain_level >= 80){
                led_rgb_red_color();
            } else if (water_level >= 55 || rain_level >= 60){
                led_rgb_yellow_color();
            } else {
                led_rgb_green_color();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50)); 
    }
}

void vDisplayTask(void *params){
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADRESS, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    joystick_data_t data;
    char water_level_str[20];
    char rain_level_str[20];
    while(true){
        if(xQueueReceive(xQueueDisplayData, &data, portMAX_DELAY)){
            uint16_t water_level = get_percentage(data.x_pos);
            uint16_t rain_level = get_percentage(data.y_pos);
            sprintf(water_level_str, "Water Level:%d%%", water_level);
            sprintf(rain_level_str, "Rain Level:%d%%", rain_level);
            ssd1306_fill(&ssd, false);
            ssd1306_draw_string(&ssd, "Flood Station", 10, 15);
            ssd1306_draw_string(&ssd, water_level_str, 0, 25);
            ssd1306_draw_string(&ssd, rain_level_str, 0, 35);
            if(water_level >= 70 || rain_level >= 80){
                ssd1306_draw_string(&ssd, "DANGER", 35, 45);
            } else if (water_level >= 55 || rain_level >= 60){
                ssd1306_draw_string(&ssd, "WARNING", 30, 45);
            } else {
                ssd1306_draw_string(&ssd, "Safe", 40, 45);
            }
            ssd1306_send_data(&ssd);
        }
        vTaskDelay(pdMS_TO_TICKS(50)); 
    }   

}

void vMatrixTask(void *params){
    matrix_init();
    joystick_data_t data;
    while(true){
        if(xQueueReceive(xQueueMatrixData, &data, portMAX_DELAY)){
            uint16_t water_level = get_percentage(data.x_pos);
            uint16_t rain_level = get_percentage(data.y_pos);
            update_matrix(water_level, rain_level);
        }
        vTaskDelay(pdMS_TO_TICKS(50)); 
    }
}

void vBuzzerTask(void *params){
    buzzer_init_all();
    joystick_data_t data;
    while(true){
        if(xQueueReceive(xQueueMatrixData, &data, portMAX_DELAY)){
            uint16_t water_level = get_percentage(data.x_pos);
            uint16_t rain_level = get_percentage(data.y_pos);
            if(water_level >= 70 || rain_level >= 80){
                for(int i = 0; i < 8; i++){
                    play_buzzers();
                    vTaskDelay(pdMS_TO_TICKS(50));
                    stop_buzzers();
                    vTaskDelay(pdMS_TO_TICKS(50));
                };
                vTaskDelay(pdMS_TO_TICKS(500));
            } else {
                stop_buzzers();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50)); 
    }
}

void gpio_irq_handler(uint gpio, uint32_t events){
    clear_matrix();
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    reset_usb_boot(0, 0);
}

int main(){
    stdio_init_all();
    button_init(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    srand(to_ms_since_boot(get_absolute_time()));

    xQueueMatrixData = xQueueCreate(5, sizeof(joystick_data_t));
    xQueueLedData = xQueueCreate(5, sizeof(joystick_data_t));
    xQueueDisplayData = xQueueCreate(5, sizeof(joystick_data_t));

    xTaskCreate(vJoystickTask, "Joystick Task", 256, NULL, 1, NULL);
    xTaskCreate(vDisplayTask, "Display Task", 512, NULL, 1, NULL);
    xTaskCreate(vMatrixTask, "Matrix Task", 512, NULL, 1, NULL);
    xTaskCreate(vLEDsTask, "LED Task", 256, NULL, 1, NULL);
    xTaskCreate(vBuzzerTask, "Buzzer Task", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    panic_unsupported();
}
