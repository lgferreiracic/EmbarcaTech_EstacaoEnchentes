# EmbarcaTech_EstacaoEnchentes
<p align="center">
  <img src="Group 658.png" alt="EmbarcaTech" width="300">
</p>

## Projeto: Estação de Alerta de Enchente com Simulação por Joystick

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)
![Raspberry Pi](https://img.shields.io/badge/-Raspberry_Pi-C51A4A?style=for-the-badge&logo=Raspberry-Pi)
![HTML](https://img.shields.io/badge/HTML-%23E34F26.svg?style=for-the-badge&logo=html5&logoColor=white)
![CSS](https://img.shields.io/badge/CSS-1572B6?style=for-the-badge&logo=css3&logoColor=fff)
![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)
![Windows 11](https://img.shields.io/badge/Windows%2011-%230079d5.svg?style=for-the-badge&logo=Windows%2011&logoColor=white)

## Descrição do Projeto

Este projeto tem como objetivo simular uma Estação de Alerta de Enchente utilizando o microcontrolador Raspberry Pi Pico W, com interface física por joystick e visualização via Web Server. A estação monitora o "nível da água" e o "volume de chuva" com valores simulados a partir dos eixos analógicos do joystick. 

Todos os dados são processados por tarefas FreeRTOS que se comunicam exclusivamente por filas, conforme os requisitos do desafio. O sistema emite alertas visuais, sonoros e informativos em tempo real por meio de LED RGB, matriz de LEDs, buzzer, display OLED e servidor web embarcado.

## Componentes Utilizados

- **Joystick (ADC nos eixos X e Y)**: Simula o volume de chuva (eixo X) e o nível da água (eixo Y).
- **Microcontrolador Raspberry Pi Pico W (RP2040)**: Responsável pela execução do FreeRTOS e controle dos periféricos.
- **LED RGB**: Indicação visual dos níveis e status de alerta (normal ou crítico).
- **Matriz de LEDs WS2812B**: Exibição de ícone de alerta em caso de risco de enchente.
- **Display OLED SSD1306 (I2C)**: Exibe os valores de chuva, água e status geral do sistema.
- **Buzzers (2x)**: Alertas sonoros distintos para avisos e emergências.
- **Botões (A e B)**: Interação com o sistema e simulação de eventos.
- **Servidor Web**: Interface web acessível via Wi-Fi para exibição remota dos dados em tempo real.

## Ambiente de Desenvolvimento

- **VS Code**: Utilizado para desenvolvimento e debug do código.
- **C (Pico SDK)**: Linguagem e SDK principal para programação embarcada no RP2040.
- **FreeRTOS**: Sistema operacional de tempo real utilizado com tarefas e filas.
- **LwIP**: Implementação leve da pilha de protocolos IP para o servidor web.
- **HTML e CSS**: Interface web leve e responsiva para monitoramento remoto.

## Guia de Instalação

1. Clone este repositório.
2. Abra o projeto no VS Code com a extensão da Raspberry Pi.
3. Compile o código com o ambiente já configurado com o Pico SDK.
4. Conecte o RP2040 em modo **BOOTSEL** e arraste o arquivo `.uf2`.
5. Acesse a interface web com um dispositivo conectado à mesma rede Wi-Fi da placa.

## Modos de Operação

### 🟢 Modo Normal
- Exibição contínua dos valores de nível da água e volume de chuva no display OLED.
- Sem alertas sonoros.
- LED RGB indica estado "normal" (verde).
- Interface web mostra dados atualizados em tempo real.

### 🔴 Modo Alerta
Ativado automaticamente se:
- Nível da água ≥ 70% **ou**
- Volume de chuva ≥ 80%

Neste modo:
- LED RGB muda para vermelho.
- Buzzer emite sons de alerta contínuos.
- Display OLED destaca o status de alerta.
- Interface web exibe mensagem de emergência em destaque.

## Guia de Uso

Ao mover o joystick:
- O eixo X simula o volume de chuva (0–100%).
- O eixo Y simula o nível da água (0–100%).

O sistema processa os dados via tarefas FreeRTOS com comunicação por **filas**. O modo de operação é alterado automaticamente com base nos valores recebidos.

## Testes Realizados

1. Simulação de leituras com joystick e envio por fila.
2. Atualização correta do display OLED com os dados simulados.
3. Verificação das cores e alertas no LED RGB.
4. Emissão dos sons apropriados pelos buzzers.
5. Exibição da interface web com valores sincronizados.
6. Alteração entre modos normal e alerta com base em limites críticos.

## Vídeo da Solução

[Assista no YouTube](https://www.youtube.com/)
