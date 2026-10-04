#!/usr/bin/env python3
"""Render the Logic Analyze manual from doc/manual/<lang>/*.md to HTML and PDF.

The script uses only the Python standard library. It reads a small Markdown
subset (the subset that the manual uses):

  # Chapter, ## Section, ### Subsection, optional {#id} at the end
  paragraphs, "1." and "-" lists (continuation lines indented 3 spaces)
  | tables |, ```code blocks```, ![caption](../figures/x.png)
  > [!WARNING] / [!CAUTION] / [!NOTE] blocks
  **bold**, *italic*, `code`, [text](NN-chapter.md) and [text](#id) links,
  <https://...> links, <!-- comments -->

A block between "<!-- edition: NAME -->" and "<!-- end edition -->" is only for
one edition of the app: "download" (the release from GitHub, the default) or
"appstore" (the Mac App Store edition, --edition appstore).

Output, for each language:

  <out>/<lang>/index.html    one page with all chapters
  <out>/figures/             the figures; figures/<lang>/ has the pictures of the app
                             in each language
  <out>/<lang>/logic-analyze-manual-<lang>.pdf   with --pdf

The PDF step prints the HTML page with a headless Chrome or Chromium. Set
CHROME to the browser executable if the script does not find it.

Usage:
  tools/manual/build_manual.py [--out DIR] [--pdf] [--lang en,de,...] [--edition appstore]
"""
import argparse
import html
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
SRC = os.path.join(ROOT, 'doc', 'manual')

CHROME_CANDIDATES = [
    '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome',
    '/Applications/Chromium.app/Contents/MacOS/Chromium',
    '/Applications/Microsoft Edge.app/Contents/MacOS/Microsoft Edge',
    '/Applications/Brave Browser.app/Contents/MacOS/Brave Browser',
    'google-chrome', 'chromium', 'chromium-browser', 'chrome',
]

# System fonts that cover each script. The PDF uses the fonts of the build machine.
FONT_STACK = {
    'zh-CN': '"PingFang SC", "Hiragino Sans GB", "Noto Sans CJK SC", "Microsoft YaHei", sans-serif',
    'zh-TW': '"PingFang TC", "Noto Sans CJK TC", "Microsoft JhengHei", sans-serif',
    'ja': '"Hiragino Sans", "Hiragino Kaku Gothic ProN", "Noto Sans CJK JP", "Yu Gothic", sans-serif',
    'ko': '"Apple SD Gothic Neo", "Noto Sans CJK KR", "Malgun Gothic", sans-serif',
}
DEFAULT_FONT = '-apple-system, "Helvetica Neue", "Segoe UI", "Noto Sans", Arial, sans-serif'


class Doc:
    def __init__(self, meta):
        self.meta = meta
        self.fig = 0
        self.toc = []          # (level, number, text, id)
        self.ids = set()
        self.chapter = 0
        self.section = 0
        self.sub = 0

    def uid(self, base):
        u, n = base, 2
        while u in self.ids:
            u = f'{base}-{n}'
            n += 1
        self.ids.add(u)
        return u


def inline(text, chapter_ids):
    out = []
    pos = 0
    # Code spans first: their content stays literal.
    for m in re.finditer(r'`([^`]+)`', text):
        out.append(inline_plain(text[pos:m.start()], chapter_ids))
        out.append('<code>' + html.escape(m.group(1)) + '</code>')
        pos = m.end()
    out.append(inline_plain(text[pos:], chapter_ids))
    return ''.join(out)


def inline_plain(t, chapter_ids):
    t = html.escape(t, quote=False)
    t = re.sub(r'&lt;(https?://[^&\s]+)&gt;', r'<a href="\1">\1</a>', t)

    def link(m):
        label, href = m.group(1), m.group(2)
        mm = re.match(r'^(\d\d-[\w-]+)\.md(?:#(.*))?$', href)
        if mm:
            href = '#' + (mm.group(2) or chapter_ids.get(mm.group(1), mm.group(1)))
        return f'<a href="{html.escape(href)}">{label}</a>'
    t = re.sub(r'\[([^\]]+)\]\(([^)\s]+)\)', link, t)
    t = re.sub(r'\*\*(.+?)\*\*', r'<strong>\1</strong>', t)
    t = re.sub(r'(?<![\w*])\*(?!\s)(.+?)(?<!\s)\*(?![\w*])', r'<em>\1</em>', t)
    return t


def render_chapter(md, stem, doc, chapter_ids):
    lines = md.split('\n')
    out = []
    i = 0
    labels = doc.meta['labels']

    def para_until(j):
        buf = []
        while j < len(lines):
            s = lines[j].strip()
            if not s or re.match(r'^(#|```|\||!\[|>|\d+\.\s|[-*]\s)', s) or s.startswith('<!--'):
                break
            buf.append(s)
            j += 1
        return ' '.join(buf), j

    while i < len(lines):
        line = lines[i]
        s = line.strip()
        if not s:
            i += 1
            continue
        if s.startswith('<!--'):
            while '-->' not in lines[i]:
                i += 1
            out.append(lines[i].strip() if s.endswith('-->') else '')
            i += 1
            continue
        m = re.match(r'^(#{1,3})\s+(.*?)\s*(?:\{#([\w-]+)\})?$', s)
        if m:
            level = len(m.group(1))
            text = m.group(2)
            if level == 1:
                doc.chapter += 1
                doc.section = doc.sub = 0
                num = f'{doc.chapter}'
                hid = doc.uid(chapter_ids[stem])
            elif level == 2:
                doc.section += 1
                doc.sub = 0
                num = f'{doc.chapter}.{doc.section}'
                hid = doc.uid(m.group(3) or f'{chapter_ids[stem]}-{doc.section}')
            else:
                doc.sub += 1
                num = f'{doc.chapter}.{doc.section}.{doc.sub}'
                hid = doc.uid(m.group(3) or f'{chapter_ids[stem]}-{doc.section}-{doc.sub}')
            doc.toc.append((level, num, inline(text, chapter_ids), hid))
            out.append(f'<h{level} id="{hid}"><span class="num">{num}</span> {inline(text, chapter_ids)}</h{level}>')
            i += 1
            continue
        if s.startswith('```'):
            lang = s[3:].strip()
            buf = []
            i += 1
            while i < len(lines) and not lines[i].strip().startswith('```'):
                buf.append(lines[i])
                i += 1
            i += 1
            out.append(f'<pre class="{html.escape(lang)}"><code>' + html.escape('\n'.join(buf)) + '</code></pre>')
            continue
        m = re.match(r'^!\[([^\]]*)\]\(([^)]+)\)', s)
        if m:
            doc.fig += 1
            cap = inline(m.group(1), chapter_ids)
            src = m.group(2)
            todo = i + 1 < len(lines) and 'TODO' in lines[i + 1]
            cls = ' class="old-ui"' if todo else ''
            out.append(f'<figure{cls}><img src="{html.escape(src)}" alt="{html.escape(m.group(1))}">'
                       f'<figcaption>{html.escape(labels["figure"])} {doc.fig}. {cap}</figcaption></figure>')
            i += 1
            continue
        if s.startswith('>'):
            buf = []
            while i < len(lines) and lines[i].strip().startswith('>'):
                buf.append(lines[i].strip()[1:].strip())
                i += 1
            kind = 'note'
            mm = re.match(r'^\[!(WARNING|CAUTION|NOTE)\]\s*(.*)$', buf[0]) if buf else None
            if mm:
                kind = mm.group(1).lower()
                buf[0] = mm.group(2)
            body = ' '.join(b for b in buf if b)
            out.append(f'<div class="admonition {kind}" role="note"><p class="label">{html.escape(labels[kind])}</p>'
                       f'<p>{inline(body, chapter_ids)}</p></div>')
            continue
        if s.startswith('|'):
            rows = []
            while i < len(lines) and lines[i].strip().startswith('|'):
                rows.append(lines[i].strip())
                i += 1
            cells = [[c.strip() for c in r.strip('|').split('|')] for r in rows]
            head = None
            if len(cells) > 1 and all(re.match(r'^:?-+:?$', c) for c in cells[1]):
                head, cells = cells[0], cells[2:]
            t = ['<table>']
            if head:
                t.append('<thead><tr>' + ''.join(f'<th>{inline(c, chapter_ids)}</th>' for c in head) + '</tr></thead>')
            t.append('<tbody>')
            for r in cells:
                t.append('<tr>' + ''.join(f'<td>{inline(c, chapter_ids)}</td>' for c in r) + '</tr>')
            t.append('</tbody></table>')
            out.append(''.join(t))
            continue
        m = re.match(r'^(\d+\.|[-*])\s+(.*)$', s)
        if m:
            ordered = m.group(1)[0].isdigit()
            tag = 'ol' if ordered else 'ul'
            items = []
            while i < len(lines):
                s2 = lines[i].strip()
                mm = re.match(r'^(\d+\.|[-*])\s+(.*)$', s2)
                if mm and (mm.group(1)[0].isdigit()) == ordered and not lines[i].startswith('   '):
                    items.append([mm.group(2)])
                    i += 1
                elif s2 and lines[i].startswith('   ') and items:
                    items[-1].append(s2)
                    i += 1
                else:
                    break
            first = int(m.group(1)[:-1]) if ordered else 1
            start = f' start="{first}"' if first != 1 else ''
            out.append(f'<{tag}{start}>' + ''.join(f'<li>{inline(" ".join(it), chapter_ids)}</li>' for it in items) + f'</{tag}>')
            continue
        text, i = para_until(i)
        if not text:
            # A line that the block rules did not take; keep it as text.
            text = s
            i += 1
        out.append(f'<p>{inline(text, chapter_ids)}</p>')
    return '\n'.join(out)


CSS = r"""
:root { --fg: #1b1f24; --muted: #57606a; --bg: #ffffff; --line: #d0d7de; --code: #f3f4f6;
        --warn: #b42318; --warn-bg: #fef3f2; --caut: #9a6700; --caut-bg: #fff8e5;
        --note: #0b5cad; --note-bg: #eef6ff; }
@media (prefers-color-scheme: dark) {
  :root { --fg: #e6edf3; --muted: #9ba7b4; --bg: #0d1117; --line: #30363d; --code: #161b22;
          --warn: #ff7b72; --warn-bg: #2d1214; --caut: #e3b341; --caut-bg: #2b2111;
          --note: #79c0ff; --note-bg: #0c2136; }
}
html { background: var(--bg); }
body { font-family: FONTS; color: var(--fg); background: var(--bg); line-height: 1.55;
       max-width: 52rem; margin: 0 auto; padding: 2rem 1rem 4rem; font-size: 16px; }
h1, h2, h3 { line-height: 1.25; }
h1 { font-size: 1.8rem; border-bottom: 2px solid var(--line); padding-bottom: .3rem; margin-top: 3rem; }
h2 { font-size: 1.35rem; margin-top: 2.2rem; }
h3 { font-size: 1.1rem; margin-top: 1.6rem; }
.num { color: var(--muted); margin-right: .4rem; }
.title { font-size: 2.2rem; border: 0; margin-top: 0; }
.subtitle { color: var(--muted); margin-top: -.8rem; }
code { font-family: ui-monospace, Menlo, Consolas, monospace; background: var(--code); padding: .05rem .3rem;
       border-radius: 4px; font-size: .92em; }
pre { background: var(--code); padding: .8rem 1rem; overflow-x: auto; border-radius: 6px; }
pre code { padding: 0; background: none; }
table { border-collapse: collapse; width: 100%; margin: 1rem 0; display: block; overflow-x: auto; }
th, td { border: 1px solid var(--line); padding: .35rem .6rem; text-align: left; vertical-align: top; }
th { background: var(--code); }
figure { margin: 1.2rem 0; }
figure img { max-width: 100%; height: auto; border: 1px solid var(--line); border-radius: 4px; background: #fff; }
figcaption { color: var(--muted); font-size: .9rem; margin-top: .3rem; }
.admonition { border-left: 5px solid; border-radius: 4px; padding: .5rem 1rem; margin: 1rem 0; }
.admonition p { margin: .3rem 0; }
.admonition .label { font-weight: 700; letter-spacing: .03em; }
.warning { border-color: var(--warn); background: var(--warn-bg); } .warning .label { color: var(--warn); }
.caution { border-color: var(--caut); background: var(--caut-bg); } .caution .label { color: var(--caut); }
.note { border-color: var(--note); background: var(--note-bg); } .note .label { color: var(--note); }
nav.toc ol { list-style: none; padding-left: 1rem; }
nav.toc > ol { padding-left: 0; }
nav.toc a { text-decoration: none; color: inherit; }
nav.toc a:hover { text-decoration: underline; }
a { color: var(--note); }
footer { margin-top: 4rem; color: var(--muted); font-size: .85rem; border-top: 1px solid var(--line); padding-top: 1rem; }
@media print {
  :root { --fg: #000; --bg: #fff; --code: #f3f4f6; --line: #bbb; }
  body { max-width: none; padding: 0; font-size: 10.5pt; }
  h1 { break-before: page; }
  h1.title { break-before: avoid; }
  h1, h2, h3 { break-after: avoid; }
  figure, .admonition, tr, pre { break-inside: avoid; }
  nav.toc { break-after: page; }
  a { color: inherit; text-decoration: none; }
  @page { size: A4; margin: 18mm 16mm; }
}
"""


EDITION_BLOCK = re.compile(r'<!-- edition: (\w+) -->\n(.*?)<!-- end edition -->\n', re.S)


def select_edition(md, edition):
    return EDITION_BLOCK.sub(lambda m: m.group(2) if m.group(1) == edition else '', md)


def build_lang(lang, out_dir, edition):
    src = os.path.join(SRC, lang)
    with open(os.path.join(src, 'manual.json'), encoding='utf-8') as f:
        meta = json.load(f)
    files = sorted(f for f in os.listdir(src) if re.match(r'^\d\d-.*\.md$', f))
    chapter_ids = {f[:-3]: re.sub(r'^\d\d-', '', f[:-3]) for f in files}
    doc = Doc(meta)
    body = []
    for f in files:
        with open(os.path.join(src, f), encoding='utf-8') as fh:
            body.append(f'<section>{render_chapter(select_edition(fh.read(), edition), f[:-3], doc, chapter_ids)}</section>')
    toc = ['<nav class="toc"><h2>' + html.escape(meta['labels']['contents']) + '</h2><ol>']
    for level, num, text, hid in doc.toc:
        if level <= 2:
            pad = '' if level == 1 else ' style="margin-left:1.2rem"'
            weight = ' style="font-weight:600"' if level == 1 else ''
            toc.append(f'<li{pad}><a href="#{hid}"{weight}><span class="num">{num}</span> {text}</a></li>')
    toc.append('</ol></nav>')
    fonts = FONT_STACK.get(lang, DEFAULT_FONT)
    if fonts != DEFAULT_FONT:
        fonts = fonts.replace(', sans-serif', ', ' + DEFAULT_FONT)
    page = f"""<!doctype html>
<html lang="{html.escape(meta['html_lang'])}">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>{html.escape(meta['title'])}</title>
<style>{CSS.replace('FONTS', fonts)}</style>
</head>
<body>
<header><h1 class="title">{html.escape(meta['title'])}</h1>
<p class="subtitle">{html.escape(meta['subtitle'])}</p></header>
{''.join(toc)}
{''.join(body)}
<footer>{html.escape(meta['footer'])}</footer>
</body>
</html>
"""
    lang_out = os.path.join(out_dir, lang)
    os.makedirs(lang_out, exist_ok=True)
    path = os.path.join(lang_out, 'index.html')
    with open(path, 'w', encoding='utf-8') as fh:
        fh.write(page)
    return path


def find_chrome():
    env = os.environ.get('CHROME')
    if env:
        return env
    for c in CHROME_CANDIDATES:
        if os.path.isabs(c) and os.access(c, os.X_OK):
            return c
        if not os.path.isabs(c) and shutil.which(c):
            return shutil.which(c)
    return None


def print_pdf(chrome, html_path, pdf_path, timeout=180):
    """Print with headless Chrome. On macOS, Chrome sometimes writes the PDF but
    does not exit, so the script also stops it when the file is complete."""
    if os.path.exists(pdf_path):
        os.remove(pdf_path)
    with tempfile.TemporaryDirectory() as profile:
        cmd = [chrome, '--headless', '--disable-gpu', '--no-first-run', '--no-default-browser-check',
               f'--user-data-dir={profile}', '--no-pdf-header-footer',
               f'--print-to-pdf={pdf_path}', 'file://' + os.path.abspath(html_path)]
        p = subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
        deadline = time.time() + timeout
        last = -1
        stable = 0
        while time.time() < deadline:
            if p.poll() is not None:
                break
            size = os.path.getsize(pdf_path) if os.path.exists(pdf_path) else -1
            stable = stable + 1 if size > 0 and size == last else 0
            last = size
            if stable >= 4:      # the same size for 2 s: Chrome is done
                break
            time.sleep(0.5)
        if p.poll() is None:
            p.terminate()
            try:
                p.wait(10)
            except subprocess.TimeoutExpired:
                p.kill()
        err = p.stderr.read().decode(errors='replace')
    if not os.path.exists(pdf_path) or os.path.getsize(pdf_path) == 0:
        sys.stderr.write(err)
        raise SystemExit(f'build_manual: PDF failed for {html_path}')


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--out', default=os.path.join(ROOT, 'build', 'manual'), help='output directory')
    ap.add_argument('--pdf', action='store_true', help='also print a PDF for each language (needs Chrome)')
    ap.add_argument('--lang', help='comma-separated language directories (default: all)')
    ap.add_argument('--edition', choices=('download', 'appstore'), default='download',
                    help='the edition of the app that the manual describes')
    args = ap.parse_args()

    langs = sorted(d for d in os.listdir(SRC) if os.path.exists(os.path.join(SRC, d, 'manual.json')))
    if args.lang:
        want = args.lang.split(',')
        missing = [l for l in want if l not in langs]
        if missing:
            raise SystemExit('build_manual: no manual for ' + ', '.join(missing))
        langs = want
    os.makedirs(args.out, exist_ok=True)
    fig_out = os.path.join(args.out, 'figures')
    if os.path.isdir(fig_out):
        shutil.rmtree(fig_out)
    shutil.copytree(os.path.join(SRC, 'figures'), fig_out)

    chrome = find_chrome() if args.pdf else None
    if args.pdf and not chrome:
        raise SystemExit('build_manual: --pdf needs Chrome or Chromium; set CHROME=/path/to/browser')
    for lang in langs:
        path = build_lang(lang, args.out, args.edition)
        line = path
        if chrome:
            pdf = os.path.join(os.path.dirname(path), f'logic-analyze-manual-{lang}.pdf')
            print_pdf(chrome, path, pdf)
            line += '  ' + pdf
        print(line)


if __name__ == '__main__':
    main()
