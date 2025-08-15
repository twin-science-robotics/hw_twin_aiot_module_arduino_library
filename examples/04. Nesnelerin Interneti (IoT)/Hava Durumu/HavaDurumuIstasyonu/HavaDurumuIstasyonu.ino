/*
  HavaDurumuIstasyonu

  Açıklama:
  Bu örnek, Twin AIoT kartınızı basit bir hava istasyonuna dönüştürür.
  Yerel Wi-Fi ağınıza bağlanır, OpenWeatherMap servisinden anlık hava durumu
  verilerini çeker ve bu bilgileri Serial Monitor'de gösterir.
  
  Yapılacaklar:
  1. https://openweathermap.org/ adresinden OpenWeather'a ücretsiz üye olun.
  2. "My API keys" bölümünden API anahtarınızı alın.
  3. API anahtarları sekmesinde, varsayılan bir anahtar göreceksiniz. 
  Bu siteden bilgi çekmek için ihtiyaç duyacağınız benzersiz bir anahtardır. Bu anahtarı kopyalayıp koddaki "api_anahtariniz" yazan yere yapıştırınız.
  4. Gerekli kütüphaneler yüklü değilse yükleyiniz. Library Manager'dan kütüphane ismini yazarak yükleyebilirsiniz.
  5. Gerekli diğer bilgileri doldurunuz. Wifi adı, WiFi şifresi, İl, Ülke Kodu, birim

  Not : Serial Monitor'ün aktif olması için Tools sekmesinden "USB CDC on Boot" seçeneğini Enabled olarak seçmelisiniz.

  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  14 Ağustos 2025
  Twin Science & Robotics
*/

#include <WiFi.h>         // Wifi101 kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.
#include <HTTPClient.h>   // ArduinoHttpClient kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.
#include <ArduinoJson.h>  // ArduinoJson kütüphanesini ekleyin, yüklü değilse Kütüphane Yöneticisi'nden yükleyin.

// WiFi bilgileri
const char* ssid = "wifi_isminiz";          // "wifi_isminiz" yazan yere wifi adınızı giriniz.
const char* password = "wifi_sifreniz";     // "wifi_sifreniz" yazan yere wifi şifrenizi giriniz.

// OpenWeather API yapılandırması
String apiKey = "api_anahtariniz";          // "api_anahtariniz" yazan yere OpenWeather'dan aldığınız api anahtarını giriniz.
String sehir = "sehir";                     // "sehir" yazan yere yaşadığınız şehri giriniz. (Örnek: Denizli)
String ulkeKodu = "ulke_kodu";              // "ulke_kodu" yazan yere ülke kodunuzu giriniz. (Örnek: Türkiye için "TR")
String birimler = "metric";                 // "santigrat için "metric", Fahrenheit için "imperial" yazınız.

// OpenWeatherMap API isteği için oluşturulan URL
String apiUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + sehir + "," + ulkeKodu + "&appid=" + apiKey + "&units=" + birimler + "&lang=tr";

// Veri güncelleme zamanlayıcısı için değişkenler
const unsigned long guncellemeAraligi = 60000;  // veriyi 1 dakikada bir güncelle (1 dk = 60000ms), buraya istediğiniz aralığı ms cinsinden değiştirebilirsiniz. 
unsigned long sonGuncellemeZamani = 0;

void setup() {
  Serial.begin(115200);             // Seri haberleşmeyi 115200 baud hızında başlatıyoruz.
  delay(1000);                      // Serial port'un düzgün başlaması için 1 saniye gecikme ekliyoruz.
  
  wifiBaglan();                     // Wifi ağına bağlanmak için yazdığımız fonksiyonu çağıyoruz.
  
  havaDurumuVerisiAl();             // Belirlediğimiz süreyi beklemeden ilk hava durumu çağırısını yapıyoruz.
  sonGuncellemeZamani = millis();   // Fonksiyon çalıştıktan sonra son güncelleme zamanını şu anki zamanla güncelliyoruz.
}

void loop() {
  // Belirlediğimiz güncelleme aralığı (1 dakika) geçti mi diye kontrol ediyoruz.
  // millis() fonksiyonu, programın başlangıcından bu yana geçen milisaniye sayısını verir.
  // Bu yöntem, delay() kullanmaktan daha verimlidir çünkü kodun akışını durdurmaz.

  if ((millis() - sonGuncellemeZamani) > guncellemeAraligi) {                 // İlk kontrol: Fonskiyonun son çalışma zamanından sonraki geçen süre belirlediğimiz aralığı geçmiş mi?
    if (WiFi.status() == WL_CONNECTED) {                                      // İkinci kontrol: Hala wifi ağına bağlı mıyız?
      havaDurumuVerisiAl();                                                   // İki kontrolü de geçerse Hava durumu verisini al.
    } else {                                                                  // Wifi ağına bağlı değilse seri monitöre mesaj gönder ve tekrar bağlanmaya çalış.                                                  
      Serial.println("WiFi bağlantısı kesildi. Yeniden bağlanılıyor...");  
      wifiBaglan();                                          
    }
    sonGuncellemeZamani = millis();                                           // Son güncelleme zamanını şu anki zamanla güncelliyoruz.
  }
}

// WiFi bağlantı fonksiyonu
void wifiBaglan() {
  Serial.print("Wi-Fi ağına bağlanılıyor: ");
  
  WiFi.begin(ssid, password);  // Wi-Fi bağlantısını başlatıyoruz.
  int denemeSayisi = 0;
  const int maxDeneme = 20;    // wifi bağlantısı kurulması için maksimum 10 saniye bekliyoruz (20 x 500ms).
  
  // Bağlantı kurulana kadar bekliyoruz ve ekrana nokta koyarak geri bildirim veriyoruz.
  while (WiFi.status() != WL_CONNECTED && denemeSayisi < maxDeneme) {
    delay(500);
    Serial.print(".");
    denemeSayisi++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi bağlantısı başarılı!");
  } else {
    Serial.println("\nWi-Fi'ye bağlanılamadı!");
    Serial.println("Lütfen wifi bilgilerinizi kontrol edip tekrar deneyin.");
  }
}

void havaDurumuVerisiAl() {
  HTTPClient http;                                    // HTTPClient nesnesi oluşturuyoruz. Bu nesne, web isteklerini yönetmemizi sağlar.
  http.begin(apiUrl);                                 // HTTP isteğini başlatıyoruz.
  
  int httpCevapKodu = http.GET();                     // GET isteğini gönderiyoruz ve sunucudan gelen cevap kodunu alıyoruz. // (200: Başarılı, 404: Bulunamadı, vb.)
  
  if (httpCevapKodu == 200) {
    // Successfully got data
    String havaDurumuHamVeri = http.getString();      // Eğer kod başarılıysa hava durumu ham verisini OpenWeather'dan al.
    havaDurumuVerisiniIsle(havaDurumuHamVeri);        // Ham veriyi işleyen fonskiyonu çağırıyoruz.
  } else {
    Serial.print("Hata Kodu: ");                      // Eğer bir hata oluştuysa, hata kodunu ekrana yazdırıyoruz.
    Serial.println(httpCevapKodu);
  }
  
  http.end();                                         // HTTP isteğini sonlandırıp kaynakları serbest bırakıyoruz.
}

void havaDurumuVerisiniIsle(String jsonString) {
  // ArduinoJson kütüphanesini kullanarak bir JSON belgesi nesnesi oluşturuyoruz.
  // 1024 byte, OpenWeather'dan gelen veri için yeterli bir boyuttur.
  DynamicJsonDocument doc(1024);
  DeserializationError hata = deserializeJson(doc, jsonString);  // Gelen metni (String) JSON formatına dönüştürmeyi deniyoruz.
  
  // Eğer JSON'a dönüştürme sırasında bir hata oluşursa fonksiyondan çıkılır.
  if (hata) {
    Serial.print("JSON işleme hatası: ");
    Serial.println(hata.c_str());
    return;
  }
  
  // Verileri JSON belgesinden çekiyoruz.
  String alinanSehir = doc["name"];                                // alınan şehir adı
  float sicaklik = doc["main"]["temp"];                            // sıcaklık değeri
  int nem = doc["main"]["humidity"];                               // nem değeri
  String aciklama = doc["weather"][0]["description"];              // hava durumu açıklaması
  float ruzgarHizi = doc["wind"]["speed"];                         // rüzgar hızı değeri
  
  // Ayıklanan verileri Seri Port Ekranı'na yazdırıyoruz
  Serial.println("\n--- HAVA DURUMU RAPORU ---");
  Serial.print("Şehir: ");
  Serial.println(alinanSehir);              // Şehir bilgisini seri porta yazdır.

  Serial.print("Sıcaklık: ");
  Serial.print(sicaklik);                   // Sıcaklık bilgisini seri porta yazdır.
  if (birimler == "metric") {
    Serial.println(" °C");
  } else {
    Serial.println(" °F");
  }

  Serial.print("Nem: ");
  Serial.print("%");
  Serial.print(nem);                        // Nem bilgisini seri porta yazdır.
  
  Serial.print("\nDurum: ");
  Serial.println(aciklama);                 // Hava durumu açıklama bilgisini seri porta yazdır.

  Serial.print("Rüzgar Hızı: ");
  Serial.print(ruzgarHizi);                 // Rüzgar hızı bilgisini seri porta yazdır.
  Serial.println(" m/s");
  Serial.println("--------------------------");
}