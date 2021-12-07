//
//  Token.h
//  Croix
//
//  Created by Joshua Pepple on 2021-12-04.
//

#ifndef Token_h
#define Token_h

#include "TokenTypes.h"
#include <string>

class Token {
public:
    Token(TokenType t, std::string lxm, int ln) {
        type = t;
        lexeme = lxm;
        line = ln;
    }

    Token() {
        type = EOF_;
        lexeme = "";
        line = 0;
    }

    std::string String() {
        return lexeme;
    }

    TokenType type;
    std::string lexeme;
    int line;
};

#endif