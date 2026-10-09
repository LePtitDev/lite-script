#!/usr/bin/env python3
"""Generate the HTML and PDF versions of the cheatsheets of docs/cheatsheet.

Usage: python scripts/cheatsheet.py [names...] [--no-pdf] [--browser PATH]

Each "<name>.md" gives "<name>.html" and "<name>.pdf" (every ".md" file of the
folder by default, for example: python scripts/cheatsheet.py assembly-cheatsheet).

Only the standard library is needed. The PDF is printed by a Chromium based
browser in headless mode (Edge or Chrome, found automatically).
"""

import argparse
import html
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
FOLDER = ROOT / "docs" / "cheatsheet"

KEYWORDS = {
    "var", "function", "class", "static", "operator", "new", "this", "return", "if", "else",
    "for", "while", "do", "switch", "case", "default", "break", "continue", "try", "catch",
    "throw", "namespace", "true", "false", "null", "undefined", "global",
}

TOKEN = re.compile(r"""
    (?P<comment>//[^\n]*|/\*.*?\*/|\#[^\n]*)
  | (?P<string>"(?:\\.|[^"\\\n])*"|'(?:\\.|[^'\\\n])*')
  | (?P<number>\b\d+(?:\.\d+)?(?:e-?\d+)?\b|\.\d+(?:e-?\d+)?\b)
  | (?P<word>[A-Za-z_$][\w$]*)
""", re.VERBOSE | re.DOTALL)


ASM_ARGUMENT = re.compile(r"^[+-]?(\d+(\.\d*)?|\.\d+)(e[+-]?\d+)?$|^(true|false)$")


def highlight_asm(code):
    """Assembly: one line per instruction, numbered by CSS.

    The annotations ("; text") aren't syntax: they can't be selected, so that
    the copied code can be assembled. They aren't allowed after value.string.
    """
    lines = []
    for line in code.split("\n"):
        note = ""
        if not line.lstrip().startswith("value.string"):
            m = re.search(r"\s*;.*$", line)
            if m:
                note = f'<span class="c note">{html.escape(m.group())}</span>'
                line = line[:m.start()]
        name, _, argument = line.partition(" ")
        text = f'<span class="k">{html.escape(name)}</span>' if name else ""
        if argument:
            cls = "n" if ASM_ARGUMENT.match(argument.strip()) else "s"
            text += f' <span class="{cls}">{html.escape(argument)}</span>'
        lines.append(f'<span class="ln">{text}{note}</span>')
    return "\n".join(lines)


def highlight(code, lang):
    """Minimal syntax coloring for LiteScript, assembly and shell code."""
    if lang == "asm":
        return highlight_asm(code)
    out, last = [], 0
    for m in TOKEN.finditer(code):
        kind = m.lastgroup
        text = m.group()
        if kind == "comment" and text.startswith("#") and lang != "bash":
            continue
        if kind == "comment" and text.startswith("//") and lang == "bash":
            continue
        out.append(html.escape(code[last:m.start()]))
        if kind == "word":
            if lang != "bash" and text in KEYWORDS:
                out.append(f'<span class="k">{text}</span>')
            elif lang == "bash" and text == "litescript":
                out.append(f'<span class="k">{text}</span>')
            else:
                out.append(html.escape(text))
        else:
            out.append(f'<span class="{kind[0]}">{html.escape(text)}</span>')
        last = m.end()
    out.append(html.escape(code[last:]))
    return "".join(out)


def inline(text):
    """Inline Markdown: code spans, bold, links and escaped pipes."""
    # The code spans are replaced by placeholders so that bold text can contain them
    codes = []

    def keep(m):
        codes.append("<code>" + html.escape(m.group(1).replace("\\|", "|")) + "</code>")
        return f"\0{len(codes) - 1}\0"

    text = re.sub(r"`([^`]*)`", keep, text)
    text = html.escape(text.replace("\\|", "|"), quote=False)
    text = re.sub(r"\*\*(.+?)\*\*", r"<strong>\1</strong>", text)
    text = re.sub(r"\[([^\]]+)\]\(([^)]+)\)", r'<a href="\2">\1</a>', text)
    return re.sub(r"\0(\d+)\0", lambda m: codes[int(m.group(1))], text)


def split_row(line):
    cells = re.split(r"(?<!\\)\|", line.strip())
    return [c.strip() for c in cells[1:-1]]


def convert(markdown):
    """Convert the Markdown subset used by the cheatsheet into HTML.

    Each "##" heading opens a <section> (a block that isn't split between
    columns), each "#" heading opens a <div class="part">.
    """
    lines = markdown.splitlines()
    out = []
    title = "LiteScript cheatsheet"
    lead_text = ""
    in_part = in_section = False
    i = 0

    def close_section():
        nonlocal in_section
        if in_section:
            out.append("</section>")
            in_section = False

    def close_part():
        nonlocal in_part
        close_section()
        if in_part:
            out.append("</div>")
            in_part = False

    while i < len(lines):
        line = lines[i]
        if not line.strip():
            i += 1
        elif line.startswith("```"):
            lang = line[3:].strip()
            i += 1
            code = []
            while i < len(lines) and not lines[i].startswith("```"):
                code.append(lines[i])
                i += 1
            i += 1
            out.append(f'<pre class="lang-{lang}"><code>{highlight(chr(10).join(code), lang)}</code></pre>')
        elif line.startswith("# "):
            close_part()
            text = line[2:].strip()
            if not out:
                title = re.sub(r"`", "", text)
                out.append(f'<header><h1>{inline(text)}</h1>')
                in_part = False
                # The lead paragraph belongs to the header
                i += 1
                lead = []
                while i < len(lines) and not lines[i].startswith("#"):
                    if lines[i].strip():
                        lead.append(lines[i].strip())
                    i += 1
                lead_text = " ".join(lead)
                out.append(f'<p class="lead">{inline(lead_text)}</p></header>')
                out.append('<div class="part">')
                in_part = True
                continue
            slug = re.sub(r"[^a-z0-9]+", "-", re.sub(r"`", "", text).lower()).strip("-")
            out.append(f'<div class="part part-{slug.split("-")[0]}"><h1 class="part-title" id="{slug}">{inline(text)}</h1>')
            in_part = True
            i += 1
        elif line.startswith("## "):
            close_section()
            text = line[3:].strip()
            slug = re.sub(r"[^a-z0-9]+", "-", text.lower()).strip("-")
            out.append(f'<section id="{slug}"><h2>{inline(text)}</h2>')
            in_section = True
            i += 1
        elif line.startswith("|"):
            header = split_row(line)
            aligns = split_row(lines[i + 1]) if i + 1 < len(lines) else []
            i += 2
            rows = []
            while i < len(lines) and lines[i].startswith("|"):
                rows.append(split_row(lines[i]))
                i += 1
            cls = []
            if all(not h for h in header):
                cls.append("noheader")
            if any(a.startswith(":") and a.endswith(":") for a in aligns[:1]):
                cls.append("center-first")
            t = [f'<table class="{" ".join(cls)}">']
            if "noheader" not in cls:
                t.append("<thead><tr>" + "".join(f"<th>{inline(h)}</th>" for h in header) + "</tr></thead>")
            t.append("<tbody>")
            for r in rows:
                t.append("<tr>" + "".join(f"<td>{inline(c)}</td>" for c in r) + "</tr>")
            t.append("</tbody></table>")
            out.append("".join(t))
        elif re.match(r"[-*] ", line):
            items = []
            while i < len(lines) and re.match(r"[-*] ", lines[i]):
                items.append(lines[i][2:].strip())
                i += 1
            out.append("<ul>" + "".join(f"<li>{inline(it)}</li>" for it in items) + "</ul>")
        else:
            para = []
            while i < len(lines) and lines[i].strip() and not re.match(r"(#|\||```|[-*] )", lines[i]):
                para.append(lines[i].strip())
                i += 1
            out.append(f"<p>{inline(' '.join(para))}</p>")
    close_part()
    return title, lead_text, "\n".join(out)


CSS = """
:root {
  --bg: #ffffff; --fg: #1d2330; --muted: #5d6677; --rule: #dde2ea;
  --code-bg: #f3f5f9; --accent: #2457c5; --accent-soft: #e8eefb;
  --pg: #b4570c; --pg-soft: #fbefe3;
  --k: #8a2bb9; --s: #187a3c; --n: #b4570c; --c: #8a93a3;
}
@media (prefers-color-scheme: dark) {
  :root:not([data-theme="light"]) {
    --bg: #15181e; --fg: #dfe4ec; --muted: #9aa3b2; --rule: #2c323d;
    --code-bg: #1d2129; --accent: #7aa2ff; --accent-soft: #1f2a44;
    --pg: #f0a35e; --pg-soft: #3a2a1b;
    --k: #d19af0; --s: #8fd19e; --n: #f0a35e; --c: #737c8c;
  }
}
* { box-sizing: border-box; }
html { -webkit-text-size-adjust: 100%; }
body {
  margin: 0; padding: 20px 16px 40px; background: var(--bg); color: var(--fg);
  font: 13px/1.4 "Segoe UI", system-ui, -apple-system, Roboto, "Helvetica Neue", Arial, sans-serif;
}
main { max-width: 1500px; margin: 0 auto; }
a { color: var(--accent); }
header { margin-bottom: 10px; border-bottom: 3px solid var(--accent); padding-bottom: 6px; }
header h1 { margin: 0; font-size: 22px; letter-spacing: -0.01em; }
.lead { margin: 2px 0 0; color: var(--muted); }
nav { margin: 6px 0 12px; font-size: 12px; }
nav a { text-decoration: none; margin-right: 10px; white-space: nowrap; }
.part {
  display: grid; grid-template-columns: repeat(auto-fill, minmax(min(400px, 100%), 1fr));
  gap: 14px 18px; align-items: start;
}
.part + .part { margin-top: 18px; }
.part-title {
  grid-column: 1 / -1; column-span: all; margin: 0 0 8px; padding: 4px 10px; font-size: 17px;
  color: var(--pg); background: var(--pg-soft); border-left: 5px solid var(--pg); border-radius: 3px;
}
section { margin: 0; min-width: 0; }
pre, ul, p, tr { break-inside: avoid; page-break-inside: avoid; }
thead { break-after: avoid; }
h2 {
  break-after: avoid; page-break-after: avoid;
  margin: 0 0 4px; padding: 2px 6px; font-size: 13.5px; color: var(--accent);
  background: var(--accent-soft); border-radius: 3px;
}
.part ~ .part h2 { color: var(--pg); background: var(--pg-soft); }
p { margin: 3px 0; }
ul { margin: 3px 0; padding-left: 16px; }
li { margin: 1px 0; }
code { font: 11.5px/1.35 "Cascadia Mono", Consolas, "SFMono-Regular", Menlo, monospace; }
:not(pre) > code { background: var(--code-bg); padding: 0 3px; border-radius: 3px; }
pre {
  margin: 4px 0; padding: 5px 7px; background: var(--code-bg); border-radius: 4px;
  overflow-x: auto; white-space: pre; border-left: 3px solid var(--rule);
}
pre code { font-size: 11px; }
pre.lang-asm code { counter-reset: line; }
pre.lang-asm .ln { counter-increment: line; }
pre.lang-asm .ln::before {
  content: counter(line); display: inline-block; width: 2em; margin-right: 0.7em; padding-right: 0.4em;
  text-align: right; color: var(--c); border-right: 1px solid var(--rule);
}
.note { user-select: none; -webkit-user-select: none; }
.k { color: var(--k); font-weight: 600; } .s { color: var(--s); } .n { color: var(--n); } .c { color: var(--c); font-style: italic; }
table { width: 100%; border-collapse: collapse; margin: 4px 0; font-size: 12px; }
th, td { text-align: left; vertical-align: top; padding: 2px 5px; border-bottom: 1px solid var(--rule); }
th { font-weight: 600; color: var(--muted); border-bottom: 1.5px solid var(--muted); }
td:first-child { white-space: normal; }
table.center-first td:first-child { text-align: center; font-weight: 600; color: var(--muted); width: 1%; white-space: nowrap; }

@page { size: A4 landscape; margin: 8mm; }
@media print {
  :root, :root:not([data-theme="light"]) {
    --bg: #fff; --fg: #111; --muted: #555; --rule: #d6d9df; --code-bg: #f2f4f7;
    --accent: #1f4fb5; --accent-soft: #e6ecf9; --pg: #a24c08; --pg-soft: #fbeee1;
    --k: #7a1fa8; --s: #146b33; --n: #a24c08; --c: #7d8594;
  }
  body { padding: 0; font-size: 7.6pt; line-height: 1.3; }
  main { max-width: none; }
  nav { display: none; }
  header { margin-bottom: 5px; padding-bottom: 3px; border-bottom-width: 2px; }
  header h1 { font-size: 13pt; }
  .lead { font-size: 7.6pt; }
  .part { display: block; column-count: 3; column-gap: 12px; column-rule: 1px solid var(--rule); }
  .part + .part { margin-top: 0; break-before: page; page-break-before: always; }
  .part-title { font-size: 11pt; padding: 2px 8px; margin-bottom: 5px; }
  section { margin-bottom: 6px; }
  h2 { font-size: 8.6pt; padding: 1px 5px; margin-bottom: 2px; }
  code, pre code { font-size: 7pt; }
  pre { padding: 3px 5px; margin: 2px 0; overflow: visible; white-space: pre-wrap; }
  pre code { font-size: 6.9pt; line-height: 1.28; }
  table { font-size: 7.3pt; margin: 2px 0; }
  th, td { padding: 1px 3px; }
  ul { padding-left: 12px; }
  a { color: inherit; text-decoration: none; }
}
"""


def plain(text):
    """Text of inline Markdown, for the metadata."""
    text = re.sub(r"\[([^\]]+)\]\([^)]+\)", r"\1", text)
    return html.escape(re.sub(r"[`*]", "", text))


def build_html(markdown):
    title, lead, body = convert(markdown)
    page_title = " ".join(w[:1].upper() + w[1:] for w in title.split())
    # The first sentence of the lead paragraph
    description = plain(lead.split(". ")[0].rstrip(".") + ".")
    sections = re.findall(r'<(?:section|h1 class="part-title") id="([^"]+)">(?:<h2>)?(.*?)</(?:h2|h1)>', body)
    nav = " ".join(f'<a href="#{sid}">{re.sub("<[^>]+>", "", label)}</a>' for sid, label in sections)
    # The navigation goes after the header
    body = body.replace("</header>", f"</header>\n<nav>{nav}</nav>", 1)
    return f"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>{page_title}</title>
<meta name="description" content="{description}">
<style>{CSS}</style>
</head>
<body>
<main>
{body}
</main>
</body>
</html>
"""


def find_browser():
    candidates = [
        shutil.which("msedge"), shutil.which("chrome"), shutil.which("google-chrome"),
        shutil.which("chromium"), shutil.which("chromium-browser"),
        r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe",
        r"C:\Program Files\Microsoft\Edge\Application\msedge.exe",
        r"C:\Program Files\Google\Chrome\Application\chrome.exe",
        "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome",
        "/Applications/Microsoft Edge.app/Contents/MacOS/Microsoft Edge",
    ]
    for c in candidates:
        if c and os.path.isfile(c):
            return c
    return None


def build_pdf(html_path, pdf_path, browser):
    if pdf_path.exists():
        pdf_path.unlink()
    profile = tempfile.mkdtemp(prefix="cheatsheet-")
    try:
        subprocess.run([
            browser, "--headless", "--disable-gpu", "--no-first-run",
            f"--user-data-dir={profile}", "--no-pdf-header-footer",
            f"--print-to-pdf={pdf_path}", html_path.as_uri(),
        ], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=120)
        # On Windows the launcher can return before the PDF is written:
        # wait until the file exists and its size is stable
        size, deadline = -1, time.monotonic() + 60
        while time.monotonic() < deadline:
            if pdf_path.exists() and pdf_path.stat().st_size == size and size > 0:
                return True
            size = pdf_path.stat().st_size if pdf_path.exists() else -1
            time.sleep(0.5)
        return False
    finally:
        shutil.rmtree(profile, ignore_errors=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("names", nargs="*", help="cheatsheets to generate (all by default)")
    parser.add_argument("--no-pdf", action="store_true", help="only generate the HTML pages")
    parser.add_argument("--browser", help="path of Chrome, Chromium or Edge")
    args = parser.parse_args()

    names = [Path(n).stem for n in args.names] or sorted(p.stem for p in FOLDER.glob("*.md"))
    browser = None
    if not args.no_pdf:
        browser = args.browser or find_browser()
        if not browser:
            print("No Chromium based browser found, use --browser or --no-pdf", file=sys.stderr)
            return 1

    for name in names:
        md_path = FOLDER / f"{name}.md"
        html_path = FOLDER / f"{name}.html"
        pdf_path = FOLDER / f"{name}.pdf"
        if not md_path.is_file():
            print(f"{md_path.relative_to(ROOT)} doesn't exist", file=sys.stderr)
            return 1

        with open(html_path, "w", encoding="utf-8", newline="\n") as f:
            f.write(build_html(md_path.read_text(encoding="utf-8")))
        print(f"Written {html_path.relative_to(ROOT)}")

        if browser:
            if not build_pdf(html_path, pdf_path, browser):
                print("The browser didn't produce the PDF", file=sys.stderr)
                return 1
            print(f"Written {pdf_path.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
