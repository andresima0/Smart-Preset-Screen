#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

// Definições do display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Instância do display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pinos dos botões para cada contador
const int botao1Pin = 2;  // Incrementa A
const int botao2Pin = 4;  // Decrementa A
const int botao3Pin = 7;  // Incrementa B
const int botao4Pin = 8;  // Decrementa B
const int botao5Pin = 9;  // Incrementa C
const int botao6Pin = 10; // Decrementa C

// Pino do botão para trocar os presets
const int presetButtonPin = 12; // Botão para alternar entre os presets

// Pinos do LED RGB
const int redPin = 13;    // Vermelho
const int greenPin = A0;  // Verde
const int bluePin = A1;   // Azul

// Pinos PWM para as saídas analógicas
const int pwmPinA = 3; // PWM - Saída para o contador A
const int pwmPinB = 5; // PWM - Saída para o contador B
const int pwmPinC = 6; // PWM - Saída para o contador C

// Variáveis para os contadores (matrizes para 6 presets)
int contadorA[6] = {0, 0, 0, 0, 0, 0};
int contadorB[6] = {0, 0, 0, 0, 0, 0};
int contadorC[6] = {0, 0, 0, 0, 0, 0};

// Índice do preset atual (0 a 5)
int presetIndex = 0;

// Variáveis para debouncing e estado dos botões
int botaoState[6] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
int lastBotaoState[6] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
unsigned long lastDebounceTime[6] = {0, 0, 0, 0, 0, 0};
unsigned long botaoPressionadoTime[6] = {0, 0, 0, 0, 0, 0};
unsigned long lastIncrementTime[6] = {0, 0, 0, 0, 0, 0};
bool botaoPressionado[6] = {false, false, false, false, false, false};

int incrementoRapido = 1; // Valor padrão de incremento/decremento

// Variáveis para o botão de preset
int presetButtonState = HIGH;
int lastPresetButtonState = HIGH;
unsigned long lastPresetDebounceTime = 0;

unsigned long debounceDelay = 50;   // Tempo de debounce
unsigned long holdDelay = 500;      // Tempo para considerar que o botão está sendo segurado (500 ms)
unsigned long repeatDelay = 50;    // Intervalo entre incrementos/decrementos quando o botão está sendo segurado (150 ms)

// Variáveis para o auto-salvamento na EEPROM
bool presetModificado = false;             // Indica se o preset atual foi modificado
unsigned long lastModificacaoTime = 0;     // Timestamp da última modificação
const unsigned long saveDelay = 5000;      // Delay antes de salvar na EEPROM (em milissegundos)

void setup() {
  Serial.begin(9600); // Inicializa a comunicação serial a 9600 bps

  // Inicializa o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha ao inicializar o display OLED"));
    for (;;);
  }

  // Configura os pinos dos botões como entradas com resistores pull-up internos
  pinMode(botao1Pin, INPUT_PULLUP);
  pinMode(botao2Pin, INPUT_PULLUP);
  pinMode(botao3Pin, INPUT_PULLUP);
  pinMode(botao4Pin, INPUT_PULLUP);
  pinMode(botao5Pin, INPUT_PULLUP);
  pinMode(botao6Pin, INPUT_PULLUP);

  // Configura o pino do botão de preset
  pinMode(presetButtonPin, INPUT_PULLUP);

  // Configura os pinos do LED RGB como saídas
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Desliga o LED RGB inicialmente
  desligarRGB();

  // Configura os pinos PWM como saídas
  pinMode(pwmPinA, OUTPUT);
  pinMode(pwmPinB, OUTPUT);
  pinMode(pwmPinC, OUTPUT);

  // Carrega as configurações da EEPROM
  carregarConfiguracoes();

  // Limpa e exibe os valores iniciais
  display.clearDisplay();
  atualizarDisplay();
  atualizarRGB();
  atualizarPWM();
}

void loop() {
  unsigned long tempoAtual = millis();

  // Verifica o botão de preset
  verificarBotaoPreset(tempoAtual);

  // Verifica cada botão e realiza a operação correspondente
  verificarBotao(botao1Pin, 0, &contadorA[presetIndex], 100, 1, tempoAtual);  // Incrementa A
  verificarBotao(botao2Pin, 1, &contadorA[presetIndex], 0, -1, tempoAtual);   // Decrementa A
  verificarBotao(botao3Pin, 2, &contadorB[presetIndex], 100, 1, tempoAtual);  // Incrementa B
  verificarBotao(botao4Pin, 3, &contadorB[presetIndex], 0, -1, tempoAtual);   // Decrementa B
  verificarBotao(botao5Pin, 4, &contadorC[presetIndex], 100, 1, tempoAtual);  // Incrementa C
  verificarBotao(botao6Pin, 5, &contadorC[presetIndex], 0, -1, tempoAtual);   // Decrementa C

  // Salva o preset na EEPROM após o delay se modificado
  if (presetModificado && (tempoAtual - lastModificacaoTime >= saveDelay)) {
    salvarConfiguracoes(presetIndex);
    presetModificado = false;
  }

  // Atualiza as tensões no monitor serial a cada segundo
  static unsigned long lastPrintTime = 0;
  if (tempoAtual - lastPrintTime >= 1000) { // A cada 1000 ms (1 segundo)
    atualizarPWM(); // Atualiza as saídas PWM e imprime as tensões
    lastPrintTime = tempoAtual;
  }
}

// Função para verificar o estado do botão de preset
void verificarBotaoPreset(unsigned long tempoAtual) {
  int leitura = digitalRead(presetButtonPin);

  if (leitura != lastPresetButtonState) {
    lastPresetDebounceTime = tempoAtual;
  }

  if ((tempoAtual - lastPresetDebounceTime) > debounceDelay) {
    if (leitura != presetButtonState) {
      presetButtonState = leitura;

      if (presetButtonState == LOW) {
        // Botão pressionado

        // Não precisamos mais salvar aqui, pois o auto-salvamento está implementado no loop()

        presetIndex++;
        if (presetIndex >= 6) {
          presetIndex = 0;
        }
        atualizarDisplay();
        atualizarRGB();
        atualizarPWM(); // Atualiza as saídas PWM para o novo preset
      }
    }
  }
  lastPresetButtonState = leitura;
}

// Função para atualizar o LED RGB com base no preset atual
void atualizarRGB() {
  // Desliga o LED RGB
  desligarRGB();

  int presetValue = presetIndex + 1; // Preset 1 a 6

  // Obtém a representação binária do presetValue
  int bit0 = (presetValue >> 0) & 0x01; // LSB
  int bit1 = (presetValue >> 1) & 0x01;
  int bit2 = (presetValue >> 2) & 0x01; // MSB

  // Controla os pinos do LED RGB
  digitalWrite(redPin, bit0 ? HIGH : LOW);   // Vermelho
  digitalWrite(greenPin, bit1 ? HIGH : LOW); // Verde
  digitalWrite(bluePin, bit2 ? HIGH : LOW);  // Azul
}

// Função para desligar o LED RGB
void desligarRGB() {
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
}

// Função para verificar o estado de um botão
void verificarBotao(int pin, int indice, int* contador, int limite, int operacao, unsigned long tempoAtual) {
  int leitura = digitalRead(pin);

  if (leitura != lastBotaoState[indice]) {
    lastDebounceTime[indice] = tempoAtual;
  }

  if ((tempoAtual - lastDebounceTime[indice]) > debounceDelay) {
    if (leitura != botaoState[indice]) {
      botaoState[indice] = leitura;

      if (botaoState[indice] == LOW) {
        // Botão pressionado
        botaoPressionado[indice] = true;
        botaoPressionadoTime[indice] = tempoAtual;
        lastIncrementTime[indice] = tempoAtual; // Inicializa o tempo do último incremento
        incrementoRapido = 1; // Reinicia o valor de incremento rápido
        // Aciona uma vez imediatamente
        alterarContador(contador, limite, operacao);
      } else {
        // Botão liberado
        botaoPressionado[indice] = false;
      }
    }
  }
  lastBotaoState[indice] = leitura;

  // Verifica se o botão está sendo segurado
  if (botaoPressionado[indice]) {
    if ((tempoAtual - botaoPressionadoTime[indice]) > holdDelay) {
      if ((tempoAtual - lastIncrementTime[indice]) >= repeatDelay) {
        incrementoRapido = 5; // Após meio segundo, incrementa de 5 em 5
        alterarContador(contador, limite, operacao);
        lastIncrementTime[indice] = tempoAtual;
      }
    }
  }
}

// Função para alterar o contador respeitando os limites
void alterarContador(int* contador, int limite, int operacao) {
  int incremento = incrementoRapido * operacao;

  if (operacao == 1 && *contador < limite) {
    *contador += incremento;
    if (*contador > limite) {
      *contador = limite;
    }
    atualizarDisplay();
    atualizarPWM(); // Atualiza as saídas PWM
    presetModificado = true; // Marca o preset como modificado
    lastModificacaoTime = millis(); // Registra o tempo da modificação
  } else if (operacao == -1 && *contador > limite) {
    *contador += incremento;
    if (*contador < limite) {
      *contador = limite;
    }
    atualizarDisplay();
    atualizarPWM(); // Atualiza as saídas PWM
    presetModificado = true; // Marca o preset como modificado
    lastModificacaoTime = millis(); // Registra o tempo da modificação
  }
}

// Função para atualizar o display OLED com os valores dos contadores e desenhar as barras
void atualizarDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Exibe o cabeçalho com o nome do preset atual
  display.setTextSize(1); // Tamanho de texto para o cabeçalho
  String titulo = "PRESET " + String(presetIndex + 1);
  centralizarTexto(titulo, 0, SCREEN_WIDTH, 0);

  // Configuração para centralizar o texto
  display.setTextSize(1);  // Tamanho de texto para os números e rótulos

  // Posições Y para os números, barras e rótulos
  int posYNumero = 12;
  int posYBarraTopo = 25;
  int posYBarraBase = 55;
  int posYRotulo = 57;

  // Altura máxima da barra
  int alturaMaxBarra = posYBarraBase - posYBarraTopo;

  // Largura de cada coluna com margem
  int margem = 2; // Margem lateral de 2 pixels
  int larguraColuna = SCREEN_WIDTH / 3;

  // Largura da barra
  int larguraBarra = 15; // A largura da barra gráfica

  // Exibe o contador A
  int colA = 0 * larguraColuna + margem;
  centralizarTexto(String(contadorA[presetIndex]), colA, larguraColuna - 2 * margem, posYNumero);
  desenharBarra(contadorA[presetIndex], colA + (larguraColuna - larguraBarra - 2 * margem) / 2, posYBarraTopo, larguraBarra, alturaMaxBarra);
  centralizarTexto("A", colA, larguraColuna - 2 * margem, posYRotulo);

  // Exibe o contador B
  int colB = 1 * larguraColuna + margem;
  centralizarTexto(String(contadorB[presetIndex]), colB, larguraColuna - 2 * margem, posYNumero);
  desenharBarra(contadorB[presetIndex], colB + (larguraColuna - larguraBarra - 2 * margem) / 2, posYBarraTopo, larguraBarra, alturaMaxBarra);
  centralizarTexto("B", colB, larguraColuna - 2 * margem, posYRotulo);

  // Exibe o contador C
  int colC = 2 * larguraColuna + margem;
  centralizarTexto(String(contadorC[presetIndex]), colC, larguraColuna - 2 * margem, posYNumero);
  desenharBarra(contadorC[presetIndex], colC + (larguraColuna - larguraBarra - 2 * margem) / 2, posYBarraTopo, larguraBarra, alturaMaxBarra);
  centralizarTexto("C", colC, larguraColuna - 2 * margem, posYRotulo);

  // Atualiza o display
  display.display();
}

// Função para desenhar a barra vertical
void desenharBarra(int valor, int posX, int posYTopo, int largura, int alturaMaxima) {
  // Limita o valor entre 0 e 100
  if (valor < 0) valor = 0;
  if (valor > 100) valor = 100;

  // Calcula a altura da barra proporcional ao valor (0 a 100)
  int alturaBarra = map(valor, 0, 100, 0, alturaMaxima);
  // Calcula a posição Y inicial da barra para que cresça de baixo para cima
  int posYBarra = posYTopo + (alturaMaxima - alturaBarra);
  // Desenha o retângulo preenchido representando a barra
  display.fillRect(posX, posYBarra, largura, alturaBarra, SSD1306_WHITE);
}

// Função auxiliar para centralizar o texto em uma área específica
void centralizarTexto(String texto, int xInicio, int largura, int posY) {
  int16_t x1, y1;
  uint16_t w, h;
  display.setTextWrap(false); // Desabilita a quebra de linha automática
  display.getTextBounds(texto, 0, 0, &x1, &y1, &w, &h);

  // Ajusta o posicionamento para garantir que o texto fique dentro da área
  int posX = xInicio + (largura - w) / 2;

  // Garante que o texto não ultrapasse os limites laterais
  if (posX < xInicio) {
    posX = xInicio;
  } else if (posX + w > xInicio + largura) {
    posX = xInicio + largura - w;
  }

  display.setCursor(posX, posY);
  display.print(texto);
}

// Função para atualizar as saídas PWM de acordo com os contadores e imprimir as tensões
void atualizarPWM() {
  // Converte o valor do contador (0-100) para o valor PWM (0-255)
  int valorPWMA = map(contadorA[presetIndex], 0, 100, 0, 255);
  int valorPWMB = map(contadorB[presetIndex], 0, 100, 0, 255);
  int valorPWMC = map(contadorC[presetIndex], 0, 100, 0, 255);

  // Escreve os valores PWM nos pinos correspondentes
  analogWrite(pwmPinA, valorPWMA);
  analogWrite(pwmPinB, valorPWMB);
  analogWrite(pwmPinC, valorPWMC);

  // Calcula as tensões correspondentes
  float tensaoA = (valorPWMA / 255.0) * 5.0;
  float tensaoB = (valorPWMB / 255.0) * 5.0;
  float tensaoC = (valorPWMC / 255.0) * 5.0;

  // Imprime as tensões no monitor serial
  Serial.print("Preset ");
  Serial.print(presetIndex + 1);
  Serial.print(" - ");

  Serial.print("Tensão A: ");
  Serial.print(tensaoA, 2);
  Serial.print(" V\t");

  Serial.print("Tensão B: ");
  Serial.print(tensaoB, 2);
  Serial.print(" V\t");

  Serial.print("Tensão C: ");
  Serial.print(tensaoC, 2);
  Serial.println(" V");
}

// Função para carregar as configurações da EEPROM
void carregarConfiguracoes() {
  // Verifica se a EEPROM já foi inicializada
  if (EEPROM.read(100) != 123) {
    // EEPROM não inicializada, define valores padrão
    for (int i = 0; i < 6; i++) {
      contadorA[i] = 0;
      contadorB[i] = 0;
      contadorC[i] = 0;
    }
    // Escreve o código mágico na EEPROM
    EEPROM.write(100, 123);
    // Salva os valores padrão na EEPROM
    for (int i = 0; i < 6; i++) {
      salvarConfiguracoes(i);
    }
  } else {
    // EEPROM já inicializada, carrega os valores
    for (int i = 0; i < 6; i++) {
      int enderecoBase = i * 3;
      contadorA[i] = EEPROM.read(enderecoBase);
      contadorB[i] = EEPROM.read(enderecoBase + 1);
      contadorC[i] = EEPROM.read(enderecoBase + 2);

      // Verifica se os valores estão dentro do intervalo válido (0 a 100)
      if (contadorA[i] < 0 || contadorA[i] > 100) contadorA[i] = 0;
      if (contadorB[i] < 0 || contadorB[i] > 100) contadorB[i] = 0;
      if (contadorC[i] < 0 || contadorC[i] > 100) contadorC[i] = 0;
    }
  }
}

// Função para salvar as configurações na EEPROM
void salvarConfiguracoes(int preset) {
  int enderecoBase = preset * 3;
  EEPROM.update(enderecoBase, contadorA[preset]);
  EEPROM.update(enderecoBase + 1, contadorB[preset]);
  EEPROM.update(enderecoBase + 2, contadorC[preset]);
}