// Node harness for the full-firmware WASM: WASI stub + _initialize, then
// drive the emulator exactly like the worklet does (buttons/enc/render).
const fs = require("fs");
(async () => {
  const b = fs.readFileSync(process.argv[2] || "web/emu/emu.wasm");
  const { instance } = await WebAssembly.instantiate(b, {
    wasi_snapshot_preview1: { fd_write() { return 0; } },
  });
  const ex = instance.exports;
  ex._initialize();
  ex.web_boot();
  const run = (ms) => { for (let i = 0; i < (ms / 1000) * 44100; i += 128) ex.web_render(128); };
  const log = (...a) => console.log(...a);

  run(300);
  log("after boot:   playing=", ex.web_seq_playing(), "tempo=", ex.web_tempo(), "study=", ex.web_study(), "draws=", ex.web_screen_draws());

  // PLAY button (panel bit 12) — press and release like the page does
  ex.web_buttons(1 << 12);
  run(60);
  ex.web_buttons(0);
  run(400);
  log("after PLAY:   playing=", ex.web_seq_playing(), "tempo=", ex.web_tempo());

  // BPM knob: SET page is knob 2 (enc[2]); HOME page also enc[2]=BPM
  ex.web_enc(2, +10);
  run(400);
  log("after BPM+10: tempo=", ex.web_tempo());

  // MIX page then FEEL knob (enc[3])
  ex.web_buttons(1 << 2); run(60); ex.web_buttons(0); run(300);
  ex.web_enc(3, +20); run(300);
  log("MIX + FEEL:   ok (draws=", ex.web_screen_draws(), ")");

  // a note key (pad hit): keys bits are note indices 0..26
  ex.web_keys(1 << 3); run(60); ex.web_keys(0); run(300);
  log("after key:    draws=", ex.web_screen_draws());

  // FILL
  ex.web_buttons(1 << 10); run(60); ex.web_buttons(0); run(300);
  log("after FILL:   playing=", ex.web_seq_playing());

  // STOP via PLAY toggle
  ex.web_buttons(1 << 12); run(60); ex.web_buttons(0); run(400);
  log("after STOP:   playing=", ex.web_seq_playing());

  // direct engine commands still work (transport bar path)
  log("web_cmd START:", ex.web_cmd(1, 0, 0));
  run(300);
  log("after cmd:    playing=", ex.web_seq_playing());
  log("web_cmd STOP: ", ex.web_cmd(2, 0, 0));
  run(200);

  // ---- palette check: framebuffer colors must equal the firmware palette ----
  const mem = ex.memory.buffer;
  const fb = new Uint16Array(mem, ex.web_screen(), 240 * 240);
  const q = (v, bits) => Math.round((v * 255) / ((1 << bits) - 1));
  const pal = {
    INK: [0, 0, 0], PLUM: [7, 20, 6], CREAM: [200, 212, 138], GOLD: [253, 255, 0],
    PINK: [137, 255, 0], MINT: [168, 224, 104], LILAC: [186, 220, 132], RED: [240, 255, 60],
    DIM: [96, 116, 70], LINE: [30, 44, 22], RAY: [10, 26, 12], GRILL: [24, 40, 18],
    WOOD: [20, 42, 14], CYAN: [150, 240, 100], ORANGE: [253, 255, 0], BLUE: [120, 200, 80],
    DIM2: [122, 150, 88], RING: [46, 66, 34], PAPER: [176, 204, 130], INKBLUE: [8, 28, 10],
  };
  const raw = {};
  for (const [k, [r, g, bb]] of Object.entries(pal)) raw[k] = ((r >> 3) << 11) | ((g >> 2) << 5) | (bb >> 3);
  const cnt = new Map();
  for (let i = 0; i < 240 * 240; i++) cnt.set(fb[i], (cnt.get(fb[i]) || 0) + 1);
  const top = [...cnt.entries()].sort((a, b) => b[1] - a[1]).slice(0, 12);
  let exact = 0;
  const rev = new Map(Object.entries(raw).map(([k, v]) => [v, k]));
  for (const [v, n] of top) {
    const name = rev.get(v) || "-";
    if (name !== "-") exact++;
    console.log(`  fb 0x${v.toString(16).padStart(4, "0")} x${n}  rgb(${q((v >> 11) & 31, 5)},${q((v >> 5) & 63, 6)},${q(v & 31, 5)})  ${name}`);
  }
  console.log(exact >= 6 ? "PALETTE OK (" + exact + "/12 exact matches)" : "PALETTE STILL OFF (" + exact + "/12)");
})();
