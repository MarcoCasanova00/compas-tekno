# Compás TEKNO — User Guide (real FM-1 controls)

Everything below maps to the physical M-VAVE FM-1: **27 keys**, **4+
knobs** (shown here by the function printed on screen), and the real buttons
`OCT− · OCT+ · FX · SCL · ENV · LFO · EDIT · GLO · HOME · SAVE · ARP · SEQ ·
PLAY · REC`.

---

## 1. The buttons (what they do)

| Button | Function |
|---|---|
| **PLAY** | start / stop (toggle). Loops are infinite — it never auto-stops. |
| **OCT− / OCT+** | home & pages: root/octave & bar select (context). |
| **FX** | mixer page: drum mute/solo. Also the **global FX layer** (hold it anywhere + pads). |
| **SCL** | song-slot page (16 slots: save/load/cue). |
| **ENV** | bass voice page (style, tone, bite, level). |
| **LFO** | percussion page (4 overlay levels, clave direction). |
| **EDIT** | step editor for the current part (keys 1–16 toggle steps). |
| **GLO** | rack/groove page: SONG knob cycles grooves & leads; press again = record toggle. |
| **HOME** | main groove screen. |
| **SAVE** | save current settings into the selected slot (SCL page). |
| **ARP** | fill / roll. |
| **SEQ** | section jump (end of phrase), or cue on SCL. |
| **REC** | accent emphasis toggle (when not used as GLO's record). |

---

## 2. The pages (nav: press the labelled button)

| Page | What you see/do |
|---|---|
| **HOME** | groove runs; knobs: **BPM**, **FEEL** (swing), **LEVEL** (bass), **OVERLAY**; MODE knob = energy (BASE/LIFT/BREAK). |
| **FX** | drum mixer: pads 1–6 mute, 7–12 solo; row shows `M`/`S` state. |
| **ENV** | bass voice: KNOB 1 **TONE**, 2 **BITE** (resonance), 3 **LEVEL**, 4 **STYLE** (0 flute · 1 acid · 2 Moog · 3 trance). OCT± = root. |
| **LFO** | percussion: 4 overlay levels; OCT± = clave direction. |
| **EDIT** | step editor; OCT± switches bar 1/2. |
| **GLO** | rack: **SONG knob** cycles grooves 0–32 then **LEAD** entries 33–36; press GLO again = record; FILL = roll; SEQ = edit mode (choose sound, steps, BPM, strength, level). |
| **SCL** | slots 1–16: OSONG/OCT± pick; SEQ cues; SAVE saves; repeats/phrases knob, BPM knob, count-in knob; keys 1–16 cue directly. |

---

## 3. Live performance mechanics

**PLAY / loop** — press PLAY; the groove loops forever. No auto-stop.

**Drop (kick cut)** — hold **FX** + pad 1 to mute the kick instantly from any
page; tap again to bring it back. Same for hats/clap/perc/openhat/bass
(pads 1–6). Solos: pads 7–12.

**Build/breakdown** — add parts one-by-one with FX+mutes; for a breakdown mute
kick + clap (pads 1 & 3), raise MODE to BREAK, play a lead; for the drop,
unmute the kick on the downbeat.

**Fill** — **ARP** fires a fill/roll; double-tap = longer roll.

**Section jump** — **SEQ** jumps to the section end (use at phrase peaks).

**Energy** — MODE knob cycles BASE → LIFT → BREAK (arrangement + screen).

**Accent** — REC toggles accent emphasis on hits.

**Groove change mid-set** — GLO page, turn **SONG knob**: the next groove is
cued and lands at the next bar, playback never drops. **Tempo never changes
on song change — tempo is knob-only.**

**Leads** — stop playback → GLO → SONG knob past groove 32 → a LEAD entry →
PLAY. The lead plays over the groove backing. (Turn the SONG knob again to
leave the lead.)

**GLO record** — press GLO while already on GLO page: toggles sequence record
(records pad hits into the rack sequence).

---

## 4. Keyboard & pads

- 27 keys trigger the groove's instruments (pads) and, in the note row,
  parts: INTRO · VERSE · VARIATION · CHORUS · BREAK · END plus layer keys
  (overlays 0–3 and bass mute).
- Keys light dim/bright according to the active pattern map.

---

## 5. A free-party set, step by step

1. **Boot** → TRIBAL TEKNO @155. Press PLAY.
2. **Warm**: FX+mute kick+clap → hats/perc bed.
3. **Drop 1**: unmute kick (pad 1) on the downbeat; add clap.
4. **Progression**: GLO → SONG to a 165 BPM groove (tempo stays yours).
5. **Breakdown**: FX mute kick+clap; MODE → BREAK; SONG → LEAD FLUTE, PLAY.
6. **Peak**: SONG → a hardtek/double-kick groove; style 3 (trance) on ENV;
   XLEAD lead; ARP fills.
7. **Land**: drop again; loop or switch.

---

## 6. Flashing

- `firmware/compas-tekno.fwsc`, identity FM-1_710.
- Back up stock V15; if coming from another custom firmware, **return to
  official V15 first**; never unplug mid-write.

---

## 7. FAQ — quick reference for testing

**Q: Is the bank really the Electribe/UFO import? Are there only WAVs?**
A: Yes — the 15 drum voices are your UFO/Electribe imports (HardTekno,
TribeCore Narko, Tribe190), packed losslessly. Roles: kick/hat/clap/perc/
openhat/bass. The old TR-727/cumbia bank is archived, not used.

**Q: Are the synths real synthesis? I only heard a sine.**
A: Yes — the bass/lead voices are synthesized in-engine. The 4 bass styles:
0 flute (soft, near-pure by design) · 1 acid (nasal) · 2 Moog (fat, saw-
stacked, analog drift — the default) · 3 trance (bright band-pass). If it
still sounds too round, raise TONE (ENV KNOB 1) or choose style 3. Demos:
`demo/synth_*.wav`, `demo/live_*_bass.wav`.

**Q: Can I make my own sequences, or is it all pre-written?**
A: Both. The 33 grooves + 4 leads are pre-written. You can also:
- **Record live** (GLO twice → hit pads → GLO again) into the rack,
- **Step-program** (GLO → SEQ edit → pick sound, toggle 16 steps),
- **Edit the current part** (EDIT page),
- **Save it all** to a slot (SCL → SAVE) for later recall.

**Q: How do I hear the bass / a lead?**
A: Bass: ENV page → KNOB 4 changes style while you listen; FX+pad 12
solos it. Lead: stop → GLO → SONG knob past groove 32 → a LEAD entry → PLAY.

**Q: Why does tempo change when I change song?**
A: It shouldn't anymore — tempo is locked from boot (155) and only the BPM
knob changes it. If a build before this fix is running, reflash.

**Q: How do I do a drop/breakdown?**
A: Hold FX + pad 1 (kick mute) any time; bring it back on the downbeat for
the drop. Breakdown: FX + pads 1 & 3 (kick+clap out), MODE → BREAK.

**Q: Do songs loop forever? Can I change songs smoothly?**
A: Yes — playback never auto-stops. Change grooves live on GLO (SONG knob);
the new groove lands at the next bar without dropping playback.

**Q: The emulator has buttons the hardware doesn't, right?**
A: No — the emulator now mirrors the real FM-1 exactly (same 14 buttons,
27 keys, knobs, master). No invented controls.

**Q: Is flashing safe?**
A: Identity FM-1_710, CRCs verified, loader write-guard, V15-return gate on
the page. Back up stock; if coming from another custom firmware, return to
official V15 first.

**Q: What should I test first after flashing?**
A: Follow `docs/TESTING-GUIDE.md` (§0 baseline → §1 grooves/BPM → §3 synth
voices → §4 leads → §5 live/→ §6 loops).
