#include "./lib/button.h"

// Inicialização de um botão
void button_init(uint gpio){
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
    gpio_pull_up(gpio);
}

