#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

#include "animacoes.pio.h"

#define NUM_PIXELS 25

#define OUT_PIN 7

// variável de intensidade padrão dos LEDs
double intensity = 0.2;

// variável que define a cor RGB padrão dos LEDs
uint8_t r = 0, g = 0, b = 255;

// keypad////////////////////////////////////////////
uint columns[4] = {6, 8, 9, 10};
uint rows[4] = {2, 3, 4, 5};

char KEY_MAP[16] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D'};

uint _columns[4];
uint _rows[4];
char _matrix_values[16];
uint all_columns_mask = 0x0;
uint column_mask[4];

void init_keypad(uint columns[4], uint rows[4], char matrix_values[16])
{
    for (int i = 0; i < 16; i++)
    {
        _matrix_values[i] = matrix_values[i];
    }

    for (int i = 0; i < 4; i++)
    {
        _columns[i] = columns[i];
        _rows[i] = rows[i];

        gpio_init(_columns[i]);
        gpio_init(_rows[i]);

        gpio_set_dir(_columns[i], GPIO_IN);
        gpio_set_dir(_rows[i], GPIO_OUT);

        gpio_put(_rows[i], 1);

        all_columns_mask = all_columns_mask + (1 << _columns[i]);
        column_mask[i] = 1 << _columns[i];
    }
}

char get_key(void)
{
    int row;
    uint32_t cols;
    bool pressed = false;

    cols = gpio_get_all();
    cols = cols & all_columns_mask;

    if (cols == 0x0)
    {
        return 0;
    }

    for (int j = 0; j < 4; j++)
    {
        gpio_put(_rows[j], 0);
    }

    for (row = 0; row < 4; row++)
    {
        gpio_put(_rows[row], 1);
        busy_wait_us(10000);
        cols = gpio_get_all();
        gpio_put(_rows[row], 0);
        cols = cols & all_columns_mask;
        if (cols != 0x0)
        {
            break;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        gpio_put(_rows[i], 1);
    }

    if (cols == column_mask[0])
    {
        return (char)_matrix_values[row * 4 + 0];
    }
    else if (cols == column_mask[1])
    {
        return (char)_matrix_values[row * 4 + 1];
    }
    if (cols == column_mask[2])
    {
        return (char)_matrix_values[row * 4 + 2];
    }
    else if (cols == column_mask[3])
    {
        return (char)_matrix_values[row * 4 + 3];
    }
    else
    {
        return 0;
    }
}

// def intensidade cores

uint32_t matrix_rgb(double b, double r, double g)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

////////////////////////////////////////

// init matrix ted

void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    for (int16_t i = 0; i < NUM_PIXELS; i++)
    {
        valor_led = matrix_rgb(b * desenho[24 - i], r * desenho[24 - i], g * desenho[24 - i]);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

/////////////////////////////////////////

// Criar função de cada desenho para ser chamada no switch-case em int main

void batimento_cardiaco()
{

    // desenho de 5 movimentos do batimento cardiaco

    double batimento1[NUM_PIXELS] = {
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 1.0, 0.0, 0.0,
        1.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0};

    double batimento2[NUM_PIXELS] = {
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0};

    double batimento3[NUM_PIXELS] = {
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0};

    double batimento4[NUM_PIXELS] = {
        0.0, 0.0, 1.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0};

    double batimento5[NUM_PIXELS] = {
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0};

    PIO pio = pio0;
    uint sm = 0;
    uint32_t valor_led;

    desenho_pio(batimento1, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);

    desenho_pio(batimento2, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);

    desenho_pio(batimento3, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);

    desenho_pio(batimento4, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);

    desenho_pio(batimento5, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);
}

void animacao_carinha()
{

    // Desenho de 5 carinhas diferentes

    double carinha1[NUM_PIXELS] = {
        0.0, 1.0, 1.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 1.0, 0.0, 1.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 1.0, 1.0, 0.0};

    double carinha2[NUM_PIXELS] = {
        0.0, 1.0, 1.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 1.0, 1.0, 0.0};

    double carinha3[NUM_PIXELS] = {
        0.0, 1.0, 1.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 1.0, 1.0, 0.0};

    double carinha4[NUM_PIXELS] = {
        0.0, 1.0, 1.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 1.0, 1.0, 0.0};

    double carinha5[NUM_PIXELS] = {
        0.0, 1.0, 1.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 1.0, 0.0, 1.0,
        1.0, 0.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0, 0.0};

    PIO pio = pio0;
    uint sm = 0;
    uint32_t valor_led;

    desenho_pio(carinha1, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);

    desenho_pio(carinha2, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);

    desenho_pio(carinha3, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);

    desenho_pio(carinha4, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);

    desenho_pio(carinha5, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);
}
void pulso()
{
    // Animação de pulso
    double pulso1[NUM_PIXELS] = {
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0};

    double pulso2[NUM_PIXELS] = {
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0, 0.0,
        0.0, 0.5, 1.0, 0.5, 0.0,
        0.0, 0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0};

    double pulso3[NUM_PIXELS] = {
        0.0, 0.0, 0.5, 0.0, 0.0,
        0.0, 0.5, 1.0, 0.5, 0.0,
        0.5, 1.0, 1.0, 1.0, 0.5,
        0.0, 0.5, 1.0, 0.5, 0.0,
        0.0, 0.0, 0.5, 0.0, 0.0};

    double pulso4[NUM_PIXELS] = {
        0.0, 0.5, 1.0, 0.5, 0.0,
        0.5, 1.0, 1.0, 1.0, 0.5,
        1.0, 1.0, 1.0, 1.0, 1.0,
        0.5, 1.0, 1.0, 1.0, 0.5,
        0.0, 0.5, 1.0, 0.5, 0.0};

    double pulso5[NUM_PIXELS] = {
        0.0, 0.0, 0.5, 0.0, 0.0,
        0.0, 0.5, 1.0, 0.5, 0.0,
        0.5, 1.0, 1.0, 1.0, 0.5,
        0.0, 0.5, 1.0, 0.5, 0.0,
        0.0, 0.0, 0.5, 0.0, 0.0};

    PIO pio = pio0;
    uint sm = 0;
    uint32_t valor_led;

    desenho_pio(pulso1, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);

    desenho_pio(pulso2, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);

    desenho_pio(pulso3, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);

    desenho_pio(pulso4, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);

    desenho_pio(pulso5, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);
}
void love()
{

    // desenho love

    double love1[NUM_PIXELS] =
        {1.0, 0.0, 0.0, 0.0, 0.0,
         1.0, 0.0, 0.0, 0.0, 0.0,
         1.0, 0.0, 0.0, 0.0, 0.0,
         1.0, 0.0, 0.0, 0.0, 0.0,
         1.0, 1.0, 1.0, 1.0, 1.0};

    double love2[NUM_PIXELS] =
        {0.0, 1.0, 1.0, 1.0, 0.0,
         1.0, 0.0, 0.0, 0.0, 1.0,
         1.0, 0.0, 0.0, 0.0, 1.0,
         1.0, 0.0, 0.0, 0.0, 1.0,
         0.0, 1.0, 1.0, 1.0, 0.0};

    double love3[NUM_PIXELS] =
        {1.0, 0.0, 0.0, 0.0, 1.0,
         1.0, 0.0, 0.0, 0.0, 1.0,
         1.0, 0.0, 0.0, 0.0, 1.0,
         0.0, 1.0, 0.0, 1.0, 0.0,
         0.0, 0.0, 1.0, 0.0, 0.0};

    double love4[NUM_PIXELS] =
        {1.0, 1.0, 1.0, 1.0, 1.0,
         1.0, 0.0, 0.0, 0.0, 0.0,
         1.0, 1.0, 1.0, 1.0, 1.0,
         1.0, 0.0, 0.0, 0.0, 0.0,
         1.0, 1.0, 1.0, 1.0, 1.0};

    double love5[NUM_PIXELS] =
        {0.0, 1.0, 0.0, 1.0, 0.0,
         1.0, 1.0, 1.0, 1.0, 1.0,
         1.0, 1.0, 1.0, 1.0, 1.0,
         0.0, 1.0, 1.0, 1.0, 0.0,
         0.0, 0.0, 1.0, 0.0, 0.0};

    PIO pio = pio0;
    uint sm = 0;
    uint32_t valor_led;

    desenho_pio(love1, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);

    desenho_pio(love2, valor_led, pio, sm, 0.0, 1.0, 0.0);
    sleep_ms(1000);

    desenho_pio(love3, valor_led, pio, sm, 0.0, 0.0, 1.0);
    sleep_ms(1000);

    desenho_pio(love4, valor_led, pio, sm, 0.0, 1.0, 0.0);
    sleep_ms(1000);

    desenho_pio(love5, valor_led, pio, sm, 1.0, 0.0, 0.0);
    sleep_ms(1000);
}

// frames da animação espiral
uint32_t spiral_animation_frames[26] = {
    0x0000000, 0x0001000, 0x0001800, 0x0001900, 0x0001980,
    0x00019C0, 0x00039C0, 0x00139C0, 0x00339C0, 0x00739C0,
    0x00F39C0, 0x00F3DC0, 0x00F3FC0, 0x00F3FC1, 0x00F3FC3,
    0x00F3FC7, 0x00F3FCF, 0x00F3FDF, 0x00F3FFF, 0x00F7FFF,
    0x00FFFFF, 0x10FFFFF, 0x18FFFFF, 0x1CFFFFF, 0x1EFFFFF,
    0x1FFFFFF};

// função para configurar a intensidade dos leds em cada frame
double *apply_intensity_frame_pio(uint32_t frame, size_t total_frames, double intensity)
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

// função para converter a cor RGB em binário
uint32_t uint_matrix_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    // printf("%b\n", ((uint32_t)(r) << 16) | ((uint32_t)(g) << 24) | (uint32_t)(b << 8));
    return ((uint32_t)(r) << 16) |
           ((uint32_t)(g) << 24) |
           (uint32_t)(b << 8);
}

// função para aplicar cor aos leds conforme valores RGBs definidos pelo usuário e a intensidade prefedenida no frame
void uint_desenho_pio(double *desenho, PIO pio, uint sm, uint8_t r, uint8_t g, uint8_t b)
{
    for (size_t i = 0; i < NUM_PIXELS; ++i)
    {
        // printf("%.1f\n", desenho[(NUM_PIXELS-1) - i]);
        uint32_t valor_led = uint_matrix_rgb(desenho[(NUM_PIXELS - 1) - i] * r, desenho[(NUM_PIXELS - 1) - i] * g, desenho[(NUM_PIXELS - 1) - i] * b);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// função da animação espiral
void spiral_animation()
{
    PIO pio = pio0;
    uint sm = 0;

    size_t size_spiral_animation = sizeof(spiral_animation_frames) / sizeof(uint32_t);
    for (size_t i = 0; i < size_spiral_animation; i++)
    {
        uint_desenho_pio(apply_intensity_frame_pio(spiral_animation_frames[i], size_spiral_animation, intensity), pio, sm, r, g, b);
        sleep_ms(100);
    }
    for (size_t i = (size_spiral_animation - 1); i > 0; i--)
    {
        uint_desenho_pio(apply_intensity_frame_pio(spiral_animation_frames[i], size_spiral_animation, intensity), pio, sm, r, g, b);
        sleep_ms(100);
    }
}

////////////////////////////////////////

int main()
{
    PIO pio = pio0;
    bool ok;
    uint32_t valor_led;

    ok = set_sys_clock_khz(128000, false);

    // Inicializa todos os códigos stdio
    stdio_init_all();

    printf("iniciando a transmissão PIO");
    if (ok)
        printf("clock set to %ld\n", clock_get_hz(clk_sys));

    // Configurações PIO, quem souber configurar é legal fazer

    uint offset = pio_add_program(pio, &animacoes_program);
    uint sm = pio_claim_unused_sm(pio, true);
    animacoes_program_init(pio, sm, offset, OUT_PIN);

    init_keypad(columns, rows, KEY_MAP);

    char caracter;

    while (true)
    {
        caracter = get_key();

        switch (caracter)
        {
        case 'A':
            /* code */
            break;
        case 'B':
            /* code */
            break;
        case 'C':
            /* code */
            break;
        case 'D':
            /* code */
            break;
        case '#':
            /* code */
            break;
        case '1':
            batimento_cardiaco();
            break;
        case '2':
            animacao_carinha();
            break;
        case '3':
            pulso();
            break;
        case '4':
            love();
        case '5':
            spiral_animation();
            break;
        default:
            break;
        }
    }
}
