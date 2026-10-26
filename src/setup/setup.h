#ifndef TINGONE_SETUP_H
#define TINGONE_SETUP_H

#define STORE_FILE "/home/burmo/.local/share/ting/log"
#define BUFF_SIZE 2048



// listen for cds attach an Id to the path and start a time.
// TODO: implement cwd observation.
int setup_target_path(const char *target_path, int is_active);


int extract_data(const char* buffer);

char* read_line();
// after reading the line split the comma seperated values or "|" seperated values
int get_path(const int fd);
int get_duration(const int fd);
int get_frequency(const int fd);//count
int is_active(const int fd);
// after reading the line split the comma seperated values or "|" seperated values

int load_data(const int fd);


int look_up_storage_for_active_path(const int fd, const char *target_path);

#endif
