cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

enable_testing()

include(CTest)

find_package(unity REQUIRED)
find_package(GTest REQUIRED)

if(RUN_COVERAGE)
        message("Coverage enabled")
        find_program(LCOV_PATH lcov)
            if(NOT LCOV_PATH)
                message(FATAL_ERROR "lcov not found! Aborting...")
        endif()
        find_program(GCOV_PATH gcov)
            if(NOT GCOV_PATH)
                message(FATAL_ERROR "gcov not found! Aborting...")
        endif()
        find_program(GENHTML_PATH genhtml)
            if(NOT GENHTML_PATH)
                message(FATAL_ERROR "genhtml not found! Aborting...")
        endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage --coverage")   
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fprofile-arcs -ftest-coverage --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")   
endif()


file(GLOB TESTS_FILES_SERVER ${CMAKE_CURRENT_SOURCE_DIR}/unit/test_unit_server.cpp) 
file(GLOB SRC_FILES_SERVER 
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/server/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/dataAccess/*.cpp
)
list(FILTER SRC_FILES_SERVER EXCLUDE REGEX ".*main\\.cpp$")

add_executable(test_${PROJECT_NAME}_server ${TESTS_FILES_SERVER} ${SRC_FILES_SERVER})

target_include_directories(test_${PROJECT_NAME}_server PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/server
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/dataAccess
)

target_link_libraries(test_${PROJECT_NAME}_server 
    gtest::gtest
    mongo::mongocxx_static
    cryptopp::cryptopp
    uv
    cjson::cjson
)
add_test(NAME test_${PROJECT_NAME}_server COMMAND test_${PROJECT_NAME}_server)


file(GLOB TESTS_FILES_CLIENT ${CMAKE_CURRENT_SOURCE_DIR}/unit/test_unit_client.c) 
file(GLOB SRC_FILES_CLIENT ${CMAKE_CURRENT_SOURCE_DIR}/../src/client/*.c) 
list(FILTER SRC_FILES_CLIENT EXCLUDE REGEX ".*main\\.c$")

add_executable(test_${PROJECT_NAME}_client ${TESTS_FILES_CLIENT} ${SRC_FILES_CLIENT})

target_include_directories(test_${PROJECT_NAME}_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include/client)

target_link_libraries(test_${PROJECT_NAME}_client 
    unity::unity
    uv
    cjson::cjson
)
add_test(NAME test_${PROJECT_NAME}_client COMMAND test_${PROJECT_NAME}_client -q 100 -s 3 -u usr1 -p abc)

# file(GLOB TESTS_FILES_TIMER ${CMAKE_CURRENT_SOURCE_DIR}/unit/test_timer_functions.c) 
# file(GLOB SRC_FILES_TIMER ${CMAKE_CURRENT_SOURCE_DIR}/../src/client/*.c)
# list(FILTER SRC_FILES_TIMER EXCLUDE REGEX ".*main\\.c$")

# add_executable(test_${PROJECT_NAME}_timer ${TESTS_FILES_TIMER} ${SRC_FILES_TIMER})

# target_include_directories(test_${PROJECT_NAME}_timer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include/client)

# target_link_libraries(test_${PROJECT_NAME}_timer 
#     unity::unity
#     uv
#     cjson::cjson
# )
# add_test(NAME test_${PROJECT_NAME}_timer COMMAND test_${PROJECT_NAME}_timer -q 100 -s 3 -u usr1 -p abc)