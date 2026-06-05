cmake_minimum_required(VERSION 4.0.0)

find_package(Python3 REQUIRED)

project(rqmt)

# Define the version part as a CMake variable (default: revision)
set(VERSION_PART "build" CACHE STRING "Version part to bump: major, minor, patch, or revision")

# Function to bump the version before building, uses a python script to update 
# the relevant constant in the CMAKE_CURRENT_SOURCE_DIR/version.h file
function(bump_version)
    if(VERSION_PART)
        add_custom_target(bump_version_target ALL
            COMMAND ${Python3_EXECUTABLE} 
                    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bump_version.py
                    ${CMAKE_CURRENT_SOURCE_DIR}/util/version.h
                    ${VERSION_PART}
                    ${CMAKE_CURRENT_SOURCE_DIR}
            BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/util/version.h
            COMMENT "Bumping ${VERSION_PART} version in version.h"
        )
    endif()
endfunction()

bump_version()

add_subdirectory(util) 
add_subdirectory(tokenizer)
add_subdirectory(cpp_parser)
add_subdirectory(workspace)
add_subdirectory(lsp)

# Function to copy a target to a specified output location
# ARG0  The name of the target exe (without extension) in the working directory
# ARG1  The name (i.e. the full path, including file name of the end location)
function(rename_exe)
    set(TARGET_NAME ${ARGV0})
    set(NEW_LOCATION ${ARGV1})

    get_filename_component(OUTPUT_DIR ${NEW_LOCATION} DIRECTORY)

    if(UNIX)
        add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
            COMMAND ${CMAKE_COMMAND} -E rename ${TARGET_NAME}.x ${NEW_LOCATION}
        )
    elseif(IOS)
        add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
            COMMAND ${CMAKE_COMMAND} -E rename ${TARGET_NAME}.x ${NEW_LOCATION}
        )
    elseif(WIN32)
        add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
            COMMAND ${CMAKE_COMMAND} -E rename ${TARGET_NAME}.exe ${NEW_LOCATION}
        )
    endif()
endfunction(rename_exe)

# Function to build a target based on the libraries util, tokenizer, parser
# ARG0  The name of the target (the .c file-name must be identical, without 
#       extension)
# ARG1  The extension (c or cpp)
function(build_target)

    set(TARGET_NAME ${ARGV0})
    set(TARGET_FILE ${ARGV0}.${ARGV1})

    if(UNIX)
        set(OUTPUT_FILE  ../server/out/bin/linux/${ARGV0})
    elseif(WIN32)
        set(OUTPUT_FILE  ../server/out/bin/windows/${ARGV0}.exe)
    elseif(IOS)
        set(OUTPUT_FILE  ../server/out/bin/macos/${ARGV0})
    endif()

    add_executable(${TARGET_NAME} ${TARGET_FILE})

    if(TARGET bump_version_target)
        add_dependencies(${TARGET_NAME} bump_version_target)
    endif()

    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)

    target_link_libraries(${TARGET_NAME} util tokenizer)
    
        target_include_directories(${TARGET_NAME} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/tokenizer
            ${CMAKE_CURRENT_SOURCE_DIR}/util
        )

    if(${ARGV1} STREQUAL  "cpp")
        target_link_libraries(${TARGET_NAME} cpp_parser workspace lsp)

        target_include_directories(${TARGET_NAME} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/cpp_parser
            ${CMAKE_CURRENT_SOURCE_DIR}/workspace
            ${CMAKE_CURRENT_SOURCE_DIR}/lsp
            ${CMAKE_CURRENT_SOURCE_DIR}/util
        )
    endif()

    rename_exe(${TARGET_NAME} ${OUTPUT_FILE})
endfunction(build_target)

function(build_target_c)
    build_target(${ARGV0} c)
endfunction(build_target_c)

function(build_target_cpp)
    build_target(${ARGV0} cpp)
endfunction(build_target_cpp)

build_target_cpp(rqmt)