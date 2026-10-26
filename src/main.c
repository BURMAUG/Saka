#include <stdio.h>
#include <string.h>
#include "setup.h"

int
main(int argc, char **argv)
{
    if (argc > 2 && strcmp(argv[1], "cd") == 0) {
        int rc = setup_target_path(argv[2], 1);
        if (rc != 0) {
            fprintf(stderr, "unsuccessfully opened and started log files argc %i\n", argc);
            return rc;
        }
        fprintf(stdout, "successfully opened and started log files\n");
    }

    return 0;
}