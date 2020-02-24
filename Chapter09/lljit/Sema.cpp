#include "Sema.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

namespace {
class DeclCheck : public ASTVisitor {
  llvm::StringSet<> Scope;
  bool HasError;

  StringMap<size_t> &JITtedFunctionsMap;

  enum ErrorType { Twice, Not };

  void error(ErrorType ET, llvm::StringRef V) {
    llvm::errs() << "Variable " << V << " "
                 << (ET == Twice ? "already" : "not")
                 << " declared\n";
    HasError = true;
  }

public:
  DeclCheck(StringMap<size_t> &JITtedFunctions)
      : JITtedFunctionsMap(JITtedFunctions), HasError(false) {}

  bool hasError() { return HasError; }

  virtual void visit(Factor &Node) override {
    if (Node.getKind() == Factor::Ident) {
      if (Scope.find(Node.getVal()) == Scope.end())
        error(Not, Node.getVal());
    }
  };

  virtual void visit(BinaryOp &Node) override {
    if (Node.getLeft())
      Node.getLeft()->accept(*this);
    else
      HasError = true;
    if (Node.getRight())
      Node.getRight()->accept(*this);
    else
      HasError = true;
  };

  virtual void visit(DefDecl &Node) override {
    for (auto I = Node.begin(), E = Node.end(); I != E;
         ++I) {
      if (!Scope.insert(*I).second)
        error(Twice, *I);
    }
    if (Node.getExpr())
      Node.getExpr()->accept(*this);
    else
      HasError = true;
  };

  virtual void visit(FuncCallFromDef &Node) override {
    llvm::StringRef FuncCallName = Node.getFnName();
    unsigned NumArgs = Node.getArgs().size();
    unsigned NumOriginallyDefinedArgs = 0;
    // Check to ensure that the function specified by the user was
    // previously defined.
    auto LookUpFunctionCall = JITtedFunctionsMap.find(FuncCallName);
    if (LookUpFunctionCall != JITtedFunctionsMap.end())
      NumOriginallyDefinedArgs = LookUpFunctionCall->second;
    else {
      llvm::errs() << "Specified function definition does not exist!\n";
      HasError = true;
    }
    if (NumArgs != NumOriginallyDefinedArgs) {
      llvm::errs() << "Number of parameters specified to the function does not "
                   << "match it's definition!\n";
      HasError = true;
    }
  };
};
}

bool Sema::semantic(AST *Tree, StringMap<size_t> &JITtedFunctionsMap) {
  if (!Tree)
    return false;
  DeclCheck Check(JITtedFunctionsMap);
  Tree->accept(Check);
  return Check.hasError();
}
