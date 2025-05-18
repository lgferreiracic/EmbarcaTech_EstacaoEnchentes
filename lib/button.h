#ifndef BUTTON_H
#define BUTTON_H

#include "stdio.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"

#define BUTTON_A 5
#define BUTTON_B 6

void button_init(uint gpio); // Inicializa o botão

#endif