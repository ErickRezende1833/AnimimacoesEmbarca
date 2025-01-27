#ifndef SPIRAL
#define SPIRAL

#include "hardware/pio.h"
#include "pico/stdlib.h" 
 
// função para configurar a intensidade dos leds em cada frame
double *apply_intensity_frame(uint32_t frame, size_t total_frames, double intensity);

// função para converter a cor RGB em binário
uint32_t rgb(uint8_t r, uint8_t g, uint8_t b);

// função para aplicar cor aos leds conforme parâmetros definidos pelo usuário e a intensidade predefinida no frame
void apply_color_frame(double *desenho, PIO pio, uint sm, uint8_t r, uint8_t g, uint8_t b);

// função da animação espiral
void spiral_animation(PIO pio, uint sm);

#endif