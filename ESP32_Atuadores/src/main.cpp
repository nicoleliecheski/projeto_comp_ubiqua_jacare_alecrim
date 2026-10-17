#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h> 

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

const char* mqtt_client_id = "JBchHxsRBhkaIBotNRElKC8";
const char* mqtt_username  = "JBchHxsRBhkaIBotNRElKC8";
const char* mqtt_password  = "bYk8vCfT7wvFfHtlJP/xSjVK";

#define SensoresChannelID 3499366
#define AtuadoresChannelID 3540202

float t = -100.00;
float h = -100.00;
float ppm = -100.0;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  String topicStr = String(topic);

  //Serial.println("novo dado recebido\n");
  //Serial.print(topic);
  //Serial.print("] ");
  //for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
  //}
  //Serial.println();

  JsonDocument j;
  deserializeJson(j,message);

  String strT = String(j["field1"]);
  String strH = String(j["field2"]);
  String strPPM = String(j["field3"]);

  if (strT != "null"){
    Serial.println("novo dado recebido\n");
    Serial.println("temperatura: " + strT + " °C");
    t = strT.toFloat();
  }
  if (strH != "null") {
    Serial.println("umidade: " + strH + " %");
    h = strH.toFloat();
  }
  if (strPPM != "null") {
    Serial.println("gas: " + strPPM + " PPM");
    ppm = strPPM.toFloat();
  }
}
 

void mqttPublish(int ligarAC, int ligarUmi, int ligarVent) {
  String topicString ="channels/" + String(AtuadoresChannelID) + "/publish";

  String payload = "field1=" + String(ligarAC) + "&field2=" + String(ligarUmi) + "&field3=" + String(ligarVent);

  //Serial.println(payload);

  if(mqttClient.publish(topicString.c_str(), payload.c_str())){
    Serial.println("envio sucesso");
  } else {
    Serial.println("erro no envio.");
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
  String myTopic = "channels/"+String(SensoresChannelID)+"/subscribe";
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

#define PIN_AC 2       
#define PIN_UMI 4      
#define PIN_VENT 5 

void setup() {
  Serial.begin(115200);
  Serial.println("teste\n");

  pinMode(PIN_AC, OUTPUT);
  pinMode(PIN_UMI, OUTPUT);
  pinMode(PIN_VENT, OUTPUT);
  digitalWrite(PIN_AC, LOW);
  digitalWrite(PIN_UMI, LOW);
  digitalWrite(PIN_VENT, LOW);

  connectWifi();

  // Configure the MQTT client
  mqttClient.setServer( server, mqttPort ); 

  client.setCACert(thingspeak_ca_cert);
  
  mqttClient.setCallback(mqttCallback);
  mqttClient.setBufferSize( 2048 );
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

  int ligarAC = 0;
  if(t == -100.0){
    Serial.println("dado inicial");
  } else if(t < -40.0 || t > 80.0){
    Serial.print("dado invalido de temperatura ");
    Serial.println(t);
  } else{
    ligarAC = (t > 28.0) ? 1 : 0;
  }

  int ligarUmi = 0;
  if(h == -100.0){
    Serial.println("dado inicial");
  } else if(h < 0.0 || h > 100.0){
    Serial.println("dado invalido de humidade");
  } else{
    ligarUmi = (h < 40.0) ? 1 : 0;
  }

  int ligarVent = 0;
  if(ppm == -100.0){
    Serial.println("dado inicial");
  } else if(ppm < 0.1 || ppm > 100000.0){
    Serial.println("dado invalido de gas inflamavel");
  } else{
    ligarVent = (ppm > 2000.0) ? 1 : 0;
  }

  digitalWrite(PIN_AC, ligarAC);
  digitalWrite(PIN_UMI, ligarUmi);
  digitalWrite(PIN_VENT, ligarVent);

  mqttPublish(ligarAC, ligarUmi, ligarVent);
  
  delay(7000); 
  Serial.println("\n----\n");
}
