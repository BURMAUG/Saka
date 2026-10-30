#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "setup.h"


char* stitch(const char* h, const char* p, char* n)
{
	int i = 0;
	int j = i;

	int l = strlen(h);
	for(i; i < l; ++i){
		n[i] = h[i];
	}
	l = strlen(p);
	for(j; j < l; ++j, ++i)
	{
		fprintf(stdout,"store: %c\n", p[i]);
		n[i] = p[j];
	}

	n[i] = '\0';
	fprintf(stdout,"new_path: %s\n", n);
	return n;
}

int main(int argc, char **argv)
{
	if (argc > 2 && strcmp(argv[1], "cd") == 0)
	{
		// call stats to take the path 
		// argv[2]
		// i need to know when to stop the watch 
		// if another cd is made I need to stop any current timer 
		// because it means I am out of the current target_path
		const char *home = getenv("HOME");
		int t_len = strlen(STORE_FILE);
		int len = strlen(home);

		char *new_store = malloc(len + t_len +1);
		// I need concate home and path
		// stitch path

		stitch(home, STORE_FILE, new_store);
		fprintf(stderr, "new_path: %s\n", new_store);

		int fd = setup_target_path(argv[2], new_store, 1);
		if (fd < 0)
	       	{
			fprintf(stderr, "unsuccessfully opened and started log files argc %i\n", argc);
			return fd;
		}
		free(new_store);
		fprintf(stdout, "successfully opened and started log files\n");
	}

	return 0;
}
