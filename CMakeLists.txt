cmake_minimum_required(VERSION 3.29)
project(tbig VERSION 0.0.1 LANGUAGES C CXX ASM) # asm should be only if we enable it but i have no idea how to do this great

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
option(BUILD_ASM "if for some reason you think this is a good idea" ON)

if (MSVC OR APPLE)
    set(BUILD_ASM OFF) # not supported
endif ()


add_executable(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PRIVATE
        -madx -mbmi2 -mavx2 -march=native # -O3 # -O3 should be ONLY in release build (cmake enable it by default)
)

file(GLOB_RECURSE CPP_FILES src/*.cpp)
file(GLOB_RECURSE C_FILES src/*.c)
if (${BUILD_ASM})
    file(GLOB_RECURSE ASM_FILES src/*.S)
    set_source_files_properties(${ASM_FILES} PROPERTIES LANGUAGE ASM)
    add_library(AsmBigIntImpl STATIC ${ASM_FILES})
    target_link_libraries(${PROJECT_NAME} PRIVATE AsmBigIntImpl)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ASM_ENABLED)
endif ()

target_sources(${PROJECT_NAME} PRIVATE
        ${CPP_FILES}
        ${C_FILES}
)
