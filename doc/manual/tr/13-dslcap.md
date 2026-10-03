# dslcap aracı

`dslcap` aracı, ana pencere olmadan bir DSLogic cihazından veri yakalar. Betiklerde ve otomatik testlerde kullanın. Araç örnekleri bir ikili dosyaya yazar. Sonucu içeren bir JSON nesnesini standart çıkışa yazar.

Araç uygulama paketinin içindedir:

```sh
"/Applications/Logic Analyze.app/Contents/MacOS/dslcap"
```

> [!NOTE]
> Cihazı aynı anda yalnızca bir program kullanabilir. `dslcap` kullanmadan önce Logic Analyze'dan çıkın.

## Cihazları listeleme

Kitaplığın bulabildiği cihazları listelemek için şu komutu yazın:

```sh
dslcap --list
```

Bağlı her DSLogic cihazının USB tanımlayıcısını listelemek için şu komutu yazın:

```sh
dslcap --list-ids
```

`--list-ids` komutu yalnızca macOS'un USB cihazları hakkında tuttuğu bilgileri okur. Cihaza veri göndermez. Çıktı her cihaz için modeli, USB konumunu ve bir kayıt defteri tanımlayıcısını verir.

## Veri yakalama

Şu komut 10 MHz'de kanal 0 ve 1'de 1000000 örnek yakalar:

```sh
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000 --out /tmp/capture
```

Araç örnekleri `/tmp/capture.bin` dosyasına yazar. Bu adda bir dosya varsa araç bir hatayla durur. Araç bir dosyanın yerine yazmaz.

Yakalama seçenekleri şunlardır:

| Seçenek | İşlev | Başlangıç değeri |
| --- | --- | --- |
| `--channels LIST` | Kaydedilecek kanallar; örneğin `0,1,2`. | `0` |
| `--samplerate HZ` | Hz cinsinden örnekleme hızı. | `10000000` |
| `--samples N` | Her kanal için örnek sayısı. | `1000000` |
| `--vth VOLTS` | Eşik gerilimi. | `1.6` |
| `--mode MODE` | `buffer` veya `stream`. | `buffer` |
| `--trigger CH[:T]` | CH kanalında bir tetikleme. T için `R` (yükselen kenar), `F` (düşen kenar), `C` (yükselen kenar veya düşen kenar), `1` (yüksek seviye) veya `0` (düşük seviye) kullanın. | Tetikleme yok. Yalnızca CH verirseniz `R`. |
| `--trigpos PERCENT` | Örneklerin yüzdesi olarak tetikleme konumu. | `10` |
| `--timeout SEC` | Saniye cinsinden en uzun yakalama süresi. | `30` |
| `--out PATH` | `.bin` uzantısı olmadan çıktı dosyasının yolu. | Bu seçenek gereklidir. |
| `--log-level N` | Standart hata çıkışındaki kitaplık iletilerinin miktarı; 0 (yok) ile 5 (tümü) arası. | `1` |

Araç, cihazı kullanmadan önce tüm seçenekleri inceler. Bir seçenek doğru değilse araç durur ve bir hata verir.

## Çıktı dosyası

`.bin` dosyası kanalları en düşük numaradan başlayarak numara sırasıyla içerir. Her kanal için dosya o kanalın tüm örneklerini içerir. Her bayt 8 örnek tutar. İlk örnek en az anlamlı bittir. Her kanalın verileri tam sayıda 8 baytlık birim doldurur. Bu nedenle her kanal `ceil(samples / 64) × 8` bayt kullanır.

## JSON sonucu

Araç tek satıra bir JSON nesnesi yazar. Doğru bir yakalamadan sonra nesne cihaz adını, örnekleme hızını, örnek sayısını ve kanalları verir. Ayrıca eşik gerilimini, modu, tetiklemeyi, yakalama süresini ve `.bin` dosyasının yolunu verir. Yakalama doğru değilse nesne bir `error` anahtarı içerir. Bu durumda araç bir `.bin` dosyası oluşturmaz.

Sonucu yalnızca çıkış durumu 0 olduğunda ve JSON nesnesi tam olduğunda kullanın.

## Çıkış durumu

| Durum | Anlam |
| --- | --- |
| 0 | Yakalama tamamlandı. |
| 1 | İşlem sırasında bir hata oluştu; örneğin bir G/Ç hatası. |
| 2 | Bir seçenek doğru değil veya bir ayar cihazda kullanılamıyor. |
| 3 | Yakalama tamamlanmadı. |

## dslcap'i başlatan programlar için seçenekler

- `--parent-fd N`: Aracı başlatan program N tanımlayıcılı boruyu kapattığında araç durur. Sonra yakalama tamamlanmadıysa araç çıktı dosyasını kaldırır.
- `--res DIR`: Aygıt yazılımı dosyalarının bulunduğu klasör. Genellikle araç bu klasörü otomatik olarak bulur. `DSLCAP_RES` ortam değişkenini de ayarlayabilirsiniz.
- `--res-manifest FD`: Araç, her aygıt yazılımı dosyasını cihaza göndermeden önce dosyanın SHA-256 değerini inceler.

Kaynak koddaki `tools/dslcap/README.md` dosyası bu seçeneklerle ilgili tüm bilgileri verir.
