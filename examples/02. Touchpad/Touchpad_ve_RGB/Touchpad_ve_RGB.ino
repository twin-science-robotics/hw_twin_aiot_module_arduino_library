/* 
  Touchpad_ve_RGB

  Twin AIoT modülü üzerindeki Touchpad'i dokunmatik bir buton olarak kullanıp RGB LED'i yakabilirsiniz.
  Serial Monitor'den Touchpad'den okunan değeri görebilirsiniz. 
  Eğer okunan değer bir eşiğin üzerinde ise Touchpad'e dokunulmuş demektir, eşik değerinin altında ise parmak dokunmatik yüzeyden çekilmiş anlamına gelmektedir. 
  
  Not : Serial Monitor'ün aktif olması için Tools sekmesinden "USB CDC on Boot" seçeneğini Enabled olarak seçmelisiniz.

  // (R,G,B) değerlerini 0-255 aralığında olacak şekilde istediğiniz gibi ayarlayabilirsiniz
  // Aşağıdan istediğiniz rengi seçebilirisiniz

  renkSec(255, 0, 0);   // Kırmızı
  renkSec(0, 255, 0);   // Yeşil
  renkSec(0, 0, 255);   // Mavi
  renkSec(255, 255, 0); // Sarı
  renkSec(0, 255, 255); // Cyan
  renkSec(255, 0, 255); // Mor
  renkSec(255, 165, 0); // Turuncu
  renkSec(128, 0, 128); // Menekşe
  renkSec(75, 0, 130); // Çivit Mavisi
  renkSec(255, 255, 255); // Beyaz

  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  14 Ağustos 2025
  Twin Science & Robotics
*/

#include <FastLED.h>              // RGB LED kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.

#define TOUCHPAD_THRESHOLD 22000  // Eşik değerini Serial Monitorden gözlemleyip belirleyebilirsiniz
#define TOUCH_PIN 13              // Touchpad'in bağlı olduğu pin numarası

#define RGB_PIN 15                // RGB LED'in bağlı olduğu pin numarası 
#define LED_SAYISI 1              // Modül üzerindeki RGB LED sayısı
#define PARLAKLIK 64              // 0-255 aralığında bir parlaklık ayarlayabilirsiniz

CRGB leds[LED_SAYISI];            // Bir RGB LED nesnesi oluşturunuz.

// reset tuşuna bastığınızda veya kartı açtığınızda kurulum işlevi (setup) bir kez çalışır
void setup() {
  Serial.begin(115200);                                         // Serial Monitor ile haberleşmeyi başlat
  FastLED.addLeds<WS2812B,RGB_PIN, GRB>(leds, LED_SAYISI);      // RGB LED'i tanımla 
  FastLED.setBrightness(PARLAKLIK);                             // Parlaklık ayarı
}

// döngü (loop) fonksiyonu sonsuza kadar tekrar tekrar çalışır
void loop() {
  uint32_t touchpad_degeri = touchRead(TOUCH_PIN);// Touchpad'den okuma yap ve bir değişkene ata
  Serial.print("Touchpad'den okunan değer: ");      
  Serial.println(touchpad_degeri);                // Serial monitorde okunan değeri göster
  Serial.println(" ");                            // Bir satır boşluk

  if (touchpad_degeri > TOUCHPAD_THRESHOLD) {     // Touchpad'e dokunulduğu durum
    Serial.println("Touchpad'e dokundunuz, led açıldı");      
    Serial.println(" ");                          // Bir satır boşluk
    renkSec(255, 165, 0);                         // RGB LED'i Turuncu yak
  }
  else{                                           // Touchpad'e dokunulmadığı durum
    Serial.println("Touchpad'den parmağınızı çektiniz, led kapatıldı.");   
    Serial.println(" ");                          // Bir satır boşluk
    renkSec(0, 0, 0);                             // RGB LED'i söndür
  }
    

  delay(100); // döngü başa sarmadan önce kısa bir bekleme, Touchpad'den değer okuma periodu ayarlanır.

}

void renkSec(uint8_t r, uint8_t g, uint8_t b) {
  leds[0] = CRGB(r, g, b); // LED'e renk ataması
  FastLED.show();          // Renkleri LED'e uygula, ayarlanan RGB değerlerini RGB LED'de göster
}
