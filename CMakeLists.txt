cmake_minimum_required(VERSION 3.6)
project(rin C CXX)
include(/usr/local/pspdev/psp/share/cmake/PSP.cmake)

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE all_src FOLLOW_SYMLINKS
        "*.h"
        "*.cpp"
        "*.c"
        )

MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list *.h)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

set(return_list ".")
#HEADER_DIRECTORIES(${return_list} ../pspdev/psp/sdk/include)
#include_directories(${return_list} ../pspdev/psp/sdk/include)



add_executable(rin ${all_src} tools.c tools.h menu_rewind.c menu_rewind.h menu_credits.c menu_credits.h)
set_target_properties(rin PROPERTIES LINKER_LANGUAGE CXX)