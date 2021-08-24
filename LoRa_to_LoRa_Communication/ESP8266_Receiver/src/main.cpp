/*
Bu projede LoRa sensorlerimiz ile uzak mesafe iletisimi kurarak veri aktarimi saglayacagiz.
Nesnelerin interneti ve LoRa iletisimini birlestirerek 3kmye kadar uzanan bir mesafe icinde vericiye bagli olan
sicaklik ve nem okuyucumuz aliciya verileri aktaracak. Alicimiz ise MQTT protokolu ile duzenli araliklarla
gelen verileri yayinlayacak.
Bu proje herhangi bir fabrikanin, isletmenin uzaktaki bir odada ya da mekandaki verilerini baska bir yerden elde etmemizi saglayabilir.
Verici olarak ESP32uC kullandim. Alici olarak ESP8266uC kullandım ve WiFi uzerinden internete aktarim sagladim.

Lora Transmitter-Receiver icin kullandigimiz modul datasheet'i -> https://www.ebyte.com/en/product-view-news.html?id=130
Sicaklik-nem olcen sensorun datasheet'i -> https://mikroshop.ch/pdf/DHT21.pdf


 * E32-TTL-100----- ESP8266(RECEIVER)
 * M0         ----- GND
 * M1         ----- GND
 * TX         ----- D4  (PullUP) (Istege baglidir. SoftwareSerial kutuphanesi sayesinde)
 * RX         ----- D2 (PullUP & Voltage divider)  (Istege baglidir. SoftwareSerial kutuphanesi sayesinde)
 * AUX        ----- Herhangi bir LED (Haberlesme esnasinda high seviyededir. Takip icin led bagladim. Istege baglidir.)
 * VCC        ----- 3.3v/5v
 * GND        ----- GND

 */

#include "LoRa_E32.h"
#include <SoftwareSerial.h>   //Dijital pinleri seri haberlesmede kullanmak icin kullandigimiz kutuphane
#include <PubSubClient.h>   // MQTT kullanmak icin kullanilan kutuphane.
#include <ESP8266WiFi.h>    // ESP8266'yi WiFi'a baglayan kutuphane.
// RX D2 ---- TX D4
SoftwareSerial mySerial(D2, D4);
LoRa_E32 e32ttl(&mySerial);
// -------------------------------------

#define wifi_ssid "digyold"         //WiFi SSID
#define wifi_password "1121012063"     // Wifi Password
#define mqtt_server "192.168.1.102"  // Mqtt Broker IP Adresi
WiFiClient espClient;
PubSubClient client(espClient);

 void setup_wifi()  // WiFi baglantisini yapan fonksiyonumuz.
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid,wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.print("WiFi connected. IP Adress: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Waiting MQTT connection...");
     String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
     Serial.println("\nConnected");
      // Once connected, publish an announcement...
      client.publish("DHT21", "Connected"); // Bağlantı kurulduğunda bu topic'e bildirim gelmesini istedim.
    } else {
      Serial.print("Connection Error!");
      Serial.print(client.state());
      Serial.println("Wait for 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);
  while (!mySerial) {
	    ; 
    }
  e32ttl.begin();
  delay(300);
	setup_wifi();                           //WiFi baglanma
	client.setServer(mqtt_server, 1883);   // Port ayari
}
  
void loop()
{
	if (!client.connected()) {
    reconnect();
  }
    client.loop();
  
	while(e32ttl.available() > 1 ){     
    String gelenmesaj= mySerial.readString();   // Gelen veriyi bir string olarak tanimlama
    Serial.println(gelenmesaj);
    delay(300);
    char message[78];
     gelenmesaj.toCharArray(message,78);   //Stringten char array'e
     delay(200);
    client.publish("dht21", message);  //MQTT uzerinden gelen verileri yayinlama. Bu topicle baglanmamiz lazim.
    Serial.println("Temperature and humidity sent to MQTT.");
    Serial.println("----------------------------------------"); 
    }   
	}