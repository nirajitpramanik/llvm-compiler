#!/bin/bash
# Build script for the PIM compiler

clang++ -std=c++17 -o pim_compiler pim_llvm_compiler.cpp isa_encoder.cpp memory_mapper.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core`
