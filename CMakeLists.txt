cmake_minimum_required(VERSION 2.8)

project(libxftw)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -fPIC")

if (("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang") OR (("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU") AND (CMAKE_C_COMPILER_VERSION VERSION_GREATER 4.0)))
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
endif()

# the following is until we learn how to reorder the gcc arguments to correctly link on Ubuntu
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed")

# ensure large file support 
add_definitions("-D_FILE_OFFSET_BITS=64")
add_definitions("-D_LARGEFILE64_SOURCE")
add_definitions("-D_LARGEFILE_SOURCE")
add_definitions("-D__USE_LARGEFILE64")

# this will provide access to struct FTW
add_definitions("-D_XOPEN_SOURCE=700")

add_library(xftw.o OBJECT
    xftw.c
    )

add_library(xftw SHARED
    $<TARGET_OBJECTS:xftw.o>
    )

# set the subst vars that configure_file() will use
set(PROJECT_VERSION 1.0)

set(PKG_CONFIG_LIBS
    "-lxftw"
)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/pkg-config.pc.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
)

install (TARGETS xftw DESTINATION lib)
install (FILES "${PROJECT_BINARY_DIR}/xftw.h" DESTINATION include)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(PKG_CONFIG_INSTALL_DIR "lib/pkgconfig")
else()
    set(PKG_CONFIG_INSTALL_DIR "libdata/pkgconfig")
endif()
install (FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION ${PKG_CONFIG_INSTALL_DIR})

