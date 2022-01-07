#pragma once

#include <iostream>
#include <string>
#include "../AST/TokenTypes.h"
#include "../AST/Token.h"
#include "../AST/Expr.h"
#include "../AST/Stmt.h"
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

class Parser {
public:
    Parser(vector < Token > ts, ErrHandler* e) {
        tokens = ts;
        tokensIndex = 0;
        err = e;
    }

    // top level parse function to begin all parsing
    vector < Stmt* > parse() {
        vector < Stmt* > stmts;
        while (!isAtEnd()) {
            Stmt* s = declaration();
            if (s) {
                stmts.push_back(s);
            } 
        }
        return stmts;
    }
    
private:
    // this handles variable declarations
    // or it routes to statements
    Stmt* declaration() {
        try {
            if (match(CLASS)) return classDeclaration();
            if (match(FUN)) return funcDeclaration("function");
            if (match(VAR)) return varDeclaration(); 
            return statement();
        } catch (ParseError e) {
            synchronize(); // we use synchronize here to survive the panic, and try to parse even further after reporting errors
            return NULL;
        }
    }   

    Stmt* classDeclaration() {
        Token name = consume(IDENTIFIER, "Expected class name.");

        consume(LEFT_BRACE, "Expected '{' before class body.");
        vector < Function* > methods;

        while(!check(RIGHT_BRACE) && !isAtEnd()) {
            Function* fn = dynamic_cast<Function *>(funcDeclaration("method"));
            
            if (fn != NULL) {
                methods.push_back(fn);
            }
        }

        consume(RIGHT_BRACE, "Expected '}' to terminate class body.");
        return new Class(name, methods);
    }

    Stmt* funcDeclaration(string kind) {
        // lambda expression
        // if (match(LEFT_PAREN)) return lambdaDeclaration(kind);
        Token fnName = consume(IDENTIFIER, "Expected " + kind + " name.");
        vector < Token > params;

        consume(LEFT_PAREN, "Expected '(' after " + kind + " name.");
        if (!check(RIGHT_PAREN)) { // we do have params for this func definition        
            do {
                if (params.size() >= 255) 
                    error(peek(), "Cannot have more than 255 parameters.");

                params.push_back(consume(IDENTIFIER, "Expected parameter name."));
            } while (match(COMMA));
        }
        consume(RIGHT_PAREN, "Expected ')' after parameters.");

        consume(LEFT_BRACE, "Expected '{' before " + kind + " body.");
        vector < Stmt* > body = block();
        return new Function(fnName, params, new Block(body));
    }

    // Stmt* lambdaDeclaration(string kind) {
    //     vector < Token > params;

    //     if (!check(RIGHT_PAREN)) {
    //         do {
    //             if (params.size() >= 255) 
    //                 error(peek(), "Cannot have more than 255 parameters.");

    //             params.push_back(consume(IDENTIFIER, "Expected parameter name."));
    //         } while (match(COMMA));
    //     }
    //     consume(RIGHT_PAREN, "Expected ')' after parameters.");

    //     consume(LEFT_BRACE, "Expected '{' before " + kind + " body.");
    //     vector < Stmt* > body = block();
    //     return new Expression(new Lambda(params, new Block(body)));
    // }

    Stmt* varDeclaration() {
        Token variableName = consume(IDENTIFIER, "Expected variable name.");

        Expr* init = NULL;
        if (match(EQUAL)) // perform assignment of expression
            init = expression();
        
        consume(SEMICOLON, "Expected ';' to terminate variable declaration.");
        return new Var(variableName, init);
    }

    // describes the statement types in this language
    Stmt* statement() {
        if (match(IF)) return ifStatement();
        if (match(PRINT)) return printStatement();
        if (match(WHILE)) return whileStatement();
        if (match(FOR)) return forStatement();
        if (match(LEFT_BRACE)) return new Block(block());
        if (match(RETURN)) return returnStatement();
        return expressionStatement();
    }

    vector < Stmt* > block() {
        vector < Stmt * > stmts;

        while(!check(RIGHT_BRACE) && !isAtEnd()) {
            // this allows variable bindings among other
            // statement types in blocks
            stmts.push_back(declaration());
        }
        consume(RIGHT_BRACE, "Expected '}' to terminate block.");
        return stmts;
    }

    Stmt* returnStatement() {
        Token ret = previous();
        Expr* val = NULL;
        if (!check(SEMICOLON)) { // check if we have some return value
            val = expression();
        }
        consume(SEMICOLON, "Expected ';' after return value.");
        return new Return(ret, val);
    }

    // for '(' (varDecl | exprStmt | ';') expression?1 ';' expression?2 ')' Stmt
    // (varDecl | exprStmt | ';') -> init
    // expression?1 -> cond
    // expression?2 -> increment
    Stmt* forStatement() {
        consume(LEFT_PAREN, "Expected '(' after 'for'.");
        
        // handle init or lack of
        Stmt* init = NULL;
        if (match(SEMICOLON)) { // no init 

        } else if (match(VAR)) { // init var declaration
            init = varDeclaration();
        } else { // we could then take an ExprStmt, like an already declared variable
            init = expressionStatement();
        }
        
        Expr* cond = NULL;
        // check to see if clause was omitted
        if (!check(SEMICOLON))
            cond = expression();
        consume(SEMICOLON, "Expected ';' after loop condition.");

        Expr* increment = NULL;
        // check to see if increment was omitted
        if (!check(RIGHT_PAREN))
            increment = expression();
        consume(RIGHT_PAREN, "Expected ')' after for clauses.");
        Stmt* body = statement();

        // desugar for loop into while loop in a block statement
        // ? means optional
        // for (a?; b?; c?) d becomes:
        // {
        //      a?;
        //      while (b? | true) {
        //          d;
        //          c?;
        //      }
        // }

        // first, add increment statement to end of body and put them into a block statement
        vector < Stmt* > stmts;
        stmts.push_back(body);
        stmts.push_back(new Expression(increment));
        Stmt* block = new Block(stmts);

        // then construct a while loop with cond and block as its body
        if (cond == NULL) cond = new Boolean(true);
        Stmt* while_ = new While(cond, block);

        // finally create an enclosing block if there is an initializer
        // if there isn't, return the while statement
        if (init == NULL) {
            body = while_;
        } else {
            vector < Stmt* > desugared_for;
            desugared_for.push_back(init);
            desugared_for.push_back(while_);
            body = new Block(desugared_for);
        }
        return body;
    }

    Stmt* whileStatement() {
        consume(LEFT_PAREN, "Expected '(' after 'while'.");
        Expr* cond = expression();
        consume(RIGHT_PAREN, "Expected ')' after while condition.");

        Stmt* body = statement();

        return new While(cond, body);
    }

    Stmt* ifStatement() {
        consume(LEFT_PAREN, "Expected '(' after 'if'.");
        Expr* cond = expression();
        consume(RIGHT_PAREN, "Expected ')' after if condition.");

        // does this let me chain together else if statements??
        // yes it doesssss
        Stmt* then = statement();
        Stmt* else_ = NULL;
        if (match(ELSE))
            else_ = statement();

        return new If(cond, then, else_);
    }

    // print statement
    Stmt* printStatement() {
        Expr *e = NULL;
        if (match(SEMICOLON)) {
            // do nothing
        } else {
            e = comma();
            consume(SEMICOLON, "Expected ';' to terminate print statement");
        }
            
        return new Print(e);
    }

    // expression statement
    Stmt* expressionStatement() {
        Expr *e = comma();
        consume(SEMICOLON, "Expected ';' to terminate expression statement");
        return new Expression(e);
    }


    // read 1 expr or more separated by ,
    Expr* comma() {
        Expr *e = expression();

        // it is equally a binary operator
        while (match(COMMA)) {
            Token op = previous();
            Expr *r = expression();
            e = new Binary(e, op, r);
        }

        return e;
    }

    // PARSE HIERARCHY
    // expression -> equality
    Expr* expression() {
        return assignment();
    }

    // handles the modification of variables
    Expr* assignment() {
        Expr* target = or_();

        if (match(EQUAL)) { // we are assigning
            Token eq = previous();
            Expr* v = assignment();

            switch(target->type()) {
                case 'v': {
                    Token varName = dynamic_cast<Variable *>(target)->name; 
                    return new Assign(varName, v);
                    break;
                }
                default: {
                    error(eq, "Cannot assign to specified target.");
                }
            }
        }

        return target; // we aren't assigning
    }

    Expr* or_() {
        Expr* e = and_();
        
        while (match(OR)) {
            Token op = previous();
            Expr* r = and_();
            e = new Logical(e, op, r);
        }

        return e;
    }

    Expr* and_() {
        Expr* e = ternary();

        while (match(AND)) {
            Token op = previous();
            Expr* r = ternary();
            e = new Logical(e, op, r);
        }

        return e;
    }

    // potential solution for ternary problem
    // use the comma solution, but differently
    // a ? b : c becomes:
    // Binary("?", a, Binary(":", b, c))
    Expr* ternary() {
        Expr* e = equality();

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
                break;
            }
        }

        if (matches(ops)) {
            Token op = previous();
            Expr* r = unary();
            return new Unary(op, r);
        }

        // not a unary operation so match call or primary Exprs
        return call();
    }

    Expr* call() {
        Expr* e = primary();

        while (match(LEFT_PAREN)) {
            e = buildCallExpr(e);
        }

        // Nystrom's version
        // while (true) {
        //     if (match(LEFT_PAREN))
        //         e = buildCallExpr(e);
        //     else
        //         break;
        // }
        return e;
    }

    Expr* buildCallExpr(Expr* callable) {
        vector < Expr* > arguments;

        if (!check(RIGHT_PAREN)) { // we have arguments to pass to this call, process them
            do {
                // restrict number of arguments that can be processed
                if (arguments.size() >= 255)
                    error(peek(), "Cannot have more than 255 arguments.");
                arguments.push_back(expression());
            } while (match(COMMA)); // look for a comma to continue parsing more arguments
        }

        // use this closing operator to report errors
        Token op = consume(RIGHT_PAREN, "Expected ')' after arguments.");
        return new Call(callable, op, arguments);
    }

    Expr* primary() {
        switch (peek().type) {
            // replace Boolean() with constant
            // versions
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
            case IDENTIFIER: {
                advanceIndex();
                return new Variable(previous()); // get the identifier
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
        err->error(t, msg);
        return ParseError();
    }

    vector < Token > tokens;
    int tokensIndex;
    ErrHandler* err;
};
