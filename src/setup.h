#ifndef TINGONE_SETUP_H
#define TINGONE_SETUP_H

#define STORE_FILE "/.local/share/ting/log"
#define BUFF_SIZE 2048



// listen for cds attach an Id to the path and start a time.
// TODO: implement cwd observation.
int setup_target_path(const char *target_path,char *store,  int is_active);

#endif /* TINGONE_SETUP_H */