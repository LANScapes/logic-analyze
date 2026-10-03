# Giriş

## Logic Analyze hakkında

Logic Analyze, DreamSourceLab mantık analizörleri için bir macOS uygulamasıdır. LANScapes bu uygulamayı sağlar. Uygulama, DreamSourceLab'ın bir programı olan DSView'dan gelir. DSView, sigrok projesinin yazılımını kullanır.

Uygulama, bir DSLogic mantık analizörü ile dijital sinyalleri kaydeder. Sonra sinyalleri dalga biçimi olarak gösterir. Dalga biçimlerini ölçebilir ve seri protokollerin kodunu çözebilirsiniz. Verileri kaydedebilir ve başka biçimlere dışa aktarabilirsiniz.

Bu kılavuz, örnekler için DSLogic Plus kullanır. Diğer DSLogic modelleri aynı yordamlarla çalışır. Kanal, bellek ve örnekleme hızı sınırları farklıdır.

Uygulamada `dslcap` aracı da vardır. Bu araç, ana pencere olmadan veri yakalar. Bkz. [dslcap aracı](13-dslcap.md).

## Bu kılavuz hakkında

Bu kılavuz, ASD-STE100 Simplified Technical English kurallarını izler. Her cümle kısadır. Bir yordamdaki her adım tek bir talimat verir. Bu kılavuzdaki her teknik terimin tek bir anlamı vardır. [Teknik terimler ve fiiller](15-terms.md) bölümünde teknik terimlerin listesi vardır.

Bu kılavuz şu metin biçimlerini kullanır:

- **Kalın metin**, uygulamadaki bir etiketi gösterir; örneğin bir düğme, bir menü öğesi veya bir alan.
- `Kod metni`, klavyedeki bir tuşu, bir komutu, bir dosya adını veya yazdığınız bir değeri gösterir.
- Menülerdeki bir yol › işaretini kullanır; örneğin **Dosya** › **Kaydet...**.
- Numaralı adımlardan oluşan bir liste bir yordamdır. Adımları verilen sırayla uygulayın.

## Güvenlik talimatları

Bu kılavuz, güvenlik talimatları için şu etiketleri kullanır:

- **UYARI**, yaralanma veya ölüm riskini belirtir.
- **DİKKAT**, ekipman hasarı riskini veya verileriniz için bir riski belirtir.
- **NOT**, size yardımcı olan bilgi verir. **NOT** sonrasındaki bilgi bir talimat değildir.

Güvenlik talimatı, ilgili olduğu adımdan önce gelir. Bir yordama başlamadan önce tüm güvenlik talimatlarını okuyun.

> [!WARNING]
> Probları şebeke gerilimine bağlamayın. Probları şebeke gerilimiyle elektriksel bağlantısı olan bir devreye bağlamayın. Probların bilgisayarla elektriksel bağlantısı vardır. Bu gerilim yaralanmaya veya ölüme neden olabilir.

> [!CAUTION]
> Bir kanal girişine, cihaz teknik özelliklerindeki sınırdan yüksek bir gerilim uygulamayın. Çok yüksek bir gerilim mantık analizörüne hasar verebilir.

> [!CAUTION]
> Mantık analizörünün toprak telleri, USB kablosu üzerinden bilgisayarınızın toprağına bağlanır. Toprak tellerini yalnızca ölçtüğünüz devrenin toprağına bağlayın. İki toprağın gerilimi farklıysa büyük bir akım akabilir ve devreye, mantık analizörüne ve bilgisayara hasar verebilir.

## Sistem gereksinimleri

Şu ekipman gereklidir:

- macOS yüklü bir Mac. En düşük macOS sürümü sürüm notlarında yazar.
- Bir USB bağlantı noktası. USB 3.0 bağlantı noktası en yüksek hızı verir. USB 2.0 bağlantı noktası da çalışır.
- Bir DSLogic mantık analizörü, USB kablosu ve prob kablosu.

Uygulamayı mantık analizörü olmadan da kullanabilirsiniz. **Demo** cihazı test sinyalleri üretir. Önceki bir yakalamanın veri dosyasını da açabilirsiniz.
