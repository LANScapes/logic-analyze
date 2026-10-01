#!/usr/bin/env python3
"""Generate the Logic Analyze UI icons for both themes.

    make_icons.py OUTDIR

Writes a tree that mirrors DSView/: OUTDIR/icons/{light,dark}/<name>.svg for
every icon below, OUTDIR/icons/<name>.svg for the theme-independent ones (USB),
and OUTDIR/themes/{light,dark}/{h,v}movetoolbar.svg for the toolbar grip.
The branded build compiles these in place of DSView's own files (see
brand_qrc.py); the plain DSView build keeps upstream's icons. Every icon is drawn
on a 24-unit grid as round-capped 1.75-unit strokes in one ink colour, with
colour kept for meaning only: green start, red stop and the USB speeds.

The names are the file names DSView already loads, so the set drops in place.
"""
import math
import os
import sys

INK = {"light": "#2A2A2A", "dark": "#D7D7D7"}   # the colours DSView's own icons use
GREEN, RED = "#1FA05A", "#D64545"
USB2, USB3 = "#1FA05A", "#2F86D8"
SW = 1.75


def svg(body):
    return ('<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" '
            f'fill="none" stroke-width="{SW}" stroke-linecap="round" stroke-linejoin="round">'
            f"{body}</svg>\n")


def p(d, c="{ink}", extra=""):
    return f'<path d="{d}" stroke="{c}"{extra}/>'


def fill(d, c="{ink}"):
    return f'<path d="{d}" fill="{c}" stroke="none"/>'


def circle(cx, cy, r, c="{ink}", filled=False):
    if filled:
        return f'<circle cx="{cx}" cy="{cy}" r="{r}" fill="{c}" stroke="none"/>'
    return f'<circle cx="{cx}" cy="{cy}" r="{r}" stroke="{c}"/>'


def rect(x, y, w, h, rx, c="{ink}"):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{rx}" stroke="{c}"/>'


def gear():
    pts = []
    for i in range(16):
        a = math.pi * 2 * i / 16 - math.pi / 16
        r = 9 if i % 2 == 0 else 6.8
        for da in (-0.13, 0.13):
            pts.append((12 + r * math.cos(a + da), 12 + r * math.sin(a + da)))
    d = "M" + " L".join(f"{x:.2f} {y:.2f}" for x, y in pts) + " Z"
    return p(d) + circle(12, 12, 3)


def sun():
    rays = "".join(
        f"M{12 + 7 * math.cos(a):.2f} {12 + 7 * math.sin(a):.2f} "
        f"L{12 + 9.5 * math.cos(a):.2f} {12 + 9.5 * math.sin(a):.2f} "
        for a in (math.pi * k / 4 for k in range(8)))
    return circle(12, 12, 4) + p(rays)


def usb(c):
    return (p("M5 12 H18.5", c) + circle(4.2, 12, 1.7, c, True)
            + fill("M18 9 L22 12 L18 15 Z", c)
            + p("M8.5 12 L11 7.5 H13.5", c) + circle(14.8, 7.5, 1.4, c, True)
            + p("M11.5 12 L14 16.5 H15.5", c) + fill("M15.5 15.2 H18.1 V17.8 H15.5 Z", c))


def lissajous():
    """A 3:2 Lissajous figure, the shape an X-Y view draws."""
    pts = [(12 + 7.5 * math.sin(3 * t + math.pi / 2), 12 + 7.5 * math.sin(2 * t))
           for t in (2 * math.pi * k / 120 for k in range(121))]
    return "M" + " L".join(f"{x:.2f} {y:.2f}" for x, y in pts) + " Z"



SQUARE = "M3 17 H6.5 V7 H11 V17 H15 V7 H19 V17 H21"
SINE = "M3 12 C5 3.5 7 3.5 9 12 C11 20.5 13 20.5 15 12 C17 3.5 19 3.5 21 12"
NOISE = ("M3 12 L4.5 10 L6 14 L7.5 6.5 L9 17.5 L10.5 4.5 L12 19.5 L13.5 6 L15 16.5 "
         "L16.5 8.5 L18 14.5 L19.5 10.5 L21 12")
EYE = "M2.5 12 C5 7 8.5 5 12 5 C15.5 5 19 7 21.5 12 C19 17 15.5 19 12 19 C8.5 19 5 17 2.5 12 Z"
FOLDER = ("M3 7 C3 5.9 3.9 5 5 5 H9.3 L11.3 7 H19 C20.1 7 21 7.9 21 9 V17 C21 18.1 20.1 19 19 19 "
          "H5 C3.9 19 3 18.1 3 17 Z")

HELP = (circle(12, 12, 9) + p("M9.4 9.4 C9.4 7.9 10.6 6.9 12 6.9 C13.5 6.9 14.6 7.9 14.6 9.3 "
                              "C14.6 10.6 13.7 11.1 12.9 11.6 C12.3 12 12 12.5 12 13.3 V13.7")
        + circle(12, 16.9, 1.1, filled=True))

# name: (drawing, what it is for). Drawings use {ink} for the theme colour.
ICONS = {
    # sampling bar
    "start": (rect(2.5, 5.5, 19, 13, 6.5, GREEN) + fill("M10 9 L15.5 12 L10 15 Z", GREEN),
              "Start a capture"),
    "stop": (rect(2.5, 5.5, 19, 13, 6.5, RED) + fill("M9.5 9.5 H14.5 V14.5 H9.5 Z", RED),
             "Stop a capture"),
    "single": (rect(2.5, 5.5, 19, 13, 6.5) + fill("M13 7.8 L9 12.8 H12 L11 16.2 L15 11.2 H12 Z"),
               "Instant: capture now, without waiting for a trigger"),
    "once": (p("M3 12 H15.5 M11.5 8 L15.5 12 L11.5 16 M20 6 V18"), "Mode: single capture"),
    "repeat": (p("M17 7 H8 C5.8 7 4 8.8 4 11 V12 M14 4 L17 7 L14 10 "
                 "M7 17 H16 C18.2 17 20 15.2 20 13 V12 M10 14 L7 17 L10 20"), "Mode: repeat"),
    "loop": (p("M12 12 C9 8.3 4 8.3 4 12 C4 15.7 9 15.7 12 12 C15 8.3 20 8.3 20 12 "
               "C20 15.7 15 15.7 12 12 Z"), "Mode: loop (stream)"),
    "params": (p("M4 7 H7 M11 7 H20 M4 12 H13 M17 12 H20 M4 17 H5 M9 17 H20")
               + circle(9, 7, 2) + circle(15, 12, 2) + circle(7, 17, 2), "Device options"),
    # trigger bar
    "trigger": (p("M3 18 H9 V6 H21") + p("M6.5 11.5 L9 9 L11.5 11.5"), "Trigger settings"),
    "protocol": (p("M3 8 H6 L8 5 H16 L18 8 H21 M6 8 L8 11 H16 L18 8")
                 + p("M3 19 H5.5 V15 H8.5 V19 H11.5 V15 H14.5 V19 H17.5 V15 H21"),
                 "Decode: protocol decoders"),
    "measure": (p("M3 18 H8 V9 H16 V18 H21") + p("M8 3.5 V6.5 M16 3.5 V6.5 M8 5 H16"),
                "Measure: cursors and timing"),
    "search-bar": (circle(10.5, 10.5, 6.5) + p("M15.2 15.2 L20.5 20.5")
                   + p("M7 12.5 H9 V8.5 H12 V12.5 H14", extra=' stroke-width="1.5"'),
                   "Search the capture for a pattern"),
    "function": (p("M14.5 4 C11.8 4 11 5.3 10.7 7.6 L9.3 16.4 C9 18.7 8.2 20 5.5 20 M7.5 10 H13.5")
                 + p("M15 13 L19.5 18.5 M19.5 13 L15 18.5"), "Oscilloscope functions (FFT, math)"),
    "display": (rect(3, 4, 18, 12.5, 2) + p("M9 20 H15 M12 16.5 V20")
                + p("M6 12.5 H8.5 V8 H12.5 V12.5 H15 V8 H18", extra=' stroke-width="1.5"'),
                "Display options"),
    "fft": (p("M4 4 V20 H20 M8 17 V13 M11 17 V7 M14 17 V10 M17 17 V14.5"), "FFT"),
    "math": (p("M7 4 V10 M4 7 H10 M14 7 H20 M4.5 14.5 L9.5 19.5 M9.5 14.5 L4.5 19.5 M14 17 H20")
             + circle(17, 14.2, 0.9, filled=True) + circle(17, 19.8, 0.9, filled=True), "Math"),
    "lissajous": (p(lissajous()), "Lissajous (X-Y) view"),
    # file bar
    "file": (p(FOLDER), "File menu"),
    "open": (p("M3 17 V7 C3 5.9 3.9 5 5 5 H9.3 L11.3 7 H17 C18.1 7 19 7.9 19 9 V10.5")
             + p("M3 17 L5.4 11.6 C5.7 10.9 6.4 10.5 7.2 10.5 H20.4 C21.1 10.5 21.6 11.2 21.3 11.9 "
                 "L19.2 17.7 C18.9 18.5 18.2 19 17.4 19 H5 C3.9 19 3 18.1 3 17 Z"), "Open"),
    "save": (p("M6 4 H16 L20 8 V18 C20 19.1 19.1 20 18 20 H6 C4.9 20 4 19.1 4 18 V6 C4 4.9 4.9 4 6 4 Z")
             + p("M8 4 V8.5 H15 V4 M8 20 V14 H16 V20"), "Save"),
    "export": (p("M14 4 H20 V10 M20 4 L11 13 M18 14 V18 C18 19.1 17.1 20 16 20 H6 C4.9 20 4 19.1 4 18 "
                 "V8 C4 6.9 4.9 6 6 6 H10"), "Export"),
    "capture": (p("M4 8.5 C4 7.4 4.9 6.5 6 6.5 H8 L9.5 4.5 H14.5 L16 6.5 H18 C19.1 6.5 20 7.4 20 8.5 "
                  "V17 C20 18.1 19.1 19 18 19 H6 C4.9 19 4 18.1 4 17 Z") + circle(12, 12.5, 3.5),
                "Screenshot"),
    # help menu (logo bar)
    # Help is the standard circled question mark. DSView swaps these two files
    # when a device connects; the USB button already shows that, so both match.
    "logo_color": (HELP, "Help menu (device connected)"),
    "logo_noColor": (HELP, "Help menu (no device)"),
    "about": (circle(12, 12, 9) + p("M12 11 V16.5") + circle(12, 7.8, 1.1, filled=True), "About"),
    "manual": (p("M12 6.5 C12 5.1 10.9 4 9.5 4 H4 V18 H9.5 C10.9 18 12 19.1 12 20.5 "
                 "C12 19.1 13.1 18 14.5 18 H20 V4 H14.5 C13.1 4 12 5.1 12 6.5 Z M12 6.5 V20.5"),
               "User guide"),
    "bug": (p("M12 8.5 C9.2 8.5 7.5 10.8 7.5 14 C7.5 17.6 9.4 20 12 20 C14.6 20 16.5 17.6 16.5 14 "
              "C16.5 10.8 14.8 8.5 12 8.5 Z M12 11 V20 M9.5 8.8 C9.5 7.2 10.6 6 12 6 C13.4 6 14.5 7.2 14.5 8.8 "
              "M10.2 6.4 L8.5 4.2 M13.8 6.4 L15.5 4.2 M7.5 12.5 H4.5 M7.6 16 H4.5 M16.5 12.5 H19.5 "
              "M16.4 16 H19.5 M8.4 18.6 L6 20.5 M15.6 18.6 L18 20.5"), "Report an issue"),
    "update": (p("M19.5 9 A8 8 0 0 0 5.6 7 M4.5 15 A8 8 0 0 0 18.4 17 M19.8 4.2 V9.2 H14.8 "
                 "M4.2 19.8 V14.8 H9.2"), "Check for updates"),
    "log": (p("M7 3 H14 L19 8 V19 C19 20.1 18.1 21 17 21 H7 C5.9 21 5 20.1 5 19 V5 C5 3.9 5.9 3 7 3 Z "
              "M14 3 V8 H19 M8.5 12 H15.5 M8.5 15 H15.5 M8.5 18 H12.5"), "Log"),
    "dark": (p("M20 14.5 A8 8 0 1 1 9.5 4 A6.5 6.5 0 0 0 20 14.5 Z"), "Dark theme"),
    "light": (sun(), "Light theme"),
    # device modes (top-left of the waveform)
    "la": (p(SQUARE), "Logic analyzer mode"),
    "square-la": (p(SQUARE), "Logic analyzer mode (menu)"),
    "osc": (p(SINE), "Oscilloscope mode"),
    "square-osc": (p(SINE), "Oscilloscope mode (menu)"),
    "daq": (p(NOISE), "Data acquisition mode"),
    "square-daq": (p(NOISE), "Data acquisition mode (menu)"),
    # panels and small controls
    "gear": (gear(), "Settings"),
    "search": (circle(10.5, 10.5, 6.5) + p("M15.2 15.2 L20.5 20.5"), "Search"),
    "nav": (p("M4 11 L20 4 L13 20 L11 13 Z"), "Go to the decoded item"),
    "shown": (p(EYE) + circle(12, 12, 3), "Shown"),
    "hidden": (p(EYE) + p("M4 4 L20 20"), "Hidden"),
    "add": (p("M12 5 V19 M5 12 H19"), "Add"),
    "del": (p("M6.5 6.5 L17.5 17.5 M17.5 6.5 L6.5 17.5"), "Remove"),
    "close": (p("M6 6 L18 18 M18 6 L6 18"), "Close"),
    "next": (p("M9 5 L16 12 L9 19"), "Next"),
    "pre": (p("M15 5 L8 12 L15 19"), "Previous"),
    "minimize": (p("M5 12 H19"), "Minimize (window title bar)"),
    "maximize": (rect(5, 5, 14, 14, 1.5), "Maximize (window title bar)"),
    "restore": (rect(4, 8, 12, 12, 1.5) + p("M8 8 V5.5 C8 4.7 8.7 4 9.5 4 H18.5 C19.3 4 20 4.7 20 5.5 "
                                            "V14.5 C20 15.3 19.3 16 18.5 16 H16"),
                "Restore (window title bar)"),
}
# Theme-independent icons, in the icons folder itself.
ROOT = {
    "usb2": (usb(USB2), "Connected over USB 2.0"),
    "usb3": (usb(USB3), "Connected over USB 3.0"),
}


GRIP = {"light": "#80858c", "dark": "#8a8f96"}


def grip(w, h, cols, rows, color):
    """A macOS-style drag handle: a grid of dots (2 x 3, or 3 x 2 on a vertical bar)."""
    step, r = 5, 1.3
    x0 = w / 2 - step * (cols - 1) / 2
    y0 = h / 2 - step * (rows - 1) / 2
    dots = "".join(f'<circle cx="{x0 + c * step:.1f}" cy="{y0 + rw * step:.1f}" r="{r}" fill="{color}"/>'
                   for c in range(cols) for rw in range(rows))
    return (f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}px" height="{h}px" '
            f'viewBox="0 0 {w} {h}">{dots}</svg>\n')


def write(path, text):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as fh:
        fh.write(text)


def main():
    out = sys.argv[1]
    for theme, ink in INK.items():
        for name, (body, _) in ICONS.items():
            write(os.path.join(out, "icons", theme, name + ".svg"), svg(body.replace("{ink}", ink)))
        write(os.path.join(out, "themes", theme, "hmovetoolbar.svg"), grip(16, 64, 2, 3, GRIP[theme]))
        write(os.path.join(out, "themes", theme, "vmovetoolbar.svg"), grip(54, 16, 3, 2, GRIP[theme]))
    for name, (body, _) in ROOT.items():
        write(os.path.join(out, "icons", name + ".svg"), svg(body))
    print(f"{len(ICONS)} themed icons x 2 themes, {len(ROOT)} root icons, grips -> {out}")


if __name__ == "__main__":
    main()
