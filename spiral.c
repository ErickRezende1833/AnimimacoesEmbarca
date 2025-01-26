

double intensity = 0.2;

// variável que define a cor RGB padrão dos LEDs
uint8_t r = 0, g = 0, b = 255;

// frames da animação espiral
uint32_t spiral_animation_frames[26] = {
    0x0000000, 0x0001000, 0x0001800, 0x0001900, 0x0001980,
    0x00019C0, 0x00039C0, 0x00139C0, 0x00339C0, 0x00739C0,
    0x00F39C0, 0x00F3DC0, 0x00F3FC0, 0x00F3FC1, 0x00F3FC3,
    0x00F3FC7, 0x00F3FCF, 0x00F3FDF, 0x00F3FFF, 0x00F7FFF,
    0x00FFFFF, 0x10FFFFF, 0x18FFFFF, 0x1CFFFFF, 0x1EFFFFF,
    0x1FFFFFF};

uint32_t spiral_animation_clear[1] = {0x0000000};