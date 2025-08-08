cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME project1)
project(${PROJECT_NAME} C ASM)

set(CMAKE_C_STANDARD 11)

# -----------------------------------------------------------------------------
#  Toolchain check
# -----------------------------------------------------------------------------
if(NOT CMAKE_TOOLCHAIN_FILE)
    message(WARNING "You should invoke CMake with -DCMAKE_TOOLCHAIN_FILE=<path/to/arm-toolchain.cmake>")
endif()

# -----------------------------------------------------------------------------
#  CPU / FPU / ABI flags
# -----------------------------------------------------------------------------
set(MCU_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)
set(OPTIMIZE_FLAGS
    -Oz
)
# -----------------------------------------------------------------------------
#  Common flags
# -----------------------------------------------------------------------------
set(COMMON_FLAGS
    -Wall
    -ffunction-sections
    -fdata-sections
    -fno-unroll-loops
    -fomit-frame-pointer
    -flto
)
set(LINKER_OPTIMIZE
    -Wl,--gc-sections
)
# Apply the flags to every compile command
add_compile_options(${MCU_FLAGS} ${COMMON_FLAGS} ${OPTIMIZE_FLAGS})

# Apply the flags to every compile command
add_compile_options(${MCU_FLAGS} ${COMMON_FLAGS} ${OPTIMIZE_FLAGS})

# -----------------------------------------------------------------------------
#  Include directories 
# -----------------------------------------------------------------------------

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)
set(INCLUDE_DIRS )
foreach(header ${HEADERS})
    get_filename_component(header_dir ${header} DIRECTORY )
    list(APPEND INCLUDE_DIRS ${header_dir})
endforeach()
# Remove duplicate path
list(REMOVE_DUPLICATES INCLUDE_DIRS)

include_directories(
    ${INCLUDE_DIRS}
)
# -----------------------------------------------------------------------------
#  Source files (C) 
# -----------------------------------------------------------------------------

file(GLOB_RECURSE ALL_C_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)

# Filter out CMake temporary files
set(SOURCES)
foreach(c_file ${ALL_C_FILES})
    # Skip CMake compiler test files
    if(NOT c_file MATCHES "CMakeCCompilerId\\.c$" AND 
       NOT c_file MATCHES "CMakeFiles/" AND
       NOT c_file MATCHES "build/")
        list(APPEND SOURCES ${c_file})
    endif()
endforeach()
# -----------------------------------------------------------------------------
#  Startup assembly file (search both .s and .S in Core/Startup)
# -----------------------------------------------------------------------------
# file(GLOB STARTUP_FILE
#      Core/Startup/*.s
#      Core/Startup/*.S
# )
file(GLOB_RECURSE STARTUP_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/*.s
    ${CMAKE_CURRENT_SOURCE_DIR}/*.S
)

if(STARTUP_FILE)
    list(APPEND SOURCES ${STARTUP_FILE})
else()
    message(WARNING "Startup file not found in Core/Startup – check file name (e.g. startup_stm32f411vetx.s)")
endif()

# -----------------------------------------------------------------------------
#  Linker script — adjust name if you rename the .ld file
# -----------------------------------------------------------------------------
file(GLOB_RECURSE LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/*_FLASH.ld)
if(NOT EXISTS ${LINKER_SCRIPT})
    message(FATAL_ERROR "Linker script not found: ${LINKER_SCRIPT}")
endif()

# -----------------------------------------------------------------------------
#  Create ELF target
# -----------------------------------------------------------------------------
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# -----------------------------------------------------------------------------
#  Linker options — order matters: place script first
# -----------------------------------------------------------------------------

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${LINKER_SCRIPT}
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map
    -Wl,--gc-sections
    -static
    --specs=nano.specs
    --specs=nosys.specs
    ${MCU_FLAGS}
    ${OPTIMIZE_FLAGS}
)

# -----------------------------------------------------------------------------
#  Post‑build: BIN, HEX & size report
# -----------------------------------------------------------------------------
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE}              $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "[post-build]  Generated BIN & HEX, printed size"
)
