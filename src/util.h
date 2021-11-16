#ifndef SIST2_UTIL_H
#define SIST2_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "third-party/utf8.h/utf8.h"
#include "libscan/scan.h"

#define MD5_STR_LENGTH 33


char *abspath(const char *path);

char *expandpath(const char *path);

dyn_buffer_t url_escape(char *str);

extern int PrintingProgressBar;

void progress_bar_print(double percentage, size_t tn_size, size_t index_size);

GHashTable *incremental_get_table();


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
static int md5_digest_is_null(const unsigned char digest[MD5_DIGEST_LENGTH]) {
    return (*(int64_t *) digest) == 0 && (*((int64_t *) digest + 1)) == 0;
}


__always_inline
static void incremental_put(GHashTable *table, const unsigned char path_md5[MD5_DIGEST_LENGTH], int mtime) {
    char *ptr = malloc(MD5_STR_LENGTH);
    buf2hex(path_md5, MD5_DIGEST_LENGTH, ptr);
    g_hash_table_insert(table, ptr, GINT_TO_POINTER(mtime));
}

__always_inline
static void incremental_put_str(GHashTable *table, const char *path_md5, int mtime) {
    char *ptr = malloc(MD5_STR_LENGTH);
    strcpy(ptr, path_md5);
    g_hash_table_insert(table, ptr, GINT_TO_POINTER(mtime));
}

__always_inline
static int incremental_get(GHashTable *table, const unsigned char path_md5[MD5_DIGEST_LENGTH]) {
    if (table != NULL) {
        char md5_str[MD5_STR_LENGTH];
        buf2hex(path_md5, MD5_DIGEST_LENGTH, md5_str);
        return GPOINTER_TO_INT(g_hash_table_lookup(table, md5_str));
    } else {
        return 0;
    }
}

__always_inline
static int incremental_get_str(GHashTable *table, const char *path_md5) {
    if (table != NULL) {
        return GPOINTER_TO_INT(g_hash_table_lookup(table, path_md5));
    } else {
        return 0;
    }
}

__always_inline
static int incremental_mark_file_for_copy(GHashTable *table, const unsigned char path_md5[MD5_DIGEST_LENGTH]) {
    char *ptr = malloc(MD5_STR_LENGTH);
    buf2hex(path_md5, MD5_DIGEST_LENGTH, ptr);
    return g_hash_table_insert(table, ptr, GINT_TO_POINTER(1));
}

#endif
