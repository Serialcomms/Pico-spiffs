#include <stdio.h>
#include "string.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include <spiffs.h>
#include <spiffs_extras.h>
#include <spiffs_directory.h>
#include "spiffs_pico_external.h"

#include "lib_winbond_w25q/include/winbond_W25Q_main.h"

#define PICO_PIN_MISO 16
#define PICO_PIN_CS   17
#define PICO_PIN_SCK  18
#define PICO_PIN_MOSI 19
#define PICO_SPI_BAUD (40 * 1000 * 1000)
#define PICO_SPI_DEVICE spi0

#define FLASH_FS_START 0x00000000
#define FLASH_FS_SIZE 0x00100000

#define EXTERNAL_FLASH_LOG_PAGE_SIZE 256U
#define EXTERNAL_FLASH_PAGE_SIZE 256U

u8_t  __attribute__((aligned(4))) spiffs_external_fds[32*4];
u8_t  __attribute__((aligned(4))) spiffs_external_work_buf[EXTERNAL_FLASH_LOG_PAGE_SIZE*4];
u8_t  __attribute__((aligned(4))) spiffs_external_cache_buf[(EXTERNAL_FLASH_LOG_PAGE_SIZE+32)*32];

pico_spi_device_t winbond_spi_struct = {

  .PIN_CS = PICO_PIN_CS,
  .PIN_SCK = PICO_PIN_SCK,
  .PIN_MOSI = PICO_PIN_MOSI,
  .PIN_MISO = PICO_PIN_MISO,
  .SPI_BAUD = PICO_SPI_BAUD,
  .device = PICO_SPI_DEVICE,
  .DMA_BYTES = 256,
  .dma_channel_tx = 0,
  .dma_channel_rx = 0,
  .dma_channel_mask_tx_rx = 0

};

pico_spi_device_t *winbond_spi = &winbond_spi_struct;

spiffs pico_fs;

spiffs_config pico_spiffs_external_config;

s32_t pico_spiffs_external_read(u32_t addr, u32_t size, u8_t *dst) {
   
  gpio_put(26, 1);

   if (size == 256) {

    lib_winbond_read_data_dma(winbond_spi, addr, dst, size);

  } else {

    lib_winbond_read_data_spi(winbond_spi, addr, dst, size);

  }

  gpio_put(26, 0);
   
  return SPIFFS_OK;
}

static s32_t pico_spiffs_external_write(u32_t addr, u32_t size, u8_t *src) {
   
  gpio_put(27, 1);

  lib_winbond_program_flash_page(winbond_spi, addr, src, size); // supports full-page and part-page writes

  gpio_put(27, 0);
   
  return SPIFFS_OK;
}

static s32_t pico_spiffs_external_erase(u32_t addr, u32_t size) {
   
  gpio_put(28, 1);

  lib_winbond_erase_size(winbond_spi, addr, size);

  gpio_put(28, 0);
   
  return SPIFFS_OK;

} 

void pico_spiffs_mount_external_filesystem() {

    pico_spiffs_external_config.hal_read_f = pico_spiffs_external_read;
    pico_spiffs_external_config.hal_write_f = pico_spiffs_external_write;
    pico_spiffs_external_config.hal_erase_f = pico_spiffs_external_erase;

    pico_spiffs_external_config.phys_size = (4 * 1024 * 1024);

    pico_spiffs_external_config.phys_addr = 0x000000;           // start address in external Pico flash

    pico_spiffs_external_config.log_page_size = 256;

    pico_spiffs_external_config.log_block_size = (256 * 256);    // 65k

    pico_spiffs_external_config.phys_erase_block = (256 * 256);  // 65k

    pico_spiffs_external_config.fh_ix_offset = 99;               // first file handle issued = 100

    printf("Attempting to mount SPIFFS filesystem on external Winbond flash ...\n");
    
    int mount_result = SPIFFS_mount(&pico_fs,
        
        &pico_spiffs_external_config,
        spiffs_external_work_buf,
        spiffs_external_fds,
        sizeof(spiffs_external_fds),
        spiffs_external_cache_buf,
        sizeof(spiffs_external_cache_buf),
        0
    );

    if (mount_result != 0) {

      printf("Mount failed, formatting SPIFFS filesystem, result=%i\n", mount_result);

      SPIFFS_format(&pico_fs);

    } else {
    
      u32_t total, used;

      SPIFFS_info(&pico_fs, &total, &used);

      printf("SPIFFS total / used space = %i / %i bytes\n", (int)total, (int)used);

      if (used == 0) {

        printf("SPIFFS Filesystem mounted, no directory files\n");
        printf("===================================================\n");
        printf("===================================================\n");

      } else {

        printf("SPIFFS Filesystem mounted, listing directory files\n");

        spiffs_list_directory();

        //printf("Testing POSIX\n");

        //test_posix();

      }
      
    }

}