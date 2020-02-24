include "Keyword.td"

/*****************/
// Names from clang/lib/Basic/IdentifierTable.
def KEYC99 : Flag<"KEYC99", 0x1>;
def KEYCXX : Flag<"KEYCXX", 0x2>;
def KEYCXX11: Flag<"KEYCXX11", 0x4>;
def KEYGNU: Flag<"KEYGNU", 0x8>;
def KEYMS: Flag<"KEYMS", 0x10>;
def BOOLSUPPORT: Flag<"BOOLSUPPORT", 0x20>;
def KEYALTIVEC: Flag<"KEYALTIVEC", 0x40>;
def KEYNOCXX: Flag<"KEYNOCXX", 0x80>;
def KEYBORLAND: Flag<"KEYBORLAND", 0x100>;
def KEYOPENCLC: Flag<"KEYOPENCLC", 0x200>;
def KEYC11: Flag<"KEYC11", 0x400>;
def KEYNOMS18: Flag<"KEYNOMS18", 0x800>;
def KEYNOOPENCL: Flag<"KEYNOOPENCL", 0x1000>;
def WCHARSUPPORT: Flag<"WCHARSUPPORT", 0x2000>;
def HALFSUPPORT: Flag<"HALFSUPPORT", 0x4000>;
def CHAR8SUPPORT: Flag<"CHAR8SUPPORT", 0x8000>;
def KEYCONCEPTS: Flag<"KEYCONCEPTS", 0x10000>;
def KEYOBJC: Flag<"KEYOBJC", 0x20000>;
def KEYZVECTOR: Flag<"KEYZVECTOR", 0x40000>;
def KEYCOROUTINES: Flag<"KEYCOROUTINES", 0x80000>;
def KEYMODULES: Flag<"KEYMODULES", 0x100000>;
def KEYCXX20: Flag<"KEYCXX20", 0x200000>;
def KEYOPENCLCXX: Flag<"KEYOPENCLCXX", 0x400000>;
def KEYMSCOMPAT: Flag<"KEYMSCOMPAT", 0x800000>;
def KEYSYCL: Flag<"KEYSYCL", 0x1000000>;
def KEYALLCXX : Flag<"KEYALLCXX", !or(KEYCXX.Val, KEYCXX11.Val, KEYCXX20.Val)>;
def KEYALL : Flag<"KEYALL", !and(0x1ffffff, !xor(-1, KEYNOMS18.Val), !xor(-1, KEYNOOPENCL.Val))>;


def : Tok<"unknown">;
def : Tok<"eof">;
def : Tok<"comment">;

def : Punctuator<"plus", "+">;
def : Punctuator<"minus", "-">;

def kw_auto: Keyword<"auto", [KEYALL]>;
def kw_char : Keyword<"char", [KEYALL]>;
def kw_const : Keyword<"const", [KEYALL]>;
def kw_continue : Keyword<"continue", [KEYALL]>;
def kw_default : Keyword<"default", [KEYALL]>;
def kw_do : Keyword<"do", [KEYALL]>;
def kw_double : Keyword<"double", [KEYALL]>;
def kw_else : Keyword<"else", [KEYALL]>;
def kw_enum : Keyword<"enum", [KEYALL]>;
def kw_extern : Keyword<"extern", [KEYALL]>;
def kw_float : Keyword<"float", [KEYALL]>;
def kw_for : Keyword<"for", [KEYALL]>;
def kw_goto : Keyword<"goto", [KEYALL]>;
def kw_if : Keyword<"if", [KEYALL]>;
def kw_inline : Keyword<"inline", [KEYC99,KEYCXX,KEYGNU]>;
def kw_int : Keyword<"int", [KEYALL]>;
def kw_long : Keyword<"long", [KEYALL]>;
def kw_register : Keyword<"register", [KEYALL]>;
def kw_restrict : Keyword<"restrict", [KEYC99]>;
def kw_return : Keyword<"return", [KEYALL]>;
def kw_short : Keyword<"short", [KEYALL]>;
def kw_signed : Keyword<"signed", [KEYALL]>;


def : TokenFilter<[kw_auto, kw_char, kw_const, kw_continue, kw_default, kw_do, kw_double,
                   kw_else, kw_enum, kw_extern, kw_float, kw_for, kw_goto, kw_if, kw_inline,
                   kw_int, kw_long, kw_register, kw_restrict, kw_return, kw_short, kw_signed]>;

/*
def kw_A : Keyword<"a", [KEYALL]>;
def kw_C : Keyword<"c", [KEYALL]>;
def : TokenFilter<[kw_A, kw_C]>;
*/