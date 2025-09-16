/*
  Flappy Bird

  Bu örnek, Twin AIoT modülündeki Touchpad ve Led Matrix ile temel seviyede flappy bird 
  oynanabilmesi için geliştirilmiştir.

  Oyunda touchpad'e basarak kuşun engeller arası boşluklardan geçirilmesi amaçlanır. Kuşun hareketi sadece
  dikey olarak yapılır. Touchpad'e basıldığında zıplar, basılmadığı durumda ise yer çekimi efektiyle yere düşer.
  Engeller gittikçe hızlanır ve oyun zorlaşır. Engellerin arası uzaklık da değişkenlik gösterir. 
  Bu şekilde oyunun stabil kalmaması sağlanır. Oyuncunun skoru yani kaç engelden geçtiği serial monitorde ve oyun bittiğinde led matrixte gösterilir.

  Not: Serial Monitor'ün aktif olması için Tools sekmesinden "USB CDC on Boot" seçeneğini Enabled olarak seçmelisiniz.

  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  02 Eylül 2025
  Twin Science & Robotics
*/

// 8x8'lik Led Matrix'i süren shift registerların bağlı bulundukları pinler
#define latchPin 34
#define clockPin 47
#define dataPin  48

// Touchpad
#define TOUCHPAD_THRESHOLD 22000  // Eşik değeri
#define TOUCH_PIN 13              // Touchpad'in bağlı olduğu pin

#define MAX_KARAKTER_LIMITI 40

// Oyun Ayarları
const float GRAVITY = 0.5;                // Kuşun aşağı doğru ne kadar hızlı çekileceğini belirler. Yüksek değer = daha hızlı düşüş.
const float JUMP_FORCE = -1.4;            // Kuşun zıplama kuvveti. Negatif olması, Y ekseninde yukarı gitmesini sağlar.
const int BIRD_X_POS = 1;                 // Kuşun ekrandaki sabit yatay (X) konumu (0-7 arası).
const int PIPE_GAP = 3;                   // Engellerin arasındaki dikey boşluğun boyutu (LED sayısı).
const int BIRD_PHYSICS_SPEED = 120;       // Kuşun fizik motorunun ne sıklıkla güncelleneceğini belirler. Düşük değer = daha akıcı hareket.

// Engel Ayarları
const int INITIAL_PIPE_SPEED = 220;       // Engellerin başlangıç hızı (ms). Yüksek değer = yavaş başlangıç.
const int MINIMUM_PIPE_SPEED = 60;        // Engellerin ulaşabileceği en yüksek hız (ms). Düşük değer = çok hızlı.
const int SPEED_INCREASE_AMOUNT = 15;     // Her hızlanmada bekleme süresinden kaç ms düşüleceği.
const int SCORE_TO_SPEED_UP = 3;          // Kaç puanda bir oyunun hızlanacağı.

// Çoklu engel ayarları
const int NUM_PIPES = 2;                  // Ekranda yönetilecek toplam engel sayısı.
const int PIPE_SPACING = 5;               // Engeller arasındaki NORMAL yatay boşluk (piksel).
const int SINGLE_PIPE_CHANCE = 50;        // % kaç ihtimalle engeller arasında uzun boşluk olacağı.
const int WIDE_SPACING_MULTIPLIER = 2;    // Uzun boşluğun, normal boşluğun kaç katı olacağı.

// Oyun Değişkenleri
float birdY = 3.0;                        // Kuşun dikey (Y) konumu. Pürüzsüz hareket için ondalıklı (float) sayıdır.
float birdVelocity = 0.0;                 // Kuşun anlık dikey hızı. Yerçekimi ile artar, zıplama ile azalır.
int pipeX[NUM_PIPES];                     // Engellerin yatay (X) konumlarını tutan dizi. Her eleman bir engeli temsil eder.
int pipeGapY[NUM_PIPES];                  // Engellerin boşluklarının dikey (Y) konumlarını tutan dizi.
int score = 0;                            // Oyuncunun mevcut skoru.
bool gameOver = true;                     // Oyunun bitip bitmediğini kontrol eden bayrak (flag). Başlangıçta 'true' başlar.
int currentPipeSpeed;                     // Engellerin o anki hızını (ms bekleme süresi) tutan değişken. Oyun ilerledikçe değeri düşer.
bool isGameStarting = false;              // Oyunun başlangıç anındaki gecikme durumunu kontrol eden bayrak.

// Zamanlayıcılar
unsigned long lastBirdUpdateTime = 0;     // Kuş hareketinin en son güncellendiği zaman.
unsigned long lastPipeMoveTime = 0;       // Engellerin en son hareket ettiği zaman.
unsigned long lastTouchTime = 0;          // En son dokunma algılandığı zaman.
const unsigned long touchDebounce = 150;  // Dokunma sinyali sıçramalarını önlemek ve art arda basmaları engellemek için gereken bekleme süresi (ms).
const int INITIAL_PIPE_DELAY = 1000;      // Engellerin oyuna dahil olmadan önceki bekleme süresi (ms)

struct LEDMatrixkarakter {
  char karakter;      // karakterin kendisi
  uint8_t patern[8];  // karakterin paterni (dizisi)
};

// Her karakter için belirli bir patern vardır, bu patern bilgisine göre hangi ledin yanıp yanmayacağına karar verilir.
const LEDMatrixkarakter LEDMatrixkarakterler[] = {
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

const size_t karaktersayisi = sizeof(LEDMatrixkarakterler) / sizeof(LEDMatrixkarakter);

uint8_t screen[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void setup() {
  Serial.begin(115200);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  randomSeed(analogRead(0));
  Serial.println("Flappy Bird Oyunu Başlatıldı!");
  Serial.println("Oyunu başlatmak veya zıplamak için Touchpad'e dokunun.");
}

void loop() {
  // Dokunma Kontrolü
  bool isTouching = (touchRead(TOUCH_PIN) > TOUCHPAD_THRESHOLD);
  if (isTouching && (millis() - lastTouchTime > touchDebounce)) {
    lastTouchTime = millis();
    if (gameOver) {
      resetGame();
    } else {
      birdVelocity = JUMP_FORCE;
    }
  }

  // Kuş Hareketi Güncellemesi
  if (!gameOver && (millis() - lastBirdUpdateTime > BIRD_PHYSICS_SPEED)) {
    lastBirdUpdateTime = millis();
    updateBird();
  }

  // Engel Hareketi Güncellemesi
  if (!gameOver) {
    if (isGameStarting) {
      // Eğer oyun "yeni başladı" durumundaysa, sadece 1 saniye bekle.
      if (millis() - lastPipeMoveTime > INITIAL_PIPE_DELAY) {
        isGameStarting = false;       // Gecikme durumunu kapat. 
        lastPipeMoveTime = millis();  // Engellerin normal hareketi için zamanlayıcıyı şimdi sıfırla.
      }
    } else {
      // Gecikme bittikten sonra engelleri normal hızda hareket ettir.
      if (millis() - lastPipeMoveTime > currentPipeSpeed) {
        lastPipeMoveTime = millis();
        updatePipes();
      }
    }
  }

  drawScreen(); // Ekranı Çiz
}

void resetGame() {
  birdY = 3.0;
  birdVelocity = 0.0;
  score = 0;
  gameOver = false;
  currentPipeSpeed = INITIAL_PIPE_SPEED;
  isGameStarting = true;        // Başlangıç gecikmesini aktif et.
  lastPipeMoveTime = millis();  // Gecikme süresini ölçmek için zamanlayıcıyı sıfırla.

  // Tüm engelleri başlangıç konumlarına ayarla
  for (int i = 0; i < NUM_PIPES; i++) {
    // İlk engeli ekranın sağına, diğerlerini aradaki boşluk kadar uzağa yerleştir.
    pipeX[i] = 8 + (i * PIPE_SPACING);
    pipeGapY[i] = random(1, 6 - PIPE_GAP);
  }
  Serial.println("--- YENİ OYUN ---");
}

void updateBird() {
  // Düşme Hareketi
  birdVelocity += GRAVITY;
  birdY += birdVelocity;

  if (birdY >= 7.5) { endGame(); return; }          // Zemine değerse oyunu bitir
  if (birdY < 0) { birdY = 0; birdVelocity = 0; }   // Tavana değerse orada tut
  
  // Kuşun engellere çarpıp çarpmadığını kontrol et
  for (int i = 0; i < NUM_PIPES; i++) {
    // Eğer kuş ve engellerden biri aynı X konumundaysa ve kuşun Y konumu engelin boşluğunda değilse, çarpışma var demektir.
    if (pipeX[i] == BIRD_X_POS || pipeX[i] + 1 == BIRD_X_POS) {
      if (round(birdY) < pipeGapY[i] || round(birdY) >= pipeGapY[i] + PIPE_GAP) {
        endGame();
        return;
      }
    }
  }
}

void updatePipes() {
  // Tüm engelleri güncelle
  for (int i = 0; i < NUM_PIPES; i++) {
    pipeX[i]--; // Engeli bir piksel sola kaydır

    // Engel ekran dışına çıktıysa, onu geri dönüştür
    if (pipeX[i] < -1) {
      int max_x = 0;
      for (int j = 0; j < NUM_PIPES; j++) {
        if (pipeX[j] > max_x) {
          max_x = pipeX[j];
        }
      }

      // Rastgele bir ihtimalle normal veya geniş boşluk bırak
      if (random(100) < SINGLE_PIPE_CHANCE) {
        // Geniş boşluk bırak (tek engel etkisi yaratır)
        pipeX[i] = max_x + (PIPE_SPACING * WIDE_SPACING_MULTIPLIER);
      } else {
        // Normal boşluk bırak (iki engel etkisi yaratır)
        pipeX[i] = max_x + PIPE_SPACING;
      }
      
      pipeGapY[i] = random(1, 7 - PIPE_GAP); // Engel boşluğunu rastgele ata
    }
    
    // Skor ve Hızlandırma
    if (pipeX[i] == BIRD_X_POS - 1) {
      score++;
      Serial.print("Skor: ");
      Serial.println(score);

      // Skor belirli bir sayının katı ise hızı artır
      if ((score > 0) && (score % SCORE_TO_SPEED_UP == 0)) {
        if (currentPipeSpeed > MINIMUM_PIPE_SPEED) {
          currentPipeSpeed -= SPEED_INCREASE_AMOUNT;
        }
      }
    }
  }
}

void endGame(){
    gameOver = true;
    Serial.println("--- OYUN BİTTİ ---");
    Serial.print("Final Skor: ");
    Serial.println(score);
    Serial.println("Tekrar oynamak için dokunun...");
    String skorMetni = String(score);
    metniKaydir(skorMetni);
}

void drawScreen() {
  for (int i = 0; i < 8; i++) { screen[i] = 0x00; }
  
  if(gameOver){
    // Game Over "X" işareti
    screen[1] = 0b01100110; screen[2] = 0b00111100; screen[3] = 0b00011000;
    screen[4] = 0b00011000; screen[5] = 0b00111100; screen[6] = 0b01100110;
  } else {
    // Kuşu Çiz
    int intBirdY = round(birdY);
    if (intBirdY >= 0 && intBirdY < 8) {
      bitSet(screen[intBirdY], 7 - BIRD_X_POS);
    }
    // Tüm Engelleri Çiz
    for (int i = 0; i < NUM_PIPES; i++) {
      // Sadece ekranda görünenleri çiz
      for (int y = 0; y < 8; y++) {
        if (y < pipeGapY[i] || y >= pipeGapY[i] + PIPE_GAP) {
          // Engelin birinci sütununu çiz
          if (pipeX[i] >= 0 && pipeX[i] < 8) {
            bitSet(screen[y], 7 - pipeX[i]);
          }
        }
      }
    }
  }
  refreshMatrix();
}

// Martisi yeniler
void refreshMatrix() {
  for (int row = 0; row < 8; row++) {
    uint8_t rowData = (1 << row);
    uint8_t colData = ~reverseBits(screen[row]); 
    shiftOutAll(colData, rowData);
    delay(1);
    shiftOutAll(0xFF, 0x00);
  }
}

// Led matrix'te gösterilecek karakteri belirli bir süre boyunca gösterir.
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

// Her karakter için gerekli paterni alır
const uint8_t* paterniAl(char c) {
  for (size_t i = 0; i < karaktersayisi; i++) {
    if (LEDMatrixkarakterler[i].karakter == c) {
      return LEDMatrixkarakterler[i].patern;
    }
  }
  return LEDMatrixkarakterler[karaktersayisi - 1].patern;
}

// Verilen metni LED matrix üzerinde akıcı bir şekilde kaydırır.
void metniKaydir(String metin) {
  if (metin.length() > MAX_KARAKTER_LIMITI) {
    metin = metin.substring(0, MAX_KARAKTER_LIMITI);
  }

  String tamMetin = " " + metin + " "; // Başlangıçta ekranın boş girmesi için
  tamMetin.toUpperCase();

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
      karakteri_goster(buffer, 80); // Girilen metnin kayma hızı (örnek: 80 ms), kayma hızını artırmak için süreyi azaltabilirsiniz.
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

// LED matrix'i çalıştır
void shiftOutAll(uint8_t colData, uint8_t rowData) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, colData);
  shiftOut(dataPin, clockPin, MSBFIRST, rowData);
  digitalWrite(latchPin, HIGH);
}