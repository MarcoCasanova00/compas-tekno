/*
 * Handshake proof: compile the real device ota.c (board/usb/src/ota.c) with
 * stub hooks, feed it the exact SysEx probe the web installer sends
 * (HS_QUERY), and decode the reply with the installer's own parseIdentity
 * algorithm. If it yields "FM-1_710", the installed device will be found by
 * the installer and V15 rollback works through the normal app path.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* ---- static hooks ota.c expects (device: supplied by runtime.inc) ---- */
static uint8_t rx[1024]; static uint32_t rx_len; static int rx_valid;
static uint8_t tx[2048]; static uint32_t tx_len;

static int ota_wire_send(const uint8_t *p, uint32_t n) {
    if (n > sizeof tx) return -1;
    memcpy(tx, p, n); tx_len = n; return 0;
}
static int ota_frame_get(const uint8_t **p, uint32_t *n) {
    if (!rx_valid) return 0;
    *p = rx; *n = rx_len; rx_valid = 0; return 1;
}
static void ota_frame_done(void) {}
static uint32_t ota_now_ms(void) { return 1000; }
static void ota_idle(void) {}
static int ota_erase(uint32_t off) { (void)off; return 0; }
static int ota_prog(uint32_t off, const void *p, uint32_t n) { (void)off; (void)p; (void)n; return 0; }
static int ota_fread(uint32_t off, void *p, uint32_t n) { (void)off; (void)p; (void)n; return 0; }
static void ota_show(uint32_t step, int32_t code) { (void)step; (void)code; }
static void ota_commit(const uint8_t *parm) { (void)parm; }

#define FELUCCA_ID "FM-1_710"
#define FELUCCA_HANDSHAKE_ID "FM-1_710"
#include "../board/usb/src/ota.c"

/* ---- installer-side decode (mirrors install.html parseIdentity) ---- */
static uint32_t unpack7(const uint8_t *s, uint32_t n, uint8_t *out, uint32_t max) {
    uint32_t acc = 0, nb = 0, o = 0;
    for (uint32_t i = 0; i < n; i++) {
        acc |= (uint32_t)(s[i] & 0x7F) << nb;
        nb += 7;
        if (nb >= 8) {
            if (o < max) out[o] = (uint8_t)acc;
            o++;
            acc >>= 8; nb -= 8;
        }
    }
    return o;
}

int main(void) {
    /* HS_QUERY from install.html: F0 00 32 45 00 00 00 40 7F F7.
     * The device collects the inner bytes (between F0/F7) in sx_frame and
     * ota_take() unpacks them; feed the inner bytes directly. */
    static const uint8_t query[] = { 0x00, 0x32, 0x45, 0x00, 0x00, 0x00, 0x40, 0x7F };
    memcpy(rx, query, sizeof query); rx_len = sizeof query; rx_valid = 1;
    tx_len = 0;
    ota_service();                    /* device: answer handshakes */
    if (!tx_len) { printf("FAIL: no reply\n"); return 1; }
    /* decode F0..F7 wire body into logical bytes, check framing like parseIdentity */
    uint8_t body[128]; uint32_t blen = tx_len - 2;
    uint32_t dlen = unpack7(tx + 1, blen, body, sizeof body);
    if (dlen != 34 || body[0] != 0 || body[1] != 0x59 || body[2] != 0x11) {
        printf("FAIL: unexpected reply framing (decoded=%u)\n", dlen);
        return 1;
    }
    char txt[28]; memcpy(txt, body + 6, 27); txt[27] = 0;
    printf("identity reply: \"%s\"\n", txt);
    if (strcmp(txt, "FM-1_710") != 0) {
        printf("FAIL: identity != FM-1_710\n");
        return 1;
    }
    printf("PASS: device answers handshake as FM-1_710 — installer will find it, rollback path open\n");
    return 0;
}
