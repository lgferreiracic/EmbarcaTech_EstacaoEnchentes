#include "./lib/matrix.h"

//Cores 
const RGB BLUE = {0, 0, 0.1};
const RGB CYAN = {0, 0.1, 0.1};
const RGB BLACK = {0, 0, 0};

uint8_t matrix[5][5] = {0};
uint16_t water_level = 0;
uint16_t rain_level = 0;

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
  R = r * 100;
  G = g * 100;
  B = b * 100;
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

void generate_rain_drops(uint16_t rain_level) {
    if(rand() % 100 > rain_level)
        return;
    uint y = rand() % 5;
    matrix[0][y] = RAIN;
}

// Função para mover as gotas para baixo
void move_rain_drops() {
    for (int y = 0; y < 5; y++) {
        if (matrix[4 - water_level][y] == RAIN) {
            matrix[4 - water_level][y] = EMPTY;
        }
    }
    for (int x = 5 - 2; x >= 0; x--) {
        for (int y = 0; y < 5; y++) {
            if (matrix[x][y] == RAIN) {
                matrix[x][y] = EMPTY;
                matrix[x + 1][y] = RAIN;
            }
        }
    }
}

// Função para definir o nível de água
void set_water_level(uint16_t level) {
    water_level = level;
    for(int y = 0; y < 5; y++){
        if(matrix[2][y] == WATER)
            matrix[2][y] = EMPTY;
        if(matrix[3][y] == WATER)
            matrix[3][y] = EMPTY;
        matrix[4][y] = EMPTY;
    }

    if(water_level >= 70) {
        for(int y = 0; y < 5; y++){
            matrix[2][y] = WATER;
            matrix[3][y] = WATER;
            matrix[4][y] = WATER;
        }
    }else if(water_level >= 55){
        for(int y = 0; y < 5; y++){
            matrix[3][y] = WATER;
            matrix[4][y] = WATER;
        }
    }
    else{
        for(int y = 0; y < 5; y++)
            matrix[4][y] = WATER;
    }
}

void draw_rain_and_water_levels() {
    RGB pixels[NUM_PIXELS];

    for (int i = 0; i < NUM_PIXELS; i++) {
        int x = i % 5;
        int y = i / 5;

        if (matrix[y][x] == WATER) {
            pixels[i] = BLUE;
        } else if (matrix[y][x] == RAIN) {
            pixels[i] = CYAN;
        } else {
            pixels[i] = BLACK;
        }
    }
    draw_matrix(pixels);
}

void update_matrix(uint16_t water_level, uint16_t rain_level) {
    move_rain_drops();
    generate_rain_drops(rain_level);
    set_water_level(water_level);
    draw_rain_and_water_levels();
}

void clear_matrix() {
    RGB pixels[NUM_PIXELS];
    for (int i = 0; i < NUM_PIXELS; i++) {
        pixels[i] = BLACK;
    }
    draw_matrix(pixels);
}