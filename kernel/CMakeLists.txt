
add_subdirectory(boot "${CMAKE_CURRENT_BINARY_DIR}/boot")
add_subdirectory(mm "${CMAKE_CURRENT_BINARY_DIR}/mm")
add_subdirectory(terminal "${CMAKE_CURRENT_BINARY_DIR}/terminal")
add_subdirectory(utils "${CMAKE_CURRENT_BINARY_DIR}/utils")
add_subdirectory(kern "${CMAKE_CURRENT_BINARY_DIR}/kern")
add_subdirectory("${ARCH}" "${CMAKE_CURRENT_BINARY_DIR}/${ARCH}")

set(CMAKE_C_FLAGS "-D_KERNEL ${ARCH_C_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${ARCH_C_FLAGS} -T ${PLATFORM_LINKER_SCRIPT}")

include_directories("${PROJECT_SOURCE_DIR}/kernel")

add_executable(kernel "")

set_target_properties(kernel PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(kernel "-Wl,--start-group" kern mm boot terminal utils "${ARCH}" "-Wl,--end-group" gcc)
