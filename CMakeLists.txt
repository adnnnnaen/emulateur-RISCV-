cmake_minimum_required(VERSION 3.20)
project(riscv_emu C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)


add_compile_options(
    -Wall -Wextra -Wpedantic
    -Wshadow -Wconversion -Wsign-conversion
    -Wmissing-prototypes -Wstrict-prototypes
)

# Debug par défaut (release : cmake -DCMAKE_BUILD_TYPE=Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Tests activés
enable_testing()

# Sub-modules
add_subdirectory(src)
add_subdirectory(tests)
