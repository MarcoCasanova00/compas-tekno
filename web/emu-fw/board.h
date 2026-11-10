/* SPDX-License-Identifier: GPL-3.0-only
 * Minimal board.h for building the Compás engine (COMPAS_BOARD) without the
 * FM-1 HAL: only the surface engine.c touches. */
#ifndef TEKNO_BOARD_H
#define TEKNO_BOARD_H
#include <stdint.h>

enum { BASS_ENSEMBLE = 0, BASS_ACID = 1 };
enum { HANAN_OFF = 0, HANAN_SOLO = 1, HANAN_LAYER = 2 };
enum { ROLL_SIXTEENTH = 0, ROLL_EIGHTH = 1, ROLL_TRIPLET = 2, ROLL_THIRTYSECOND = 3 };
enum { ROLL_SIXTEENTH_MASK = 1 << ROLL_SIXTEENTH, ROLL_EIGHTH_MASK = 1 << ROLL_EIGHTH,
       ROLL_TRIPLET_MASK = 1 << ROLL_TRIPLET, ROLL_THIRTYSECOND_MASK = 1 << ROLL_THIRTYSECOND };
enum { KUMBIA_SETTINGS = 4, KUMBIA_CLOCK_MAX = 4, KUMBIA_PITCH = 0, KUMBIA_DECAY = 1,
       KUMBIA_DRIVE = 2, KUMBIA_CLOCK = 3, HANAN_PATTERNS = 8, HANAN_LAYER_BIT = 1 << 6 };
enum { RACK_SOUNDS = 15, RACK_VOICES = 8, SET_SLOTS = 16, BOARD_WORDS = 32 };
enum { BOARD_RHYTHM_PATTERNS = 1, BOARD_GROOVE_PRESETS = 1, BOARD_RHYTHM_BYTES = 32,
       BOARD_RHYTHM_SLOTS_BYTES = 128, BOARD_SET_BYTES = 64 };

extern uint8_t board_piano_selected;
extern uint8_t board_hanan_mode;
extern uint16_t board_roll_keys;
extern uint8_t board_roll_mode;
extern unsigned board_rhythm_only;

unsigned board_drums_only(void);
void board_clear727(unsigned role);
int board_playing727(unsigned role);
int board_note727(unsigned study, unsigned role, unsigned event, unsigned velocity);
int32_t board_sample727(unsigned role);
int32_t board_rack_sample(void);
void board_mix_clip(void);
int board_piano_frame(int16_t *frame);
int board_song_boundary(unsigned end);
unsigned board_groove_section(unsigned pos, unsigned steps, unsigned section);
void board_groove_bar(void);
void board_overlay_step(unsigned study, unsigned step);
void board_sequence_step(unsigned pos);
void board_roll_trigger(void);
void board_bass_note_on(void);

void cp_board_clave(unsigned direction);
void cp_board_root(unsigned root);
void cp_board_section(unsigned section);
void cp_board_start_count(void);
unsigned cp_board_counting(void);
unsigned cp_board_nearest_step(void);
void cp_board_clear_backing(void);
int board_kumbia_restore(const uint8_t settings[KUMBIA_SETTINGS]);
void board_kumbia_export(uint8_t settings[KUMBIA_SETTINGS]);
unsigned board_kumbia_hz(unsigned clock);
#endif
