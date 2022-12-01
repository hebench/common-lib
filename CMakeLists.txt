# Copyright (C) 2021 Intel Corporation
# SPDX-License-Identifier: Apache-2.0


# libcommon-lib.a - Static library containing common utilities for C++
#   applications such as command line argument parser, file system,
#   threading, etc.
#

cmake_minimum_required(VERSION 3.12)

project(hebench_common-lib VERSION 0.1.5 LANGUAGES C CXX)

if(CMAKE_BUILD_TYPE)
  set(RELEASE_TYPES
      Debug
      Release
      RelWithDebInfo
      MinSizeRel)
  list(FIND RELEASE_TYPES ${CMAKE_BUILD_TYPE} INDEX_FOUND)
  if(${INDEX_FOUND} EQUAL -1)
    message(
      FATAL_ERROR
        "CMAKE_BUILD_TYPE must be one of Debug, Release, RelWithDebInfo, or MinSizeRel"
      )
  endif()
else()
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of Build" FORCE)
endif()

set(${PROJECT_NAME}_HEADERS
    modules/general/include/pool.h
    modules/general/include/nocopy.h
    modules/general/include/error.h
    modules/general/include/memory_buffer.h
    modules/general/include/api_decl.h
    modules/general/include/hebench_utilities.h
    modules/general/include/hebench_math_utils.h
    modules/args_parser/include/args_parser.h
    modules/config_reader/include/config_reader.h
    modules/logging/include/logging.h
    modules/threading/include/threading.h
    modules/threading/include/safe_queue.h
    modules/timer/include/timer.h
    )

list(APPEND ${PROJECT_NAME}_HEADERS
    modules/threading/include/inl/safe_queue.inl
    )

set(${PROJECT_NAME}_SOURCES
    modules/args_parser/src/args_parser.cpp
    modules/config_reader/src/config_reader.cpp
    modules/threading/src/threading.cpp
    modules/general/src/hebench_utilities.cpp
    modules/general/src/hebench_math_utils.cpp
    modules/general/src/memory_buffer.cpp
    modules/logging/src/logging.cpp
    )

# create the target library
add_library(
    ${PROJECT_NAME} STATIC
    # files
    ${${PROJECT_NAME}_HEADERS}
    ${${PROJECT_NAME}_SOURCES}
    )

# add the include path to this library
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# set compiler properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -fPIC)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
foreach(HEADER IN LISTS ${PROJECT_NAME}_HEADERS)
    get_filename_component(HEADER_PATH ${HEADER} DIRECTORY)
    install(FILES ${HEADER} DESTINATION include/hebench/${HEADER_PATH})
endforeach()

