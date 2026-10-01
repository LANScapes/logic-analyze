/* Standalone regression harness; no analyzer is accessed.
 * cc -Ilibsigrok4DSL -Icommon $(pkg-config --cflags glib-2.0) tools/dslcap/test_spool.c
 *    $(pkg-config --libs glib-2.0) -Wl,-dead_strip -o /tmp/dslfix/16/test_spool
 * /tmp/dslfix/16/test_spool /tmp/dslfix/16/raw /tmp/dslfix/16/output.bin
 */
#define main dslcap_main
#include "dslcap.c"
#undef main
#include <assert.h>

int main(int argc, char **argv)
{
    assert(argc == 3);
    const int channels[] = {1, 2, 16, 32};
    const uint64_t frames = 5003;
    for (size_t test = 0; test < G_N_ELEMENTS(channels); test++) {
        int nch = channels[test];
        size_t bytes = frames * nch * 8;
        uint64_t *raw = g_malloc(bytes + 7);
        for (uint64_t k = 0; k < frames; k++)
            for (int c = 0; c < nch; c++) raw[k * nch + c] = (k << 16) | c;
        memset((char *)raw + bytes, 0xFF, 7); /* Incomplete last word. */
        g_raw = fopen(argv[1], "w+b");
        assert(g_raw);
        g_raw_bytes = 0;
        g_io_error = 0;
        /* Packet boundaries may split words and channel groups. */
        for (size_t pos = 0; pos < bytes + 7;) {
            size_t length = MIN((pos % 4096) + 3, bytes + 7 - pos);
            struct sr_datafeed_logic logic = {.length = length, .format = LA_CROSS_DATA,
                                             .data = (char *)raw + pos};
            struct sr_datafeed_packet packet = {.type = SR_DF_LOGIC, .payload = &logic};
            on_data(NULL, &packet);
            assert(!g_io_error);
            pos += length;
        }
        assert(g_raw_bytes == bytes + 7);
        assert(write_output(argv[2], nch, g_raw_bytes / 8 / nch) == 0);
        FILE *out = fopen(argv[2], "rb");
        assert(out);
        for (int c = 0; c < nch; c++)
            for (uint64_t k = 0; k < frames; k++) {
                uint64_t word;
                assert(fread(&word, 8, 1, out) == 1);
                assert(word == ((k << 16) | c));
            }
        assert(fgetc(out) == EOF);
        fclose(out);

        /* Payload accounting crosses the old GByteArray 32-bit limit. */
        g_raw_bytes = UINT32_MAX - 3ULL;
        struct sr_datafeed_logic logic = {.length = 8, .format = LA_CROSS_DATA, .data = raw};
        struct sr_datafeed_packet packet = {.type = SR_DF_LOGIC, .payload = &logic};
        on_data(NULL, &packet);
        assert(g_raw_bytes == (1ULL << 32) + 4);
        fclose(g_raw);

        /* A spool write failure must stop collection and report failure. */
        g_raw = fopen(argv[1], "rb");
        assert(g_raw);
        g_done = 0;
        on_data(NULL, &packet);
        assert(g_io_error && g_done);
        fclose(g_raw);
        g_free(raw);
    }
    unlink(argv[1]);
    unlink(argv[2]);
    puts("spool tests passed: packet boundaries, channel layout, 64-bit count, I/O failure");
    return 0;
}
