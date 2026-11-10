/* SPDX-License-Identifier: GPL-3.0-only
 * Minimal board shim for the Compás engine in the browser/desktop.
 * Provides exactly the board_* surface engine.c needs, with the real packed
 * bank playback (delta/RLE + choke groups), same as board/board.c's Player.
 */
#include <stdint.h>

/* clip table from the bank build (static in this TU) */
typedef struct { uint32_t offset, frames; } Clip;
#include "727.inc"
#include "727_packed.h"

uint8_t board_roll_mode;
uint8_t board_rhythm_only;

typedef struct { const uint8_t *next; uint32_t remaining; int16_t sample;
                 uint8_t gain; uint8_t sound; uint8_t repeats; } Player;
static Player seq[6];
static const uint8_t group_[15] = {0,0,1,1,2,3,4,5,5,6,6,7,7,8,9};

unsigned board_drums_only(void) { return 0; }
void board_clear727(unsigned role) { seq[role].remaining = 0; }
int board_playing727(unsigned role) { return seq[role].remaining != 0; }

static void choked(unsigned sound) {
    for (unsigned i = 0; i < 6; i++)
        if (seq[i].remaining && group_[seq[i].sound] == group_[sound])
            seq[i].remaining = 0;
}

#include "tekno_kit.h"

int board_note727(unsigned study, unsigned role, unsigned event, unsigned velocity) {
    (void)event;
    /* kit[study][role]: 0 = synthesize, 1..15 = slot+1 */
    unsigned sound = tekno_kit[study][role];
    if (!sound) return 0;
    choked(sound - 1);
    Player *p = &seq[role];
    p->next = compas_packed727 + clips[sound - 1].offset;
    p->sample = 0;
    p->remaining = clips[sound - 1].frames;
    p->gain = (uint8_t)velocity;
    p->sound = (uint8_t)(sound - 1);
    p->repeats = 0;
    return 1;
}

static int32_t next727(Player *p) {
    int32_t sample = p->sample;
    if (p->repeats) --p->repeats;
    else {
        unsigned code = *p->next++;
        if (code >= 128u) sample = (int32_t)((code - 128u) * 256u + *p->next++) - 16384;
        else if (code) sample += (int32_t)code - 64;
        else p->repeats = (uint8_t)(*p->next++ - 1u);
        p->sample = (int16_t)sample;
    }
    --p->remaining;
    return sample;
}

int32_t board_sample727(unsigned role) {
    Player *p = &seq[role];
    if (!p->remaining) return 0;
    return next727(p) * p->gain / 128;
}

/* --- stubs for the rest of the board surface --- */
int32_t board_rack_sample(void) { return 0; }
void board_mix_clip(void) {}
unsigned board_piano_selected; /* external? keep local zero */
int board_piano_frame(int16_t *out) { (void)out; return 0; }
int board_song_boundary(unsigned end) { (void)end; return 0; }
unsigned board_groove_section(unsigned pos, unsigned steps, unsigned section) {
    (void)pos; (void)steps; return section;
}
void board_groove_bar(void) {}
void board_overlay_step(unsigned study, unsigned step) { (void)study; (void)step; }
void board_sequence_step(unsigned pos) { (void)pos; }
unsigned board_roll_keys; void board_roll_trigger(void) {}
void board_bass_note_on(void) {}
uint8_t board_hanan_mode; /* unused */
extern void cp_board_section(unsigned); /* not needed */
