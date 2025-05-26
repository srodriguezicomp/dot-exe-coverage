cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(DOT_EXE_LOGISTICS)

option(P_TARGET "Build target: server or client" "server")

include_directories(include)
include(${CMAKE_BINARY_DIR}/build/Release/generators/conan_toolchain.cmake)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-DP_TARGET_SERVER)
include_directories(include/server include/dataAccess)
find_package(mongocxx REQUIRED)
find_package(cryptopp REQUIRED)
find_package(libuv REQUIRED)
find_package(cJSON REQUIRED)
file(GLOB SOURCES
    src/server/*.cpp
    src/dataAccess/*.cpp
    src/common/*.cpp
)
add_executable(dot_exe_server ${SOURCES})
target_include_directories(dot_exe_server PRIVATE include)
target_link_libraries(dot_exe_server
    mongo::mongocxx_static
    cryptopp::cryptopp
    uv
    cjson::cjson
)

add_definitions(-DP_TARGET_CLIENT)
include_directories(include/client)
file(GLOB SOURCES
    src/client/*.c
    src/common/*.c
)
add_executable(dot_exe_client ${SOURCES})
target_link_libraries(dot_exe_client
    uv
    cjson::cjson
)

if(RUN_COVERAGE EQUAL 1)
    add_subdirectory(tests)
endif()



