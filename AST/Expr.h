//
// Expr.h
// Croix
//
// Generated by Joshua Pepple on 2021-12-06.
//

#pragma once

#include <iostream>
#include <string>
#include "Token.h"

using namespace std;

class Binary;
class Unary;
class Grouping;
class Boolean;
class Number;
class String;
class Nil;

// class to be inherited by abstract base class
// to allow the template defined types visit this class
// it is visited by V1 and returns R1
// it is visited by V2 and returns R2
template <typename V1, typename R1, typename V2, typename R2>
class Visitable {
public:
    virtual R1 accept(V1) = 0;
    virtual R2 accept(V2) = 0;
};

// class to be inherited by classes that intend to visit
template <typename ReturnValue>
class ExprVisitor {
public:
    virtual ReturnValue visitBinaryExpr(Binary*) = 0;
    virtual ReturnValue visitUnaryExpr(Unary*) = 0;
    virtual ReturnValue visitGroupingExpr(Grouping*) = 0;
    virtual ReturnValue visitBooleanExpr(Boolean*) = 0;
    virtual ReturnValue visitNumberExpr(Number*) = 0;
    virtual ReturnValue visitStringExpr(String*) = 0;
    virtual ReturnValue visitNilExpr(Nil*) = 0;
};

// anything that is an ExprVisitor can visit this class
class Expr : public Visitable< ExprVisitor<string>* , string, ExprVisitor< Expr * > *, Expr * > {
public:
    virtual char type() const = 0;
};

class Binary : public Expr {
public:
    Binary(Expr* left, Token op, Expr* right) {
        this->left = left;
        this->op = op;
        this->right = right;
    }
    
    string accept(ExprVisitor<string>* ev) {
        return ev->visitBinaryExpr(this);
    }

    Expr* accept(ExprVisitor< Expr * > * ev) {
        return ev->visitBinaryExpr(this);
    }

    char type() const {
        return 'b';
    }

    Expr* left;
    Token op;
    Expr* right;
};

class Unary : public Expr {
public:
    Unary(Token op, Expr* right) {
        this->op = op;
        this->right = right;
    }
    
    string accept(ExprVisitor<string> *ev) {
        return ev->visitUnaryExpr(this);
    }

    Expr* accept(ExprVisitor< Expr * > * ev) {
        return ev->visitUnaryExpr(this);
    }

    char type() const {
        return 'U';
    }

    Token op;
    Expr* right;
};

class Grouping : public Expr {
public:
    Grouping(Expr* expr) {
        this->expr = expr;
    }
    
    string accept(ExprVisitor<string> *ev) {
        return ev->visitGroupingExpr(this);
    }

    Expr* accept(ExprVisitor< Expr * > * ev) {
        return ev->visitGroupingExpr(this);
    }

    char type() const {
        return 'G';
    }

    Expr* expr;
};

class Boolean : public Expr {
public:
    Boolean(bool value) {
        this->value = value;
    }
    
    string accept(ExprVisitor<string> *ev) {
        return ev->visitBooleanExpr(this);
    }

    Expr* accept(ExprVisitor< Expr * > * ev) {
        return ev->visitBooleanExpr(this);
    }

    char type() const {
        return 'B';
    }

    bool value;
};

class Number : public Expr {
public:
    Number(double value) {
        this->value = value;
    }
    
    string accept(ExprVisitor<string> *ev) {
        return ev->visitNumberExpr(this);
    }

    Expr* accept(ExprVisitor< Expr * > * ev) {
        return ev->visitNumberExpr(this);
    }

    char type() const {
        return 'N';
    }

    double value;
};

class String : public Expr {
public:
    String(string value) {
        this->value = value;
    }
    
    string accept(ExprVisitor<string> *ev) {
        return ev->visitStringExpr(this);
    }

    Expr* accept(ExprVisitor< Expr * > * ev) {
        return ev->visitStringExpr(this);
    }

    char type() const {
        return 's';
    }

    string value;
};

class Nil : public Expr {
public:
    Nil() {
    }
    
    string accept(ExprVisitor<string> *ev) {
        return ev->visitNilExpr(this);
    }

    Expr* accept(ExprVisitor< Expr * > * ev) {
        return ev->visitNilExpr(this);
    }

    char type() const {
        return '\0';
    }
};