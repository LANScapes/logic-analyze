# Örnekleme hızı ve örnekleme süresi

Araç çubuğunda yakalama uzunluğu için iki liste vardır. Üstteki liste örnekleme süresidir. Alttaki liste örnekleme hızıdır.

- **Örnekleme süresi**, yakalamanın zaman uzunluğudur.
- **Örnekleme hızı**, her kanal için saniyedeki örnek sayısıdır.

Kullanılabilen değerler cihaza, USB bağlantısına, çalışma moduna ve kanal moduna göre değişir.

## En uzun örnekleme süresi

**Tampon modu.** Cihazın belleği örnekleme süresini sınırlar. Şu formülü kullanın:

```text
maximum duration = memory size / (sample rate × number of enabled channels)
```

DSLogic Plus'ın 256 Mbit belleği vardır. İki örnek:

- 100 MHz ve 16 kanalda en uzun örnekleme süresi yaklaşık 167.77 ms'dir.
- 400 MHz ve 1 kanalda en uzun örnekleme süresi yaklaşık 671.09 ms'dir.

**Akış modu.** Bilgisayarın belleği örnekleme süresini sınırlar. Uygulama her kanal için 16 G örnek tutabilir. İki örnek:

- 1 MHz'de en uzun örnekleme süresi yaklaşık 4.77 saattir.
- 100 MHz'de en uzun örnekleme süresi yaklaşık 2.86 dakikadır.

## Örnekleme hızını seçme

Örnekleme hızını, sinyaldeki en yüksek frekansın 4 ile 10 katına ayarlayın.

Sinyal frekansının 4 katında uygulama her kenarı kaydeder. Ancak her kenarın zamanında sinyal periyodunun %25'ine kadar hata olur. Sinyal frekansının 10 katında hata %10'a düşer.

Bir kenarın zaman hatası bir örnekleme periyoduna eşit veya daha küçüktür. Örneğin 100 MHz'de örnekleme periyodu 10 ns'dir. Bu nedenle her kenarın hatası ±10 ns veya daha azdır.

![Örnekleme hızının kaydedilen dalga biçimine etkisi](../figures/sample-rate-effect.png)

Tipik değerler şunlardır:

| Sinyal | Tipik örnekleme hızı |
| --- | --- |
| 115200 baud UART | 2 MHz |
| 400 kHz I2C | 4 MHz - 10 MHz |
| 40 MHz SPI | 400 MHz |

## Çok yüksek bir örnekleme hızı kullanmayın

Daha yüksek bir örnekleme hızı daha doğru bir dalga biçimi verir. Ancak yüksek bir örnekleme hızının şu sorunları da vardır:

1. Uygulama her saniye daha çok veri kaydeder. Bu nedenle en uzun örnekleme süresi kısalır. Uygulama verileri göstermek ve kodunu çözmek için de daha çok zaman kullanır.
2. Yavaş bir sinyalin kenarları da yavaş olabilir. Yüksek bir örnekleme hızında uygulama her yavaş kenarda eşik yakınında küçük darbeler kaydedebilir. Bu darbeler çözücülerde hatalara neden olabilir.

Yavaş sinyallerde istenmeyen kısa darbeler görürseniz örnekleme hızını düşürün. **Filtre Hedefleri** ayarını **1 Örnekleme Periyodu** olarak da ayarlayabilirsiniz. Bkz. [Cihaz seçenekleri](05-device-options.md).

## Örnekleme hızını ve süreyi ayarlama

1. Çalışma modunu ve kanal modunu ayarlayın. Bkz. [Cihaz seçenekleri](05-device-options.md).
2. Araç çubuğundaki alttaki listede örnekleme hızını seçin.
3. Araç çubuğundaki üstteki listede örnekleme süresini seçin.

> [!NOTE]
> Kanal modunu değiştirdiğinizde uygulama örnekleme hızını değiştirebilir. Cihaz seçeneklerindeki her değişiklikten sonra örnekleme hızını yeniden denetleyin.
