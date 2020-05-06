cmake_minimum_required(VERSION 2.8)
project(DanPaVM)

include_directories(src/.)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR} src/*.c)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR} src/*.h)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-override-init -O2 -ffast-math -g")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME})
