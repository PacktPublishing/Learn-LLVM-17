// Base definition the generator knows about.
class Flag<string name, int val> {
    string Name = name;
    int Val = val;
}

class Token {
    string Name;
}

class Tok<string name> : Token {
    let Name = name;
}

class Punctuator<string name, string spelling> : Token {
    let Name = name;
    string Spelling = spelling;
}

class Keyword<string name, list<Flag> flags> : Token {
    let Name = name;
    list<Flag> Flags = flags;
}

class TokenFilter<list<Token> tokens> {
    string FunctionName;
    list<Token> Tokens = tokens;
}
