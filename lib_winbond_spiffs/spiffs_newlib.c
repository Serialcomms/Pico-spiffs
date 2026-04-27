#include <stdio.h>
#include "pico/stdio.h"
#include "pico/stdlib.h"

#include <fcntl.h>   // open()
#include <unistd.h>  // close()
#include <errno.h>   // errno
#include <sys/stat.h>

#include "spiffs_newlib.h"

#include <spiffs.h>

extern spiffs pico_fs;

spiffs_flags fopen_flags_to_spiffs_open_flags(int fopen_flags);

int _open(const char *file_name, int oflag, ...) {

    spiffs_flags spiffs_open_flags = fopen_flags_to_spiffs_open_flags(oflag);

    spiffs_file open_file = SPIFFS_open(&pico_fs, file_name, spiffs_open_flags, 0);

    return (int) open_file;
   
}

off_t _lseek(int fd, off_t pos, int whence) {

    return (off_t) SPIFFS_lseek(&pico_fs, fd, pos, whence);
}

int _read(int handle, char *buffer, int length) {

    return (int) SPIFFS_read(&pico_fs, handle, buffer, length);
}

int _write(int handle, char *buffer, int length) {

    return (int) SPIFFS_write(&pico_fs, handle, buffer, length);

}

int _close(int fd) {

    return (int) SPIFFS_close(&pico_fs,fd);

}

int _fstat(int fd, struct stat *buf) {

    static spiffs_stat s;

    int stat_result = SPIFFS_fstat(&pico_fs, fd, &s);

    buf->st_size = s.size;
    buf->st_ino = 1234 + s.obj_id;

    buf->st_nlink = 1;
    buf->st_mode = REGULAR_FILE_PERMISSIONS_ALL;

    return stat_result;

}

int _unlink(const char *path) {

    return (int) SPIFFS_remove(&pico_fs, path);
  
}

int _isatty(int fd) {

   // no need to change this for now?

    return fd == STDIO_HANDLE_STDIN || fd == STDIO_HANDLE_STDOUT || fd == STDIO_HANDLE_STDERR;
}

spiffs_flags fopen_flags_to_spiffs_open_flags(int fopen_flags) {

    // https://pubs.opengroup.org/onlinepubs/7908799/xsh/open.html

    spiffs_flags spiffs_open_flags = 0;

    bool read_only =  (fopen_flags & (O_RDWR | O_WRONLY) == O_RDONLY);
    bool write_only = (fopen_flags & (O_RDWR | O_WRONLY) == O_WRONLY);

    if (read_only) {

        spiffs_open_flags |= SPIFFS_O_RDONLY;
  
    } else if (write_only) {
            
        spiffs_open_flags |= SPIFFS_O_WRONLY;

    } else {

        spiffs_open_flags |= SPIFFS_O_RDWR;
    }

    if (fopen_flags & O_EXCL)   spiffs_open_flags |= SPIFFS_O_EXCL;
    if (fopen_flags & O_CREAT)  spiffs_open_flags |= SPIFFS_O_CREAT;
    if (fopen_flags & O_TRUNC)  spiffs_open_flags |= SPIFFS_O_TRUNC;
    if (fopen_flags & O_APPEND) spiffs_open_flags |= SPIFFS_O_APPEND; 
  
    return spiffs_open_flags;

}


/*

// Pico SDK weak functions which are replace by the ones above

// https://github.com/raspberrypi/pico-sdk/blob/master/src/rp2_common/pico_clib_interface/newlib_interface.c

int __attribute__((weak)) _read(int handle, char *buffer, int length) {
#if LIB_PICO_STDIO
    if (handle == STDIO_HANDLE_STDIN) {
        return stdio_get_until(buffer, length, at_the_end_of_time);
    }
#endif
    return -1;
}

int __attribute__((weak)) _write(int handle, char *buffer, int length) {
#if LIB_PICO_STDIO
    if (handle == STDIO_HANDLE_STDOUT || handle == STDIO_HANDLE_STDERR) {
        stdio_put_string(buffer, length, false, true);
        return length;
    }
#endif
    return -1;
}

int __attribute__((weak)) _open(__unused const char *fn, __unused int oflag, ...) {
    return -1;
}

int __attribute__((weak)) _close(__unused int fd) {
    return -1;
}

off_t __attribute__((weak)) _lseek(__unused int fd, __unused off_t pos, __unused int whence) {
    return -1;
}

int __attribute__((weak)) _fstat(__unused int fd, __unused struct stat *buf) {
    return -1;
}

int __attribute__((weak)) _isatty(int fd) {
    return fd == STDIO_HANDLE_STDIN || fd == STDIO_HANDLE_STDOUT || fd == STDIO_HANDLE_STDERR;
}

*/
