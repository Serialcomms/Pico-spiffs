#include "pico/stdlib.h"
#include "pico/printf.h"

#include "spiffs.h"
#include "spiffs_extras.h"
#include "spiffs_directory.h"

extern spiffs pico_fs;

static const spiffs_entry_type_t SPIFFS_ENTRY_TYPE_DECODER[] = {

    [0] = { "Unknown"},
    [1] = { "File"},
    [2] = { "Directory"},
    [3] = { "Hard Link"},
    [4] = { "Soft Link"},
    [5] = { "Unknown"},

};

spiffs_entry_type_t *entry_lookup = SPIFFS_ENTRY_TYPE_DECODER;

const uint spiffs_max_entry = count_of(SPIFFS_ENTRY_TYPE_DECODER) - 1;

char *get_spiffs_entry_type_from_id(uint spiffs_entry_id) {

  if (spiffs_entry_id < 5) {

    return entry_lookup[spiffs_entry_id].SPIFFS_ERROR_TEXT;

  } else {

    return entry_lookup[spiffs_max_entry].SPIFFS_ERROR_TEXT;

  }

}

void spiffs_snprintf_uint_commas(uint unsigned_integer, char printf_result[14U]) {

  // Maximum unsigned integer value = 2,147,483,647
  // Maximum digit_10 value = 2 (2,nnn,nnn,nnn)

  // IMPORTANT: minimum printf_result size = 14 characters
  // = 10 digits, 3 commas and 1 null terminator character
  // -Wstringop-overflow may be generated if buffer too small

  const size_t buffer_size = 14U;

  uint8_t *printf_buffer = printf_result;
  
  uint digit_10_only = (unsigned_integer / (uint)1e9);  
  uint digits_9_to_7 = (unsigned_integer / (uint)1e6) % 1000U;
  uint digits_6_to_4 = (unsigned_integer / (uint)1e3) % 1000U;
  uint digits_3_to_1 = (unsigned_integer / (uint)1e0) % 1000U;

  if (unsigned_integer < (uint)1e3) {           // < 1,000          
    
    snprintf(printf_buffer, buffer_size, "%u", 
      digits_3_to_1);

  } else if (unsigned_integer < (uint)1e6) {    // < 1,000,000

    snprintf(printf_buffer, buffer_size, "%u,%03u", 
      digits_6_to_4, digits_3_to_1);

  } else if  (unsigned_integer < (uint)1e9) {   // < 1,000,000,000

    snprintf(printf_buffer, buffer_size, "%u,%03u,%03u", 
      digits_9_to_7, digits_6_to_4, digits_3_to_1);

  } else {                                      // > 999,999,999

    snprintf(printf_buffer, buffer_size, "%u,%03u,%03u,%03u", 
      digit_10_only, digits_9_to_7, digits_6_to_4, digits_3_to_1);
  }

}

void spiffs_list_directory() {

  static u32_t bytes_total, bytes_used;

  char buffer_bytes_total[14];
  char buffer_bytes_used[14];

  int32_t filesystem_info = SPIFFS_info(&pico_fs, &bytes_total, &bytes_used);

  spiffs_snprintf_uint_commas(bytes_total, buffer_bytes_total);
  spiffs_snprintf_uint_commas(bytes_used, buffer_bytes_used);

  if (bytes_used == 0) {

    printf("---------------------------------------------------\n");
    printf("  SPIFFS Filesystem mounted, no directory entries  \n");
    printf("---------------------------------------------------\n");
  
    printf("  SPIFFS used  bytes = %10s \n", buffer_bytes_used);
    printf("  SPIFFS total bytes = %10s \n", buffer_bytes_total);
     
  } else {

    spiffs_list_entries();
    
  }
  
}

void spiffs_list_entries() {

  spiffs_DIR d;
  struct spiffs_dirent e;
  struct spiffs_dirent *pe = &e;

  char printf_file_size[14];

  SPIFFS_opendir(&pico_fs, "/", &d);

  printf("\n");

  printf("╔════════╤════════════╤═══════════════╤═════════════╗\n");
  printf("║ Obj ID │ Entry Type │   File Name   │  File Size  ║\n");                                   
  printf("╟────────┼────────────┼───────────────┼─────────────╢\n");

  while ((pe = SPIFFS_readdir(&d, pe))) {

    spiffs_snprintf_uint_commas(pe->size, printf_file_size);

    const char *entry_type = get_spiffs_entry_type_from_id(pe->type);

    printf("║ [%04x] │ %10s │ %13s │ %11s ║ \n", 

    pe->obj_id, 
    entry_type,
    pe->name, 
    printf_file_size  

    );
   
  }

  printf("╚════════╧════════════╧═══════════════╧═════════════╝\n");

  printf("\n\r");

  static u32_t bytes_total = 0;
  static u32_t bytes_used = 0;

  char printf_bytes_total[14];
  char printf_bytes_used[14];

  int32_t filesystem_info = SPIFFS_info(&pico_fs, &bytes_total, &bytes_used);

  spiffs_snprintf_uint_commas(bytes_total, printf_bytes_total);
  spiffs_snprintf_uint_commas(bytes_used, printf_bytes_used);

  printf(" %38s %11s\n\r", "SPIFFS Used  Bytes  ", printf_bytes_used); 
  printf(" %38s %11s\n\r", "SPIFFS Total Bytes  ", printf_bytes_total);    

  printf("\n\r");

  SPIFFS_closedir(&d);

}