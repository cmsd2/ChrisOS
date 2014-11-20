enable_language(ASM-ATT)

function(load_profile ARCH PLATFORM)
    message(STATUS "loading profile for ${ARCH}-${PLATFORM}")
    file(GLOB ARCH_SRCS "${PROJECT_SOURCE_DIR}/kernel/${ARCH}/*.c")
    file(GLOB PLATFORM_SRCS "${PROJECT_SOURCE_DIR}/kernel/${ARCH}/${PLATFORM}/*.c")

    include("${PROJECT_SOURCE_DIR}/kernel/${ARCH}/toolchain.cmake")
    include("${PROJECT_SOURCE_DIR}/kernel/${ARCH}/${PLATFORM}/flags.cmake")

    set(PLATFORM_LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/kernel/${ARCH}/${PLATFORM}/link.ld" PARENT_SCOPE)
    set(ARCH_SRCS ${ARCH_SRCS} PARENT_SCOPE)
    set(PLATFORM_SRCS ${PLATFORM_SRCS} PARENT_SCOPE)

    set(ARCH ${ARCH} PARENT_SCOPE)
    set(PLATFORM ${PLATFORM} PARENT_SCOPE)

    set(ARCH_C_FLAGS ${ARCH_C_FLAGS} PARENT_SCOPE)
    set(ARCH_ASM_FLAGS ${ARCH_ASM_FLAGS} PARENT_SCOPE)
endfunction(load_profile)
