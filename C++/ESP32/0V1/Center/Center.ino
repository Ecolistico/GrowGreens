/***** Incluir librerías *****/
#include "DHTesp.h"
#include <WiFi.h>
#include <PubSubClient.h>

/***** Definición de pines *****/
byte dht_1R_pin = 13;
byte dht_1L_pin = 12;
byte dht_2R_pin = 14;
byte dht_2L_pin = 27;
byte dht_3R_pin = 26;
byte dht_3L_pin = 25;
byte dht_4R_pin = 33;
byte dht_4L_pin = 32;

/***** Definición de variables globales *****/
DHTesp dht_1R;
DHTesp dht_1L;
DHTesp dht_2R;
DHTesp dht_2L;
DHTesp dht_3R;
DHTesp dht_3L;
DHTesp dht_4R;
DHTesp dht_4L;

/***** Definición de Variables Auxiliares *****/
TempAndHumidity data_1R;
TempAndHumidity data_1L;
TempAndHumidity data_2R;
TempAndHumidity data_2L;
TempAndHumidity data_3R;
TempAndHumidity data_3L;
TempAndHumidity data_4R;
TempAndHumidity data_4L;

// Constante (suavizante) filtro exponencial
float alpha = 0.5;

// Variable temporal
unsigned long update_time;
const unsigned long update_constant = 5000; // Actualizar información cada 5 segundos.

// SSID/Password red WiFi
// Cambiar por internet
const char* ssid = "ATT_Internet_En_Casa_4364";
const char* password = "31LADY8Y5R8";

// MQTT Broker dirección IP:
const char* mqtt_server = "192.168.8.73";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// Filtro exponencial
float exponencial_filter(float alpha, float t, float t_1){
  if(isnan(alpha) || isnan(t) || isnan(t_1)){
    return t;
  }
  else{
    return (t*alpha+(1-alpha)*t_1);  
  }
}

TempAndHumidity getData(DHTesp &dht, TempAndHumidity &input_data){
  TempAndHumidity previous_data = input_data;
  input_data = dht.getTempAndHumidity();
  // Aplicar filtros
  input_data.temperature = exponencial_filter(alpha, input_data.temperature, previous_data.temperature);
  input_data.humidity = exponencial_filter(alpha, input_data.humidity, previous_data.humidity);
  return input_data;
}

void updateData(){
  getData(dht_1R, data_1R);
  getData(dht_1L, data_1L);
  getData(dht_2R, data_2R);
  getData(dht_2L, data_2L);
  getData(dht_3R, data_3R);
  getData(dht_3L, data_3L);
  getData(dht_4R, data_4R);
  getData(dht_4L, data_4L);
}

void sendData(){
  String textString = String(data_1R.temperature) + "," + String(data_1R.humidity) + "," + String(data_1L.temperature)+ "," + String(data_1L.humidity) + ",";
  textString += String(data_2R.temperature) + "," + String(data_2R.humidity) + "," + String(data_2L.temperature) + "," + String(data_2L.humidity) + ",";
  textString += String(data_3R.temperature) + "," + String(data_3R.humidity) + "," + String(data_3L.temperature) + "," + String(data_3L.humidity) + ",";
  textString += String(data_4R.temperature) + "," + String(data_4R.humidity) + "," + String(data_4L.temperature) + "," + String(data_4L.humidity);
  char textData[128];
  textString.toCharArray(textData,128);
  client.publish("alfa/sensor/center", textData);  
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("alfa/sensor/center");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  
  if (String(topic) == "alfa/sensor/center" && messageTemp == "sendData") {
    sendData();
  }
}

void setup(){
  // Inicializar Serial
  Serial.begin(115200);
  Serial.println();
  Serial.println("Serial Inicializado");
  
  // Inicializar sensores
  dht_1R.setup(dht_1R_pin, DHTesp::DHT22);
  dht_1L.setup(dht_1L_pin, DHTesp::DHT22);
  dht_2R.setup(dht_2R_pin, DHTesp::DHT22);
  dht_2L.setup(dht_2L_pin, DHTesp::DHT22);
  dht_3R.setup(dht_3R_pin, DHTesp::DHT22);
  dht_3L.setup(dht_3L_pin, DHTesp::DHT22);
  dht_4R.setup(dht_4R_pin, DHTesp::DHT22);
  dht_4L.setup(dht_4L_pin, DHTesp::DHT22);
  Serial.println("DHT´s Inicializados");

  // Inicializar servidor
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); // Función para recibir mensajes y ejecutar acciones
  
  update_time = millis();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  if(millis()-update_time>update_constant){
    updateData();
    update_time = millis();
  }
  client.loop();
}
