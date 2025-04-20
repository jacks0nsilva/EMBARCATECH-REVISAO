#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "libs/ssd1306.h"
#include "libs/definicoes.h"
#include "libs/pio_config.h"

static volatile uint32_t last_time = 0; // Variável para armazenar o tempo da última interrupção
static volatile bool game_is_active = false; // Estado do jogo
static volatile int player_one_score = 0; // Pontuação do jogador 1
static volatile int player_two_score = 0; // Pontuação do jogador 2
static volatile GameState current_state = WAITING_START; // Estado atual do jogo
absolute_time_t countdown_start_time; // Tempo de início da contagem
absolute_time_t result_display_start; // Tempo de início da exibição do resultado
bool reaction_received = false; // Indica se a reação foi recebida
uint8_t winner = 0; // Jogador vencedor
bool game_over = false; // Indica se o jogo acabou
bool displayed = false; // Indica se o resultado foi exibido
ssd1306_t ssd; // Estrutura para o display OLED

// Posição do quadrado 
uint8_t square_x = 60;
uint8_t square_y = 28;

// Declarações de funções 
void initialize_gpio(int pin, bool direction); // Inicialização do GPIO
void move_square(); // Movimento do quadrado via joystick
void start_countdown(); // Início da contagem regressiva
static void gpio_irq_handler(uint gpio, uint32_t events); // Manipula interrupção GPIO
void game(); // Função principal do jogo
void set_rgb(bool r, bool g, bool b); // Configura LEDs RGB
void pwm_init_buzzer(uint pin); // Inicializa o buzzer
void beep(uint pin, uint duration_ms); // Emissão de sinal PWM (beep)
void update_matrix_color(); // Atualiza a cor da matriz de LEDs de acordo com o joystick

int main() {
    stdio_init_all();

    adc_init();
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);

    initialize_gpio(SW_PIN, GPIO_IN);
    initialize_gpio(BUTTON_A, GPIO_IN);
    initialize_gpio(BUTTON_B, GPIO_IN);
    initialize_gpio(LED_GREEN, GPIO_OUT);
    initialize_gpio(LED_BLUE, GPIO_OUT);
    initialize_gpio(LED_RED, GPIO_OUT);
    pwm_init_buzzer(BUZZER_A);

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    gpio_set_irq_enabled_with_callback(SW_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    np_init(MATRIZ_LEDS);
    np_clear();

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADRESS, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    while (true) {
        if (!game_is_active) {
            move_square();
            update_matrix_color();
        } else {
            np_clear();
            game();
        }
    }
}

// Função para tratar as interrupções do GPIO
static void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (current_time - last_time > 900000) {
        last_time = current_time;

        // Reinício da partida após vitória
        if (current_state == SHOW_RESULT && game_over && gpio == SW_PIN) {
            player_one_score = 0;
            player_two_score = 0;
            game_over = false;
            winner = 0;
            reaction_received = false;
            current_state = WAITING_START;
            game_is_active = true;
            start_countdown();
            return;
        }

        // Botão do joystick inicia o jogo
        if (gpio == SW_PIN && !game_over) {
            game_is_active = !game_is_active;
            if (game_is_active) {
                start_countdown();
            } else {
                current_state = WAITING_START;
            }
        }

        // Falsa largada durante a contagem
        if (current_state == COUNTDOWN) {
            set_rgb(true, false, false); // Vermelho
            if (gpio == BUTTON_A) {
                player_two_score++;
                winner = 2;
                current_state = SHOW_RESULT;
            } else if (gpio == BUTTON_B) {
                player_one_score++;
                winner = 1;
                current_state = SHOW_RESULT;
            }
        }

        // Reação após o sinal
        else if (current_state == WAIT_FOR_REACTION && !reaction_received) {
            if (gpio == BUTTON_A) {
                player_one_score++;
                winner = 1;
                reaction_received = true;
                current_state = SHOW_RESULT;
            } else if (gpio == BUTTON_B) {
                player_two_score++;
                winner = 2;
                reaction_received = true;
                current_state = SHOW_RESULT;
            }
        }
    }
}

// Função principal do jogo
void game(){
    absolute_time_t now = get_absolute_time();

    switch (current_state) {
        case COUNTDOWN: {
            int elapsed = to_ms_since_boot(now) - to_ms_since_boot(countdown_start_time);
            int remaining = 5000 - elapsed;
            set_rgb(false, false, true); // Azul

            ssd1306_fill(&ssd, false);
            if (remaining > 0) {
                char msg[20];
                sprintf(msg, "INICIANDO EM %d", remaining / 1000 + 1);
                printf("INICIANDO EM %d\n", remaining / 1000 + 1);
                ssd1306_draw_string(&ssd, msg, 10, 25);
            } else {
                current_state = WAIT_FOR_REACTION;
                ssd1306_draw_string(&ssd, "VAI", 10, 25);
            }
            ssd1306_send_data(&ssd);
            sleep_ms(100);
            break;
        }

        case WAIT_FOR_REACTION:
            set_rgb(false, true, false); // Verde
            sleep_ms(10); // Aguarda o botão via interrupção
            break;

        case SHOW_RESULT: {
            if (!displayed) {
                ssd1306_fill(&ssd, false);
                if (player_one_score == 3 || player_two_score == 3) {
                    set_rgb(true, true, true); // Branco (vencedor)
                    game_over = true;
                    displayed = true;
                    char win_msg[32];
                    sprintf(win_msg, "J%d VENCEU", (player_one_score == 3) ? 1 : 2);
                    printf("J%d VENCEU\n", (player_one_score == 3) ? 1 : 2);

                    ssd1306_draw_string(&ssd, win_msg, 2, 10);
                    ssd1306_draw_string(&ssd, "PRESS O JOYSTICK PARA REINICIAR", 2, 30);
                    printf("PRESSIONE O JOYSTICK PARA REINICIAR\n");
                } else {
                    beep(BUZZER_A, 400);
                    char msg[32];
                    sprintf(msg, "PONTO DO J%d", winner);
                    printf("PONTO DO JOGADOR %d\n", winner);

                    ssd1306_draw_string(&ssd, msg, 2, 10);

                    char score[32];
                    sprintf(score, "J1: %d  J2: %d", player_one_score, player_two_score);
                    ssd1306_draw_string(&ssd, score, 2, 30);
                    printf("JOGADOR 1: %d  JOGADOR 2: %d\n", player_one_score, player_two_score);

                    ssd1306_draw_string(&ssd, "PROX RODADA", 2, 50);
                }

                ssd1306_send_data(&ssd);
                result_display_start = get_absolute_time();
                displayed = true;
            }

            if (!game_over && absolute_time_diff_us(result_display_start, get_absolute_time()) > 3000000) {
                displayed = false;
                start_countdown();
            }
            break;
        }

        case WAITING_START:
            sleep_ms(10);
            break;
    }
}

// Movimento do quadrado via joystick 
void move_square() {

    set_rgb(false, false, false);
    adc_select_input(1);
    uint16_t vrx = adc_read();
    sleep_us(10);

    adc_select_input(0);
    uint16_t vry = adc_read();
    sleep_us(10);

    int8_t dx = 0, dy = 0;

    if (vrx < 1400) dx = -4;
    if (vrx > 2700) dx = 4;
    if (vry < 1400) dy = 4;
    if (vry > 2700) dy = -4;

    square_x = (square_x + dx >= 2 && square_x + dx <= 116) ? square_x + dx : square_x;
    square_y = (square_y + dy >= 2 && square_y + dy <= 52) ? square_y + dy : square_y;

    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 0, 0, 128, 64, true, false);
    ssd1306_rect(&ssd, square_y, square_x, 8, 8, true, true);
    ssd1306_send_data(&ssd);
}

// Função para configurar a cor do LED RGB
void set_rgb(bool r, bool g, bool b) {
    gpio_put(LED_RED, r);
    gpio_put(LED_GREEN, g);
    gpio_put(LED_BLUE, b);
}

// Função para inicializar o buzzer PWM
void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (5000 * 4096));  // Frequência do PWM
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0);  // Inicializa com o PWM desligado
}

// Função para emitir o sinal PWM (beep) com duração especificada
void beep(uint pin, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_gpio_level(pin, 2048);  // Configura o duty cycle para 50% (ativo)
    sleep_ms(duration_ms);         // Duração do beep
    pwm_set_gpio_level(pin, 0);    // Desativa o PWM
}

// Função para atualizar a matriz de LEDs com base nos valores do joystick
void update_matrix_color() {
    adc_select_input(1);
    uint16_t vrx = adc_read();
    sleep_us(10);

    adc_select_input(0);
    uint16_t vry = adc_read();
    sleep_us(10);

    // Converte valores do joystick para faixas de cor RGB (0 a 255)
    uint8_t r = (vrx * 20) / 4095;
    uint8_t g = (vry * 20) / 4095;
    uint8_t b = 20 - ((vrx + vry) / 2 * 20 / 4095);

    bool buffer[25];
    for (int i = 0; i < 25; i++) buffer[i] = true; // todos os LEDs acesos

    np_set_leds(buffer, r, g, b);
    sleep_ms(100);
}

// Função para iniciar a contagem regressiva do jogo
void start_countdown() {
    current_state = COUNTDOWN;
    countdown_start_time = get_absolute_time();
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "PREPARAR", 20, 25);
    ssd1306_send_data(&ssd);
    reaction_received = false;
    displayed = false;
    winner = 0;
}

// Inicialização dos GPIOs
void initialize_gpio(int pin, bool direction) {
    gpio_init(pin);
    gpio_set_dir(pin, direction);
    if(direction == GPIO_IN){
        gpio_pull_up(pin);
    } else {
        gpio_put(pin, 0);
    }
}