
#include "pico/stdlib.h"

#include "pico/flash.h"
#include "hardware/flash.h"
#include "pico/bootrom.h"

#include <spiffs.h>
#include <spiffs_extras.h>

spiffs pico_fs;

#define LOG_PAGE_SIZE 256
#define ONBOARD_FLASH_PAGE_SIZE 256U
  
u8_t __aligned(4) spiffs_fds[32*4];
u8_t __aligned(4) spiffs_work_buf[LOG_PAGE_SIZE*4];
u8_t __aligned(4) spiffs_cache_buf[(LOG_PAGE_SIZE+32)*128];

spiffs_config pico_spiffs_config;

void spiffs_list_directory();

void test_posix();

static s32_t pico_spiffs_read(u32_t addr, u32_t size, u8_t *dst) {

  gpio_put(26, 1);

  const void *xip_flash_address = (void *)(XIP_BASE + addr);

  __builtin_memcpy(dst, xip_flash_address, size);
  
  gpio_put(26, 0);
   
  return SPIFFS_OK;
}

static s32_t pico_spiffs_write_part_page(u32_t addr, u32_t size, u8_t *src) {

  uint8_t repage_data_buffer[ONBOARD_FLASH_PAGE_SIZE];

  uint32_t full_page_address = (addr / ONBOARD_FLASH_PAGE_SIZE) * ONBOARD_FLASH_PAGE_SIZE;

  uint8_t part_page_offset = addr % ONBOARD_FLASH_PAGE_SIZE;

  void *repage_buffer = &repage_data_buffer[part_page_offset];

  __builtin_memset(repage_data_buffer, 0xFF, ONBOARD_FLASH_PAGE_SIZE);

  __builtin_memcpy(repage_buffer, src, size);

  flash_range_program(full_page_address, repage_data_buffer, ONBOARD_FLASH_PAGE_SIZE);
  
}

static s32_t pico_spiffs_write(u32_t addr, u32_t size, u8_t *src) {

  // IMPORTANT - Raspberry Pico SDK does *not* allow flash writes to :- 
  // a) non-page (256 byte) aligned addresses 
  // b) partial page lengths (< 256 bytes)
  //
  // As SPIFFS uses both of the above, a full page array is first initialised with values 0xFF
  // It is the updated by the required SPIFFS write (e.g. 4 bytes) data at the required offset
  // The full page array is then written to flash using standard SDK calls. As a consequence 
  // of how NOR flash works, bytes with value 0xFF have no net effect when written to flash.

  gpio_put(27, 1);
 
  uint32_t address_page_boundary = addr % ONBOARD_FLASH_PAGE_SIZE;

  uint32_t saved_interrupts = save_and_disable_interrupts();

  if (address_page_boundary != 0 || size != ONBOARD_FLASH_PAGE_SIZE) {

    pico_spiffs_write_part_page(addr, size, src);

  } else {

    flash_range_program(addr, src, size);

  }

  restore_interrupts(saved_interrupts);

  gpio_put(27, 0);
   
  return SPIFFS_OK;
}

static s32_t pico_spiffs_erase(u32_t addr, u32_t size) {

  gpio_put(28, 1);
  
  uint32_t flash_address = (addr);

  uint32_t saved_interrupts = save_and_disable_interrupts();

  flash_range_erase(addr, size);

  restore_interrupts(saved_interrupts);

  gpio_put(28, 0);
   
  return SPIFFS_OK;
} 

void pico_spiffs_mount_filesystem() {

  pico_spiffs_config.hal_read_f = pico_spiffs_read;
  pico_spiffs_config.hal_write_f = pico_spiffs_write;
  pico_spiffs_config.hal_erase_f = pico_spiffs_erase;

  pico_spiffs_config.phys_size = (1 * 1024 * 1024);
  pico_spiffs_config.phys_addr = 0x0100000; // start address in onboard Pico flash

  pico_spiffs_config.log_page_size = 256;
  pico_spiffs_config.log_block_size = (4*1024);

  pico_spiffs_config.phys_erase_block = (4*1024);

  pico_spiffs_config.fh_ix_offset = 99;  // first file handle issued = 100

  printf("Attempting to mount SPIFFS filesystem ...\n");
    
  int mount_result = SPIFFS_mount(&pico_fs,
        
    &pico_spiffs_config,
    spiffs_work_buf,
    spiffs_fds,
    sizeof(spiffs_fds),
    spiffs_cache_buf,
    sizeof(spiffs_cache_buf),
    0);

    if (mount_result != 0) {

      printf("SPIFFS Mount Failed, result %i = %s\n", mount_result, get_spiffs_error_text_from_value(mount_result));

      printf("Formatting SPIFFS filesystem ...\n");

      int format_result = SPIFFS_format(&pico_fs);

      printf("SPIFFS Formatted, result %i = %s\n", format_result, get_spiffs_error_text_from_value(format_result));

    } else {

      printf("SPIFFS Mounted, result = %s\n", get_spiffs_error_text_from_value(mount_result));

      spiffs_list_directory();
  }
}

void test_posix() {

  char read_buffer[100];

  //SPIFFS_CACHE_STATS

  FILE *posix_file;

  const char posix_text_string[] = "Hello Pico POSIX 3\n";

  const void *posix_text = posix_text_string;

  const size_t posix_text_length = count_of(posix_text_string);
  
  posix_file = fopen("POSIX3.txt", "w+");

  fwrite(posix_text, 1, posix_text_length, posix_file);

  fclose(posix_file);

  posix_file = fopen("POSIX3.txt", "r");

  fread(read_buffer, 1, posix_text_length, posix_file);

  fclose(posix_file);

  printf("SPIFFS Readback Buffer = %s\n", read_buffer);

}

