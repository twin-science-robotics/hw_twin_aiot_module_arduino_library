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

  Not: Sıcaklık nem sensörünü coding modda olması gerekmektedir. Ayrıca Twin AIoT kartınızdan seçtiğiniz giriş için
  kodda ilgili pinin "DHT_PIN" ayarlamasını yapın. D4 için 8, D8 için 11, D12 için 9 yazın.

  Not : Serial Monitor'ün aktif olması için Tools sekmesinden "USB CDC on Boot" seçeneğini Enabled olarak seçmelisiniz.
  
  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  2 Eylül 2025
  Twin Science & Robotics
*/

#include <WiFi.h>           // Wifi101 kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.
#include <HTTPClient.h>     // ArduinoHttpClient kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.
#include "DHT11.h"          // Herhangi bir kütüphane indirmenize gerek yok.

#define DHT_PIN 8           // D4 için 8, D8 için 11, D12 için 9 yazın.

// Mux pinleri
const int s1_pini = 37;
const int s2_pini = 38;
const int s3_pini = 14;

const char* ssid = "wifi_isminiz";      // "wifi_isminiz" yazan yere wifi adınızı giriniz.
const char* password = "wifi_sifreniz"; // "wifi_sifreniz" yazan yere wifi şifrenizi giriniz.

String url = "sizin_urlniz";  // sizin_urlniz IFTTT Webhook url'nizi buraya yapıştırın.

DHT11 dht11(DHT_PIN); // DHT sensör nesnesi oluştur

// Zaman değişkenleri
unsigned long onceki_zaman = 0;
const unsigned long bekleme_suresi = 60000; // 1 dakika (60000 ms)

// Alarm sınır değerleri
const float max_sicaklik = 25.0;    // Sıcaklık Limiti (°C)
const float max_nem = 50.0;         // Nem Limiti (%)

// Dijital girişi seçimi
void digitalPinSecimi(int secilen_pin) {
  // Önce tüm s pinlerini LOW yaparak resetleyelim
  pinMode(s1_pini, OUTPUT);
  pinMode(s2_pini, OUTPUT);
  pinMode(s3_pini, OUTPUT);
  digitalWrite(s1_pini, LOW);
  digitalWrite(s2_pini, LOW);
  digitalWrite(s3_pini, LOW);

  // Seçilen pine göre doğru s pinini HIGH yap
  switch (secilen_pin) {
    case 8: // D4
      digitalWrite(s1_pini, HIGH);
      break;
    case 11: // D8
      digitalWrite(s2_pini, HIGH);
      break;
    case 9: // D12
      digitalWrite(s3_pini, HIGH);
      break;
  }
}

void setup() {
  Serial.begin(115200); // Seri haberleşmeyi başlat
  Serial.println("\nIFTTT ile Sıcaklık Kontrol");

  pinMode(DHT_PIN, INPUT); // DHT pinini input olarak ayarlıyoruz.
  digitalPinSecimi(DHT_PIN);

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
    float sicaklik = dht11.readTemperature();  // Celsius cinsinden
    float nem = dht11.readHumidity();          // Yüzde cinsinden
    
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
