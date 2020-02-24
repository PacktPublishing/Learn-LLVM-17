#include "tinylang/Parser/Parser.h"
#include "tinylang/Basic/TokenKinds.h"

using namespace tinylang;

namespace {
OperatorInfo fromTok(Token Tok) {
  return OperatorInfo(Tok.getLocation(), Tok.getKind());
}
} // namespace

Parser::Parser(Lexer &Lex, Sema &Actions)
    : Lex(Lex), Actions(Actions) {
  advance();
}

ModuleDeclaration *Parser::parse() {
  ModuleDeclaration *ModDecl = nullptr;
  parseCompilationUnit(ModDecl);
  return ModDecl;
}

bool Parser::parseCompilationUnit(
    ModuleDeclaration *&D) {
  auto _errorhandler = [this] { return skipUntil(); };
  if (consume(tok::kw_MODULE))
    return _errorhandler();
  if (expect(tok::identifier))
    return _errorhandler();
  D = Actions.actOnModuleDeclaration(
      Tok.getLocation(), Tok.getIdentifier());

  EnterDeclScope S(Actions, D);
  advance();
  if (consume(tok::semi))
    return _errorhandler();
  while (Tok.isOneOf(tok::kw_FROM, tok::kw_IMPORT)) {
    if (parseImport())
      return _errorhandler();
  }
  DeclList Decls;
  StmtList Stmts;
  if (parseBlock(Decls, Stmts))
    return _errorhandler();
  if (expect(tok::identifier))
    return _errorhandler();
  Actions.actOnModuleDeclaration(D, Tok.getLocation(),
                                 Tok.getIdentifier(),
                                 Decls, Stmts);
  advance();
  if (consume(tok::period))
    return _errorhandler();
  return false;
}

bool Parser::parseImport() {
  auto _errorhandler = [this] {
    return skipUntil(tok::kw_BEGIN, tok::kw_CONST,
                     tok::kw_END, tok::kw_FROM,
                     tok::kw_IMPORT, tok::kw_PROCEDURE,
                     tok::kw_TYPE, tok::kw_VAR);
  };
  IdentList Ids;
  StringRef ModuleName;
  if (Tok.is(tok::kw_FROM)) {
    advance();
    if (expect(tok::identifier))
      return _errorhandler();
    ModuleName = Tok.getIdentifier();
    advance();
  }
  if (consume(tok::kw_IMPORT))
    return _errorhandler();
  if (parseIdentList(Ids))
    return _errorhandler();
  if (expect(tok::semi))
    return _errorhandler();
  Actions.actOnImport(ModuleName, Ids);
  advance();
  return false;
}

bool Parser::parseBlock(DeclList &Decls,
                        StmtList &Stmts) {
  auto _errorhandler = [this] {
    return skipUntil(tok::identifier);
  };
  while (Tok.isOneOf(tok::kw_CONST, tok::kw_PROCEDURE,
                     tok::kw_TYPE, tok::kw_VAR)) {
    if (parseDeclaration(Decls))
      return _errorhandler();
  }
  if (Tok.is(tok::kw_BEGIN)) {
    advance();
    if (parseStatementSequence(Stmts))
      return _errorhandler();
  }
  if (consume(tok::kw_END))
    return _errorhandler();
  return false;
}

bool Parser::parseDeclaration(DeclList &Decls) {
  auto _errorhandler = [this] {
    return skipUntil(tok::kw_BEGIN, tok::kw_CONST,
                     tok::kw_END, tok::kw_PROCEDURE,
                     tok::kw_TYPE, tok::kw_VAR);
  };
  if (Tok.is(tok::kw_CONST)) {
    advance();
    while (Tok.is(tok::identifier)) {
      if (parseConstantDeclaration(Decls))
        return _errorhandler();
      if (consume(tok::semi))
        return _errorhandler();
    }
  } else if (Tok.is(tok::kw_TYPE)) {
    advance();
    while (Tok.is(tok::identifier)) {
      if (parseTypeDeclaration(Decls))
        return _errorhandler();
      if (consume(tok::semi))
        return _errorhandler();
    }
  } else if (Tok.is(tok::kw_VAR)) {
    advance();
    while (Tok.is(tok::identifier)) {
      if (parseVariableDeclaration(Decls))
        return _errorhandler();
      if (consume(tok::semi))
        return _errorhandler();
    }
  } else if (Tok.is(tok::kw_PROCEDURE)) {
    if (parseProcedureDeclaration(Decls))
      return _errorhandler();
    if (consume(tok::semi))
      return _errorhandler();
  } else {
    /*ERROR*/
    return _errorhandler();
  }
  return false;
}

bool Parser::parseConstantDeclaration(DeclList &Decls) {
  auto _errorhandler = [this] {
    return skipUntil(tok::semi);
  };
  if (expect(tok::identifier))
    return _errorhandler();
  SMLoc Loc = Tok.getLocation();

  StringRef Name = Tok.getIdentifier();
  advance();
  if (expect(tok::equal))
    return _errorhandler();
  Expr *E = nullptr;
  advance();
  if (parseExpression(E))
    return _errorhandler();
  Actions.actOnConstantDeclaration(Decls, Loc, Name, E);
  return false;
}

bool Parser::parseTypeDeclaration(DeclList &Decls) {
  auto _errorhandler = [this] {
    return skipUntil(tok::semi);
  };
  if (expect(tok::identifier))
    return _errorhandler();
  SMLoc Loc = Tok.getLocation();

  StringRef Name = Tok.getIdentifier();
  advance();
  if (consume(tok::equal))
    return _errorhandler();
  if (Tok.is(tok::identifier)) {
    Decl *D;
    if (parseQualident(D))
      return _errorhandler();
    Actions.actOnAliasTypeDeclaration(Decls, Loc, Name,
                                      D);
  } else if (Tok.is(tok::kw_POINTER)) {
    advance();
    if (expect(tok::kw_TO))
      return _errorhandler();
    Decl *D;
    advance();
    if (parseQualident(D))
      return _errorhandler();
    Actions.actOnPointerTypeDeclaration(Decls, Loc,
                                        Name, D);
  } else if (Tok.is(tok::kw_ARRAY)) {
    advance();
    if (expect(tok::l_square))
      return _errorhandler();
    Expr *E = nullptr;
    advance();
    if (parseExpression(E))
      return _errorhandler();
    if (consume(tok::r_square))
      return _errorhandler();
    if (expect(tok::kw_OF))
      return _errorhandler();
    Decl *D;
    advance();
    if (parseQualident(D))
      return _errorhandler();
    Actions.actOnArrayTypeDeclaration(Decls, Loc, Name,
                                      E, D);
  } else if (Tok.is(tok::kw_RECORD)) {
    FieldList Fields;
    advance();
    if (parseFieldList(Fields))
      return _errorhandler();
    if (expect(tok::kw_END))
      return _errorhandler();
    Actions.actOnRecordTypeDeclaration(Decls, Loc, Name,
                                       Fields);
    advance();
  } else {
    /*ERROR*/
    return _errorhandler();
  }
  return false;
}

bool Parser::parseFieldList(FieldList &Fields) {
  auto _errorhandler = [this] {
    return skipUntil(tok::kw_END);
  };
  if (parseField(Fields))
    return _errorhandler();
  while (Tok.is(tok::semi)) {
    advance();
    if (parseField(Fields))
      return _errorhandler();
  }
  return false;
}

bool Parser::parseField(FieldList &Fields) {
  auto _errorhandler = [this] {
    return skipUntil(tok::semi, tok::kw_END);
  };
  Decl *D;
  IdentList Ids;
  if (parseIdentList(Ids))
    return _errorhandler();
  if (consume(tok::colon))
    return _errorhandler();
  if (parseQualident(D))
    return _errorhandler();
  Actions.actOnFieldDeclaration(Fields, Ids, D);
  return false;
}

bool Parser::parseVariableDeclaration(DeclList &Decls) {
  auto _errorhandler = [this] {
    return skipUntil(tok::semi);
  };
  Decl *D;
  IdentList Ids;
  if (parseIdentList(Ids))
    return _errorhandler();
  if (consume(tok::colon))
    return _errorhandler();
  if (parseQualident(D))
    return _errorhandler();
  Actions.actOnVariableDeclaration(Decls, Ids, D);
  return false;
}

bool Parser::parseProcedureDeclaration(
    DeclList &ParentDecls) {
  auto _errorhandler = [this] {
    return skipUntil(tok::semi);
  };
  if (consume(tok::kw_PROCEDURE))
    return _errorhandler();
  if (expect(tok::identifier))
    return _errorhandler();
  ProcedureDeclaration *D =
      Actions.actOnProcedureDeclaration(
          Tok.getLocation(), Tok.getIdentifier());

  EnterDeclScope S(Actions, D);
  FormalParamList Params;
  Decl *RetType = nullptr;
  advance();
  if (Tok.is(tok::l_paren)) {
    if (parseFormalParameters(Params, RetType))
      return _errorhandler();
  }
  Actions.actOnProcedureHeading(D, Params, RetType);
  if (expect(tok::semi))
    return _errorhandler();
  DeclList Decls;
  StmtList Stmts;
  advance();
  if (parseBlock(Decls, Stmts))
    return _errorhandler();
  if (expect(tok::identifier))
    return _errorhandler();
  Actions.actOnProcedureDeclaration(
      D, Tok.getLocation(), Tok.getIdentifier(), Decls,
      Stmts);

  ParentDecls.push_back(D);
  advance();
  return false;
}

bool Parser::parseFormalParameters(
    FormalParamList &Params, Decl *&RetType) {
  auto _errorhandler = [this] {
    return skipUntil(tok::semi);
  };
  if (consume(tok::l_paren))
    return _errorhandler();
  if (Tok.isOneOf(tok::kw_VAR, tok::identifier)) {
    if (parseFormalParameterList(Params))
      return _errorhandler();
  }
  if (consume(tok::r_paren))
    return _errorhandler();
  if (Tok.is(tok::colon)) {
    advance();
    if (parseQualident(RetType))
      return _errorhandler();
  }
  return false;
}

bool Parser::parseFormalParameterList(
    FormalParamList &Params) {
  auto _errorhandler = [this] {
    return skipUntil(tok::r_paren);
  };
  if (parseFormalParameter(Params))
    return _errorhandler();
  while (Tok.is(tok::semi)) {
    advance();
    if (parseFormalParameter(Params))
      return _errorhandler();
  }
  return false;
}

bool Parser::parseFormalParameter(
    FormalParamList &Params) {
  auto _errorhandler = [this] {
    return skipUntil(tok::r_paren, tok::semi);
  };
  IdentList Ids;
  Decl *D;
  bool IsVar = false;
  if (Tok.is(tok::kw_VAR)) {
    IsVar = true;
    advance();
  }
  if (parseIdentList(Ids))
    return _errorhandler();
  if (consume(tok::colon))
    return _errorhandler();
  if (parseQualident(D))
    return _errorhandler();
  Actions.actOnFormalParameterDeclaration(Params, Ids,
                                          D, IsVar);
  return false;
}

bool Parser::parseStatementSequence(StmtList &Stmts) {
  auto _errorhandler = [this] {
    return skipUntil(tok::kw_ELSE, tok::kw_END);
  };
  if (parseStatement(Stmts))
    return _errorhandler();
  while (Tok.is(tok::semi)) {
    advance();
    if (parseStatement(Stmts))
      return _errorhandler();
  }
  return false;
}

bool Parser::parseStatement(StmtList &Stmts) {
  auto _errorhandler = [this] {
    return skipUntil(tok::semi, tok::kw_ELSE,
                     tok::kw_END);
  };
  if (Tok.is(tok::identifier)) {
    Decl *D;
    Expr *E = nullptr, *Desig = nullptr;
    SMLoc Loc = Tok.getLocation();
    if (parseQualident(D))
      return _errorhandler();
    if (!Tok.is(tok::l_paren)) {
      Desig = Actions.actOnDesignator(D);
      if (parseSelectors(Desig))
        return _errorhandler();
      if (consume(tok::colonequal))
        return _errorhandler();
      if (parseExpression(E))
        return _errorhandler();
      Actions.actOnAssignment(Stmts, Loc, Desig, E);
    } else if (Tok.is(tok::l_paren)) {
      ExprList Exprs;
      if (Tok.is(tok::l_paren)) {
        advance();
        if (Tok.isOneOf(tok::l_paren, tok::plus,
                        tok::minus, tok::kw_NOT,
                        tok::identifier,
                        tok::integer_literal)) {
          if (parseExpList(Exprs))
            return _errorhandler();
        }
        if (consume(tok::r_paren))
          return _errorhandler();
      }
      Actions.actOnProcCall(Stmts, Loc, D, Exprs);
    }
  } else if (Tok.is(tok::kw_IF)) {
    if (parseIfStatement(Stmts))
      return _errorhandler();
  } else if (Tok.is(tok::kw_WHILE)) {
    if (parseWhileStatement(Stmts))
      return _errorhandler();
  } else if (Tok.is(tok::kw_RETURN)) {
    if (parseReturnStatement(Stmts))
      return _errorhandler();
  } else {
    /*ERROR*/
    return _errorhandler();
  }
  return false;
}

bool Parser::parseIfStatement(StmtList &Stmts) {
  auto _errorhandler = [this] {
    return skipUntil(tok::semi, tok::kw_ELSE,
                     tok::kw_END);
  };
  Expr *E = nullptr;
  StmtList IfStmts, ElseStmts;
  SMLoc Loc = Tok.getLocation();
  if (consume(tok::kw_IF))
    return _errorhandler();
  if (parseExpression(E))
    return _errorhandler();
  if (consume(tok::kw_THEN))
    return _errorhandler();
  if (parseStatementSequence(IfStmts))
    return _errorhandler();
  if (Tok.is(tok::kw_ELSE)) {
    advance();
    if (parseStatementSequence(ElseStmts))
      return _errorhandler();
  }
  if (expect(tok::kw_END))
    return _errorhandler();
  Actions.actOnIfStatement(Stmts, Loc, E, IfStmts,
                           ElseStmts);
  advance();
  return false;
}

bool Parser::parseWhileStatement(StmtList &Stmts) {
  auto _errorhandler = [this] {
    return skipUntil(tok::semi, tok::kw_ELSE,
                     tok::kw_END);
  };
  Expr *E = nullptr;
  StmtList WhileStmts;
  SMLoc Loc = Tok.getLocation();
  if (consume(tok::kw_WHILE))
    return _errorhandler();
  if (parseExpression(E))
    return _errorhandler();
  if (consume(tok::kw_DO))
    return _errorhandler();
  if (parseStatementSequence(WhileStmts))
    return _errorhandler();
  if (expect(tok::kw_END))
    return _errorhandler();
  Actions.actOnWhileStatement(Stmts, Loc, E,
                              WhileStmts);
  advance();
  return false;
}

bool Parser::parseReturnStatement(StmtList &Stmts) {
  auto _errorhandler = [this] {
    return skipUntil(tok::semi, tok::kw_ELSE,
                     tok::kw_END);
  };
  Expr *E = nullptr;
  SMLoc Loc = Tok.getLocation();
  if (consume(tok::kw_RETURN))
    return _errorhandler();
  if (Tok.isOneOf(tok::l_paren, tok::plus, tok::minus,
                  tok::kw_NOT, tok::identifier,
                  tok::integer_literal)) {
    if (parseExpression(E))
      return _errorhandler();
  }
  Actions.actOnReturnStatement(Stmts, Loc, E);
  return false;
}

bool Parser::parseExpList(ExprList &Exprs) {
  auto _errorhandler = [this] {
    return skipUntil(tok::r_paren);
  };
  Expr *E = nullptr;
  if (parseExpression(E))
    return _errorhandler();
  if (E)
    Exprs.push_back(E);
  while (Tok.is(tok::comma)) {
    E = nullptr;
    advance();
    if (parseExpression(E))
      return _errorhandler();
    if (E)
      Exprs.push_back(E);
  }
  return false;
}

bool Parser::parseExpression(Expr *&E) {
  auto _errorhandler = [this] {
    return skipUntil(tok::r_paren, tok::comma,
                     tok::semi, tok::kw_DO,
                     tok::kw_ELSE, tok::kw_END,
                     tok::kw_THEN, tok::r_square);
  };
  if (parseSimpleExpression(E))
    return _errorhandler();
  if (Tok.isOneOf(tok::hash, tok::less, tok::lessequal,
                  tok::equal, tok::greater,
                  tok::greaterequal)) {
    OperatorInfo Op;
    Expr *Right = nullptr;
    if (parseRelation(Op))
      return _errorhandler();
    if (parseSimpleExpression(Right))
      return _errorhandler();
    E = Actions.actOnExpression(E, Right, Op);
  }
  return false;
}

bool Parser::parseRelation(OperatorInfo &Op) {
  auto _errorhandler = [this] {
    return skipUntil(tok::l_paren, tok::plus,
                     tok::minus, tok::kw_NOT,
                     tok::identifier,
                     tok::integer_literal);
  };
  if (Tok.is(tok::equal)) {
    Op = fromTok(Tok);
    advance();
  } else if (Tok.is(tok::hash)) {
    Op = fromTok(Tok);
    advance();
  } else if (Tok.is(tok::less)) {
    Op = fromTok(Tok);
    advance();
  } else if (Tok.is(tok::lessequal)) {
    Op = fromTok(Tok);
    advance();
  } else if (Tok.is(tok::greater)) {
    Op = fromTok(Tok);
    advance();
  } else if (Tok.is(tok::greaterequal)) {
    Op = fromTok(Tok);
    advance();
  } else {
    /*ERROR*/
    return _errorhandler();
  }
  return false;
}

bool Parser::parseSimpleExpression(Expr *&E) {
  auto _errorhandler = [this] {
    return skipUntil(
        tok::hash, tok::r_paren, tok::comma, tok::semi,
        tok::less, tok::lessequal, tok::equal,
        tok::greater, tok::greaterequal, tok::kw_DO,
        tok::kw_ELSE, tok::kw_END, tok::kw_THEN,
        tok::r_square);
  };
  OperatorInfo PrefixOp;
  if (Tok.isOneOf(tok::plus, tok::minus)) {
    if (Tok.is(tok::plus)) {
      PrefixOp = fromTok(Tok);
      advance();
    } else if (Tok.is(tok::minus)) {
      PrefixOp = fromTok(Tok);
      advance();
    }
  }
  if (parseTerm(E))
    return _errorhandler();
  while (
      Tok.isOneOf(tok::plus, tok::minus, tok::kw_OR)) {
    OperatorInfo Op;
    Expr *Right = nullptr;
    if (parseAddOperator(Op))
      return _errorhandler();
    if (parseTerm(Right))
      return _errorhandler();
    E = Actions.actOnSimpleExpression(E, Right, Op);
  }
  if (!PrefixOp.isUnspecified())

    E = Actions.actOnPrefixExpression(E, PrefixOp);
  return false;
}

bool Parser::parseAddOperator(OperatorInfo &Op) {
  auto _errorhandler = [this] {
    return skipUntil(tok::l_paren, tok::kw_NOT,
                     tok::identifier,
                     tok::integer_literal);
  };
  if (Tok.is(tok::plus)) {
    Op = fromTok(Tok);
    advance();
  } else if (Tok.is(tok::minus)) {
    Op = fromTok(Tok);
    advance();
  } else if (Tok.is(tok::kw_OR)) {
    Op = fromTok(Tok);
    advance();
  } else {
    /*ERROR*/
    return _errorhandler();
  }
  return false;
}

bool Parser::parseTerm(Expr *&E) {
  auto _errorhandler = [this] {
    return skipUntil(
        tok::hash, tok::r_paren, tok::plus, tok::comma,
        tok::minus, tok::semi, tok::less,
        tok::lessequal, tok::equal, tok::greater,
        tok::greaterequal, tok::kw_DO, tok::kw_ELSE,
        tok::kw_END, tok::kw_OR, tok::kw_THEN,
        tok::r_square);
  };
  if (parseFactor(E))
    return _errorhandler();
  while (Tok.isOneOf(tok::star, tok::slash, tok::kw_AND,
                     tok::kw_DIV, tok::kw_MOD)) {
    OperatorInfo Op;
    Expr *Right = nullptr;
    if (parseMulOperator(Op))
      return _errorhandler();
    if (parseFactor(Right))
      return _errorhandler();
    E = Actions.actOnTerm(E, Right, Op);
  }
  return false;
}

bool Parser::parseMulOperator(OperatorInfo &Op) {
  auto _errorhandler = [this] {
    return skipUntil(tok::l_paren, tok::kw_NOT,
                     tok::identifier,
                     tok::integer_literal);
  };
  if (Tok.is(tok::star)) {
    Op = fromTok(Tok);
    advance();
  } else if (Tok.is(tok::slash)) {
    Op = fromTok(Tok);
    advance();
  } else if (Tok.is(tok::kw_DIV)) {
    Op = fromTok(Tok);
    advance();
  } else if (Tok.is(tok::kw_MOD)) {
    Op = fromTok(Tok);
    advance();
  } else if (Tok.is(tok::kw_AND)) {
    Op = fromTok(Tok);
    advance();
  } else {
    /*ERROR*/
    return _errorhandler();
  }
  return false;
}

bool Parser::parseFactor(Expr *&E) {
  auto _errorhandler = [this] {
    return skipUntil(
        tok::hash, tok::r_paren, tok::star, tok::plus,
        tok::comma, tok::minus, tok::slash, tok::semi,
        tok::less, tok::lessequal, tok::equal,
        tok::greater, tok::greaterequal, tok::kw_AND,
        tok::kw_DIV, tok::kw_DO, tok::kw_ELSE,
        tok::kw_END, tok::kw_MOD, tok::kw_OR,
        tok::kw_THEN, tok::r_square);
  };
  if (Tok.is(tok::integer_literal)) {
    E = Actions.actOnIntegerLiteral(
        Tok.getLocation(), Tok.getLiteralData());
    advance();
  } else if (Tok.is(tok::identifier)) {
    Decl *D;
    ExprList Exprs;
    if (parseQualident(D))
      return _errorhandler();
    if (Tok.is(tok::l_paren)) {
      advance();
      if (Tok.isOneOf(tok::l_paren, tok::plus,
                      tok::minus, tok::kw_NOT,
                      tok::identifier,
                      tok::integer_literal)) {
        if (parseExpList(Exprs))
          return _errorhandler();
      }
      if (expect(tok::r_paren))
        return _errorhandler();
      E = Actions.actOnFunctionCall(D, Exprs);
      advance();
    } else {
      E = Actions.actOnDesignator(D);
      if (parseSelectors(E))
        return _errorhandler();
    }
  } else if (Tok.is(tok::l_paren)) {
    advance();
    if (parseExpression(E))
      return _errorhandler();
    if (consume(tok::r_paren))
      return _errorhandler();
  } else if (Tok.is(tok::kw_NOT)) {
    OperatorInfo Op = fromTok(Tok);
    advance();
    if (parseFactor(E))
      return _errorhandler();
    E = Actions.actOnPrefixExpression(E, Op);
  } else {
    /*ERROR*/
    return _errorhandler();
  }
  return false;
}

bool Parser::parseSelectors(Expr *&E) {
  auto _errorhandler = [this] {
    return skipUntil(
        tok::hash, tok::r_paren, tok::star, tok::plus,
        tok::comma, tok::minus, tok::slash,
        tok::colonequal, tok::semi, tok::less,
        tok::lessequal, tok::equal, tok::greater,
        tok::greaterequal, tok::kw_AND, tok::kw_DIV,
        tok::kw_DO, tok::kw_ELSE, tok::kw_END,
        tok::kw_MOD, tok::kw_OR, tok::kw_THEN,
        tok::r_square);
  };
  while (Tok.isOneOf(tok::period, tok::l_square,
                     tok::caret)) {
    if (Tok.is(tok::caret)) {
      Actions.actOnDereferenceSelector(
          E, Tok.getLocation());
      advance();
    } else if (Tok.is(tok::l_square)) {
      SMLoc Loc = Tok.getLocation();
      Expr *IndexE = nullptr;
      advance();
      if (parseExpression(IndexE))
        return _errorhandler();
      if (expect(tok::r_square))
        return _errorhandler();
      Actions.actOnIndexSelector(E, Loc, IndexE);
      advance();
    } else if (Tok.is(tok::period)) {
      advance();
      if (expect(tok::identifier))
        return _errorhandler();
      Actions.actOnFieldSelector(E, Tok.getLocation(),
                                 Tok.getIdentifier());
      advance();
    }
  }
  return false;
}

bool Parser::parseQualident(Decl *&D) {
  auto _errorhandler = [this] {
    return skipUntil(
        tok::hash, tok::l_paren, tok::r_paren,
        tok::star, tok::plus, tok::comma, tok::minus,
        tok::period, tok::slash, tok::colonequal,
        tok::semi, tok::less, tok::lessequal,
        tok::equal, tok::greater, tok::greaterequal,
        tok::kw_AND, tok::kw_DIV, tok::kw_DO,
        tok::kw_ELSE, tok::kw_END, tok::kw_MOD,
        tok::kw_OR, tok::kw_THEN, tok::l_square,
        tok::r_square, tok::caret);
  };
  D = nullptr;
  if (expect(tok::identifier))
    return _errorhandler();
  D = Actions.actOnQualIdentPart(D, Tok.getLocation(),
                                 Tok.getIdentifier());
  advance();
  while (Tok.is(tok::period) &&
         (isa<ModuleDeclaration>(D))) {
    advance();
    if (expect(tok::identifier))
      return _errorhandler();
    D = Actions.actOnQualIdentPart(D, Tok.getLocation(),
                                   Tok.getIdentifier());
    advance();
  }
  return false;
}

bool Parser::parseIdentList(IdentList &Ids) {
  auto _errorhandler = [this] {
    return skipUntil(tok::colon, tok::semi);
  };
  if (expect(tok::identifier))
    return _errorhandler();
  Ids.push_back(std::pair<SMLoc, StringRef>(
      Tok.getLocation(), Tok.getIdentifier()));
  advance();
  while (Tok.is(tok::comma)) {
    advance();
    if (expect(tok::identifier))
      return _errorhandler();
    Ids.push_back(std::pair<SMLoc, StringRef>(
        Tok.getLocation(), Tok.getIdentifier()));
    advance();
  }
  return false;
}