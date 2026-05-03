# Bibliothèque cœur émulateur — enrichie au fil des tasks
add_library(riscv_core STATIC
    cpu.c
    memory.c
    decoder.c
    executor.c
)
target_include_directories(riscv_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Exécutable principal
add_executable(riscv_emu main.c)
target_link_libraries(riscv_emu PRIVATE riscv_core)
