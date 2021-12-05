//
//  TokenTypes.h
//  Croix
//
//  Created by Joshua Pepple on 2021-12-04.
//

#ifndef TokenTypes_h
#define TokenTypes_h

#include <string>

enum TokenType {
    // single character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, MULT,
    
    // one or more tokens
    NOT, NOT_EQUAL, EQUAL, EQUAL_EQUAL, GREATER,
    GREATER_EQUAL, LESS, LESS_EQUAL,
    
    // literals
    IDENTIFIER, STRING, NUMBER,
    
    // reserved identifiers or keywords
    AND, CLASS, ELSE, TRUE_, FALSE_, FUN, FOR, IF, NIL,
    OR, PRINT, RETURN, SUPER, THIS, VAR, WHILE,
    
    EOF_
};

#endif /* TokenTypes_h */
