# Compás TEKNO 🎛️ — a light fork of Compás

> **Same firmware, new soul.** Compás TEKNO is a *light fork* of
> [Compás](https://github.com/TexasRADIO/compas-firmware) — the rhythm-section
> firmware for the M-VAVE FM-1. We kept the original engine, UI and workflow;
> we swapped the **sample bank**, retuned the **music theory**, and gave the
> whole thing a **tekno/free-party** makeover.

**Upstream:** Compás (TexasRADIO) · built on Felucca v1.6 (Leo Kuroshita,
Hügelton Instruments) · GPL-3.0.

---

## 🪩 The tekno take

- **UFO sample bank** — the factory TR-727-derived sounds are replaced with a
  French/tribe free-party kit extracted from **UFO hardtekno / tribecore /
  tribe190 Electribe (E2S) banks**: Guigoo kicks, Kawai clap, Nouvoo snare,
  closed/open hats, tribal percs, toms, hoover stabs, break FX and a spare hard
  kick — packed losslessly into the firmware's native bank format.
- **30 grooves, all kitted** — every one of the 30 presets now plays the tekno
  bank (Kick · Hat · Clap · Perc · OpenHat sampled; bass via the engine's
  acid/hoover voice). Each groove keeps its own patterns, parts and BPM — from
  `TEKNO DURO` and `FRENCH KICK` through `TRIBAL 155`, `HARD TRIBE`, to the
  flagship **TRIBAL TEKNO** (155 BPM, four-on-floor, offbeat hats, clap on
  2 & 4).
- **Wakor visual identity** — the whole UI (device + web) is rethemed to a
  neon black/green/yellow palette (#000000 · #89ff00 · #fdff00 · #075800 ·
  #c8d48a).
- **New splash** — the boot portrait is a freshly packed 16-color 108×108
  artwork (regenerate from any PNG with `tools/portrait-pack.py`).

No engine hack, no protocol change, no stolen workflow — it's the firmware you
know, re-voiced and re-skinned. **A light fork, end to end.**

## 🎛️ What you can do

- **Live performance** — play/stop, fills, accent, sections (BASE/LIFT/BREAK),
  part keys (INTRO/VERSE/CHORUS/BREAK/END), **mute/solo per instrument on the
  MIX page**, overlay layers, groove browser (RACK + SONG knob), 16 song slots
  with save/recall.
- **Browser emulator** — the full firmware (board+panel+stage+engine) compiled
  to WASM runs in your browser, same code path as the device.
- **Tekno scores** — three SANTO CENTER-theory lead pieces (phrygian flute,
  dorian-b2 hypnotic ostinato, egyptian spiral) selectable from the emulator's
  SCORES menu: 16-bar compact tables, ~no flash cost.

## ▶️ Try it in the browser

[![Try the emulator](https://img.shields.io/badge/Try%20it%20in%20the%20browser-89ff00.svg)](https://marcocasanova00.github.io/compas-tekno/)

```sh
python3 -m http.server -d web/emu 8000
# open http://localhost:8000/
```

- `web/emu/emu.wasm` — the full firmware in WASM (`web/emu-fw/emu_full.c`).
- `web/emu/worklet.js` — AudioWorklet: render + screen publish.
- `web/emu/index.html` — device screen, panel buttons, keys, MASTER, guides.

A rendered preview of TRIBAL TEKNO (16 bars @ 155 BPM) is in
`demo/tekno_preview.wav`.

## 🔧 Build

See `BUILDING.md` (pinned JieLi PI32v2 toolchain + Felucca v1.6 BSP).

```sh
export COMP_JIELI=$HOME/.jieli/toolchain
export COMP_BSP=$PWD/release-inputs/felucca
sh tools/board-target.sh
```

Bank sources: `board/727-sources.json` + `board/727-levels.json` (the original
TR-727 metadata stays archived in `board/727-original/`).

## 📦 Install on hardware

`firmware/compas-tekno.fwsc` is the flashable package for the M-VAVE FM-1
(identity `FM-1_710`, JieLi chip key `0x980F`), built against the exact
Felucca v1.6 BSP upstream pins (`tools/build_device.py` → `app.bin`, Felucca
update loader + SDK SPL via `tools/fm1pkg_make.py`).

Flash it the same way you flash Felucca/Amaru OS on the FM-1 (USB installer).

## ⚖️ License & samples

- Code: GPL-3.0 (see `LICENSE`, `LICENSING.md`).
- The tekno WAVs in the bank come from commercial UFO sample packs (owned /
  selected by the author). The packed bank ships so the emulator and firmware
  are reproducible; **redistributing the audio may violate the packs'
  licenses** — if you publish or sell a build, replace the bank with samples
  you have rights to distribute (e.g. CC0 one-shots), keeping the same slot
  layout.
- M-VAVE and FM-1 are trademarks of their respective owners; this fork is not
  affiliated with them.
