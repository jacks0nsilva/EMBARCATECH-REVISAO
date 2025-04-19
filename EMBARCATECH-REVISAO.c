#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "libs/ssd1306.h"
#include "libs/definicoes.h"


ssd1306_t ssd;

// Variáveis para definir a posição inicial do eixo X e Y do quadrado
uint8_t square_x = 60; // Posição inicial do quadrado no eixo X
uint8_t square_y = 28; // Posição inicial do quadrado no eixo Y

int main()
{
    stdio_init_all();

    adc_init();
    adc_gpio_init(VRX_PIN); // Inicializa o pino do joystick no eixo X
    adc_gpio_init(VRY_PIN); // Inicializa o pino do joystick no eixo Y


    i2c_init(I2C_PORT, 400 * 1000); // Inicializa a comunicação I2C com uma taxa de 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Configura o pino SDA como I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Configura o pino SCL como I2C

    gpio_pull_up(I2C_SDA); // Habilita o pull-up na SDA
    gpio_pull_up(I2C_SCL); // Habilita o pull-up na SCL
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADRESS,I2C_PORT); // Inicializa o display OLED
    ssd1306_config(&ssd); // Configura o display OLED
    ssd1306_send_data(&ssd); // Envia os dados para o display OLED


    while (true) {
        printf("Hello, world!\n");

        adc_select_input(1);
        uint16_t vrx = adc_read(); // Lê o valor do joystick no eixo X (0 - 4095)
        sleep_us(10);

        adc_select_input(0);
        uint16_t vry = adc_read(); // Lê o valor do joystick no eixo Y (0 - 4095)
        sleep_us(10);

        // Define deslocamento baseado nos valores do joystick
        int8_t dx = 0, dy = 0;

        if (vrx < 1400) dx = -4; // Move para a esquerda
        if (vrx > 2700) dx = 4;  // Move para a direita
        if (vry < 1400) dy = 4; // Move para cima
        if (vry > 2700) dy = -4;  // Move para baixo

        // Atualiza a posição do quadrado, garantindo que fique dentro dos limites

        square_x = (square_x + dx >= 2 && square_x + dx <= 116) ? square_x + dx : square_x;
        square_y = (square_y + dy >= 2 && square_y + dy <= 52) ? square_y + dy : square_y;


        // Limpa o display antes de desenhar o novo quadro
        ssd1306_fill(&ssd, false);

        // Redesenha o retângulo externo
        ssd1306_rect(&ssd, 0, 0, 128, 64, true, false);
        ssd1306_rect(&ssd, square_y, square_x, 8, 8, true, true);

        ssd1306_send_data(&ssd);
        sleep_ms(50);
    }
}

