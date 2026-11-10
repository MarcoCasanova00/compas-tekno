/* SPDX-License-Identifier: GPL-3.0-only
 * Simple FIFO player: the main thread pushes rendered Float32Array chunks
 * from the real firmware WASM; this worklet pulls them to the output. */
class FwPlayer extends AudioWorkletProcessor {
  constructor() {
    super();
    this.queue = [];
    this.port.onmessage = (e) => {
      this.queue.push(e.data);
      if (this.queue.length > 64) this.queue.shift();
    };
  }
  process(inputs, outputs) {
    const out = outputs[0];
    const ch0 = out[0], ch1 = out[1] || out[0];
    for (let i = 0; i < ch0.length; i++) {
      let v = 0;
      if (this.queue.length) {
        const q = this.queue[0];
        v = q[i] !== undefined ? q[i] : 0;
        if (i === q.length - 1) this.queue.shift();
      }
      ch0[i] = v; ch1[i] = v;
    }
    return true;
  }
}
registerProcessor('fw-player', FwPlayer);
