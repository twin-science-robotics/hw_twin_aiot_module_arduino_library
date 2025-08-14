/* 
  Renk_paleti

  Twin AIoT modülündeki RGB LED'i 16,581,375 farklı renkten istediğiniz renkte ayarlayabilirsiniz.
  
  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  14 Ağustos 2025
  Twin Science & Robotics
*/

#include <FastLED.h>   // RGB LED kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.

#define RGB_PIN 15     // RGB LED'in bağlı olduğu pin numarası 
#define LED_SAYISI 1   // Modül üzerindeki RGB LED sayısı
#define PARLAKLIK 64   // 0-255 aralığında bir parlaklık ayarlayabilirsiniz

CRGB leds[LED_SAYISI]; // Bir RGB LED nesnesi oluşturunuz.


// reset tuşuna bastığınızda veya kartı açtığınızda kurulum işlevi (setup) bir kez çalışır
void setup() {
  FastLED.addLeds<WS2812B,RGB_PIN, GRB>(leds, LED_SAYISI);      // RGB LED'i tanımla 
  FastLED.setBrightness(PARLAKLIK);                             // Parlaklık ayarı

  // (R,G,B) değerlerini 0-255 aralığında olacak şekilde istediğiniz gibi ayarlayabilirsiniz
  // Istediğiniz renk için satır başlarındaki // (yorum satırı) ifadesini kaldırınız

  // renkSec(255, 0, 0);   // Kırmızı
  // renkSec(0, 255, 0);   // Yeşil
  // renkSec(0, 0, 255);   // Mavi
  // renkSec(255, 255, 0); // Sarı
  // renkSec(0, 255, 255); // Cyan
  // renkSec(255, 0, 255); // Mor
  renkSec(255, 165, 0); // Turuncu
  // renkSec(128, 0, 128); // Menekşe
  // renkSec(75, 0, 130); // Çivit Mavisi
  // renkSec(255, 255, 255); // Beyaz
                
}

// döngü (loop) fonksiyonu sonsuza kadar tekrar tekrar çalışır
void loop() {

}

void renkSec(uint8_t r, uint8_t g, uint8_t b) {
  leds[0] = CRGB(r, g, b); // LED'e renk ataması
  FastLED.show();          // Renkleri LED'e uygula, ayarlanan RGB değerlerini RGB LED'de göster
}
