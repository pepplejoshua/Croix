//
//  Lexer.h
//  Croix
//
//  Created by Joshua Pepple on 2021-12-04.
//

#ifndef Lexer_h
#define Lexer_h

#include <vector>
#include <string> 
#include <stdexcept>
#include "../AST/TokenTypes.h"
#include "../AST/Token.h"
#include "../Helpers/ErrHandler.h"
#include <map>

using namespace std;

class Lexer {
public:
    Lexer(string src, ErrHandler* h) {
        source = src;
        start = current = 0;
        line = 1;
        eReporter = h;

        const string sKeywords[] = {
            "and",
            "class",
            "else",
            "false",
            "for",
            "fun",
            "if",
            "nil",
            "or",
            "print",
            "return",
            "super",
            "this",
            "true",
            "var",
            "while"
        };

        TokenType tKeywords[] = {
            AND,
            CLASS,
            ELSE,
            FALSE_,
            FOR,
            FUN,
            IF,
            NIL,
            OR,
            PRINT,
            RETURN,
            SUPER,
            THIS,
            TRUE_,
            VAR,
            WHILE
        };
        
        int length = sizeof(sKeywords)/sizeof(sKeywords[0]);
        for (int i = 0; i < length; ++i) {
            keywords.insert(pair < string, TokenType >(sKeywords[i], tKeywords[i]));
        }
    }

    bool isAtEnd() {
        return current >= source.length();
    }

    vector < Token > lexTokens() {
        while (!isAtEnd()) {
            start = current; // reposition start for next token
            lexToken(); // lex next token
        }
        tokens.push_back(Token(EOF_, "", line));
        return tokens;
    }

    void lexToken() {
        char c = advanceCurrent(); 

        switch (c) {
            case '(': addToken(LEFT_PAREN); break;
            case ')': addToken(RIGHT_PAREN); break;
            case '{': addToken(LEFT_BRACE); break;
            case '}': addToken(RIGHT_BRACE); break;
            case ',': addToken(COMMA); break;
            case '.': addToken(DOT); break;
            case '-': addToken(MINUS); break;
            case '+': addToken(PLUS); break;
            case '*': addToken(MULT); break;
            case ';': addToken(SEMICOLON); break;
            case ':': addToken(COLON); break;
            case '?': addToken(QUESTION_MARK); break;
            case '^': addToken(EXPONENT); break;
            case '>': {
                if (match('=')) {
                    addToken(GREATER_EQUAL);
                } else {
                    addToken(GREATER);
                }
                break;
            }
            case '<': {
                if (match('=')) {
                    addToken(LESS_EQUAL);
                } else {
                    addToken(LESS);
                }
                break;
            }
            case '=': {
                if (match('=')) {
                    addToken(EQUAL_EQUAL);
                } else {
                    addToken(EQUAL);
                }
                break;
            }
            case '!': {
                if (match('=')) {
                    addToken(NOT_EQUAL);
                } else {
                    addToken(NOT);
                }
                break;
            }
            // special characters
            case '/': {
                // if the next char is also a /
                // then we need to skip over a comment
                if (match('/')) {
                    skipSingleLineComment();
                } else {
                    addToken(SLASH);
                }
                break;
            }
            case ' ':
            case '\r':
            case '\t':
                break;
            case '\n': line++; break;
            case '"': lexString(); break;
            default: {
                if (isdigit(c))
                    lexNumber();
                else if (isAlpha(c))
                    lexIdentifierOrKeyword();
                else
                    eReporter->error(line, "Unexpected character -> " + string(1, c)); 
                break;
            }
        }
    }

    bool isAlpha(char c) {
        if (isalpha(c) || c == '_')
            return true;
        return false;
    }

    void lexIdentifierOrKeyword() {
        while (isalnum(peek())) advanceCurrent();

        string text = source.substr(start, current-start);
        map < string, TokenType >::iterator it;
        it = keywords.find(text);

        TokenType type;
        // it is actually a keyword
        if (it != keywords.end()) {
            type = it->second;
        } else { // it is not a keyword, but an identifier
            type = IDENTIFIER;
        }
        addToken(type);
    }

    void lexNumber() {
        // advance long as we are still seeing numbers
        while(isdigit(peek()))
            advanceCurrent();

        // look for a decimal part
        if (peek() == '.' && isdigit(peekNext())) {
            // skip over the '.' and continue build number up
            advanceCurrent();

            while (isdigit(peek())) advanceCurrent();
        }

        // make sure we can convert this to a valid
        // double later
        string numSlice = source.substr(start, current-start);
        double d = 0;
        try {
            d = stod(numSlice);
        } catch (const invalid_argument&) {
            eReporter->error(line, numSlice + " is an invalid number.");
            return;
        } catch (const out_of_range&) {
            eReporter->error(line, numSlice + " is out of range of a double.");
            return;
        }
        addToken(NUMBER);
    }

    void lexString() {
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') line++;
            advanceCurrent();
        }

        if (isAtEnd()) {
            eReporter->error(line, "Unterminated string.");
            return;
        }

        advanceCurrent(); // skip over the closing "
        addToken(STRING);
    }

    // unlike advance, this doesn't look at current-1
    // instead returns current itself, allowing us peek characters
    char peek() {
        if (isAtEnd()) return '\0';
        return source[current];
    }

    // unlike peek that looks at current to allow us peek
    // characters, we use current + 1 to let us peek 2 characters
    // ahead
    char peekNext() {
        if (current + 1 >= source.length()) return '\0';
        return source[current+1];
    }

    // used to check if the peek character matches
    // provided character. advances current if it is 
    // as a side effect
    bool match(char expected) {
        if (isAtEnd()) return false;
        if (source[current] != expected)
            return false;

        current++;
        return true;
    }

    // skips a single line comment
    void skipSingleLineComment() {
        while (peek() != '\n' && !isAtEnd()) 
            advanceCurrent();
    }

    // advances the current token by one
    // and returns the char before current
    char advanceCurrent() {
        current++;
        return source[current - 1];
    }

    // adds a token to vector
    void addToken(TokenType t) {
        string snippet;
        // skip the "" from both directions
        if (t == STRING)
            snippet = source.substr(start+1, current-start-2);
        else
            snippet = source.substr(start, current-start);
        tokens.push_back(Token(t, snippet, line));
    }

private:
    int start, current, line;
    string source;
    vector < Token > tokens;
    ErrHandler* eReporter;
    map < string, TokenType > keywords;
};
#endif