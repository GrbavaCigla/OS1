cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

set(TOOLPREFIX riscv64-linux-gnu-)

set(CMAKE_ASM_COMPILER ${TOOLPREFIX}gcc)
set(CMAKE_C_COMPILER   ${TOOLPREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLPREFIX}g++)
set(CMAKE_LINKER       ${TOOLPREFIX}ld)
set(CMAKE_OBJDUMP      ${TOOLPREFIX}objdump)
set(CMAKE_OBJCOPY      ${TOOLPREFIX}objcopy)
set(CMAKE_AR           ${TOOLPREFIX}ar)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(kernel LANGUAGES ASM C CXX)

set(MARCH rv64ima)
set(MABI  lp64)

set(COMMON_FLAGS
    -march=${MARCH}
    -mabi=${MABI}
    -mcmodel=medany
    -mno-relax
    -nostdlib
    -ffreestanding
    -fno-common
    -fno-omit-frame-pointer
    -fno-stack-protector
    -fno-pie
    -Og
    -ggdb
    -Wall
    -Werror
)

add_compile_options("$<$<COMPILE_LANGUAGE:ASM>:-march=${MARCH}>")
add_compile_options("$<$<COMPILE_LANGUAGE:ASM>:-mabi=${MABI}>")
add_compile_options("$<$<COMPILE_LANGUAGE:ASM>:-mno-relax>")
add_compile_options("$<$<COMPILE_LANGUAGE:ASM>:-ggdb>")

add_compile_options("$<$<COMPILE_LANGUAGE:C>:${COMMON_FLAGS}>")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:${COMMON_FLAGS}>")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-std=c++11>")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>")
add_compile_definitions(DEBUG_PRINT=0)

set(HW_LIB_ORIG  ${CMAKE_SOURCE_DIR}/lib/hw.lib)
set(HW_LIB_FIXED ${CMAKE_BINARY_DIR}/hw.lib)
set(HW_LIB_TMP   ${CMAKE_BINARY_DIR}/hwlib.tmp)

add_custom_command(
    OUTPUT ${HW_LIB_FIXED}
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${HW_LIB_TMP}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${HW_LIB_TMP}
    COMMAND cd ${HW_LIB_TMP} && ${CMAKE_AR} x ${HW_LIB_ORIG}
    COMMAND ${CMAKE_OBJCOPY} --set-section-flags .entry_os=alloc,load,readonly,code
                             ${HW_LIB_TMP}/entry.o
    COMMAND cd ${HW_LIB_TMP} && ${CMAKE_AR} rcs ${HW_LIB_FIXED} *.o
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${HW_LIB_TMP}
    DEPENDS ${HW_LIB_ORIG}
    COMMENT "Patching hw.lib entry.o section flags"
)

add_custom_target(hw_lib_fixed DEPENDS ${HW_LIB_FIXED})

file(GLOB_RECURSE SOURCES_ASM CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.S)
file(GLOB_RECURSE SOURCES_C   CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE SOURCES_CPP CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(kernel
    ${SOURCES_ASM}
    ${SOURCES_C}
    ${SOURCES_CPP}
)

add_dependencies(kernel hw_lib_fixed)

set(LDSCRIPT ${CMAKE_SOURCE_DIR}/kernel.ld)

set(CMAKE_CXX_LINK_EXECUTABLE
    "<CMAKE_LINKER> -z max-page-size=4096 -T ${LDSCRIPT} -o <TARGET> <OBJECTS> --start-group ${HW_LIB_FIXED} ${CMAKE_SOURCE_DIR}/lib/mem.lib ${CMAKE_SOURCE_DIR}/lib/console.lib --end-group")

set_target_properties(kernel PROPERTIES
    OUTPUT_NAME kernel
    SUFFIX ""
)

set(QEMU qemu-system-riscv64)
set(QEMU_OPTS
    -machine virt
    -bios none
    -kernel ${CMAKE_BINARY_DIR}/kernel
    -m 128M
    -smp 1
    -nographic
)

add_custom_target(qemu
    COMMAND ${QEMU} ${QEMU_OPTS}
    DEPENDS kernel
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running kernel in QEMU"
)

add_custom_target(qemu-gdb
    COMMAND ${QEMU} ${QEMU_OPTS} -S -gdb tcp::1234
    DEPENDS kernel
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running kernel in QEMU (waiting for GDB on port 1234)"
)