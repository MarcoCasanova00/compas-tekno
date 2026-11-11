# Tekno bank verification (UFO / HardTekno / TribeCore imports)

The 15-voice sample bank (`build/board/727.pack` ← `web/emu-fw/727_packed.h`)
is packed losslessly from one-shot WAVs (HardTekno / TribeCore Narko / Tribe190
packs) with every voice normalized to 0.25 FS, matching `board/727-levels.json`
(`peak24 = 2097152 = 0.25 × 2^23`).

## Checks (rerun: `python3 tools/bank-from-header.py` then the Python below)

| # | Sound | Frames | Peak | ZCR | Verdict |
|---|-------|-------:|-----:|----:|---------|
| 0 | Closed hat | 4636 | 8192 | 0.295 | OK — hissy hat |
| 1 | Open hat | 10656 | 8192 | 0.410 | OK — airy open hat |
| 2 | Kick Guigoo Trib | 14700 | 8192 | 0.003 | OK — sub-heavy kick |
| 3 | Kick Guigoo Bass | 13923 | 8192 | 0.007 | OK — bass kick |
| 4 | Clap Kawai | 9562 | 8192 | 0.303 | OK — noisy clap |
| 5 | Snare Nouvoo | 12706 | 8192 | 0.049 | OK — tonal snare |
| 6 | Drum Snar | 6875 | 8192 | 0.270 | OK |
| 7 | Tribal Perc 1 | 4672 | 8192 | 0.055 | OK |
| 8 | Tribal Perc 2 | 3411 | 8192 | 0.013 | OK |
| 9 | Tribal Tom | 7808 | 8192 | 0.151 | OK |
| 10 | Tribal Conga | 10931 | 8192 | 0.042 | OK |
| 11 | Hoover Stab | 18940 | 8192 | 0.087 | OK — chord stab |
| 12 | Pad Stab | 70854 | 8192 | 0.010 | OK — sustained pad |
| 13 | Break FX | 55695 | 8192 | 0.140 | OK — continuous break loop |
| 14 | Hard Kick Big | 19915 | 8192 | 0.022 | OK — sustained hard kick |

- Every clip decodes to **exactly** its metadata frame count (pack is self-delimiting;
  stream ends at the expected byte, no trailing data).
- Every clip peaks at exactly **8192** (= 0.25 × 32768): the normalization spec is hit
  bit-for-bit.
- Zero decode glitches: no adjacent-sample discontinuities > 0.25 FS (except the
  Hard Kick's own attack transient, 5/19 915 samples = 0.03%).
- Spectral character per ZCR matches each sound's name (hats 0.3–0.4, kicks
  ≈0.005, clap ≈0.3, tom ≈0.15, pad/stab ≈0.01–0.09).
- Boundaries are clean (clips start near silence and end near silence, except
  Break FX which is an intentional loop).

## Kits — all 33 grooves now use the bank (1-based clip indices, `score.json` `board.kit`)

Every groove's kit is `[3, 1, 5, 8, 2, 0]` (same as study 29's): the patterns/theory per groove stay as authored, the voices are the bank's. Bass stays synthesized.

`[3, 1, 5, 8, 2, 0]` → Kick = Kick Guigoo Trib, Hat = Closed hat,
Clap = Clap Kawai, Perc = Tribal Perc 1, OpenHat = Open hat, Bass = none
(synthesized — the engine's fallback synthesis). `board_note727()` tries the
sample first; `0` in the kit means "synthesize".

## Web vs device banks

- Web/wasm bank: full 407 790 B pack, 265 284 frames (matches `727-levels.json`).
- Device bank: flash-trimmed 209 286 B pack, 132 873 frames, extracted from the
  built image by `tools/device-bank-from-app.py` into `build/device/board/`
  (kept separate so the web bank artifacts in `build/board/` are never clobbered).
