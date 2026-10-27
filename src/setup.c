#include "setup.h"
#include "store.h"
#if __has_include("color.h")
#include "color.h"
#else
static inline const char *color_reset(void) { return ""; }
static inline const char *color_green(void) { return ""; }
static inline const char *color_red(void) { return ""; }
static inline const char *color_cyan(void) { return ""; }
static inline const char *color_yellow(void) { return ""; }
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

static const char *
store_file_path(void)
{
    const char *p = getenv("TING_STORE_FILE");
    return (p != NULL && p[0] != '\0') ? p : STORE_FILE;
}

static int
ensure_parent_dir(const char *file_path)
{
    char tmp[MAX_PATH_LEN];
    size_t n = strlen(file_path);
    if (n >= sizeof(tmp))
        return -1;
    memcpy(tmp, file_path, n + 1);

    /* Strip the file name to get the directory. */
    char *slash = strrchr(tmp, '/');
    if (slash == NULL)
        return 0;
    *slash = '\0';
    if (tmp[0] == '\0')
        return 0;

    /* mkdir -p (only one level is needed for our default path, but be safe). */
    char acc[MAX_PATH_LEN];
    acc[0] = '\0';
    char *tok = strtok(tmp, "/");
    int first = 1;
    while (tok != NULL) {
        if (first) {
            /* Absolute path keeps leading '/'. */
            if (file_path[0] == '/')
                snprintf(acc, sizeof(acc), "/%s", tok);
            else
                snprintf(acc, sizeof(acc), "%s", tok);
            first = 0;
        } else {
            size_t cur = strlen(acc);
            snprintf(acc + cur, sizeof(acc) - cur, "/%s", tok);
        }
        if (mkdir(acc, 0755) != 0 && errno != EEXIST)
            return -1;
        tok = strtok(NULL, "/");
    }
    return 0;
}

int
parse_storage_buffer(const char *buf, size_t len,
                    store_record_t *records, size_t max_records,
                    size_t *out_count)
{
    if (buf == NULL || records == NULL || out_count == NULL)
        return -1;

    size_t count = 0;
    size_t i = 0;
    while (i < len && count < max_records) {
        if (buf[i] == '\n') {
            i++;
            continue;
        }
        size_t start = i;
        while (i < len && buf[i] != '\n')
            i++;
        size_t line_len = i - start;
        if (i < len)
            i++; /* consume '\n' */

        if (line_len == 0)
            continue;

        char line[MAX_PATH_LEN];
        size_t copy = line_len < sizeof(line) - 1 ? line_len : sizeof(line) - 1;
        memcpy(line, buf + start, copy);
        line[copy] = '\0';

        /* Strip trailing '\r' (CRLF). */
        size_t ll = strlen(line);
        if (ll > 0 && line[ll - 1] == '\r')
            line[ll - 1] = '\0';

        /* Parse trailing numeric fields from the right so paths may contain
         * commas. Supports 4 fields (legacy) or 5 fields (with started_at). */
        size_t seps[4];
        int nsep = 0;
        for (char *q = line + strlen(line) - 1; q >= line && nsep < 4; q--) {
            if (*q == ',') {
                seps[nsep++] = (size_t)(q - line);
            }
        }

        unsigned long frequency = 0;
        unsigned long duration = 0;
        unsigned int is_active = 0;
        unsigned long started_at = 0;

        int use_new = 0;
        if (nsep >= 4) {
            char *freq_field = line + seps[3] + 1;
            char *endptr = NULL;
            errno = 0;
            strtoul(freq_field, &endptr, 10);
            if (endptr == line + seps[2] && endptr != freq_field && errno == 0)
                use_new = 1;
        }

        if (use_new) {
            /* New format: path,freq,duration,is_active,started_at */
            char *s0 = line + seps[0];
            char *s1 = line + seps[1];
            char *s2 = line + seps[2];
            char *s3 = line + seps[3];
            *s0 = '\0'; *s1 = '\0'; *s2 = '\0'; *s3 = '\0';
            started_at = strtoul(s0 + 1, NULL, 10);
            is_active = (unsigned int)strtoul(s1 + 1, NULL, 10);
            duration = strtoul(s2 + 1, NULL, 10);
            frequency = strtoul(s3 + 1, NULL, 10);
        } else if (nsep >= 3) {
            /* Legacy format: path,freq,duration,is_active */
            char *s0 = line + seps[0];
            char *s1 = line + seps[1];
            char *s2 = line + seps[2];
            *s0 = '\0'; *s1 = '\0'; *s2 = '\0';
            is_active = (unsigned int)strtoul(s0 + 1, NULL, 10);
            duration = strtoul(s1 + 1, NULL, 10);
            frequency = strtoul(s2 + 1, NULL, 10);
        } else {
            continue;
        }

        const char *path = line;
        if (path[0] == '\0')
            continue;

        snprintf(records[count].path, MAX_PATH_LEN, "%s", path);
        records[count].frequency = frequency;
        records[count].duration = duration;
        records[count].is_active = is_active;
        records[count].started_at = started_at;
        count++;
    }

    *out_count = count;
    return 0;
}

int
lookup_record(const store_record_t *records, size_t count,
              const char *target_path)
{
    if (records == NULL || target_path == NULL)
        return -1;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(records[i].path, target_path) == 0)
            return (int)i;
    }
    return -1;
}

void
finalize_record(store_record_t *record, unsigned long now)
{
    if (record == NULL)
        return;
    if (record->is_active && record->started_at > 0 && now >= record->started_at) {
        record->duration += now - record->started_at;
    }
    record->is_active = 0;
    record->started_at = 0;
}

int
stop_tracking(void)
{
    const char *file = store_file_path();
    int fd = open(file, O_RDONLY);
    if (fd < 0) {
        if (errno == ENOENT)
            return 0;
        return -1;
    }

    char buffer[BUFF_SIZE];
    memset(buffer, 0, sizeof(buffer));
    size_t total = 0;
    while (total + 1 < BUFF_SIZE) {
        ssize_t r = read(fd, buffer + total, BUFF_SIZE - total - 1);
        if (r < 0) {
            close(fd);
            return -1;
        }
        if (r == 0)
            break;
        total += (size_t)r;
    }
    buffer[total] = '\0';
    close(fd);

    static store_record_t records[MAX_RECORDS];
    size_t count = 0;
    memset(records, 0, sizeof(records));
    if (parse_storage_buffer(buffer, total, records, MAX_RECORDS, &count) != 0)
        return -1;

    unsigned long now = (unsigned long)time(NULL);
    int changed = 0;
    for (size_t i = 0; i < count; i++) {
        if (records[i].is_active) {
            finalize_record(&records[i], now);
            changed = 1;
        }
    }

    if (!changed)
        return 0;

    fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        return -1;
    for (size_t i = 0; i < count; i++) {
        char line[MAX_PATH_LEN + 64];
        snprintf(line, sizeof(line), "%s,%lu,%lu,%u,%lu\n",
                 records[i].path, records[i].frequency,
                 records[i].duration, records[i].is_active,
                 records[i].started_at);
        if (write_string_fd(fd, line) != 0) {
            close(fd);
            return -1;
        }
    }
    close(fd);
    return 0;
}

static int
compare_records(const void *a, const void *b)
{
    const store_record_t *ra = a;
    const store_record_t *rb = b;
    if (rb->frequency != ra->frequency)
        return (rb->frequency > ra->frequency) ? 1 : -1;
    if (rb->duration != ra->duration)
        return (rb->duration > ra->duration) ? 1 : -1;
    return strcmp(ra->path, rb->path);
}

int
list_tracked(void)
{
    const char *file = store_file_path();
    int fd = open(file, O_RDONLY);
    if (fd < 0) {
        if (errno == ENOENT) {
            fprintf(stdout, "no tracked directories yet\n");
            return 0;
        }
        return -1;
    }

    static store_record_t records[MAX_RECORDS];
    memset(records, 0, sizeof(records));
    char buffer[BUFF_SIZE];
    memset(buffer, 0, sizeof(buffer));
    size_t total = 0;
    while (total + 1 < BUFF_SIZE) {
        ssize_t r = read(fd, buffer + total, BUFF_SIZE - total - 1);
        if (r < 0) {
            close(fd);
            return -1;
        }
        if (r == 0)
            break;
        total += (size_t)r;
    }
    buffer[total] = '\0';
    close(fd);

    size_t count = 0;
    if (parse_storage_buffer(buffer, total, records, MAX_RECORDS, &count) != 0)
        return -1;

    if (count == 0) {
        fprintf(stdout, "no tracked directories yet\n");
        return 0;
    }

    qsort(records, count, sizeof(records[0]), compare_records);

    fprintf(stdout, "%-4s %-40s %10s %10s %s\n",
            "#", "path", "freq", "duration", "active");
    for (size_t i = 0; i < count; i++) {
        fprintf(stdout, "%-4zu %s%-40s%s %10lu %10lu %s\n",
                i + 1,
                color_cyan(), records[i].path, color_reset(),
                records[i].frequency, records[i].duration,
                records[i].is_active ? "yes" : "");
    }
    return 0;
}

int
look_up_storage_for_active_path(int fd, const char *target_path)
{
    if (target_path == NULL)
        return -1;

    char buffer[BUFF_SIZE];
    memset(buffer, 0, sizeof(buffer));

    ssize_t n = 0;
    if (fd >= 0) {
        /* Read up to BUFF_SIZE-1 bytes. */
        size_t total = 0;
        while (total + 1 < BUFF_SIZE) {
            ssize_t r = read(fd, buffer + total, BUFF_SIZE - total - 1);
            if (r < 0)
                return -1;
            if (r == 0)
                break;
            total += (size_t)r;
        }
        buffer[total] = '\0';
        n = (ssize_t)total;
    }

    store_record_t records[MAX_RECORDS];
    size_t count = 0;
    if (parse_storage_buffer(buffer, (size_t)n, records, MAX_RECORDS, &count) != 0)
        return -1;

    int idx = lookup_record(records, count, target_path);
    (void)idx;

    fprintf(stdout, "buffer: %s\n\tread: %zd\n", buffer, n);
    return fd;
}

int
setup_target_path(const char *target_path, int is_active)
{
    if (target_path == NULL)
        return -1;

    const char *file = store_file_path();
    if (ensure_parent_dir(file) != 0)
        return -1;

    static store_record_t records[MAX_RECORDS];
    size_t count = 0;
    memset(records, 0, sizeof(records));

    int fd = open(file, O_RDONLY);
    if (fd >= 0) {
        char buffer[BUFF_SIZE];
        memset(buffer, 0, sizeof(buffer));
        size_t total = 0;
        while (total + 1 < BUFF_SIZE) {
            ssize_t r = read(fd, buffer + total, BUFF_SIZE - total - 1);
            if (r < 0) {
                close(fd);
                return -1;
            }
            if (r == 0)
                break;
            total += (size_t)r;
        }
        buffer[total] = '\0';
        close(fd);
        if (parse_storage_buffer(buffer, total, records, MAX_RECORDS, &count) != 0)
            return -1;
    } else if (errno != ENOENT) {
        return -1;
    }

    unsigned long now = (unsigned long)time(NULL);

    /* Finalize every currently active directory before switching. */
    for (size_t i = 0; i < count; i++) {
        if (records[i].is_active)
            finalize_record(&records[i], now);
    }

    int idx = lookup_record(records, count, target_path);
    if (idx < 0) {
        if (count >= MAX_RECORDS)
            return -1;
        snprintf(records[count].path, MAX_PATH_LEN, "%s", target_path);
        records[count].frequency = 1;
        records[count].duration = 0;
        records[count].is_active = is_active ? 1 : 0;
        records[count].started_at = is_active ? now : 0;
        count++;
        idx = (int)count - 1;
    } else {
        if (!records[idx].is_active)
            records[idx].frequency += 1;
        records[idx].is_active = is_active ? 1 : 0;
        records[idx].started_at = is_active ? now : 0;
    }

    /* Write records back. */
    fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        return -1;

    for (size_t i = 0; i < count; i++) {
        char line[MAX_PATH_LEN + 64];
        snprintf(line, sizeof(line), "%s,%lu,%lu,%u,%lu\n",
                 records[i].path, records[i].frequency,
                 records[i].duration, records[i].is_active,
                 records[i].started_at);
        if (write_string_fd(fd, line) != 0) {
            close(fd);
            return -1;
        }
    }
    close(fd);

    fprintf(stdout, "path: %s%s%s\n", color_cyan(), target_path, color_reset());
    return 0;
}