#ifndef ISA_ENCODER_H
#define ISA_ENCODER_H

#include <llvm/IR/Function.h>
#include <string>

// Main function to encode LLVM IR to target ISA
std::string encodeISA(const llvm::Function &func);

#endif // ISA_ENCODER_H