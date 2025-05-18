#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "stdio.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define JOYSTICK_X_PIN 26 // Define o pino do eixo x do joystick
#define JOYSTICK_Y_PIN 27 // Define o pino do eixo y do joystick

typedef struct{
    uint16_t x_pos;
    uint16_t y_pos;
} joystick_data_t;

void joystick_init(); // Inicializa o joystick
uint16_t joystick_read_x(); // Lê o valor do eixo x do joystick
uint16_t joystick_read_y(); // Lê o valor do eixo y do joystick
joystick_data_t joystick_read(); // Lê os valores dos eixos x e y do joystick
uint16_t get_percentage(uint16_t value); // Converte o valor lido do joystick para porcentagem

#endif 