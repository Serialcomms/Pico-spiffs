
#include <errno.h>
#include "spiffs.h"
#include "spiffs_extras.h"

extern spiffs pico_fs;

// https://man7.org/linux/man-pages/man3/errno.3.html

static const spiffs_error_codes_t SPIFFS_ERROR_CODE_DECODER[] = {

   [0] = { 0,               SPIFFS_OK,                         "SPIFFS OK, No Error"       },
   [1] = { EIO,             SPIFFS_ERR_NOT_MOUNTED,            "Not mounted"               },
   [2] = { ENOSPC,          SPIFFS_ERR_FULL,                   "Filesystem Full"           },
   [3] = { ENOENT,          SPIFFS_ERR_NOT_FOUND,              "File entry not found"      },
   [4] = { ENODATA,         SPIFFS_ERR_END_OF_OBJECT,          "End of object"             },
   [5] = { EIO,             SPIFFS_ERR_DELETED,                "Deleted"                   },
   [6] = { EIO,             SPIFFS_ERR_NOT_FINALIZED,          "Not Finalised"             },
   [7] = { EIO,             SPIFFS_ERR_NOT_INDEX,              "Not Index"                 },
   [8] = { ENOSPC,          SPIFFS_ERR_OUT_OF_FILE_DESCS,      "Out of file descriptors"   },
   [9] = { EIO,             SPIFFS_ERR_FILE_CLOSED,            "File Closed"               },

  [10] = { ENOENT,          SPIFFS_ERR_FILE_DELETED,           "File Deleted"              },
  [11] = { EBADF,           SPIFFS_ERR_BAD_DESCRIPTOR,         "Bad Descriptor"            },
  [12] = { EIO,             SPIFFS_ERR_IS_INDEX,               "Is Index"                  },
  [13] = { EIO,             SPIFFS_ERR_IS_FREE,                "Is Free"                   },
  [14] = { EIO,             SPIFFS_ERR_INDEX_SPAN_MISMATCH,    "Index Span Mismatch"       },
  [15] = { EIO,             SPIFFS_ERR_INDEX_REF_FREE,         "Reference Free"            },
  [16] = { EIO,             SPIFFS_ERR_INDEX_REF_LU,           "Reference LU"              },
  [17] = { EIO,             SPIFFS_ERR_INDEX_INVALID,          "Index Invalid"             },
  [18] = { EROFS,           SPIFFS_ERR_NOT_WRITABLE,           "Not Writable"              },
  [19] = { EIO,             SPIFFS_ERR_NOT_READABLE,           "Not Readable"              },

  [20] = { EEXIST,          SPIFFS_ERR_CONFLICTING_NAME,       "Conflicting name"          },
  [21] = { EIO,             SPIFFS_ERR_NOT_CONFIGURED,         "Not Configured"            },
  [22] = { EIO,             SPIFFS_ERR_NOT_A_FS,               "SPIFFS Unformatted"        },
  [23] = { EIO,             SPIFFS_ERR_MOUNTED,                "Mounted"                   },
  [24] = { EIO,             SPIFFS_ERR_ERASE_FAIL,             "Erase Fail"                },
  [25] = { EIO,             SPIFFS_ERR_MAGIC_NOT_POSSIBLE,     "Magic Not Possible"        },
  [26] = { EIO,             SPIFFS_ERR_NO_DELETED_BLOCKS,      "No deleted blocks"         },
  [27] = { EEXIST,          SPIFFS_ERR_FILE_EXISTS,            "File Exists"               },
  [28] = { EISDIR,          SPIFFS_ERR_NOT_A_FILE,             "Not a File"                },
  [29] = { EIO,             SPIFFS_ERR_RO_NOT_IMPL,            "Not Implemented"           },

  [30] = { EIO,             SPIFFS_ERR_RO_ABORTED_OPERATION,   "Aborted Operation"         },
  [31] = { ENOSPC,          SPIFFS_ERR_PROBE_TOO_FEW_BLOCKS,   "Too Few Blocks"            },
  [32] = { EIO,             SPIFFS_ERR_PROBE_NOT_A_FS,         "Not a File System"         },
  [33] = { ENAMETOOLONG,    SPIFFS_ERR_NAME_TOO_LONG,          "Name Too Long"             },
  [34] = { EIO,             SPIFFS_ERR_IX_MAP_UNMAPPED,        "IX Map Unmapped"           },
  [35] = { EIO,             SPIFFS_ERR_IX_MAP_MAPPED,          "IX Map Mapped"             },
  [36] = { EIO,             SPIFFS_ERR_IX_MAP_BAD_RANGE,       "IX Map Bad Range"          },
  [37] = { ESPIPE,          SPIFFS_ERR_SEEK_BOUNDS,            "Seek Bounds"               },
  [38] = { EIO,             SPIFFS_ERR_INTERNAL,               "Internal"                  },
  [39] = { EIO,             SPIFFS_ERR_TEST,                   "Test Error"                },
    
  [40] = { EIO,             -999999,                           "Unknown SPIFFS Error"      },

};

spiffs_error_codes_t *error_lookup = SPIFFS_ERROR_CODE_DECODER;

const uint spiffs_error_max_entry = count_of(SPIFFS_ERROR_CODE_DECODER) - 1;

char *get_spiffs_error_text_from_value(int spiffs_error_value) {

  int error_value = 0;

  uint8_t error_index = 0;

  bool value_matched = (spiffs_error_value == 0);
    
  while (value_matched == false && ++error_index < spiffs_error_max_entry) {

    value_matched = (spiffs_error_value == error_lookup[error_index].SPIFFS_ERROR_CODE);
    
  }

  return error_lookup[error_index].SPIFFS_ERROR_TEXT;

}

int get_posix_error_number_from_spiffs(int spiffs_error_value) {

  uint8_t error_index = 0;

  bool value_matched = (spiffs_error_value == 0);
    
  while (value_matched == false && ++error_index < spiffs_error_max_entry) {

    value_matched = (spiffs_error_value == error_lookup[error_index].SPIFFS_ERROR_CODE);
    
  }
 
  return error_lookup[error_index].POSIX_ERROR_CODE;

}
