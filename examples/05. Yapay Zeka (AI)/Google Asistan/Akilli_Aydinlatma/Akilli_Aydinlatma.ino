/*
  Google Asistan ile Akıllı Aydınlatma

  Bu kod, Arduino IoT Cloud ve Google Home uygulamalarını kullanarak 
  Twin AIoT modülüne bağlı bir ledi ve üzerindeki RGB ledi akıllı hale getirir.
  
  Yapılacaklar:
  1. İlk olarak Arduino AIoT Cloud hesabı oluşturunuz ve giriş yapınız. https://cloud.arduino.cc/  -> Get Started for free
  2. Sol taraftaki menüden "Things" seçeneğine tıklayınız.
  3. Yeni bir things oluşturunuz.
  4. Açılan sayfada sağ taraftaki "Associated Device" yazan yerdeki bağlantıya tıklayınız.
  5. "SET UP NEW DEVICE" yazan yere basınız. "Third party device" seçin. ESP32'yi ve aşağıdan ESP32S3 Dev Module seçeneğini seçiniz.
  6. Cihazınıza istediğiniz bir ismi veriniz. "DOWNLOAD PDF" e basarak bilgileri kaydediniz. 
     - Buradaki "Device ID" ve "Secret Key" bilgilerini aşağıdaki kodda ilgili yerlere giriniz.
  7. WiFi bilgilerinizi de aşağıdaki ilgili yerlere giriniz.
  8. Ardından yine Arduino AIoT Cloud sayfasında sağ taraftaki bölümden "Smart Home integration" yazan yere basınız ve Google Home bağlantısını yapınız.
  9. Yine aynı sayfadaki Cloud Variables-ADD butonuna basınız ve aşağıdaki değişkenleri ekleyiniz:
    - Name: light, Variable Type: Smart home -> Light, Variable Permission: Read & Write, Variable Update Policy: On change
    - Name: rgb, Variable Type: Smart home -> Colored Light, Variable Permission: Read & Write, Variable Update Policy: On change
  10. Telefonunuza Google Home uygulamasını indiriniz. Giriş yapınız.
  11. Ayarlar bölümünden "Google ile çalışır" a basınız. Aramalara Arduino yazarak hesabınızı bağlayınız.
  12. Cihazlar bölümünden light ve rgb cihazlarını ekleyiniz, cihaz isimlerini 'Ofis lambası' , 'Oturma odası lambası' şeklinde özelleştirebilirsiniz.
  13. Şimdi uygulamadan çıkıp günlük kullanımda "OK, Google Işığı aç" dediğinizde led yanacaktır.
   -veya "OK, Google rgb'yi kırmızıya ayarla" dediğinizde RGB led kırmızı yanacaktır. 
   Bağladığınız LED 'e veya RGB LED'e akıllı evinizdeki bir odayı ayarlar bölümünden atayabilirsiniz. 
   Böylece 'Ofisteki lambayı aç' dediğinizde led yanacaktır veya 'banyodaki lambanın rengini mavi yapar mısın?' dediğinizde RGB leddeki renk  değişimi görebilirsiniz.
   Ayrıca Google Home uygulaması ile de RGB ledin parlaklık ayarını ve renk seçimini yapabilir, led'i açıp kapatabilirsiniz.
   
  Not: Google Asistan ayarlarını Google Gemini olarak kullanmanız önerilir. Telefon ayarlarından değiştirebilirsiniz.

  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  25 Ağustos 2025
  Twin Science & Robotics
*/

#include <ArduinoIoTCloud.h>                    // ArduinoIoTCloud kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.
#include <Arduino_ConnectionHandler.h>          // Arduino_ConnectionHandler kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.
#include <FastLED.h>                            // FastLED kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.
#include <ESP32Servo.h>                         // ESP32Servo kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.

#define RGB_PIN 15                              // RGB LED'in bağlı olduğu pin numarası 
#define LED_SAYISI 1                            // Modül üzerindeki RGB LED sayısı
#define PARLAKLIK 64                            // 0-255 aralığında bir parlaklık ayarlayabilirsiniz

const char SSID[] = "wifi_isminiz";             // "wifi_isminiz" yazan yere wifi adınızı giriniz. 
const char PASS[] = "wifi_sifreniz";            // "wifi_sifreniz" yazan yere wifi şifrenizi giriniz.                 
const char DEVICE_KEY[] = "Secret Key";         // "Secret Key" yazan yere Arduino Cloud'dan aldığınız Secret Key i giriniz.
const char DEVICE_LOGIN_NAME[] = "Device ID";   // "Device ID" yazan yere Arduino Cloud'dan aldığınız Device ID yi giriniz.

CRGB leds[LED_SAYISI];            // Bir RGB LED nesnesi oluşturunuz.

// Bulut Değişkenleri (Google Home Uyumlu)
CloudLight light;                 // Arduino IoT Cloud 'da tanımladığımız light değişkeni
CloudColoredLight rgb;            // Arduino IoT Cloud 'da tanımladığımız rgb değişkeni

const int ledPin = 35;            // Twin AIoT kartında D6 çıkışının bağlı olduğu pin.
// const int ledPin = 36;         // Twin AIoT kartında D9 çıkışının bağlı olduğu pin. D9 çıkışını kullanmak için bu satırı yorumdan kaldırın.
// const int ledPin = 10;         // Twin AIoT kartında D10 çıkışının bağlı olduğu pin. D10 çıkışını kullanmak için bu satırı yorumdan kaldırın.

// "Işık" değişkeni durumu değiştiğinde bu fonksiyon çalışır.
void LedDurumuDegisimi() {
  bool durum = light;             // Işık değişkeninin durum bilgisi alınır.
  digitalWrite(ledPin, durum);    // ledpin'ine bağlı olan led'in yanma durumu, led durum bilgisine göre ayarlanır.
}

// "RGB" değişkeni bilgisi geldiğinde bu fonksiyon çalışır.
void RGBDurumuDegisimi() {
  uint8_t r, g, b;                  // Renk bilgilerini almak için değişken tanımla

  if(rgb.getSwitch()){              // Eğer RGB led açıksa
    rgb.getValue().getRGB(r, g, b); // Renk bilgilerini alma fonksiyonu
    leds[0] = CRGB(r, g, b);        // Gelen değerlere göre LED'e renk ataması yapılır.
    FastLED.show();                 // Renkleri LED'e uygula, ayarlanan RGB değerlerini RGB LED'de göster
  }
  else{
    leds[0] = CRGB(0, 0, 0);        // RGB'yi söndür
    FastLED.show();                 
  }
}

// Değişkenleri ve bağlantıyı başlatan fonksiyon
void ozellikleriBaslat(){
  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(light, READWRITE, ON_CHANGE, LedDurumuDegisimi);
  ArduinoCloud.addProperty(rgb, READWRITE, ON_CHANGE, RGBDurumuDegisimi);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);  // WiFi bağlantı yöneticisi

void setup() {
  pinMode(ledPin, OUTPUT);                                        // LED pinini ÇIKIŞ olarak ayarla
  FastLED.addLeds<WS2812B, RGB_PIN, GRB>(leds, LED_SAYISI);       // FastLED'i başlat
  FastLED.setBrightness(PARLAKLIK);                               // RGB LED için varsayılan parlaklığı ayarla
  ozellikleriBaslat();                                            // Bulut değişkenleri ve bağlantı başlatılır
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);              // Arduino IoT Cloud'a bağlanılır
}

void loop() {
  ArduinoCloud.update();    // Bu komut, bulut ile sürekli iletişimi sağlar.
}