// tinylang grammar. Can be used with LLtool: https://github.com/redstar/m2lang/blob/master/lib/LLtool/README.md
%token identifier, integer_literal, string_literal
%%
compilationUnit
  : "MODULE" identifier ";" ( import )* block identifier "." ;
import
  : ( "FROM" identifier )? "IMPORT" identList ";" ;
block
  : ( declaration )* ( "BEGIN" statementSequence )? "END" ;
declaration
  : "CONST" ( constantDeclaration ";" )*
  | "VAR" ( variableDeclaration ";" )*
  | procedureDeclaration ";" ;
constantDeclaration
  : identifier "=" expression ;
variableDeclaration
  : identList ":" qualident ;
procedureDeclaration
  : "PROCEDURE" identifier ( formalParameters )? ";"
    block identifier ;
formalParameters
  : "(" ( formalParameterList )? ")" ( ":" qualident )? ;
formalParameterList
  : formalParameter (";" formalParameter )* ;
formalParameter
  : ( "VAR" )? identList ":" qualident ;
statementSequence
  : statement ( ";" statement )* ;
statement
  : qualident ( ":=" expression | ( "(" ( expList )? ")" )? )
  | ifStatement | whileStatement | "RETURN" ( expression )? ;
ifStatement
  : "IF" expression "THEN" statementSequence
    ( "ELSE" statementSequence )? "END" ;
whileStatement
  : "WHILE" expression "DO" statementSequence "END" ;
expList
  : expression ( "," expression )* ;
expression
  : simpleExpression ( relation simpleExpression )? ;
relation
  : "=" | "#" | "<" | "<=" | ">" | ">=" ;
simpleExpression
  : ( "+" | "-" )? term ( addOperator term )* ;
addOperator
  : "+" | "-" | "OR" ;
term
  : factor ( mulOperator factor )* ;
mulOperator
  : "*" | "/" | "DIV" | "MOD" | "AND" ;
factor
  : integer_literal | "(" expression ")" | "NOT" factor
  | qualident ( "(" ( expList )? ")" )? ;
qualident
  : identifier ( "." identifier )* ;
identList
  : identifier ( "," identifier)* ;