# Compás TEKNO — test guide (post-conversion)

Everything new since the all-tekno conversion, in one checklist. Drums are
known-good; this guide is biased toward the **synth voices** and the new
behaviors. You can run every test on the **web emulator** (same controls as
hardware) and repeat on the FM-1.

> Controls are hardware-real: 14 buttons, 27 pads, 7 knobs, MASTER pot.
> No emulator-only features exist.

---

## 0. Baseline (before anything)

| Check | Expected |
|---|---|
| Boot | TRIBAL TEKNO, **155 BPM**, stopped |
| Press PLAY | audible groove, four-on-floor kick, offbeat hats |
| Audio level | no harsh clipping at MASTER 100 |
| Screen | wakor palette (black/green/yellow), artwork renders |

If drums sound right, proceed.

---

## 1. The 33 grooves & BPM range

**Test:** RACK → SONG knob, walk the whole list.

| Expectation |
|---|
| 33 entries: 30 tekno grooves + 3 SANTO (EVOLVED / HYPNOTIC / SPIRAL SUNRISE) |
| BPM reads **155 or 165** throughout (the free-party pocket) |
| Every groove: four-on-floor kick (quarters always present) |
| Hardtek entries add **double kicks** (KIK FLUO, POTOSÍ WEPA, DERECHO PRIMERA/SEGUNDA) |
| Hats/percs change per style (16th vs offbeat vs shuffle vs sparse) |

**Pass =** every stop has a steady kick pulse at the tempo shown.

---

## 2. Tempo lock (new)

**Test:**
1. While playing, turn HOME/SET **KNOB 1** to 170.
2. Change grooves (RACK → SONG knob).
3. Change again.

**Expect:** tempo stays **170** through every groove change. It only changes
when *you* turn KNOB 1 again. (Set 162 → it sticks to 162.)

---

## 3. Synth voices (the main event)

Bass = role 5. Get there:
- BASS page, solo bass: HOME → **MIX** → pad 12 (solo role 5).
- Or simply listen in the mix; for clean A/B, solo.

On the BASS page, **KNOB 4** cycles the 4 voices live:

| Style | Knob | What to listen for | Character |
|---|---|---|---|
| 0 **FLUTE** | 0 | soft, pure, breathy; a gentle *vibrato* on held notes | sine/triangle (low sparkle) |
| 1 **ACID** | 1 | nasal, resonant, TB-303-ish; squelchy with bite up | saw + resonant filter |
| 2 **MINIMOOG** | 2 (default) | fat low-end, rounded but *present*; slight growl | 3 saws + ladder filter |
| 3 **TRANCE LEAD** | 3 | bright, band-pass; a *pumping* filter sweep in time | saws + quarter-note LFO |

**A/B protocol:**
1. Solo the bass (MIX pad 12).
2. Sweep KNOB 4 slowly, one pass per bar.
3. Confirm each voice changes character (not just volume).
4. Raise KNOB 2 (bite) on acid/Moog — resonance should scream/ring.
5. Drop KNOB 1 (tone) — low-pass should dull each voice.

**Pass =** four clearly different timbres; flute obviously softer/purest,
trance-lead obviously brightest.

> Reference measurements (harmonic richness, higher = brighter):
> FLUTE 0.017 · MOOG 0.071 · ACID 0.075 · TRANCE 1.4.

---

## 4. Leads (new — via the SONG knob)

**Test:**
1. Stop playback.
2. RACK → SONG knob past groove 32 → LEAD entries:
   - `LEAD FLUTE` (id 33)
   - `LEAD HYPNOTIC` (34)
   - `LEAD DESERT` (35)
   - `LEAD XLEAD` (36)
3. Press PLAY.

**Expect:**
| Lead | Sounds like |
|---|---|
| FLUTE | melodic soft flute line over the groove |
| HYPNOTIC | trance ostinato, static, hypnotic |
| DESERT | long arching exotic phrases |
| XLEAD | fast runs + screaming high leaps in the refrain |

- While a lead is playing, the groove backing keeps looping underneath.
- To return: stop, turn the SONG knob to a groove, PLAY.

---

## 5. Live drum craft (drop & dynamics)

| Action | Control | Expect |
|---|---|---|
| Cut the kick (drop) | MIX → pad 1 | kick disappears instantly; hats/percs remain |
| Bring it back | pad 1 again | drop lands on the next beat |
| Solo one part | MIX pads 7–12 | only that part |
| Fill | FILL | roll/fill; ends back on the groove |
| Breakdown | mute kick+clap (pads 1 & 3) | airy percussion bed |
| Build | add parts one by one, FILL at the top | rising energy |
| Energy | MODE knob | screen + arrangement shift BASE→LIFT→BREAK |

---

## 6. Infinite loop & progressive change

- Let any groove run 5+ minutes → **never auto-stops**, loops forever.
- While playing, change groove with RACK SONG knob → lands **at the next
  bar**, playback never drops.

---

## 7. Quick regression (30 seconds)

- Press all 14 buttons → no freeze, screen updates.
- Play pads 0–26 → no stuck notes.
- Palette: black/green/yellow everywhere; text readable.
- After a lead, return to a groove and PLAY → normal drum groove returns.

---

## 8. Hardware flashing (safety)

- `firmware/compas-tekno.fwsc`, identity **FM-1_710**.
- Use the same USB installer as Amaru OS; don't unplug mid-write.
- If coming from another custom firmware: **return to official V15 first**
  (the page shows this question before flashing).
- First boot after flash: run section 0; then the rest of this guide.

---

*Passing everything above = the unit is functionally equivalent to the
emulator and ready for a set.*
