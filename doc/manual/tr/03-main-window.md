# Ana pencere

## Ana pencerenin bölümleri

Ana pencerede şu bölümler vardır:

- **Araç çubuğu.** Araç çubuğunda cihaz, yakalama ve araçlar için denetimler vardır.
- **Dalga biçimi alanı.** Dalga biçimi alanı her kanal için bir satır gösterir. Satırların üstünde bir zaman cetveli vardır.
- **Kanal etiketleri.** Her satırın solundaki etiket kanal numarasını, adı ve tetikleme düğmelerini gösterir.
- **Yan paneller.** Yan panel, dalga biçimi alanının yanındaki bir paneldir. Tetikleme, çözücü, ölçüm ve arama araçları yan panellerde açılır.

![Mantık analizörü modunda ana pencere](../figures/main-window.png)
<!-- TODO: new screenshot -->

## Araç çubuğu

Araç çubuğunda baştan sona şu öğeler vardır:

| Öğe | İşlev |
| --- | --- |
| **Dosya** | Verileri açmak, kaydetmek, dışa aktarmak ve oturumları kaydetmek için bir menü. Bkz. [Dosyalar ve oturumlar](12-files.md). |
| Cihaz türü | Bağlantıyı gösteren bir etiket: **USB 2.0**, **USB 3.0**, **Demo** veya **Dosya**. |
| Cihaz listesi | Uygulamanın kullandığı cihaz. Burada başka bir cihaz veya bir demo cihazı seçin. |
| Cihaz modu | **Mantık Analizörü**, **Osiloskop** veya **Veri Toplama**. Liste yalnızca cihazda kullanılabilen modları gösterir. |
| Örnekleme süresi | Bir yakalamanın zaman uzunluğu. |
| Örnekleme hızı | Her kanal için saniyedeki örnek sayısı. |
| **Mod** | Yakalama modu: **Tek**, **Yinelemeli** veya **Döngü**. |
| **Başlat** | Bir yakalama başlatır. Yakalama sırasında bu düğme **Durdur** olur. |
| **Anlık** | Tetiklemeyi beklemeyen bir yakalama başlatır. |
| **Tetikleme** | Tetikleme yan panelini açar. |
| **Kod Çöz** | Çözücü yan panelini açar. |
| **Ölçüm** | Ölçüm yan panelini açar. |
| **Ara** | Arama çubuğunu açar. |
| **Seçenekler** | **Cihaz Seçenekleri...** ve **Görünüm** menüsünü içeren bir menü. |
| **Yardım** | Dil, bu kılavuz, güncelleme sayfası, günlük ayarları ve sorun bildirme sayfasını içeren bir menü. |

Cihaz türü etiketi şu değerleri gösterir:

- **USB 3.0**: Cihaz bir USB 3.0 bağlantısı kullanır.
- **USB 2.0**: Cihaz bir USB 2.0 bağlantısı kullanır. Cihazda USB 3.0 bağlantısı varsa onu bir USB 3.0 bağlantı noktasına bağlayın. USB 2.0 bağlantısı, akış modundaki en yüksek örnekleme hızını düşürür.
- **Demo**: Cihaz bir demo cihazıdır. Demo cihazı test sinyalleri üretir. Uygulamanın işlevlerini denemek için kullanın.
- **Dosya**: Uygulama bir dosyadaki verileri gösterir. Cihaz yoktur.

## Klavye kısayolları

| Tuş | İşlev |
| --- | --- |
| `S` | Bir yakalamayı başlatır veya durdurur. |
| `I` | Bir anlık yakalamayı başlatır veya durdurur. Osiloskop modunda bir yakalama yapar ve durur. |
| `T` | Tetikleme yan panelini açar veya kapatır. |
| `D` | Çözücü yan panelini açar veya kapatır. |
| `M` | Ölçüm yan panelini açar veya kapatır. |
| `R` | Arama çubuğunu açar veya kapatır. |
| `O` | **Cihaz Seçenekleri** penceresini açar. |
| `Page Up` | Dalga biçimini bir pencere genişliği sola taşır. |
| `Page Down` | Dalga biçimini bir pencere genişliği sağa taşır. |
| `←` | Yakınlaştırır. |
| `→` | Uzaklaştırır. |
| `0`, `1` | Osiloskop modunda kanal 0 veya kanal 1'in ölçek denetimini seçer veya bırakır. |
| `↑`, `↓` | Osiloskop modunda seçili kanalın dikey ölçeğini değiştirir. |

Kısayollar, klavye odağı dalga biçimi alanındayken çalışır.
