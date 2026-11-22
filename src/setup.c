#include "setup.h"
#include "store.h"
#include "log.h"
#include <stddef.h>
#if __has_include("color.h")
//#include "color.h"
#else
static inline const char *color_reset(void) { return ""; }
static inline const char *color_green(void) { return ""; }
static inline const char *color_red(void) { return ""; }
static inline const char *color_cyan(void) { return ""; }
static inline const char *color_yellow(void) { return ""; }
#endif

#include <unistd.h>
#include <fcntl.h>
//#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>


int look_up_storage_for_active_path(int fd, const char *target_path);
void arrange();


int 
setup_target_path(const char *target_path,char *rumbu, int is_active)
 {
        // make dir for saka store
        // is active is set to true on cd 
        // increment freq by one 
        //mkdir(store, O_CREAT);
        int fd = open(rumbu, O_RDWR, 0644); // read and write now 
        LOG_INFO("STORE_FILE:[%s] created/open with fd: %i\n", rumbu, fd);
        if (fd < 0) 
        {
                LOG_ERROR("unable to created/open with fd: %i\n", fd);
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
 
        LOG_INFO("path: %s\n", target_path);
        
        // TODO implement the following function 
        // increase freq by 1
        // frequency(&m);
 
        return fd;
 }


// maybe this should just be where I prepare the rumbu/storage log file
// read 
// arrange in a DSA  
//
int
look_up_storage_for_active_path(int fd, const char *target_path)
{
    if (target_path == NULL)
    {
        return -1;
    }

    read_fd_to_string(fd, buffer, BUFF_SIZE);
    // even after read_fd_to_string is complete and popped off the program stack 
    // buffer is still populated with the values read;    
    LOG_INFO("in_buffer_values: %s", buffer);

    //
    // find target_path in the current buffer and split into tokens
    // update the target to active 
    // start the intial time 
    return fd;
}

void 
arrange()
{

}
