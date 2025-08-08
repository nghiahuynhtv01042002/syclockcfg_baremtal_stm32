cmake_minimum_required(VERSION 3.13)
project(baremetal)

add_subdirectory(project1)

set(FLASH_TOOL ${CMAKE_SOURCE_DIR}/tools/stm32-programmer/bin/STM32_Programmer_CLI.exe)

# Output binary paths after build
set(PROJECT_1_BIN ${CMAKE_BINARY_DIR}/PROJECT_1/project_1.bin)

# Flash addresses
set(PROJECT_1_ADDRESS 0x08000000)

# Create flash_all custom target
add_custom_target(flash_all
    COMMAND ${FLASH_TOOL} -c port=SWD -d ${PROJECT_1_BIN} ${PROJECT_1_ADDRESS} -rst
    COMMENT Flashing Bootloader and Application to STM32...
    VERBATIM
)