# Osiloskop ve veri toplama modları

Logic Analyze, DreamSourceLab'ın DSCope osiloskoplarını da çalıştırabilir. DSCope'un iki cihaz modu vardır:

- **Osiloskop**: periyodu sabit sinyaller ve tek bir sinyal koşulu için.
- **Veri Toplama**: uzun süre boyunca yavaş sinyaller için; örneğin bir besleme gerilimi veya bir sensör çıkışı.

Bu modlar DSLogic cihazlarında kullanılamaz. Bu bölüm yalnızca temel yordamları verir.

## DSCope'u bağlama

> [!WARNING]
> Probları şebeke gerilimine bağlamayın. Probları şebeke gerilimiyle elektriksel bağlantısı olan bir devreye bağlamayın. Bu gerilim yaralanmaya veya ölüme neden olabilir.

> [!CAUTION]
> Probların toprağı, DSCope'un toprağı ve bilgisayarın toprağı birbirine bağlıdır. Prob toprağını yalnızca bilgisayar toprağıyla aynı gerilimdeki bir noktaya bağlayın. Gerilim farkı ekipmana hasar verebilir.

1. DSCope'u USB kablosuyla bilgisayara bağlayın.
2. Logic Analyze'ı başlatın. Cihaz listesinin DSCope'u gösterdiğinden emin olun.
3. Probları DSCope'un girişlerine bağlayın.
4. Her probun zayıflatma anahtarını ayarlayın.
5. Her probun toprak klipsini devrenin toprağına bağlayın.
6. Prob ucunu sinyale bağlayın.

## Cihaz seçenekleri

**Seçenekler** › **Cihaz Seçenekleri...** öğesine tıklayın veya `O` tuşuna basın.

- **Çalışma Modu**: Ölçümler için **Normal**. **Dahili Test** yalnızca cihaz testleri içindir.
- **Bant Genişliği Sınırı**: **Tam Bant Genişliği** veya **20MHz**. 20 MHz sınırı yüksek frekanslı gürültüyü azaltır.

## DSCope'u kalibre etme

Girişlerin kazancı ve ofseti sıcaklık ve nemle değişir. Ölçümlerin doğru kalması için DSCope'u kalibre edin.

### Otomatik kalibrasyon

> [!CAUTION]
> Kalibrasyondan önce tüm probları girişlerden çıkarın. Kalibrasyon sırasında bir girişteki sinyal yanlış kalibrasyon değerleri verir.

1. **Cihaz Seçenekleri** penceresini açın.
2. **Otomatik Kalibrasyon** düğmesine tıklayın.
3. Tüm probları çıkarın. **Tamam** düğmesine tıklayın. Kalibrasyon birkaç dakika sürer.
4. Kalibrasyon tamamlandığında sonucu korumak için **Kaydet** düğmesine tıklayın.

Kalibrasyonu durdurmak için **Vazgeç** düğmesine tıklayın. Cihaz bu durumda önceki kalibrasyon değerlerini kullanır.

### Elle kalibrasyon

1. **Cihaz Seçenekleri** penceresini açın.
2. **Elle Kalibrasyon** düğmesine tıklayın.
3. Araç çubuğunda **Başlat** düğmesine tıklayın.
4. Ofseti ayarlamak için probu toprağa bağlayın. Kazancı ayarlamak için probu gerilimi bilinen bir sinyale bağlayın.
5. Kalibre etmek istediğiniz dikey ölçeği ayarlayın.
6. Dalga biçimi doğru olana kadar kanalın **VOFSET** veya **VKAZANÇ** kaydırıcısını hareket ettirin.
7. Her dikey ölçek için 5. ve 6. adımları yineleyin.
8. **Kaydet** düğmesine tıklayın.

Değişiklikleri atmak için **Vazgeç** düğmesine tıklayın. Değişiklikleri yalnızca cihazın bağlantısını kesene kadar kullanmak için **Çıkış** düğmesine tıklayın. İlk değerlere dönmek için **Sıfırla** düğmesine tıklayın. Sıfırlamadan sonra otomatik kalibrasyonu yeniden yapın.

## Kanal ayarları

Her kanalın dalga biçimi alanının solunda şu denetimleri vardır:

- **Etkin**: kanalı açar veya kapatır.
- **Dikey ölçek**: her bölmenin gerilimi. Pencerede 10 bölme vardır. Ölçeği değiştirmek için düğmenin üzerinde fare tekerleğini çevirin veya düğmenin üst ya da alt bölümüne tıklayın. Bir kanalın düğmesini seçmek için `0` veya `1` tuşuna basıp sonra `↑` veya `↓` tuşuna da basabilirsiniz.
- **Kuplaj**: **DC** veya **AC**.
- **Prob zayıflatması**: probdaki anahtara uyması için **x1** veya **x10** ayarlayın.
- **OTO**: girişteki sinyal için dikey ölçeği, yatay ölçeği ve tetikleme seviyesini ayarlar.

Bir kanalın dalga biçimini yukarı veya aşağı taşımak için kanal etiketini sürükleyin.

## Yatay ölçek

Araç çubuğundaki listede her bölmenin süresini seçin. Dalga biçimi alanında fare tekerleğini de çevirebilirsiniz.

## Başlatma ve durdurma

- Sürekli yakalamayı başlatmak için **Başlat** düğmesine tıklayın veya `S` tuşuna basın. Durdurmak için **Durdur** düğmesine tıklayın.
- Bir dalga biçimi yakalayıp durmak için **Tek** düğmesine tıklayın veya `I` tuşuna basın.

## Tetikleme

Tetikleme yan panelini açmak için **Tetikleme** düğmesine tıklayın veya `T` tuşuna basın. Yan panelde şu ayarlar vardır:

- **Tetikleme Konumu**: tetikleme noktasının yakalamadaki konumu, yüzde olarak.
- **Bekletme Süresi**: bir tetiklemeden sonra cihazın yeni tetiklemeleri yok saydığı süre. Darbe gruplarından kararlı bir dalga biçimi almak için kullanın.
- **Tetikleme Hassasiyeti**: bir tetikleme için gereken gerilim değişimi. Daha büyük bir değer daha çok gürültüyü yok sayar.
- **Tetikleme Kaynakları**: **Otomatik**, **Kanal 0**, **Kanal 1**, **Kanal 0 && 1** veya **Kanal 0 | 1**.
- **Tetikleme Türleri**: **Yükselen Kenar** veya **Düşen Kenar**.

Tetikleme seviyesini ayarlamak için kanalın tetikleme seviyesi etiketine tıklayın. Fareyi hareket ettirin. Seviyeyi ayarlamak için yeniden tıklayın.

## Ölçümler

### Otomatik ölçümler

Dalga biçimi alanının altında otomatik ölçümler için 10 kutu vardır.

1. Bir ölçüm kutusuna tıklayın.
2. Kanalı seçin.
3. Ölçümü seçin. Kutuyu temizlemek için **Sıfırla** düğmesine tıklayın.

Uygulama bu ayarları bir sonraki başlatma için korur.

### İmleçler

- Bir zaman imleci eklemek için zaman cetveline tıklayın. Dalga biçimi alanında sağ fare düğmesine tıklayıp **Y İmleci Ekle** öğesini de seçebilirsiniz.
- Bir gerilim imleci eklemek için dalga biçimi alanında sağ fare düğmesine tıklayın ve **X İmleci Ekle** öğesini seçin. Her gerilim imlecinin iki yatay çizgisi vardır. Çizgilerin arasındaki etiket gerilim farkını gösterir.
- İki imleç arasındaki zamanı ölçmek için ölçüm yan panelindeki **İmleç Mesafesi** grubunu kullanın.

### Fare işaretçisiyle ölçme

Yakalamayı durdurduktan sonra fare işaretçisini dalga biçiminin üzerine getirin. Uygulama işaretçideki örneğin gerilimini gösterir.

Bir zamanı ölçmek için dalga biçiminin boş bir alanına çift tıklayın. İkinci noktaya tıklayın. Frekansı, periyodu ve doluluk oranını görmek için üçüncü noktaya tıklayın. İptal etmek için sağ fare düğmesine tıklayın.

## Spektrum (FFT)

1. **Fonksiyon** › **FFT** öğesine tıklayın.
2. **FFT Etkin** seçeneğini seçin.
3. **FFT Uzunluğu**, **Örnekleme Aralığı**, **FFT Kaynağı** ve **FFT Penceresi** ayarlarını yapın.
4. **Y Ekseni Modu** ve **DBV Aralığı** ayarlarını yapın.
5. **Tamam** düğmesine tıklayın.

Spektrum dalga biçiminin altında görünür. Frekans ölçeğini yakınlaştırmak için spektrumda fare tekerleğini çevirin. Spektrumu taşımak için sürükleyin. Frekansı ve genliği görmek için fare işaretçisini spektrumun üzerine getirin.

## Matematik kanalı

1. **Fonksiyon** › **Matematik** öğesine tıklayın.
2. **Etkin** seçeneğini seçin.
3. **İşlem Türü** seçin: **Topla**, **Çıkar**, **Çarp** veya **Böl**.
4. **1. Kaynak** ve **2. Kaynak** seçin.
5. **Tamam** düğmesine tıklayın.

## Lissajous şekli

1. **Seçenekler** › **Görünüm** › **Lissajous** öğesine tıklayın.
2. **Etkin** seçeneğini seçin.
3. **X ekseni** ve **Y ekseni** için kanalı seçin.
4. **Tamam** düğmesine tıklayın.

## Veri toplama modu

1. Araç çubuğundaki cihaz modu listesinde **Veri Toplama** seçin.
2. **Cihaz Seçenekleri** penceresini açın.
3. Her kanal için **Etkin**, **Kuplaj** ve **Volt/böl** ayarlarını yapın.
4. Başka bir birim göstermek için **Eşleme Birimi**, **Eşleme Min** ve **Eşleme Maks** ayarlarını yapın. Örneğin bir sıcaklık sensörünün çıkışını °C olarak gösterin.
5. **Tamam** düğmesine tıklayın.
6. Araç çubuğunda örnekleme hızını ve örnekleme süresini seçin.
7. **Başlat** düğmesine tıklayın veya `S` tuşuna basın.

Yakalama sırasında kanal ayarlarını değiştiremezsiniz. En yüksek örnekleme hızı olan 10 MHz'de en uzun örnekleme süresi yaklaşık 10 saniyedir. 1 kHz'de yakalama bir gün sürebilir.

Veri toplama modu osiloskop modunun kalibrasyonunu kullanır. Bir kanalda ofset görürseniz cihazı osiloskop modunda kalibre edin.
