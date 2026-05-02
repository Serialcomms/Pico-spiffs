
typedef const struct SPIFFS_OBJECT_ENTRY_TYPES_PROFILE {
    
  char *OBJECT_ENTRY_TYPE_TEXT;

} spiffs_object_entry_type_t;

void spiffs_list_directory();
void spiffs_list_entries();

void spiffs_snprintf_uint_commas(uint unsigned_integer, char printf_result[14U]);

/*

//         <   8  > <    12    > <     15      > <     13    >
  printf("╔════════╤════════════╤═══════════════╤═════════════╗\n");
  printf("║ Obj ID │ Entry Type │   File Name   │  File Size  ║\n");  
//printf("║12345678│123456789ABC│123456789ABCDEF│123456789ABCD║\n");                                    
  printf("╟────────┼────────────┼───────────────┼─────────────╢\n");
//printf("╚════════╧════════════╧═══════════════╧═════════════╝\n" ); // copy to after entries

*/