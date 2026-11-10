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

```sh
python3 -m http.server -d web/emu 8000
# open http://localhost:8000/
```

The emulator decodes the same `727.pack` byte stream the firmware uses, with the
device's delta/RLE player, choke groups and per-role velocity.

A rendered preview of the TRIBAL TEKNO preset (16 bars) is in `demo/tekno_preview.wav`.

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
