
int get_posix_error_number_from_spiffs(int spiffs_error_value);

char *get_spiffs_entry_type_from_id(uint spiffs_entry_id);

char *get_spiffs_error_text_from_value(int spiffs_error_value);

typedef const struct SPIFFS_ERROR_CODES_PROFILE {

    const int POSIX_ERROR_CODE;
    const int SPIFFS_ERROR_CODE;
    char *SPIFFS_ERROR_TEXT;

} spiffs_error_codes_t;


