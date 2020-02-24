#ifndef AST_H
#define AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

class AST;
class Expr;
class Factor;
class BinaryOp;
class DefDecl;
class FuncCallFromDef;

class ASTVisitor {
public:
  virtual void visit(AST &){};
  virtual void visit(Expr &){};
  virtual void visit(Factor &) = 0;
  virtual void visit(BinaryOp &) = 0;
  virtual void visit(DefDecl &) = 0;
  virtual void visit(FuncCallFromDef &) = 0;
};

class AST {
public:
  virtual ~AST() {}
  virtual void accept(ASTVisitor &V) = 0;
  virtual llvm::StringRef getFnName() const {
    return "main";
  }
};

class Expr : public AST {
public:
  Expr() {}
};

class Factor : public Expr {
public:
  enum ValueKind { Ident, Number };

private:
  ValueKind Kind;
  llvm::StringRef Val;

public:
  Factor(ValueKind Kind, llvm::StringRef Val)
      : Kind(Kind), Val(Val) {}
  ValueKind getKind() { return Kind; }
  llvm::StringRef getVal() { return Val; }
  virtual void accept(ASTVisitor &V) override {
    V.visit(*this);
  }
};

class BinaryOp : public Expr {
public:
  enum Operator { Plus, Minus, Mul, Div };

private:
  Expr *Left;
  Expr *Right;
  Operator Op;

public:
  BinaryOp(Operator Op, Expr *L, Expr *R)
      : Op(Op), Left(L), Right(R) {}
  Expr *getLeft() { return Left; }
  Expr *getRight() { return Right; }
  Operator getOperator() { return Op; }
  virtual void accept(ASTVisitor &V) override {
    V.visit(*this);
  }
};

class DefDecl : public AST {
  llvm::StringRef DefFnName;
  using VarVector = llvm::SmallVector<llvm::StringRef, 8>;
  VarVector Vars;
  Expr *E;

public:
  DefDecl(llvm::StringRef DefFnName, llvm::SmallVector<llvm::StringRef, 8> Vars,
           Expr *E)
      : DefFnName(DefFnName), Vars(Vars), E(E) {}
  VarVector::const_iterator begin() { return Vars.begin(); }
  VarVector::const_iterator end() { return Vars.end(); }
  Expr *getExpr() { return E; }
  virtual VarVector getVars() const { return Vars; }
  virtual void accept(ASTVisitor &V) override {
    V.visit(*this);
  }
  virtual llvm::StringRef getFnName() const override {
    return DefFnName;
  }
};

class FuncCallFromDef : public AST {
  llvm::StringRef DefFnName;
  using ArgVector = llvm::SmallVector<llvm::StringRef, 8>;
  ArgVector Args;

public:
  FuncCallFromDef(llvm::StringRef DefFnName,
                  llvm::SmallVector<llvm::StringRef, 8> Args)
      : DefFnName(DefFnName), Args(Args) {}
  ArgVector::const_iterator begin() { return Args.begin(); }
  ArgVector::const_iterator end() { return Args.end(); }
  virtual ArgVector getArgs() const { return Args; }
  virtual void accept(ASTVisitor &V) override {
    V.visit(*this);
  }
  virtual llvm::StringRef getFnName() const override {
    return DefFnName;
  }
};
#endif
