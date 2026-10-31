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

    return fd;
}

int setup_target_path(const char *target_path,char *rumbu, int is_active)
 {
        // make dir for saka store
        // is active is set to true on cd 
        // increment freq by one 
        //mkdir(store, O_CREAT);
        int fd = open(rumbu, O_RDONLY | O_CREAT, 0644); // read only for the first lookup
        fprintf(stdout, "STORE_FILE:[%s] created/open with fd: %i\n", rumbu, fd);
        if (fd < 0) 
        {
                fprintf(stderr, "unable to created/open with fd: %i\n", fd);
                return  1;
        }
        //
        //int read =
        look_up_storage_for_active_path(fd, target_path);
        close(fd);
 
        // TODO 
        // only do this if the target_path is not in the log file 
        // or target_path is not active to increase frequency, and duration and is_active
        //fd = open(STORE_FILE, O_WRONLY, 0644); 
 
        // prepare here
        //m.path = target_path;
 
        fprintf(stdout, "path: %s\n", target_path);
        
        // TODO implement the following function 
        // increase freq by 1
        // frequency(&m);
 
        return fd;
 }

