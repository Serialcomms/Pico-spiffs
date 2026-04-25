
const char *get_spiffs_entry_type_from_id(uint spiffs_entry_id);

const char *get_spiffs_error_text_from_value(int spiffs_error_value);

void spiffs_snprintf_uint_commas(uint unsigned_integer, char printf_result[14U]);

void spiffs_list_directory();

void spiffs_list_entries();

typedef const struct SPIFFS_ERROR_CODES_PROFILE {

    const int SPIFFS_ERROR_CODE;
    const char *SPIFFS_ERROR_TEXT;

} spiffs_error_codes_t;

typedef const struct SPIFFS_ENTRY_TYPES_PROFILE {
    
    const char *SPIFFS_ERROR_TEXT;

} spiffs_entry_type_t;
