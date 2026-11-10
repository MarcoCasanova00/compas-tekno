# Compás TEKNO

A tekno conversion of **Compás** — the rhythm-section firmware for the M-VAVE FM-1
(original project: [TexasRADIO/compas-firmware](https://github.com/TexasRADIO/compas-firmware), GPL-3.0).

**What changed vs. upstream:**

- **Sample bank replaced** with a French/tribe tekno kit extracted from UFO
  hardtekno / tribecore / tribe190 Electribe banks: Guigoo kick, Kawai clap,
  Nouvoo snare, closed/open hats, tribal percs and toms, hoover stabs, spare
  hard kick. Packed with the repo's lossless encoder (`tools/board-bank.py`)
  into the same format the device plays.
- **New "Tribal Tekno" study** at **155 BPM**: four-on-floor kick, offbeat
  closed hats, clap on 2 & 4, rolling tribal percussion, synthesized acid/hoover
  bass (the engine's bass voice).
- **Presets renamed** to tekno-flavored names (TEKNO DURO, CORE TEKNO, MENTAL
  TEKNO, FRENCH KICK, TRIBAL 155, HARD TRIBE, etc.).
- **Browser emulator** added (`web/emu/`) that plays the actual packed bank in
  the browser.

## Try it in the browser

[![Try the emulator](https://img.shields.io/badge/Try%20it%20in%20the%20browser-ff7a00.svg)](https://marcocasanova00.github.io/compas-tekno/)

The emulator runs the **full firmware build** — `board.c` + `panel.c` +
`stage.c` + `engine.c` (the real board UI and audio engine) compiled to
WebAssembly with a simulated FM-1 (framebuffer LCD, keys, encoders). The LCD
you see is the firmware's own frame buffer; the audio is rendered by the same
code path as the device.

```sh
python3 -m http.server -d web/emu 8000
# open http://localhost:8000/
```

- `web/emu/emu.wasm` — the full firmware in WASM (sources: `web/emu-fw/emu_full.c`).
- `web/emu/worklet.js` — AudioWorklet running the firmware (render + screen publish).
- `web/emu/index.html` — device screen + control buttons (labels 0–13, encoder 0).
- `web/emu-fw/emu_full.c` — host platform: framebuffer, input, clock, audio glue.

A render of the TRIBAL TEKNO preset from the real engine (16 bars @ 155 BPM)
is in `demo/tekno_preview.wav`.

## Build the firmware

See `BUILDING.md` for the pinned toolchain (JieLi PI32v2 + Felucca BSP).
Example:

```sh
export COMP_JIELI=$HOME/.jieli/toolchain      # or your SDK root
export COMP_BSP=/path/to/felucca
export COMP_STUDIO=$PWD/release-inputs/studio # original studio sources
sh tools/board-target.sh
```

The modified bank sources live in `board/727-sources.json` + `board/727-levels.json`
(originals archived in `board/727-original/`).

## License & samples

- Code: GPL-3.0 (see `LICENSE`, `LICENSING.md`).
- The tekno WAVs embedded in the bank come from commercial UFO sample packs
  (owned/selected by the author). The packed bank in this repo is included so
  the emulator and firmware are reproducible; **redistributing the audio
  samples may violate the packs' licenses** — if you publish or sell a build,
  replace `web/emu/bank.json` / the bank sources with samples you have rights to
  distribute (e.g., CC0 one-shots), keeping the same slot layout.

## Install on the hardware

`firmware/compas-tekno.fwsc` is the flashable package for the M-VAVE FM-1
(identity `FM-1_710`, JieLi chip key `0x980F`), built with the JieLi PI32v2
toolchain against the exact Felucca v1.6 BSP that upstream Compás pins
(`tools/build_device.py` → `app.bin`, plus the Felucca update loader and SDK
SPL, `tools/fm1pkg_make.py`).

- `firmware/compas-tekno.fwsc` — the installable package (USB update).
- `firmware/compas-tekno-app.bin` — the raw app image (for tools that take a binary).
- `firmware/compas-tekno-ota.bin` — the update loader.
- `firmware/compas-tekno-app.dis` — disassembly for verification.

Flash it the same way you flash Felucca/Amaru OS on the FM-1 (USB
install/transporter tooling).
