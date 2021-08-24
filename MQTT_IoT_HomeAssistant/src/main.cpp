/*
MQTT protokolü ile remote sekilde LED kontrolu.
ESP32 ve ESP8266 gibi WiFi'a baglanabilen mikrokontrolculer ve 
MQTT'nin asenkron olmasi ve dusuk kaynak tuketimi birlesince IoT evreninde basarili isler cikiyor.
Senaryolar eklenebilir. MQTT Broker burada mesaj yayinlama merkezimiz ama bunu HomeAssistant'a aktararak
bir nevi lambayi telefonla bilgisayarla kapatiyor gibi dusunebiliriz.

Bu projede ESP8266 modulu ve PubsubClient kutuphanesini kullaniyorum.
LED'i herhangi bir GPIO baglayabilirsiniz.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>//Bu kutuphaneyi kullanarak ESP8266'da MQTT protokolunu kullanabiliyoruz.
// Asagidaki verileri kendi bilgilerimize göre dolduruyoruz.
const char* ssid = "YOUR WIFI SSID";
const char* password = "YOUR WIFI PASSWORD";
const char* mqtt_server = "YOUR MQTT BROKER IP ADRESS";
const char* mqtt_username = "HOMEASSISTANT MQTT USERNAME";
const char* mqtt_password = "HOMEASSISTANT MQTT USER PASSWORD";
#define LED 2

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("\nArrived message from -> [");
  Serial.println(topic);
  Serial.print("]= ");
  Serial.print("Message: ");

  String message;
  for (int i = 0; i < length; i++) {
    message = message + (char)payload[i];  // Byte'tan string'e cevirme.
  }
   Serial.print(message);
  if(message == "0") {digitalWrite(LED,LOW);}
  if(message == "1") {digitalWrite(LED,HIGH);}
  Serial.println();
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);     // Cikis olarak LED'i tanimlama
  digitalWrite(LED,LOW);    // Baslangicta LED'i sondurme
  Serial.begin(115200); // Burada tanimladigimizi .ini dosyasında da tanımlayarak baud rate'in uyumlu olmasini saglamamiz lazim.
  delay(10);

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - IP Adress: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);

  if(client.connect("Light",mqtt_username,mqtt_password)){
    Serial.println("Baglandi");
    client.subscribe("inLight");    //BURAYA DIKKAT. MQTT Broker'da kullanacagimiz topic burasiyla ayni olmali.
  }else{
    Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
  }
}

void loop() {
  client.loop();
}