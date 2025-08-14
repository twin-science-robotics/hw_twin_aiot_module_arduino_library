/* 
  Touchpad_buton

  Twin AIoT modülü üzerindeki Touchpad'i, buton olarak kullanabilir ve Serial Monitor'den Touchpad'den okunan değeri görebilirsiniz.
  Eğer okunan değer bir eşiğin üzerinde ise Touchpad'e dokunulmuş demektir, eşik değerinin altında ise parmak dokunmatik yüzeyden çekilmiş anlamına gelmektedir. 
  
  Not : Serial Monitor'ün aktif olması için Tools sekmesinden "USB CDC on Boot" seçeneğini Enabled olarak seçmelisiniz.

  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  14 Ağustos 2025
  Twin Science & Robotics
*/

#define TOUCHPAD_THRESHOLD 22000  // Eşik değerini Serial Monitorden gözlemleyip belirleyebilirsiniz
#define TOUCH_PIN 13              // Touchpad'in bağlı olduğu pin numarası

// reset tuşuna bastığınızda veya kartı açtığınızda kurulum işlevi (setup) bir kez çalışır
void setup() {
  Serial.begin(115200);   // Serial Monitor ile haberleşmeyi başlat
}

// döngü (loop) fonksiyonu sonsuza kadar tekrar tekrar çalışır
void loop() {
  uint32_t touchpad_degeri = touchRead(TOUCH_PIN);          // Touchpad'den okuma yap ve bir değişkene ata
  Serial.print("Touchpad'den okunan değer: ");      
  Serial.println(touchpad_degeri);                          // Serial monitorde okunan değeri göster
  Serial.println(" ");                                      // Bir satır boşluk

  if (touchpad_degeri > TOUCHPAD_THRESHOLD) {               // Touchpad'e dokunulduğu durum
    Serial.println("Touchpad'e dokundunuz.");      
    Serial.println(" ");                                    // Bir satır boşluk
  }
  else{                                                     // Touchpad'e dokunulmadığı durum
    Serial.println("Touchpad'den parmağınızı çektiniz.");   
    Serial.println(" ");                                    // Bir satır boşluk
  }
    

  delay(100); // döngü başa sarmadan önce kısa bir bekleme, Touchpad'den değer okuma periodu ayarlanır.

}