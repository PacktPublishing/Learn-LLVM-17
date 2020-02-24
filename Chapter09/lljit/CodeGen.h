#ifndef CODEGEN_H
#define CODEGEN_H

#include "AST.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/LLVMContext.h"

using namespace llvm;

class CodeGen {
public:
  void compileToIR(
      AST *Tree, Module *M, StringMap<size_t> &JITtedFunctions);
  void prepareCalculationCallFunc(
      AST *FuncCall, Module *M, llvm::StringRef FnName,
      StringMap<size_t> &JITtedFunctions);
};
#endif
