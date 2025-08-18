/*
  Blynk Buton ile LED Kontrolü

  Bu kod, Blynk uygulamasına eklenen bir buton widget'ı ile Twin AIoT kartına
  bağlı bir LED'i kontrol etmenizi sağlar.

  Yapılacaklar
  1. Blynk Cloud üyeliği oluşturun ve giriş yapın.
  2. Blynk konsolunda "Developer Zone'dan" "My Templates" yazan yere gidin ve "+ New Template" ile yeni bir Template oluşturun. Adını istediğiniz gibi verebilirsiniz.
  3. Donanım tipi olarak "ESP32" seçin.
  4. Template oluşturduktan sonra "Datastreams" sekmesine gidin ve buton için yeni bir Datastream oluşturun. (New Datastream --> Virtaul Pin)
  5. Name: "Buton" koyabilirsiniz, pin olarak V0'dan V255'e kadar istediğinizi seçebilirsiniz. Biz bu kodda V0 kullanılmıştır.
  6. Buton(switch) deneyi için "Data Type: Integer, MIN: 0, MAX:1, DEFAULT VALUE: 0" şeklinde yapılandırma ayarlarını tamamlayınız.
  7. "Web Dashboard" sekmesinden dashboard ayarlamasını yapalım. Burada sadece "Switch" yazan widget'ı ekleyin ve Buton widget ayarlarından seçtiğiniz "Buton" adlı değişkeni seçiniz.
  8. Ayarları kaydedin.
  9. Ardından sol menüde yazan "Devices" yazan yere gidin. "New Device" -> "From template" seçeneğini seçin ve az önce oluşturduğunuz şablonu seçin.
  10. Cihazı oluşturduğunuzda otomatik olarak size BLYNK_TEMPLATE_ID, BLYNK_TEMPLATE_NAME, BLYNK_AUTH_TOKEN bilgilerini gösterecektir. Bunları kopyalayın ve koddaki ilgili yerlere yapıştırın.
  11. Gerekli diğer bilgileri doldurunuz. Wifi adı, WiFi şifresi
  12. Aynı template'e Blynk mobil uygulamasıyla ulaşabilirsiniz.

  Not : Serial Monitor'ün aktif olması için Tools sekmesinden "USB CDC on Boot" seçeneğini Enabled olarak seçmelisiniz.

  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  18 Ağustos 2025
  Twin Science & Robotics

*/

// BLYNK TANIMLAMALARI
// Bu bilgileri Blynk uygulamasında oluşturduğunuz projenin ayarlarından almalısınız.
#define BLYNK_TEMPLATE_ID "TMPLxxxxxx"      // Buraya template id yapıştırın.
#define BLYNK_TEMPLATE_NAME "Deney adı"     // Buraya template adınızı yapıştırın.
#define BLYNK_AUTH_TOKEN "Blynk_tokeniniz"  // Buraya blnyk token yapıştırın.

// #define BLYNK_PRINT Serial               // Blynk'in seri port üzerinden hata ayıklama mesajları göndermesi için kullanılır. İsterseniz yorumdan kaldırabilirsiniz.      

#include <WiFi.h>                           // Wifi101 kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.
#include <BlynkSimpleEsp32.h>               // Blynk kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.

const char ssid[] = "wifi_isminiz";         // "wifi_isminiz" yazan yere wifi adınızı giriniz.   
const char pass[] = "wifi_sifreniz";        // "wifi_sifreniz" yazan yere wifi şifrenizi giriniz.

const int ledPin = 35;      // Twin AIoT kartında D6 çıkışının bağlı olduğu pin.
// const int ledPin = 36;   // Twin AIoT kartında D9 çıkışının bağlı olduğu pin. D9 çıkışını kullanmak için bu satırı yorumdan kaldırın.
// const int ledPin = 10;   // Twin AIoT kartında D10 çıkışının bağlı olduğu pin. D10 çıkışını kullanmak için bu satırı yorumdan kaldırın.

// Bu özel fonksiyon, Blynk uygulamasındaki V0 pinine bağlı butona her
// dokunulduğunda Blynk kütüphanesi tarafından OTOMATİK olarak çağrılır.
BLYNK_WRITE(V0){
  int pinDegeri = param.asInt();                      // param.asInt() komutu, butondan gelen değeri (0 veya 1) alır ve pinDegeri adında bir tamsayı değişkenine atar.
  Serial.print("\nSanal Pin V0'dan gelen değer: ");  
  Serial.print(pinDegeri);                            // Gelen değer seri monitörde gösterilir.
  digitalWrite(ledPin, pinDegeri);                    // digitalWrite fonksiyonu ledin bağlı olduğu pine gelen değere göre HIGH(1) veya LOW(0) sinyali gönderir.
}


void setup(){
  Serial.begin(115200);                             // Seri monitör başlatılır.
  delay(100);                                       // Seri portun stabil hale gelmesi için kısa bir bekleme.

  pinMode(ledPin, OUTPUT);                          // LED'in bağlı olduğu pin pinMode ile ÇIKIŞ (OUTPUT) olarak ayarlanır.

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);        // Blynk sunucusuna token, ssid ve password bilgileri kullanılarak bağlanılır.
}

void loop() {
  // Blynk.run() komutu, Blynk ile ilgili tüm arka plan işlemlerini yürütür:
  // - Sunucu ile bağlantıyı canlı tutar.
  // - Uygulamadan gelen yeni komutları (buton basılması gibi) kontrol eder.
  // - Gerekli BLYNK_WRITE fonksiyonlarını tetikler.
  Blynk.run();
}