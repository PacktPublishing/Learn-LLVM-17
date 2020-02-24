#include "TableGenBackends.h"
#include "llvm/Support/Format.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"
#include <algorithm>

using namespace llvm;

namespace {
class TokenAndKeywordFilterEmitter {
  RecordKeeper &Records;

public:
  explicit TokenAndKeywordFilterEmitter(RecordKeeper &R)
      : Records(R) {}

  void run(raw_ostream &OS);

private:
  void emitFlagsFragment(raw_ostream &OS);
  void emitTokenKind(raw_ostream &OS);
  void emitKeywordFilter(raw_ostream &OS);
};
} // End anonymous namespace

void TokenAndKeywordFilterEmitter::run(raw_ostream &OS) {
  // Emit Flag fragments.
  Records.startTimer("Emit flags");
  emitFlagsFragment(OS);

  // Emit token kind enum and functions.
  Records.startTimer("Emit token kind");
  emitTokenKind(OS);

  // Emit keyword filter code.
  Records.startTimer("Emit keyword filter");
  emitKeywordFilter(OS);
  Records.stopTimer();
}

void TokenAndKeywordFilterEmitter::emitFlagsFragment(
    raw_ostream &OS) {
  OS << "#ifdef GET_TOKEN_FLAGS\n";
  OS << "#undef GET_TOKEN_FLAGS\n";
  for (Record *CC :
       Records.getAllDerivedDefinitions("Flag")) {
    StringRef Name = CC->getValueAsString("Name");
    int64_t Val = CC->getValueAsInt("Val");
    OS << Name << " = " << format_hex(Val, 2) << ",\n";
  }
  OS << "#endif\n";
}

void TokenAndKeywordFilterEmitter::emitTokenKind(
    raw_ostream &OS) {
  OS << "#ifdef GET_TOKEN_KIND_DECLARATION\n"
     << "#undef GET_TOKEN_KIND_DECLARATION\n"
     << "namespace tok {\n"
     << "  enum TokenKind : unsigned short {\n";
  for (Record *CC :
       Records.getAllDerivedDefinitions("Token")) {
    StringRef Name = CC->getValueAsString("Name");
    OS << "    ";
    if (CC->isSubClassOf("Keyword"))
      OS << "kw_";
    OS << Name << ",\n";
  }
  OS << "    NUM_TOKENS\n"
     << "  };\n";
  OS << "  const char *getTokenName(TokenKind Kind) "
        "LLVM_READNONE;\n"
     << "  const char *getPunctuatorSpelling(TokenKind "
        "Kind) LLVM_READNONE;\n"
     << "  const char *getKeywordSpelling(TokenKind "
        "Kind) "
        "LLVM_READNONE;\n"
     << "}\n"
     << "#endif\n";
  OS << "#ifdef GET_TOKEN_KIND_DEFINITION\n";
  OS << "#undef GET_TOKEN_KIND_DEFINITION\n";
  OS << "static const char * const TokNames[] = {\n";
  for (Record *CC :
       Records.getAllDerivedDefinitions("Token")) {
    OS << "  \"" << CC->getValueAsString("Name")
       << "\",\n";
  }
  OS << "};\n\n";
  OS << "const char *tok::getTokenName(TokenKind Kind) "
        "{\n"
     << "  if (Kind <= tok::NUM_TOKENS)\n"
     << "    return TokNames[Kind];\n"
     << "  llvm_unreachable(\"unknown TokenKind\");\n"
     << "  return nullptr;\n"
     << "};\n\n";
  OS << "const char "
        "*tok::getPunctuatorSpelling(TokenKind "
        "Kind) {\n"
     << "  switch (Kind) {\n";
  for (Record *CC :
       Records.getAllDerivedDefinitions("Punctuator")) {
    OS << "    " << CC->getValueAsString("Name")
       << ": return \""
       << CC->getValueAsString("Spelling") << "\";\n";
  }
  OS << "    default: break;\n"
     << "  }\n"
     << "  return nullptr;\n"
     << "};\n\n";
  OS << "const char *tok::getKeywordSpelling(TokenKind "
        "Kind) {\n"
     << "  switch (Kind) {\n";
  for (Record *CC :
       Records.getAllDerivedDefinitions("Keyword")) {
    OS << "    kw_" << CC->getValueAsString("Name")
       << ": return \"" << CC->getValueAsString("Name")
       << "\";\n";
  }
  OS << "    default: break;\n"
     << "  }\n"
     << "  return nullptr;\n"
     << "};\n\n";
  OS << "#endif\n";
}

void TokenAndKeywordFilterEmitter::emitKeywordFilter(
    raw_ostream &OS) {
  // Simplification: assume only one TokenFilter is
  // defined
  std::vector<Record *> AllTokenFilter =
      Records.getAllDerivedDefinitionsIfDefined(
          "TokenFilter");
  if (AllTokenFilter.empty())
    return;
  ListInit *TokenFilter = dyn_cast_or_null<ListInit>(
      AllTokenFilter[0]
          ->getValue("Tokens")
          ->getValue());
  if (!TokenFilter)
    return;

  // Collect the keyword/flag values.
  using KeyFlag = std::pair<StringRef, uint64_t>;
  std::vector<KeyFlag> Table;
  for (size_t I = 0, E = TokenFilter->size(); I < E;
       ++I) {
   Record *CC = TokenFilter->getElementAsRecord(I);
   StringRef Name = CC->getValueAsString("Name");
   uint64_t Val = 0;
   ListInit *Flags = nullptr;
   if (RecordVal *F = CC->getValue("Flags"))
      Flags = dyn_cast_or_null<ListInit>(F->getValue());
   if (Flags) {
      for (size_t I = 0, E = Flags->size(); I < E; ++I) {
        Val |=
            Flags->getElementAsRecord(I)->getValueAsInt(
                "Val");
      }
   }
   Table.emplace_back(Name, Val);
  }
  llvm::sort(Table.begin(), Table.end(),
             [](const KeyFlag A, const KeyFlag B) {
               return A.first < B.first;
             });
  OS << "#ifdef GET_KEYWORD_FILTER\n"
     << "#undef GET_KEYWORD_FILTER\n";
  OS << "bool lookupKeyword(llvm::StringRef Keyword, "
        "unsigned &Value) {\n";
  OS << "  struct Entry {\n"
     << "    unsigned Value;\n"
     << "    llvm::StringRef Keyword;\n"
     << "  };\n"
     << "static const Entry Table[" << Table.size()
     << "] = {\n";
  for (const auto &[Keyword, Value] : Table) {
   OS << "    { " << Value << ", llvm::StringRef(\""
      << Keyword << "\", " << Keyword.size()
      << ") },\n";
  }
  OS << "  };\n\n";
  OS << "  const Entry *E = "
        "std::lower_bound(&Table[0], "
        "&Table["
     << Table.size()
     << "], Keyword, [](const Entry &A, const "
        "StringRef "
        "&B) {\n";
  OS << "    return A.Keyword < B;\n";
  OS << "  });\n";
  OS << "  if (E != &Table[" << Table.size()
     << "]) {\n";
  OS << "    Value = E->Value;\n";
  OS << "    return true;\n";
  OS << "  }\n";
  OS << "  return false;\n";
  OS << "}\n";

  // Add some test code. This is optional.
  OS << "\n#ifdef SELFCHECK\n";
  OS << "void selfcheck() {\n";
  OS << "  auto Check = [](StringRef Key, unsigned "
        "ExpVal) "
        "{\n";
  OS << "    unsigned Val;\n";
  OS << "    if (!lookupKeyword(Key, Val)) { "
        "llvm::dbgs() "
        "<< \"Failed to find: "
        "\" << Key << \"\\n\"; exit(1); }\n";
  OS << "    if (Val != ExpVal) { llvm::dbgs() << Key "
        "<< "
        "\"Wrong value\\n\"; "
        "exit(1); }\n";
  OS << "  };\n";
  for (auto &[Key, Val] : Table) {
   OS << "  Check(\"" << Key << "\", " << Val << ");\n";
  }
  OS << "  llvm::dbgs() << \"Selfcheck done\\n\";\n";
  OS << "}\n";
  OS << "\n#endif // SELFCHECK\n";

  OS << "#endif\n";
}

void EmitTokensAndKeywordFilter(RecordKeeper &RK,
                                raw_ostream &OS) {
  emitSourceFileHeader("Token Kind and Keyword Filter "
                       "Implementation Fragment",
                       OS);
  TokenAndKeywordFilterEmitter(RK).run(OS);
}
