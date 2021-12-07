#pragma once

#include <iostream>
#include <string>
#include "../AST/TokenTypes.h"
#include "../AST/Token.h"
#include "../AST/Expr.h"
#include "../Helpers/ErrHandler.h"
#include <vector>

using namespace std;

// i perform no error checking here because 
// lexNumber performs it ahead of time to catch
// invalid doubles

double stringToDouble(string n) {
    double dN = stod(n);
    return dN;
}

class ParseError : public exception {
};

class Parser {
public:
    Parser(vector < Token > ts, ErrHandler e) {
        tokens = ts;
        tokensIndex = 0;
        err = e;
    }

    // top level parse function to begin all parsing
    Expr* parse() {
        try {
            return expression();
        } catch (ParseError e) {
            return NULL;
        }
    }

private:
    // returns unprocessed current token
    Token peek() {
        return tokens[tokensIndex];
    }

    // returns last processed token
    Token previous() {
        return tokens[tokensIndex - 1];
    }

    // checks if we have finished parsing
    bool isAtEnd() {
        if (peek().type == EOF_)
            return true;
        return false;
    }

    // checks if the current token has any of the types provided
    bool matches(vector < TokenType > types) {
        for (int i = 0; i < types.size(); ++i) {
            TokenType tt = types[i];
            if (match(tt)) return true; // use single match function   
        }
        return false;
    }     

    Token advanceIndex() {
        if (!isAtEnd()) tokensIndex++; // advance by one
        return previous(); // return recently consumed token
    }

    bool match(TokenType t) {
        if (check(t)) {
            advanceIndex(); // we found a matching type, consume token
            return true;
        }
        return false;
    }

    bool check(TokenType t) {
        if (isAtEnd()) return false;
        return peek().type == t;
    }

    Token consume(TokenType exp, string msg) {
        if (check(exp)) return advanceIndex();

        throw error(peek(), msg);
    }

    ParseError error(Token t, string msg) {
        err.error(t, msg);
        return ParseError();
    }

    // PARSE HIERARCHY
    // expression -> equality
    Expr* expression() {
        return ternary();
    }

    // potential solution for ternary problem
    // use the comma solution, but differently
    // a ? b : c becomes:
    // Binary("?", a, Binary(":", b, c))
    Expr* ternary() {
        Expr* e = comma();

        // we have ternary expr
        if (match(QUESTION_MARK)) {
            Token qm = previous();
            Expr *m = ternary();

            Token col = consume(COLON, "Expected ':' in Ternary expression.");
            Expr *r = ternary();
            return new Binary(
                e, 
                qm,
                new Binary(
                    m,
                    col,
                    r
                )
            );
        }

        return e;
    }

    // read 1 expr or more separated by ,
    Expr* comma() {
        Expr *e = equality();

        // it is equally a binary operator
        while (match(COMMA)) {
            Token op = previous();
            Expr *r = equality();
            e = new Binary(e, op, r);
        }

        return e;
    }

    // equality -> comparison [ ("!=", "==") comparision ]* 
    Expr* equality() {
        Expr* e = comparison(); // process LHS

        vector < TokenType > eqTTs{
            NOT_EQUAL,
            EQUAL_EQUAL
        };

        // look for operators at this level of matching
        while (matches(eqTTs)) {
            Token op = previous(); // get the matching operator
            Expr* r = comparison();
            e = new Binary(e, op, r);
        }

        return e;
    }

    Expr* comparison() {
        Expr* e = term();

        vector < TokenType > c;
        c.push_back(GREATER);
        c.push_back(LESS);
        c.push_back(GREATER_EQUAL);
        c.push_back(LESS_EQUAL);
        // vector < TokenType > c { GREATER, LESS, GREATER_EQUAL, LESS_EQUAL };

        while (matches(c)) {
            Token op = previous();
            Expr *r = term();
            e = new Binary(e, op, r);
        }

        return e;
    }

    Expr* term() {
        Expr* e = factor();

        vector < TokenType > ops {
            MINUS,
            PLUS
        };  

        while (matches(ops)) {
            Token op = previous();
            Expr *r = factor();
            e = new Binary(e, op, r);
        }

        return e;
    }

    Expr* factor() {
        Expr* e = unary();

        vector < TokenType > ops;
        ops.push_back(SLASH);
        ops.push_back(MULT);
        ops.push_back(EXPONENT);

        while (matches(ops)) {
            Token op = previous();
            Expr *r = unary();
            e = new Binary(e, op, r);
        }

        return e;
    }

    Expr* unary() {
        vector < TokenType > ops {
            NOT,
            MINUS
        };

        // string msg = "Binary operator "
        // report binary operators error
        switch (peek().type) {
            case PLUS:
            case NOT_EQUAL:
            case EQUAL_EQUAL:
            case GREATER:
            case LESS:
            case GREATER_EQUAL:
            case LESS_EQUAL:
            case SLASH:
            case MULT:
            case EXPONENT: {
                // consume()
                Token errOp = peek();
                advanceIndex();
                unary(); // parse and discard rest of operand
                throw error(errOp, "Misused Binary operator " + errOp.lexeme + ".");
            }
            default: {
                cout << "Saw " << peek().lexeme << endl;
                break;
            }
        }

        if (matches(ops)) {
            Token op = previous();
            Expr* r = unary();
            return new Unary(op, r);
        }

        // not a unary operation so match primary
        return primary();
    }

    Expr* primary() {
        switch (peek().type) {
            case TRUE_: {
                advanceIndex();
                return new Boolean(true);
                break;
            }
            case FALSE_: {
                advanceIndex();
                return new Boolean(false);
                break;
            }
            case NUMBER: {
                advanceIndex();
                string nStr = previous().lexeme;
                double n = stringToDouble(nStr);
                return new Number(n);
                break;
            }
            case STRING: {
                advanceIndex();
                return new String(previous().lexeme);
                break;
            }
            case NIL: {
                advanceIndex();
                return new Nil();
                break;
            }
            case LEFT_PAREN: {
                advanceIndex();
                Expr *e = expression();
                consume(RIGHT_PAREN, "Expected ')' following expression.");
                return new Grouping(e);
                break;
            }
            default: {
                throw error(peek(), "Expected expression, got " + peek().lexeme + ".");
            }

        }
    }

    // method used on the occurence of an error
    // to allow Parser to keep parsing without
    // crashing on first error.
    // goal is to synchronize at the start of a statement
    void synchronize() {
        advanceIndex(); // 

        while (!isAtEnd()) { // while we have tokens to consume
            // if we just saw a semi-colon, then we are good to go
            if(previous().type == SEMICOLON) return;

            // if the next token is one of these statement tokens
            // then we are also good to go because we are most likely
            // at the start of a new statement
            switch(peek().type) {
                case CLASS:
                case FUN:
                case VAR:
                case FOR:
                case IF:
                case WHILE:
                case PRINT:
                case RETURN:
                    return;
                default: {
                    break;
                }

            }
            advanceIndex();
        }    
    }

    vector < Token > tokens;
    int tokensIndex;
    ErrHandler err;
};
