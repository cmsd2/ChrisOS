
add_subdirectory(boot)
add_subdirectory(mm)
add_subdirectory(terminal)
add_subdirectory(utils)
add_subdirectory("${ARCH}")

file(GLOB KERNEL_SRCS "*.c")

set(CMAKE_C_FLAGS "-D_KERNEL ${ARCH_C_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${ARCH_C_FLAGS} -T ../${PLATFORM_LINKER_SCRIPT}")

include_directories("${PROJECT_SOURCE_DIR}/kernel")

add_executable(kernel ${KERNEL_SRCS})
target_link_libraries(kernel "-Wl,--start-group" mm boot terminal utils "${ARCH}" "-Wl,--end-group" gcc)
