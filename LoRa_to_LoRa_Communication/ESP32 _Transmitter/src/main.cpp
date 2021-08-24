/*
Bu projede LoRa sensorlerimiz ile uzak mesafe iletisimi kurarak veri aktarimi saglayacagiz.
Nesnelerin interneti ve LoRa iletisimini birlestirerek 3kmye kadar uzanan bir mesafe icinde vericiye bagli olan
sicaklik ve nem okuyucumuz aliciya verileri aktaracak. Alicimiz ise MQTT protokolu ile duzenli araliklarla
gelen verileri yayinlayacak.
Bu proje herhangi bir fabrikanin, isletmenin uzaktaki bir odada ya da mekandaki verilerini baska bir yerden elde etmemizi saglayabilir.
Verici olarak ESP32uC kullandim. Alici olarak ESP8266uC kullandım ve WiFi uzerinden internete aktarim sagladim.

Lora Transmitter-Receiver icin kullandigimiz modul datasheet'i -> https://www.ebyte.com/en/product-view-news.html?id=130
Sicaklik-nem olcen sensorun datasheet'i -> https://mikroshop.ch/pdf/DHT21.pdf


 * E32-TTL-100----- ESP32 (RECEIVER)
 * M0         ----- GND
 * M1         ----- GND
 * TX         ----- ESP32 RX  (PullUP)
 * RX         ----- ESP32 TX (PullUP & Voltage divider)
 * AUX        ----- Herhangi bir LED (Haberlesme esnasinda high seviyededir. Takip icin led bagladim. Istege baglidir.)
 * VCC        ----- 3.3v/5v
 * GND        ----- GND
 * 
 * 
 * 
 * DHT21        ESP32
 * GND    ---   GND
 * VCC    ---   5V
 * SDA    ---   32 (Istege bagli, herhangi bir GPIO secilebilir)
 */

#include "LoRa_E32.h"       // LoRa haberlesme protokolunu kullandigimiz kutuphane.
#include "DHT.h"            // DHT sensorunu kullanmakta yararlandigimiz kutuphane       
#define DHTPIN 32               
#define DHTTYPE DHT21    
DHT dht(DHTPIN, DHTTYPE); 
// RX 14 ---- TX 27
LoRa_E32 e32ttl(&Serial2);  // ESP32 için Hardware serial kullaniyoruz.
double temperature; 
double humidity;

void setup() {
  Serial.begin(9600); 
    Serial2.begin(9600);
    while (!Serial2) {
	    ;
    }
 e32ttl.begin();
 delay(400);
  dht.begin();
  Serial.println(F("\nTemperature and humidity informs from DHT21 sensor via LoRa"));
}
 

void loop() {
  delay(2500);
temperature = dht.readTemperature();  //DHT21 sensorumuzden sicaklik verisini alma
String mystring1;       //Double'dan string'e cevirme
mystring1 = String(temperature);  

humidity = dht.readHumidity();        //DHT21 sensorumuzden nem verisini alma
String mystring2;       //Double'dan string'e cevirme
mystring2 = String(humidity);

String gidendhtbilgileri = "Ortam Sıcaklığı: "+mystring1+"°C\nNem oranı: %"+ mystring2;  // LoRa Receiver'ina gonderdigim mesaj
ResponseStatus rs = e32ttl.sendMessage(gidendhtbilgileri);
Serial.println(rs.getResponseDescription());      //Mesajin gonderildigine ya da neden gonderdilemedigine dair geri donus.
}