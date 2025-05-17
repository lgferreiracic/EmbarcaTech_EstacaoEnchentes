#include "./lib/matrix.h"

//Cores 
const RGB RED = {0.1, 0, 0};
const RGB GREEN = {0, 0.1, 0};
const RGB YELLOW = {0.1, 0.1, 0};
const RGB BLACK = {0, 0, 0};

PIO pio; //Variável para armazenar a configuração da PIO
uint sm; //Variável para armazenar o estado da máquina

//rotina para inicialização da matrix de leds - ws2812b
uint matrix_init() {
   //Configurações da PIO
   pio = pio0; 
   uint offset = pio_add_program(pio, &pio_matrix_program);
   sm = pio_claim_unused_sm(pio, true);
   pio_matrix_program_init(pio, sm, offset, WS2812_PIN);
}

//rotina para definição da intensidade de cores do led
uint32_t matrix_rgb(double r, double g, double b){
  unsigned char R, G, B;
  R = r * 255;
  G = g * 255;
  B = b * 255;
  return (G << 24) | (R << 16) | (B << 8);
}

//rotina para acionar a matrix de leds - ws2812b
void set_leds(PIO pio, uint sm, double r, double g, double b) {
    uint32_t valor_led;
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        valor_led = matrix_rgb(r, g, b);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Função para converter a posição do matriz para uma posição do vetor.
int getIndex(int x, int y) {
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0) {
        return 24-(y * 5 + x); // Linha par (esquerda para direita).
    } else {
        return 24-(y * 5 + (4 - x)); // Linha ímpar (direita para esquerda).
    }
}

//rotina para acionar a matrix de leds - ws2812b
void draw_matrix(RGB pixels[NUM_PIXELS]) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        int x = i % 5;
        int y = i / 5;
        int index = getIndex(x, y);
        pio_sm_put_blocking(pio, sm, matrix_rgb(pixels[index].R, pixels[index].G, pixels[index].B));
    }
}

// Função para desenhar a matriz de LEDs com base no estado do semáforo
void draw_traffic_light_matrix(int traffic_light_state) {
    RGB pixels[NUM_PIXELS];
    switch(traffic_light_state){
        case 0:
            for (int i = 0; i < NUM_PIXELS; i++) {
                pixels[i] = GREEN;
            }
            break;
        case 1:
            for (int i = 0; i < NUM_PIXELS; i++) {
                pixels[i] = YELLOW;
            }
            break;
        case 2:
            for (int i = 0; i < NUM_PIXELS; i++) {
                pixels[i] = RED;
            }
            break;
        default:
            for (int i = 0; i < NUM_PIXELS; i++) {
                pixels[i] = BLACK;
            }
            break;
    }
    draw_matrix(pixels);
}

