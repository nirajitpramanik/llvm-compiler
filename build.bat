@echo off
rem Build script for PIM Compiler (Windows)
clang++ -std=c++17 -o pim_compiler.exe pim_llvm_compiler.cpp isa_encoder.cpp memory_mapper.cpp ^
    -I "%LLVM_DIR%\include" -L "%LLVM_DIR%\lib" -lLLVM
echo Build complete. Run "pim_compiler.exe" to execute.
