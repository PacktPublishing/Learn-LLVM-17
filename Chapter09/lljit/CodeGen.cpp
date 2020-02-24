#include "CodeGen.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>

using namespace llvm;

namespace {
class ToIRVisitor : public ASTVisitor {
  Module *M;
  IRBuilder<> Builder;
  StringMap<size_t> &JITtedFunctionsMap;
  Type *Int32Ty;

  Value *V;
  StringMap<Value *> nameMap;

public:
  ToIRVisitor(Module *M,
              StringMap<size_t> &JITtedFunctions)
      : M(M), Builder(M->getContext()), JITtedFunctionsMap(JITtedFunctions) {
    Int32Ty = Type::getInt32Ty(M->getContext());
  }

  void genFuncEvaluationCall(AST *Tree) { Tree->accept(*this); }

  void run(AST *Tree) {
    Tree->accept(*this);
    Builder.CreateRet(V);
  }

  Function *genUserDefinedFunction(llvm::StringRef FnName) {
    // If a function from the module with the user defined name exists,
    // let's use that function.
    if (Function *FuncFromModule = M->getFunction(FnName))
      return FuncFromModule;

    // Otherwise, let's create the user defined function based off the
    // function name and stored arguments.
    Function *UserDefinedFunction = nullptr;
    auto FnNameToArgCount = JITtedFunctionsMap.find(FnName);
    if (FnNameToArgCount != JITtedFunctionsMap.end()) {
      std::vector<Type *> IntArgs(FnNameToArgCount->second, Int32Ty);
      FunctionType *FuncType = FunctionType::get(Int32Ty, IntArgs, false);
      UserDefinedFunction =
          Function::Create(FuncType, GlobalValue::ExternalLinkage, FnName, M);
    }

    return UserDefinedFunction;
  }

  virtual void visit(Factor &Node) override {
    if (Node.getKind() == Factor::Ident) {
      V = nameMap[Node.getVal()];
    } else {
      int intval;
      Node.getVal().getAsInteger(10, intval);
      V = ConstantInt::get(Int32Ty, intval, true);
    }
  };

  virtual void visit(BinaryOp &Node) override {
    Node.getLeft()->accept(*this);
    Value *Left = V;
    Node.getRight()->accept(*this);
    Value *Right = V;
    switch (Node.getOperator()) {
    case BinaryOp::Plus:
      V = Builder.CreateNSWAdd(Left, Right);
      break;
    case BinaryOp::Minus:
      V = Builder.CreateNSWSub(Left, Right);
      break;
    case BinaryOp::Mul:
      V = Builder.CreateNSWMul(Left, Right);
      break;
    case BinaryOp::Div:
      V = Builder.CreateSDiv(Left, Right);
      break;
    }
  };

  virtual void visit(DefDecl &Node) override {
    llvm::StringRef FnName = Node.getFnName();
    llvm::SmallVector<llvm::StringRef, 8> FunctionVars = Node.getVars();
    // Add the function name and arguments to the list of JIT'd functions.
    (JITtedFunctionsMap)[FnName] = FunctionVars.size();

    // Create the actual user defined function.
    Function *DefFunc = genUserDefinedFunction(FnName);
    if (!DefFunc) {
      llvm::errs() << "Error occurred when generating user defined function!\n";
      return;
    }

    // Set up a new basic block to insert the user defined function into.
    BasicBlock *BB = BasicBlock::Create(M->getContext(), "entry", DefFunc);
    Builder.SetInsertPoint(BB);

    // Set the names for all function arguments.
    unsigned FIdx = 0;
    for (auto &FArg : DefFunc->args()) {
      nameMap[FunctionVars[FIdx]] = &FArg;
      FArg.setName(FunctionVars[FIdx++]);
    }

    // Generate binary operations on arguments.
    Node.getExpr()->accept(*this);
  };

  virtual void visit(FuncCallFromDef &Node) override {
    llvm::StringRef CalcExprFunName = "calc_expr_func";
    // Prepare the function signature for the temporary function that will
    // call the user defined function.
    FunctionType *CalcExprFunTy = FunctionType::get(Int32Ty, {}, false);
    Function *CalcExprFun = Function::Create(
        CalcExprFunTy, GlobalValue::ExternalLinkage, CalcExprFunName, M);

    BasicBlock *BB = BasicBlock::Create(M->getContext(), "entry", CalcExprFun);
    Builder.SetInsertPoint(BB);

    // Get the original function definition.
    llvm::StringRef CalleeFnName = Node.getFnName();
    Function *CalleeFn = genUserDefinedFunction(CalleeFnName);
    if (!CalleeFn) {
      llvm::errs() << "Cannot retrieve the original callee function!\n";
      return;
    }

    // Set the parameters for the function call.
    auto CalleeFnVars = Node.getArgs();
    llvm::SmallVector<Value *> IntParams;
    for (unsigned i = 0, end = CalleeFnVars.size(); i != end; ++i) {
      int ArgsToIntType;
      CalleeFnVars[i].getAsInteger(10, ArgsToIntType);
      Value *IntParam = ConstantInt::get(Int32Ty, ArgsToIntType, true);
      IntParams.push_back(IntParam);
    }

    Value *Res = Builder.CreateCall(CalleeFn, IntParams, "calc_expr_res");
    Builder.CreateRet(Res);
  };
};
} // namespace

void CodeGen::compileToIR(AST *Tree, Module *M,
                          StringMap<size_t> &JITtedFunctions) {
  ToIRVisitor ToIR(M, JITtedFunctions);
  ToIR.run(Tree);
  M->print(outs(), nullptr);
}

void CodeGen::prepareCalculationCallFunc(AST *FuncCall, Module *M,
                                         llvm::StringRef FnName,
                                         StringMap<size_t> &JITtedFunctions) {
  ToIRVisitor ToIR(M, JITtedFunctions);
  ToIR.genFuncEvaluationCall(FuncCall);
  M->print(outs(), nullptr);
}
