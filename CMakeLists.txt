cmake_minimum_required(VERSION 2.8)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_SOURCE_DIR}/cmake)

project(li)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

# Create the include dir in the build directory.
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/li)

function (link_build_include lib_name target_dir)
IF (WIN32)
    STRING(REGEX REPLACE "/" "\\\\"  CMAKE_SOURCE_DIR_BACKSLASH ${CMAKE_SOURCE_DIR}) 
    STRING(REGEX REPLACE "/" "\\\\"  TARGET_BS ${target_dir}) 
    execute_process(COMMAND cmd.exe /c mklink /J ${lib_name}
    "${CMAKE_SOURCE_DIR_BACKSLASH}${TARGET_BS}"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/include/li/)
ELSE()
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/include/li/${lib_name}")
        execute_process(COMMAND ln -s
                        "${CMAKE_SOURCE_DIR}${target_dir}" ${lib_name}
                        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/include/li/)
    ENDIF()
ENDIF()
endfunction()

link_build_include("metamap" "/libraries/metamap/metamap")
link_build_include("json" "/libraries/json/json")
link_build_include("sqlite" "/libraries/sqlite")
link_build_include("callable_traits" "/libraries/callable_traits")
link_build_include("http_backend" "/libraries/http_backend/http_backend")
link_build_include("http_client" "/libraries/http_client/http_client")
link_build_include("symbol" "/libraries/symbol/symbol")
link_build_include("sql" "/libraries/sql/sql")                               

include_directories(${CMAKE_BINARY_DIR}/include)

#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native -flto") 

add_custom_target(
    symbols_generation DEPENDS li_symbol_generator
    COMMAND ${CMAKE_BINARY_DIR}/libraries/symbol/li_symbol_generator ${CMAKE_CURRENT_SOURCE_DIR}/libraries)

function(li_add_executable target_name)
  add_executable(${target_name} ${ARGN})
  add_dependencies(${target_name} symbols_generation)
endfunction(li_add_executable)

#include_directories(./single_headers /usr/include/mysql)
#add_executable(test_single_headers ./single_headers/test.cc)

add_subdirectory(libraries/metamap)
add_subdirectory(libraries/callable_traits)
add_subdirectory(libraries/json)
add_subdirectory(libraries/symbol)
add_subdirectory(libraries/http_backend)
add_subdirectory(libraries/http_client)
add_subdirectory(libraries/sql)
