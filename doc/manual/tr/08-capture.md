# Veri yakalama

Bir yakalamaya başlamadan önce şu öğeleri ayarlayın:

1. Cihaz seçenekleri. Bkz. [Cihaz seçenekleri](05-device-options.md).
2. Örnekleme hızı ve örnekleme süresi. Bkz. [Örnekleme hızı ve örnekleme süresi](06-sample-rate.md).
3. Gerekirse tetikleme. Bkz. [Tetikleme](07-trigger.md).
4. Yakalama modu. Bkz. [Yakalama modları](#capture-modes).

## Yakalamayı başlatma

İki yakalama türü vardır:

- **Başlat** standart bir yakalama başlatır. Bir tetikleme ayarladıysanız cihaz tetiklemeyi bekler.
- **Anlık** bir yakalamayı hemen başlatır. Cihaz tetikleme ayarlarını kullanmaz.

Standart bir yakalama başlatmak için **Başlat** düğmesine tıklayın veya `S` tuşuna basın. Anlık bir yakalama başlatmak için **Anlık** düğmesine tıklayın veya `I` tuşuna basın. Yakalama sırasında düğme **Durdur** olur. Yakalamayı durdurmak için **Durdur** düğmesine tıklayın.

### Tampon modunda standart yakalamanın sırası

1. **Başlat** düğmesine tıklarsınız.
2. Uygulama ayarları cihaza gönderir.
3. Tetikleme yoksa cihaz hemen kaydetmeye başlar. Tetikleme varsa cihaz tetiklemeyi bekler.
4. Cihaz, örnekleme süresi bitene veya belleği dolana kadar kaydeder.
5. Cihaz verileri bilgisayara gönderir.
6. Uygulama dalga biçimini dalga biçimi alanında gösterir.

### Akış modunda standart yakalamanın sırası

1. **Başlat** düğmesine tıklarsınız.
2. Uygulama ayarları cihaza gönderir.
3. Tetikleme varsa cihaz tetiklemeyi bekler. Döngü modunda cihaz tetiklemeyi kullanmaz.
4. Cihaz, yakalama sırasında verileri bilgisayara gönderir.
5. Uygulama, yakalama sırasında dalga biçimini gösterir.
6. Yakalama, örnekleme süresinin sonunda durur. Döngü modunda yakalama, siz **Durdur** düğmesine tıklayana kadar devam eder.

## Anlık yakalamayı kullanma

Anlık yakalama standart yakalamayla aynıdır, ancak tetikleme ayarlarını kullanmaz. Şu durumlarda kullanın:

- Tetikleme koşulu oluşmadığı için standart yakalama uzun süre bekler.
- Sinyalleri şu anda görmek istiyorsunuz.
- Tetiklemeyi değiştirmeden önce sinyalleri incelemek istiyorsunuz.

Sinyal yoksa standart yakalama tetikleme konumunda bekler. Durum alanında **Tetikleme Bekleniyor!** görünür. Anlık yakalama sinyalleri hemen kaydeder.

## Yakalama modları {#capture-modes}

Yakalama modunu seçmek için araç çubuğunda **Mod** düğmesine tıklayın. Sonra şu öğelerden birini seçin:

| Yakalama modu | Tampon modu | Akış modu |
| --- | --- | --- |
| **Tek** | Evet | Evet |
| **Yinelemeli** | Evet | Evet |
| **Döngü** | Hayır | Evet |

![Yakalama modu menüsü](../figures/capture-mode-menu.png)
<!-- TODO: new screenshot -->

### Tek

Cihaz bir yakalama yapar. Sonra yakalama durur.

Tampon modunda uygulama dalga biçimini yakalamadan sonra gösterir. Akış modunda uygulama dalga biçimini yakalama sırasında gösterir.

Bir sinyal koşulunu veya şu andaki dalga biçimini yakalamak için bu modu kullanın.

### Yinelemeli

Cihaz bir yakalama yapar. Sonra sonraki yakalamayı otomatik olarak başlatır. Bu, siz **Durdur** düğmesine tıklayana kadar devam eder.

Tampon modunda uygulama, yakalamalar arasındaki aralık için bir pencere gösterir. 0.1 s ile 10 s arasında bir değer ayarlayabilirsiniz.

Birçok kez oluşan bir sinyal koşulunu görmek için bu modu kullanın. Örneğin devrenin her sıfırlanmasından veya bir düğmeye her basılmasından sonraki sinyalleri görmek için kullanın. Bir tetiklemeyle birlikte kullanın.

### Döngü

Bu mod yalnızca akış modunda kullanılabilir. Yakalama, siz **Durdur** düğmesine tıklayana kadar devam eder. Veriler örnekleme süresinden uzun olduğunda ilk veriler pencerenin solundan çıkar. En yeni veriler sağdan girer. Uygulama dışarı çıkan verileri atar.

Sinyal koşulunun zamanını bilmediğinizde bu modu kullanın. Yakalama sırasında dalga biçimine bakın. Koşulu gördüğünüzde **Durdur** düğmesine tıklayın.

> [!NOTE]
> Döngü modunda cihaz tetikleme ayarlarını kullanmaz.

## Yakalama durumu

Yakalama sırasında dalga biçimi alanı durumu gösterir:

- **Tetikleme Bekleniyor!**: Cihaz tetikleme koşulunu bekler.
- **Tetiklendi!**: Tetikleme oluştu.
- **% Yakalandı**: Yakalamanın tamamlanan yüzdesi.

Yakalamadan sonra dalga biçimi alanının altında **Tetikleme Zamanı** görünür.
