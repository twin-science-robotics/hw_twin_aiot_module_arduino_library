/* 
  EmojiGoster

  Bu örnek ile, Twin AIoT modülündeki Led Matrix'de birçok emoji gösterebilirsiniz.

  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  22 Ağustos 2025
  Twin Science & Robotics
*/

// 8x8'lik Led Matrix'i süren shift registerların bağlı bulundukları pinler
#define latchPin 34
#define clockPin 47
#define dataPin 48

// Emoji yapısını tanımla
struct LEDMatrixemoji {
  String emoji;  // emojinin kendisi
  uint8_t patern[8]; //  emojinin paterni
};

// Her emoji için belirli bir patern vardır, bu patern bilgisine göre hangi ledin yanıp yanmayacağına karar verilir.
// Aşağı kısımda geçtiği şekliyle istediğiniz emojinin ismini loop() fonksiyonunda kullanabilirsiniz.

const LEDMatrixemoji ledMatrixemojiler[] = {

  { "Mutlu", {0x3C, 0x42, 0xA5, 0x81, 0xA5, 0x99, 0x42, 0x3C}},            // Mutlu yüz ifadesi
  { "Cok mutlu", {0x3C, 0x42, 0xA5, 0x81, 0xBD, 0x99, 0x42, 0x3C}},        // Çok mutlu yüz ifadesi
  { "Uzgun", {0x3C, 0x42, 0xA5, 0x81, 0xBD, 0x81, 0x42, 0x3C}},            // Üzgün yüz ifadesi
  { "Cok uzgun", {0x3C, 0x42, 0xA5, 0x81, 0x99, 0xA5, 0x42, 0x3C}},        // Çok üzgün yüz ifadesi
  { "Saskin", {0x3C, 0x42, 0xA5, 0x81, 0x99, 0x99, 0x42, 0x3C}},           // Şaşkın yüz ifadesi
  { "Pacman", {0x38, 0x44, 0x88, 0x90, 0x88, 0x44, 0x38, 0x00}},           // Pacman
  
  { "Eskenar dortgen", {0x10, 0x28, 0x44, 0x82, 0x44, 0x28, 0x10, 0x00}},  // Eşkenar dörtgen
  { "Cember", {0x3C, 0x42, 0x81, 0x81, 0x81, 0x81, 0x42, 0x3C}},           // Çember
  { "Kare", {0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF}},             // Kare 
  { "Dikdortgen", {0x00, 0xFF, 0x81, 0x81, 0x81, 0x81, 0xFF, 0x00}},       // Dikdörtgen

  { "Acik kilit", {0x00, 0x38, 0x40, 0x44, 0x7C, 0x7C, 0x7C, 0x00}},       // Açık kilit
  { "Kapali kilit", {0x00, 0x38, 0x44, 0x44, 0x7C, 0x7C, 0x7C, 0x00}},     // Kapalı kilit
  { "Tik", {0x00, 0x01, 0x02, 0x44, 0x28, 0x10, 0x00, 0x00}},              // Tik işareti
  { "Soru", {0x3C, 0x42, 0x02, 0x04, 0x08, 0x00, 0x08, 0x00}},             // Soru işareti
  { "Unlem", {0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00}},            // Ünlem işareti
  { "Carpi", {0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81}},            // Çarpı işareti
  { "Pause", {0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00}},            // Pause simgesi 
  { "Play", {0x00, 0x20, 0x30, 0x38, 0x38, 0x30, 0x20, 0x00}},             // Play simgesi
  { "Asagi ok", {0x18, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00}},         // Aşağı ok
  { "Yukari ok", {0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x00}},        // Yukarı ok
  { "Sol ok", {0x00, 0x10, 0x30, 0x7E, 0x7E, 0x30, 0x10, 0x00}},           // Sol ok
  { "Sag ok", {0x00, 0x08, 0x0C, 0x7E, 0x7E, 0x0C, 0x08, 0x00}},           // Sağ ok

  { "Twin", {0x38, 0x44, 0xBA, 0x82, 0xBA, 0x44, 0x38, 0x00}},             // Twin Science logo
  { "Yonca", {0x66, 0x99, 0x81, 0x42, 0x42, 0x81, 0x99, 0x66}},            // Yonca çiçeği
  { "Ay yildiz", {0x00, 0x38, 0x60, 0xC2, 0xC5, 0xC2, 0x60, 0x38}},        // Ay yıldız
  { "Hilal", {0x1C, 0x30, 0x60, 0x60, 0x60, 0x32, 0x1C, 0x00}},            // Hilal 
  { "Yildiz", {0x10, 0x00, 0x10, 0xBA, 0x10, 0x00, 0x10, 0x00}},           // Yıldız 
  { "Dolunay", {0x3C, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C}},          // Dolunay 
  { "Gunes", {0x10, 0x42, 0x18, 0xBC, 0x3D, 0x18, 0x42, 0x10}},            // Güneş 
  { "Kar tanesi", {0x00, 0x10, 0x54, 0x38, 0x54, 0x10, 0x00, 0x00}},       // Kar tanesi 
  { "Parilti", {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55}},          // Parıltı
  { "Kalp", {0x00, 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18}},             // Kalp 
  { "Elmas",{0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x7E, 0x3C, 0x18}},             // Elmas 
  { "Bos", {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}               // Boş
   
  
};

const size_t emojisayisi = sizeof(ledMatrixemojiler) / sizeof(LEDMatrixemoji); // Tanımladığımız toplam emoji sayısı

// reset tuşuna bastığınızda veya kartı açtığınızda kurulum işlevi (setup) bir kez çalışır
void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}


// insan gözünün hareketli görüntüleri akıcı şekilde algılaması için 24 fps (frame per second, saniyedeki çerçeve hızı) yeterlidir
// göstereceğimiz emoji için led matrisi doğru hızda yenilemek gerekir, bu yüzden döngü (loop) fonksiyonu tekrar tekrar çalışır
void loop() {
  emoji_goster("Kalp");  // seçili emojiyi göster, Örnek :Kalp simgesi
  delay(10);             // ekran yenileme hızını bu bekleme süresiyle ayarlayabilirsiniz, bekleme süresini artırdıkça -> matrisi yenileme hızını düşürdükçe Led matrix'deki ledlerin titrediğini gözlemleyebilirsiniz
}


// Led matrix'te gösterilecek emoji için her satırdaki ledleri sırayla çok hızlı şekilde yakar
void emoji_goster(String c){

  const uint8_t* pattern = paterniAl(c);
  
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

// Her emoji için gerekli patern alınır
const uint8_t* paterniAl(String c) {

  for (size_t i = 0; i < emojisayisi; i++) {
    if (ledMatrixemojiler[i].emoji == c) {
      return ledMatrixemojiler[i].patern;
    }
  }
  return ledMatrixemojiler[emojisayisi - 1].patern;
}


// Her ledin yanma durumunu ayarla
uint8_t reverseBits(uint8_t b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}


// Led matrix'i çalıştır
void shiftOutAll(uint8_t colData, uint8_t rowData) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, colData);
  shiftOut(dataPin, clockPin, MSBFIRST, rowData);
  digitalWrite(latchPin, HIGH);
}

