cmake_minimum_required(VERSION 3.29)
project(tbig LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-madx -mbmi2 -mavx2 -O3 -march=native)

file(GLOB_RECURSE CPP_FILES src/*.cpp)
file(GLOB_RECURSE C_FILES src/*.c)
file(GLOB_RECURSE ASM_FILES src/*.S)
set_source_files_properties(${ASM_FILES} PROPERTIES LANGUAGE ASM)

add_library(AsmBigIntImpl STATIC ${ASM_FILES})

add_executable(${PROJECT_NAME} ${CPP_FILES} ${C_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE AsmBigIntImpl)
