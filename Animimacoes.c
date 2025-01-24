#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

#define NUM_PIXELS 25

#define OUT_PIN 7


//keypad////////////////////////////////////////////
uint columns[4] = {6, 7, 8, 9}; 
uint rows[4] = {2, 3, 4, 5};

char KEY_MAP[16] = {
    '1', '2' , '3', 'A',
    '4', '5' , '6', 'B',
    '7', '8' , '9', 'C',
    '*', '0' , '#', 'D'
};

uint _columns[4];
uint _rows[4];
char _matrix_values[16];
uint all_columns_mask = 0x0;
uint column_mask[4];

void init_keypad(uint columns[4], uint rows[4], char matrix_values[16]) {
    for (int i = 0; i < 16; i++) {
        _matrix_values[i] = matrix_values[i];
    }

    for (int i = 0; i < 4; i++) {
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

char get_key(void) {
    int row;
    uint32_t cols;
    bool pressed = false;

    cols = gpio_get_all();
    cols = cols & all_columns_mask;
    imprimir_binario(cols);

    if (cols == 0x0) {
        return 0;
    }

    for (int j = 0; j < 4; j++) {
        gpio_put(_rows[j], 0);
    }

    for (row = 0; row < 4; row++) {
        gpio_put(_rows[row], 1);
        busy_wait_us(10000);
        cols = gpio_get_all();
        gpio_put(_rows[row], 0);
        cols = cols & all_columns_mask;
        if (cols != 0x0) {
            break;
        }    
    }

    for (int i = 0; i < 4; i++) {
        gpio_put(_rows[i], 1);
    }

    if (cols == column_mask[0]) {
        return (char)_matrix_values[row * 4 + 0];
    }
    else if (cols == column_mask[1]) {
        return (char)_matrix_values[row * 4 + 1];
    }
    if (cols == column_mask[2]) {
        return (char)_matrix_values[row * 4 + 2];
    }
    else if (cols == column_mask[3]) {
        return (char)_matrix_values[row * 4 + 3];
    }
    else {
        return 0;
    }
}
/////////////////////////////////////////

//desenhos


////////////////////////////////////////

void imprimir_binario(int num) {
    int i;
    for (i = 31; i >= 0; i--) {
    (num & (1 << i)) ? printf("1") : printf("0");
    }
}


//def intensidade cores

uint32_t matrix_rgb(double b, double r, double g)
{
 unsigned char R, G, B;
 R = r * 255;
 G = g * 255;
 B = b * 255;
 return (G << 24) | (R << 16) | (B << 8);
}

////////////////////////////////////////

//init matrix ted

void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b){
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        if (desenho[24-i] > 0) {
            valor_led = matrix_rgb(b, r, g);
        } else {
            valor_led = matrix_rgb(0.0, 0.0, 0.0);
        }
        pio_sm_put_blocking(pio, sm, valor_led);

    }
    imprimir_binario(valor_led);
}

////////////////////////////////////////

int main()
{
    stdio_init_all();
    
    init_keypad(columns, rows, KEY_MAP);

    char caracter;

    while (true) {
            caracter = get_key();

            /*switch (caracter = "A")
            {
            case:
                
                break;
            
            default:
                break;
            }*/
    }
}
