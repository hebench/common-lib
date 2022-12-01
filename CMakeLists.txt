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
    hebench/modules/general/include/pool.h
    hebench/modules/general/include/nocopy.h
    hebench/modules/general/include/error.h
    hebench/modules/general/include/memory_buffer.h
    hebench/modules/general/include/api_decl.h
    hebench/modules/general/include/hebench_utilities.h
    hebench/modules/general/include/hebench_math_utils.h
    hebench/modules/args_parser/include/args_parser.h
    hebench/modules/config_reader/include/config_reader.h
    hebench/modules/logging/include/logging.h
    hebench/modules/threading/include/threading.h
    hebench/modules/threading/include/safe_queue.h
    hebench/modules/timer/include/timer.h
    )

list(APPEND ${PROJECT_NAME}_HEADERS
    hebench/modules/threading/include/inl/safe_queue.inl
    )

set(${PROJECT_NAME}_SOURCES
    hebench/modules/args_parser/src/args_parser.cpp
    hebench/modules/config_reader/src/config_reader.cpp
    hebench/modules/threading/src/threading.cpp
    hebench/modules/general/src/hebench_utilities.cpp
    hebench/modules/general/src/hebench_math_utils.cpp
    hebench/modules/general/src/memory_buffer.cpp
    hebench/modules/logging/src/logging.cpp
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
    install(FILES ${HEADER} DESTINATION include/${HEADER_PATH})
endforeach()

