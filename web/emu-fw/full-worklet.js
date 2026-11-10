/* SPDX-License-Identifier: GPL-3.0-only
 * Full firmware in an AudioWorklet: the emu_full.wasm (board.c + panel.c +
 * stage.c + engine.c) renders audio; between quanta the screen/LEDs go out.
 */
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
      this.port.onmessage = (e) => this.onMessage(e.data);
    }
    async onMessage(m) {
      if (m.type === "load") {
        try {
          const { instance } = await WebAssembly.instantiate(m.wasm, WASI);
          const ex = instance.exports;
          ex._initialize ? ex._initialize() : 0;
          ex.web_boot();
          this.ex = ex;
          this.port.postMessage({ type: "ready", rate: ex.web_sample_rate() });
        } catch (err) {
          this.port.postMessage({ type: "error", message: String((err && err.message) || err) });
        }
        return;
      }
      const ex = this.ex;
      if (!ex) return;
      if (m.type === "buttons" || m.type === "keys") this.queue.push(m);
      else if (m.type === "enc") ex.web_enc(m.role, m.n | 0);
    }
    publish() {
      const ex = this.ex, mem = ex.memory.buffer, msg = { type: "frame" }, transfer = [];
      const draws = ex.web_screen_draws();
      if (draws !== this.sentDraws) {
        this.sentDraws = draws;
        msg.fb = new Uint16Array(mem, ex.web_screen(), 240 * 240).slice();
        transfer.push(msg.fb.buffer);
      }
      const leds = new Uint8Array(mem, ex.web_leds(), 11).slice();
      const key = leds.join("");
      if (key !== this.sentLeds) { this.sentLeds = key; msg.leds = leds; }
      if (msg.fb || msg.leds) this.port.postMessage(msg, transfer);
    }
    input() {
      const ex = this.ex, now = ex.web_now_ms();
      while (this.queue.length && now - this.nextOk >= 0) {
        const m = this.queue.shift();
        if (m.type === "buttons") ex.web_buttons(m.mask >>> 0);
        else ex.web_keys(m.mask >>> 0);
        this.nextOk = now + 24;
      }
    }
    process(inputs, outputs) {
      const ex = this.ex;
      if (!ex) return true;
      if (this.queue.length) this.input();
      const out = outputs[0], n = out[0].length;
      ex.web_render(n);
      out[0].set(new Float32Array(ex.memory.buffer, ex.web_out_l(), n));
      if (out[1]) out[1].set(new Float32Array(ex.memory.buffer, ex.web_out_r(), n));
      if (currentTime - this.lastFrame >= this.period) {
        this.lastFrame = currentTime;
        this.publish();
      }
      return true;
    }
  }
  registerProcessor("fm1-emu", Emu);
}
