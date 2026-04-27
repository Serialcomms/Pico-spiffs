
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/flash.h"
#include "hardware/flash.h"

#include "hardware/xip_cache.h"

#include <spiffs.h>
#include <spiffs_pico.h>
#include <spiffs_extras.h>

spiffs pico_fs;

#define LOG_PAGE_SIZE 256
#define ONBOARD_FLASH_PAGE_SIZE 256U
  
u8_t __aligned(4) spiffs_fds[32*4];
u8_t __aligned(4) spiffs_work_buf[LOG_PAGE_SIZE*4];
u8_t __aligned(4) spiffs_cache_buf[(LOG_PAGE_SIZE+32)*32];

spiffs_config pico_spiffs_config;

void spiffs_list_directory();

void test_posix();

static s32_t RAM_ONLY_FUNCTION(pico_spiffs_read)(u32_t addr, u32_t size, u8_t *dst) {

  #ifdef PICO_GPIO_SPIFFS_ACTIVITY_LED_READ  
  
  gpio_put(PICO_GPIO_SPIFFS_ACTIVITY_LED_READ, 1);

  #endif

  const void *xip_flash_address = (void *)(XIP_BASE + addr);

  __builtin_memcpy(dst, xip_flash_address, size);
  
  #ifdef PICO_GPIO_SPIFFS_ACTIVITY_LED_READ  
  
  gpio_put(PICO_GPIO_SPIFFS_ACTIVITY_LED_READ, 0);

  #endif
   
  return SPIFFS_OK;
}

static s32_t RAM_ONLY_FUNCTION(pico_spiffs_write_part_page)(u32_t addr, u32_t size, u8_t *src) {

  uint8_t page_reflash_buffer[ONBOARD_FLASH_PAGE_SIZE];

  uint32_t full_page_address = (addr / ONBOARD_FLASH_PAGE_SIZE) * ONBOARD_FLASH_PAGE_SIZE;

  uint8_t part_page_offset = addr % ONBOARD_FLASH_PAGE_SIZE;

  void *reflash_buffer = &page_reflash_buffer[part_page_offset];

  __builtin_memset(page_reflash_buffer, (uint8_t)0xFF, ONBOARD_FLASH_PAGE_SIZE);

  __builtin_memcpy(reflash_buffer, src, size);

  flash_range_program(full_page_address, page_reflash_buffer, ONBOARD_FLASH_PAGE_SIZE);

  xip_cache_invalidate_range(full_page_address, ONBOARD_FLASH_PAGE_SIZE);
  
}

static s32_t RAM_ONLY_FUNCTION(pico_spiffs_write)(u32_t addr, u32_t size, u8_t *src) {

  // IMPORTANT - Raspberry Pico SDK does *not* allow flash writes to :- 
  // a) non-page (256 byte) aligned addresses 
  // b) partial page lengths (< 256 bytes)
  //
  // As SPIFFS uses both of the above, a workaround is required in order to use
  // SDK flash functions to effectively program a partial and/or non-aligned page.
  // A full 'reflash' 256-byte page array buffer is therefore initialised with all values 0xFF
  // It is then updated by the required SPIFFS write (e.g. 4 bytes) data at the required offset
  // The array is then written to the Pico's onboard flash using standard SDK calls. 
  // As a consequence of how NOR flash works, any bytes with value 0xFF will not change
  // any existing flash data when programmed.

  #ifdef PICO_GPIO_SPIFFS_ACTIVITY_LED_WRITE  
  
  gpio_put(PICO_GPIO_SPIFFS_ACTIVITY_LED_WRITE, 1);

  #endif
 
  uint32_t saved_interrupts = save_and_disable_interrupts();

  uint32_t address_page_boundary = addr % ONBOARD_FLASH_PAGE_SIZE;

  if (address_page_boundary != 0 || size != ONBOARD_FLASH_PAGE_SIZE) {

    pico_spiffs_write_part_page(addr, size, src);

  } else {

    flash_range_program(addr, src, size);

    xip_cache_invalidate_range(addr, ONBOARD_FLASH_PAGE_SIZE);

  }

 // xip_cache_invalidate_range()
  
  restore_interrupts(saved_interrupts);

  #ifdef PICO_GPIO_SPIFFS_ACTIVITY_LED_WRITE  
  
  gpio_put(PICO_GPIO_SPIFFS_ACTIVITY_LED_WRITE, 0);

  #endif
   
  return SPIFFS_OK;
}

static s32_t RAM_ONLY_FUNCTION(pico_spiffs_erase)(u32_t addr, u32_t size) {

  #ifdef PICO_GPIO_SPIFFS_ACTIVITY_LED_ERASE  
  
  gpio_put(PICO_GPIO_SPIFFS_ACTIVITY_LED_ERASE, 1);

  #endif
  
  uint32_t saved_interrupts = save_and_disable_interrupts();

  flash_range_erase(addr, size);

  xip_cache_invalidate_range(addr, size);

  restore_interrupts(saved_interrupts);

  #ifdef PICO_GPIO_SPIFFS_ACTIVITY_LED_ERASE  
  
  gpio_put(PICO_GPIO_SPIFFS_ACTIVITY_LED_ERASE, 0);

  #endif
   
  return SPIFFS_OK;
} 

void pico_spiffs_mount_filesystem() {

  pico_spiffs_config.hal_read_f = pico_spiffs_read;
  pico_spiffs_config.hal_write_f = pico_spiffs_write;
  pico_spiffs_config.hal_erase_f = pico_spiffs_erase;

  pico_spiffs_config.phys_size = (1 * 1024 * 1024);
  pico_spiffs_config.phys_addr = 0x0100000; // start address in onboard Pico flash

  pico_spiffs_config.log_page_size = 256;
  //pico_spiffs_config.log_block_size = (4*1024);
  pico_spiffs_config.log_block_size = (256 * 256);

  pico_spiffs_config.phys_erase_block = (4*1024);

  pico_spiffs_config.fh_ix_offset = 99;  // first file handle issued = 100

  printf("Attempting to mount SPIFFS filesystem ...\n\n");

  pico_spiffs_gpio_init_activity_leds();

  printf("\n");
    
  int mount_result = SPIFFS_mount(&pico_fs,
        
    &pico_spiffs_config,
    spiffs_work_buf,
    spiffs_fds,
    sizeof(spiffs_fds),
    spiffs_cache_buf,
    sizeof(spiffs_cache_buf),
    0);

    if (mount_result != 0) {

      printf(" SPIFFS Mount Failed, result %i = %s\n", mount_result, get_spiffs_error_text_from_value(mount_result));

      printf(" Formatting SPIFFS filesystem ...\n");

      int format_result = SPIFFS_format(&pico_fs);

      printf(" SPIFFS Formatted, result %i = %s\n", format_result, get_spiffs_error_text_from_value(format_result));

    } else {

      printf(" SPIFFS Mounted, result = %s\n", get_spiffs_error_text_from_value(mount_result));

      spiffs_list_directory();
  }
}

void pico_spiffs_gpio_init_activity_leds() {

  #ifdef PICO_GPIO_SPIFFS_ACTIVITY_LED_READ  
    gpio_init(PICO_GPIO_SPIFFS_ACTIVITY_LED_READ); 
    gpio_set_dir(PICO_GPIO_SPIFFS_ACTIVITY_LED_READ, 1);
    printf("Pico SPIFFS Activity LED READ  enabled on GPIO %i\n", PICO_GPIO_SPIFFS_ACTIVITY_LED_READ);
  #endif

  #ifdef PICO_GPIO_SPIFFS_ACTIVITY_LED_WRITE  
    gpio_init(PICO_GPIO_SPIFFS_ACTIVITY_LED_WRITE);
    gpio_set_dir(PICO_GPIO_SPIFFS_ACTIVITY_LED_WRITE, 1);
    printf("Pico SPIFFS Activity LED WRITE enabled on GPIO %i\n", PICO_GPIO_SPIFFS_ACTIVITY_LED_WRITE);
  #endif

  #ifdef PICO_GPIO_SPIFFS_ACTIVITY_LED_ERASE  
    gpio_init(PICO_GPIO_SPIFFS_ACTIVITY_LED_ERASE);
    gpio_set_dir(PICO_GPIO_SPIFFS_ACTIVITY_LED_ERASE, 1);
    printf("Pico SPIFFS Activity LED ERASE enabled on GPIO %i\n", PICO_GPIO_SPIFFS_ACTIVITY_LED_ERASE);
  #endif

}

void test_posix() {

  char read_buffer[100];

  FILE *posix_file;

  const char posix_text_string[] = "Hello Pico POSIX 4\n";

  const void *posix_text = posix_text_string;

  const uint8_t posix_text_length = count_of(posix_text_string);
  
  posix_file = fopen("POSIX4.txt", "w+");

  fwrite(posix_text, 1, posix_text_length, posix_file);

  fclose(posix_file);

  posix_file = fopen("POSIX4.txt", "r");

  fread(read_buffer, 1, posix_text_length, posix_file);

  fclose(posix_file);

  printf("  SPIFFS Readback Buffer = %s\n", read_buffer);

}

