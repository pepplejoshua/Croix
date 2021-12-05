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
#include "TokenTypes.h"
#include "Token.h"
#include "ErrHandler.h"

using namespace std;

class Lexer {
public:
    Lexer(string src, ErrHandler h) {
        source = src;
        start = current = 0;
        line = 1;
        eReporter = h;
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
                else
                    eReporter.error(line, "Unexpected character -> " + string(1, c)); 
                break;
            }
        }
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

        // string numSlice = source.substr(start, current-start);
        // double d = 0;
        // try {
        //     d = stod(numSlice);
        // } catch (const invalid_argument&) {
        //     eReporter.error(line, numSlice + " is an invalid number.");
        //     return;
        // } catch (const out_of_range&) {
        //     eReporter.error(line, numSlice + " is out of range of a double.");
        //     return;
        // }
        addToken(NUMBER);
    }

    void lexString() {
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') line++;
            advanceCurrent();
        }

        if (isAtEnd()) {
            eReporter.error(line, "Unterminated string.");
            return;
        }

        advanceCurrent(); // skip over the closing "
        string str = source.substr(start+1, current-start-1);
        addToken(STRING);
        // might have to dereference void pointer
        // as char array pointer
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
        string snippet = source.substr(start, current-start);
        tokens.push_back(Token(t, snippet, line));
    }

private:
    int start, current, line;
    string source;
    vector < Token > tokens;
    ErrHandler eReporter;
};
#endif