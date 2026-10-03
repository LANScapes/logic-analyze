#!/usr/bin/env python3
"""Check the UI string tables against English.

Every language listed in lang_id_keys (DSView/pv/ui/langresource.h) must have
lang/<name>/ with the same files and ids as lang/en/, and each text must keep
the English text's placeholders (%1, %s, {0}), HTML tags, line breaks, "DSView"
(replaced by the brand name at run time), the brand name, accelerator count
and leading/trailing spaces. A language other than English must also have
DSView/languages/qt_<name>.qm and my_<name>.qm, listed in language.qrc.
Every L_S() id in the code must be in the English table of its page.

Usage: tools/check_lang.py   (exit status 1 on any error)
"""
import json, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LANG = os.path.join(ROOT, 'lang')
QM = os.path.join(ROOT, 'DSView', 'languages')
TOKENS = re.compile(r'%\d|%[sdlfu]|\{\d*\}|<[^>]+>|DSView|Logic Analyze(?!r)|\n')
errors = []


def tables(lang):
    d = os.path.join(LANG, lang)
    out = {}
    for base, _, files in os.walk(d):
        for f in files:
            if f.endswith('.json'):
                p = os.path.join(base, f)
                rel = os.path.relpath(p, d)
                try:
                    items = json.load(open(p, encoding='utf-8'))
                except ValueError as e:
                    errors.append('%s/%s: bad JSON: %s' % (lang, rel, e))
                    continue
                out[rel] = {x['id']: x['text'] for x in items}
    return out


def accelerators(s):
    return len(re.findall('&', s.replace('&&', '')))


def edge_spaces(s):
    return (s[:1].isspace(), s[-1:].isspace())


header = open(os.path.join(ROOT, 'DSView', 'pv', 'ui', 'langresource.h'), encoding='utf-8').read()
table = re.search(r'lang_id_keys\[\]\s*=\s*\{(.*?)\};', header, re.S).group(1)
names = list(dict.fromkeys(re.findall(r'\{\s*\w+,\s*"([^"]+)"', table)))
qrc = open(os.path.join(QM, 'language.qrc'), encoding='utf-8').read()

en = tables('en')
for lang in names:
    if not os.path.isdir(os.path.join(LANG, lang)):
        errors.append('%s: no lang/%s directory' % (lang, lang))
        continue
    if lang != 'en':
        for qm in ('qt_%s.qm' % lang, 'my_%s.qm' % lang):
            if not os.path.exists(os.path.join(QM, qm)) or '<file>%s</file>' % qm not in qrc:
                errors.append('%s: %s missing or not in language.qrc' % (lang, qm))
    if lang == 'en':
        continue
    tr = tables(lang)
    for f in sorted(set(en) | set(tr)):
        if f not in tr:
            errors.append('%s: missing file %s' % (lang, f))
            continue
        if f not in en:
            errors.append('%s: file %s has no English counterpart' % (lang, f))
            continue
        for k in sorted(set(en[f]) - set(tr[f])):
            errors.append('%s/%s: missing id %s' % (lang, f, k))
        for k in sorted(set(tr[f]) - set(en[f])):
            errors.append('%s/%s: id %s is not in English' % (lang, f, k))
        for k in sorted(set(en[f]) & set(tr[f])):
            a, b = en[f][k], tr[f][k]
            where = '%s/%s: %s' % (lang, f, k)
            if not b.strip():
                errors.append('%s: empty' % where)
            elif sorted(TOKENS.findall(a)) != sorted(TOKENS.findall(b)):
                errors.append('%s: placeholders/markup %s != %s' % (where, TOKENS.findall(a), TOKENS.findall(b)))
            elif a.count('&&') != b.count('&&') or accelerators(a) != accelerators(b):
                errors.append('%s: accelerators differ: %r -> %r' % (where, a, b))
            elif edge_spaces(a) != edge_spaces(b):
                errors.append('%s: leading/trailing space differs: %r -> %r' % (where, a, b))

# Every id the code looks up on a page must be in that page's English table.
PAGES = {'STR_PAGE_TOOLBAR': 'toolbar.json', 'STR_PAGE_MSG': 'msg.json', 'STR_PAGE_DLG': 'dlg.json'}
lookup = re.compile(r'L_S\(\s*(STR_PAGE_\w+)\s*,\s*S_ID\(\s*(\w+)\s*\)')
for base, _, files in os.walk(os.path.join(ROOT, 'DSView')):
    for f in files:
        if f.endswith(('.cpp', '.h')):
            src = open(os.path.join(base, f), encoding='utf-8', errors='replace').read()
            for page, sid in lookup.findall(src):
                if page in PAGES and sid not in en.get(PAGES[page], {}):
                    errors.append('%s: %s on %s is not in lang/en/%s'
                                  % (os.path.relpath(os.path.join(base, f), ROOT), sid, page, PAGES[page]))

for d in sorted(os.listdir(LANG)):
    if os.path.isdir(os.path.join(LANG, d)) and d not in names:
        errors.append('lang/%s is not listed in lang_id_keys' % d)

for e in errors:
    print('ERROR', e)
print('%d languages checked against English: %d errors' % (len(names) - 1, len(errors)))
sys.exit(1 if errors else 0)
