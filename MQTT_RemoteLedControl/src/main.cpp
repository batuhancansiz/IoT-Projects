/*
MQTT protokolü ile remote sekilde LED kontrolu.
Senaryolar eklenebilir. MQTT Broker burada mesaj yayinlama merkezimiz. 
ESP32 ve ESP8266 gibi WiFi'a baglanabilen mikrokontrolculer ve 
MQTT'nin asenkron olmasi ve dusuk kaynak tuketimi birlesince IoT evreninde basarili isler cikiyor.

Bu projede ESP8266 modulu ve PubsubClient kutuphanesini kullaniyorum.
LED'i herhangi bir GPIO baglayabilirsiniz.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>  //Bu kutuphaneyi kullanarak ESP8266'da MQTT protokolunu kullanabiliyoruz.
// Asagidaki verileri kendi bilgilerimize göre dolduruyoruz.
const char* ssid = "YOUR WIFI SSID";
const char* password = "YOUR WIFI PASSWORD";
const char* mqtt_server = "YOUR MQTT BROKER IP";
#define LED1 2 // led tanimlama.

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() // WiFi agina baglanmamizi saglayan fonksiyon.
{
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
}

//Asagida 2 cesit callback fonksiyonu yazilisi var. Amaciniza gore secebilirsiniz.
// Birincisi ve ikincisi format olarak farklı şekilde bit payload gönderiyor.
void callback(char* topic, byte* payload, unsigned int length) {
   Serial.print("Message has been arrived to topic: [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.print("Message: ");

  String message;
  for (int i = 0; i < length; i++) 
  { message = message + (char)payload[i]; }  // Byte'tan string'e cevirme.
   Serial.print(message);

   //Aşağıdaki "" içini string ile doldurabiliriz. Ac-kapat sinyalini neyle vermek istersek.
  if(message == "0") 
  {
    digitalWrite(LED1,LOW);
    Serial.println("\nLED1 KAPALI"); 
  }
  else if(message == "1") 
  {
    digitalWrite(LED1,HIGH);
    Serial.println("\nLED1 ACIK"); 
  }
  else {Serial.print("\nSadece 0 veya 1'e basiniz");}

  Serial.println();
}

// Bu callback fonksiyonu, farkli topiclerdeki switch kontrolu icin yazilmistir. MQTT broker'da topicler ona gore ayarlanmalidir.
//Tabii ki ayni sekilde giris cikislar ve parametreler de duzeltilmelidir.
/*void callback(char* topic, byte* payload, unsigned int length) {
   Serial.print("Mesaj topic'e ulasti: [");
  Serial.print(topic);
  Serial.print("] ");

  Serial.print("Mesaj: ");

  String message;
  for (int i = 0; i < length; i++) {
    message = message + (char)payload[i];  //Byte'tan string'e cevirme.
  }
   Serial.print(message);
  if(strcmp(topic,"inLight/1023")==0){
    if(message == "ac") 
    {
      digitalWrite(LED1,HIGH);
      Serial.println("\nLED1 acildi");
    }
    else if(message == "kapat") 
    {
      digitalWrite(LED1,LOW);
      Serial.println("\nLED1 kapandi");}
    else
    {Serial.println("\nSadece "ac" ve "kapat" yaziniz");}
  } 
  if(strcmp(topic,"inLight/1024")==0){
    if(message == "ac") 
    {
      digitalWrite(LED2,HIGH);
      Serial.println("\nLED2 acildi");
    }
    else if(message == "kapat") 
    {
      digitalWrite(LED2,LOW);
      Serial.println("\nLED2 kapandi");}
    else
    {Serial.println("\nSadece "ac" ve "kapat" yaziniz");}
  }
  Serial.println();
}
*/

void reconnect() {
  while (!client.connected()) {
    Serial.print("Waiting for MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected!");
      // Once connected, publish an announcement...
      client.publish("DurumTopic", "Baglanildi!"); //Baglaninca DurumTopic adli topic'in bildirim yayinlamasini istedim.
     client.subscribe("inLight");
     //2. callback fonksiyonunu yazdiysak asagidaki topicleri hem yazilimda hem de MQTT broker icinde aktif etmemiz gerekiyor.
     //client.subscribe("inLight/1023");
     //client.subscribe("inLight/1024");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void setup() {
    // put your setup code here, to run once:
  pinMode(LED1, OUTPUT);  //Ledleri cikis olarak ayarlama
  digitalWrite(LED1,LOW);
  Serial.begin(115200);  // Burada tanimladigimizi .ini dosyasında da tanımlayarak baud rate'in uyumlu olmasini saglamamiz lazim.
  setup_wifi();
  client.setServer(mqtt_server, 1883); //Port ayari
  client.setCallback(callback);
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
