#!/usr/bin/env python3
"""Compile the Compás firmware for the FM-1 against the pinned Felucca v1.6
BSP (board/BSP.json hashes) using the JieLi PI32v2 toolchain natively.

Usage:
  JIELI_TOOLCHAIN=~/.jieli/toolchain python3 tools/build_device.py
Outputs build/device/app.bin (+ app.elf, disassembly).
"""
import json, os, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BSP = Path(os.environ.get("COMP_BSP", ROOT / "release-inputs/felucca"))
TC = Path(os.environ.get("JIELI_TOOLCHAIN", Path.home() / ".jieli/toolchain"))
OUT = ROOT / "build/device"
OUT.mkdir(parents=True, exist_ok=True)

CLANG = TC / "pi32v2/bin/clang"
LD = TC / "pi32v2/bin/ld"
OBJCOPY = TC / "common/bin/objcopy"
OBJDUMP = TC / "common/bin/objdump"

CFLAGS = ["-target", "pi32v2", "-mcpu=r3", "-mfprev1", "-Oz", "-g", "-std=c11",
          "-Wall", "-Wextra", "-Werror", "-Wno-unused-function", "-Wno-type-limits",
          "-ffreestanding", "-fno-builtin", "-ffunction-sections", "-fdata-sections",
          "-include", "stdint.h", "-DCOMPAS_BOARD",
          f"-I{ROOT/'build/device/board'}", f"-I{ROOT/'board'}", f"-I{ROOT/'src'}",
          f"-I{ROOT/'build'}", f"-I{ROOT/'build/board'}",
          f"-I{BSP/'firmware/hal'}", f"-I{BSP/'firmware/src'}"]

def cc(src, out, extra=None):
    flags = list(CFLAGS)
    if Path(src).suffix == ".c":
        flags += ["-include", "stdint.h"]
    else:
        flags = [f for f in flags if f not in ("-include", "stdint.h")]
    subprocess.run([str(CLANG), *flags, *(extra or []), "-c", str(src), "-o", str(out)], check=True)

# 1) BSP bootstrap + our IRQ
cc(BSP/"firmware/crt0.S", OUT/"crt0.o")
cc(BSP/"firmware/hal/fm1_vec.S", OUT/"fm1_vec.o")
cc(ROOT/"board/irq.S", OUT/"irq.o")
# 2) Compás units
for u in ("engine", "board", "panel", "piano", "fm1"):
    src = ROOT/"src/engine.c" if u == "engine" else ROOT/"board"/f"{u}.c"
    cc(src, OUT/f"{u}.o")
# 3) BSP LCD driver (no audio.c: our board/irq.S feeds the engine)
cc(BSP/"firmware/src/lcd.c", OUT/"lcd.o",
   extra=["-include", "fm1_lcd_hw.h", "-include", "fm1_sys.h"])
# 4) packed tekno bank array (device trim lives in build/device/board)
bank_h = ROOT/"build/device/board/727_packed.h"
if not bank_h.exists():
    import base64
    pack = (ROOT/"build/device/board/727.pack").read_bytes()
    rows = "\n".join(",".join(map(str, pack[i:i+32])) + "," for i in range(0, len(pack), 32))
    bank_h.write_text("/* generated from build/device/board/727.pack */\n"
                      "const unsigned char compas_packed727[]={\n" + rows + "\n};\n")
bank_c = OUT/"device_bank.c"
bank_c.write_text('#include "727_packed.h"\n')
cc(bank_c, OUT/"device_bank.o", extra=[f"-I{ROOT/'build/device/board'}"])
# 4) link
elf = OUT/"app.elf"
subprocess.run([str(LD), "-T", str(BSP/"firmware/app.ld"), "-o", str(elf),
                str(OUT/"crt0.o"), str(OUT/"fm1_vec.o"), str(OUT/"irq.o"),
                str(OUT/"engine.o"), str(OUT/"board.o"), str(OUT/"panel.o"),
                str(OUT/"piano.o"), str(OUT/"fm1.o"), str(OUT/"lcd.o"),
                str(OUT/"device_bank.o")], check=True)
subprocess.run([str(OBJCOPY), "-O", "binary", "-j", ".text", str(elf), str(OUT/"app.bin")], check=True)
subprocess.run([str(OBJDUMP), "-d", str(elf)], stdout=open(OUT/"app.dis", "w"), check=True)
print("linked", elf, "size", elf.stat().st_size, "| app.bin", (OUT/"app.bin").stat().st_size)
