cmake_minimum_required( VERSION 2.8 )

set( CMAKE_CXX_FLAGS "-std=c++17" )

PROJECT(Echo)

MESSAGE(STATUS "Project: Echo")

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

set(SRC
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

set(TEST
    ${PROJECT_SOURCE_DIR}/src/test/test.cpp
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

set(LIB_NAME Echo_lib)

add_library(${LIB_NAME} STATIC ${SRC})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
   
add_executable(${PROJECT_NAME} ${SRC})
   
add_executable(test ${TEST})

target_link_libraries(${PROJECT_NAME} ${LIB_NAME})