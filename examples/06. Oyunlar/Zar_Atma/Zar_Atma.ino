/* 
  Touchpad Zar Oyunu - LED Matrix

  Bu örnek, Twin AIoT modülündeki Touchpad'e her dokunulduğunda
  LED Matrix'te 1-6 arası rastgele bir zar sayısı gösterir.

  RASTGELE SAYI ÜRETME: randomSeed(), rastgele sayı üretecini başlatır. Eğer bunu kullanmazsak,
  kart her açıldığında aynı "rastgele" sayı dizisini üretir (örn: 5, 2, 6, 5, 2, 6...).
  Başlangıç noktası olarak analogRead(0) kullanırız çünkü boş bir analog pin,
  ortamdaki tahmin edilemez elektriksel gürültüyü okur. Bu da bize her seferinde
  gerçekten farklı bir başlangıç noktası ve dolayısıyla farklı bir zar dizisi verir. 

  DEBOUNCE: Dokunmatik yüzeye bir kez dokunduğumuzda, milisaniyeler içinde birden çok
  sahte sinyal (zıplama) oluşabilir. Debounce, bu sahte sinyalleri filtreleyerek
  tek bir dokunuşu tek bir eylem olarak algılamamızı sağlar.

  Not: Serial Monitor'ün aktif olması için Tools sekmesinden "USB CDC on Boot" seçeneğini Enabled olarak seçmelisiniz.

  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  19 Ağustos 2025
  Twin Science & Robotics
*/

// 8x8'lik Led Matrix'i süren shift registerların bağlı bulundukları pinler
#define latchPin 34
#define clockPin 47
#define dataPin 48

// Touchpad
#define TOUCHPAD_THRESHOLD 22000  // Eşik değeri
#define TOUCH_PIN 13              // Touchpad'in bağlı olduğu pin

// Sayı yapısını tanımla
struct LEDMatrixSayi {
  int sayi;             // sayı
  uint8_t patern[8];    // sayınin paterni
};

// Zar sayıları için paternler (1-6)
const LEDMatrixSayi ledMatrixSayilar[] = {
  { 1, {0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00} }, // 1 - orta nokta
  { 2, {0x00, 0x60, 0x60, 0x00, 0x00, 0x06, 0x06, 0x00} }, // 2 - çapraz
  { 3, {0x00, 0x60, 0x60, 0x18, 0x18, 0x06, 0x06, 0x00} }, // 3 - çapraz + orta
  { 4, {0x00, 0x66, 0x66, 0x00, 0x00, 0x66, 0x66, 0x00} }, // 4 - dört köşe
  { 5, {0x00, 0x66, 0x66, 0x18, 0x18, 0x66, 0x66, 0x00} }, // 5 - dört köşe + orta
  { 6, {0x66, 0x66, 0x00, 0x66, 0x66, 0x00, 0x66, 0x66} }  // 6 - altı nokta
};

const size_t sayiSayisi = sizeof(ledMatrixSayilar) / sizeof(LEDMatrixSayi);   // Tanımladığımız toplam zar sayısı

// Değişkenler
int mevcutSayi = 1;                        // Ekranda gösterilen mevcut zar sayısı
bool oncekiDokunmaDurumu = false;          // Dokunmanın durumunu takip eden değişken
unsigned long sonDokunmaZamani = 0;        // Debounce için son dokunma anını saklar
const unsigned long debounceGecikme = 200; // 200ms debounce gecikmesi

void setup() {
  Serial.begin(115200);   // Seri monitörü 115200 baud hızında başlatıyoruz.
  
  // LED Matrix pinlerini çıkış olarak ayarla
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  // Rastgele sayı üreteci için başlangıç noktasını ayarlama
  randomSeed(analogRead(0));
  
  Serial.println("Touchpad Zar Oyunu başlatıldı!");
  Serial.println("Touchpad'e dokunarak zar atın...");
}

void loop() {
  bool suankiDokunmaDurumu = (touchRead(TOUCH_PIN) > TOUCHPAD_THRESHOLD); // Touchpad'den okuma yap ve eşik değeri geçilirse 1 geçilemezse 0 olarak değişkene ata

  // Dokunma anını tespit et
  if (suankiDokunmaDurumu && !oncekiDokunmaDurumu) {
    // Debounce kontrolü: Son dokunmadan bu yana yeterli süre geçti mi?
    // millis(), kartınıza enerji verildiği andan itibaren geçen süreyi milisaniye cinsinden sayan ve hiç durmayan bir kronometre gibidir.
    if (millis() - sonDokunmaZamani > debounceGecikme) {
      mevcutSayi = random(1, 7); // Yeni rastgele sayı üret, 1-6 arası
      
      Serial.print("Zar atıldı! Sonuç: ");
      Serial.println(mevcutSayi);

      sonDokunmaZamani = millis();  // Son dokunma zamanını güncelle
    }
  }

  oncekiDokunmaDurumu = suankiDokunmaDurumu;  // Dokunma durumunu bir sonraki döngü için güncelle 
  
  sayiGoster(mevcutSayi);     // Mevcut sayıyı LED matrix'te göster
  delay(10);                  // Ekran yenileme hızını bu bekleme süresiyle ayarlayabilirsiniz, bekleme süresini artırdıkça -> matrisi yenileme hızını düşürdükçe Led matrix'deki ledlerin titrediğini gözlemleyebilirsiniz.
}

// Led matrix'te gösterilecek sayı için her satırdaki ledleri sırayla çok hızlı şekilde yakar
void sayiGoster(int sayi) {
  const uint8_t* pattern = paterniAl(sayi);
  
  for (int row = 0; row < 8; row++) {
    uint8_t patternRow = pattern[row];
    uint8_t reversedPattern = reverseBits(patternRow);
    uint8_t rowData = (1 << row);
    uint8_t colData = ~reversedPattern;
    shiftOutAll(colData, rowData);
    delay(1);
    shiftOutAll(0x00, 0x00);
  }
}

// Her zar sayısı için patern (dizi) alınır.
const uint8_t* paterniAl(int sayi) {
  for (size_t i = 0; i < sayiSayisi; i++) {
    if (ledMatrixSayilar[i].sayi == sayi) {
      return ledMatrixSayilar[i].patern;
    }
  }
  // Varsayılan olarak 1'i döndür
  return ledMatrixSayilar[0].patern;
}

// Bit sırasını ters çevir (ledlerin dizilişinden dolayı gerekli)
uint8_t reverseBits(uint8_t b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}

// LED matrix'i çalıştır
void shiftOutAll(uint8_t colData, uint8_t rowData) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, colData);
  shiftOut(dataPin, clockPin, MSBFIRST, rowData);
  digitalWrite(latchPin, HIGH);
}
