<p align="center"><img src="src/gui/resources/images/readme-icon.png" alt="" /></p>

<h1 align="center">Grabber <sup><sub>(Fable fork)</sub></sup></h1>

<p align="center">
  <a href="https://github.com/ShugokiFable/imgbrd-grabber/releases/latest"><img src="https://img.shields.io/github/v/release/ShugokiFable/imgbrd-grabber?label=latest%20release" alt="Latest release" /></a>
  <a href="https://github.com/ShugokiFable/imgbrd-grabber/releases"><img src="https://img.shields.io/github/downloads/ShugokiFable/imgbrd-grabber/total" alt="Downloads" /></a>
  <a href="LICENSE"><img src="https://img.shields.io/github/license/ShugokiFable/imgbrd-grabber" alt="License" /></a>
  <a href="https://github.com/Bionus/imgbrd-grabber"><img src="https://img.shields.io/badge/upstream-Bionus%2Fimgbrd--grabber-blue" alt="Upstream" /></a>
</p>

**This is a maintained fork of [Bionus/imgbrd-grabber](https://github.com/Bionus/imgbrd-grabber)** — the imageboard/booru downloader.

It tracks upstream `develop`, then layers Fable fixes for reliability, broken sources, packaging, and UI themes. Use **this** repo’s releases if you want those changes; use [upstream](https://github.com/Bionus/imgbrd-grabber) for the official project.

---

## What this fork changes

Compared to stock Grabber, this fork focuses on things that break real use: hung downloads, dead APIs, silent packaging bugs, and missing dark UI options.

### Reliability

| Change | Why it matters |
|--------|----------------|
| **Download queue hang fix** | Aborted network replies no longer leak concurrency slots. Before this, canceling or failing requests could freeze *all* further downloads until you restarted the app. Also avoids a related shutdown crash. |
| **Gelbooru 0.2 / Rule34 XML errors** | Sites that return HTTP 200 with an XML *error* body (e.g. unauthenticated Rule34) no longer crash the JS parser (`Cannot read property 'post' of undefined`). The engine reports the error and can fall back cleanly. |

### Sources

| Change | Why it matters |
|--------|----------------|
| **Kemono → kemono.cr + v1 API** | Kemono left `kemono.su`; the old `/api/recent` path is dead. This fork uses `kemono.cr` and `/api/v1/posts` (tag search, attachments, required headers, HTTPS defaults). |
| **Order / sort + free-text hygiene** | Real `order:` / `sort:` mapping across Gelbooru-family, Danbooru, E621, Moebooru, etc., plus freestyle aliases (`newest`, `popular`, …). Multi-site free-text search is cleaned so meta tags don’t poison APIs that don’t understand them. Also improves DeviantArt / Reddit / Twitter sort handling. |
| **R34.xxx default source order** | Restores a sensible default source order for Rule34.xxx. |

### Packaging & UI

| Change | Why it matters |
|--------|----------------|
| **Translations actually ship in zips** | Release packaging created `languages/` *after* copying `.qm` files, so every archive silently shipped `languages.ini` with **zero** compiled translations. The package script now creates the directory first (19 `.qm` files in current Windows builds). |
| **8 modern dark themes** | Bundled alongside Default: **Catppuccin Mocha**, **Dracula**, **Fluent Dark**, **Gruvbox Dark**, **Nord Dark**, **One Dark**, **Solarized Dark**, **Tokyo Night** (plus existing QDarkStyleSheet). |

### Upstream kept in sync

`develop` is periodically merged from [Bionus/imgbrd-grabber](https://github.com/Bionus/imgbrd-grabber) so you still get official fixes (referers, date parsing, Danbooru user-agent, blacklist improvements, CI bumps, and so on) **on top of** the Fable changes above.

---

## Download (this fork)

**Latest Windows build:** [Releases](https://github.com/ShugokiFable/imgbrd-grabber/releases/latest)

| Tag | Notes |
|-----|--------|
| [`v7.13.0-fable.2`](https://github.com/ShugokiFable/imgbrd-grabber/releases/tag/v7.13.0-fable.2) | Current. Full rebuild after upstream merge + all Fable fixes. Portable x64 zip (`Grabber.exe`). |
| [`v7.13.0-fable.1`](https://github.com/ShugokiFable/imgbrd-grabber/releases/tag/v7.13.0-fable.1) | First Fable package with translation packaging fix. |

Unpack the zip and run `Grabber.exe`. No installer required.

> Official upstream installers/nightlies remain at [Bionus releases](https://github.com/Bionus/imgbrd-grabber/releases) and do **not** include the Fable-only changes listed above.

Upstream docs still apply for install/usage details:

* [Windows](https://www.bionus.org/imgbrd-grabber/docs/install/windows.html) · [Linux](https://www.bionus.org/imgbrd-grabber/docs/install/linux.html) · [macOS](https://www.bionus.org/imgbrd-grabber/docs/install/macos.html) · [Android](https://www.bionus.org/imgbrd-grabber/docs/install/android.html)

---

## What Grabber is (upstream feature set)

Imageboard/booru downloader for bulk search and download across many sites, with powerful filename tokens, blacklists, multi-source tabs, themes, auth, CLI, and more.

**Highlights:** multi-tab / multi-site search · tag autocomplete · blacklist & post-filter · bulk download · custom `%token%` or JS filenames · favorites / “view later” · CSS themes · optional local-booru / DB hooks.

**Default sources include:** Danbooru, Gelbooru, E-Hentai, Pixiv, yande.re, Shimmie, e621, Konachan, rule34, safebooru, Anime-Pictures, Zerochan, Twitter, Kemono, and others you can add yourself.

Full feature write-up and screenshots: [upstream README / site](https://github.com/Bionus/imgbrd-grabber).

---

## Branches

| Branch | Role |
|--------|------|
| `develop` | **Default.** Fable fixes + synced upstream develop. Tagged Fable releases are cut from here. |
| `feature/fable-develop` | Same tip as `develop` (integration / WIP line). |
| `master` | Older merge line; prefer `develop` + Fable tags for builds. |

---

## Building from source

Same stack as upstream (Qt 6, CMake/Ninja, OpenSSL). See [Compilation](https://bionus.github.io/imgbrd-grabber/docs/compilation.html).

```bash
# Linux
./build.sh

# macOS
./build-mac.sh
```

Windows: configure with CMake against Qt 6 + MSVC, build `Release`, then package with `scripts/package-windows.bat` (needs Git Bash, 7-Zip, `windeployqt`).

Fable Windows release builds use Qt **6.9.x** / MSVC and stamp portable zips as `Grabber_7.13.0-fable.N_x64.zip`.

---

## Relationship to upstream

- **Upstream project:** [Bionus/imgbrd-grabber](https://github.com/Bionus/imgbrd-grabber) by [Jack Vasti (Bionus)](https://github.com/Bionus) and [contributors](https://github.com/Bionus/imgbrd-grabber#contributors).
- **This fork:** [ShugokiFable/imgbrd-grabber](https://github.com/ShugokiFable/imgbrd-grabber).
- License remains the upstream project license (see [`LICENSE`](LICENSE)).
- Bug reports for **Fable-specific** behavior: open issues **here**. Core Grabber questions and PRs that belong upstream should go to Bionus when possible.

Support the original author if you use Grabber:

* [PayPal](https://www.paypal.me/jvasti) · [Patreon](https://www.patreon.com/bionus) · [Discord](https://discord.gg/pWnY5eW3rz)

---

## Changelog (Fable tags)

### v7.13.0-fable.2
- Rebuild on latest upstream `develop` merge (referer defaults, ISO 8601 dates, blacklist `QStringList`, Danbooru UA, removable site headers, …).
- Retains all fable.1 fixes below.

### v7.13.0-fable.1
- Fix release packaging so compiled translations (`.qm`) are included.
- Fix network abort path that wedged the download queue / could crash on exit.
- Gelbooru 0.2 XML error-document handling.
- Kemono `kemono.cr` + v1 API.
- Order/sort + free-text hygiene across major sources.
- Eight bundled modern dark themes.
