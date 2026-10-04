#!/usr/bin/env python3
"""Compare the structure of each translation with the English manual.

For each chapter file, the translation must have the same number of headings,
list items, figures, safety instructions, table rows and code blocks, the same
figure paths, heading ids and chapter links, and the same code blocks. This
catches a step, a warning or a figure that a translation lost or added.

Usage: tools/manual/check_parity.py [lang ...]   (default: all languages)
Exit status 1 if a difference is found.
"""
import os
import re
import sys

SRC = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'doc', 'manual')


def shape(path):
    with open(path, encoding='utf-8') as f:
        md = f.read()
    code = re.findall(r'```.*?```', md, re.S)
    body = re.sub(r'```.*?```', '', md, flags=re.S)
    lines = body.split('\n')
    lang = os.path.basename(os.path.dirname(path))
    return {
        'h1': sum(1 for l in lines if re.match(r'^# ', l)),
        'h2': sum(1 for l in lines if re.match(r'^## ', l)),
        'h3': sum(1 for l in lines if re.match(r'^### ', l)),
        'steps': sum(1 for l in lines if re.match(r'^\d+\. ', l)),
        'bullets': sum(1 for l in lines if re.match(r'^- ', l)),
        'table rows': sum(1 for l in lines if l.startswith('|')),
        'WARNING': body.count('[!WARNING]'),
        'CAUTION': body.count('[!CAUTION]'),
        'NOTE': body.count('[!NOTE]'),
        'TODO': body.count('TODO: new screenshot'),
        # A figure in figures/<lang>/ shows the app in that language.
        'figures': [f.replace(f'figures/{lang}/', 'figures/<lang>/')
                    for f in re.findall(r'!\[[^\]]*\]\(([^)]+)\)', body)],
        'editions': re.findall(r'<!-- (edition: \w+|end edition) -->', body),
        'ids': re.findall(r'\{#([\w-]+)\}', body),
        'links': sorted(re.findall(r'\]\((\d\d-[\w-]+\.md|#[\w-]+)\)', body)),
        'code': code,
    }


def main():
    langs = sys.argv[1:] or sorted(d for d in os.listdir(SRC)
                                   if d != 'en' and os.path.exists(os.path.join(SRC, d, 'manual.json')))
    en_dir = os.path.join(SRC, 'en')
    chapters = sorted(f for f in os.listdir(en_dir) if re.match(r'^\d\d-.*\.md$', f))
    bad = 0
    for lang in langs:
        for ch in chapters:
            p = os.path.join(SRC, lang, ch)
            if not os.path.exists(p):
                print(f'{lang}/{ch}: missing')
                bad += 1
                continue
            a, b = shape(os.path.join(en_dir, ch)), shape(p)
            for k in a:
                if a[k] != b[k]:
                    shown = (a[k], b[k]) if not isinstance(a[k], list) else ('differs', '')
                    print(f'{lang}/{ch}: {k}: en {shown[0]} {lang} {shown[1]}')
                    bad += 1
        extra = sorted(set(f for f in os.listdir(os.path.join(SRC, lang)) if re.match(r'^\d\d-', f)) - set(chapters))
        for f in extra:
            print(f'{lang}/{f}: not in English')
            bad += 1
    print(f'parity: {len(langs)} languages, {bad} differences')
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
