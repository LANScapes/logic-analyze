#!/usr/bin/env python3
"""Check Markdown manual text against a subset of the ASD-STE100 writing rules.

This script applies the rules. It does not contain the standard. The optional
word check reads your own copy of ASD-STE100 Issue 9 (the pdftotext output)
at run time, from --ste100 PATH or the STE100_TXT environment variable.
ASD owns the copyright of the standard. Do not add it to this repository.

Checks (rule numbers are ASD-STE100 Issue 9):
  long-procedural   a sentence in a procedure has more than 20 words (5.1)
  long-descriptive  a descriptive sentence has more than 25 words (6.3)
  passive           a form of "be" or "get" with a past participle (3.4, 3.6)
  ing               a word that ends in "-ing" that is not permitted (3.5)
  semicolon         a semicolon (8.1)
  paragraph         a paragraph with more than six sentences (6.6)
  modal             "should", "may", "might", "could", "would", "shall" (1.1, 3.4)
  contraction       a contraction such as "don't" (4.2)
  word              a word that is not approved, not a form of an approved word,
                    and not in the project term list (1.1). Only with --ste100.

Word count (8.5 to 8.7): text in parentheses, `code`, **labels**, quoted
text, numbers with units, and hyphenated words each count as one word.

Exit status: 0 when there are no findings, 1 when there are findings, 2 for
a usage error.
"""
import argparse
import json
import os
import re
import sys
from collections import Counter, defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
TERMS_FILE = os.path.join(HERE, 'ste-terms.txt')

MODALS = {'should', 'may', 'might', 'could', 'would', 'shall', 'ought'}
BE_FORMS = r'(?:is|are|was|were|be|been|being|am|get|gets|got|gotten|become|becomes|became)'
# Irregular past participles that do not end in -ed.
IRREGULAR_PP = {
    'begun', 'bent', 'bound', 'broken', 'brought', 'built', 'bought', 'caught', 'chosen', 'cut', 'dealt',
    'done', 'drawn', 'driven', 'eaten', 'fallen', 'felt', 'found', 'forgotten', 'frozen', 'given', 'gone',
    'grown', 'held', 'hidden', 'hit', 'hung', 'kept', 'known', 'laid', 'led', 'left', 'lent', 'lost', 'made',
    'meant', 'met', 'paid', 'put', 'read', 'run', 'said', 'seen', 'sent', 'set', 'shown', 'shut', 'sold',
    'spent', 'split', 'spread', 'stood', 'struck', 'taken', 'taught', 'thrown', 'told', 'thought', 'understood',
    'won', 'written', 'worn', 'withdrawn', 'upset', 'overridden', 'rewritten', 'reset', 'undone',
}
# Words that end in -ed but are not participles in normal use.
NOT_PP = {'need', 'speed', 'red', 'bed', 'feed', 'seed', 'shed', 'hundred', 'indeed', 'embed'}
# "-ing" words that STE permits (part 1, rule 3.5) or that are not verb forms.
ING_OK = {'during', 'something', 'anything', 'nothing', 'everything', 'thing', 'things', 'missing',
          'remaining', 'opening', 'openings', 'lighting', 'routing', 'servicing', 'mating', 'string',
          'strings', 'ring', 'spring', 'bring', 'king', 'swing', 'wing', 'sing', 'ping', 'ceiling'}

NUMBER_WORDS = {'zero', 'one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine', 'ten',
                'eleven', 'twelve', 'sixteen', 'twenty', 'hundred', 'thousand', 'second', 'third', 'fourth'}

STOP_ADVERBS = {'not', 'also', 'only', 'then', 'now', 'always', 'never', 'fully', 'automatically',
                'immediately', 'correctly', 'usually', 'again', 'still', 'all'}

IMPERATIVE_HINT = re.compile(r'^(?:[A-Z][a-z]+)')


def load_terms(path):
    """Project technical nouns and technical verbs, one per line; '#' starts a comment."""
    words = set()
    if not os.path.exists(path):
        return words
    with open(path, encoding='utf-8') as f:
        for line in f:
            line = line.split('#', 1)[0].strip()
            if not line:
                continue
            for w in re.findall(r"[A-Za-z][A-Za-z'\-]*", line):
                words.add(w.lower())
    return words


ENTRY = re.compile(r"^ ?([A-Za-z][A-Za-z'\-]*(?: [A-Za-z'\-]+){0,3}) \((n|v|adj|adv|pron|art|prep|conj|con)\),?(?:\s|$)")


def load_ste(path):
    """Read approved words and their listed forms from a pdftotext copy of the standard."""
    with open(path, encoding='utf-8', errors='replace') as f:
        lines = f.read().split('\n')
    starts = [i for i, l in enumerate(lines) if 'Part 2 – Dictionary' in l or 'Part 2 - Dictionary' in l]
    if not starts:
        sys.exit('ste_check: no dictionary found in ' + path)
    start = starts[0]
    approved = {}
    i = start
    while i < len(lines):
        m = ENTRY.match(lines[i])
        if not m:
            i += 1
            continue
        word, pos = m.group(1), m.group(2)
        if word != word.upper():
            i += 1
            continue
        key = word.lower()
        approved.setdefault(key, set()).add(pos)
        # The forms follow in column 1: "SHOWS,", "(LARGER,", "IS, WAS".
        for j in range(i + 1, min(i + 7, len(lines))):
            col1 = re.split(r'\s{2,}', lines[j].rstrip())[0]
            if not col1 or col1[0] == ' ':
                continue
            if ENTRY.match(lines[j]) or re.match(r'^[a-z]', col1):
                break
            for f in re.findall(r"[A-Z][A-Z'\-]+", col1):
                approved.setdefault(f.lower(), set()).add(pos)
        i += 1
    return approved


# ---------------------------------------------------------------- Markdown

def strip_markup(text):
    """Return (clean text, tokens counted as one word)."""
    text = re.sub(r'<!--.*?-->', ' ', text)
    text = re.sub(r'<https?://[^>]+>|https?://\S+', ' LABEL ', text)
    text = re.sub(r'!\[[^\]]*\]\([^)]*\)', ' ', text)            # images
    text = re.sub(r'\[([^\]]+)\]\([^)]*\)', r'\1', text)          # links keep their text
    text = re.sub(r'`[^`]*`', ' LABEL ', text)                     # code: one word
    text = re.sub(r'\*\*[^*]+\*\*', ' LABEL ', text)               # UI labels: one word
    text = re.sub(r'"[^"]*"|“[^”]*”', ' LABEL ', text)             # quoted text: one word
    text = re.sub(r'\([^()]*\)', ' PAREN ', text)                  # parentheses: one word
    text = re.sub(r'\*([^*]+)\*', r'\1', text)
    text = re.sub(r'\b\d[\d.,]*\s?(?:%|[kMGT]?(?:Hz|V|s|bit|bits|B|byte|bytes)|ms|µs|us|ns|x)\b', ' NUM ', text)
    return text


def split_blocks(md):
    """Yield (kind, text, line number). kind is 'proc' or 'desc'."""
    lines = md.split('\n')
    in_code = False
    in_comment = False
    para = []
    para_kind = 'desc'
    para_line = 0

    def flush():
        nonlocal para
        if para:
            yield_list.append((para_kind, ' '.join(para), para_line))
        para = []

    yield_list = []
    for n, raw in enumerate(lines, 1):
        line = raw.rstrip()
        if line.strip().startswith('```'):
            flush()
            in_code = not in_code
            continue
        if in_code:
            continue
        if '<!--' in line and '-->' not in line:
            in_comment = True
            continue
        if in_comment:
            if '-->' in line:
                in_comment = False
            continue
        s = line.strip()
        if not s:
            flush()
            continue
        if s.startswith('#'):
            flush()
            continue
        if s.startswith('|'):
            flush()
            if re.match(r'^\|[\s:\-|]+\|$', s):
                continue
            nxt = lines[n].strip() if n < len(lines) else ''
            if re.match(r'^\|[\s:\-|]+\|$', nxt):
                continue  # the header row of a table
            for cell in s.strip('|').split('|'):
                cell = cell.strip()
                if cell:
                    yield_list.append(('cell', cell, n))
            continue
        if s.startswith('!['):
            flush()
            continue
        if s.startswith('>'):
            s = s.lstrip('>').strip()
            s = re.sub(r'^\[!(WARNING|CAUTION|NOTE)\]\s*', '', s)
            if not s:
                continue
        m = re.match(r'^(\d+\.|[-*])\s+(.*)$', s)
        if m:
            flush()
            para_kind = 'proc' if m.group(1)[0].isdigit() else 'item'
            para_line = n
            para = [m.group(2)]
            continue
        if not para:
            para_line = n
            # An indented line after a list item continues it; a new paragraph is descriptive.
            para_kind = para_kind if raw.startswith('   ') and para_kind in ('proc', 'item') else 'desc'
        para.append(s)
    flush()
    return yield_list


def sentences(text):
    # Keep "e.g." style abbreviations out of the text: STE does not use them.
    parts = re.split(r'(?<=[.!?:])\s+(?=[A-Z0-9`*"“(\[])', text)
    return [p.strip() for p in parts if p.strip()]


def words_of(clean):
    return re.findall(r"[A-Za-z0-9][A-Za-z0-9'’\-/.]*", clean)


def check_file(path, args, approved, terms):
    with open(path, encoding='utf-8') as f:
        md = f.read()
    findings = []
    for kind, block, line in split_blocks(md):
        sents = sentences(block)
        if kind == 'desc' and len(sents) > 6:
            findings.append((line, 'paragraph', f'{len(sents)} sentences', block[:80]))
        for s in sents:
            clean = strip_markup(s)
            ws = [w.strip('.:,!?') for w in words_of(clean)]
            ws = [w for w in ws if w]
            n = len(ws)
            first = ws[0].lower() if ws else ''
            procedural = kind == 'proc' or (kind in ('item', 'desc') and first in args.imperatives)
            limit = 20 if procedural else 25
            if n > limit:
                findings.append((line, 'long-procedural' if procedural else 'long-descriptive',
                                 f'{n} words (limit {limit})', s))
            low = ' ' + clean.lower() + ' '
            for m in re.finditer(r'\b' + BE_FORMS + r'\s+(?:(\w+ly|not|also|only|then|now|always|never|all)\s+)?(\w+)\b', low):
                w = m.group(2)
                if (w.endswith('ed') and w not in NOT_PP) or w in IRREGULAR_PP:
                    if w in args.state_adjectives:
                        continue
                    findings.append((line, 'passive', m.group(0).strip(), s))
            for w in ws:
                lw = w.lower().strip("'’")
                if lw.endswith('ing') and lw not in ING_OK and lw not in terms and len(lw) > 4:
                    findings.append((line, 'ing', w, s))
                if lw in MODALS:
                    findings.append((line, 'modal', w, s))
                if re.search(r"n['’]t$|['’](ll|re|ve|d)$", lw):
                    findings.append((line, 'contraction', w, s))
            if ';' in clean:
                findings.append((line, 'semicolon', ';', s))
            if approved is not None:
                for idx, w in enumerate(ws):
                    if w in ('LABEL', 'PAREN', 'NUM'):
                        continue
                    if not re.match(r'^[A-Za-z]', w) or re.search(r'\d', w):
                        continue
                    if idx > 0 and w[0].isupper():
                        continue  # a name, a label or an abbreviation (rule 8.6)
                    if w.isupper() and len(w) > 1:
                        continue
                    if not known(w.lower(), approved, terms):
                        findings.append((line, 'word', w, s))
    return findings


def known(w, approved, terms):
    w = w.strip("'’-/.")
    if not w:
        return True
    if w in terms or w in NUMBER_WORDS:
        return True
    if '-' in w:
        return all(known(p, approved, terms) for p in w.split('-') if p)
    if '/' in w:
        return all(known(p, approved, terms) for p in w.split('/') if p)
    cands = {w}
    if w.endswith("'s") or w.endswith('’s'):
        cands.add(w[:-2])
    if w.endswith('ies'):
        cands.add(w[:-3] + 'y')
    if w.endswith('es'):
        cands.add(w[:-2])
    if w.endswith('s'):
        cands.add(w[:-1])
    for c in cands:
        if c in terms or c in NUMBER_WORDS:
            return True
        pos = approved.get(c)
        if pos is None:
            continue
        if c == w:
            return True
        if 'n' in pos or 'v' in pos:   # a plural noun or a third-person verb form
            return True
    # The forms of a technical verb follow the rules for approved verbs (rule 1.12).
    tv = TECH_VERBS(TERMS_FILE)
    for suffix, add in (('ed', ''), ('ed', 'e'), ('d', ''), ('s', ''), ('es', ''), ('ied', 'y')):
        if w.endswith(suffix) and (w[:-len(suffix)] + add) in tv:
            return True
    if w.endswith('ed') and len(w) > 4 and w[-3] == w[-4] and w[:-3] in tv:
        return True  # doubled final consonant: "dragged"
    return False


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('files', nargs='+')
    ap.add_argument('--ste100', default=os.environ.get('STE100_TXT'),
                    help='pdftotext copy of ASD-STE100 Issue 9 (or set STE100_TXT); enables the word check')
    ap.add_argument('--terms', default=TERMS_FILE, help='project technical nouns and verbs')
    ap.add_argument('--only', help='comma-separated list of checks to report')
    ap.add_argument('--summary', action='store_true', help='print only the counts')
    ap.add_argument('--json', action='store_true', help='print the counts as JSON')
    args = ap.parse_args()

    terms = load_terms(args.terms)
    approved = load_ste(args.ste100) if args.ste100 else None
    verbs = {w for w, p in (approved or {}).items() if 'v' in p}
    args.imperatives = verbs | {t for t in terms if t in TECH_VERBS(args.terms)}
    args.state_adjectives = set()

    total = Counter()
    per_file = defaultdict(Counter)
    out = []
    for path in args.files:
        for line, kind, what, s in check_file(path, args, approved, terms):
            if args.only and kind not in args.only.split(','):
                continue
            total[kind] += 1
            per_file[path][kind] += 1
            out.append(f'{path}:{line}: {kind}: {what}: {s}')
    if args.json:
        print(json.dumps({'total': dict(total), 'files': {k: dict(v) for k, v in per_file.items()}}, indent=1))
    else:
        if not args.summary:
            print('\n'.join(out))
        print('summary: ' + (', '.join(f'{k} {v}' for k, v in sorted(total.items())) or 'no findings')
              + ('' if approved is not None else ' (word check off: no --ste100)'))
    sys.exit(1 if total else 0)


_TV_CACHE = {}


def TECH_VERBS(path):
    """Words marked 'v' in the term list: '<word>  # v ...'."""
    if path in _TV_CACHE:
        return _TV_CACHE[path]
    out = _TV_CACHE[path] = set()
    if not os.path.exists(path):
        return out
    with open(path, encoding='utf-8') as f:
        for line in f:
            if '#' in line and re.search(r'#\s*v\b', line):
                for w in re.findall(r"[A-Za-z][A-Za-z'\-]*", line.split('#', 1)[0]):
                    out.add(w.lower())
    return out


if __name__ == '__main__':
    main()
