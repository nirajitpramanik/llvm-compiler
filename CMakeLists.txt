cmake_minimum_required(VERSION 3.10)
project(PIM_Compiler)

# Enforce C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find LLVM package
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Add LLVM includes and definitions
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Add include directories for your headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Compiler-specific flags (for MSVC)
if (MSVC)
    add_compile_options(/std:c++17 /EHsc)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "/MT")  # Match LLVM's release build
set(CMAKE_CXX_FLAGS_DEBUG "/MTd")   # Match LLVM's debug build

# Create the executable
add_executable(pim_compiler
  pim_llvm_compiler.cpp
  isa_encoder.cpp
  memory_mapper.cpp
)

# Link against LLVM libraries
llvm_map_components_to_libnames(llvm_libs core support irreader)
target_link_libraries(pim_compiler ${llvm_libs})
