cmake_minimum_required(VERSION 3.20)
project(optiz VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
endif()

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake from: ${LLVM_DIR}")

add_definitions(${LLVM_DEFINITIONS})

if(NOT LLVM_ENABLE_RTTI)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
    message(STATUS "RTTI disabled")
endif()

include(CheckLinkerFlag)
check_linker_flag(CXX "-fuse-ld=lld" HAS_LLD)
if(HAS_LLD)
    add_link_options("-fuse-ld=lld")
    message(STATUS "Using LLD linker")
endif()

add_executable(optiz
    src/main.cpp 
    # src/fe/AST.cpp
    # src/fe/ASTPrinter.cpp
    src/fe/Diagnostic.cpp
    src/fe/Lexer.cpp
    # src/fe/Parser.cpp
    src/fe/SrcLocation.cpp
)

target_include_directories(optiz PRIVATE
    include 
    ${LLVM_INCLUDE_DIRS}
)

llvm_map_components_to_libnames(llvm_libs core support native)

target_link_libraries(optiz PRIVATE 
    ${llvm_libs}
)
