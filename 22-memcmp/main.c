#include <stdint.h>

#include "stdriot.h"

void usage(const char *executable_name) {
    printf("usage :\n");
    printf("%s memcmp\n", executable_name);
    printf("%s strcmp\n", executable_name);
    printf("%s strncmp\n", executable_name);
    printf("%s stat <filename>\n", executable_name);
    /* open and close are  also tested here */
    printf("%s fstat <filename>\n", executable_name);
    printf("%s statvfs <filename>\n", executable_name);
    /* open and close are  also tested here */
    printf("%s fstatvfs <filename>\n", executable_name);
    /* open, close, lseek and readline are also tested here */
    printf("%s rw <filename>\n", executable_name);
    printf("%s normalize_path\n", executable_name);
    printf("%s rename <from_path> <to_path>\n", executable_name);
    /* open, close, lseek, write and read are also tested here */
    printf("%s fsync <filename>\n", executable_name);
    /* open and close are also tested here */
    printf("%s fcntl <filename>\n", executable_name);
    printf("%s mdkir <path>\n", executable_name);
}

static int test_memcmp(int argc, const char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }
    if (memcmp("mem_a", "mem_b", sizeof("mem_a")) >= 0) {
        printf("Error : memcmp(\"mem_a\", \"mem_b\", sizeof(\"mem_a\")) >= 0.\n");
        return 2;
    }
    if (memcmp("mem_b", "mem_a", sizeof("mem_a")) <= 0) {
        printf("Error : memcmp(\"mem_b\", \"mem_a\", sizeof(\"mem_a\")) <= 0.\n");
        return 3;
    }
    if (memcmp("mem_a", "mem_a", sizeof("mem_a")) != 0) {
        printf("Error : memcmp(\"mem_a\", \"mem_a\", sizeof(\"mem_a\")) != 0.\n");
        return 4;
    }
    printf("memcmp test succeeded.\n");
    return 0;
}

static int test_strcmp(int argc, const char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }
    if (strcmp("str_a", "str_b") >= 0) {
        printf("Error : strcmp(\"str_a\", \"str_b\") >= 0.\n");
        return 2;
    }
    if (strcmp("str_b", "str_a") <= 0) {
        printf("Error : strcmp(\"str_b\", \"str_a\") <= 0.\n");
        return 3;
    }
    if (strcmp("str_a", "str_a") != 0) {
        printf("Error : strcmp(\"str_a\", \"str_a\") != 0.\n");
        return 4;
    }
    printf("strcmp test succeeded.\n");
    return 0;
}

static int test_strncmp(int argc, const char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    if (strncmp("str_a", "str_b", sizeof("str_a")) >= 0) {
        printf("Error : strncmp(\"str_a\", \"str_b\", sizeof(\"str_a\")) >= 0.\n");
        return 2;
    }
    if (strncmp("str_a456", "str_b", sizeof("str_a")) >= 0) {
        printf("Error : strncmp(\"str_a456\", \"str_b\", sizeof(\"str_a\")) >= 0.\n");
        return 2;
    }
    if (strncmp("str_a", "str_b123", sizeof("str_a")) >= 0) {
        printf("Error : strncmp(\"str_a\", \"str_b123\", sizeof(\"str_a\")) >= 0.\n");
        return 2;
    }
    if (strncmp("str_a456", "str_b123", sizeof("str_a")) >= 0) {
        printf("Error : strncmp(\"str_a456\", \"str_b123\", sizeof(\"str_a\")) >= 0.\n");
        return 2;
    }

    if (strncmp("str_b", "str_a", sizeof("str_a")) <= 0) {
        printf("Error : strncmp(\"str_b\", \"str_a\", sizeof(\"str_a\")) <= 0.\n");
        return 3;
    }
    if (strncmp("str_b123", "str_a", sizeof("str_a")) <= 0) {
        printf("Error : strncmp(\"str_b123\", \"str_a\", sizeof(\"str_a\")) <= 0.\n");
        return 3;
    }
    if (strncmp("str_b", "str_a456", sizeof("str_a")) <= 0) {
        printf("Error : strncmp(\"str_b\", \"str_a456\", sizeof(\"str_a\")) <= 0.\n");
        return 3;
    }
    if (strncmp("str_b456", "str_a123", sizeof("str_a")) <= 0) {
        printf("Error : strncmp(\"str_b456\", \"str_a123\", sizeof(\"str_a\")) <= 0.\n");
        return 3;
    }

    if (strncmp("str_a", "str_a", sizeof("str_a") - 1) != 0) {
        printf("Error : strncmp(\"str_a\", \"str_a\", sizeof(\"str_a\") - 1) != 0.\n");
        return 4;
    }
    if (strncmp("str_a", "str_a123", sizeof("str_a") - 1) != 0) {
        printf("Error : strncmp(\"str_a\", \"str_a123\", sizeof(\"str_a\") - 1) != 0.\n");
        return 4;
    }
    if (strncmp("str_a123", "str_a", sizeof("str_a") - 1) != 0) {
        printf("Error : strncmp(\"str_a123\", \"str_a\", sizeof(\"str_a\") - 1) != 0.\n");
        return 4;
    }
    if (strncmp("str_a123", "str_a456", sizeof("str_a") - 1) != 0) {
        printf("Error : strncmp(\"str_a123\", \"str_a456\", sizeof(\"str_a\") - 1) != 0.\n");
        return 4;
    }
    printf("strncmp test succeeded.\n");
    return 0;
}

typedef int (*cmd_callback_t)(int argc, const char *argv[]);
typedef struct cmd_s {
    const char *name;
    const cmd_callback_t callback;
} cmd_t;

static const cmd_t cmds[] = {
    { .name = "memcmp", .callback = test_memcmp },
    { .name = "strcmp", .callback = test_strcmp },
    { .name = "strncmp", .callback = test_strncmp },
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
