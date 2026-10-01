#!/usr/bin/env python3
"""Write a copy of a Qt resource list that takes brand files where they exist.

    brand_qrc.py QRC_IN DSVIEW_DIR BRAND_DIR QRC_OUT

Every <file>PATH</file> in QRC_IN keeps its resource path (as an alias) but is
read from BRAND_DIR when the brand tree has that file, and from DSView's own
tree otherwise. The code loads icons by resource path, so the branded build
gets the brand icons without touching upstream's files or code.
"""
import os
import re
import sys

qrc_in, dsview, brand, qrc_out = (os.path.abspath(a) for a in sys.argv[1:5])
sub = os.path.relpath(os.path.dirname(qrc_in), dsview)   # "." or "themes"
replaced = 0


def entry(m):
    global replaced
    indent, attrs, path = m.group(1), m.group(2) or "", m.group(3)
    if "alias=" in attrs:
        sys.exit(f"brand_qrc: aliases in {qrc_in} are not supported")
    src = os.path.normpath(os.path.join(brand, sub, path))
    if os.path.exists(src):
        replaced += 1
    else:
        src = os.path.join(os.path.dirname(qrc_in), path)
    return f'{indent}<file alias="{path}"{attrs}>{src}</file>'


text = open(qrc_in, encoding="utf-8").read()
text = re.sub(r"^(\s*)<file(\s[^>]*)?>([^<]+)</file>", entry, text, flags=re.M)
os.makedirs(os.path.dirname(qrc_out), exist_ok=True)
with open(qrc_out, "w", encoding="utf-8") as fh:
    fh.write(text)
print(f"brand_qrc: {os.path.basename(qrc_in)}: {replaced} brand files")
