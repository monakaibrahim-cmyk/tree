cmake_minimum_required(VERSION 3.10)
project(tree C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(SOURCES
	src/main.c
)

add_executable(tree
	${SOURCES}
)

target_include_directories(tree PRIVATE
	src
)

if(MSVC)
    target_compile_options(tree PRIVATE /std:c11)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(tree PRIVATE -Wno-implicit-function-declaration)
endif()
