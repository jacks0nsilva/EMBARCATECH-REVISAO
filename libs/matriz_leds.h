#ifndef MATRIZ_LEDS_H
#define MATRIZ_LEDS_H

// Buffer para armazenar os LEDS que estão ativos na matriz 5x5
bool matriz [10][25] = {{
    0, 0, 1, 0, 0, 
    0, 1, 0, 1, 0, 
    1, 1, 1, 1, 1, // Desenha o número <-
    0, 1, 0, 1, 0, 
    0, 0, 1, 0, 0
},
{
    0, 0, 1, 0, 0, 
    0, 0, 0, 1, 0, 
    1, 1, 1, 1, 1, //Desenha o número seta ->
    0, 0, 0, 1, 0, 
    0, 0, 1, 0, 0
}
};
#endif
