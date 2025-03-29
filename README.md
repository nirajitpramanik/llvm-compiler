# PIM LLVM Compiler

This is a simple LLVM-based compiler for matrix multiplication using ISA encoding.

## 🛠️ Setup Instructions

### **1. Clone the Repository**
```sh
git clone https://github.com/nirajitpramanik/llvm-compiler.git
cd llvm-compiler
```

### **2. Install Dependencies**
Ensure you have **LLVM**, **CMake**, and **Ninja** installed.

- **Ubuntu**:
  ```sh
  sudo apt install llvm clang cmake ninja-build
  ```
- **Windows** (via Chocolatey):
  ```sh
  choco install llvm cmake ninja
  ```
- **Windows** (via Winget):
  ```sh
  winget install LLVM.LLVM
  winget install Kitware.CMake
  winget install Ninja-build.Ninja
  ```

### **3. Build the Project**

Run the build script:

- **Windows (Command Prompt or PowerShell)**
  ```sh
  build.bat
  ```
- **Linux/macOS**
  ```sh
  ./build.sh
  ```

Alternatively, manually configure and build using:
```sh
mkdir build && cd build
cmake ..
cmake --build .
```

### **4. Run the Compiler**
After successful compilation, run:

- **Windows**
  ```sh
  .\build\Release\pim_compiler.exe
  ```
- **Linux/macOS**
  ```sh
  ./build/pim_compiler
  ```
---

## 📝 Notes
- Ensure that `LLVM_DIR` is properly set if you face CMake errors.
- Modify `pim_llvm_compiler.cpp` to test different inputs.
- Report issues via GitHub if any problem occurs.

Happy Coding! 🚀

