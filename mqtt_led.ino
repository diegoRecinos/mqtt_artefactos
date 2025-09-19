#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/
#define WLAN_SSID "**"
#define WLAN_PASS "**"

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "recinos000"
#define AIO_KEY "TU_CLAVE_ADAFRUIT_AQUI"  // Reemplaza con tu clave real

/************ Global State (no necesitas cambiar esto!) ******************/
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feed para Suscribirse ***************************************/
// Configura un feed llamado 'FeedTest' para recibir datos.
Adafruit_MQTT_Subscribe FeedTest = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/FeedTest");

/* **************** Declaración de variables globales *******************/
const int ledPin = D0;

/*************************** Sketch Code ************************************/
void handleMessage(char *data, uint16_t len) {
  Serial.print("Mensaje recibido del feed 'FeedTest': ");
  Serial.println(data);
  String message = String(data);
  message.trim();

  if (message == "ON") {
    digitalWrite(ledPin, HIGH);
    Serial.println("  -> LED encendido (HIGH) ");
  } else if (message == "OFF") {
    digitalWrite(ledPin, LOW);
    Serial.println("  -> LED apagado (LOW) ");
  } else {
    Serial.println("  -> Mensaje no reconocido.");
  }
}

/*********************************************** SetUp *****************************/
void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(ledPin, OUTPUT);
  Serial.println("\nIniciando configuracion...");

  // Conexión a la red Wi-Fi
  Serial.print("Conectando a WiFi: ");
  Serial.print(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado! ");

  // Configura la función de callback para el feed
  FeedTest.setCallback(handleMessage);

  // Suscríbete al feed
  mqtt.subscribe(&FeedTest);
}

//***************************************************** Loop ********************************/
void loop() {
  // Asegura la conexión a MQTT
  MQTT_connect();

  // Procesa los paquetes entrantes del MQTT (mensajes de Adafruit.io)
  mqtt.processPackets(100);

  // Mantiene la conexión viva
  if (!mqtt.ping()) {
    mqtt.disconnect();
  }
}

// Función para conectar y reconectar al servidor MQTT
void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Conectando a MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reintentando conexión MQTT en 10 segundos...");
    mqtt.disconnect();
    delay(10000);
    retries--;
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("MQTT Conectado! ");
}