# Logic Analyze manual: source files

This folder contains the source of the user manual. The text uses ASD-STE100 Simplified Technical English (Issue 9).

- `en/` is the source text. Each file is one chapter.
- Each other folder is a translation: `de`, `es`, `fr`, `it`, `ja`, `ko`, `nl`, `pl`, `pt-BR`, `ru`, `tr`, `uk`, `vi`, `zh-CN`, `zh-TW`.
- `<lang>/manual.json` gives the title and the labels for the rendered page.
- `<lang>/term-list.md` gives the fixed translation of each technical name. It is not part of the rendered manual.
- `figures/` contains the figures that all languages use. `figures/<lang>/` contains the pictures of the app in each language, for example `figures/de/main-window.png`.

## Rules for the text

1. Write procedural sentences of 20 words or less. Write descriptive sentences of 25 words or less.
2. Write one instruction in each step. Use the imperative and the active voice.
3. Put a safety instruction before the step that it applies to. Use `> [!WARNING]` for a risk of injury or death. Use `> [!CAUTION]` for a risk of damage or data loss. Use `> [!NOTE]` for information only.
4. Use only the words that ASD-STE100 approves, or a technical name or a technical verb from `tools/manual/ste-terms.txt`. Use each term with one meaning only.
5. Write each label of the app in **bold**, with the exact text that the app shows.
6. Put `<!-- TODO: new screenshot -->` on the line after a figure that shows an old version of the app.

ASD owns the copyright of ASD-STE100. Do not copy the text or the dictionary of the standard into this repository.

## Check the English text

```sh
python3 tools/manual/ste_check.py doc/manual/en/*.md
```

The script finds long sentences, passive verbs, words that end in "-ing", modal verbs, contractions, semicolons and long paragraphs. To also find words that the STE dictionary does not approve, give the script your own copy of the standard as text:

```sh
pdftotext -layout ASD-STE100-Issue9.pdf /tmp/ste100.txt
STE100_TXT=/tmp/ste100.txt python3 tools/manual/ste_check.py doc/manual/en/*.md
```

## Make the pictures of the app

The language check `tools/lang_ui_check` makes the pictures of the app in each language. It uses the Demo Device and does not start a capture.

```sh
cmake -B build -DLANSCAPES_BRAND=ON -DLANG_UI_CHECK=ON
cmake --build build --target lang_ui_check
build/lang_ui_check/MacOS/lang_ui_check /tmp/lang-ui-check --shots /tmp/shots
python3 tools/manual/import_shots.py /tmp/shots
```

`import_shots.py` puts the pictures in `figures/<lang>/` and makes the files small. The check cannot make `trigger-position.png`, because the Demo Device does not send a trigger.

## Check the translations

After you change the English text, change each translation in the same way. Then do this check:

```sh
python3 tools/manual/check_parity.py
```

The script compares the headings, the steps, the safety instructions, the tables, the figures and the links of each translation with the English text.

## Make the HTML and PDF files

```sh
python3 tools/manual/build_manual.py              # HTML in build/manual/<lang>/index.html
python3 tools/manual/build_manual.py --pdf        # also build/manual/<lang>/logic-analyze-manual-<lang>.pdf
python3 tools/manual/build_manual.py --lang en,de # only some languages
python3 tools/manual/build_manual.py --edition appstore  # the Mac App Store edition
```

Text between `<!-- edition: download -->` and `<!-- end edition -->` is only for the release from GitHub. Text between `<!-- edition: appstore -->` and `<!-- end edition -->` is only for the Mac App Store edition. `package.sh` builds the App Store manual when it packages the App Store edition.

The script uses only the Python standard library. The PDF step uses Google Chrome or Chromium in headless mode. If the script does not find the browser, set `CHROME` to the path of the browser.

`packaging/macos/package.sh` puts the HTML manual in `Contents/Resources/manual` of the app. **Help** › **Manual...** opens `manual/<lang>/index.html` for the language of the user interface. If that language has no manual, the app opens the English manual.
