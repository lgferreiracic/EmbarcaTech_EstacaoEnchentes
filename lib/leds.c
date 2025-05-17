#include "./lib/leds.h"

// Inicializa o LED no pino especificado
void led_init(uint led_pin) {
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
}

// Inicializa todos os LEDs
void led_init_all() {
    led_init(LED_GREEN_PIN);
    led_init(LED_BLUE_PIN);
    led_init(LED_RED_PIN);
}

// Função para piscar o LED vermelho
void led_rgb_red_color() {
    gpio_put(LED_GREEN_PIN, false);
    gpio_put(LED_BLUE_PIN, false);
    gpio_put(LED_RED_PIN, true);
}

// Função para piscar o LED verde
void led_rgb_green_color() {
    gpio_put(LED_GREEN_PIN, true);
    gpio_put(LED_BLUE_PIN, false);
    gpio_put(LED_RED_PIN, false);
}

// Função para piscar o LED em amarelo
void led_rgb_yellow_color() {
    gpio_put(LED_GREEN_PIN, true);
    gpio_put(LED_BLUE_PIN, false);
    gpio_put(LED_RED_PIN, true);
}

// Função para desligar os LEDs
void led_rgb_off() {
    gpio_put(LED_GREEN_PIN, false);
    gpio_put(LED_BLUE_PIN, false);
    gpio_put(LED_RED_PIN, false);
}