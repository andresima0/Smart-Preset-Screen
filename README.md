# SmartPresetScreen

SmartPresetScreen is an interactive Arduino-based controller that allows users to manage and adjust multiple preset configurations with real-time visual feedback through an OLED display, PWM outputs, and an RGB LED indicator. The project is designed to provide easy control of three separate counters across six preset configurations, enabling rapid customization and persistent storage.

https://github.com/user-attachments/assets/1c58f296-61d3-4c8d-9178-f073c27e793e

## Features

- **OLED Display Interface**: Displays the current preset, counter values, and graphical bar representation for each counter.
- **RGB LED Indicator**: Provides a visual cue for the active preset, with color codes representing presets 1 to 6.
- **Adjustable Counters**: Three independent counters (A, B, and C) with increment and decrement buttons for fine-tuning.
- **Six Preset Configurations**: Switch between six different presets, with settings stored in EEPROM for persistence.
- **PWM Outputs**: Generate PWM signals based on counter values for external device control.
- **Auto-save Functionality**: Automatically saves modified presets to EEPROM after a delay to reduce write operations.

## Hardware Requirements

- Arduino (compatible with ATmega328 or similar)
- SSD1306 OLED Display (128x64 pixels)
- RGB LED
- Buttons (x7) for increment, decrement, and preset switching
- EEPROM for data storage
- PWM-capable pins for counter outputs

## Pin Connections

- **Display (OLED)**: Uses I2C (`SDA` and `SCL` lines)
- **Buttons**: 
  - `botao1Pin` (Increment A) – Pin 2
  - `botao2Pin` (Decrement A) – Pin 4
  - `botao3Pin` (Increment B) – Pin 7
  - `botao4Pin` (Decrement B) – Pin 8
  - `botao5Pin` (Increment C) – Pin 9
  - `botao6Pin` (Decrement C) – Pin 10
  - `presetButtonPin` (Preset Switch) – Pin 12
- **RGB LED**:
  - `redPin` – Pin 13
  - `greenPin` – Analog Pin A0
  - `bluePin` – Analog Pin A1
- **PWM Outputs**:
  - `pwmPinA` (Counter A) – Pin 3
  - `pwmPinB` (Counter B) – Pin 5
  - `pwmPinC` (Counter C) – Pin 6

## Code Structure

### Preset Management

- **Presets**: Six separate presets are available, each containing individual values for counters A, B, and C.
- **Switching Presets**: The `presetButtonPin` cycles through presets, and the RGB LED changes color based on the preset selected.

### Button Controls

- **Increment/Decrement**: Each counter has buttons assigned for incrementing and decrementing values. The code includes debouncing and a "hold-to-increase" feature for quicker adjustments.
- **Debouncing and Hold Delay**: Debouncing prevents erratic readings, and holding a button for 500ms enables faster increments of 5.

### OLED Display

- **Display of Values**: The OLED shows the current preset, counter values, and bar representations of each counter’s value (0-100).
- **Real-Time Updates**: The display updates automatically whenever a counter is changed or a new preset is selected.

### PWM Outputs

- **Mapping Counter Values to PWM**: Counter values (0-100) are mapped to PWM ranges (0-255) and output through specific pins.
- **Voltage Display**: The corresponding voltage for each counter is calculated and displayed on the serial monitor.

### EEPROM Storage

- **Auto-Save with Delay**: Modified presets are saved to EEPROM after 5 seconds, reducing the frequency of EEPROM writes.
- **Initialization Check**: The first run initializes EEPROM with default values if no presets are found.

## Functions

- **`verificarBotao()`**: Handles debouncing, incrementing/decrementing, and hold-to-increase for each counter button.
- **`verificarBotaoPreset()`**: Switches between presets and updates RGB LED based on the active preset.
- **`atualizarDisplay()`**: Refreshes the OLED display with current counter values and visual bar representations.
- **`atualizarPWM()`**: Maps counter values to PWM and outputs, with real-time voltage feedback through serial output.
- **`salvarConfiguracoes()`** and **`carregarConfiguracoes()`**: Manages EEPROM storage, auto-saving modified presets, and retrieving values at startup.

## Installation

1. Clone or download the repository.
2. Ensure the required libraries are installed:
   - `Wire.h` (for I2C communication)
   - `Adafruit_GFX.h` and `Adafruit_SSD1306.h` (for OLED display control)
   - `EEPROM.h` (for EEPROM storage)
3. Upload the code to your Arduino.
4. Connect the hardware as per the **Pin Connections** section.

## Usage

- Press preset switch to toggle between presets.
- Use increment/decrement buttons to adjust counters A, B, and C.
- The OLED display will reflect changes instantly, and RGB LED will indicate the active preset.
- Counter values are automatically saved after 5 seconds of inactivity.

#Portuguese Verision:

SmartPresetScreen é um controlador interativo baseado em Arduino que permite aos usuários gerenciar e ajustar múltiplas configurações de presets com feedback visual em tempo real, através de um display OLED, saídas PWM e um indicador LED RGB. O projeto foi projetado para fornecer um controle fácil de três contadores independentes em seis configurações de presets, permitindo personalização rápida e armazenamento persistente.

https://github.com/user-attachments/assets/1c58f296-61d3-4c8d-9178-f073c27e793e

## Funcionalidades

- **Interface de Display OLED**: Exibe o preset atual, valores dos contadores e representação gráfica em barra para cada contador.
- **Indicador LED RGB**: Fornece um indicativo visual do preset ativo, com códigos de cor representando os presets de 1 a 6.
- **Contadores Ajustáveis**: Três contadores independentes (A, B e C) com botões de incremento e decremento para ajustes finos.
- **Seis Configurações de Presets**: Alterna entre seis presets diferentes, com configurações salvas na EEPROM para persistência.
- **Saídas PWM**: Gera sinais PWM com base nos valores dos contadores para controle de dispositivos externos.
- **Auto-salvamento**: Salva automaticamente os presets modificados na EEPROM após um atraso para reduzir operações de gravação.

## Requisitos de Hardware

- Arduino (compatível com ATmega328 ou similar)
- Display OLED SSD1306 (128x64 pixels)
- LED RGB
- Botões (x7) para incremento, decremento e troca de presets
- EEPROM para armazenamento de dados
- Pinos com capacidade PWM para as saídas dos contadores

## Conexões de Pinos

- **Display (OLED)**: Usa comunicação I2C (`SDA` e `SCL`)
- **Botões**:
  - `botao1Pin` (Incremento A) – Pino 2
  - `botao2Pin` (Decremento A) – Pino 4
  - `botao3Pin` (Incremento B) – Pino 7
  - `botao4Pin` (Decremento B) – Pino 8
  - `botao5Pin` (Incremento C) – Pino 9
  - `botao6Pin` (Decremento C) – Pino 10
  - `presetButtonPin` (Troca de Preset) – Pino 12
- **LED RGB**:
  - `redPin` – Pino 13
  - `greenPin` – Pino A0 (Analógico)
  - `bluePin` – Pino A1 (Analógico)
- **Saídas PWM**:
  - `pwmPinA` (Contador A) – Pino 3
  - `pwmPinB` (Contador B) – Pino 5
  - `pwmPinC` (Contador C) – Pino 6

## Estrutura do Código

### Gerenciamento de Presets

- **Presets**: Seis presets independentes, cada um com valores individuais para os contadores A, B e C.
- **Alternância de Presets**: O `presetButtonPin` permite alternar entre os presets, e o LED RGB muda de cor com base no preset ativo.

### Controle dos Botões

- **Incremento/Decremento**: Cada contador possui botões dedicados para incrementar e decrementar valores. O código inclui debouncing e uma funcionalidade "pressionar e segurar" para facilitar ajustes rápidos.
- **Debouncing e Delay de Segurando**: Debouncing previne leituras erradas devido a ruídos, e segurar um botão por mais de 500 ms permite incrementos rápidos de 5.

### Display OLED

- **Exibição de Valores**: O OLED mostra o preset atual, valores dos contadores e gráficos de barra verticais que indicam visualmente o valor dos contadores (0-100).
- **Atualizações em Tempo Real**: O display é atualizado automaticamente sempre que um contador é alterado ou um novo preset é selecionado.

### Saídas PWM

- **Mapeamento de Valores para PWM**: Valores dos contadores (0-100) são mapeados para valores PWM (0-255) e saem nos pinos correspondentes.
- **Exibição de Tensão**: A tensão correspondente para cada contador é calculada e exibida no monitor serial.

### Armazenamento em EEPROM

- **Auto-salvamento com Atraso**: Presets modificados são salvos na EEPROM após 5 segundos de inatividade, reduzindo a frequência de gravações na EEPROM.
- **Verificação de Inicialização da EEPROM**: Na primeira execução, a EEPROM é inicializada com valores padrão, caso nenhum preset seja encontrado.

## Funções

- **`verificarBotao()`**: Controla o debouncing, incremento/decremento e o "pressionar e segurar" para cada botão de contador.
- **`verificarBotaoPreset()`**: Alterna entre os presets e atualiza o LED RGB com base no preset ativo.
- **`atualizarDisplay()`**: Atualiza o display OLED com os valores atuais dos contadores e gráficos de barra.
- **`atualizarPWM()`**: Mapeia os valores dos contadores para saídas PWM e exibe a tensão correspondente no monitor serial.
- **`salvarConfiguracoes()`** e **`carregarConfiguracoes()`**: Gerencia o armazenamento na EEPROM, salvando automaticamente os presets modificados e recuperando os valores na inicialização.

## Instalação

1. Clone ou baixe o repositório.
2. Certifique-se de que as bibliotecas necessárias estão instaladas:
   - `Wire.h` (para comunicação I2C)
   - `Adafruit_GFX.h` e `Adafruit_SSD1306.h` (para controle do display OLED)
   - `EEPROM.h` (para armazenamento na EEPROM)
3. Carregue o código no seu Arduino.
4. Conecte o hardware conforme a seção **Conexões de Pinos**.

## Uso

- Pressione o botão de troca de preset para alternar entre os presets.
- Use os botões de incremento/decremento para ajustar os contadores A, B e C.
- O display OLED refletirá as mudanças instantaneamente, e o LED RGB indicará o preset ativo.
- Os valores dos contadores são salvos automaticamente após 5 segundos de inatividade.
