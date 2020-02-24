#include "Parser.h"

AST *Parser::parse() {
  AST *Res = parseCalc();
  expect(Token::eoi);
  return Res;
}

AST *Parser::parseCalc() {
  Expr *E;
  llvm::SmallVector<llvm::StringRef, 8> Vars;
  llvm::StringRef DefFnName;
  if (Tok.is(Token::KW_def)) {
    advance();
    if (expect(Token::ident))
      goto _error;
    DefFnName = Tok.getText();
    advance();
    if (consume(Token::l_paren))
      goto _error;
    if (expect(Token::ident))
      goto _error;
    Vars.push_back(Tok.getText());
    advance();
    while (Tok.is(Token::comma)) {
      advance();
      if (expect(Token::ident))
        goto _error;
      Vars.push_back(Tok.getText());
      advance();
    }
    if (consume(Token::r_paren))
      goto _error;
    if (consume(Token::equals))
      goto _error;
    E = parseExpr();
    if (expect(Token::eoi))
      goto _error;
    if (Vars.empty())
      return E;
    else
      return new DefDecl(DefFnName, Vars, E);
  } else if (Tok.is(Token::ident)) {
    DefFnName = Tok.getText();
    advance();
    if (consume(Token::l_paren))
      goto _error;
    if (expect(Token::number))
      goto _error;
    Vars.push_back(Tok.getText());
    advance();
    while (Tok.is(Token::comma)) {
      advance();
      if (expect(Token::number))
        goto _error;
      Vars.push_back(Tok.getText());
      advance(); }
    if (consume(Token::r_paren))
      goto _error;
    if (expect(Token::eoi))
      goto _error;
    return new FuncCallFromDef(DefFnName, Vars);
  } else
    llvm::errs() << "Expect function definition or call!\n";
_error:
  while (Tok.getKind() != Token::eoi)
    advance();
  return nullptr;
}

Expr *Parser::parseExpr() {
  Expr *Left = parseTerm();
  while (Tok.isOneOf(Token::plus, Token::minus)) {
    BinaryOp::Operator Op = Tok.is(Token::plus)
                                ? BinaryOp::Plus
                                : BinaryOp::Minus;
    advance();
    Expr *Right = parseTerm();
    Left = new BinaryOp(Op, Left, Right);
  }
  return Left;
}

Expr *Parser::parseTerm() {
  Expr *Left = parseFactor();
  while (Tok.isOneOf(Token::star, Token::slash)) {
    BinaryOp::Operator Op =
        Tok.is(Token::star) ? BinaryOp::Mul : BinaryOp::Div;
    advance();
    Expr *Right = parseFactor();
    Left = new BinaryOp(Op, Left, Right);
  }
  return Left;
}

Expr *Parser::parseFactor() {
  Expr *Res = nullptr;
  switch (Tok.getKind()) {
  case Token::number:
    Res = new Factor(Factor::Number, Tok.getText());
    advance(); break;
  case Token::ident:
    Res = new Factor(Factor::Ident, Tok.getText());
    advance(); break;
  case Token::l_paren:
    advance();
    Res = parseExpr();
    if (!consume(Token::r_paren)) break;
  case Token::r_paren:
    advance(); break;
  default:
    if (!Res)
      error();
    while (!Tok.isOneOf(Token::r_paren, Token::star,
                        Token::plus, Token::minus,
                        Token::slash, Token::eoi))
      advance();
  }
  return Res;
}
