/* SPDX-License-Identifier: GPL-3.0-only
 * Tribal Tekno AudioWorklet: device-style playback of the packed Compás bank.
 * Same delta/RLE decoder + choke groups as the firmware's Player. */

const GROUPS = [0,0,1,1,2,3,4,5,5,6,6,7,7,8,9];
const RATE = 44100;

class TeknoWorklet extends AudioWorkletProcessor {
  constructor() {
    super();
    this.playing = false;
    this.study = 0;
    this.bpm = 155;
    this.vol = 0.85;
    this.clips = [];          // Float32Array per slot
    this.parts = [];          // current study
    this.steps = 32;
    this.kit = [0,0,0,0,0,0];
    this.stepPos = 0;         // fractional sample position within 16th
    this.stepLen = 0;
    this.curStep = 0;
    this.voices = [];
    for (let i = 0; i < 6; i++) this.voices.push({active:false, pos:0, clip:0, gain:0, group:-1});
    this.bass = {phase:0, lp:0, active:false, pos:0, len:0, freq:55, vol:0};
    this.port.onmessage = (e) => this.onMsg(e.data);
  }

  onMsg(m) {
    switch (m.type) {
      case 'init':
        this.clips = m.clips;
        break;
      case 'set':
        this.study = m.study;
        this.parts = m.parts;
        this.steps = m.steps;
        this.kit = m.kit;
        if (m.bpm) this.bpm = m.bpm;
        this.stepLen = RATE * 60 / this.bpm / 4;
        break;
      case 'bpm':
        this.bpm = m.bpm;
        this.stepLen = RATE * 60 / this.bpm / 4;
        break;
      case 'play': this.playing = true; this.curStep = 0; this.stepPos = 0; break;
      case 'stop': this.playing = false; this.clearVoices(); break;
      case 'vol': this.vol = m.vol; break;
    }
  }

  clearVoices() {
    for (const v of this.voices) v.active = false;
    this.bass.active = false;
  }

  trigger(role, slot, vel) {
    const v = this.voices[role];
    const g = GROUPS[slot];
    for (const ov of this.voices) if (ov.active && ov.group === g) ov.active = false;
    v.active = true; v.pos = 0; v.clip = slot; v.gain = vel / 128; v.group = g;
  }

  bassNote(freq, len) {
    this.bass.freq = freq; this.bass.len = len; this.bass.pos = 0;
    this.bass.active = true; this.bass.lp = 0;
  }

  stepEvents() {
    const st = this.curStep;
    for (let r = 0; r < 6; r++) {
      const p = this.parts[r];
      if (!p) continue;
      const val = p.base[st];
      if (!val) continue;
      const slot = this.kit[r];
      if (slot > 0) this.trigger(r, slot, val === 2 ? 115 : 83);
      else if (val >= 16) {
        const note = val - 16;
        this.bassNote(55 * Math.pow(2, note / 12), this.stepLen * 4 / RATE);
      }
    }
    this.curStep = (this.curStep + 1) % this.steps;
  }

  tick() {
    if (!this.playing) return;
    this.stepPos += 1;
    if (this.stepPos >= this.stepLen) {
      this.stepPos -= this.stepLen;
      this.stepEvents();
    }
  }

  sampleVoice(v) {
    if (!v.active) return 0;
    const c = this.clips[v.clip];
    if (!c) return 0;
    if (v.pos >= c.length) { v.active = false; return 0; }
    return c[v.pos++ | 0] * v.gain;
  }

  sampleBass() {
    const b = this.bass;
    if (!b.active) return 0;
    if (b.pos >= b.len) { b.active = false; return 0; }
    b.pos += 1;
    b.phase += b.freq / RATE;
    if (b.phase >= 1) b.phase -= 1;
    const saw = 2 * b.phase - 1;
    b.lp += 0.12 * (saw - b.lp);
    const env = Math.min(1, b.pos / 200) * Math.exp(-3 * b.pos / b.len);
    return b.lp * env * 0.8;
  }

  process(inputs, outputs) {
    const out = outputs[0];
    const ch0 = out[0];
    const ch1 = out[1] || out[0];
    for (let i = 0; i < ch0.length; i++) {
      this.tick();
      let mix = 0;
      for (const v of this.voices) mix += this.sampleVoice(v);
      mix += this.sampleBass();
      mix *= this.vol;
      // soft clip
      mix = Math.tanh(mix * 1.4) / Math.tanh(1.4);
      ch0[i] = mix; ch1[i] = mix;
    }
    return true;
  }
}

registerProcessor('tekno-worklet', TeknoWorklet);
