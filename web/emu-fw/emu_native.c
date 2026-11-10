/* SPDX-License-Identifier: GPL-3.0-only
 * Native render harness: runs the real Compás engine (COMPAS_BOARD + shim)
 * and writes a WAV. Usage: emu_native <study-index> <bpm> <seconds> <out.wav>
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "engine.h"

int main(int argc, char **argv) {
    if (argc < 5) { fprintf(stderr, "usage: %s study bpm seconds out.wav\n", argv[0]); return 2; }
    unsigned study = (unsigned)atoi(argv[1]);
    unsigned bpm = (unsigned)atoi(argv[2]);
    unsigned seconds = (unsigned)atoi(argv[3]);
    const char *out = argv[4];
    const unsigned SR = 44100;
    if (cp_init(SR) != CP_OK) return 3;
    if (cp_command(CP_STUDY, study, 0) != CP_OK) { fprintf(stderr, "study %u bad\n", study); return 4; }
    if (cp_command(CP_BPM, bpm, 0) != CP_OK) return 5;
    cp_command(CP_FEEL, 30, 0);
    cp_command(CP_START, 0, 0);
    FILE *f = fopen(out, "wb");
    if (!f) return 6;
    unsigned char hdr[44] = {0};
    unsigned total = seconds * SR * 2;
    unsigned data = (unsigned)(seconds * SR * 2);
    hdr[0]='R';hdr[1]='I';hdr[2]='F';hdr[3]='F';
    hdr[4]=(unsigned char)(36+data);hdr[5]=(unsigned char)((36+data)>>8);hdr[6]=(unsigned char)((36+data)>>16);hdr[7]=(unsigned char)((36+data)>>24);
    hdr[8]='W';hdr[9]='A';hdr[10]='V';hdr[11]='E';
    hdr[12]='f';hdr[13]='m';hdr[14]='t';hdr[15]=' ';
    hdr[16]=16; hdr[20]=1; hdr[22]=1; /* PCM mono */
    hdr[24]=(unsigned char)SR; hdr[25]=(unsigned char)(SR>>8); hdr[26]=(unsigned char)(SR>>16); hdr[27]=(unsigned char)(SR>>24);
    unsigned br = SR*2; hdr[28]=(unsigned char)br; hdr[29]=(unsigned char)(br>>8); hdr[30]=(unsigned char)(br>>16); hdr[31]=(unsigned char)(br>>24);
    hdr[32]=2; hdr[34]=16;
    hdr[36]='d';hdr[37]='a';hdr[38]='t';hdr[39]='a';
    hdr[40]=(unsigned char)data;hdr[41]=(unsigned char)(data>>8);hdr[42]=(unsigned char)(data>>16);hdr[43]=(unsigned char)(data>>24);
    fwrite(hdr, 1, 44, f);
    const unsigned BLK = 256; /* CP_MAX_BLOCK */
    int16_t *pcm = cp_pcm();
    unsigned remaining = total / 2;
    while (remaining) {
        unsigned n = remaining > BLK ? BLK : remaining;
        if (cp_render(n) != CP_OK) break;
        fwrite(pcm, 2, n, f);
        remaining -= n;
    }
    fclose(f);
    fprintf(stderr, "rendered %u frames\n", total / 2 - remaining);
    return 0;
}
