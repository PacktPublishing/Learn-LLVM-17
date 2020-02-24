include "Keyword.td"

/*****************/
// Names from clang/lib/Basic/IdentifierTable.
// Flags allowed:
//   KEYALL   - This is a keyword in all variants of Modula-2
//   KEYPIM   - This is a keyword from "Programming in Modula-2", 4th edition.
//   KEYISO   - This is a keyword introduced in ISO 10514-1.
//   KEYISOGS - This is a keyword introduced in ISO 10514-2 (genrics).
//   KEYISOOO - This is a keyword introduced in ISO 10514-3 (OO layer).
//   KEYR10   - This is a keyword introduced with R10.
//   KEYM2P   - This is a keyword introduced with Modula-2+.

def KEYPIM : Flag<"KEYPIM", 0x01>;
def KEYISO : Flag<"KEYISO", 0x02>;
def KEYISOGS : Flag<"KEYISOGS", 0x04>;
def KEYISOOO : Flag<"KEYISOOO", 0x08>;
def KEYR10 : Flag<"KEYR10", 0x10>;
def KEYM2P : Flag<"KEYM2P", 0x20>;
def KEYALL : Flag<"KEYALL", !or(KEYPIM.Val, KEYISO.Val, KEYISOGS.Val, KEYISOOO.Val, KEYR10.Val, KEYM2P.Val)>;


def : Tok<"unknown">;            // Not a token.
def : Tok<"eof">;                // End of file.
def : Tok<"comment">;            // Comment

def : Tok<"identifier">;         // abcde123

def : Tok<"integer_literal">;    // 123, 123B, 123H
def : Tok<"real_literal">;       // 123.4E+5
def : Tok<"char_literal">;       // 12C
def : Tok<"string_literal">;     // "foo", 'foo'


def : Punctuator<"plus",                "+">;
def : Punctuator<"minus",               "-">;
def : Punctuator<"star",                "*">;
def : Punctuator<"slash",               "/">;
def : Punctuator<"colonequal",          ":=">;
def : Punctuator<"period",              ".">;
def : Punctuator<"comma",               ",">;
def : Punctuator<"semi",                ";">;
def : Punctuator<"l_paren",             "(">;
def : Punctuator<"l_square",            "[">;
def : Punctuator<"l_brace",             "{">;
def : Punctuator<"caret",               "^">;
def : Punctuator<"equal",               "=">;
def : Punctuator<"hash",                "#">;
def : Punctuator<"less",                "<">;
def : Punctuator<"greater",             ">">;
def : Punctuator<"lessequal",           "<=">;
def : Punctuator<"greaterequal",        ">=">;
def : Punctuator<"ellipsis",            "..">;
def : Punctuator<"colon",               ":">;
def : Punctuator<"r_paren",             ")">;
def : Punctuator<"r_square",            "]">;
def : Punctuator<"r_brace",             "}">;
def : Punctuator<"pipe",                "|">;
def : Punctuator<"lessstar",            "<*">; // Begin of directive
def : Punctuator<"stargreater",         "*>">; // End of directive

def kw_ABSTRACT: Keyword<"ABSTRACT"            , [KEYISOOO]>;
def kw_ALIAS: Keyword<"ALIAS"                  , [KEYR10]>;
def kw_AND: Keyword<"AND"                      , [KEYALL]>;
def kw_ARGLIST: Keyword<"ARGLIST"              , [KEYR10]>;
def kw_ARRAY: Keyword<"ARRAY"                  , [KEYALL]>;
def kw_AS: Keyword<"AS"                        , [KEYISOOO]>;
def kw_BEGIN: Keyword<"BEGIN"                  , [KEYALL]>;
def kw_BITS: Keyword<"BITS"                    , [KEYM2P]>;
def kw_BLUEPRINT: Keyword<"BLUEPRINT"          , [KEYR10]>;
def kw_BY: Keyword<"BY"                        , [KEYALL]>;
def kw_CASE: Keyword<"CASE"                    , [KEYALL]>;
def kw_CLASS: Keyword<"CLASS"                  , [KEYISOOO]>;
def kw_CONST: Keyword<"CONST"                  , [KEYALL]>;
def kw_COPY: Keyword<"COPY"                    , [KEYR10]>;
def kw_DEFINITION: Keyword<"DEFINITION"        , [KEYALL]>;
def kw_DIV: Keyword<"DIV"                      , [KEYALL]>;
def kw_DO: Keyword<"DO"                        , [KEYALL]>;
def kw_ELSE: Keyword<"ELSE"                    , [KEYALL]>;
def kw_ELSIF: Keyword<"ELSIF"                  , [KEYALL]>;
def kw_END: Keyword<"END"                      , [KEYALL]>;
def kw_EXCEPT: Keyword<"EXCEPT"                , [KEYISO, KEYM2P]>;
def kw_EXCEPTION: Keyword<"EXCEPTION"          , [KEYM2P]>;
def kw_EXIT: Keyword<"EXIT"                    , [KEYALL]>;
def kw_EXPORT: Keyword<"EXPORT"                , [KEYPIM, KEYISO, KEYM2P]>;
def kw_FINALLY: Keyword<"FINALLY"              , [KEYISO, KEYM2P]>;
def kw_FOR: Keyword<"FOR"                      , [KEYALL]>;
def kw_FORWARD: Keyword<"FORWARD"              , [KEYISO]>;
def kw_FROM: Keyword<"FROM"                    , [KEYALL]>;
def kw_GENERIC: Keyword<"GENERIC"              , [KEYISOGS]>;
def kw_GENLIB: Keyword<"GENLIB"                , [KEYR10]>;
def kw_GUARD: Keyword<"GUARD"                  , [KEYISOOO]>;
def kw_IF: Keyword<"IF"                        , [KEYALL]>;
def kw_IMPLEMENTATION: Keyword<"IMPLEMENTATION", [KEYALL]>;
def kw_IMPORT: Keyword<"IMPORT"                , [KEYALL]>;
def kw_IN: Keyword<"IN"                        , [KEYALL]>;
def kw_INHERIT: Keyword<"INHERIT"              , [KEYISOOO]>;
def kw_LOCK: Keyword<"LOCK"                    , [KEYM2P]>;
def kw_LOOP: Keyword<"LOOP"                    , [KEYALL]>;
def kw_MOD: Keyword<"MOD"                      , [KEYALL]>;
def kw_MODULE: Keyword<"MODULE"                , [KEYALL]>;
def kw_NEW: Keyword<"NEW"                      , [KEYR10]>;
def kw_NONE: Keyword<"NONE"                    , [KEYR10]>;
def kw_NOT: Keyword<"NOT"                      , [KEYALL]>;
def kw_OF: Keyword<"OF"                        , [KEYALL]>;
def kw_OPAQUE: Keyword<"OPAQUE"                , [KEYM2P, KEYR10]>;
def kw_OR: Keyword<"OR"                        , [KEYALL]>;
def kw_OVERRIDE: Keyword<"OVERRIDE"            , [KEYISOOO]>;
def kw_PACKEDSET: Keyword<"PACKEDSET"          , [KEYISO]>;
def kw_PASSING: Keyword<"PASSING"              , [KEYM2P]>;
def kw_POINTER: Keyword<"POINTER"              , [KEYALL]>;
def kw_PROCEDURE: Keyword<"PROCEDURE"          , [KEYALL]>;
def kw_QUALIFIED: Keyword<"QUALIFIED"          , [KEYALL]>;
def kw_RAISES: Keyword<"RAISES"                , [KEYM2P]>;
def kw_READONLY: Keyword<"READONLY"            , [KEYISOOO]>;
def kw_RECORD: Keyword<"RECORD"                , [KEYALL]>;
def kw_REF: Keyword<"REF"                      , [KEYM2P]>;
def kw_REFERENTIAL: Keyword<"REFERENTIAL"      , [KEYR10]>;
def kw_RELEASE: Keyword<"RELEASE"              , [KEYR10]>;
def kw_REM: Keyword<"REM"                      , [KEYISO]>;
def kw_REPEAT: Keyword<"REPEAT"                , [KEYALL]>;
def kw_RETAIN: Keyword<"RETAIN"                , [KEYR10]>;
def kw_RETRY: Keyword<"RETRY"                  , [KEYISO]>;
def kw_RETURN: Keyword<"RETURN"                , [KEYALL]>;
def kw_REVEAL: Keyword<"REVEAL"                , [KEYISOOO]>;
def kw_SAFE: Keyword<"SAFE"                    , [KEYM2P]>;
def kw_SET: Keyword<"SET"                      , [KEYALL]>;
def kw_THEN: Keyword<"THEN"                    , [KEYALL]>;
def kw_TO: Keyword<"TO"                        , [KEYALL]>;
def kw_TRACED: Keyword<"TRACED"                , [KEYISOOO]>;
def kw_TRY: Keyword<"TRY"                      , [KEYM2P]>;
def kw_TYPE: Keyword<"TYPE"                    , [KEYALL]>;
def kw_TYPECASE: Keyword<"TYPECASE"            , [KEYM2P]>;
def kw_UNSAFEGUARDED: Keyword<"UNSAFEGUARDED"  , [KEYISOOO]>;
def kw_UNTIL: Keyword<"UNTIL"                  , [KEYALL]>;
def kw_VAR: Keyword<"VAR"                      , [KEYALL]>;
def kw_WHILE: Keyword<"WHILE"                  , [KEYALL]>;
def kw_WITH: Keyword<"WITH"                    , [KEYPIM, KEYISO, KEYM2P]>;
def kw_YIELD: Keyword<"YIELD"                  , [KEYR10]>;

def : TokenFilter<[kw_ABSTRACT, kw_ALIAS, kw_AND, kw_ARGLIST, kw_ARRAY, kw_AS,
                   kw_BEGIN, kw_BITS, kw_BLUEPRINT, kw_BY, kw_CASE,kw_CLASS,
                   kw_CONST, kw_COPY, kw_DEFINITION, kw_DIV, kw_DO, kw_ELSE,
                   kw_ELSIF, kw_END, kw_EXCEPT, kw_EXCEPTION, kw_EXIT,
                   kw_EXPORT, kw_FINALLY, kw_FOR, kw_FORWARD, kw_FROM,
                   kw_GENERIC, kw_GENLIB, kw_GUARD, kw_IF, kw_IMPLEMENTATION,
                   kw_IMPORT, kw_IN, kw_INHERIT, kw_LOCK, kw_LOOP, kw_MOD,
                   kw_MODULE, kw_NEW, kw_NONE, kw_NOT, kw_OF, kw_OPAQUE,
                   kw_OR, kw_OVERRIDE, kw_PACKEDSET, kw_PASSING, kw_POINTER,
                   kw_PROCEDURE, kw_QUALIFIED, kw_RAISES, kw_READONLY,
                   kw_RECORD, kw_REF, kw_REFERENTIAL, kw_RELEASE, kw_REM,
                   kw_REPEAT, kw_RETAIN, kw_RETRY, kw_RETURN, kw_REVEAL,
                   kw_SAFE, kw_SET, kw_THEN, kw_TO, kw_TRACED, kw_TRY, kw_TYPE,
                   kw_TYPECASE, kw_UNSAFEGUARDED, kw_UNTIL, kw_VAR, kw_WHILE,
                   kw_WITH, kw_YIELD, ]>;
