#include "joystick.h"

// Função para inicialização do joystick
void joystick_init(){
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
}

// Função para leitura do eixo x do joystick
uint16_t joystick_read_x(){
    adc_select_input(1);
    return adc_read();
}

// Função para leitura do eixo y do joystick
uint16_t joystick_read_y(){
    adc_select_input(0);
    return adc_read();
}

// Função para leitura do joystick, verificando se o joystick está posicionado para cima, baixo, esquerda, direita ou no centro.
joystick_data_t joystick_read(){
    joystick_data_t data;
    data.x_pos = joystick_read_x();
    data.y_pos = joystick_read_y();
    return data;
}

// Função para converter o valor lido do joystick para porcentagem
uint16_t get_percentage(uint16_t value){
    return (value * 100) / 4095;
}