#include <stdio.h>
#include <string.h>
#include "setup.h"


int main(int argc, char **argv)
{
	if (argc > 2 && strcmp(argv[1], "cd") == 0)
	{
		// call stats to take the path 
		// argv[2]
		// i need to know when to stop the watch 
		// if another cd is made I need to stop any current timer 
		// because it means I am out of the current target_path
		int fd = setup_target_path(argv[2], 1);
		if (fd < 0)
	       	{
			fprintf(stderr, "unsuccessfully opened and started log files argc %i\n", argc);
			return fd;
		}
		fprintf(stdout, "successfully opened and started log files\n");
	}

	return 0;
}
