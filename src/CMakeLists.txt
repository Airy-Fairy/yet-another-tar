cmake_minimum_required(VERSION 2.8.11)

project(yet-another-tar)

set(EXE_NAME yat)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
    main.cpp
    parser.cpp
    archiver.cpp)

set(HEADERS
    parser.h
    archiver.h)

add_executable(${EXE_NAME}
    ${SOURCES}
    ${HEADERS}
)

if (UNIX)
    target_link_libraries(${EXE_NAME} stdc++fs)
endif()

source_group("sources" FILES ${SOURCES})
source_group("headers" FILES ${HEADERS})

