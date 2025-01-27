#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "spiral.h"

#include "animacoes.pio.h"

#define NUM_PIXELS 25

#define OUT_PIN 7
  
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

// função que realiza o reboot do dispositivo em modo BOOTSEL
void do_bootsel()
{
    printf("Entrando em modo bootsel...");
    rom_reset_usb_boot_extra(-1, 0, false);
}

// matriz padrão para usar nos casos A, B, C, D e #
double padrao[NUM_PIXELS] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0};

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

    while (true)
    {
        do
        {
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
        } while (get_key() == '1');

        break;
    }
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
            desenho_pio(padrao, valor_led, pio, sm, 0.0, 0.0, 0.0); // Desliga todos os leds
            break;
        case 'B':
            desenho_pio(padrao, valor_led, pio, sm, 0.0, 0.0, 1.0); // LED azul em 100%
            break;
        case 'C':
            desenho_pio(padrao, valor_led, pio, sm, 0.8, 0.0, 0.0); // LED vermelho em 80%
            break;
        case 'D':
            desenho_pio(padrao, valor_led, pio, sm, 0.0, 0.5, 0.0); // LED verde em 50%
            break;
        case '#':
            desenho_pio(padrao, valor_led, pio, sm, 0.2, 0.2, 0.2); // LEDs brancos em 20%
            break;
        case '*':
            do_bootsel();
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
            break;
        case '5':
            spiral_animation(pio, sm);
            break;
        default:
            break;
        }
    }

    // remove o animacoes_program e libera a máquina de estados quando não estiver mais em uso
    pio_remove_program_and_unclaim_sm(&animacoes_program, pio, sm, offset);
}
