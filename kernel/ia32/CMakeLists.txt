file(GLOB IA32_SRCS "*.S" "*.c")

set(CMAKE_C_FLAGS "-D_KERNEL ${ARCH_C_FLAGS}")

include_directories("${PROJECT_SOURCE_DIR}/kernel")

add_library(ia32 STATIC ${IA32_SRCS})
