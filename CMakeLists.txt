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

# this will provide access to struct FTW
add_definitions("-D_XOPEN_SOURCE=500")

add_library(xftw OBJECT
    xftw.c
    )

add_library(libxftw SHARED
    $<TARGET_OBJECTS:xftw>
    )

install (TARGETS libxftw DESTINATION lib)
