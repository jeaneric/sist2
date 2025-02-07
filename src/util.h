#ifndef SIST2_UTIL_H
#define SIST2_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "third-party/utf8.h/utf8.h"
#include "libscan/scan.h"


char *abspath(const char *path);

char *expandpath(const char *path);

dyn_buffer_t url_escape(char *str);

extern int PrintingProgressBar;

void progress_bar_print_json(size_t done, size_t count,  size_t tn_size, size_t index_size, int waiting);
void progress_bar_print(double percentage, size_t tn_size, size_t index_size);

const char *find_file_in_paths(const char **paths, const char *filename);


void str_escape(char *dst, const char *str);

void str_unescape(char *dst, const char *str);

static int hex2buf(const char *str, int len, unsigned char *bytes) {
    static const uint8_t hashmap[] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    for (int pos = 0; pos < len; pos += 2) {
        int idx0 = (uint8_t) str[pos + 0];
        int idx1 = (uint8_t) str[pos + 1];
        bytes[pos / 2] = (uint8_t) (hashmap[idx0] << 4) | hashmap[idx1];
    }
    return TRUE;
}

__always_inline
static void buf2hex(const unsigned char *buf, size_t buflen, char *hex_string) {
    static const char hexdig[] = "0123456789abcdef";

    const unsigned char *p;
    size_t i;

    char *s = hex_string;
    for (i = 0, p = buf; i < buflen; i++, p++) {
        *s++ = hexdig[(*p >> 4) & 0x0f];
        *s++ = hexdig[*p & 0x0f];
    }
    *s = '\0';
}


__always_inline
static void generate_doc_id(const char *rel_path, char *doc_id) {
    unsigned char md[MD5_DIGEST_LENGTH];

    MD5((unsigned char *) rel_path, strlen(rel_path), md);
    buf2hex(md, sizeof(md), doc_id);
}

#define MILLISECOND 1000

struct timespec timespec_add(struct timespec ts1, long usec);

#define TIMER_INIT() struct timespec timer_begin
#define TIMER_START() clock_gettime(CLOCK_REALTIME, &timer_begin)
#define TIMER_END(x) do { \
    struct timespec timer_end;                   \
    clock_gettime(CLOCK_REALTIME, &timer_end);   \
    x = (timer_end.tv_sec - timer_begin.tv_sec) * 1000000 + (timer_end.tv_nsec - timer_begin.tv_nsec) / 1000; \
} while (0)

#define pthread_cond_timedwait_ms(cond, mutex, delay_ms) do {\
        struct timespec now; \
        clock_gettime(CLOCK_REALTIME, &now); \
        struct timespec end_time = timespec_add(now, MILLISECOND * delay_ms); \
        pthread_cond_timedwait(cond, mutex, &end_time); \
    } while (0)

#define array_foreach(arr) \
    for (int i = 0; (arr)[i] != NULL; i++)

#endif
