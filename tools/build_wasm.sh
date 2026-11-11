#!/bin/sh
# Build the full-firmware browser WASM (web/emu/emu.wasm) from the same board
# sources that go into the device image: emu_full.c (simulated FM-1 host) +
# engine + board + panel + piano. Requires emsdk's clang on PATH, e.g.
#   PATH=~/emsdk/upstream/bin:$PATH tools/build_wasm.sh
set -e
cd "$(dirname "$0")/.."
OUT=web/emu/emu.wasm
clang --target=wasm32 -fuse-ld=lld -Oz -std=c11 -Wall -Wextra \
  -Wno-unused-function -Wno-type-limits \
  -ffreestanding -fno-builtin -nostdlib -DCOMPAS_BOARD \
  -Iboard -Isrc -Ibuild -Ibuild/board -Iweb/emu-fw \
  -Wl,--no-entry -Wl,--strip-all -Wl,-z,stack-size=65536 \
  -Wl,--initial-memory=8388608 -Wl,--max-memory=8388608 \
  -Wl,--export=web_boot -Wl,--export=web_sample_rate \
  -Wl,--export=web_render -Wl,--export=web_out_l -Wl,--export=web_out_r \
  -Wl,--export=web_screen -Wl,--export=web_screen_draws \
  -Wl,--export=web_leds -Wl,--export=web_button_id \
  -Wl,--export=web_keys -Wl,--export=web_buttons -Wl,--export=web_enc \
  -Wl,--export=web_now_ms -Wl,--export=web_seq_playing \
  -Wl,--export=web_kb_mode -Wl,--export=web_cmd -Wl,--export=web_tempo \
  -Wl,--export=web_study -Wl,--export=_initialize \
  web/emu-fw/emu_full.c src/engine.c board/board.c board/panel.c board/piano.c \
  -o "$OUT" 2>&1 | grep -v "^ \|^web/emu-fw/emu_full.c:.*warning: conflicting types" || true
ls -la "$OUT"
