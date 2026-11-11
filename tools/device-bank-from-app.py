"""Rebuild build/board/* from the bank embedded in a built device image.

The flash-sized bank (a trim of the Studio bank: every sound capped near 0.3 s)
only exists inside firmware/compas-tekno-app.bin: the pack stream sits in
device_bank.o's rodata and the clips table in board.o's rodata. This tool
recovers both so a device rebuild reproduces the same bank without the Studio
WAVs.

Discovery is anchored on committed data:
  - the clips table is the unique run of 15 ascending (offset, frames) uint32
    pairs with sane frame counts, and
  - the stream starts with the same bytes as the web bank's sound 0 (the two
    banks share the first sounds), parsed from web/emu-fw/727_packed.h.

Writes 727.pack, 727.pcm, 727.inc, 727.json and 727_packed.h (the header
build_device.py would otherwise synthesize) under build/board. The full walk
must consume the stream exactly; any mismatch aborts.
"""
import hashlib
import json
import re
import struct
import sys
from pathlib import Path

root = Path(__file__).resolve().parents[1]
app_path = Path(sys.argv[1]) if len(sys.argv) > 1 else root / "firmware/compas-tekno-app.bin"
out = root / "build/device/board"          # device build's bank artifacts (kept separate from the web bank in build/board)
out.mkdir(parents=True, exist_ok=True)
app = app_path.read_bytes()

# --- locate the stream: it shares sound 0 with the committed web bank ------
header = (root / "web/emu-fw/727_packed.h").read_text()
web = bytes(int(m) for m in re.findall(r"\d+", header.split("[", 1)[1].split("];", 1)[0]))
sig = web[:64]
starts = [m.start() for m in re.finditer(re.escape(sig), app)]
assert len(starts) == 1, f"expected one sound-0 signature, found {len(starts)}"
stream_at = starts[0]

# --- locate the clips table --------------------------------------------------
table_at = None
for pos in range(0, len(app) - 120, 4):
    pairs = [struct.unpack_from("<II", app, pos + 8 * k) for k in range(15)]
    offs = [p[0] for p in pairs]
    frames = [p[1] for p in pairs]
    if offs[0] != 0 or any(offs[k] <= offs[k - 1] for k in range(1, 15)):
        continue
    if any(f == 0 or f > 100000 for f in frames) or not (50000 < sum(frames) < 300000):
        continue
    if max(offs) >= len(app) - stream_at:
        continue
    table_at = pos
    break
assert table_at is not None, "clips table not found"
offs = [struct.unpack_from("<II", app, table_at + 8 * k)[0] for k in range(15)]
frames = [struct.unpack_from("<II", app, table_at + 8 * k + 4)[0] for k in range(15)]
assert offs[0] == 0 and sum(frames) < 300000

# --- walk the stream: exact frame budgets, exact consumption -----------------
stream = app[stream_at:]
pos = 0
pcm = bytearray()
entries = []
value = 0
for index, budget in enumerate(frames):
    assert pos == offs[index], f"sound {index} starts at byte {pos}, table says {offs[index]}"
    start_pcm = len(pcm) // 2
    done = 0
    while done < budget:
        assert pos < len(stream), f"stream exhausted inside sound {index}"
        b = stream[pos]
        if b == 0:
            run = stream[pos + 1]
            assert run > 0, f"empty repeat run in sound {index}"
            pos += 2
            done += run
            pcm.extend(struct.pack("<%dh" % run, *([value] * run)))
        elif b < 128:
            pos += 1
            done += 1
            value += b - 64
            pcm.extend(struct.pack("<h", value))
        else:
            word = ((b & 0x7F) << 8) | stream[pos + 1]
            pos += 2
            done += 1
            value = word - 16384
            pcm.extend(struct.pack("<h", value))
    assert done == budget, f"sound {index} frame overrun"
    entries.append({
        "sound": index, "offset": start_pcm, "frames": budget,
        "packedOffset": offs[index],
        "packedBytes": (offs[index + 1] - offs[index]) if index + 1 < 15 else pos - offs[index],
        "peak16": max(abs(v[0]) for v in struct.iter_unpack("<h", bytes(pcm[start_pcm * 2:(start_pcm + budget) * 2]))),
    })
# the stream ends with sound 14; anything after it in the file is other rodata
assert pos > offs[-1], "walk ended before the last sound"

# --- accent gains: reference ratios, frame-independent (board-bank.py math) --
voices = json.loads((root / "board/727-levels.json").read_text())["voices"]
assert len(voices) == 15
accent_floor = min(level["accented"]["peak24"] / level["peak24"] for level in voices)
accent_gains = [
    round(level["accented"]["peak24"] / level["peak24"] / accent_floor * 4096)
    for level in voices
]
assert min(accent_gains) == 4096 and max(accent_gains) * 127 + 2048 < 256 * 4096

# --- write the bank artifacts ------------------------------------------------
pack = bytes(stream[:pos])
(out / "727.pack").write_bytes(pack)
(out / "727.pcm").write_bytes(pcm)
(out / "727.inc").write_text(
    "static const Clip clips[] = {\n" +
    "".join("    {%d, %d},\n" % (e["packedOffset"], e["frames"]) for e in entries) + "};\n" +
    "static const uint16_t accent_gain_q12[] = {\n    " +
    ", ".join(map(str, accent_gains)) + "\n};\n")
(out / "727_packed.h").write_text(
    "/* generated from build/board/727.pack (device trim) */\n"
    "const unsigned char compas_packed727[]={\n" +
    "\n".join(",".join(map(str, pack[i:i + 32])) + "," for i in range(0, len(pack), 32)) +
    "\n};\n")
(out / "727.json").write_text(json.dumps({
    "bytes": len(pcm) // 2, "packedBytes": len(pack), "clips": entries,
    "pcmSha256": hashlib.sha256(pcm).hexdigest(),
    "packSha256": hashlib.sha256(pack).hexdigest(),
    "source": str(app_path) + f"@0x{stream_at:x}",
    "tableAt": f"0x{table_at:x}",
}, indent=2) + "\n")

print(f"stream at 0x{stream_at:x}, table at 0x{table_at:x}")
print("sounds:", len(entries), "frames:", len(pcm) // 2, "packed bytes:", len(pack))
print("pack sha256:", hashlib.sha256(pack).hexdigest())
