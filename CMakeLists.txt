cmake_minimum_required (VERSION 2.8)
project (ChrisOS C ASM)

enable_language(ASM-ATT)

include("common.cmake")

load_profile("ia32" "pc")

configure_file (
    "${PROJECT_SOURCE_DIR}/include/ChrisOS_config.h.in"
    "${PROJECT_BINARY_DIR}/include/ChrisOS_config.h"
)

include_directories("${PROJECT_BINARY_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/libs/uthash/src")

add_subdirectory(kernel)

