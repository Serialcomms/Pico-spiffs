# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(spiffs_tester_v01 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(spiffs_tester_v01 
    main.c 
  
  
    )

add_subdirectory(lib_pico_spiffs)

pico_set_program_name(spiffs_tester_v01 "spiffs_tester_v01")
pico_set_program_version(spiffs_tester_v01 "0.1")

# no_flash means the target is to run from RAM
# pico_set_binary_type(spiffs_tester_v01 no_flash)
pico_set_binary_type(spiffs_tester_v01 copy_to_ram)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(spiffs_tester_v01 1)
pico_enable_stdio_usb(spiffs_tester_v01 0)

# Add the standard library to the build
target_link_libraries(spiffs_tester_v01
        pico_stdlib
        hardware_flash)

# Add the standard include files to the build
target_include_directories(spiffs_tester_v01 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/lib_pico_spiffs
)

# Add any user requested libraries
target_link_libraries(spiffs_tester_v01 
        
    hardware_dma
    pico_bootrom
    pico_unique_id
    pico_status_led
    lib_pico_spiffs
    )

target_compile_definitions(spiffs_tester_v01 PRIVATE

	#PICO_EMBED_XIP_SETUP=1
)

pico_add_extra_outputs(spiffs_tester_v01)

