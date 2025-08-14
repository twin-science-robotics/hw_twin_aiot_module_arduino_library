/* 
  Blink

  Twin AIoT modülündeki RGB LED'i bir saniye boyunca AÇIK, ardından bir saniye boyunca KAPALI konuma getirir ve bu işlemi tekrarlar.
  
  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  11 Ağustos 2025
  Twin Science & Robotics
*/

#include <FastLED.h>   // RGB LED kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.

#define RGB_PIN 15     // RGB LED'in bağlı olduğu pin numarası 
#define LED_SAYISI 1   // Modül üzerindeki RGB LED sayısı
#define PARLAKLIK 64   // 0-255 aralığında bir parlaklık ayarla

CRGB leds[LED_SAYISI]; // Bir RGB LED nesnesi oluşturun.


// reset tuşuna bastığınızda veya kartı açtığınızda kurulum işlevi (setup) bir kez çalışır
void setup() {
  FastLED.addLeds<WS2812B,RGB_PIN, GRB>(leds, LED_SAYISI);      // RGB LED'i tanımla 
  FastLED.setBrightness(PARLAKLIK);                             // Parlaklık ayarı
}

// döngü (loop) fonksiyonu sonsuza kadar tekrar tekrar çalışır
void loop() {
  renkSec(0, 255, 0);           // RGB LED'i yeşil yak, (R,G,B) değerlerini 0-255 aralığında olacak şekilde istediğiniz gibi ayarlayabilirsiniz
  delay(1000);                  // 1 saniye bekle
  renkSec(0, 0, 0);             // RGB LED'i söndürmek için (R,G,B) değerlerinin hepsini 0 olarak ayarla
  delay(1000);                  // 1 saniye bekle
}

void renkSec(uint8_t r, uint8_t g, uint8_t b) {
  leds[0] = CRGB(r, g, b); // LED'e renk ataması
  FastLED.show();          // Renkleri LED'e uygula, ayarlanan RGB değerlerini RGB LED'de göster
}
