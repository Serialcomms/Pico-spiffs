
#ifndef SPIFFS_NEWLIB_H
#define SPIFFS_NEWLIB_H

#include "pico/stdlib.h"
#include <fcntl.h>   // mode_t

#define STDIO_HANDLE_STDIN  0
#define STDIO_HANDLE_STDOUT 1
#define STDIO_HANDLE_STDERR 2

#define RAM_ONLY_FUNCTION __no_inline_not_in_flash_func

static const mode_t REGULAR_FILE_PERMISSIONS_ALL = S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO;

typedef const struct LFS_FILE_SYSTEM_PROFILE {

    int file_descriptor; 
    int LFS_ERROR_CODE;
    char *LFS_ERROR_TEXT;

} lfs_files_t;

int RAM_ONLY_FUNCTION (_is_a_tty)(int fd);
int RAM_ONLY_FUNCTION (_read_from_tty)(char *buffer, int length);
int RAM_ONLY_FUNCTION (_write_to_tty)(char *buffer, int length);
int RAM_ONLY_FUNCTION (_open)(const char *fn, int oflag, ...);
int RAM_ONLY_FUNCTION (_read)(int handle, char *buffer, int length);
int RAM_ONLY_FUNCTION (_write)(int handle, char *buffer, int length);
int RAM_ONLY_FUNCTION (_fstat)(int fd, struct stat *buf);
int RAM_ONLY_FUNCTION (_lseek)(int fd, off_t pos, int whence);
int RAM_ONLY_FUNCTION (_unlink)(const char *path);      // __unlink = delete
int RAM_ONLY_FUNCTION (_close)(int fd);



#endif