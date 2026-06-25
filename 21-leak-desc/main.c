#include <stdint.h>

#include "stdriot.h"

void usage(const char *executable_name) {
    printf("usage :\n");
    printf("%s files <directory_path> <nb> [<first_id>]\n", executable_name);
}

static int leak_file(int argc, const char *argv[]) {
    if ((argc != 4) && (argc != 5)) {
        usage(argv[0]);
        return 1;
    }

    long first_id = 0;
    if (argc == 5) {
        first_id = strtol(argv[4], NULL, 10);
        // CANNOT CHECK ERRNO FOR NOW
        if (first_id < 0) {
            return 2;
        }
    }

    long nb = strtol(argv[3], NULL, 10);
    // CANNOT CHECK ERRNO FOR NOW
    if (nb < 0)
        return 3;

    char normalized_path[128];
    char buffer[128];
    for (size_t i = 0; i < (size_t)nb; i++) {
        const size_t id = ((size_t)first_id) + i;
        int res = snprintf(buffer, sizeof(buffer), "%s/file%zu.tst", argv[2], id);
        if ((res < 0) || ((unsigned)res >= sizeof(buffer))) {
            printf("Failed to snprintf \"file%zu.tst\"\n", id);
            return 4 + i;
        }

        if (vfs_normalize_path(normalized_path, buffer, sizeof(normalized_path)) < 0) {
            printf("Failed to normalize path \"%s\"\n", buffer);
            return 4 + i;
        }

        printf("Opening %s...", normalized_path);

        res = vfs_open(normalized_path, O_CREAT, 0);
        if (res < 0) {
            printf("Failed to create \"%s\"\n", normalized_path);
            printf("res = %d\n", res);
            return 4 + i;
        }

        printf("%d.\n", res);
    }

    return 0;
}

typedef int (*cmd_callback_t)(int argc, const char *argv[]);
typedef struct cmd_s {
    const char *name;
    const cmd_callback_t callback;
} cmd_t;

static const cmd_t cmds[] = {
    { .name = "files", .callback = leak_file },
};

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    const size_t cmds_count = sizeof(cmds) / sizeof(cmds[0]);
    for (size_t i = 0; i < cmds_count; ++i) {
        if (strcmp(argv[1], cmds[i].name) == 0) {
            return cmds[i].callback(argc, argv);
        }
    }

    usage(argv[0]);
    return 1;
}
