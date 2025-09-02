/*
  IFTTT ile Sıcaklık Alarmı
  
  Bu örnek Twin AIoT kartı ile sıcaklık ve nem sensörünü kullanarak belirlediğiniz alarm durumunda
  IFTTT (If This Then That) servisine veri gönderir. Belirli sıcaklık veya nem 
  değerlerine ulaşıldığında IFTTT mobil uygulaması üzerinden bildirim alırsınız.

  YAPILACAKLAR:
  1. IFTTT hesabı oluşturun ve giriş yapın. (https://ifttt.com)
  2. "Create" butonuna tıklayın
  3. "If This" kısmında "Webhooks" servisini seçin. Webhooks servisi için pro hesabı gerektirmektedir. 7 günlük ücretsiz kullanabilirsiniz.
  4. "Receive a web request" seçin ve Connect' e basın.
  5. Event Name: "sicaklik_alarm" yazın ve Create trigger'a basın. (Kodla aynı olmalı)
  6. "Then That" kısmında Notifications eylemini seçin.
  7. "Send a notification from the IFTTT app" seçin. Connect'e basın.
  8. Message bölümüne aşağıdaki ifadeyi yapıştırabilirsiniz veya istediğiniz şekilde düzenleyebilirsiniz:
  
  ⚠️ Ortam Alarmı! 🌡️
  Sıcaklık: {{Value1}}°C
  Nem: {{Value2}}%
  Lütfen ortamı kontrol edin.

  9. Create action butonuna tıklayın.
  10. Continue butonuna tıklayın ardından finish'e basın.
  11. Oluşturduğunuz eventin altında webhook url'niz gösterilecektir. Bu urlyi kopyalayın kodda ilgili yere yepıştırın.
  12. IFTTT mobil uygulamasını indirin ve hesabınızla giriş yapın.
  13. Kodu yüklediğinizde 60 saniyede bir kontrol edilip telefonunuza bildirim gönderilecektir.

  DONANIM:
  Multiplexer (Çoklayıcı) Nedir?
  Multiplexer, birden fazla dijital sinyal hattından (kanal) hangisinin kullanılacağını
  seçmemizi sağlayan bir dijital anahtar gibidir. Twin AIoT kartındaki D4, D8, D12
  girişlerini aktif etmek için kullanılır. Bu pinler başlangıçta analog modda kullanılmaktadır.

  'S' pinleri (s1, s2, s3), multiplexer'ın "düğmeleri" gibidir. Bu pinlere HIGH (1)
  veya LOW (0) sinyali göndererek girişin analog mu dijital mi olacağını seçeriz. Bu projede,
  DHT11 sensörü dijital giriş kullanacağından dolayı D4 dijital girişini aktif etmek için `s1_pini` HIGH durumuna
  getirilmektedir.

  Twin AIoT Kartında kullanmak istediğiniz girişe göre aşağıdaki pin ayarlamalarını yapabilirsiniz.

  D4 için digital pin: 8 --> D4 için s1 pin: 37
  D8 için digital pin: 11 --> D8 için s2 pin: 38
  D12 için digital pin: 9 --> D12 için s3 pin: 14

  Not: Sıcaklık nem sensörünü coding modda olması gerekmektedir.

  Not : Serial Monitor'ün aktif olması için Tools sekmesinden "USB CDC on Boot" seçeneğini Enabled olarak seçmelisiniz.
  
  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  2 Eylül 2025
  Twin Science & Robotics
*/

#include <WiFi.h>           // Wifi101 kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.
#include <HTTPClient.h>     // ArduinoHttpClient kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.
#include <DHT.h>            // DHT sensor library by Adafruit kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.

// DHT11 sensör ayarları
#define DHT_PIN 8      // DHT11 pini
#define DHT_TYPE DHT11 // Sensör tipi: DHT11

const char* ssid = "wifi_isminiz";      // "wifi_isminiz" yazan yere wifi adınızı giriniz.
const char* password = "wifi_sifreniz"; // "wifi_sifreniz" yazan yere wifi şifrenizi giriniz.

const int s1_pini = 37; // Multiplexer s1 pini

DHT dht(DHT_PIN, DHT_TYPE); // DHT sensör nesnesi oluştur

String url = "sizin_urlniz";  // IFTTT Webhook url'nizi buraya yapıştırın.

// Zaman değişkenleri
unsigned long onceki_zaman = 0;
const unsigned long bekleme_suresi = 60000; // 1 dakika (60000 ms)

// Alarm sınır değerleri
const float max_sicaklik = 25.0;    // Sıcaklık Limiti (°C)
const float max_nem = 50.0;         // Nem Limiti (%)

void setup() {
  Serial.begin(115200); // Seri haberleşmeyi başlat
  Serial.println("\nIFTTT ile Sıcaklık Kontrol");

  pinMode(s1_pini, OUTPUT);     // s1 pinini output olarak ayarlıyoruz.
  pinMode(DHT_PIN, INPUT);      // DHT pinini input olarak ayarlıyoruz.
  digitalWrite(s1_pini, HIGH);  // D4 kullanacağımız için s1 pinini HIGH yapıyoruz. 
  
  dht.begin(); // DHT sensörünü başlat

  WiFi.begin(ssid, password); // WiFi bağlantısını başlat

  // WiFi bağlantısı kurulana kadar bekle
  int deneme_sayisi = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    deneme_sayisi++;
    if (deneme_sayisi > 30) {
      Serial.println("\nWiFi baglantisi kurulamadi!");
      break;
    }
  }
}

void loop() {
  olcumYapVeKontrolEt();
}

// Sensörden veri okuma, kontrol etme ve alarm gönderme işlemlerini yapan fonksiyon
void olcumYapVeKontrolEt() {
  unsigned long simdiki_zaman = millis(); // Mevcut zamanı al
  
  // Belirlenen bekleme süresi dolunca ölçüm yap
  if (simdiki_zaman - onceki_zaman >= bekleme_suresi) {
    onceki_zaman = simdiki_zaman;
    
    // DHT11'den sıcaklık ve nem oku
    float sicaklik = dht.readTemperature();  // Celsius cinsinden
    float nem = dht.readHumidity();          // Yüzde cinsinden
    
    // Ölçüm sonuçlarını seri port'a yazdır
    Serial.println("--------------------");
    Serial.print("Sıcaklık: ");
    Serial.print(sicaklik);
    Serial.println(" °C");
    Serial.print("Nem: ");
    Serial.print(nem);
    Serial.println(" %");
    
    // Alarm durumunu kontrol et
    if (sicaklik > max_sicaklik || nem > max_nem) {
      Serial.println("ALARM: Sinir degerler asildi! IFTTT'ye bildiriliyor...");
      ifttt_gonder(sicaklik, nem);
    } else {
      Serial.println("Degerler normal sinirlar icerisinde.");
    }
  }
}

// IFTTT'ye HTTP POST isteği gönderen fonksiyon
void ifttt_gonder(float sicaklik, float nem) {
  // WiFi bağlantısını kontrol et
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    
    // JSON veri formatı oluştur
    String json_data = "{";
    json_data += "\"value1\":\"" + String(sicaklik) + "\",";
    json_data += "\"value2\":\"" + String(nem) + "\"";
    json_data += "}";
    
    http.POST(json_data);   // HTTP POST isteği gönder
    http.end();             // HTTP bağlantısını kapat
    
  } else {
    Serial.println("WiFi bağlantısı yok! IFTTT'ye mesaj gönderilemedi.");
  }
}