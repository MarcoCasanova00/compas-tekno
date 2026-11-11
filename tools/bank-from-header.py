"""Rebuild build/board bank artifacts from committed data.

The Studio WAVs and the original bank build folder are not in the repo, but
web/emu-fw/727_packed.h carries the whole packed stream (it was generated from
build/board/727.pack) and board/727-levels.json carries each sound's frame
count. The pack encoding is self-delimiting (byte 0 = repeat run, 1..127 =
delta token, 0x80.. = big-endian absolute word), so walking the stream while
counting frames recovers every sound boundary exactly. Any mismatch aborts.

Writes 727.pack, 727.pcm, 727.inc, 727-reference.inc and 727.json under
build/board — the same files tools/board-bank.py produces from the WAVs.
"""
import hashlib
import json
import re
import struct
import sys
from pathlib import Path

root = Path(__file__).resolve().parents[1]
folder = root / "build/board"
folder.mkdir(parents=True, exist_ok=True)

header = (root / "web/emu-fw/727_packed.h").read_text()
packed = bytes(int(m) for m in re.findall(r"\d+", header.split("[", 1)[1].split("];", 1)[0]))
levels = json.loads((root / "board/727-levels.json").read_text())
voices = levels["voices"]
reference_maximum = levels["referenceMaxPeak24"]
assert reference_maximum == max(level["peak24"] for level in voices)
assert len(voices) == 15

# Walk the stream, counting frames; split into sounds at each frame budget.
pcm = bytearray()
entries = []
pos = 0
frames = 0
value = 0
sound_frames = [v["frames"] for v in voices]
for index, budget in enumerate(sound_frames):
    packed_offset = pos
    start_pcm = len(pcm) // 2
    while frames < budget:
        assert pos < len(packed), f"stream exhausted inside sound {index}"
        b = packed[pos]
        if b == 0:
            run = packed[pos + 1]
            assert run > 0, f"empty repeat run in sound {index}"
            pos += 2
            frames += run
            pcm.extend(struct.pack("<%dh" % run, *([value] * run)))
        elif b < 128:
            pos += 1
            frames += 1
            value += b - 64
            pcm.extend(struct.pack("<h", value))
        else:
            word = ((b & 0x7F) << 8) | packed[pos + 1]
            pos += 2
            frames += 1
            value = word - 16384
            pcm.extend(struct.pack("<h", value))
    assert frames == budget, f"sound {index} frame overrun"
    entries.append({
        "sound": voices[index]["sound"],
        "offset": start_pcm,
        "frames": budget,
        "packedOffset": packed_offset,
        "packedBytes": pos - packed_offset,
        "peak16": max(abs(v[0]) for v in struct.iter_unpack("<h", bytes(pcm[start_pcm * 2:(start_pcm + budget) * 2]))),
        "referencePeak24": voices[index]["peak24"],
        "accentReferencePeak24": voices[index]["accented"]["peak24"],
    })
    frames = 0
assert pos == len(packed), f"trailing bytes: {len(packed) - pos}"

# Accent gains follow the retained Studio common-lift math (board-bank.py).
accent_floor = min(level["accented"]["peak24"] / level["peak24"] for level in voices)
accent_gains = [
    round(level["accented"]["peak24"] / level["peak24"] / accent_floor * 4096)
    for level in voices
]
assert min(accent_gains) == 4096

(folder / "727.pcm").write_bytes(pcm)
(folder / "727.pack").write_bytes(packed)
for filename, offset in (("727.inc", "packedOffset"), ("727-reference.inc", "offset")):
    (folder / filename).write_text(
        "static const Clip clips[] = {\n" +
        "".join("    {%d, %d},\n" % (e[offset], e["frames"]) for e in entries) + "};\n" +
        "static const uint16_t accent_gain_q12[] = {\n    " +
        ", ".join(map(str, accent_gains)) + "\n};\n")
(folder / "727.json").write_text(json.dumps({
    "bytes": len(pcm) // 2, "packedBytes": len(packed), "clips": entries,
    "referenceArchiveSha256": levels["archiveSha256"],
    "pcmSha256": hashlib.sha256(pcm).hexdigest()}, indent=2) + "\n")

print("sounds:", len(entries), "frames:", len(pcm) // 2, "packed bytes:", len(packed))
print("pcm sha256:", hashlib.sha256(pcm).hexdigest())
