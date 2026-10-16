#include <WiFi.h>
#include "DHT.h"
#include <math.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
  
const char * PROGMEM thingspeak_ca_cert = \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n" \
  "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
  "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n" \
  "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
  "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
  "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n" \
  "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n" \
  "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n" \
  "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n" \
  "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n" \
  "tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n" \
  "vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n" \
  "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n" \
  "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n" \
  "1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n" \
  "NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n" \
  "Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n" \
  "8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n" \
  "pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n" \
  "MrY=\n" \
  "-----END CERTIFICATE-----\n";

#define mqttPort 8883
WiFiClientSecure client; 
const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* server = "mqtt3.thingspeak.com";
int status = WL_IDLE_STATUS; 
int connectionDelay = 1;
PubSubClient mqttClient( client );

const char* mqtt_client_id = "CQwCCDQkLj0ALgUfPRoXEi8";
const char* mqtt_username  = "CQwCCDQkLj0ALgUfPRoXEi8";
const char* mqtt_password  = "5AXyB//pVstosEuFmkVDONq/";

#define channelID 3499366

#define DHTPIN 26
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
#define MQ2PIN 34         

void mqttPublish(float t, float h, float ppm) {
  String topicString ="channels/" + String(channelID) + "/publish";

  String payload = "field1=" + String(t) + "&field2=" + String(h) + "&field3=" + String(ppm);

  if(mqttClient.publish(topicString.c_str(), payload.c_str())){
    Serial.println("envio sucesso");
  } else {
    Serial.println("erro no envio.");
  }
}

int estadoAC = 2;
int estadoUMI = 2;
int estadoVENT = 2;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  String topicStr = String(topic);

  Serial.println("novo dado recebido\n");
  if (topicStr.indexOf("field4") > 0) {
    Serial.println("ar-condicionado: " + message);
    estadoAC = message.toInt();
  } else if (topicStr.indexOf("field5") > 0) {
    Serial.println("umidificador: " + message);
    estadoUMI = message.toInt();
  } else if (topicStr.indexOf("field6") > 0) {
    Serial.println("ventilacao: " + message);
    estadoVENT = message.toInt();
  }
}

void mqttConnect() {
  // Loop until connected.
  while ( !mqttClient.connected() )
  {
    // Connect to the MQTT broker.
    if ( mqttClient.connect( mqtt_client_id, mqtt_username, mqtt_password ) ) {
      Serial.print( "MQTT to " );
      Serial.print( server );
      Serial.print (" at port ");
      Serial.print( mqttPort );
      Serial.println( " successful." );
    } else {
      Serial.print( "MQTT connection failed, rc = " );
      // See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
      Serial.print( mqttClient.state() );
      Serial.println( " Will try again in a few seconds" );
      delay( connectionDelay*1000 );
    }
  }
}

// Subscribe to ThingSpeak channel for updates.
void mqttSubscribe(){
  String myTopic = "channels/"+String(channelID)+"/subscribe";
  mqttClient.subscribe(myTopic.c_str());
}

void connectWifi() {
  Serial.println("\nconectando wifi\n");
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
  delay(500);
  }
  Serial.println("wifi conectado\n");
}

float calcularPPM(int valorCru) {
  if (valorCru <= 843) return 0.1;
  if (valorCru >= 4041) return 100000.0;
  float ratio = (4095.0 / (float)valorCru) - 1.0;
  return pow(10, (-2.478 * log10(ratio) + 0.356));
}

void setup() {
  Serial.begin(115200);
  
  dht.begin();
  analogSetPinAttenuation(MQ2PIN, ADC_11db);
  pinMode(MQ2PIN, INPUT);

  connectWifi();

  // Configure the MQTT client
  mqttClient.setServer( server, mqttPort ); 

  client.setCACert(thingspeak_ca_cert);
  
}

void loop() {
  // Reconnect to WiFi if it gets disconnected.
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }
  
  // Connect if MQTT client is not connected and resubscribe to channel updates.
  if (!mqttClient.connected()) {
    mqttConnect(); 
    mqttSubscribe();
  }

  // Call the loop to maintain connection to the server.
  mqttClient.loop();

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float ppm = calcularPPM(analogRead(MQ2PIN));

  if(t < -40.0 || t > 80.0){
    Serial.print("dado invalido de temperatura ");
    Serial.println(t);
  } else if (h < 0.0 || h > 100.0){
    Serial.print("dado invalido de humidade");
    Serial.println(h);
  } else if(ppm < 0.1 || ppm > 100000.0){
    Serial.print("dado invalido de gas inflamavel");
    Serial.println(ppm);
  } else{
    mqttPublish(t, h, ppm);
  }

  Serial.println("delay 15s");
  delay(15000);

  if(estadoAC == 2){
    Serial.println("ar-condicionado no estado inicial");
  } else if((t > 28.0 && estadoAC == 1) || (t <= 28.0 && estadoAC == 0)){
    Serial.println("ar-condicionado se comportou como esperado");
  } else{
    Serial.print("ar-condicionado nao se comportou como esperado: t = ");
    Serial.println(String(t) + " estadoAC = " + String(estadoAC));
  }

  if(estadoUMI == 2){
    Serial.println("umidificador no estado inicial");
  } else if((h < 40.0 && estadoUMI == 1) || (h >= 40.0 && estadoUMI == 0)){
    Serial.println("umidificador se comportou como esperado");
  } else{
    Serial.print("umidificador nao se comportou como esperado: h = ");
    Serial.println(String(h) + " estadoUMI = " + String(estadoUMI));
  }
  
  if(estadoVENT == 2){
    Serial.println("ventilacao no estado inicial");
  } else if((ppm > 2000.0 && estadoVENT == 1) || (ppm <= 2000.0 && estadoVENT == 0)){
    Serial.println("ventilacao se comportou como esperado");
  } else{
    Serial.print("ventilacao nao se comportou como esperado: ppm = ");
    Serial.println(String(ppm) + " estadoVENT = " + String(estadoVENT));
  }

  Serial.println("\n----\n");
}