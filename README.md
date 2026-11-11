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
- **33 grooves, full tekno** — every preset is a real tekno machine at
  **155 or 165 BPM** (the free-party pocket): four-on-floor with double-kick
  hardtek variants, SANTO CENTER-derived hats (offbeat / 16th / tribe-shuffle /
  mental-sparse), interlocking tribal percussion, and per-style bass motifs
  (octave-jump tribe, rolling 16th gallop, hypnotic mutations). All on the
  UFO bank; bass via the synth voices.
- **Wakor visual identity** — the whole UI (device + web) is rethemed to a
  neon black/green/yellow palette (#000000 · #89ff00 · #fdff00 · #075800 ·
  #c8d48a).
- **Four voices via the BASS page style knob** — 0 = shamanic breathy
  flute (slow vibrato + breath noise), 1 = acid (TD-3-ish, resonant),
  2 = **Minimoog** (own 3-osc ladder emulation: detuned oscillators,
  saturating 24 dB/oct filter, envelope cutoff, resonance), 3 =
  **Trance lead** (Da Hool “Love Parade” style: dual detuned saws →
  band-pass, quarter-note triangle LFO on cutoff, ~21% drive, near-organ
  sustain). All are original implementations modeled on classic recipes —
  no third-party code.
- **New splash** — the boot portrait is a freshly packed 16-color 108×108
  artwork (regenerate from any PNG with `tools/portrait-pack.py`).

No engine hack, no protocol change, no stolen workflow — it's the firmware you
know, re-voiced and re-skinned. **A light fork, end to end.**

## 🎛️ What you can do

- **Live performance** — play/stop, fills, accent, sections (BASE/LIFT/BREAK),
  part keys (INTRO/VERSE/CHORUS/BREAK/END), **mute/solo per instrument on the
  MIX page**, overlay layers, groove browser (RACK + SONG knob), 16 song slots
  with save/recall.
- **The drop** — mute the kick during the breakdown, unmute for the drop:
  **MIX** page + pad 1 (toggles kick mute).
- **Infinite loop** — songs loop indefinitely; nothing auto-stops. Grooves
  change live at the bar boundary (RACK + SONG knob) for progressive sets.
- **Leads on the hardware SONG knob** — after groove 32 the rack list extends
  with four LEAD entries (flute / hypnotic / desert / XLEAD) that route to the
  score engine: stop, turn the SONG knob past 32, PLAY.
- **Live guide** — `docs/TRIBE-MENTAL-LIVE.md`: drum add/subtract, kicks/breaks,
  synth leads & bass, free-party set flow.
- **Synth demos** — `demo/synth_flute.acid/moog/trance.wav` (steady tones) to
  audition the four bass voices directly.
- **Live-output demos** — `demo/live_moog_bass.wav`, `demo/live_flute_bass.wav`
  (recorded from the real output path, bass soloed).
- **Test guide** — `docs/TESTING-GUIDE.md`: verify grooves/BPM, tempo lock,
  the 4 synth voices, leads, drop, loops — on web or hardware.
- **User guide** — `docs/USER-GUIDE.md`: full real-hardware workflow (pages,
  knobs, live mechanics, free-party set recipe).
- **Browser emulator** — the full firmware (board+panel+stage+engine) compiled
  to WASM runs in your browser, same code path as the device.
- **Tekno scores** — four lead pieces selectable from the emulator's SCORES
  menu: three SANTO CENTER-theory pieces (phrygian flute, dorian-b2 hypnotic
  ostinato, egyptian spiral) + an **EXTREME LEAD · XLEAD arp** score inspired
  by the XLEAD hardtek sysex bank (E-rooted, octave+ screams). All are
  16-bar compact tables, ~no flash cost.

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

> ### ⚠️ Before you flash — did you go back to V15 first?
>
> **"Did you go back to V15 before?"**
>
> If the FM-1 is running **any custom firmware** (Felucca, Baud Girl, Carnac,
> Amaru OS, or anything else), **return to official V15 first**, then install
> this package. Writing straight over another custom firmware is the #1 cause
> of an FM-1 that stops answering. Coming from stock V15? You're clear.

`firmware/compas-tekno.fwsc` is the flashable package for the M-VAVE FM-1
(identity `FM-1_710`, JieLi chip key `0x980F`), built against the exact
Felucca v1.6 BSP upstream pins (`tools/build_device.py` → `app.bin`, Felucca
update loader + SDK SPL via `tools/fm1pkg_make.py`).

Flash it the same way you flash Felucca/Amaru OS on the FM-1 (USB installer).
Never unplug mid-write; if interrupted, replug and install again (the update
resumes from the start). Keep your stock V15 image backed up.

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
