/* SPDX-License-Identifier: GPL-3.0-only
 * WASM entry points for the browser emulator: the real Compás engine
 * (engine.c + tekno_board_shim.c) compiled to WebAssembly.
 */
#include "engine.h"

unsigned fw_init(unsigned sr) { return cp_init(sr); }
unsigned fw_command(unsigned cmd, unsigned a, unsigned b) { return cp_command(cmd, a, b); }
unsigned fw_status(unsigned key) { return cp_status(key); }
unsigned fw_render(unsigned count) { return cp_render(count); }
unsigned fw_step(unsigned role, unsigned step) { return cp_step(role, step); }
unsigned fw_level(unsigned role) { return cp_level(role); }
int fw_pcm(unsigned i) { return cp_pcm()[i]; }
unsigned fw_state_bytes(void) { return cp_state_bytes(); }
