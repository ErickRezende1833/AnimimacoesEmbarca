#ifndef SPIRAL
#define SPIRAL

#include "hardware/pio.h"
#include "pico/stdlib.h"

/**
 * @brief Função que permite configurar a intensidade dos leds de um frame
 * @param frame Item da animação que se deseja aplicar a intensidade
 * @param total_frames Total de frames que contidos na animaçao
 * @param intensity Valor atribuido a intensidade de brilho do LED de 0.0 à 1.0
 */
double *apply_intensity_frame(uint32_t frame, size_t total_frames, double intensity);

/**
 * @brief Função que permite a conversão da cor, no format RGB, em um valor binário
 * @param r Valor referente a cor vermelha
 * @param g Valor referente a cor verde
 * @param b Valor referente a cor azul
 */
uint32_t rgb(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Função que permite aplicar uma determinada cor, no padrão RGB, ao led
 * @note O efeito luminoso produzido pela cor será influencido pela intensidade predefinida nos framee
 * @param r Valor referente a cor vermelha
 * @param g Valor referente a cor verde
 * @param b Valor referente a cor azul
 */
void apply_color_frame(double *desenho, PIO pio, uint sm, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Função responsável por aplicar os efeitos visuais aos frames e executar a animação da espiral
 * @note O efeito produzido pela cor será influencido pela intensidade predefinida no frame
 * @param pio Valor referente ao controlador de entra e saída PIO
 * @param sm Valor referente a máquina de estados
 */
void spiral_animation(PIO pio, uint sm);

#endif