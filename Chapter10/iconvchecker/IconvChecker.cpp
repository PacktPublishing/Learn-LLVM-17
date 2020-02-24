#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CallDescription.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CallEvent.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Frontend/CheckerRegistry.h"
#include <optional>

using namespace clang;
using namespace ento;

namespace {
class IconvState {
  const bool IsOpen;

  IconvState(bool IsOpen) : IsOpen(IsOpen) {}

public:
  bool isOpen() const { return IsOpen; }

  static IconvState getOpened() {
    return IconvState(true);
  }
  static IconvState getClosed() {
    return IconvState(false);
  }

  bool operator==(const IconvState &O) const {
    return IsOpen == O.IsOpen;
  }

  void Profile(llvm::FoldingSetNodeID &ID) const {
    ID.AddInteger(IsOpen);
  }
};
} // namespace

REGISTER_MAP_WITH_PROGRAMSTATE(IconvStateMap, SymbolRef,
                               IconvState)

namespace {
class IconvChecker
    : public Checker<check::PostCall, check::PreCall,
                     check::DeadSymbols,
                     check::PointerEscape> {
  CallDescription IconvOpenFn, IconvFn, IconvCloseFn;

  std::unique_ptr<BugType> DoubleCloseBugType;
  std::unique_ptr<BugType> LeakBugType;

  void report(ArrayRef<SymbolRef> Syms,
              const BugType &Bug, StringRef Desc,
              CheckerContext &C, ExplodedNode *ErrNode,
              std::optional<SourceRange> Range =
                  std::nullopt) const;

public:
  IconvChecker();
  void checkPostCall(const CallEvent &Call,
                     CheckerContext &C) const;
  void checkPreCall(const CallEvent &Call,
                    CheckerContext &C) const;
  void checkDeadSymbols(SymbolReaper &SymReaper,
                        CheckerContext &C) const;
  ProgramStateRef
  checkPointerEscape(ProgramStateRef State,
                     const InvalidatedSymbols &Escaped,
                     const CallEvent *Call,
                     PointerEscapeKind Kind) const;
};
} // namespace

IconvChecker::IconvChecker()
    : IconvOpenFn({"iconv_open"}), IconvFn({"iconv"}),
      IconvCloseFn({"iconv_close"}, 1) {
  DoubleCloseBugType.reset(new BugType(
      this, "Double iconv_close", "Iconv API Error"));

  LeakBugType.reset(new BugType(
      this, "Resource Leak", "Iconv API Error",
      /*SuppressOnSink=*/true));
}

void IconvChecker::checkPostCall(
    const CallEvent &Call, CheckerContext &C) const {
  if (!Call.isGlobalCFunction())
    return;
  if (!IconvOpenFn.matches(Call))
    return;
  if (SymbolRef Handle =
          Call.getReturnValue().getAsSymbol()) {
    ProgramStateRef State = C.getState();
    State = State->set<IconvStateMap>(
        Handle, IconvState::getOpened());
    C.addTransition(State);
  }
}

void IconvChecker::checkPreCall(
    const CallEvent &Call, CheckerContext &C) const {
  if (!Call.isGlobalCFunction()) {
    return;
  }
  if (!IconvCloseFn.matches(Call)) {
    return;
  }
  if (SymbolRef Handle =
          Call.getArgSVal(0).getAsSymbol()) {
    ProgramStateRef State = C.getState();
    if (const IconvState *St =
            State->get<IconvStateMap>(Handle)) {
      if (!St->isOpen()) {
        if (ExplodedNode *N = C.generateErrorNode()) {
          report(Handle, *DoubleCloseBugType,
                 "Closing a previous closed iconv "
                 "descriptor",
                 C, N, Call.getSourceRange());
        }
        return;
      }
    }

    State = State->set<IconvStateMap>(
        Handle, IconvState::getClosed());
    C.addTransition(State);
  }
}

void IconvChecker::checkDeadSymbols(
    SymbolReaper &SymReaper, CheckerContext &C) const {
  ProgramStateRef State = C.getState();
  SmallVector<SymbolRef, 8> LeakedSyms;
  for (auto [Sym, St] : State->get<IconvStateMap>()) {
    if (SymReaper.isDead(Sym)) {
      if (St.isOpen()) {
        bool IsLeaked = true;
        if (const llvm::APSInt *Val =
                State->getConstraintManager().getSymVal(
                    State, Sym))
          IsLeaked = Val->getExtValue() != -1;
        if (IsLeaked)
          LeakedSyms.push_back(Sym);
      }
      State = State->remove<IconvStateMap>(Sym);
    }
  }

  if (ExplodedNode *N =
          C.generateNonFatalErrorNode(State)) {
    report(LeakedSyms, *LeakBugType,
           "Opened iconv descriptor not closed", C, N);
  }
}

ProgramStateRef IconvChecker::checkPointerEscape(
    ProgramStateRef State,
    const InvalidatedSymbols &Escaped,
    const CallEvent *Call,
    PointerEscapeKind Kind) const {
  if (Kind == PSK_DirectEscapeOnCall) {
    if (IconvFn.matches(*Call) ||
        IconvCloseFn.matches(*Call))
      return State;
    if (Call->isInSystemHeader() ||
        !Call->argumentsMayEscape())
      return State;
  }

  for (SymbolRef Sym : Escaped)
    State = State->remove<IconvStateMap>(Sym);
  return State;
}

void IconvChecker::report(
    ArrayRef<SymbolRef> Syms, const BugType &Bug,
    StringRef Desc, CheckerContext &C,
    ExplodedNode *ErrNode,
    std::optional<SourceRange> Range) const {
  for (SymbolRef Sym : Syms) {
    auto R = std::make_unique<PathSensitiveBugReport>(
        Bug, Desc, ErrNode);
    R->markInteresting(Sym);
    if (Range)
      R->addRange(*Range);
    C.emitReport(std::move(R));
  }
}

extern "C" void
clang_registerCheckers(CheckerRegistry &registry) {
  registry.addChecker<IconvChecker>(
      "unix.IconvChecker",
      "Check handling of iconv functions", "");
}

extern "C" const char clang_analyzerAPIVersionString[] =
    CLANG_ANALYZER_API_VERSION_STRING;
