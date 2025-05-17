#include "./lib/buzzer.h"

uint16_t duty;

// Inicializa o buzzer no pino especificado
void buzzer_init(uint gpio){
        gpio_set_function(gpio, GPIO_FUNC_PWM);
        uint buzzer_slice = pwm_gpio_to_slice_num(gpio);
        uint32_t wrap = 125000;     
        pwm_set_wrap(buzzer_slice, wrap);
        pwm_set_clkdiv(buzzer_slice, 1.0f); 
        duty = 0.05 * wrap;
        pwm_set_gpio_level(gpio, 0);
        pwm_set_enabled(buzzer_slice, true);
}

// Inicializa todos os buzzers
void buzzer_init_all(){
    buzzer_init(BUZZER_A_PIN);
    buzzer_init(BUZZER_B_PIN);
}

// Função para tocar os buzzers 
void play_buzzers() {
    pwm_set_gpio_level(BUZZER_A_PIN, duty);  
    pwm_set_gpio_level(BUZZER_B_PIN, duty);                        
}

// Função para parar os buzzers
void stop_buzzers() {
    pwm_set_gpio_level(BUZZER_A_PIN, 0);  
    pwm_set_gpio_level(BUZZER_B_PIN, 0);                        
}
