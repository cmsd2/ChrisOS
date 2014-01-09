file(GLOB UTILS_SRCS "*.c")

set(CMAKE_C_FLAGS "${ARCH_C_FLAGS}")

include_directories("${PROJECT_SOURCE_DIR}/kernel")

add_library(utils STATIC ${UTILS_SRCS})
