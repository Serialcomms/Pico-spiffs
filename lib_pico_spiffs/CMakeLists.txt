
set(PICO_LIBRARY_NAME lib_pico_spiffs)

add_library(${PICO_LIBRARY_NAME} INTERFACE)

target_sources(${PICO_LIBRARY_NAME} INTERFACE

    spiffs_cache.c
    spiffs_check.c
    spiffs_nucleus.c
    spiffs_hydrogen.c
    spiffs_gc.c

    spiffs_newlib.c
    spiffs_extras.c
    spiffs_directory.c
    spiffs_pico.c
    
)

target_include_directories(${PICO_LIBRARY_NAME} INTERFACE

    ${CMAKE_CURRENT_SOURCE_DIR}/

)

target_link_libraries(${PICO_LIBRARY_NAME} INTERFACE

    pico_stdlib
    pico_status_led
 
)

message("-------------------------------------------------------------")
message("--  Building pellepl SPIFFS flash library - ${PICO_LIBRARY_NAME}  ")
message("-------------------------------------------------------------")