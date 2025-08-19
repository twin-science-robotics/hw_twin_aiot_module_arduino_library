/*
  Blynk Slider ile Servo Kontrolü

  Bu kod, Blynk uygulamasına eklenen bir slider widget'ı ile Twin AIoT kartına
  bağlı bir servonun açısını kontrol etmenizi sağlar.

  Yapılacaklar
  1. Blynk Cloud üyeliği oluşturun ve giriş yapın.
  2. Blynk konsolunda "Developer Zone'dan" "My Templates" yazan yere gidin ve "+ New Template" ile yeni bir Template oluşturun. Adını istediğiniz gibi verebilirsiniz.
  3. Donanım tipi olarak "ESP32" seçin.
  4. Template oluşturduktan sonra "Datastreams" sekmesine gidin ve buton için yeni bir Datastream oluşturun. (New Datastream --> Virtaul Pin)
  5. Ayarları aşağıdaki gibi yapabilirsiniz.
    - Name: "ServoAci", 
    - Pin: V5, 
    - Data Type: Integer,
    - MIN: 0, MAX: 180, DEFAULT VALUE: 0
  6. "Web Dashboard" sekmesinden dashboard ayarlamasını yapalım. Burada sadece "Slider" yazan widget'ı ekleyin ve widget ayarlarından oluşturduğunuz "ServoAci" adlı değişkeni seçiniz.
  7. Ayarları kaydedin.
  8. Ardından sol menüde yazan "Devices" yazan yere gidin. "New Device" -> "From template" seçeneğini seçin ve az önce oluşturduğunuz şablonu seçin.
  9. Cihazı oluşturduğunuzda otomatik olarak size BLYNK_TEMPLATE_ID, BLYNK_TEMPLATE_NAME, BLYNK_AUTH_TOKEN bilgilerini gösterecektir. Bunları kopyalayın ve koddaki ilgili yerlere yapıştırın.
  10. Kodda gerekli diğer bilgileri doldurunuz. Wifi adı, WiFi şifresi
  
  Not: Aynı template'e Blynk mobil uygulamasından da ulaşabilirsiniz.

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
#include <ESP32Servo.h>                   // ESP32Servo kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.

Servo TwinServo;                          // Servo motorumuza kod içinde bir isim veriyoruz (bir "nesne" oluşturuyoruz).

const char ssid[] = "wifi_isminiz";       // "wifi_isminiz" yazan yere wifi adınızı giriniz.   
const char pass[] = "wifi_sifreniz";      // "wifi_sifreniz" yazan yere wifi şifrenizi giriniz.

const int servoPin = 35;                  // Twin AIoT kartında D6 çıkışının bağlı olduğu pin.
// const int servoPin = 36;               // Twin AIoT kartında D9 çıkışının bağlı olduğu pin. D9 çıkışını kullanmak için bu satırı yorumdan kaldırın.
// const int servoPin = 10;               // Twin AIoT kartında D10 çıkışının bağlı olduğu pin. D10 çıkışını kullanmak için bu satırı yorumdan kaldırın.


BLYNK_WRITE(V5){
  int aci = param.asInt();                // Slider'dan gelen açı değerini (0-180 arası) alıp "aci" değişkenine atıyoruz.

  TwinServo.write(aci);                   // write() fonksiyonu ile servoya gitmesi gereken açı girilir.                  
}


void setup(){

  TwinServo.attach(servoPin);                   // Servo nesnesini bağladığımız fiziksel pine atama işlemi yapılır.
  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);    // Blynk sunucusuna token, ssid ve password bilgileri kullanılarak bağlanılır.
}

void loop() {
  // Blynk.run() komutu, Blynk ile ilgili tüm arka plan işlemlerini yürütür, bağlatıyı canlı tutar, BLYNK_WRITE fonksiyonunu tetikler.
  Blynk.run();
}