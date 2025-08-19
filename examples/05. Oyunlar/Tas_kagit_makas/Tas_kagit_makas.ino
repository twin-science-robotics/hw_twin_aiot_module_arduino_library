/* 
  Touchpad Taş-Kağıt-Makas Oyunu - LED Matrix

  Bu örnek, Twin AIoT modülündeki Touchpad'e her dokunulduğunda kartın
  rastgele olarak Taş, Kağıt veya Makas seçmesini ve sonucunu LED Matrix'te
  bir simgeyle göstermesini sağlar.

  RASTGELE SAYI ÜRETME: randomSeed(), rastgele sayı üretecini başlatır. Eğer bunu kullanmazsak,
  kart her açıldığında aynı "rastgele" sayı dizisini üretir (örn: 5, 2, 6, 5, 2, 6...).
  Başlangıç noktası olarak analogRead(0) kullanırız çünkü boş bir analog pin,
  ortamdaki tahmin edilemez elektriksel gürültüyü okur. Bu da bize her seferinde farklı bir başlangıç noktası verir.

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

// Simge yapısını tanımla
struct LEDMatrixSimge {
  int id;              // Simgenin id'si
  uint8_t patern[8];   // Simgenin 8x8'lik deseni
};

// Taş, Kağıt, Makas simgeleri için paternler
const LEDMatrixSimge ledMatrixSimgeler[] = {
  { 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} }, // Boş Patern
  { 1, {0x00, 0x00, 0x3C, 0x3C, 0x3C, 0x3C, 0x00, 0x00} }, // Taş
  { 2, {0xFF, 0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF} }, // Kağıt
  { 3, {0x62, 0x64, 0x68, 0x10, 0x10, 0x68, 0x64, 0x62} }  // Makas
};

const size_t simgeSayisi = sizeof(ledMatrixSimgeler) / sizeof(LEDMatrixSimge);    // Tanımladığımız simge sayısı

// Değişkenler
int mevcutSecim = 0;                        // Başlangıçta ekranın boş olması için -1 ile başlatıyoruz.
bool oncekiDokunmaDurumu = false;           // Dokunmanın durumunu takip eden değişken
unsigned long sonDokunmaZamani = 0;         // Debounce için son dokunma anını saklar
const unsigned long debounceGecikme = 200;  // 200ms debounce gecikmesi

void setup() {
  Serial.begin(115200); // Seri monitörü 115200 baud hızında başlatıyoruz.

  // LED Matrix pinlerini çıkış olarak ayarla
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // Rastgele sayı üreteci için başlangıç noktasını ayarlama
  randomSeed(analogRead(0));

  Serial.println("Taş-Kağıt-Makas Oyunu başlatıldı!");
  Serial.println("Oynamak için touchpad'e dokunun...");
}

void loop() {
  bool suankiDokunmaDurumu = (touchRead(TOUCH_PIN) > TOUCHPAD_THRESHOLD);  // Touchpad'den okuma yap ve eşik değeri geçilirse true, geçilemezse false olarak değişkene ata

  // Dokunma anını tespit et
  if (suankiDokunmaDurumu && !oncekiDokunmaDurumu) {
    // Debounce kontrolü: Son dokunmadan bu yana yeterli süre geçti mi?
    // millis(), kartınıza enerji verildiği andan itibaren geçen süreyi milisaniye cinsinden sayan ve hiç durmayan bir kronometre gibidir.
    if (millis() - sonDokunmaZamani > debounceGecikme) {
      mevcutSecim = random(1, 4); // Yeni rastgele seçim üret (1, 2, veya 3)

      Serial.print("Seçim yapıldı! Sonuç: ");
      if(mevcutSecim == 1) {
        Serial.println("TAŞ");
      } else if(mevcutSecim == 2) {
        Serial.println("KAĞIT");
      } else if(mevcutSecim == 3) {
        Serial.println("MAKAS");
      } else {
        Serial.println("Boş patern gösteriliyor");
      }
       
      sonDokunmaZamani = millis(); // Son dokunma zamanını güncelle
    }
  }

  oncekiDokunmaDurumu = suankiDokunmaDurumu; // Dokunma durumunu bir sonraki döngü için güncelle

  simgeGoster(mevcutSecim); // Mevcut seçimi LED matrix'te göster
  delay(10);                // Ekran yenileme hızını bu bekleme süresiyle ayarlayabilirsiniz, bekleme süresini artırdıkça -> matrisi yenileme hızını düşürdükçe Led matrix'deki ledlerin titrediğini gözlemleyebilirsiniz.
}

// Led matrix'te gösterilecek simge için her satırdaki ledleri sırayla çok hızlı şekilde yakar
void simgeGoster(int id) {
  const uint8_t* pattern = paterniAl(id);

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

// Her simge için patern (dizi) alınır.
const uint8_t* paterniAl(int id) {
  for (size_t i = 0; i < simgeSayisi; i++) {
    if (ledMatrixSimgeler[i].id == id) {
      return ledMatrixSimgeler[i].patern;
    }
  }
  // Varsayılan olarak boş paterni döndür
  return ledMatrixSimgeler[0].patern;
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
