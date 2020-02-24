#ifndef TABLEGENBACKENDS_H
#define TABLEGENBACKENDS_H

namespace llvm {
class raw_ostream;
class RecordKeeper;
} // namespace llvm

void EmitTokensAndKeywordFilter(llvm::RecordKeeper &RK,
                                llvm::raw_ostream &OS);

#endif
