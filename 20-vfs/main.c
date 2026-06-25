#include <stdint.h>

#include "stdriot.h"

void usage(int argc, const char *argv[]) {

    printf("Invalid command line :\n\t");
    const char *separator = "";
    for (int i =  0; i < argc; i++) {
        printf("%s%s", separator, argv[0]);
        separator = " ";
    }

    printf("\nusage :\n");

    const char *executable_name = argv[0];

    printf("%s stat <filename>\n", executable_name);
    printf("%s vfs_stat <filename>\n", executable_name);

    /* open and close are  also tested here */
    printf("%s fstat <filename>\n", executable_name);
    /* vfs_open and vfs_close are  also tested here */
    printf("%s vfs_fstat <filename>\n", executable_name);

    printf("%s statvfs <filename>\n", executable_name);
    printf("%s vfs_statvfs <filename>\n", executable_name);

    /* open and close are  also tested here */
    printf("%s fstatvfs <filename>\n", executable_name);
    /* vfs_open and vfs_close are  also tested here */
    printf("%s vfs_fstatvfs <filename>\n", executable_name);

    /* open, close, lseek and readline are also tested here */
    printf("%s rw <filename>\n", executable_name);
    /* vfs_open, vfs_close, vfs_lseek and vfs_readline are also tested here */
    printf("%s rw <filename>\n", executable_name);

    printf("%s vfs_normalize_path\n", executable_name);

    printf("%s rename <from_path> <to_path>\n", executable_name);
    printf("%s vfs_rename <from_path> <to_path>\n", executable_name);

    /* open, close, lseek, write and read are also tested here */
    printf("%s fsync <filename>\n", executable_name);
    /* vfs_open, vfs_close, vfs_lseek, vfs_write and vfs_read are also tested here */
    printf("%s vfs_fsync <filename>\n", executable_name);

    /* open and close are also tested here */
    printf("%s fcntl <filename>\n", executable_name);
    /* vfs_open and vfs_close are also tested here */
    printf("%s vfs_fcntl <filename>\n", executable_name);

    printf("%s mdkir <path>\n", executable_name);
    printf("%s vfs_mdkir <path>\n", executable_name);
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

typedef int (*stat_func_t)(const char *restrict path, struct stat *restrict buf);
static int test_stat_base(int argc, const char *argv[],stat_func_t stat_func) {
    if (argc != 3) {
        usage(argc, argv);
        return 1;
    }

    struct stat file_stats;
    if(stat_func(argv[2], &file_stats) < 0) {
        printf("%s %s %s failed.\n", argv[0], argv[1], argv[2]);
        return 2;
    }
    print_stat(&file_stats);
    printf("%s %s %s succeeded.\n", argv[0], argv[1], argv[2]);
    return 0;
}

static int test_stat(int argc, const char *argv[]) {
    return test_stat_base(argc, argv, stat);
}

static int test_vfs_stat(int argc, const char *argv[]) {
    return test_stat_base(argc, argv, vfs_stat);
}

typedef int (*open_func_t)(const char *name, int flags, mode_t mode);
typedef int (*close_func_t)(int fd);
typedef int (*fstat_func_t)(int fd, struct stat *buf);

static int libc_open_wrapper(const char *name, int flags, mode_t mode) {
    return open(name, flags, mode);
}

static int test_fstat_base(int argc, const char *argv[],
                           open_func_t  open_func,
                           fstat_func_t fstat_func,
                           close_func_t  close_func) {
    if (argc != 3) {
        usage(argc, argv);
        return 1;
    }

    int fd = open_func(argv[2], O_RDONLY, 0);
    if (fd < 0) {
        printf("%s %s %s failed to open file.\n", argv[0], argv[1], argv[2]);
        return 2;
    }

    struct stat file_stats;
    if (fstat_func(fd, &file_stats) < 0) {
        printf("%s %s %s failed.\n", argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 3;
    }

    if (close_func(fd) < 0) {
        printf("%s %s %s failed to close file descriptor.\n", argv[0], argv[1], argv[2]);
        return 4;
    }

    print_stat(&file_stats);
    printf("%s %s %s succeeded.\n", argv[0], argv[1], argv[2]);
    return 0;
}

static int test_fstat(int argc, const char *argv[]) {
    return test_fstat_base(argc, argv, libc_open_wrapper, fstat, close);
}

static int test_vfs_fstat(int argc, const char *argv[]) {
    return test_fstat_base(argc, argv, vfs_open, vfs_fstat, vfs_close);
}

static void print_statvfs(const struct statvfs *stats) {
    printf("f_bsize %lu, f_frsize %lu, f_blocks %lu, f_bfree %lu, f_bavail %lu, "
           "f_files %lu, f_ffree %lu, f_favail %lu, f_fsid %lu, f_flag %lu, f_namemax %lu.\n",
           stats->f_bsize, stats->f_frsize, (uint32_t)stats->f_blocks, (uint32_t)stats->f_bfree, (uint32_t)stats->f_bavail,
           stats->f_files, stats->f_ffree, stats->f_favail, stats->f_fsid, stats->f_flag,
           stats->f_namemax
    );
}

typedef int (*statvfs_func_t)(const char *restrict path, struct statvfs *restrict buf);
static int test_statvfs_base(int argc, const char *argv[], statvfs_func_t statvfs_func) {
    if (argc != 3) {
        usage(argc, argv);
        return 1;
    }

    struct statvfs stats;
    if (statvfs_func(argv[2], &stats) < 0) {
        printf("%s %s %s failed.\n", argv[0], argv[1], argv[2]);
        return 2;
    }

    print_statvfs(&stats);
    printf("%s %s %s succeeded.\n", argv[0], argv[1], argv[2]);
    return 0;
}

static int test_statvfs(int argc, const char *argv[]) {
    return test_statvfs_base(argc, argv, statvfs);
}

static int test_vfs_statvfs(int argc, const char *argv[]) {
    return test_statvfs_base(argc, argv, vfs_statvfs);
}

typedef int (*fstatvfs_func_t)(int fd, struct statvfs *buf);
static int test_fstatvfs_base(int argc, const char *argv[],
                              open_func_t open_func,
                              fstatvfs_func_t fstatvfs_func,
                              close_func_t close_func) {
    if (argc != 3) {
        usage(argc, argv);
        return 1;
    }

    int fd = open_func(argv[2], O_RDONLY, 0);
    if (fd < 0) {
        printf("%s %s %s failed to open file.\n", argv[0], argv[1], argv[2]);
        return 2;
    }

    struct statvfs stats;
    if (fstatvfs_func(fd, &stats) < 0) {
        printf("%s %s %s failed.\n", argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 3;
    }

    if (close_func(fd) < 0) {
        printf("%s %s %s failed to close file descriptor.\n", argv[0], argv[1], argv[2]);
        return 4;
    }

    print_statvfs(&stats);
    printf("%s %s %s succeeded.\n", argv[0], argv[1], argv[2]);
    return 0;
}

static int test_fstatvfs(int argc, const char *argv[]) {
    return test_fstatvfs_base(argc, argv, libc_open_wrapper, fstatvfs, close);
}

static int test_vfs_fstatvfs(int argc, const char *argv[]) {
    return test_fstatvfs_base(argc, argv, vfs_open, vfs_fstatvfs, vfs_close);
}

typedef off_t (*lseek_func_t)(int fd, off_t off, int whence);
typedef ssize_t (*write_func_t)(int fd, const void *src, size_t count);
typedef ssize_t (*read_func_t)(int fd, void *dest, size_t count);
static int test_rw_base(int argc, const char *argv[],
                        open_func_t open_func,
                        write_func_t write_func,
                        read_func_t read_func,
                        lseek_func_t lseek_func,
                        close_func_t close_func) {
    if (argc != 3) {
        usage(argc, argv);
        return 1;
    }

    int fd = open_func(argv[2], O_CREAT | O_RDWR, 0);
    if (fd < 0) {
        printf("%s %s %s failed to open file.\n", argv[0], argv[1], argv[2]);
        return 2;
    }

    const char bytes[] = { 0xC0, 0xCA, 0xC0, 0x1A };
    if (write_func(fd, bytes, sizeof(bytes)) < 0) {
        printf("%s %s %s failed to write bytes.\n", argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 3;
    }

    const char string[] = "foobarbaz\n";
    if (write_func(fd, string, sizeof(string) - 1) < 0) {
        printf("%s %s %s failed to write string.\n", argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 4;
    }

    if (lseek_func(fd, sizeof(bytes), SEEK_SET) < 0) {
        printf("%s %s %s failed to seek to the start of \"%s\".\n",
               argv[0], argv[1], argv[2], string);
        (void)close_func(fd);
        return 5;
    }

    const char f00[] = "f00";
    if (write_func(fd, f00, sizeof(f00) - 1) < 0) {
        printf("%s %s %s failed to write \"f00\".\n", argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 6;
    }

    if (lseek_func(fd, 0, SEEK_SET) < 0) {
        printf("%s %s %s failed to seek to the start of the file.\n",
               argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 7;
    }

    char buffer[16];
    if (read_func(fd, buffer, sizeof(bytes)) < 0) {
        printf("%s %s %s failed to read bytes.\n", argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 8;
    }

    if (memcmp(bytes, buffer, sizeof(bytes)) != 0) {
        printf("%s %s %s failed because read bytes are different from expected ones.\n",
               argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 9;
    }

    const char f00barbaz[] = "f00barbaz\n";
    /*
     * WARNING WE USE VFS_READLINE FOR BOTH LIBC/VFS.
     */
    if (vfs_readline(fd, buffer, sizeof(buffer)) < 0) {
        printf("%s %s %s failed to readline.\n", argv[0], argv[1], argv[2]);
        (void)close_func(fd);
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
        (void)close_func(fd);
        return 11;
    }

    if (close_func(fd) < 0) {
        printf("%s %s %s failed to close file descriptor.\n", argv[0], argv[1], argv[2]);
        return 12;
    }

    printf("%s %s %s succeeded (read/write/seek/readline).\n", argv[0], argv[1], argv[2]);
    return 0;
}

static int test_rw(int argc, const char *argv[]) {
    return test_rw_base(argc, argv,
                        libc_open_wrapper,
                        write, read, lseek,
                        close);
}

static int test_vfs_rw(int argc, const char *argv[]) {
    return test_rw_base(argc, argv,
                        vfs_open,
                        vfs_write, vfs_read, vfs_lseek,
                        vfs_close);
}

static int test_vfs_normalize_path(int argc, const char *argv[]) {
    if (argc != 2) {
        usage(argc, argv);
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
        if (vfs_normalize_path(buffer, cases[i].path_to_normalize, sizeof(buffer)) < 0) {
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

    printf("%s %s succeeded.\n", argv[0], argv[1]);
    return 0;
}

typedef int (*rename_func_t)(const char *from_path, const char *to_path);
static int test_rename_base(int argc, const char *argv[],
                            open_func_t open_func,
                            rename_func_t rename_func,
                            close_func_t close_func) {
    if (argc != 4) {
        usage(argc, argv);
        return 1;
    }

    int ret = open_func(argv[2], O_CREAT, 0);
    if (ret < 0) {
        printf("%s %s %s %s failed to open file.\n", argv[0], argv[1], argv[2], argv[3]);
        return 2;
    }


    if (close_func(ret) < 0) {
        printf("%s %s %s %s failed to close file.\n", argv[0], argv[1], argv[2], argv[3]);
        return 3;
    }

    if (rename_func(argv[2], argv[3]) < 0) {
        printf("%s %s %s %s failed.\n", argv[0], argv[1], argv[2], argv[3]);
        return 4;
    }

    printf("%s %s %s %s succeeded.\n", argv[0], argv[1], argv[2], argv[3]);
    return 0;
}

static int test_rename(int argc, const char *argv[]) {
    return test_rename_base(argc, argv, libc_open_wrapper, rename, close);
}

static int test_vfs_rename(int argc, const char *argv[]) {
    return test_rename_base(argc, argv, vfs_open, vfs_rename, vfs_close);
}

typedef int (*fsync_func_t)(int fd);
static int test_fsync_base(int argc, const char *argv[],
                           open_func_t open_func,
                           write_func_t write_func,
                           read_func_t read_func,
                           lseek_func_t lseek_func,
                           fstat_func_t fstat_func,
                           fsync_func_t fsync_func,
                           close_func_t close_func) {
    if (argc != 3) {
        usage(argc, argv);
        return 1;
    }
    int fd = open_func(argv[2], O_CREAT | O_RDWR, 0);
    if (fd < 0) {
        printf("%s %s %s failed to open file.\n", argv[0], argv[1], argv[2]);
        return 2;
    }

    const char pattern[] = "0123456789ABCDEF";
    const size_t pattern_size = sizeof(pattern);
    if (write_func(fd, pattern, pattern_size) < 0) {
        printf("%s %s %s failed to write pattern.\n", argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 3;
    }

    if (fsync_func(fd) < 0) {
        printf("%s %s %s failed.\n", argv[0], argv[1], argv[2]);
        (void)close(fd);
        return 4;
    }

    struct stat stats;
    if (fstat_func(fd, &stats) < 0) {
        printf("%s %s %s failed to stat.\n", argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 5;
    }

    if (stats.st_size != pattern_size) {
        printf("%s %s %s failed because file size is different from expected one : %lu VS %zu.\n",
               argv[0], argv[1], argv[2], stats.st_size, pattern_size);
        (void)close_func(fd);
        return 6;
    }

    if (lseek_func(fd, 0, SEEK_SET) < 0) {
        printf("%s %s %s failed to seek to the start of the file.\n",
               argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 7;
    }

    char buffer[pattern_size];
    if (read_func(fd, buffer, sizeof(buffer)) < 0) {
        printf("%s %s %s failed to read bytes.\n", argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 8;
    }

    if (memcmp(buffer, pattern, pattern_size) != 0) {
        printf("%s %s %s failed because read bytes are different from pattern.\n",
               argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 9;
    }

    if (close_func(fd) < 0) {
        printf("%s %s %s failed to close file descriptor.\n", argv[0], argv[1], argv[2]);
        return 10;
    }

    printf("%s %s %s succeeded.\n", argv[0], argv[1], argv[2]);
    return 0;
}

static int test_fsync(int argc, const char *argv[]) {
    return test_fsync_base(argc, argv,
                           libc_open_wrapper,
                           write, read, lseek, fstat,
                           fsync,
                           close);
}

static int test_vfs_fsync(int argc, const char *argv[]) {
    return test_fsync_base(argc, argv,
                           vfs_open,
                           vfs_write, vfs_read, vfs_lseek, vfs_fstat,
                           vfs_fsync,
                           vfs_close);
}

typedef int (*fcntl_func_t)(int fd, int cmd, int arg);
static int libc_fcntl_wrapper(int fd, int cmd, int arg) {
    return fcntl(fd, cmd, arg);
}

static int test_fcntl_base(int argc, const char *argv[],
                           open_func_t open_func,
                           fcntl_func_t fcntl_func,
                           close_func_t close_func) {
    if (argc != 3) {
        usage(argc, argv);
        return 1;
    }

    const int flags_setting = O_CREAT | O_RDWR;
    int fd = open_func(argv[2], flags_setting, 0);
    if (fd < 0) {
        printf("%s %s %s failed to open file.\n", argv[0], argv[1], argv[2]);
        return 2;
    }

    int res = fcntl_func(fd, F_GETFL, 0);
    if (res < 0) {
        printf("%s %s %s failed.\n", argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 2;
    }

    if (res != flags_setting) {
        printf("%s %s %s failed because flags are different from setting.\n", argv[0], argv[1], argv[2]);
        (void)close_func(fd);
        return 3;
    }

    if (close_func(fd) < 0) {
        printf("%s %s %s failed to close file descriptor.\n", argv[0], argv[1], argv[2]);
        return 4;
    }

    printf("%s %s %s succeeded.\n", argv[0], argv[1], argv[2]);
    return 0;
}

static int test_fcntl(int argc, const char *argv[]) {
    return test_fcntl_base(argc, argv,
                           libc_open_wrapper, libc_fcntl_wrapper, close);
}

static int test_vfs_fcntl(int argc, const char *argv[]) {
    return test_fcntl_base(argc, argv,
                           vfs_open, vfs_fcntl, vfs_close);
}

typedef int (*mkdir_func_t)(const char *name, mode_t mode);
static int test_mkdir_base(int argc, const char *argv[], mkdir_func_t mkdir_func) {
    if (argc != 3) {
        usage(argc, argv);
        return 1;
    }

    if (mkdir_func(argv[2], 0) < 0) {
        printf("%s %s %s failed.\n", argv[0], argv[1], argv[2]);
        return 2;
    }

    printf("%s %s %s succeeded.\n", argv[0], argv[1], argv[2]);
    return 0;
}

static int test_mkdir(int argc, const char *argv[]) {
    return test_mkdir_base(argc, argv, mkdir);
}

static int test_vfs_mkdir(int argc, const char *argv[]) {
    return test_mkdir_base(argc, argv, vfs_mkdir);
}

typedef int (*cmd_callback_t)(int argc, const char *argv[]);
typedef struct cmd_s {
    const char *name;
    const cmd_callback_t callback;
} cmd_t;

static const cmd_t cmds[] = {
    { .name = "stat", .callback = test_stat },
    { .name = "vfs_stat", .callback = test_vfs_stat },

    { .name = "fstat", .callback = test_fstat },
    { .name = "vfs_fstat", .callback = test_vfs_fstat },

    { .name = "statvfs", .callback = test_statvfs },
    { .name = "vfs_statvfs", .callback = test_vfs_statvfs },

    { .name = "fstatvfs", .callback = test_fstatvfs },
    { .name = "vfs_fstatvfs", .callback = test_vfs_fstatvfs },

    { .name = "rw", .callback = test_rw },
    { .name = "vfs_rw", .callback = test_vfs_rw },

    { .name = "vfs_normalize_path", .callback = test_vfs_normalize_path },

    { .name = "rename", .callback = test_rename },
    { .name = "vfs_rename", .callback = test_vfs_rename },

    { .name = "fsync", .callback = test_fsync },
    { .name = "vfs_fsync", .callback = test_vfs_fsync },

    { .name = "fcntl", .callback = test_fcntl },
    { .name = "vfs_fcntl", .callback = test_vfs_fcntl },

    { .name = "mkdir", .callback = test_mkdir },
    { .name = "vfs_mkdir", .callback = test_vfs_mkdir },
};

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        usage(argc, argv);
        return 1;
    }

    const size_t cmds_count = sizeof(cmds) / sizeof(cmds[0]);
    for (size_t i = 0; i < cmds_count; ++i) {
        if (strcmp(argv[1], cmds[i].name) == 0) {
            return cmds[i].callback(argc, argv);
        }
    }

    usage(argc, argv);
    return 1;
}
