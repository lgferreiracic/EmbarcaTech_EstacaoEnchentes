#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define NUM_PIXELS 25
#define WS2812_PIN 7

#define RAIN 2
#define WATER 1
#define EMPTY 0

// Definição da estrutura RGB para representar as cores
typedef struct {
    double R; // Intensidade da cor vermelha
    double G; // Intensidade da cor verde
    double B; // Intensidade da cor azul
} RGB;

//Cores 
extern const RGB BLUE; // Azul
extern const RGB CYAN; // Ciano
extern const RGB BLACK; // Preto

uint matrix_init(); // Inicializa a matriz de LEDs RGB
uint32_t matrix_rgb(double r, double g, double b); // Função para converter as intensidades de cor para um valor RGB
void set_leds(PIO pio, uint sm, double r, double g, double b); // Função para definir as cores dos LEDs
int getIndex(int x, int y); // Função para obter o índice do LED RGB
void draw_matrix(RGB pixels[NUM_PIXELS]); // Função para desenhar a matriz de LEDs RGB
void generate_rain_drops(uint16_t rain_level);
void move_rain_drops(); // Função para mover as gotas de chuva
void set_water_level(uint16_t level); // Função para definir o nível da água
void draw_rain_and_water_levels(); // Função para desenhar os níveis de água e chuva na matriz
void update_matrix(uint16_t water_level, uint16_t rain_level);
void clear_matrix(); // Função para limpar a matriz de LEDs RGB

#endif