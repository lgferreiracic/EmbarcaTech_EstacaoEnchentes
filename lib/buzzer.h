#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define BUZZER_A_PIN 10 // Define o pino do buzzer A
#define BUZZER_B_PIN 21 // Define o pino do buzzer B

void buzzer_init(uint gpio); // Inicializa o buzzer
void buzzer_init_all(); // Inicializa todos os buzzers
void play_buzzers(); // Toca os buzzers
void stop_buzzers(); // Para os buzzers

#endif