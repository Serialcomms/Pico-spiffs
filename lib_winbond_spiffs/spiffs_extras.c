
#include "spiffs.h"
#include "spiffs_extras.h"

extern spiffs pico_fs;

static const spiffs_error_codes_t SPIFFS_ERROR_CODE_DECODER[] = {

     [0] = { SPIFFS_OK,                         "SPIFFS OK, No Error"       },
     [1] = { SPIFFS_ERR_NOT_MOUNTED,            "Not mounted"               },
     [2] = { SPIFFS_ERR_FULL ,                  "Filesystem Full"           },
     [3] = { SPIFFS_ERR_NOT_FOUND,              "Not found"                 },
     [4] = { SPIFFS_ERR_END_OF_OBJECT,          "End of object"             },
     [5] = { SPIFFS_ERR_DELETED,                "Deleted"                   },
     [6] = { SPIFFS_ERR_NOT_FINALIZED,          "Not Finalised"             },
     [7] = { SPIFFS_ERR_NOT_INDEX,              "Not Index"                 },
     [8] = { SPIFFS_ERR_OUT_OF_FILE_DESCS,      "Out of file descriptors"   },
     [9] = { SPIFFS_ERR_FILE_CLOSED,            "File Closed"               },

    [10] = { SPIFFS_ERR_FILE_DELETED,           "File Deleted"              },
    [11] = { SPIFFS_ERR_BAD_DESCRIPTOR,         "Bad Descriptor"            },
    [12] = { SPIFFS_ERR_IS_INDEX,               "Is Index"                  },
    [13] = { SPIFFS_ERR_IS_FREE,                "Is Free"                   },
    [14] = { SPIFFS_ERR_INDEX_SPAN_MISMATCH,    "Index Span Mismatch"       },
    [15] = { SPIFFS_ERR_INDEX_REF_FREE,         "Reference Free"            },
    [16] = { SPIFFS_ERR_INDEX_REF_LU,           "Reference LU"              },
    [17] = { SPIFFS_ERR_INDEX_INVALID,          "Index Invalid"             },
    [18] = { SPIFFS_ERR_NOT_WRITABLE,           "Not Writable"              },
    [19] = { SPIFFS_ERR_NOT_READABLE,           "Not Readable"              },

    [20] = { SPIFFS_ERR_CONFLICTING_NAME,       "Conflicting name"          },
    [21] = { SPIFFS_ERR_NOT_CONFIGURED,         "Not Configured"            },
    [22] = { SPIFFS_ERR_NOT_A_FS,               "SPIFFS Unformatted"        },
    [23] = { SPIFFS_ERR_MOUNTED,                "Mounted"                   },
    [24] = { SPIFFS_ERR_ERASE_FAIL,             "Erase File"                },
    [25] = { SPIFFS_ERR_MAGIC_NOT_POSSIBLE,     "Magic Not Possible"        },
    [26] = { SPIFFS_ERR_NO_DELETED_BLOCKS,      "No deleted blocks"         },
    [27] = { SPIFFS_ERR_FILE_EXISTS,            "File Exists"               },
    [28] = { SPIFFS_ERR_NOT_A_FILE,             "Not a File"                },
    [29] = { SPIFFS_ERR_RO_NOT_IMPL,            "Not Implemented"           },

    [30] = { SPIFFS_ERR_RO_ABORTED_OPERATION,   "Aborted"                   },
    [31] = { SPIFFS_ERR_PROBE_TOO_FEW_BLOCKS,   "Too Few Blocks"            },
    [32] = { SPIFFS_ERR_PROBE_NOT_A_FS,         "Not a File System"         },
    [33] = { SPIFFS_ERR_NAME_TOO_LONG,          "Name Too Long"             },
    [33] = { SPIFFS_ERR_IX_MAP_UNMAPPED,        "IX Map Unmapped"           },
    [34] = { SPIFFS_ERR_IX_MAP_MAPPED,          "IX Map mapped"             },
    [35] = { SPIFFS_ERR_IX_MAP_BAD_RANGE,       "IX Map Bad Range"          },
    [36] = { SPIFFS_ERR_SEEK_BOUNDS,            "Seek Bounds"               },
    [37] = { SPIFFS_ERR_INTERNAL,               "Internal"                  },
    [38] = { SPIFFS_ERR_TEST,                   "Test Error"                },
    [39] = { -999999,                           "Unknown SPIFFS Error"      },

};

spiffs_error_codes_t *error_lookup = SPIFFS_ERROR_CODE_DECODER;

const uint spiffs_error_max_entry = count_of(SPIFFS_ERROR_CODE_DECODER) - 1;

const char *get_spiffs_error_text_from_value(int spiffs_error_value) {

    int error_value = 0;

    uint8_t error_index = 0;

    bool value_matched = (spiffs_error_value == 0);
    
    while (value_matched == false && ++error_index < spiffs_error_max_entry) {

        value_matched = (spiffs_error_value == error_lookup[error_index].SPIFFS_ERROR_CODE);
    }

    return error_lookup[error_index].SPIFFS_ERROR_TEXT;

}

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

const char *get_spiffs_entry_type_from_id(uint spiffs_entry_id) {

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
  
      printf("  SPIFFS used  bytes = %-10s \n", buffer_bytes_used);
      printf("  SPIFFS total bytes = %-10s \n", buffer_bytes_total);
     
    } else {

        spiffs_list_entries();
    }
}

void spiffs_list_entries() {

  spiffs_DIR d;
  struct spiffs_dirent e;
  struct spiffs_dirent *pe = &e;

  int column_width = 8;

  SPIFFS_opendir(&pico_fs, "/", &d);

  printf("\n");

//         <   8  > <    12    > <     13    > <     13    >
  printf("╔════════╤════════════╤═════════════╤═════════════╗\n");
  printf("║ Obj ID │ Entry Type │  File Name  │  File Size  ║\n");  
//printf("║12345678│123456789ABC│123456789ABCD│123456789ABCD║\n");                                    
  printf("╟────────┼────────────┼─────────────┼─────────────╢\n");
//printf("╚════════╧════════════╧═════════════╧═════════════╝\n" ); // copy to after entries

  char printf_file_size[14];

  while ((pe = SPIFFS_readdir(&d, pe))) {

    spiffs_snprintf_uint_commas(pe->size, printf_file_size);

    const char *entry_type = get_spiffs_entry_type_from_id(pe->type);

//           <   6  > < 10 > < 11 > < 11 >      // set width to column size above - 2
    printf("║ [%04x] │ %10s │ %11s │ %11s ║ \n", 

    pe->obj_id, 
    entry_type,
    pe->name, 
    printf_file_size  

    );
   
  }

  printf("╚════════╧════════════╧═════════════╧═════════════╝\n");

  printf("\n\r");

  static u32_t bytes_total = 0;
  static u32_t bytes_used = 0;

  char printf_bytes_total[14];
  char printf_bytes_used[14];

  int32_t filesystem_info = SPIFFS_info(&pico_fs, &bytes_total, &bytes_used);

  spiffs_snprintf_uint_commas(bytes_total, printf_bytes_total);
  spiffs_snprintf_uint_commas(bytes_used, printf_bytes_used);

  //printf(" SPIFFS total bytes=%s used bytes=%s\n", printf_bytes_total, printf_bytes_used);

  printf("  SPIFFS used  bytes = %10s \n", printf_bytes_used);
  printf("  SPIFFS total bytes = %10s \n", printf_bytes_total);

  printf("\n\r");

  SPIFFS_closedir(&d);

}