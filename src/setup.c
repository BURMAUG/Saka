#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "setup.h"
#include "stats.h"


struct map m[2048];
union stats s;

int setup_target_path(const char *target_path, int is_active)
{
	// is active is set to true on cd 
	// increment freq by one 
	int fd = open(STORE_FILE, O_RDONLY, 0644); // read only for the first lookup
	fprintf(stdout, "STORE_FILE:[%s] created/open with fd: %i\n", STORE_FILE, fd);
	if (fd < 0) 
	{
		fprintf(stderr, "unable to created/open with fd: %i\n", fd);
		return -1;
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


// look_up_storage_for_active_path 
// structure of the STORE_FILE should be path, freq, duration*, is_active[0,1]
// 	- I am contemplating on if path: should be a map to the rest of the infomation about the path
// if is_active is 1 then count is not incremented, duration is left as-is
// if is_active is 0 then count is incremented, duration is updated
int look_up_storage_for_active_path(const int fd, const char *target_path)
{
	struct stat sb;
	fstat(fd, &sb);
	fprintf(stdout,"file_size: %d\n", (int)sb.st_size);
	// buffer should be the size of the file itself and nothing arbitrary
	// for now I do not know if that is ok down the line
	char buffer[(int)sb.st_size]; 
	// is active is set to false if the target_path is different from the previous_current active path
	// fetch store file 
	// then look for active cd
	// if different front target_path the set is_active false for previous_current and current stop time
	//
	//
	// NOTE: reading into the buffer take everying in the file and store it in the whole buffer
	int size_read = read(fd, buffer, sizeof(buffer));
	extract_data(buffer);

	fprintf(stdout,"buffer_size: %zu\n", sizeof(buffer));
	fprintf(stdout, "buffer: %s\n\tread: %d\n", buffer, size_read);
	return fd;
}

int 
extract_data(const char* buffer)
{
	// read_line

	//const char *read_buff = buffer; // use read_buff to loop over csv or | to get the data
	char line[23];
	// process data 
	size_t len = strcspn(buffer, "\n");
	memcpy(line, buffer,len);
	line[len] = '\0';
	fprintf(stdout, "line: %s \tlen: %i\n", line, (int)len);
	const char* l = buffer;
	len = len+1;
	l += len;

	while (*l != '\0')
	{
		len = strcspn(l, "\n");
		memcpy(line, buffer,len);
		line[len] = '\0';
		fprintf(stdout, "line: %s \tlen: %i\n", line, (int)len);
		len = len+1;
		l += len;
	}

	//while (*read_buff != '\0')
	//{
		// read line
	//	line = read_buff;
	//	while(*line != '\n')
	//	{
	//		fprintf(stdout, "line: %s\n", line);
	//		line += 1;
	//	}
	//	line += 1;
	//	read_buff = line;
	//}

	return 0;
}
