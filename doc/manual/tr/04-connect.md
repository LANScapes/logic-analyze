# DSLogic Plus'ı bağlama

## USB kablosunu bağlama

> [!NOTE]
> Cihazla gelen USB kablosunu veya kısa ve kaliteli bir USB kablosu kullanın. Kabloyu doğrudan bilgisayardaki bir bağlantı noktasına bağlayın. Bir USB hub veya uzun bir kablo yakalamada hatalara neden olabilir.

1. USB kablosunu DSLogic Plus'a bağlayın.
2. USB kablosunun diğer ucunu bilgisayardaki bir USB bağlantı noktasına bağlayın.
3. DSLogic Plus üzerindeki göstergenin yandığından emin olun. Uygulama başlamadan önce gösterge kırmızıdır.
4. Logic Analyze'ı başlatın.
5. Göstergenin yeşile döndüğünden emin olun.
6. Araç çubuğundaki cihaz listesinin **DSLogic Plus** gösterdiğinden emin olun.

![USB bağlantısı](../figures/usb-connection.png)

Cihaz listesi cihazı göstermezse şu adımları uygulayın:

1. USB kablosunu bilgisayardan çıkarın.
2. 5 saniye bekleyin.
3. USB kablosunu başka bir USB bağlantı noktasına bağlayın.
4. 3. adımdan sonra cihaz listesi cihazı yine göstermezse uygulamadan çıkın ve uygulamayı yeniden başlatın.

> [!NOTE]
> Cihazı aynı anda yalnızca bir program kullanabilir. `dslcap` veya başka bir program cihazı kullanıyorsa uygulama cihazı bulamaz.

## Prob kablosunu bağlama

Prob kablosunda 16 kanal teli vardır. Her kanal telinin bir ekranı, bir sinyal ucu ve bir toprak ucu vardır. Tellerin renkleri 0 ile 15 arasındaki kanalları belirtir. Bir tel daha şu sinyalleri taşır:

- **CK**: Harici saat girişi. Yalnızca **Harici Saat Kullan** ayarıyla kullanın.
- **TI**: Harici tetikleme sinyali girişi.
- **TO**: Tetikleme sinyali çıkışı. Tetikleme olduğunda cihaz TO üzerinden bir darbe gönderir.

Genellikle CK, TI ve TO tellerini bağlamazsınız.

![Prob kablosu ve kanalları](../figures/probe-cable-channels.png)

1. Prob kablosunu DSLogic Plus'ın giriş konektörüne bağlayın.
2. Konektörü cihaza sonuna kadar itin.

## Kanalları devreye bağlama

> [!WARNING]
> Probları şebeke gerilimine bağlamayın. Probları şebeke gerilimiyle elektriksel bağlantısı olan bir devreye bağlamayın. Bu gerilim yaralanmaya veya ölüme neden olabilir.

> [!CAUTION]
> Bir toprak telini bağlamadan önce devrenin toprağı ile bilgisayarın toprağının aynı gerilimde olduğundan emin olun. Gerilim farkı büyük bir akıma ve ekipman hasarına neden olabilir.

1. Ölçtüğünüz devrenin gücünü kesin.
2. En az bir toprak telini devrenin toprağına bağlayın.
3. Kullandığınız her kanal telini devredeki bir sinyale bağlayın.
4. Hiçbir probun başka bir kontağa değmediğinden emin olun.
5. Devreye gücü verin.

![Toprak bağlantıları: tek ortak toprak (sol) veya her kanal için bir toprak (sağ)](../figures/probe-grounding.png)

Frekansı 5 MHz'den düşük sinyaller için tüm kanallara tek bir toprak teli yeterlidir. Daha yüksek frekanslı sinyaller için her kanal telinin toprak ucunu kendi sinyalinin yakınındaki toprağa bağlayın. Kısa toprak bağlantıları temiz sinyal kenarları verir.

## DSLogic Plus bağlantısını kesme

> [!CAUTION]
> Yakalama sırasında USB kablosunu çıkarmayın. Kabloyu çıkarırsanız yakalama verilerinde hatalar olabilir.

1. Yakalamayı durdurun. Araç çubuğunda **Durdur** görünüyorsa ona tıklayın.
2. Devrenin gücünü kesin.
3. Probları devreden çıkarın.
4. USB kablosunu çıkarın.
