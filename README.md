  Bu kutuphane, Arduino IDE'de Twin AIoT modulu ile yapilabilecek ornek deneyleri icermektedir.


  Twin AIoT modülünü (v1.0), Arduino IDE ile programlayabilmek için bazı donanımsal özelliklerini bilmek gerekir.
  Kütüphanedeki örnek projeleri inceledikten sonra daha ileri seviye uygulama yapmak isterseniz aşağıdaki bilgiler işinize yarayacaktır.  

  -Mikrodenetleyici
    Modül üzerinde Espressif Systems firmasına ait ESP32-S3 mini-1 modelindeki mikrodenetleyici kullanılmaktadır.
    Arduino IDE Board Manager kısmından 'esp32 by Espressif Systems' kartlarını yükleyiniz.
    Sonrasında Tools sekmesinden board seçiminde ESP32S3 Dev Module seçilmelidir.

  -RGB Led
    Modül üzerindeki RGB Led mikrodenetleyicinin 15 numaralı pinine bağlıdır. Kütüphanedeki örnekleri inceleyebilirsiniz.
  
  -Led Matrix
    Modül üzerindeki 8x8'lik Led Matrix için mikrodenetleyicinin 3 adet pini ve sürücü olarak 2 adet shift register kullanılmaktadır. 
    Kütüphanedeki örnekleri inceleyebilirsiniz.
    latchPin --> 34  , clockPin --> 47 , dataPin --> 48 .
      
  -Header Pinleri
    Modül üzerinde 1 adet 1x4 ve 1 adet 2x4'lük header pinleri bulunmaktadır. 
    Headerlar üzerinde yazan pin numaraları, mikrodenetleyicide bağlı oldukları pin numaralarını ifade eder.
    1x4'lük header HCSR04 ultrasonik mesafe sensörü için özelleştirilmiştir. 5 numaralı pin TRIG, 6 numaralı pin ECHO pinleridir.
    2x4'lük header, jumper kablolar kullanılarak farklı harici bileşenlerle kullanılabilir. GPIO, I2C, SPI gibi birçok fonksiyonu kullanabilirsiniz.
    RX, TX pinleri sırasıyla 40, 39 numaralı pinlere bağlıdır, GPIO olarak da kullanabilirsiniz.

  -Touchpad
    Modül üzerindeki Touchpad, 13 numaralı pine bağlıdır. Kütüphanedeki örnekleri inceleyebilirsiniz.

  -Giriş Konnektörleri
    Modül üzerinde diğer Twin modüllerini bağlayabileceğiniz 3 adet giriş konnektörü bulunmaktadır. D4, D8 ve D12 girişleri.
    Bu girişler, bağlanılan giriş modülünün tipine (dijital/analog) bağlı olarak mikrodenetleyicinin farklı pinlerine gitmektedir.
    Ayrıca giriş konnektörlerine bağlı Twin modülünün tipini, mikrodenetleyicinin bilebilmesi için 3 ayrı seçim pini belirlenmiştir.

    Bu seçim pinleri;
      S1 --> 37 
      S2 --> 38 
      S3 --> 14 numaralı mikrodenetleyici pinleridir. 
    
    Eğer giriş modülü dijital ise (buton, hareket algılayıcı, sıcaklık-nem sensörleri gibi) ilgili seçim pinini HIGH konumuna,
    ancak eğer analog ise (kızaklı dim, ses algılayıcı, ışık algılayıcı, mesafe algılayıcı gibi) ilgili seçim pinini LOW konumuna almalısınız. Varsayılan olarak LOW durumundadırlar.
    D4 girişi için --> S1 
    D8 girişi için --> S2 
    D12 girişi için --> S3  kullanılır.  

    Eğer giriş modülü dijital bir sensör ise , digitalRead() fonksiyonu ile dijital bir okuma yapmak istiyorsanız,
      D4 girişi --> 8 
      D8 girişi --> 11
      D12 girişi --> 9 numaralı mikrodenetleyici pinlerine bağlıdır.

    ancak eğer giriş modülü analog bir sensör ise , analogRead() fonskiyonu ile analog okuma yapmak istiyorsanız,
      D4 girişi --> 17
      D8 girişi --> 16 
      D12 girişi --> 7 numaralı mikrodenetleyici pinlerine bağlıdır.

    Örnek 1)
      D4 girişine kızaklı dim modülünü takıp analog okuma yapmak istiyorsanız:
        digitalWrite(37,LOW); // 37 --> S1 seçim pini LOW durumuna çekilir, böylece giriş sinyali 17. pine yönlendirilir.
        analogRead(17);       // 17 --> D4 girişi analog ise 17. pine bağlıdır.
      
    Örnek 2)
      D12 girişine buton modülünü takıp dijital okuma yapmak istiyorsanız:
        digitalWrite(14,HIGH); // 14 --> S3 seçim pini HIGH durumuna çekilir, böylece giriş sinyali 9. pine yönlendirilir.
        digitalRead(9);       // 9 --> D12 girişi dijital ise 9. pine bağlıdır.


  -Çıkış Konnektörleri
    Modül üzerinde diğer Twin modüllerini bağlayabileceğiniz 3 adet çıkış konnektörü bulunmaktadır. D6, D9 ve D10 çıkışları.
    D6 çıkışı --> 35
    D9 çıkışı --> 36
    D10 çıkışı --> 10 numaralı mikrodenetleyici pinlerine bağlıdır.

  26 Eylül 2025
  by Twin Science
    

