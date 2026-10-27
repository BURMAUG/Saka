#include <stdio.h>
#include <string.h>
#include "setup.h"
#include "color.h"

int
main(int argc, char **argv)
{
    if (argc > 2 && strcmp(argv[1], "cd") == 0) {
        int rc = setup_target_path(argv[2], 1);
        if (rc != 0) {
            fprintf(stderr, "%serror:%s unsuccessfully opened/started log files (argc %i)\n",
                    color_red(), color_reset(), argc);
            return rc;
        }
        fprintf(stdout, "%ssuccessfully opened and started log files%s\n",
                color_green(), color_reset());
        fprintf(stdout, "path: %s%s%s\n", color_cyan(), argv[2], color_reset());
        return 0;
    }

    if (argc > 1 && (strcmp(argv[1], "ls") == 0 ||
                      strcmp(argv[1], "list") == 0 ||
                      strcmp(argv[1], "ds") == 0)) {
        int rc = list_tracked();
        if (rc != 0) {
            fprintf(stderr, "%serror:%s failed to list tracked directories\n",
                    color_red(), color_reset());
            return rc;
        }
        return 0;
    }

    if (argc > 1 && (strcmp(argv[1], "stop") == 0 ||
                      strcmp(argv[1], "flush") == 0)) {
        int rc = stop_tracking();
        if (rc != 0) {
            fprintf(stderr, "%serror:%s failed to finalize tracked durations\n",
                    color_red(), color_reset());
            return rc;
        }
        fprintf(stdout, "%sfinalized tracked durations%s\n",
                color_green(), color_reset());
        return 0;
    }

    return 0;
}