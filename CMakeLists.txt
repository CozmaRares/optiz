cmake_minimum_required(VERSION 3.20)
project(optiz VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find the Packages
find_package(LLVM REQUIRED CONFIG)
# find_package(Clang REQUIRED CONFIG)
# find_package(MLIR REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake from: ${LLVM_DIR}")

# 2. Add Definitions
add_definitions(${LLVM_DEFINITIONS})

# 3. Define the executable
add_executable(optiz
    src/main.cpp 
    src/fe/AST.cpp
    src/fe/ASTPrinter.cpp
    src/fe/Diagnostic.cpp
    src/fe/Lexer.cpp
    src/fe/Parser.cpp
    src/fe/SrcLocation.cpp
)

# 4. Add Include Directories for all three
target_include_directories(optiz PRIVATE
    include 
    ${LLVM_INCLUDE_DIRS}
    # ${CLANG_INCLUDE_DIRS}
    # ${MLIR_INCLUDE_DIRS}
)

# 5. Link LLVM Components
llvm_map_components_to_libnames(llvm_libs core support native)

# 6. Link Everything
# Note: Clang and MLIR libraries usually need to be listed specifically 
# because they don't use the 'map_components' macro as extensively as LLVM Core.
target_link_libraries(optiz PRIVATE 
    ${llvm_libs}
    # clangAST
    # clangBasic
    # clangFrontend
    # MLIRIR
    # MLIRSupport
    # Add more MLIR/Clang libs here as you start using them
)

# 7. Use LLD for linking
add_link_options("-fuse-ld=lld")
