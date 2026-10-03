#!/usr/bin/env python3
"""Copy the manual figures that lang_ui_check --shots made into doc/manual/figures/<lang>/.

  build/lang_ui_check/MacOS/lang_ui_check out --shots /tmp/shots
  python3 tools/manual/import_shots.py /tmp/shots

The script renames the app's language folders to the manual's folders (cn to
zh-CN and so on), makes each picture at most MAX_WIDTH pixels wide and makes the
files small. It uses pngquant or oxipng if they are installed, else Pillow if it
is installed, else it copies the files as they are.
"""
import os
import shutil
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
DEST = os.path.join(ROOT, 'doc', 'manual', 'figures')
LANGS = {'cn': 'zh-CN', 'zh_TW': 'zh-TW', 'pt_BR': 'pt-BR'}
MAX_WIDTH = 1100

try:
    from PIL import Image
except ImportError:
    Image = None


def shrink(path):
    if Image:
        im = Image.open(path)
        if im.width > MAX_WIDTH:
            im = im.resize((MAX_WIDTH, round(im.height * MAX_WIDTH / im.width)), Image.LANCZOS)
        if not shutil.which('pngquant'):
            # A screenshot of the app has few colors; a 256-color palette is not visible.
            im = im.convert('RGBA').quantize(256, method=Image.Quantize.FASTOCTREE)
        im.save(path, optimize=True)
    if shutil.which('pngquant'):
        subprocess.run(['pngquant', '--force', '--skip-if-larger', '--strip', '--ext', '.png', path], check=False)
    if shutil.which('oxipng'):
        subprocess.run(['oxipng', '-q', '-o', '4', '--strip', 'safe', path], check=False)


def main():
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    src = sys.argv[1]
    manual_langs = {d for d in os.listdir(os.path.dirname(DEST))
                    if os.path.exists(os.path.join(os.path.dirname(DEST), d, 'manual.json'))}
    for app_lang in sorted(os.listdir(src)):
        lang = LANGS.get(app_lang, app_lang)
        if lang not in manual_langs:
            continue
        os.makedirs(os.path.join(DEST, lang), exist_ok=True)
        for name in sorted(os.listdir(os.path.join(src, app_lang))):
            if name.endswith('.png'):
                out = os.path.join(DEST, lang, name)
                shutil.copyfile(os.path.join(src, app_lang, name), out)
                shrink(out)
        print(lang)


if __name__ == '__main__':
    main()
