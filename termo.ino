#include "DHTesp.h"
#include "ESP8266WiFi.h"
#include "PubSubClient.h"
 
DHTesp dht;

const char *mqtt_server = "192.168.1.44";
const int   mqtt_port = 1883;
const char *root_topic_publish = "weatherSensor";

const char *ssid = "MOVISTAR_7C0C";
const char *password = "KAKITA";

WiFiClient espClient;
PubSubClient client(espClient);

#define MSG_BUFFER_SIZE  (100)
char msg[MSG_BUFFER_SIZE];

void callback(char* topic, byte* payload, int length);
void reconnect();
void setup_wifi();
 
void setup() {
    Serial.begin(115200);
    dht.setup(D5, DHTesp::DHT22);
    setup_wifi();
    client.setServer(mqtt_server, mqtt_port);
}
 
void loop() {

    if ( !client.connected()) {
      reconnect();
    }

    if ( client.connected()) {    

      client.loop();
      
      float h = dht.getHumidity();
      float t = dht.getTemperature();
 
      snprintf(msg, MSG_BUFFER_SIZE, "{\"humedad\": %.2f , \"temperatura\": %.2f}", h, t);
      Serial.print(root_topic_publish);
      Serial.print("\t-->");
      Serial.print(msg);
      Serial.println("<--");
      client.publish(root_topic_publish, msg, true);
      delay(2000);
    }
}

void setup_wifi() {
  delay(10);

  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);
  
  WiFi.begin((char *)ssid, (char *)password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());  
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    String clientId = "FERNANCODER-";
    clientId += String(random(0xffff), HEX);
    if ( client.connect(clientId.c_str())) {
      Serial.print(clientId);
      Serial.println(" conectado!");
    } else {
      Serial.print("Error: ");
      Serial.print(client.state());
      Serial.println(" reintento en cinco segundos");
      delay(5000);
    }
  }
}
