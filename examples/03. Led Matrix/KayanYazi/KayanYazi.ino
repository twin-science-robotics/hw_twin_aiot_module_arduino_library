/* 
  KayanYazi

  Bu örnek, Twin AIoT modülündeki Led Matrix'de istenen ifadeyi kayan yazı şeklinde gösterir.

  Açıklama
  İstediğiniz ifadeyi loop fonksiyonunda bulunan metniKaydir() fonksiyonun içinden değiştirebilirsiniz.
  Yazdırmak istediğiniz metin için 40 karakter limiti vardır. Bu limiti aşılırsa metnin başından 
  itibaren 40 karakter olacak şekilde kırpılır. Bu limiti ilgili yerden değiştirebilirsiniz. 
  Ayrıca türkçe karakter desteklenmemektedir. Bu örneğe 26 harf ve 10 tane rakam paterni eklenmiştir. 
  Dilerseniz farklı karakterler de ekleyebilirsiniz.

  Not: Geçersiz karakter kullanımında boş dizi gösterilir.

  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  26 Ağustos 2025
  Twin Science & Robotics
*/

// 8x8'lik Led Matrix'i süren shift registerların bağlı bulundukları pinler
#define latchPin 34
#define clockPin 47
#define dataPin  48

#define MAX_KARAKTER_LIMITI 40 // Maksimum girilebilecek karakter limiti. Dilerseniz değiştirebilirsiniz

struct LEDMatrixkarakter {
  char karakter;      // karakterin kendisi
  uint8_t patern[8];  // karakterin paterni (dizisi)
};

// Her karakter için belirli bir patern vardır, bu patern bilgisine göre hangi ledin yanıp yanmayacağına karar verilir.
const LEDMatrixkarakter LEDMatrixkarakterler[] = {
  // Harfler
  { 'A', {0x18, 0x24, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x00} },
  { 'B', {0x7C, 0x42, 0x42, 0x7C, 0x42, 0x42, 0x7C, 0x00} },
  { 'C', {0x3C, 0x42, 0x40, 0x40, 0x40, 0x42, 0x3C, 0x00} },
  { 'D', {0x78, 0x44, 0x42, 0x42, 0x42, 0x44, 0x78, 0x00} },
  { 'E', {0x7E, 0x40, 0x40, 0x7C, 0x40, 0x40, 0x7E, 0x00} },
  { 'F', {0x7E, 0x40, 0x40, 0x7C, 0x40, 0x40, 0x40, 0x00} },
  { 'G', {0x3C, 0x42, 0x40, 0x40, 0x46, 0x42, 0x3C, 0x00} },
  { 'H', {0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x00} },
  { 'I', {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00} },
  { 'J', {0x7E, 0x02, 0x02, 0x02, 0x02, 0x42, 0x3C, 0x00} },
  { 'K', {0x42, 0x44, 0x48, 0x70, 0x48, 0x44, 0x42, 0x00} },
  { 'L', {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7E, 0x00} },
  { 'M', {0x42, 0x66, 0x5A, 0x42, 0x42, 0x42, 0x42, 0x00} },
  { 'N', {0x42, 0x62, 0x52, 0x4A, 0x46, 0x42, 0x42, 0x00} },
  { 'O', {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00} },
  { 'P', {0x7C, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40, 0x00} },
  { 'Q', {0x3C, 0x42, 0x42, 0x42, 0x4A, 0x46, 0x3C, 0x00} },
  { 'R', {0x7C, 0x42, 0x42, 0x7C, 0x48, 0x44, 0x42, 0x00} },
  { 'S', {0x3C, 0x42, 0x40, 0x3C, 0x02, 0x42, 0x3C, 0x00} },
  { 'T', {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00} },
  { 'U', {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00} },
  { 'V', {0x42, 0x42, 0x42, 0x42, 0x42, 0x24, 0x18, 0x00} },
  { 'W', {0x42, 0x42, 0x42, 0x42, 0x5A, 0x66, 0x42, 0x00} },
  { 'X', {0x42, 0x42, 0x24, 0x18, 0x24, 0x42, 0x42, 0x00} },
  { 'Y', {0x42, 0x42, 0x42, 0x24, 0x18, 0x18, 0x18, 0x00} },
  { 'Z', {0x7E, 0x02, 0x04, 0x08, 0x10, 0x20, 0x7E, 0x00} },
  
  // Rakamlar
  { '0', {0x3C, 0x46, 0x4A, 0x52, 0x62, 0x42, 0x3C, 0x00} },
  { '1', {0x08, 0x18, 0x28, 0x08, 0x08, 0x08, 0x3E, 0x00} },
  { '2', {0x3C, 0x42, 0x02, 0x0C, 0x30, 0x40, 0x7E, 0x00} },
  { '3', {0x3C, 0x42, 0x02, 0x1C, 0x02, 0x42, 0x3C, 0x00} },
  { '4', {0x04, 0x0C, 0x14, 0x24, 0x7E, 0x04, 0x04, 0x00} },
  { '5', {0x7E, 0x40, 0x7C, 0x02, 0x02, 0x42, 0x3C, 0x00} },
  { '6', {0x3C, 0x40, 0x7C, 0x42, 0x42, 0x42, 0x3C, 0x00} },
  { '7', {0x7E, 0x42, 0x04, 0x08, 0x10, 0x10, 0x10, 0x00} },
  { '8', {0x3C, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x3C, 0x00} },
  { '9', {0x3C, 0x42, 0x42, 0x3E, 0x02, 0x42, 0x3C, 0x00} },
  
  { ' ', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} }  // Boş dizi
};

const size_t karaktersayisi = sizeof(LEDMatrixkarakterler) / sizeof(LEDMatrixkarakter); // Tanımladığımız toplam karakter sayısı

// reset tuşuna bastığınızda veya kartı açtığınızda kurulum işlevi (setup) bir kez çalışır
void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
  metniKaydir("Twin AIoT");   // İstediğimiz metni sonsuz döngüde kaydırıyoruz. Buraya kaydırmak istediğiniz metni giriniz.
  delay(500);                 // Kaydırılma bittikten sonra tekrar başlamadan önce yarım saniye bekleme
}

// Led matrix'te gösterilecek karakteri belirli bir süre boyunca gösterir. Bunun için her satırdaki ledleri sırayla çok hızlı şekilde yakar. 
void karakteri_goster(const uint8_t* frame, int sureMs) {
  unsigned long start = millis();
  while (millis() - start < sureMs) {
    for (int row = 0; row < 8; row++) {
      uint8_t rowData = (1 << row);
      uint8_t colData = ~frame[row];
      shiftOutAll(colData, rowData);
      delay(1);
      shiftOutAll(0x00, 0x00);
    }
  }
}

// Her karakter için gerekli patern alınır
const uint8_t* paterniAl(char c) {
  for (size_t i = 0; i < karaktersayisi; i++) {
    if (LEDMatrixkarakterler[i].karakter == c) {
      return LEDMatrixkarakterler[i].patern;
    }
  }
  return LEDMatrixkarakterler[karaktersayisi - 1].patern;
}

// Verilen metni LED matrix üzerinde akıcı bir şekilde soldan sağa kaydırır.
void metniKaydir(String metin) {
  if (metin.length() > MAX_KARAKTER_LIMITI) {
    metin = metin.substring(0, MAX_KARAKTER_LIMITI); // Eğer aşıyorsa, metnin başından limite kadar kırpılır
  }

  String tamMetin = " " + metin + " "; // Akıcı giriş ve çıkış için metnin başına ve sonuna boşluk ekleme.
  tamMetin.toUpperCase();              // Tüm metni büyük harfe çevir. Küçük harfleri göstermek için uygun paterni tanımladıktan sonra burayı yoruma alabilirsiniz.

  // Animasyon Döngüsü
  for (int i = 0; i < tamMetin.length() - 1; i++) {
    const uint8_t* tamMetinmevcutKarakterDeseni = paterniAl(tamMetin[i]);
    const uint8_t* sonrakiKarakterDeseni = paterniAl(tamMetin[i+1]);

    for (int shift = 0; shift < 8; shift++) {
      uint8_t buffer[8];
      for (int row = 0; row < 8; row++) {
        uint8_t mevcutPart = reverseBits(tamMetinmevcutKarakterDeseni[row]) >> shift;
        uint8_t sonrakiPart = reverseBits(sonrakiKarakterDeseni[row]) << (8 - shift);
        buffer[row] = mevcutPart | sonrakiPart;
      }
      karakteri_goster(buffer, 100); // Girilen metnin kayma hızı, hızlandırmak için süreyi azaltabilirsiniz.
    }
  }
}

// Bit sırasını ters çevir (ledlerin dizilişinden dolayı gerekli)
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