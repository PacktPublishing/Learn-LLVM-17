#include "CodeGen.h"
#include "Parser.h"
#include "Sema.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"
#include <iostream>

using namespace llvm;
using namespace llvm::orc;

ExitOnError ExitOnErr;

int main(int argc, const char **argv) {
  llvm::InitLLVM X(argc, argv);

  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  // Create a new LLJITBuilder.
  auto JIT = ExitOnErr(LLJITBuilder().create());
  // A map to keep track of the functions we've JIT'ed. The representation is
  // a mapping between the name of the user defined function (as a string), and
  // the name of the arguments (a vector of strings). All of these arguments
  // will automatically be represented as integers.
  StringMap<size_t> JITtedFunctions;

  while (true) {
    outs() << "JIT calc > ";
    std::string calcExp;
    std::getline(std::cin, calcExp);

    // Create a new context and module.
    std::unique_ptr<LLVMContext> Ctx = std::make_unique<LLVMContext>();
    std::unique_ptr<Module> M = std::make_unique<Module>("JIT calc.expr", *Ctx);
    M->setDataLayout(JIT->getDataLayout());

    // Declare the code generator.
    CodeGen CodeGenerator;

    Lexer Lex(calcExp);
    Token::TokenKind CalcTok = Lex.peek();
    if (CalcTok == Token::KW_def) {
      Parser Parser(Lex);
      AST *Tree = Parser.parse();
      if (!Tree || Parser.hasError()) {
        llvm::errs() << "Syntax errors occured\n";
        return 1;
      }
      Sema Semantic;
      if (Semantic.semantic(Tree, JITtedFunctions)) {
        llvm::errs() << "Semantic errors occured\n";
        return 1;
      }
      // Generate the IR.
      CodeGenerator.compileToIR(Tree, M.get(), JITtedFunctions);
      ExitOnErr(
          JIT->addIRModule(ThreadSafeModule(std::move(M), std::move(Ctx))));
    } else if (calcExp.find("quit") != std::string::npos) {
      outs() << "Quitting the JIT calc program.\n";
      break;
    } else if (CalcTok == Token::ident) {
      outs() << "Attempting to evaluate expression:\n";
      Parser Parser(Lex);
      AST *Tree = Parser.parse();
      if (!Tree || Parser.hasError()) {
        llvm::errs() << "Syntax errors occured\n";
        return 1;
      }
      Sema Semantic;
      if (Semantic.semantic(Tree, JITtedFunctions)) {
        llvm::errs() << "Semantic errors occured\n";
        return 1;
      }
      llvm::StringRef FuncCallName = Tree->getFnName();
      // Prepare the function call to the JITted (previously defined) function.
      CodeGenerator.prepareCalculationCallFunc(Tree, M.get(), FuncCallName,
                                               JITtedFunctions);
      // A resource tracker will track the memory that is allocated to the
      // JITted function that calls the user defined function.
      auto RT = JIT->getMainJITDylib().createResourceTracker();
      auto TSM = ThreadSafeModule(std::move(M), std::move(Ctx));
      ExitOnErr(JIT->addIRModule(RT, std::move(TSM)));
      // Generate the function call by querying the JIT for the temporary
      // call function (that will call the function defined by the user
      // to evaluate the calculator expression).
      auto CalcExprCall = ExitOnErr(JIT->lookup("calc_expr_func"));
      // Get the address of the symbol for "calc_expr_func" (the temporary call
      // function, and cast to the appropriate type. This function returns an
      // integer and takes no arguments. It simply produces a call to the
      // calculator function (previously) defined by the user.
      int (*UserFnCall)() = CalcExprCall.toPtr<int (*)()>();
      outs() << "User defined function evaluated to: " << UserFnCall() << "\n";
      // Free the memory that was previously allocated.
      ExitOnErr(RT->remove());
    }
  }

  return 0;
}
