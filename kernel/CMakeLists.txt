
add_subdirectory(boot)
add_subdirectory(mm)
add_subdirectory(terminal)
add_subdirectory(utils)
add_subdirectory(kern)
add_subdirectory("${ARCH}")

set(CMAKE_C_FLAGS "-D_KERNEL ${ARCH_C_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${ARCH_C_FLAGS} -T ${PROJECT_SOURCE_DIR}/${PLATFORM_LINKER_SCRIPT}")

include_directories("${PROJECT_SOURCE_DIR}/kernel")

add_executable(kernel "")

set_target_properties(kernel PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(kernel "-Wl,--start-group" kern mm boot terminal utils "${ARCH}" "-Wl,--end-group" gcc)
