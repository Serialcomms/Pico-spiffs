#include <stdio.h>
#include "pico/stdio.h"
#include "pico/stdio_uart.h"
#include "pico/status_led.h"
#include "pico/stdlib.h"

#include <string.h>
#include <fcntl.h>   // open()
#include <unistd.h>  // close()
#include <errno.h>   // errno
#include <sys/stat.h>

#include "spiffs_newlib.h"
#include "spiffs_extras.h"

#include <spiffs.h>

extern spiffs pico_fs;

extern void dma_printf(const char *format, ...);

spiffs_flags  RAM_ONLY_FUNCTION (fopen_flags_to_spiffs_open_flags)(int fopen_flags);

int RAM_ONLY_FUNCTION (_open)(const char *file_name, int oflag, ...) {

    SPIFFS_clearerr(&pico_fs);

    spiffs_flags spiffs_open_flags = fopen_flags_to_spiffs_open_flags(oflag);

    spiffs_file open_file = SPIFFS_open(&pico_fs, file_name, spiffs_open_flags, 0);

    s32_t spiffs_error = SPIFFS_errno(&pico_fs);

    errno = get_posix_error_number_from_spiffs(spiffs_error);

    //dma_printf("spiffs _open, handle=%i, spiffs=%i, error=%i\r\n", open_file, spiffs_error, errno);

    return (int) open_file;
   
}

off_t RAM_ONLY_FUNCTION (_lseek)(int fd, off_t pos, int whence) {

    SPIFFS_clearerr(&pico_fs);

    s32_t seek_result = SPIFFS_lseek(&pico_fs, fd, pos, whence);

    s32_t spiffs_error = SPIFFS_errno(&pico_fs);

    errno = get_posix_error_number_from_spiffs(spiffs_error);

    // dma_printf("spiffs _lseek, result=%i, spiffs=%i, error=%i\r\n", seek_result, spiffs_error, errno);

    return (int) seek_result;

}

int RAM_ONLY_FUNCTION (_read)(int handle, char *buffer, int length) {

    SPIFFS_clearerr(&pico_fs);

    s32_t read_result = SPIFFS_read(&pico_fs, handle, buffer, length);

    s32_t spiffs_error = SPIFFS_errno(&pico_fs);

    errno = get_posix_error_number_from_spiffs(spiffs_error);

    dma_printf("spiffs _read, result=%i, spiffs=%i, error=%i\r\n", read_result, spiffs_error, errno);

    return (int) read_result;
}

int RAM_ONLY_FUNCTION (_write)(int handle, char *buffer, int length) {

    SPIFFS_clearerr(&pico_fs);

    s32_t write_result = SPIFFS_write(&pico_fs, handle, buffer, length);

    s32_t spiffs_error = SPIFFS_errno(&pico_fs);

    errno = get_posix_error_number_from_spiffs(spiffs_error);

    dma_printf("spiffs _write, result=%i, spiffs=%i, error=%i\r\n", write_result, spiffs_error, errno);

    return (int) write_result;

}

int RAM_ONLY_FUNCTION (_close)(int fd) {

    SPIFFS_clearerr(&pico_fs);

    s32_t close_result = SPIFFS_close(&pico_fs,fd);

    s32_t spiffs_error = SPIFFS_errno(&pico_fs);

    errno = get_posix_error_number_from_spiffs(spiffs_error);

    dma_printf("spiffs _close, result=%i, spiffs=%i, error=%i\r\n", close_result, spiffs_error, errno);

    return (int) close_result;

}

int RAM_ONLY_FUNCTION (_fstat)(int fd, struct stat *buf) {

    static spiffs_stat s;

    SPIFFS_clearerr(&pico_fs);
    
    int fstat_result = SPIFFS_fstat(&pico_fs, fd, &s);

    s32_t spiffs_error = SPIFFS_errno(&pico_fs);

    errno = get_posix_error_number_from_spiffs(spiffs_error);

    dma_printf("spiffs _fstat, result=%i, spiffs=%i, error=%i\r\n", fstat_result, spiffs_error, errno);

    if (spiffs_error == 0) {

        buf->st_size = s.size;
        buf->st_ino = 1234 + s.obj_id;

        buf->st_nlink = 1;
        buf->st_mode = REGULAR_FILE_PERMISSIONS_ALL;

    } else {

        buf->st_size = 0;
        buf->st_ino = 0;

        buf->st_nlink = 0;
        buf->st_mode = 0;

    }

    return fstat_result;

}

int RAM_ONLY_FUNCTION (_unlink)(const char *path) {

    SPIFFS_clearerr(&pico_fs);

    s32_t remove_result = SPIFFS_remove(&pico_fs, path);

    s32_t spiffs_error = SPIFFS_errno(&pico_fs);

    errno = get_posix_error_number_from_spiffs(spiffs_error);

    dma_printf("spiffs unlink, result=%i, spiffs=%i, error=%i\r\n", remove_result, spiffs_error, errno);

    return remove_result;
  
}

int RAM_ONLY_FUNCTION (_isatty)(int fd) {

    SPIFFS_clearerr(&pico_fs);

    s32_t spiffs_error = SPIFFS_errno(&pico_fs);

    errno = get_posix_error_number_from_spiffs(spiffs_error);

    return fd == STDIO_HANDLE_STDIN || fd == STDIO_HANDLE_STDOUT || fd == STDIO_HANDLE_STDERR;
}

spiffs_flags RAM_ONLY_FUNCTION (fopen_flags_to_spiffs_open_flags)(int fopen_flags) {

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
