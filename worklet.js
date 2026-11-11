// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 M-Vave-OP1 contributors
// The app in an AudioWorklet: emu.wasm is firmware/app/ on the host platform (web/emu/emu.c). Each render quantum
// asks it for 128 frames, which runs the device's clock forward; between quanta the page's input goes in and, about
// 30 times a second (4 when the page is hidden), the screen and the LEDs go out. After X0X's web/emu/worklet.js
// (charlesvestal/fm1-x0x, GPL-3.0), as M-Vave-OP1's.

export const HOLD_MS = 24;   // a key or button change stays this long (device time) before the next: the FM-1 debounces

// minimal WASI stub: the standalone build only imports wasi_snapshot_preview1.fd_write (stdout)
const WASI = { wasi_snapshot_preview1: {
  fd_write(fd, iovs, iovsLen, nwritten) { return 0; },
} };

if (typeof registerProcessor === "function") {
  class Emu extends AudioWorkletProcessor {
    constructor() {
      super();
      this.ex = null;
      this.sentDraws = -1;
      this.sentLeds = "";
      this.lastFrame = 0;
      this.period = 1 / 30;
      this.queue = [];
      this.nextOk = 0;
      this.vol = 1.0;
      this.port.onmessage = (e) => this.onMessage(e.data);
    }

    async onMessage(m) {
      if (m.type === "load") {
        try {
          const { instance } = await WebAssembly.instantiate(m.wasm, WASI);
          const ex = instance.exports;
          if (ex._initialize) ex._initialize();
          ex.web_boot();
          this.ex = ex;
          const ids = Array.from({ length: 14 }, (_, b) => ex.web_button_id(b));
          this.port.postMessage({ type: "ready", rate: ex.web_sample_rate(), ids });
        } catch (err) {
          this.port.postMessage({ type: "error", message: String((err && err.message) || err) });
        }
        return;
      }
      const ex = this.ex;
      if (!ex) return;
      if (m.type === "buttons" || m.type === "keys") this.queue.push(m);
      else if (m.type === "enc") ex.web_enc(m.role, m.n | 0);
      else if (m.type === "cmd") ex.web_cmd(m.cmd | 0, m.a | 0, m.b | 0);
      else if (m.type === "getstatus") this.port.postMessage({ type: "status", playing: ex.web_seq_playing(), tempo: ex.web_tempo(), study: ex.web_study() });
      else if (m.type === "midi") ex.web_midi(m.data[0] | 0, m.data[1] | 0, m.data[2] | 0);
      else if (m.type === "visible") this.period = m.on ? 1 / 30 : 1 / 4;
      else if (m.type === "vol") this.vol = Math.max(0, Math.min(1.5, m.v / 100));
    }

    publish() {
      const ex = this.ex, mem = ex.memory.buffer, msg = { type: "frame" }, transfer = [];
      msg.kbMode = ex.web_kb_mode ? ex.web_kb_mode() : 0;
      const leds = new Uint8Array(mem, ex.web_leds(), 11).slice();   // 11 columns, row-bit encoded
      const key = leds.join("");
      if (key !== this.sentLeds) {
        this.sentLeds = key;
        msg.leds = leds;
      }
      const draws = ex.web_screen_draws();
      if (draws !== this.sentDraws) {
        this.sentDraws = draws;
        msg.fb = new Uint16Array(mem, ex.web_screen(), 240 * 240).slice();
        transfer.push(msg.fb.buffer);
      }
      if (msg.leds || msg.fb) this.port.postMessage(msg, transfer);
    }

    input() {
      const ex = this.ex, now = ex.web_now_ms();
      while (this.queue.length && now - this.nextOk >= 0) {
        const m = this.queue.shift();
        if (m.type === "buttons") ex.web_buttons(m.mask >>> 0);
        else ex.web_keys(m.mask >>> 0);
        this.nextOk = now + HOLD_MS;
      }
    }

    process(inputs, outputs) {
      const ex = this.ex;
      if (!ex) return true;
      if (this.queue.length) this.input();
      const out = outputs[0], n = out[0].length;
      ex.web_render(n);
      const vol = this.vol;
      const l = new Float32Array(ex.memory.buffer, ex.web_out_l(), n);
      const r = new Float32Array(ex.memory.buffer, ex.web_out_r(), n);
      for (let i = 0; i < n; i++) out[0][i] = l[i] * vol;
      if (out[1]) for (let i = 0; i < n; i++) out[1][i] = r[i] * vol;
      if (currentTime - this.lastFrame >= this.period) {
        this.lastFrame = currentTime;
        this.publish();
      }
      return true;
    }
  }
  registerProcessor("fm1-emu", Emu);
}
