# Dosyalar ve oturumlar

Dosya menüsünü açmak için araç çubuğunda **Dosya** düğmesine tıklayın. Menüde şu öğeler vardır:

- **Ayarlar**: oturumları yüklemek ve kaydetmek için bir menü.
- **Aç...**: bir veri dosyasını açar.
- **Kaydet...**: yakalamanın verilerini kaydeder.
- **Dışa Aktar...**: verileri başka bir biçime aktarır.
- **Görüntü Al...**: pencerenin bir görüntüsünü kaydeder.

## Oturumlar

Bir oturum dosyası ayarları içerir, ancak yakalamanın verilerini içermez. Bir oturum cihaz seçeneklerini, etkin kanalları, kanal adlarını ve renklerini ve tetikleme ayarlarını içerir. Oturum dosyasının uzantısı `.dsc`'dir.

### Oturumu kaydetme

1. **Dosya** › **Ayarlar** › **Oturumu Kaydet** öğesine tıklayın.
2. Klasörü seçin ve dosya adını yazın.
3. **Kaydet** düğmesine tıklayın.

### Oturum yükleme

1. **Dosya** › **Ayarlar** › **Oturum Yükle** öğesine tıklayın.
2. Oturum dosyasını seçin.
3. **Aç** düğmesine tıklayın.

### İlk ayarlara dönme

**Dosya** › **Ayarlar** › **Varsayılan Oturumu Yükle** öğesine tıklayın. Uygulama cihazın tüm ayarlarını ilk değerlerine getirir.

Uygulama, çıktığınızda ayarları otomatik olarak kaydeder. Uygulamayı yeniden başlattığınızda son oturumun ayarlarını yükler.

## Verileri kaydetme

1. **Dosya** › **Kaydet...** öğesine tıklayın.
2. Klasörü seçin ve dosya adını yazın.
3. **Kaydet** düğmesine tıklayın.

Uygulama verileri ve ayarları `.dsl` uzantılı bir dosyaya kaydeder. Bu dosyayı Logic Analyze'da yeniden açabilirsiniz.

> [!CAUTION]
> Uygulama verileri otomatik olarak kaydetmez. Yeni bir yakalamaya başlamadan veya uygulamadan çıkmadan önce verileri kaydedin. Yeni bir yakalama önceki yakalamanın verilerinin yerini alır.

## Veri dosyası açma

1. **Dosya** › **Aç...** öğesine tıklayın.
2. `.dsl` uzantılı bir dosya seçin.
3. **Aç** düğmesine tıklayın.

Uygulama verileri dalga biçimi alanında gösterir. Cihaz türü etiketi **Dosya** gösterir.

## Verileri dışa aktarma

Dışa aktarma, başka programların okuyabileceği bir dosya oluşturur.

1. **Dosya** › **Dışa Aktar...** öğesine tıklayın. **Dışa Aktar** penceresi açılır.
2. **yol** düğmesine tıklayın.
3. Klasörü seçin, dosya adını yazın ve biçimi seçin.
4. **Kaydet** düğmesine tıklayın.
5. Biçim CSV ise **Özgün veri** veya **Sıkıştırılmış veri** seçin. Sıkıştırılmış veride yalnızca her değer değişikliği için bir satır vardır.
6. **Tamam** düğmesine tıklayın.

Mantık analizörü modunda şu biçimler kullanılabilir:

| Biçim | Uzantı | Kullanım |
| --- | --- | --- |
| CSV | `.csv` | Hesap tablosu programları ve betikler. |
| VCD | `.vcd` | Dalga biçimi programları; örneğin GTKWave. |
| Gnuplot | `.gnuplot` | Gnuplot programı. |
| srzip | `.srzip` | sigrok programları; örneğin PulseView. |

Osiloskop modunda ve veri toplama modunda yalnızca CSV kullanılabilir.

![CSV için dışa aktarma penceresi](../figures/export-csv.png)
<!-- TODO: new screenshot -->

## Pencerenin görüntüsünü kaydetme

1. **Dosya** › **Görüntü Al...** öğesine tıklayın.
2. Klasörü seçin ve dosya adını yazın.
3. PNG veya JPEG seçin.
4. **Kaydet** düğmesine tıklayın.
