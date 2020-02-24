#ifndef SEMA_H
#define SEMA_H

#include "AST.h"
#include "Lexer.h"
#include "llvm/ADT/StringMap.h"

using namespace llvm;

class Sema {
public:
  bool semantic(AST *Tree, StringMap<size_t> &JITtedFunctions);
};

#endif
