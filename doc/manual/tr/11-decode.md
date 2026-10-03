# Protokol çözücüler

Bir protokol çözücü, bir yakalamanın verilerini okur ve bir protokolün çerçevelerini bulur; örneğin UART, I2C veya SPI. Uygulama sonucu kanalların üstünde yeni bir satırda gösterir. Uygulamada 100'den çok çözücü vardır.

Çözücü yan panelini açmak için araç çubuğunda **Kod Çöz** düğmesine tıklayın veya `D` tuşuna basın. Yan panelin iki bölümü vardır:

- Çözücü listesi; üstünde **Çözücü ara...** alanı vardır.
- **Kod Çözme Sonuçları** listesi. Bu liste çözücüden gelen her öğeyi bir metin satırı olarak gösterir.

![Çözücü yan paneli](../figures/decoder-dock.png)
<!-- TODO: new screenshot -->

## Çözücü ekleme

> [!NOTE]
> `0:` önekli bir çözücü daha küçük bir sürümdür. Bitleri göstermez. Üzerine daha üst düzey bir protokol ekleyemezsiniz. Daha hızlı kod çözer ve daha az bellek kullanır.

1. **Çözücü ara...** alanına tıklayın. Çözücü listesi açılır.
2. Protokol adının bir bölümünü yazın; örneğin `I2C`. Liste yalnızca metne uyan çözücüleri gösterir.
3. Çözücüye tıklayın. **Çözücü Seçenekleri** penceresi açılır.
4. Protokolün kanallarını ayarlayın. Örneğin I2C için **SCL** ve **SDA** ayarlayın.
5. Protokol seçeneklerini ayarlayın; örneğin bir UART'ın baud hızı.
6. Uygulamanın göstereceği sonuç satırlarını seçin.
7. Gerekirse kod çözme bölgesini ayarlayın. Bkz. [Yakalamanın bir bölümünün kodunu çözme](#decode-region).
8. **Tamam** düğmesine tıklayın.

Uygulama verilerin kodunu çözer ve sonuçları dalga biçimi alanında yeni bir satırda gösterir.

Daha çok çözücü eklemek için yordamı her çözücü için yineleyin.

![Çözücü düğmeleri: ayarlar düğmesi çözücü seçeneklerini açar](../figures/decoder-buttons.png)

Bir çözücünün ayarlarını değiştirmek için yan panelde o çözücünün ayarlar düğmesine tıklayın.

## Yığın çözücü ekleme

Bazı protokoller daha alt düzey bir protokol kullanır. Örneğin 24xx EEPROM protokolü I2C kullanır. Üst düzey protokolü eklediğinizde uygulama alt düzey protokolleri de ekler.

1. **Çözücü ara...** alanına üst düzey protokolün adını yazın; örneğin `24xx`.
2. Çözücüye tıklayın.
3. **Çözücü Seçenekleri** penceresinde her protokol katmanının seçeneklerini ayarlayın.
4. **Tamam** düğmesine tıklayın.

Sonuçlar alt düzey protokolün çerçevelerini ve üst düzey protokolün komutlarını ve verilerini gösterir.

## Yakalamanın bir bölümünün kodunu çözme {#decode-region}

Genellikle uygulama tüm verilerin kodunu çözer. Yalnızca bir bölümün kodunu çözmek için bir başlangıç imleci ve bir bitiş imleci ayarlayın. Örneğin devrenin sıfırlanması sırasındaki gürültüyü yok sayabilirsiniz. Daha kısa bir alan kod çözme süresini de kısaltır.

1. Alanın başına ve sonuna iki imleç ekleyin. Bkz. [Ölçümler](10-measure.md).
2. Çözücünün **Çözücü Seçenekleri** penceresini açın.
3. **Başlangıç** listesinde başlangıç imlecini seçin.
4. **Bitiş** listesinde bitiş imlecini seçin.
5. **Tamam** düğmesine tıklayın.

## Sonuç listesini okuma

**Kod Çözme Sonuçları** listesi çözücüden gelen öğeleri zaman sırasıyla gösterir. Dalga biçimini bir öğeye taşımak için o öğenin satırına tıklayın.

Listenin gösterdiği sütunları değiştirmek için listenin üstündeki ayarlar düğmesine tıklayın.

## Sonuçlarda metin bulma

1. **Kod Çözme Sonuçları** listesinin arama alanına bir metin yazın.
2. Metni içeren sonraki satıra gitmek için sağ oka tıklayın. Önceki satıra gitmek için sol oka tıklayın.

Dalga biçimi, aramanın bulduğu her satırın öğesine gider. Önce bir satıra tıklarsanız arama o satırdan başlar.

![Kod çözme sonuçlarında arama](../figures/decoder-list-search.png)

Bir bayt dizisini bulmak için baytların arasına `-` işaretini koyun. Örneğin `70-70-70`, değeri 70 olan ve art arda gelen üç baytı bulur.

![Bir bayt dizisini arama](../figures/decoder-multibyte-search.png)

> [!NOTE]
> Bayt dizisi araması yalnızca UART, I2C ve SPI çözücüleriyle çalışır.

## Sonuçları dışa aktarma

1. **Kod Çözme Sonuçları** listesinin üstündeki kaydet düğmesine tıklayın. **Protokol Dışa Aktarma** penceresi açılır.
2. **Dışa Aktarma Biçimi** içinde CSV veya TXT seçin.
3. Dışa aktarmak istediğiniz her sütunu seçin. Uygulama tüm sütunları zaman sırasıyla tek bir dosyaya koyar.
4. **Tamam** düğmesine tıklayın.
5. Klasörü seçin ve dosya adını yazın.
6. **Kaydet** düğmesine tıklayın.

## Çözücü silme

![Bir çözücüyü veya tüm çözücüleri silme](../figures/decoder-delete.png)

- Bir çözücüyü silmek için o çözücünün satırındaki **×** düğmesine tıklayın.
- Tüm çözücüleri silmek için yan panelin üstünde, **+** düğmesinin yanındaki **×** düğmesine tıklayın.
