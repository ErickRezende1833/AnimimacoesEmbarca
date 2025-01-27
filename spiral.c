#include "pico/stdlib.h"
#include "hardware/pio.h"
#include <stdlib.h>

#ifndef NUM_PIXELS
#define NUM_PIXELS 25
#endif

// variável que representa o valor padrão da intensidade dos LEDs da matriz
double intensity = 0.2;

// variável que define a cor RGB padrão dos LEDs
uint8_t r = 0, g = 0, b = 255;

// frames da animação espiral
uint32_t spiral_animation_frames[] = {
    0x0000000, 0x0001000, 0x0001800, 0x0001900, 0x0001980,
    0x00019C0, 0x00039C0, 0x00139C0, 0x00339C0, 0x00739C0,
    0x00F39C0, 0x00F3DC0, 0x00F3FC0, 0x00F3FC1, 0x00F3FC3,
    0x00F3FC7, 0x00F3FCF, 0x00F3FDF, 0x00F3FFF, 0x00F7FFF,
    0x00FFFFF, 0x10FFFFF, 0x18FFFFF, 0x1CFFFFF, 0x1EFFFFF,
    0x1FFFFFF};

uint32_t spiral_animation_clear[1] = {0x0000000};

 double *apply_intensity_frame(uint32_t frame, size_t total_frames, double intensity)
{
    double *frames = (double *)calloc(total_frames, sizeof(double));
    size_t counter = 0;

    for (size_t j = 0; j < NUM_PIXELS; j++)
    {
        int binary = (int)frame & (0x1 << ((NUM_PIXELS - j) - 1));
        // printf("%025b\t\t\t %d\n", binary, binary);

        if (binary > 0)
        {
            frames[counter] = 0.2;
        }
        else
        {
            frames[counter] = 0.0;
        }
        // printf("%1b\t %.1f\n", binary, frames[counter]);
        // printf("%.5f\n", frames[counter]);

        counter++;
    }
    free(frames);
    return frames;
}

 uint32_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
    // printf("%b\n", ((uint32_t)(r) << 16) | ((uint32_t)(g) << 24) | (uint32_t)(b << 8));
    return ((uint32_t)(r) << 16) |
           ((uint32_t)(g) << 24) |
           (uint32_t)(b << 8);
}

 void apply_color_frame(double *frame, PIO pio, uint sm, uint8_t r, uint8_t g, uint8_t b)
{
    for (size_t i = 0; i < NUM_PIXELS; ++i)
    {
        // printf("%.1f\n", frame[(NUM_PIXELS-1) - i]);
        uint32_t color = rgb(frame[(NUM_PIXELS - 1) - i] * r, frame[(NUM_PIXELS - 1) - i] * g, frame[(NUM_PIXELS - 1) - i] * b);
        pio_sm_put_blocking(pio, sm, color);
    }
}

 void spiral_animation(PIO pio, uint sm)
{
    size_t size_spiral_animation = sizeof(spiral_animation_frames) / sizeof(uint32_t);
    for (size_t i = 0; i < size_spiral_animation; i++)
    {
        apply_color_frame(apply_intensity_frame(spiral_animation_frames[i], size_spiral_animation, intensity), pio, sm, r, g, b);
        sleep_ms(100);
    }
    for (size_t i = (size_spiral_animation - 1); i > 0; i--)
    {
        apply_color_frame(apply_intensity_frame(spiral_animation_frames[i], size_spiral_animation, intensity), pio, sm, r, g, b);
        sleep_ms(100);
    }
    apply_color_frame(apply_intensity_frame(spiral_animation_clear[0], size_spiral_animation, intensity), pio, sm, r, g, b);
}