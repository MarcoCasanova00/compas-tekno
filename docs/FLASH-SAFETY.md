# Flashing safety — Compás TEKNO (FM-1_710)

This fork keeps the whole Felucca/Compás safety net that Amaru OS / FM-23
depends on. Nothing in the app weakens it; the update path is exactly the one
the device already uses.

## Platform safety net (kept, never weakened)

| Guard | Status |
|---|---|
| Boot guard (two failed boots → chip boot mode / UBOOT, USB up) | kept — SDK/SPL path untouched |
| Recovery gesture (OCT− + OCT+ held 5 s → boot mode) | kept at the boot/BOOT level; the Compás app does **not** implement or reuse it (verified: no long-press handling in `panel.c`) |
| Update entry on USB-MIDI (the app always accepts a package) | kept — Felucca loader `firmware/compas-tekno-ota.bin` |
| Watchdog (~8 s) | kept — `fm1_wdt_feed()` in the main loop |
| RecoveryGuard + supervisor watermark | kept (`board/fm1.c`, `.noinit`) |
| Identity | package identity decodes to exactly **FM-1_710**; USB product/VID untouched → your installer finds it and refuses foreign packages |
| Loader write-guard | only `[0x4000, 0x97000)` ∪ `[0xE0000, 0xFC000)`; never the head, never user data |

## Package facts (verified on the shipped `firmware/compas-tekno.fwsc`)

- Container: 2 entries (flash.bin + ota.bin), both CRC16-valid; chip marker `AC791N`
- App slot `0x8DFBC` (581,564 B), app CRC16 valid; stripped app == `app.bin`
- App size 400,484 B → **~181 KB headroom**
- Deterministic rebuild: the package is reproduced byte-identical from committed sources
- Preserve area `0x90000..0x93000` is never written by the update path

## Your checklist before flashing

1. **Back up the stock flash first** — keep `firmware/FM-1_015.fwsc` (from the
   official installer site) or a transporter dump.
2. **If coming from another custom firmware** (Felucca, Baud Girl, Carnac,
   Amaru, …): **return to official V15 first**, then install this. The go/
   no-go question before flashing is: **"Did you go back to V15 before?"**
   If the answer is no, stop and restore stock V15 first. Writing over a
   foreign custom firmware is the #1 brick cause.
3. Use the **same USB installer** you used for Amaru OS; use a data USB cable.
4. Never unplug during a write. If interrupted, replug and install again — the
   update resumes from the start.
5. After the first boot of the new image, verify it responds (PLAY/keys/audio)
   before relying on it.

## Bug sweep status (last run)

- Emulator full harness: BUSY-free transport, all 14 buttons, 27 keys, palette
  12/12 exact, audio clean (peak 0.84, 0 clipped)
- Grooves 0–32 browse at correct BPM; scores 29–32 select/play; mute/solo
  verified (kick mute RMS 0.248→0.073, ZCR→0.19)
- Device image rebuild + package validation clean as above
