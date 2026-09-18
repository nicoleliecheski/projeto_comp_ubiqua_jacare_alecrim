graph TD
    %% Definição das camadas
    subgraph Edge [Hardware Simulado]
        direction TB
        subgraph Sensores
            DHT22[Sensor DHT22 <br/> Temperatura e Umidade]
            MQ2[Sensor MQ2 <br/> Gás]
        end

        ESP32[Microcontrolador ESP32 <br/> WiFi Habilitado]

        subgraph Atuadores
            AC((LED Azul <br/> Ar-condicionado))
            UMI((LED Verde <br/> Umidificador))
            VENT((LED Amarelo <br/> Ventilação))
        end
    end

    subgraph Nuvem [ThingSpeak]
        TS[(Banco de Dados <br/> IoT ThingSpeak)]
    end

    %% Fluxo de entrada (Leitura)
    DHT22 -- "Sinal Digital (Pino 15)" --> ESP32
    MQ2 -- "Sinal Analógico (Pino 34)" --> ESP32

    %% Comunicação com a Nuvem
    ESP32 -- "Etapa 1: Envia HTTP POST <br/>(Dados Brutos e Estado Lógico)" --> TS
    TS -- "Etapa 2: Responde HTTP GET <br/>(Consulta Fields 4, 5 e 6)" --> ESP32

    %% Fluxo de saída (Atuação)
    ESP32 -- "Ligar/Desligar" --> AC
    ESP32 -- "Ligar/Desligar" --> UMI
    ESP32 -- "Ligar/Desligar" --> VENT

    %% Estilização
    style Nuvem fill:#e1f5fe,stroke:#0288d1,stroke-width:2px
    style ESP32 fill:#ffecb3,stroke:#ffb300,stroke-width:2px
    style TS fill:#c8e6c9,stroke:#388e3c,stroke-width:2px