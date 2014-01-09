file(GLOB MM_SRCS "*.c")

set(CMAKE_C_FLAGS "-D_KERNEL ${ARCH_C_FLAGS}")

include_directories("${PROJECT_SOURCE_DIR}/kernel")

add_library(mm STATIC ${MM_SRCS})
