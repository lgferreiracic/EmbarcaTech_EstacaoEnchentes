#ifndef LEDS_H
#define LEDS_H

#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LED_GREEN_PIN 11 // Define o pino do LED verde
#define LED_BLUE_PIN 12 // Define o pino do LED azul
#define LED_RED_PIN 13 // Define o pino do LED vermelho

void led_init(uint led_pin); // Inicializa o LED
void led_init_all(); // Inicializa todos os LEDs
void led_rgb_red_color(); // Acende o LED vermelho
void led_rgb_green_color(); // Acende o LED verde
void led_rgb_yellow_color(); //Combinação de vermelho e verde
void led_rgb_off(); // Apaga todos os LEDs

#endif