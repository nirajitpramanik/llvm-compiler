#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <memory>
#include "isa_encoder.h"
#include "memory_mapper.h"

using namespace llvm;

int main() {
    LLVMContext Context;
    auto ModuleOb = std::make_unique<Module>("PIM_Compiler", Context);
    IRBuilder<> Builder(Context);

    // Function Signature: void matmul(int** A, int** B, int** C, int N)
    std::vector<Type *> Params = {
        PointerType::get(PointerType::get(Type::getInt32Ty(Context), 0), 0), // A
        PointerType::get(PointerType::get(Type::getInt32Ty(Context), 0), 0), // B
        PointerType::get(PointerType::get(Type::getInt32Ty(Context), 0), 0), // C
        Type::getInt32Ty(Context) // N
    };
    FunctionType *funcType = FunctionType::get(Type::getVoidTy(Context), Params, false);
    Function *matmulFunc = Function::Create(funcType, Function::ExternalLinkage, "matmul", ModuleOb.get());

    // Name function arguments
    auto args = matmulFunc->args().begin();
    Value *A = args++;
    Value *B = args++;
    Value *C = args++;
    Value *N = args;
    A->setName("A");
    B->setName("B");
    C->setName("C");
    N->setName("N");

    // Entry block
    BasicBlock *entry = BasicBlock::Create(Context, "entry", matmulFunc);
    Builder.SetInsertPoint(entry);

    // Allocate loop variables
    Value *i = Builder.CreateAlloca(Type::getInt32Ty(Context), nullptr, "i");
    Value *j = Builder.CreateAlloca(Type::getInt32Ty(Context), nullptr, "j");
    Value *k = Builder.CreateAlloca(Type::getInt32Ty(Context), nullptr, "k");

    // Initialize i = 0
    Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Context), 0), i);

    // Create all basic blocks first
    BasicBlock *loop_i_cond = BasicBlock::Create(Context, "loop_i_cond", matmulFunc);
    BasicBlock *loop_i_body = BasicBlock::Create(Context, "loop_i_body", matmulFunc);
    BasicBlock *loop_j_cond = BasicBlock::Create(Context, "loop_j_cond", matmulFunc);
    BasicBlock *loop_j_body = BasicBlock::Create(Context, "loop_j_body", matmulFunc);
    BasicBlock *loop_k_cond = BasicBlock::Create(Context, "loop_k_cond", matmulFunc);
    BasicBlock *loop_k_body = BasicBlock::Create(Context, "loop_k_body", matmulFunc);
    BasicBlock *loop_j_inc = BasicBlock::Create(Context, "loop_j_inc", matmulFunc);
    BasicBlock *loop_i_inc = BasicBlock::Create(Context, "loop_i_inc", matmulFunc);
    BasicBlock *exitBlock = BasicBlock::Create(Context, "exit", matmulFunc);

    // Entry to first loop condition
    Builder.CreateBr(loop_i_cond);

    // i loop condition
    Builder.SetInsertPoint(loop_i_cond);
    Value *i_val = Builder.CreateLoad(Type::getInt32Ty(Context), i, "i_val");
    Value *cond_i = Builder.CreateICmpSLT(i_val, N, "cond_i");
    Builder.CreateCondBr(cond_i, loop_i_body, exitBlock);

    // i loop body
    Builder.SetInsertPoint(loop_i_body);
    // Initialize j = 0
    Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Context), 0), j);
    Builder.CreateBr(loop_j_cond);

    // j loop condition
    Builder.SetInsertPoint(loop_j_cond);
    Value *j_val = Builder.CreateLoad(Type::getInt32Ty(Context), j, "j_val");
    Value *cond_j = Builder.CreateICmpSLT(j_val, N, "cond_j");
    Builder.CreateCondBr(cond_j, loop_j_body, loop_i_inc);

    // j loop body
    Builder.SetInsertPoint(loop_j_body);
    // Initialize k = 0
    Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Context), 0), k);
    
    // Initialize C[i][j] = 0 only if this is the first iteration of k loop
    Value *C_i = Builder.CreateLoad(PointerType::get(Type::getInt32Ty(Context), 0), 
                                  Builder.CreateGEP(PointerType::get(Type::getInt32Ty(Context), 0), C, {i_val}), "C_i");
    Builder.CreateStore(ConstantInt::get(Type::getInt32Ty(Context), 0), 
                       Builder.CreateGEP(Type::getInt32Ty(Context), C_i, {j_val}));
    
    Builder.CreateBr(loop_k_cond);

    // k loop condition
    Builder.SetInsertPoint(loop_k_cond);
    Value *k_val = Builder.CreateLoad(Type::getInt32Ty(Context), k, "k_val");
    Value *cond_k = Builder.CreateICmpSLT(k_val, N, "cond_k");
    Builder.CreateCondBr(cond_k, loop_k_body, loop_j_inc);

    // k loop body - actual computation
    Builder.SetInsertPoint(loop_k_body);
    // Load A[i][k]
    Value *A_i = Builder.CreateLoad(PointerType::get(Type::getInt32Ty(Context), 0), 
                                  Builder.CreateGEP(PointerType::get(Type::getInt32Ty(Context), 0), A, {i_val}), "A_i");
    Value *A_ik = Builder.CreateLoad(Type::getInt32Ty(Context), 
                                   Builder.CreateGEP(Type::getInt32Ty(Context), A_i, {k_val}), "A_ik");
    
    // Load B[k][j]
    Value *B_k = Builder.CreateLoad(PointerType::get(Type::getInt32Ty(Context), 0), 
                                  Builder.CreateGEP(PointerType::get(Type::getInt32Ty(Context), 0), B, {k_val}), "B_k");
    Value *B_kj = Builder.CreateLoad(Type::getInt32Ty(Context), 
                                   Builder.CreateGEP(Type::getInt32Ty(Context), B_k, {j_val}), "B_kj");
    
    // Load C[i][j]
    C_i = Builder.CreateLoad(PointerType::get(Type::getInt32Ty(Context), 0), 
                           Builder.CreateGEP(PointerType::get(Type::getInt32Ty(Context), 0), C, {i_val}), "C_i");
    Value *C_ij = Builder.CreateLoad(Type::getInt32Ty(Context), 
                                   Builder.CreateGEP(Type::getInt32Ty(Context), C_i, {j_val}), "C_ij");
    
    // Multiply and add
    Value *MulRes = Builder.CreateMul(A_ik, B_kj, "mul_res");
    Value *NewC = Builder.CreateAdd(C_ij, MulRes, "new_c");
    
    // Store back to C[i][j]
    Builder.CreateStore(NewC, Builder.CreateGEP(Type::getInt32Ty(Context), C_i, {j_val}));
    
    // k increment
    Value *next_k = Builder.CreateAdd(k_val, ConstantInt::get(Type::getInt32Ty(Context), 1), "next_k");
    Builder.CreateStore(next_k, k);
    Builder.CreateBr(loop_k_cond);

    // j increment
    Builder.SetInsertPoint(loop_j_inc);
    Value *next_j = Builder.CreateAdd(j_val, ConstantInt::get(Type::getInt32Ty(Context), 1), "next_j");
    Builder.CreateStore(next_j, j);
    Builder.CreateBr(loop_j_cond);

    // i increment
    Builder.SetInsertPoint(loop_i_inc);
    Value *next_i = Builder.CreateAdd(i_val, ConstantInt::get(Type::getInt32Ty(Context), 1), "next_i");
    Builder.CreateStore(next_i, i);
    Builder.CreateBr(loop_i_cond);

    // Exit block
    Builder.SetInsertPoint(exitBlock);
    Builder.CreateRetVoid();

    // Verify function
    if (verifyFunction(*matmulFunc, &errs())) {
        std::cerr << "Error: Function verification failed!\n";
        return 1;
    }

    // Print the LLVM IR
    std::cout << "Generated LLVM IR:\n";
    ModuleOb->print(outs(), nullptr);
    std::cout << "\n";

    // Generate the ISA code
    std::string isa_code = encodeISA(*matmulFunc);
    std::cout << "Generated ISA Code:\n" << isa_code << std::endl;

    return 0;
}