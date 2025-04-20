# Jogo de Reflexo 1v1 com BitDogLab

Projeto desenvolvido para a disciplina de Sistemas Embarcados utilizando a placa de desenvolvimento BitDogLab. O jogo testa o tempo de reação de dois jogadores e faz uso de diversos periféricos para fornecer uma experiência visual e auditiva interativa.

## 🎯 Objetivo

Criar um jogo de reflexo competitivo entre dois jogadores, onde vence quem marcar primeiro 3 pontos. O projeto visa exercitar o uso de GPIOs, ADC, PWM, interrupções e controle de periféricos como buzzer, matriz de LEDs e display OLED.

## ⚙️ Como Funciona

- O jogo começa com uma contagem regressiva de 5 segundos.
- Caso algum jogador pressione seu botão antes da hora (falsa largada), o adversário recebe um ponto.
- Após o tempo, o sistema mostra "VAI" e toca um beep.
- O primeiro jogador a apertar corretamente seu botão ganha o ponto.
- O placar vai até 3 pontos.
- Ao final, o display mostra o vencedor e aguarda o reinício via botão do joystick.

Antes de iniciar o jogo, o joystick pode ser usado para:

- Mover um quadrado no display OLED.
- Controlar dinamicamente a cor exibida por todos os LEDs da matriz 5x5 WS2812.

## 🧩 Periféricos Utilizados

- **Display OLED** – Mostra status do jogo, contagem, pontuação e mensagens.
- **Joystick com botão (VRX, VRY, SW)** – Controla quadrado e cor da matriz; inicia ou reinicia o jogo.
- **Botões A e B** – Cada jogador usa um botão para competir na rodada.
- **Matriz WS2812 (5x5 RGB LEDs)** – Mostra cor controlada por joystick; usada para efeitos visuais.
- **LED RGB** – Indica estados do jogo: contagem, rodada ativa, vitória, erro.
- **Buzzer** – Emite som no início da rodada e ao final do jogo.
- **ADC** – Usado para ler os eixos do joystick.
- **PWM** – Usado para gerar som com o buzzer.
- **Interrupções GPIO** – Para detecção rápida dos botões.
- **UART** – Para comunicação serial com computador.

## 🛠️ Tecnologias e Ferramentas

- **Linguagem:** C
- **Microcontrolador:** Raspberry Pi Pico W
- **IDE Recomendada:** VSCode

## 🧠 Aprendizados

Este projeto reforçou conhecimentos em:

- Leitura analógica com ADC
- Programação com GPIOs e interrupções
- Controle de periféricos WS2812 com PIO
- Geração de som com PWM
- Estruturação de estados e lógica de jogo em sistemas embarcados

## Como executar o projeto 🛠️

1.  **Configuração do Ambiente:**

    - Certifique-se de ter o SDK do Raspberry Pi Pico instalado e configurado corretamente.
    - Configure as ferramentas de compilação (CMake, etc.).

2.  **Clone o repositório**
    ```
    git clone https://github.com/jacks0nsilva/EMBARCATECH-REVISAO.git
    ```
3.  **Instale a extensão do Raspberry Pi Pico no seu VsCode**
4.  **Usando a extensão do Raspberry Pi Pico, siga os passos:**
5.  **Clean CMake: Para garantir que o projeto será compilado do zero**

6.  **Compile Project: Compilação dos binários**

7.  **Run Project [USB]: Compila e copia o firmware para a placa automaticamente**
