# Projeto Jacaré Alecrim: Marco 2
Grupo: Líbna Raffaelly, Lucas Iglesias, Nicole Liecheski e Victor Gabriel Pacheco

Este projeto simula um sistema de Internet das Coisas (IoT) de ponta a ponta. Ele utiliza um microcontrolador ESP32 para ler dados ambientais (Temperatura, Umidade e Gás) e enviá-los para um banco de dados na nuvem (ThingSpeak). Em seguida, o ESP32 consulta o próprio banco de dados para decidir se deve ou não acionar três atuadores físicos (Ar Condicionado, Umidificador e Ventilação).

## Tecnologias Utilizadas
- **Hardware (Simulado):** ESP32, Sensor DHT22, Sensor MQ2, LEDs (Atuadores).
- **Ambiente de Desenvolvimento:** VS Code.
- **Framework:** PlatformIO (Arduino).
- **Simulador:** Wokwi Simulator Extension.
- **Nuvem / Banco de Dados:** ThingSpeak (MathWorks).

## Configurações Necessárias para Integração

Para que o projeto se conecte à nuvem e funcione corretamente, é necessário possuir uma conta no [ThingSpeak](https://thingspeak.com/) e configurar as chaves de API.

1. Crie um novo **Channel** no ThingSpeak.
2. Habilite **6 Fields** nas configurações do canal.
3. Obtenha o seu `Channel ID`, a `Write API Key` e a `Read API Key`.
4. No arquivo `src/main.cpp`, substitua as credenciais pelas suas:

```cpp
unsigned long myChannelNumber = SEU_CANAL_AQUI;
const char * myWriteAPIKey = "SUA_WRITE_KEY";
const char * myReadAPIKey = "SUA_READ_KEY";
```

> **Nota:** As credenciais de WiFi já estão configuradas para o ambiente de simulação (`Wokwi-GUEST`), não as altere.

## Contrato Utilizado (ThingSpeak API v1)

A comunicação com o banco de dados ocorre via HTTP GET/POST através da biblioteca oficial `ThingSpeak.h` para escrita e requisições HTTP RESTful diretas para leitura.

### Escrita (Upload de Dados Sensoriais)

O ESP32 envia a cada 15+ segundos um payload contendo o estado local dos sensores e as regras de negócio embutidas.

**Mapeamento dos Fields:**

* `field1`: Temperatura em °C (Float) - *Origem: DHT22*
* `field2`: Umidade em % (Float) - *Origem: DHT22*
* `field3`: Concentração de Gás em PPM (Float) - *Origem: MQ2 (Logarítmico)*
* `field4`: Ordem Lógica Ar Condicionado (0 ou 1)
* `field5`: Ordem Lógica Umidificador (0 ou 1)
* `field6`: Ordem Lógica Ventilação (0 ou 1)

### Leitura (Download de Comandos de Atuação)

A rotina de atuação física da placa **ignora** as variáveis locais e consulta isoladamente cada Field da nuvem para alterar os atuadores.

* **Endpoint utilizado:** `GET https://api.thingspeak.com/channels/{CHANNEL_ID}/fields/{FIELD_ID}/last.txt?api_key={READ_API_KEY}`
* **Retorno Esperado:** String `"0"` (Desligado) ou `"1"` (Ligado).

## Instruções Mínimas para Executar a Solução (VS Code)

Para clonar e rodar esta solução no seu computador usando simulação de hardware:

1. **Instale as Extensões no VS Code:**
* Procure por `PlatformIO IDE` (ícone de formiga) e instale.
* Procure por `Wokwi Simulator` e instale.


2. **Ative o Simulador (Apenas 1ª vez):**
* Pressione `Ctrl+Shift+P` (ou `Cmd+Shift+P`), digite `Wokwi: Request a New License` e aperte Enter. Faça login no navegador para ativar a simulação gratuita.


3. **Abra o Projeto:**
* Clone este repositório ou extraia o `.zip`.
* No VS Code, vá em `File > Open Folder...` e selecione a raiz do projeto (onde o arquivo `platformio.ini` está localizado).


4. **Compile o Código:**
* Aguarde o PlatformIO carregar o projeto.
* Clique no ícone de **"Check" (✓ - Build)** localizado na barra azul inferior do VS Code. O PlatformIO fará o download das bibliotecas (DHT e ThingSpeak) e compilará o firmware.


5. **Inicie a Simulação:**
* Abra o arquivo `diagram.json` no editor.
* Clique no botão de **Play (Start Simulator)** que aparecerá no canto superior direito do código.
* A simulação abrirá na lateral. Você pode clicar nos sensores (DHT22 e MQ2) durante a execução para alterar os valores, e observar os LEDs acendendo após o tempo de resposta da nuvem (ThingSpeak).
