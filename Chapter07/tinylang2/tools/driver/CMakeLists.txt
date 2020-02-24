set(LLVM_LINK_COMPONENTS
  AllTargetsAsmParsers AllTargetsCodeGens
  AllTargetsDescs AllTargetsInfos
  AggressiveInstCombine Analysis AsmParser
  BitWriter CFGuard CodeGen Core Coroutines
  Extensions IPO IRReader IRPrinter InstCombine
  Instrumentation MC ObjCARCOpts Remarks
  ScalarOpts Support Target TargetParser
  TransformUtils Vectorize Passes
)

add_tinylang_tool(tinylang Driver.cpp)

target_link_libraries(tinylang
  PRIVATE tinylangBasic tinylangCodeGen
  tinylangLexer tinylangParser tinylangSema)
