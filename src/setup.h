#ifndef TINGONE_SETUP_H
#define TINGONE_SETUP_H

#include <stddef.h>

#define STORE_FILE "/home/burmo/.local/share/ting/log"
#define BUFF_SIZE 1024
#define MAX_RECORDS 2048
#define MAX_PATH_LEN 4096

/* One tracked directory entry persisted in STORE_FILE. */
typedef struct {
    char path[MAX_PATH_LEN];
    unsigned long frequency;
    unsigned long duration;
    unsigned int is_active;
} store_record_t;

/* Pure helpers (easy to unit test, no file I/O). */
int parse_storage_buffer(const char *buf, size_t len,
                        store_record_t *records, size_t max_records,
                        size_t *out_count);
int lookup_record(const store_record_t *records, size_t count,
                  const char *target_path);

/* File-backed entry points. */
int setup_target_path(const char *target_path, int is_active);
int look_up_storage_for_active_path(int fd, const char *target_path);

#endif /* TINGONE_SETUP_H */