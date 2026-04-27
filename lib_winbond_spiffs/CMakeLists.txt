add_library(lib_winbond_spiffs INTERFACE)

target_sources(lib_winbond_spiffs INTERFACE

    spiffs_cache.c
    spiffs_check.c
    spiffs_nucleus.c
    spiffs_hydrogen.c
    spiffs_gc.c

    spiffs_newlib.c
    spiffs_extras.c
    spiffs_pico.c
    
)

target_include_directories(lib_winbond_spiffs INTERFACE

    ${CMAKE_CURRENT_SOURCE_DIR}/

)

target_include_directories(lib_winbond_spiffs PRIVATE

)

target_link_libraries(lib_winbond_spiffs INTERFACE

    pico_stdlib
    pico_status_led
 
)

message("--------------------------------------------------------------")
message("-- Building pellepl SPIFFS flash library - lib_winbond_spiffs ")
message("--------------------------------------------------------------")