#include "isa_encoder.h"
#include <llvm/IR/Instructions.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <sstream>
#include <map>
#include <set>

std::string encodeISA(const llvm::Function &func) {
    std::ostringstream isa;
    
    // Map to track LLVM values to ISA registers
    std::map<const llvm::Value*, std::string> valueToReg;
    int regCounter = 0;
    
    // Function to get or assign a register for a value
    auto getRegister = [&](const llvm::Value *val) -> std::string {
        if (valueToReg.find(val) == valueToReg.end()) {
            valueToReg[val] = "R" + std::to_string(regCounter++);
        }
        return valueToReg[val];
    };
    
    // Track the loop indices (i, j, k) for matrix multiplication
    const llvm::Value *loopI = nullptr;
    const llvm::Value *loopJ = nullptr;
    const llvm::Value *loopK = nullptr;
    
    // Find loop variables from allocas
    for (auto &bb : func) {
        for (auto &inst : bb) {
            if (auto *alloca = llvm::dyn_cast<llvm::AllocaInst>(&inst)) {
                if (alloca->getName() == "i") loopI = alloca;
                else if (alloca->getName() == "j") loopJ = alloca;
                else if (alloca->getName() == "k") loopK = alloca;
            }
        }
    }
    
    // Look for the loop bound (N parameter)
    auto args = func.arg_begin();
    std::advance(args, 3); // N is the 4th parameter
    const llvm::Value *nParam = &(*args);
    
    isa << "; Matrix multiplication ISA code\n";
    isa << "INIT C            ; Initialize result matrix to zeros\n\n";
    
    // Register assignment for loop indices
    std::string iReg = getRegister(loopI);
    std::string jReg = getRegister(loopJ);
    std::string kReg = getRegister(loopK);
    std::string nReg = getRegister(nParam);
    
    // Generate the ISA code for nested loops
    isa << "LOAD " << nReg << ", N    ; Load matrix dimension\n";
    isa << "LOAD " << iReg << ", 0    ; Initialize i = 0\n";
    isa << "LOOP_I:              ; Start of i loop\n";
    isa << "  CMP " << iReg << ", " << nReg << "\n";
    isa << "  JGE EXIT_I         ; Exit if i >= N\n\n";
    
    isa << "  LOAD " << jReg << ", 0  ; Initialize j = 0\n";
    isa << "  LOOP_J:            ; Start of j loop\n";
    isa << "    CMP " << jReg << ", " << nReg << "\n";
    isa << "    JGE EXIT_J       ; Exit if j >= N\n\n";
    
    isa << "    LOAD R10, 0      ; Initialize C[i][j] = 0\n";
    isa << "    LOAD " << kReg << ", 0  ; Initialize k = 0\n";
    isa << "    LOOP_K:          ; Start of k loop\n";
    isa << "      CMP " << kReg << ", " << nReg << "\n";
    isa << "      JGE EXIT_K     ; Exit if k >= N\n\n";
    
    // Matrix multiplication core operations
    isa << "      ; Load A[i][k]\n";
    isa << "      MUL R8, " << iReg << ", " << nReg << "  ; R8 = i * N\n";
    isa << "      ADD R8, R8, " << kReg << "     ; R8 = i*N + k (offset for A[i][k])\n";
    isa << "      LOAD R11, A[R8]      ; R11 = A[i][k]\n\n";
    
    isa << "      ; Load B[k][j]\n";
    isa << "      MUL R9, " << kReg << ", " << nReg << "  ; R9 = k * N\n";
    isa << "      ADD R9, R9, " << jReg << "     ; R9 = k*N + j (offset for B[k][j])\n";
    isa << "      LOAD R12, B[R9]      ; R12 = B[k][j]\n\n";
    
    isa << "      ; Compute A[i][k] * B[k][j] and add to accumulator\n";
    isa << "      MUL R13, R11, R12    ; R13 = A[i][k] * B[k][j]\n";
    isa << "      ADD R10, R10, R13    ; Accumulate in R10 (for C[i][j])\n\n";
    
    isa << "      ; Increment k\n";
    isa << "      ADD " << kReg << ", " << kReg << ", 1\n";
    isa << "      JMP LOOP_K\n";
    isa << "    EXIT_K:           ; End of k loop\n\n";
    
    isa << "    ; Store accumulated value to C[i][j]\n";
    isa << "    MUL R14, " << iReg << ", " << nReg << "  ; R14 = i * N\n";
    isa << "    ADD R14, R14, " << jReg << "     ; R14 = i*N + j (offset for C[i][j])\n";
    isa << "    STORE C[R14], R10      ; C[i][j] = accumulated value\n\n";
    
    isa << "    ; Increment j\n";
    isa << "    ADD " << jReg << ", " << jReg << ", 1\n";
    isa << "    JMP LOOP_J\n";
    isa << "  EXIT_J:             ; End of j loop\n\n";
    
    isa << "  ; Increment i\n";
    isa << "  ADD " << iReg << ", " << iReg << ", 1\n";
    isa << "  JMP LOOP_I\n";
    isa << "EXIT_I:               ; End of i loop\n";
    
    return isa.str();
}