#include <stdint.h>

#include "stdriot.h"

void usage(const char *executable_name) {
    printf("usage :\n"
           "%s memcmp\n"
           "%s strcmp\n"
           "%s strncmp\n"
           "%s stat <filename>\n"
           "%s fstat <filename>\n"      /* open and close are  also tested here */
           "%s statvfs <filename>\n"
           "%s fstatvfs <filename>\n"   /* open and close are  also tested here */
           "%s rw <filename>\n"         /* open, close, lseek and readline are also tested here */
           "%s normalize_path\n"
           "%s rename <from_path> <to_path>\n"
           "%s fsync <filename>\n"      /* open, close, lseek, write and read are also tested here */
           "%s fcntl <filename>\n"      /* open and close are also tested here */
           "%s mdkir <path>\n",

           executable_name, executable_name, executable_name, executable_name,
           executable_name, executable_name, executable_name, executable_name,
           executable_name, executable_name);
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

static void print_stat(const struct stat *stats) {
    printf("st_dev 0x%lx, st_ino 0x%lx, ", stats->st_dev, stats->st_ino);

    const char *st_mode_label = "Unknown";
    switch (stats->st_mode & S_IFMT) {
        case S_IFBLK:  st_mode_label = "block device";            break;
        case S_IFCHR:  st_mode_label = "character device";        break;
        case S_IFDIR:  st_mode_label = "directory";               break;
        case S_IFIFO:  st_mode_label = "FIFO/pipe";               break;
        case S_IFLNK:  st_mode_label = "symlink";                 break;
        case S_IFREG:  st_mode_label = "regular file";            break;
        case S_IFSOCK: st_mode_label = "socket";                  break;
    }

    printf("st_mode %s, "
            "st_nlink %lu, st_size %ld, st_blksize %ld, st_blocks %ld\n",
            st_mode_label,
            stats->st_nlink,
            stats->st_size,
            stats->st_blksize,
            stats->st_blocks);
}

static int test_stat(int argc, const char *argv[]) {
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    struct stat file_stats;
    if(stat(argv[2], &file_stats) < 0) {
        printf("%s %s %s failed.\n", argv[0], argv[1], argv[2]);
        return 2;
    }
    print_stat(&file_stats);
    return 0;
}

static int test_fstat(int argc, const char *argv[]) {
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    int fd = open(argv[2], O_RDONLY, 0);
    if (fd < 0) {
        printf("%s %s %s failed to open file.\n", argv[0], argv[1], argv[2]);
        return 2;
    }

    struct stat file_stats;
    if (fstat(fd, &file_stats) < 0) {
        printf("%s %s %s failed.\n", argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 3;
    }

    if (close(fd) < 0) {
        printf("%s %s %s failed to close file descriptor.\n", argv[0], argv[1], argv[2]);
        return 4;
    }

    print_stat(&file_stats);
    return 0;
}

static void print_statvfs(const struct statvfs *stats) {
    printf("f_bsize %lu, f_frsize %lu, f_blocks %lu, f_bfree %lu, f_bavail %lu, "
           "f_files %lu, f_ffree %lu, f_favail %lu, f_fsid %lu, f_flag %lu, f_namemax %lu.\n",
           stats->f_bsize, stats->f_frsize, (uint32_t)stats->f_blocks, (uint32_t)stats->f_bfree, (uint32_t)stats->f_bavail,
           stats->f_files, stats->f_ffree, stats->f_favail, stats->f_fsid, stats->f_flag,
           stats->f_namemax
    );
}

static int test_statvfs(int argc, const char *argv[]) {
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    struct statvfs stats;
    if (statvfs(argv[2], &stats) < 0) {
        printf("%s %s %s failed.\n", argv[0], argv[1], argv[2]);
        return 2;
    }

    print_statvfs(&stats);
    return 0;
}

static int test_fstatvfs(int argc, const char *argv[]) {
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    int fd = open(argv[2], O_RDONLY, 0);
    if (fd < 0) {
        printf("%s %s %s failed to open file.\n", argv[0], argv[1], argv[2]);
        return 2;
    }

    struct statvfs stats;
    if (fstatvfs(fd, &stats) < 0) {
        printf("%s %s %s failed.\n", argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 3;
    }

    if (close(fd) < 0) {
        printf("%s %s %s failed to close file descriptor.\n", argv[0], argv[1], argv[2]);
        return 4;
    }

    print_statvfs(&stats);
    return 0;
}

static int test_rw(int argc, const char *argv[]) {
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    int fd = open(argv[2], O_CREAT | O_RDWR, 0);
    if (fd < 0) {
        printf("%s %s %s failed to open file.\n", argv[0], argv[1], argv[2]);
        return 2;
    }

    const char bytes[] = { 0xC0, 0xCA, 0xC0, 0x1A };
    if (write(fd, bytes, sizeof(bytes)) < 0) {
        printf("%s %s %s failed to write bytes.\n", argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 3;
    }

    const char string[] = "foobarbaz\n";
    if (write(fd, string, sizeof(string) - 1) < 0) {
        printf("%s %s %s failed to write string.\n", argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 4;
    }

    if (lseek(fd, sizeof(bytes), SEEK_SET) < 0) {
        printf("%s %s %s failed to seek to the start of \"%s\".\n",
               argv[0], argv[1], argv[2], string);
        (void)close(fd);
        return 5;
    }

    const char f00[] = "f00";
    if (write(fd, f00, sizeof(f00) - 1) < 0) {
        printf("%s %s %s failed to write \"f00\".\n", argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 6;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        printf("%s %s %s failed to seek to the start of the file.\n",
               argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 7;
    }

    char buffer[16];
    if (read(fd, buffer, sizeof(bytes)) < 0) {
        printf("%s %s %s failed to read bytes.\n", argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 8;
    }

    if (memcmp(bytes, buffer, sizeof(bytes)) != 0) {
        printf("%s %s %s failed because read bytes are different from expected ones.\n",
               argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 9;
    }

    const char f00barbaz[] = "f00barbaz\n";
    if (readline(fd, buffer, sizeof(buffer)) < 0) {
        printf("%s %s %s failed to readline.\n", argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 10;
    }

    /*
     * f00barbaz = { f, 0, 0, b, a, r, b, a, z,\n,\0 }
     * sizeof(f00barbaz) = 11.
     *
     * buffer = { f, 0, 0, b, a, r, b, a, z, \0 }
     *
     * comparison pattern is { f, 0, 0, b, a, r, b, a, z },
     * hence sizeof(f00barbaz) - 2 character to compare.
     */
    if (strncmp(f00barbaz, buffer, sizeof(f00barbaz) - 2) != 0) {
        printf("%s %s %s failed because read string is different from expected one.\n",
               argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 11;
    }

    if (close(fd) < 0) {
        printf("%s %s %s failed to close file descriptor.\n", argv[0], argv[1], argv[2]);
        return 12;
    }

    printf("read/write/seek/readline test succeeded.\n");
    return 0;
}

static int test_normalize_path(int argc, const char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

typedef struct path_test_case_s {
    const char *path_to_normalize;
    const char *expected_normalized_path;
} path_test_case_t;

    /* Taken from RIOT's normalize path tests. */
    const path_test_case_t cases[] = {
        { .path_to_normalize = "/this/is/a/test", .expected_normalized_path = "/this/is/a/test" },
        { .path_to_normalize = "///////////////////////////////", .expected_normalized_path = "/" },
        { .path_to_normalize = "/abc/./def/././zxcv././.", .expected_normalized_path = "/abc/def/zxcv." },
        { .path_to_normalize = "/abc/../def", .expected_normalized_path = "/def" },
        { .path_to_normalize = "/mydir/", .expected_normalized_path = "/mydir/" },
        { .path_to_normalize = "/12345/6789/..", .expected_normalized_path = "/12345" },
        { .path_to_normalize = "./", .expected_normalized_path = "/" },
        { .path_to_normalize = "", .expected_normalized_path = "" },
    };
    const size_t cases_count = sizeof(cases) / sizeof(cases[0]);
    char buffer[128];
    for (size_t i = 0; i < cases_count; i++) {
        if (normalize_path(buffer, cases[i].path_to_normalize, sizeof(buffer)) < 0) {
            printf("%s %s failed : \"%s\".\n", argv[0], argv[1], cases[i].path_to_normalize);
            return 2;
        }

        if (strcmp(buffer, cases[i].expected_normalized_path) != 0) {
            printf("%s %s failed because normalized path is different from expected one : "
                   "\"%s\" VS \"%s\".\n",
                   argv[0], argv[1], buffer, cases[i].expected_normalized_path);
            return 3;
        }
    }

    printf("normalize path test succeeded.\n");
    return 0;
}

static int test_rename(int argc, const char *argv[]) {
    if (argc != 4) {
        usage(argv[0]);
        return 1;
    }

    if (rename(argv[2], argv[3]) < 0) {
        printf("%s %s %s %s failed.\n", argv[0], argv[1], argv[2], argv[3]);
        return 3;
    }

    printf("rename test succeeded.\n");
    return 0;
}

static int test_fsync(int argc, const char *argv[]) {
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }
    int fd = open(argv[2], O_CREAT | O_RDWR, 0);
    if (fd < 0) {
        printf("%s %s %s failed to open file.\n", argv[0], argv[1], argv[2]);
        return 2;
    }

    const char pattern[] = "0123456789ABCDEF";
    const size_t pattern_size = sizeof(pattern);
    if (write(fd, pattern, pattern_size) < 0) {
        printf("%s %s %s failed to write pattern.\n", argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 3;
    }

    if (fsync(fd) < 0) {
        printf("%s %s %s failed.\n", argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 4;
    }

    struct stat stats;
    if (fstat(fd, &stats) < 0) {
        printf("%s %s %s failed to stat.\n", argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 5;
    }

    if (stats.st_size != pattern_size) {
        printf("%s %s %s failed because file size is different from expected one : %lu VS %zu.\n",
               argv[0], argv[1], argv[2], stats.st_size, pattern_size);
        (void)close(fd);
        return 6;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        printf("%s %s %s failed to seek to the start of the file.\n",
               argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 7;
    }

    char buffer[pattern_size];
    if (read(fd, buffer, sizeof(buffer)) < 0) {
        printf("%s %s %s failed to read bytes.\n", argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 8;
    }

    if (memcmp(buffer, pattern, pattern_size) != 0) {
        printf("%s %s %s failed because read bytes are different from pattern.\n",
               argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 9;
    }

    if (close(fd) < 0) {
        printf("%s %s %s failed to close file descriptor.\n", argv[0], argv[1], argv[2]);
        return 10;
    }

    printf("fsync test succeeded.\n");
    return 0;
}

static int test_fcntl(int argc, const char *argv[]) {
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    const int flags_setting = O_CREAT | O_RDWR;
    int fd = open(argv[2], flags_setting, 0);
    if (fd < 0) {
        printf("%s %s %s failed to open file.\n", argv[0], argv[1], argv[2]);
        return 2;
    }

    int res = fcntl(fd, F_GETFL);
    if (res < 0) {
        printf("%s %s %s failed.\n", argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 2;
    }

    if (res != flags_setting) {
        printf("%s %s %s failed because flags are different from setting.\n", argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 3;
    }

    if (close(fd) < 0) {
        printf("%s %s %s failed to close file descriptor.\n", argv[0], argv[1], argv[2]);
        return 4;
    }

    printf("fcntl test succeeded.\n");
    return 0;
}

static int test_mkdir(int argc, const char *argv[]) {
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    if (mkdir(argv[2], 0) < 0) {
        printf("%s %s %s failed.\n", argv[0], argv[1], argv[2]);
    }

    printf("mkdir test succeeded.\n");
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
    { .name = "stat", .callback = test_stat },
    { .name = "fstat", .callback = test_fstat },
    { .name = "statvfs", .callback = test_statvfs },
    { .name = "fstatvfs", .callback = test_fstatvfs },
    { .name = "rw", .callback = test_rw },
    { .name = "normalize_path", .callback = test_normalize_path },
    { .name = "rename", .callback = test_rename },
    { .name = "fsync", .callback = test_fsync },
    { .name = "fcntl", .callback = test_fcntl },
    { .name = "mkdir", .callback = test_mkdir },
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
