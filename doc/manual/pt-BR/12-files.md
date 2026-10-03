# Arquivos e sessões

Clique em **Arquivo** na barra de ferramentas para abrir o menu de arquivo. O menu tem estes itens:

- **Config.**: um menu para carregar e salvar sessões.
- **Abrir...**: abrir um arquivo de dados.
- **Salvar...**: salvar os dados da captura.
- **Exportar...**: exportar os dados para um outro formato.
- **Captura de tela...**: salvar uma imagem da janela.

## Sessões

Um arquivo de sessão contém as configurações, mas não contém os dados da captura. Uma sessão contém as opções do dispositivo, os canais ativados, os nomes e as cores dos canais e as configurações do gatilho. Um arquivo de sessão tem a extensão `.dsc`.

### Salvar uma sessão

1. Clique em **Arquivo** › **Config.** › **Salvar sessão**.
2. Selecione a pasta e digite o nome do arquivo.
3. Clique em **Salvar**.

### Carregar uma sessão

1. Clique em **Arquivo** › **Config.** › **Carregar sessão**.
2. Selecione o arquivo de sessão.
3. Clique em **Abrir**.

### Voltar às configurações iniciais

Clique em **Arquivo** › **Config.** › **Carregar sessão padrão**. O aplicativo define todas as configurações do dispositivo com os seus valores iniciais.

O aplicativo salva as configurações automaticamente quando você o fecha. Quando você inicia o aplicativo de novo, ele carrega as configurações da última sessão.

## Salvar os dados

1. Clique em **Arquivo** › **Salvar...**.
2. Selecione a pasta e digite o nome do arquivo.
3. Clique em **Salvar**.

O aplicativo salva os dados e as configurações em um arquivo com a extensão `.dsl`. Você pode abrir este arquivo de novo no Logic Analyze.

> [!CAUTION]
> O aplicativo não salva os dados automaticamente. Salve os dados antes de iniciar uma nova captura ou de fechar o aplicativo. Uma nova captura substitui os dados da captura anterior.

## Abrir um arquivo de dados

1. Clique em **Arquivo** › **Abrir...**.
2. Selecione um arquivo com a extensão `.dsl`.
3. Clique em **Abrir**.

O aplicativo mostra os dados na área da forma de onda. O rótulo do tipo de dispositivo mostra **Arquivo**.

## Exportar os dados

A exportação gera um arquivo que outros programas podem ler.

1. Clique em **Arquivo** › **Exportar...**. A janela **Exportar** abre.
2. Clique em **caminho**.
3. Selecione a pasta, digite o nome do arquivo e selecione o formato.
4. Clique em **Salvar**.
5. Se o formato for CSV, selecione **Dados originais** ou **Dados compactados**. Os dados compactados contêm uma linha somente para cada mudança de valor.
6. Clique em **OK**.

No modo analisador lógico, estes formatos estão disponíveis:

| Formato | Extensão | Uso |
| --- | --- | --- |
| CSV | `.csv` | Programas de planilha e scripts. |
| VCD | `.vcd` | Programas de forma de onda, por exemplo o GTKWave. |
| Gnuplot | `.gnuplot` | O programa Gnuplot. |
| srzip | `.srzip` | Programas do sigrok, por exemplo o PulseView. |

No modo osciloscópio e no modo aquisição de dados, somente o CSV está disponível.

![A janela de exportação para CSV](../figures/export-csv.png)
<!-- TODO: new screenshot -->

## Salvar uma imagem da janela

1. Clique em **Arquivo** › **Captura de tela...**.
2. Selecione a pasta e digite o nome do arquivo.
3. Selecione PNG ou JPEG.
4. Clique em **Salvar**.
