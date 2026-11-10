#!/usr/bin/env python3
"""Generate studies_fw.json: presets with their library index (score.json order)."""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
score = json.loads((ROOT / "src/score.json").read_text())
sets = json.loads((ROOT / "board/set.json").read_text())
by_id = {s["id"]: i for i, s in enumerate(score["studies"])}
import re

def phrase(text, steps, voice):
    values = [0] * steps
    for token in text.split():
        m = re.fullmatch(r"(\d+)(!|:[0-9]+)?", token)
        idx, annotation = int(m[1]), m[2] or ""
        values[idx] = (16 + int(annotation[1:])) if voice == "bass" else (2 if annotation else 1)
    return values

out = []
for song in sets["songs"]:
    if song["study"] not in by_id:
        continue
    s = score["studies"][by_id[song["study"]]]
    steps = s["meter"] * 8
    parts = []
    for name, voice, *variants in s["parts"]:
        parts.append({"name": name, "base": phrase(variants[0], steps, voice)})
    out.append({"index": by_id[song["study"]], "title": song["title"],
                "id": song["study"], "steps": steps, "parts": parts})
(ROOT / "web/emu-fw/studies_fw.json").write_text(json.dumps(out, ensure_ascii=False))
print(f"wrote {len(out)} presets")
