/* SPDX-License-Identifier: GPL-3.0-only
 * Full Compás firmware in the browser: board.c + panel.c + piano.c + stage.c
 * + engine.c against a simulated FM-1 (240x240 framebuffer, keys, encoders,
 * clock). Modeled on FM-23's host platform; drives the real panel/stage UI.
 */
#include <stdint.h>

/* minimal libc for -nostdlib builds */
void *memset(void *d, int c, unsigned n) { unsigned char *p = d; while (n--) *p++ = (unsigned char)c; return d; }
void *memcpy(void *d, const void *s, unsigned n) { unsigned char *dd = d; const unsigned char *ss = s; while (n--) *dd++ = *ss++; return d; }
int memcmp(const void *a, const void *b, unsigned n) { const unsigned char *x = a, *y = b; while (n--) { if (*x != *y) return *x - *y; x++; y++; } return 0; }
unsigned strlen(const char *s) { const char *p = s; while (*p) p++; return (unsigned)(p - s); }
int strcmp(const char *a, const char *b) { while (*a && *a == *b) { a++; b++; } return (unsigned char)*a - (unsigned char)*b; }
char *strcpy(char *d, const char *s) { char *p = d; while ((*p++ = *s++)); return d; }

/* ---- simulated SoC ---- */
#define FS 44100u
#ifndef FM1_NCOL
#define FM1_NCOL 11u
#define FM1_NKEY 41u
#define FM1_NENC 7u
static const int8_t FM1_KEYMAP[6][FM1_NCOL] = {
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    { 5, 11,  4, 10,  3,  9,  2,  8, -1, -1, -1},
    {34, 35, 36, 37, 38, 40, 39, 13,  7,  6, 12},
    {23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33},
    { 0,  1, 15, 14, 17, 16, 19, 18, 20, 21, 22},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
};
static const uint8_t FM1_ENC[FM1_NENC][4] = {
    {0, 0, 1, 0}, {2, 0, 3, 0}, {8, 1, 9, 1}, {8, 0, 9, 0},
    {6, 0, 7, 0}, {4, 0, 5, 0}, {0, 5, 1, 5},
};
#endif
static uint32_t host_ticks_now;
static uint32_t host_ms;
static volatile struct {
    uint32_t notes;
    uint32_t buttons;
    int16_t enc_steps[7];
} fm1_in;
static uint32_t host_edge_buttons, host_edge_notes;

uint32_t fm1_ticks(void) { return host_ticks_now; }
void fm1_delay_ms(uint32_t ms) { (void)ms; }
void fm1_wdt_feed(void) {}
void fm1_wdt_arm(uint32_t w) { (void)w; }
void fm1_reset_reason(void) {}
void fm1_time_init(void) {}
void fm1_irq_init(void) {}
void fm1_irq_enable_all(void) {}
void fm1_adc_init(void) {}
int32_t fm1_adc_read(uint32_t ch) { (void)ch; return 700; }  /* MASTER ~ 55% */
void fm1_input_init(void) {}
void fm1_input_scan(void) {}
uint32_t fm1_input_edges(uint32_t *released) {
    uint32_t now = fm1_in.buttons, p = now & ~host_edge_buttons;
    if (released) *released = host_edge_buttons & ~now;
    host_edge_buttons = now;
    return p;
}
uint32_t fm1_input_note_edges(void) {
    uint32_t now = fm1_in.notes, p = now & ~host_edge_notes;
    host_edge_notes = now;
    return p;
}
int32_t fm1_enc_take(uint32_t e) {
    int32_t s = (e < 7u) ? fm1_in.enc_steps[e] : 0;
    if (e < 7u) fm1_in.enc_steps[e] = 0;
    return s;
}
uint32_t fm1__lock(void) { return 0; }
void fm1__unlock(uint32_t p) { (void)p; }

/* board.c HAL surface (from fm1.c on the device) */
uint32_t board_lock(void) { return 0; }
void board_unlock(uint32_t p) { (void)p; }
uint32_t board_ticks(void) { return fm1_ticks(); }
uint32_t board_pending(void) { return 0; }
void board_ack_aux(uint32_t p) { (void)p; }
unsigned board_free_half(void) { return 1; }
void board_ack_half(void) {}
void board_disable_output(void) {}
#include "727_packed.h"
void fm1_audio_stop(void) {}
void fm1_audio_ack_half(void) {}
void fm1_audio_ack_aux(uint8_t b) { (void)b; }
uint32_t fm1_audio_pending(void) { return 0; }
unsigned fm1_audio_free_half(void) { return 1; }
void fm1_audio_init(uint32_t *dma, uint32_t words, void (*isr)(void), uint32_t cfg) {
    (void)dma; (void)words; (void)isr; (void)cfg;
}
void fm1_irq(void) {}
void isr_alnk0(void) {}
void fm1_enter_uboot(void) { for (;;) {} }
void fm1_led_key(uint32_t id, int on) { (void)id; (void)on; }
void fm1_led_dim_level(uint32_t lo) { (void)lo; }

/* ---- framebuffer display (ST7789 subset) ---- */
static uint16_t host_fb[240 * 240];
static uint32_t host_blits, host_pixels;
static uint16_t win_x0, win_y0, win_x1, win_y1;
static uint32_t win_i;

void lcd_init(void) {}
void lcd_sync(void) { host_blits++; }
void lcd_wake_now(void) {}
void lcd_set_scroll(uint16_t a, uint16_t b) { (void)a; (void)b; }
void lcd_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    win_x0 = x0; win_y0 = y0; win_x1 = x1; win_y1 = y1; win_i = 0;
}
void lcd_data(const uint16_t *px, uint32_t n) {
    /* n is in BYTES on the wire — the BSP's lcd_blit calls lcd_data(px, w*h*2).
     * Every word of a pixel stream is display_word()-swapped RGB565 (the LCD
     * takes big-endian words); the panel shows the unswapped value, so that
     * is what host_fb stores. */
    for (uint32_t i = 0; i * 2u < n && win_i < 240u * 240u; i++) {
        uint32_t col = win_x0 + (win_i % (win_x1 - win_x0 + 1u));
        uint32_t row = win_y0 + (win_i / (win_x1 - win_x0 + 1u));
        uint16_t w = px[i];
        if (col < 240u && row < 240u) host_fb[row * 240u + col] = (uint16_t)((w >> 8) | (w << 8));
        win_i++;
    }
}
void lcd_fill(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint16_t c) {
    /* The BSP's lcd_fill byte-swaps internally, so the color the panel shows
     * is c itself. */
    for (uint32_t j = y; j < y + h && j < 240u; j++)
        for (uint32_t i = x; i < x + w && i < 240u; i++)
            host_fb[j * 240u + i] = c;
    host_blits++;
    host_pixels += w * h;
}
void lcd_blit(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const uint16_t *px) {
    for (uint32_t j = 0; j < h; j++)
        for (uint32_t i = 0; i < w; i++) {
            uint32_t cx = x + i, cy = y + j;
            uint16_t p = px[j * w + i];
            if (cx < 240u && cy < 240u)
                host_fb[cy * 240u + cx] = (uint16_t)((p >> 8) | (p << 8));
        }
    host_blits++;
    host_pixels += w * h;
}

/* ---- the Compás board UI modules ---- */
#include "panel.h"
#include "piano.h"

#define STAGE_TRANSFER_POLL(pixels) ((void)(pixels))
#include "stage.c"
#undef STAGE_TRANSFER_POLL

/* ---- globals the loop needs (from fm1.c) ---- */
static Panel panel;
static uint32_t cameo_active, cameo_started;
static uint32_t compas_supervisor_watermark;
static uint8_t fm1_led[FM1_NCOL];
#define SET_SLOTS 16u

static PanelKeys screen_keys;
static uint8_t dim_keys[11], bright_keys[11], queued_keys[11];
static void update_key_map(PanelKeys map) {
    screen_keys = map;
    for (unsigned col = 0; col < 11u; col++) {
        unsigned dim = 0, bright = 0, queued = 0;
        for (unsigned row = 1; row < 5u; row++) {
            int id = FM1_KEYMAP[row][col];
            if (id < 14) continue;
            unsigned note = 1u << (id - 14);
            if (note & (map.parts | map.upper | map.pads)) dim |= 1u << row;
            if (note & (map.active | map.enabled)) bright |= 1u << row;
            if (note & map.queued) queued |= 1u << row;
        }
        dim_keys[col] = (uint8_t)dim;
        bright_keys[col] = (uint8_t)bright;
        queued_keys[col] = (uint8_t)queued;
    }
}
static void update_key_lights(uint32_t now, unsigned rapid) {
    (void)now;
    unsigned dim = 1, pulse = 0;
    (void)rapid;
    for (unsigned col = 0; col < 11u; col++)
        fm1_led[col] = (uint8_t)(bright_keys[col] | (pulse ? queued_keys[col] : 0u) | (dim ? dim_keys[col] : 0u));
}

/* ---- boot + main loop (mirrors fm1_cstart, trimmed of USB/OTA) ---- */
static unsigned opening;
static uint32_t splash_started, last_frame, last_master;
static uint32_t previous_song = SET_SLOTS;
static unsigned rapid;

static void firmware_frame(void) {
    fm1_input_scan();
    int32_t enc[7];
    const uint32_t buttons = fm1_input_edges(0), keys = fm1_in.notes;
    unsigned activity = (buttons | (keys & ~panel.held)) != 0;
    for (unsigned i = 0; i < 7u; i++) {
        enc[i] = fm1_enc_take(i);
        if (enc[i]) activity = 1;
    }
    const uint32_t now = fm1_ticks();
    panel_scan(&panel, buttons, keys, enc, now);
    update_key_lights(now, rapid);
    if (now - last_master >= 480000u) {
        last_master = now;
        panel_master(&panel, fm1_adc_read(4));
    }
    if (activity && !opening) stage_touch(now);
    if ((opening && activity) || now - last_frame >= 120000u) {
        last_frame = now;
        const BoardView v = board_view(panel.page == PANEL_BASS ? BOARD_BASS_LINE_ROLE : panel.role);
        if (opening && !activity && !v.fault && now - splash_started < 15600000u) return;
        if (v.song != previous_song) {
            previous_song = v.song;
            cameo_active = v.song < SET_SLOTS;
            cameo_started = now;
        }
        if (v.fault || now - cameo_started >= 21600000u) cameo_active = 0;
        PanelKeys k = panel_keys(&panel, v);
        update_key_map(k);
        rapid = v.next_bar;
        stage_offer(&panel, v, k, board_rack_feedback(), opening, cameo_active && !v.result, now);
        opening = 0;
        compas_supervisor_watermark++;
    }
    do { stage_work(); } while (jobs && measurement == 1);
}

void web_boot(void) {
    memset(host_fb, 0, sizeof host_fb);
    memset((void*)fm1_in.enc_steps, 0, sizeof fm1_in.enc_steps);
    host_ticks_now = 0; host_ms = 0;
    board_init();
    panel_lang = 0;
    panel_master(&panel, fm1_adc_read(4));
    /* The browser bench has no master pot. panel_master parks it at boot (the
     * device stays quiet until the pot moves), so take the level explicitly —
     * 100 = unity, what the page rendered before the real audio path. */
    board_submit((BoardCommand){BOARD_MASTER, 100, 0});
    /* Boot straight into the tekno groove: study 29 TRIBAL TEKNO · free party
     * (four-on-floor, offbeat hats, 155 BPM, UFO kit) — same cue on device. */
    board_submit((BoardCommand){BOARD_GROOVE_CUE, 29, 0});
    splash();
    opening = 1;
    splash_started = last_frame = last_master = 0;
    previous_song = SET_SLOTS;
    rapid = 0;
}

uint32_t web_sample_rate(void) { return FS; }

/* stereo Q15, before the 24-bit shift */
#define WEB_MAX 1024u
static int32_t web_buf[2u * WEB_MAX];
static float web_l[WEB_MAX], web_r[WEB_MAX];
static uint32_t web_frames;
static uint32_t web_draws;

void web_render(uint32_t n) {
    uint32_t b;
    if (n > WEB_MAX) n = WEB_MAX;
    for (b = 0; b < n; b += 128u) {
        /* Real device audio path: this services the command mailbox (so panel
         * actions apply and BUSY clears) and publishes the board view — what
         * the FM-1's audio ISR runs on every half. */
        board_render_audio(&web_buf[2u * b]);
        /* device main loop cadence: one frame per 16 ms of audio */
        web_frames += 128u;
        while (web_frames >= FS * 16u / 1000u) {
            web_frames -= FS * 16u / 1000u;
            host_ms += 16u;
            host_ticks_now = host_ms * 24000u;
            firmware_frame();
            web_draws += host_blits;
        }
    }
    for (uint32_t i = 0; i < n; i++) {
        /* board writes Q15-style codec words (sum*128); /2^20 is device-referred
         * with the mix's real headroom — int16 unity lands at 4.0, the default
         * groove peaks near 0.85. */
        web_l[i] = (float)web_buf[2u * i] / 1048576.0f;
        web_r[i] = (float)web_buf[2u * i + 1u] / 1048576.0f;
    }
}
float *web_out_l(void) { return web_l; }
float *web_out_r(void) { return web_r; }
uint16_t *web_screen(void) { return host_fb; }
uint32_t web_screen_draws(void) { return web_draws; }
uint8_t *web_leds(void) { return fm1_led; }
uint32_t web_button_id(uint32_t label) { return label < 14u ? label : 255u; }
void web_keys(uint32_t mask) { fm1_in.notes = mask & ((1u << 27u) - 1u); }
void web_buttons(uint32_t labels) { fm1_in.buttons = labels & 0x3FFFu; }
void web_enc(uint32_t role, int32_t steps) {
    if (role < 7u) fm1_in.enc_steps[role] += (int16_t)steps;
}
uint32_t web_now_ms(void) { return host_ms; }
uint32_t web_seq_playing(void) { return cp_status(CP_PLAYING); }
uint32_t web_kb_mode(void) { return 0; }
uint32_t web_cmd(uint32_t cmd, uint32_t a, uint32_t b) {
    uint32_t r=cp_command(cmd,a,b);
    if(!r && cmd==9 && a) board_set_user_tempo((uint8_t)a);   /* 9=CP_BPM */
    return r;
}
uint32_t web_board(uint32_t cmd, uint32_t a, uint32_t b) { return board_submit((BoardCommand){cmd, a, b}); }
uint32_t web_bass_style(void) { return cp_bass_style(); }
uint32_t web_score(uint32_t id) { return board_submit((BoardCommand){BOARD_PIANO_CUE, id, 0}); }
uint32_t web_score_count(void) { return piano_count(); }
/* Tekno score titles for the browser selector (ids 29..31, appended to
 * repertoire/board-scores.json). Score tables live in the same piano.inc
 * the device uses — this is UI metadata only. */
uint32_t web_score_title(uint32_t id) {
    static const char *const titles[] = {
        [29] = "TRIBAL TEKNO · Phrygian flute",
        [30] = "MENTAL HYPNOTIC · Dorian b2",
        [31] = "DESERT TRIBE · Egyptian spiral",
    };
    if (id >= 29 && id <= 31 && titles[id]) return (uint32_t)(uintptr_t)titles[id];
    return 0;
}
uint32_t web_tempo(void) { return cp_status(CP_TEMPO); }
uint32_t web_study(void) { return cp_status(CP_STUDY_ID); }
void _initialize(void) {}   /* WASI reactor no-op for the worklet's guarded call */

#ifdef COMPAS_NATIVE_MAIN
#include <stdio.h>
#include <stdlib.h>
static void dump_ppm(const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f) return;
    fprintf(f, "P6\n240 240\n255\n");
    for (uint32_t i = 0; i < 240u * 240u; i++) {
        uint16_t c = host_fb[i];
        uint8_t rgb[3] = {(uint8_t)((c >> 11) * 255u / 31u),
                          (uint8_t)(((c >> 5) & 63u) * 255u / 63u),
                          (uint8_t)((c & 31u) * 255u / 31u)};
        fwrite(rgb, 1, 3, f);
    }
    fclose(f);
}
int main(int argc, char **argv) {
    unsigned seconds = argc > 1 ? (unsigned)atoi(argv[1]) : 5u;
    const char *out = argc > 2 ? argv[2] : "/tmp/fw_full.wav";
    web_boot();
    FILE *f = fopen(out, "wb");
    if (!f) return 2;
    unsigned char hdr[44] = {0};
    unsigned data = seconds * FS * 2;
    hdr[0]='R';hdr[1]='I';hdr[2]='F';hdr[3]='F';
    hdr[4]=(unsigned char)(36+data);hdr[5]=(unsigned char)((36+data)>>8);hdr[6]=(unsigned char)((36+data)>>16);hdr[7]=(unsigned char)((36+data)>>24);
    hdr[8]='W';hdr[9]='A';hdr[10]='V';hdr[11]='E';
    hdr[12]='f';hdr[13]='m';hdr[14]='t';hdr[15]=' ';
    hdr[16]=16; hdr[20]=1; hdr[22]=1;
    hdr[24]=(unsigned char)FS; hdr[25]=(unsigned char)(FS>>8); hdr[26]=(unsigned char)(FS>>16); hdr[27]=(unsigned char)(FS>>24);
    unsigned br=FS*2; hdr[28]=(unsigned char)br; hdr[29]=(unsigned char)(br>>8); hdr[30]=(unsigned char)(br>>16); hdr[31]=(unsigned char)(br>>24);
    hdr[32]=2; hdr[34]=16;
    hdr[36]='d';hdr[37]='a';hdr[38]='t';hdr[39]='a';
    hdr[40]=(unsigned char)data;hdr[41]=(unsigned char)(data>>8);hdr[42]=(unsigned char)(data>>16);hdr[43]=(unsigned char)(data>>24);
    fwrite(hdr, 1, 44, f);
    const unsigned BLK = 1024;
    for (unsigned done = 0; done < data / 2; done += BLK) {
        unsigned n = data / 2 - done > BLK ? BLK : data / 2 - done;
        web_render(n);
        int16_t mono[BLK];
        for (unsigned i = 0; i < n; i++) mono[i] = (int16_t)(web_l[i] * 32767.0f);
        fwrite(mono, 2, n, f);
    }
    fclose(f);
    if (argc > 3) dump_ppm(argv[3]);
    return 0;
}
#endif
