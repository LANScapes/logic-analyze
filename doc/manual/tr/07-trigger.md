# Tetikleme

Tetikleme, sinyallerdeki bir koşuldur. Koşul oluştuğunda cihaz o zamanı tetikleme noktası olarak işaretler. Tetikleme, incelemek istediğiniz sinyal bölümünü yakalamanızı sağlar.

Uygulamada iki tetikleme türü vardır:

- **Basit Tetikleme**: Bir veya daha çok kanalda bir kenar veya bir seviye.
- **Gelişmiş Tetikleme**: Bir koşul dizisi veya bir seri veri yolundaki bir değer.

Tetikleme yan panelini açmak için araç çubuğunda **Tetikleme** düğmesine tıklayın veya `T` tuşuna basın.

> [!NOTE]
> Sinyal tetikleme koşuluna uymazsa yakalama beklemeye devam eder. Sinyali tetikleme olmadan görmek için **Anlık** düğmesine tıklayın. Beklemeyi durdurmak için **Durdur** düğmesine tıklayın.

## Tetikleme konumu

**Tetikleme Konumu** ayarı, tetikleme noktasının yakalamadaki yerini belirler. Değer, örnekleme süresinin yüzdesidir.

- Küçük bir değer, örneğin %10, tetiklemeden sonraki sinyalin daha çoğunu gösterir.
- Büyük bir değer, örneğin %90, tetiklemeden önceki sinyalin daha çoğunu gösterir.

Tetikleme konumu cihazın belleğini kullanır. Bu nedenle onu yalnızca tampon modunda ayarlayabilirsiniz. Akış modunda tetikleme konumu her zaman yaklaşık %1'dir.

![Tetikleme konumu %10 (sol) ve %90 (sağ)](../figures/trigger-position.png)
<!-- TODO: new screenshot -->

## Basit tetikleme

Dalga biçimi alanındaki her kanal etiketinde beş tetikleme düğmesi vardır. Soldan sağa düğmeler şunlardır:

1. Yükselen kenar
2. Yüksek seviye
3. Düşen kenar
4. Düşük seviye
5. Yükselen kenar veya düşen kenar

![Kanal etiketindeki tetikleme düğmeleri](../figures/simple-trigger-buttons.png)

Basit tetikleme ayarlamak için şu adımları uygulayın:

1. Tetikleme yan panelini açın.
2. **Basit Tetikleme** seçeneğini seçin.
3. Bir kanalın etiketinde istediğiniz tetikleme düğmesine tıklayın. Düğme farklı bir renkte görünür.
4. Bir kanaldan tetiklemeyi kaldırmak için aynı düğmeye yeniden tıklayın.
5. **Tetikleme Konumu** ayarını yapın.

Birden çok kanalda tetikleme ayarlarsanız tüm koşullar aynı örnekte oluşmalıdır (mantıksal VE).

## Gelişmiş tetikleme

> [!NOTE]
> Gelişmiş tetikleme yalnızca tampon modunda kullanılabilir. Kullanmak için **Çalışma Modu** ayarını **Tampon Modu** yapın. Bkz. [Cihaz seçenekleri](05-device-options.md).

Gelişmiş tetiklemeyi kullanmak için tetikleme yan panelinde **Gelişmiş Tetikleme** seçeneğini seçin. Sonra **Aşamalı Tetikleme** sekmesini veya **Seri Tetikleme** sekmesini seçin.

### Her kanal için değerler

Aşamalı tetikleme ve seri tetikleme 16 karakterlik bir satır kullanır. Her karakter bir kanalın koşuludur. Sağdaki karakter kanal 0'dır. Soldaki karakter kanal 15'tir.

| Karakter | Koşul |
| --- | --- |
| `X` | Tüm değerler (kanalın etkisi yoktur). |
| `0` | Düşük seviye. |
| `1` | Yüksek seviye. |
| `R` | Yükselen kenar. |
| `F` | Düşen kenar. |
| `C` | Yükselen kenar veya düşen kenar. |

### Aşamalı tetikleme

Aşamalı tetikleme bir koşul dizisidir. Her koşul bir aşamadır. Cihaz önce aşama 0'ı inceler. Bir aşamanın koşulu oluştuğunda cihaz sonraki aşamaya geçer. Son aşama tamamlandığında tetikleme oluşur. En çok 16 aşama kullanabilirsiniz.

Her aşamada şu ayarlar vardır:

- İki kanal koşulu satırı.
- Her satır için `==` veya `!=`. `==` ile kanallar satıra uyduğunda koşul oluşur. `!=` ile kanallar satıra uymadığında koşul oluşur.
- **Ve** veya **Veya**. Bu ayar iki satırı birleştirir.
- **Sayaç**: Aşama tamamlanmadan önce koşulun oluşması gereken sayı.
- **Ardışık**: Bu onay kutusunu seçtiğinizde koşul, ara vermeden birbirini izleyen örneklerde oluşmalıdır.

![Aşamalı tetikleme ayarları](../figures/stage-trigger-panel.png)
<!-- TODO: new screenshot -->

Aşamalı tetikleme ayarlamak için şu adımları uygulayın:

1. **Toplam Tetikleme Aşaması** içinde aşama sayısını seçin.
2. Sağdaki aşama listesinde aşama 0'a tıklayın.
3. Birinci satıra kanal koşullarını yazın.
4. Gerekirse ikinci satıra kanal koşullarını yazın ve **Ve** veya **Veya** seçin.
5. **Sayaç** alanına bir değer yazın.
6. Diğer her aşama için 2 ile 5 arasındaki adımları yineleyin.

Üç örnek:

**Örnek 1.** Kanal 0, 1000 örnekten uzun süre yüksek kaldığında tetikleme:

1. **Toplam Tetikleme Aşaması** değerini 1 yapın.
2. Aşama 0'da birinci satırda kanal 0 için `1` yazın.
3. **Ardışık** seçeneğini seçin.
4. **Sayaç** değerini 1000 yapın.

![Örnek 1](../figures/stage-example-level-count.png)

**Örnek 2.** Kanal 0'daki yükselen kenarda veya kanal 1'deki düşen kenarda tetikleme:

1. **Toplam Tetikleme Aşaması** değerini 1 yapın.
2. Aşama 0'da birinci satırda kanal 0 için `R` yazın.
3. İkinci satırda kanal 1 için `F` yazın.
4. **Veya** seçin.

![Örnek 2](../figures/stage-example-or.png)

**Örnek 3.** Kanal 0'da bir yükselen kenar, sonra kanal 1'de 100 düşen kenar, sonra kanal 2'de yüksek seviye olduğunda tetikleme:

1. **Toplam Tetikleme Aşaması** değerini 3 yapın.
2. Aşama 0'da kanal 0 için `R` yazın.
3. Aşama 1'de kanal 1 için `F` yazın. **Sayaç** değerini 100 yapın.
4. Aşama 2'de kanal 2 için `1` yazın.

![Örnek 3](../figures/stage-example-sequence.png)

### Seri tetikleme

Seri tetikleme, bir seri veri yolunda bir veri değeri bulur. Bir kaydırma yazmacı gibi çalışır. Ayarlar şunlardır:

- **Başlangıç Bayrağı**: Seri tetiklemeyi başlatan koşul.
- **Bitiş Bayrağı**: Kaydırma yazmacını temizleyen koşul.
- **Saat Bayrağı**: Kaydırma yazmacına bir bit ekleyen koşul.
- **Veri Kanalı**: Veriyi ileten kanal.
- **Veri Bitleri**: Değerdeki bit sayısı.
- **Veri Değeri**: Tetiklemeye neden olan değer.

Başlangıç bayrağı oluştuktan sonra cihaz her saat bayrağında veri kanalını okur. Cihaz bu biti kaydırma yazmacına taşır. Kaydırma yazmacının son bitleri **Veri Değeri** ile eşit olduğunda tetikleme oluşur. Bitiş bayrağı oluştuğunda cihaz kaydırma yazmacını temizler.

![Seri tetikleme ayarları](../figures/serial-trigger-panel.png)
<!-- TODO: new screenshot -->

**Örnek 4.** Bir I2C veri yolunda `010000100` değeri oluştuğunda tetikleme. Kanal 0 SCL, kanal 1 SDA'dır.

1. **Başlangıç Bayrağı** ayarını, SCL yüksekken SDA'daki düşen kenar yapın: sağdaki iki karakterde `F1`.
2. **Bitiş Bayrağı** ayarını, SCL yüksekken SDA'daki yükselen kenar yapın: `R1`.
3. **Saat Bayrağı** ayarını SCL'deki yükselen kenar yapın: kanal 0 için `R`.
4. **Veri Kanalı** değerini 1 yapın.
5. **Veri Bitleri** değerini 9 yapın.
6. **Veri Değeri** alanına `010000100` yazın.

![Örnek 4](../figures/serial-example-i2c.png)

**Örnek 5.** Bir SPI veri yolunun MOSI hattında `0x1234` değeri oluştuğunda tetikleme. Kanal 0 CS#, kanal 1 CLK, kanal 2 MISO ve kanal 3 MOSI'dir.

1. **Başlangıç Bayrağı** ayarını CS#'deki düşen kenar yapın: kanal 0 için `F`.
2. **Bitiş Bayrağı** ayarını CS#'deki yükselen kenar yapın: kanal 0 için `R`.
3. **Saat Bayrağı** ayarını CLK'deki yükselen kenar yapın: kanal 1 için `R`.
4. **Veri Kanalı** değerini 3 yapın.
5. **Veri Bitleri** değerini 16 yapın.
6. **Veri Değeri** alanına `0001001000110100` yazın.

![Örnek 5](../figures/serial-example-spi.png)

Değeri onaltılık olarak yazmak için **Onaltılık biçimde gir** seçeneğini seçin. Sonra değeri **Onaltılık** alanına yazın.
