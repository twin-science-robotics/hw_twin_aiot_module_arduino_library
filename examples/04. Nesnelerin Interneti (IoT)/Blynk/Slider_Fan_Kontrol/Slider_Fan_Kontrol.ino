/*
  Blynk Slider ile Fan Hızı Kontrolü

  Bu kod, Blynk uygulamasına eklenen bir slider widget'ı ile Twin AIoT kartına
  bağlı bir fanı kontrol etmenizi sağlar.

  PWM (Pulse Width Modulation): PWM, Türkçeye "Sinyal Genişlik Modülasyonu" olarak çevrilebilir. 
  En basit haliyle PWM, dijital bir sistemin (sadece AÇIK ve KAPALI durumlarını bilen bir bilgisayarın), 
  sanki analogmuş gibi davranarak ara değerler üretme tekniğidir. Bunu, bir ışık düğmesini çok hızlı açıp kapatmaya benzetebiliriz.
  Düğmeyi uzun süre AÇIK, kısa süre KAPALI tutarsak ışık parlak görünür. Kısa süre
  AÇIK, uzun süre KAPALI tutarsak ışık loş görünür.
  Bu kodda, Twin AIoT kartı fana giden gücü saniyede binlerce kez çok hızlı açıp kapatarak
  fanın hızını 0 (duruyor) ile 255 (tam hız) arasında hassas bir şekilde ayarlar.

  Yapılacaklar
  1. Blynk Cloud üyeliği oluşturun ve giriş yapın.
  2. Blynk konsolunda "Developer Zone'dan" "My Templates" yazan yere gidin ve "+ New Template" ile yeni bir Template oluşturun. Adını istediğiniz gibi verebilirsiniz.
  3. Donanım tipi olarak "ESP32" seçin.
  4. Template oluşturduktan sonra "Datastreams" sekmesine gidin ve buton için yeni bir Datastream oluşturun. (New Datastream --> Virtaul Pin)
  5. Name: "FanHızı" koyabilirsiniz, pin olarak V0'dan V255'e kadar istediğinizi seçebilirsiniz. Bu kodda V1 kullanılmıştır.
  6. FanHızı değişkeni için "Data Type: Integer, MIN: 0, MAX:100, DEFAULT VALUE: 0" şeklinde yapılandırma ayarlarını tamamlayabilirsiniz.
  7. "Web Dashboard" sekmesinden dashboard ayarlamasını yapalım. Burada sadece "Slider" yazan widget'ı ekleyin ve widget ayarlarından oluşturduğunuz "FanHızı" adlı değişkeni seçiniz.
  8. Ayarları kaydedin.
  9. Ardından sol menüde yazan "Devices" yazan yere gidin. "New Device" -> "From template" seçeneğini seçin ve az önce oluşturduğunuz şablonu seçin.
  10. Cihazı oluşturduğunuzda otomatik olarak size BLYNK_TEMPLATE_ID, BLYNK_TEMPLATE_NAME, BLYNK_AUTH_TOKEN bilgilerini gösterecektir. Bunları kopyalayın ve koddaki ilgili yerlere yapıştırın.
  11. Kodda gerekli diğer bilgileri doldurunuz. Wifi adı, WiFi şifresi
  
  Not: Aynı template'e Blynk mobil uygulamasından da ulaşabilirsiniz.

  Not: Serial Monitor'ün aktif olması için Tools sekmesinden "USB CDC on Boot" seçeneğini Enabled olarak seçmelisiniz.

  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  18 Ağustos 2025
  Twin Science & Robotics

*/

// BLYNK TANIMLAMALARI
// Bu bilgileri Blynk uygulamasında oluşturduğunuz projenin ayarlarından almalısınız.
#define BLYNK_TEMPLATE_ID "TMPLxxxxxx"
#define BLYNK_TEMPLATE_NAME "Deney adı"
#define BLYNK_AUTH_TOKEN "Blynk_tokeniniz"

// #define BLYNK_PRINT Serial               // Blynk'in seri port üzerinden hata ayıklama mesajları göndermesi için kullanılır. İsterseniz yorumdan kaldırabilirsiniz.      

#include <WiFi.h>                           // Wifi101 kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.
#include <BlynkSimpleEsp32.h>               // Blynk kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.

const char ssid[] = "wifi_isminiz";         // "wifi_isminiz" yazan yere wifi adınızı giriniz.   
const char pass[] = "wifi_sifreniz";        // "wifi_sifreniz" yazan yere wifi şifrenizi giriniz.

const int fanPin = 35;                      // Twin AIoT kartında D6 çıkışının bağlı olduğu pin.
// const int fanPin = 36;                   // Twin AIoT kartında D9 çıkışının bağlı olduğu pin. D9 çıkışını kullanmak için bu satırı yorumdan kaldırın.
// const int fanPin = 10;                   // Twin AIoT kartında D10 çıkışının bağlı olduğu pin. D10 çıkışını kullanmak için bu satırı yorumdan kaldırın.

// PWM Ayarları
const int pwmFrekansi = 500;
const int pwmCozunurlugu = 8;


// Bu özel fonksiyon, Blynk uygulamasındaki V1 pinine bağlı slider değerini
// her değiştirdiğinizde Blynk kütüphanesi tarafından OTOMATİK olarak çağrılır.
BLYNK_WRITE(V1){
  int fanHiziDegeri = param.asInt();                        // param.asInt() komutu, sliderdan gelen değeri (0-100 arası) alır ve fanHiziDegeri adında bir tamsayı değişkenine atar.
  Serial.print("\nSanal Pin V1'dan gelen değer: ");  
  Serial.print(fanHiziDegeri);
  
  int pwmDegeri = map(fanHiziDegeri, 0, 100, 0, 255);       // Gelen 0-100 aralığındaki değeri 0-255 aralığına dönüştürüyoruz.
  ledcWrite(fanPin, pwmDegeri);                             // ledcWrite fonksiyonu esp32 için kullanılan PWM sinyali üretmek için kullanılır. AnalogWrite() gibi düşünebilirsiniz.
  Serial.print("\nDönüştürülmüş PWM Değeri: ");  
  Serial.print(pwmDegeri);
}


void setup(){
  Serial.begin(115200);
  delay(100);

  ledcAttach(fanPin, pwmFrekansi, pwmCozunurlugu);
  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);                // Blynk sunucusuna token, ssid ve password bilgileri kullanılarak bağlanılır.
}

void loop() {
  // Blynk.run() komutu, Blynk ile ilgili tüm arka plan işlemlerini yürütür, bağlatıyı canlı tutar, BLYNK_WRITE fonksiyonunu tetikler.
  Blynk.run();
}