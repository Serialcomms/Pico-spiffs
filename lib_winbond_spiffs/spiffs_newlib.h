
#include "pico/stdlib.h"
#include <fcntl.h>   // mode_t

#define STDIO_HANDLE_STDIN  0
#define STDIO_HANDLE_STDOUT 1
#define STDIO_HANDLE_STDERR 2

static const mode_t REGULAR_FILE_PERMISSIONS_ALL = S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO;

int _isatty(int fd);

int _open(const char *fn, int oflag, ...);
int _read(int handle, char *buffer, int length);
int _write(int handle, char *buffer, int length);
int _close(int fd);

int _fstat(int fd, struct stat *buf);
int _unlink(const char *path);          // delete

off_t _lseek(int fd, off_t pos, int whence);
