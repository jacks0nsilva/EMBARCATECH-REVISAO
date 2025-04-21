#ifndef DEFINICOES_H
#define DEFINICOES_H

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ADRESS 0x3C
#define VRX_PIN 27 // Pino do eixo X do  joystick
#define VRY_PIN 26 // Pino do eixo Y do joystick
#define SW_PIN 22 // Pino do botão do joystick
#define BUTTON_A 5 
#define BUTTON_B 6
#define LED_GREEN 11 // Pino do LED verde
#define LED_BLUE 12 // Pino do LED azul
#define LED_RED 13 /// Pino do LED vermelho
#define BUZZER_A 21 // Pino do 
#define MATRIZ_LEDS 7 // Pino da matriz de LEDs

typedef enum {
    WAITING_START,
    COUNTDOWN,
    WAIT_FOR_REACTION,
    SHOW_RESULT
} GameState;

#endif