#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>
#include <stdio.h>
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define NUM_PIXELS 25
#define WS2812_PIN 7

// Definição da estrutura RGB para representar as cores
typedef struct {
    double R; // Intensidade da cor vermelha
    double G; // Intensidade da cor verde
    double B; // Intensidade da cor azul
} RGB;

//Cores 
extern const RGB RED; // Vermelho
extern const RGB GREEN; // Verde
extern const RGB YELLOW; // Amarelo
extern const RGB BLACK; // Preto

uint matrix_init(); // Inicializa a matriz de LEDs RGB
uint32_t matrix_rgb(double r, double g, double b); // Função para converter as intensidades de cor para um valor RGB
void set_leds(PIO pio, uint sm, double r, double g, double b); // Função para definir as cores dos LEDs
int getIndex(int x, int y); // Função para obter o índice do LED RGB
void draw_matrix(RGB pixels[NUM_PIXELS]); // Função para desenhar a matriz de LEDs RGB
void draw_traffic_light_matrix(int traffic_light_state); // Função para desenhar o semáforo na matriz de LEDs RGB

#endif