#include <WiFi.h>
#include "ThingSpeak.h"
#include "DHT.h"
#include <math.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
unsigned long myChannelNumber = 3499366;
const char * myWriteAPIKey = "MY1P115DDMULU82J";
const char * myReadAPIKey = "5BLHWBEE89ELZTLF";
WiFiClient client;

#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
#define MQ2PIN 34
#define PIN_AC 2       
#define PIN_UMI 4      
#define PIN_VENT 5     

float calcularPPM(int valorCru) {
  if (valorCru <= 843) return 0.1;
  if (valorCru >= 4041) return 100000.0;
  float ratio = (4095.0 / (float)valorCru) - 1.0;
  return pow(10, (-2.478 * log10(ratio) + 0.356));
}

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_AC, OUTPUT);
  pinMode(PIN_UMI, OUTPUT);
  pinMode(PIN_VENT, OUTPUT);
  digitalWrite(PIN_AC, LOW);
  digitalWrite(PIN_UMI, LOW);
  digitalWrite(PIN_VENT, LOW);
  
  dht.begin();
  analogSetPinAttenuation(MQ2PIN, ADC_11db);
  pinMode(MQ2PIN, INPUT);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("\nwifi conectado\n");
  ThingSpeak.begin(client);
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
    
    Serial.println("etapa 1: gravando no thingspeak");
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    float ppm = calcularPPM(analogRead(MQ2PIN));

    if (!isnan(t) && !isnan(h)) {
      int ligarAC = (t > 28.0) ? 1 : 0;        
      int ligarUmi = (h < 40.0) ? 1 : 0;       
      int ligarVent = (ppm > 2000.0) ? 1 : 0;  
      
      ThingSpeak.setField(1, t);
      ThingSpeak.setField(2, h);
      ThingSpeak.setField(3, ppm);
      ThingSpeak.setField(4, ligarAC);
      ThingSpeak.setField(5, ligarUmi);
      ThingSpeak.setField(6, ligarVent);
      
      int httpCodeEnvio = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      if(httpCodeEnvio == 200){
        Serial.println("envio com sucesso");
      } else {
        Serial.println("erro de envio, code http: " + String(httpCodeEnvio));
      }
    }

    Serial.println("\n15s delay antes de ler\n");
    delay(15000);

    Serial.println("etapa 2: lendo no thingspeak");
    
    int comandoAC = ThingSpeak.readIntField(myChannelNumber, 4, myReadAPIKey);
    if(ThingSpeak.getLastReadStatus() == 200){
      digitalWrite(PIN_AC, comandoAC);
      Serial.print("ar-condicionado: "); Serial.println(comandoAC == 1 ? "HIGH" : "LOW");
    } else {
      Serial.println("erro ar-condicionado");
    }
    
    // Serial.println("delay 15s");
    // delay(15000); 

    int comandoUmi = ThingSpeak.readIntField(myChannelNumber, 5, myReadAPIKey);
    if(ThingSpeak.getLastReadStatus() == 200){
      digitalWrite(PIN_UMI, comandoUmi);
      Serial.print("umidificador: "); Serial.println(comandoUmi == 1 ? "HIGH" : "LOW");
    } else {
      Serial.println("erro umidificador");
    }

    // Serial.println("delay 15s");
    // delay(15000);

    int comandoVent = ThingSpeak.readIntField(myChannelNumber, 6, myReadAPIKey);
    if(ThingSpeak.getLastReadStatus() == 200){
      digitalWrite(PIN_VENT, comandoVent);
      Serial.print("ventilacao: "); Serial.println(comandoVent == 1 ? "HIGH" : "LOW");
    } else {
      Serial.println("erro ventilacao");
    }
    
    Serial.println("\n----\n");
  }
}