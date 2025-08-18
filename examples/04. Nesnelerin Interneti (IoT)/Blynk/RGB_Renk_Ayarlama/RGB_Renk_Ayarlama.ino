/*
  Blynk Slider ile Fan Hızı Kontrolü

  Bu kod, Blynk uygulamasındaki zeRGBa renk seçici widget'ı ile Twin AIoT
  kartındaki RGB LED'in rengini kontrol eder.

  Yapılacaklar
  1. Blynk Cloud üyeliği oluşturun ve giriş yapın.
  2. Blynk konsolunda "Developer Zone'dan" "My Templates" yazan yere gidin ve "+ New Template" ile yeni bir Template oluşturun. Adını istediğiniz gibi verebilirsiniz.
  3. Donanım tipi olarak "ESP32" seçin.
  4. Template oluşturduktan sonra "Datastreams" sekmesine gidin ve buton için yeni bir Datastream oluşturun. (New Datastream --> Virtaul Pin)
  5. Üç tane yeni data oluşturmanız gerekir:
    - İlki kırmızı için sanal pin V2'ye ayarlayın. (Integer 0-255 aralığı)
    - İkincisi yeşil için sanal pin V3'e ayarlayın. (Integer 0-255 aralığı)
    - Üçüncüsü mavi için sanal pin V4'e ayarlayın. (Integer 0-255 aralığı)
  6. Ayarları kaydedin.
  7. Ardından sol menüde yazan "Devices" yazan yere gidin. "New Device" -> "From template" seçeneğini seçin ve az önce oluşturduğunuz şablonu seçin.
  8. Cihazı oluşturduğunuzda otomatik olarak size BLYNK_TEMPLATE_ID, BLYNK_TEMPLATE_NAME, BLYNK_AUTH_TOKEN bilgilerini gösterecektir. Bunları kopyalayın ve koddaki ilgili yerlere yapıştırın.
  9. Kodda gerekli diğer bilgileri doldurunuz. Wifi adı, WiFi şifresi
  10. Mobil uygulamayı açın ve ana sayfadaki oluşturduğunuz cihaza tıklayın.
  11. Ayarlar ikonuna tıklayın ve "+"" yazan yere basın.
  12. Buradan zeRGBa widget'ını ekleyin.
  13. Bu widget'a yukarıda oluşturduğunuz dataları ekleyin.

  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  18 Ağustos 2025
  Twin Science & Robotics

*/

// BLYNK TANIMLAMALARI
// Bu bilgileri Blynk uygulamasında oluşturduğunuz projenin ayarlarından almalısınız.
#define BLYNK_TEMPLATE_ID "TMPLxxxxxx"
#define BLYNK_TEMPLATE_NAME "Deney_adi"
#define BLYNK_AUTH_TOKEN "Blynk_tokeniniz"

#include <WiFi.h>                         // Wifi101 kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.
#include <BlynkSimpleEsp32.h>             // Blynk kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.
#include <FastLED.h>                      // FastLED kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.

#define RGB_PIN 15                        // RGB LED'in bağlı olduğu pin numarası 
#define LED_SAYISI 1                      // Modül üzerindeki RGB LED sayısı
#define PARLAKLIK 64                      // 0-255 aralığında bir parlaklık ayarlayabilirsiniz

CRGB leds[LED_SAYISI];                    // Bir RGB LED nesnesi oluşturunuz.   

const char ssid[] = "wifi_isminiz";       // "wifi_isminiz" yazan yere wifi adınızı giriniz.   
const char pass[] = "wifi_sifreniz";      // "wifi_sifreniz" yazan yere wifi şifrenizi giriniz.

int kirmiziDegeri = 0;         
int yesilDegeri = 0;
int maviDegeri = 0;

void lediGuncelle(){
  leds[0] = CRGB(kirmiziDegeri, yesilDegeri, maviDegeri);   // Gelen değerlere göre LED'e renk ataması yapılır.
  FastLED.show();                                           // Renkleri LED'e uygula, ayarlanan RGB değerlerini RGB LED'de göster
}

// BLYNK FONKSİYONLARI
// Kırmızı değeri V2'den geldiğinde çalışır
BLYNK_WRITE(V2) {
  kirmiziDegeri = param.asInt();
  lediGuncelle();
}

// Yeşil değeri V3'ten geldiğinde çalışır
BLYNK_WRITE(V3) {
  yesilDegeri = param.asInt();
  lediGuncelle();
}

// Mavi değeri V4'ten geldiğinde çalışır
BLYNK_WRITE(V4) {
  maviDegeri = param.asInt();
  lediGuncelle();
}


void setup(){
  FastLED.addLeds<WS2812B,RGB_PIN, GRB>(leds, LED_SAYISI);  // RGB LED'i tanımla 
  FastLED.setBrightness(PARLAKLIK);                         // Parlaklık ayarı

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);                // Blynk sunucusuna token, ssid ve password bilgileri kullanılarak bağlanılır.
}

void loop() {
  // Blynk.run() komutu, Blynk ile ilgili tüm arka plan işlemlerini yürütür, bağlatıyı canlı tutar, BLYNK_WRITE fonksiyonunu tetikler.
  Blynk.run();
}